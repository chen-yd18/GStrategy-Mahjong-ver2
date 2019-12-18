#define _CRT_SECURE_NO_WARNINGS
#include "logic.h"
#include "map4plr.h"
#include <stdio.h>
#include <stdlib.h>

Logic* Logic::logic = nullptr;

Logic::Logic() {
	freopen("log.txt", "w", stdout);
	pos[RED] = pos[BLUE] = NODE_START;
	coin[RED] = coin[BLUE] = 0;
	finishTurn[RED] = finishTurn[BLUE] = 0;
	for (int i = 1; i <= NODE_COUNT; i++)
		nodeCDleft[i] = nodeCD[i];
}

Logic* Logic::getInstance() {
	if (logic == nullptr)
		logic = new Logic;
	return logic;
}
void Logic::updateMap() {
	printf("COIN %d %d\n", coin[RED], coin[BLUE]);
	for (int i = 1; i <= NODE_COUNT; i++) 
		if (nodeCDleft[i] > 0) {
			nodeCDleft[i]--;
			if (nodeCDleft[i] == 0) {
				printf("SPAWN %d\n", i);
			}
		}
}

void Logic::SinglePlayerRun(int pl) {
	if (army[pl].countCard() == 13)return;
	int power = 0;
	if (coin[pl] >= 5 && pos[pl] == 1)power = 1;
	else if (army[pl].canBus() &&
		(pos[pl] == 2 || pos[pl] == 17 || pos[pl] == 19 || pos[pl] == 41
			|| pos[pl] == 42 || pos[pl] == 60 || pos[pl] == 63 || pos[pl] == 64)) {
		power = 2;
	}
	else if(army[pl].canSwim()&&
		(pos[pl]==1||pos[pl]==4||pos[pl]==5||pos[pl]==12
			||pos[pl]==22||(pos[pl]>=26&&pos[pl]<=30)||pos[pl]==42)){
		power = 3;
	}
	//if (power != 1)power = 3;//ÓÎÓ¾¼¼ÄÜ²âÊÔ
	Map4Plr map(nodeCDleft, army[pl].getCards(), pos[pl], coin[pl], power);
	int op = pl == RED ? players.playerRedAI(0, map) 
		: players.playerBlueAI(0, map);
	int M = nodeU[pos[pl] + 1] - nodeU[pos[pl]];
	int MM = M;
	if (power == 1)MM = M + 9;
	if (power == 2)MM = M + 8;
	if (power == 3)MM = M + 11;
	op = (op % MM + MM) % MM;
	if (op < M) {
		printf("MOVE %d %d %d\n", pl, pos[pl], edgeV[nodeU[pos[pl]] + op]);
		pos[pl] = edgeV[nodeU[pos[pl]] + op];
aftermove:	if (nodeCDleft[pos[pl]] == 0) {
			nodeCDleft[pos[pl]] = nodeCD[pos[pl]];
			coin[pl]++;
			printf("PICK %d %d\n", pl, pos[pl]);
		}
		else if (nodeCDleft[pos[pl]] < 0) {
			if (coin[pl] >= -nodeCDleft[pos[pl]]) {
				coin[pl] += nodeCDleft[pos[pl]];
				shop.generate();
				printf("LAB %d %d\n", shop.view() / 10, shop.view() % 10);
				Map4Plr map2(nodeCDleft, army[pl].getCards(), pos[pl], coin[pl], 0);
				int op2 = pl == RED ? players.playerRedAI(shop.view(), map2)
					: players.playerBlueAI(shop.view(), map2);
				op2 = (op2 % CHOICE_COUNT + CHOICE_COUNT) % CHOICE_COUNT;
				shop.take(op2);
				bool success=false;
				if (op2 == Shop::LEFT)success=army[pl].addCard(shop.view() / 10);
				else if (op2 == Shop::RIGHT)success=army[pl].addCard(shop.view() % 10);
				if (success)printf("ADDMAJ %d %d\n", pl, op2);
				else printf("FLY %d %d\n", pl, op2);
				nodeCDleft[pos[pl]]--;
			}
		}
	}
	else if(op>=M){
		op -= M - 1;
		if (power == 1) {
			if (!army[pl].full(op)) {
				army[pl].addCard(op);
				printf("DELIVER %d %d\n", pl, op);
			}
		}
		if (power == 2) {
			int oldpos = pos[pl];
			if (op == 1)pos[pl] = 2;
			if (op == 2)pos[pl] = 17;
			if (op == 3)pos[pl] = 19;
			if (op == 4)pos[pl] = 41;
			if (op == 5)pos[pl] = 42;
			if (op == 6)pos[pl] = 60;
			if (op == 7)pos[pl] = 63;
			if (op == 8)pos[pl] = 64;
			printf("OUTERRING %d %d %d\n", pl, oldpos, pos[pl]);
			goto aftermove;
		}
		if (power == 3) {
			int oldpos = pos[pl];
			if (op == 1)pos[pl] = 1;
			if (op == 2)pos[pl] = 4;
			if (op == 3)pos[pl] = 6;
			if (op == 4)pos[pl] = 12;
			if (op == 5)pos[pl] = 22;
			if (op == 6)pos[pl] = 26;
			if (op == 7)pos[pl] = 27;
			if (op == 8)pos[pl] = 28;
			if (op == 9)pos[pl] = 29;
			if (op == 10)pos[pl] = 30;
			if (op == 11)pos[pl] = 42;
			printf("WATERWAY %d %d %d\n", pl, oldpos, pos[pl]);
			goto aftermove;
		}
	}
}

void Logic::GameRun() {
	for (int i = 1; i <= TURN_COUNT; i++) {
		updateMap();
		if (rand() % 2) {
			SinglePlayerRun(RED);
			SinglePlayerRun(BLUE);
		}
		else {
			SinglePlayerRun(BLUE);
			SinglePlayerRun(RED);
		}
		if (finishTurn[RED]==0 && army[RED].countCard() == 13)
			finishTurn[RED] = i;
		if (finishTurn[BLUE]==0 && army[BLUE].countCard() == 13)
			finishTurn[BLUE] = i;
	}
	printf("RED %d %d %d\n", army[RED].calcTing(), army[RED].calcScore(), finishTurn[RED]);
	printf("BLUE %d %d %d\n", army[BLUE].calcTing(), army[BLUE].calcScore(), finishTurn[BLUE]);
}
