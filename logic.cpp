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
	Map4Plr map(nodeCDleft, pos[pl]);
	int op = pl == RED ? players.playerRedAI(0, map) 
		: players.playerBlueAI(0, map);
	int M = nodeU[pos[pl] + 1] - nodeU[pos[pl]];
	op = (op % M + M) % M;
	printf("MOVE %d %d %d\n", pl, pos[pl], edgeV[nodeU[pos[pl]] + op]);
	pos[pl] = edgeV[nodeU[pos[pl]] + op];
	if (nodeCDleft[pos[pl]] == 0) {
		nodeCDleft[pos[pl]] = nodeCD[pos[pl]];
		coin[pl]++;
		printf("PICK %d %d\n", pl, pos[pl]);
	}
	else if (nodeCDleft[pos[pl]] < 0) {
		if (coin[pl] >= -nodeCDleft[pos[pl]]) {
			coin[pl] += nodeCDleft[pos[pl]];
			shop.generate();
			printf("LAB %d %d\n", shop.view() / 10, shop.view() % 10);
			Map4Plr map2(nodeCDleft, pos[pl]);
			int op2 = pl == RED ? players.playerRedAI(shop.view(), map2) 
				: players.playerBlueAI(shop.view(), map2);
			op2 = (op2 % CHOICE_COUNT + CHOICE_COUNT) % CHOICE_COUNT;
			shop.take(op2);
			printf("ADDMAJ %d %d\n", pl, op2);
			if (op2 == Shop::LEFT)army[pl].addCard(shop.view() / 10);
			else if (op2 == Shop::RIGHT)army[pl].addCard(shop.view() % 10);
			nodeCDleft[pos[pl]]--;
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
	}
	printf("RED %d %d\n", army[RED].calcTing(), army[RED].calcScore());
	printf("BLUE %d %d\n", army[BLUE].calcTing(), army[BLUE].calcScore());
}
