#define _CRT_SECURE_NO_WARNINGS
#include "ronia.h"
#include <stdio.h>

const int ronia::mianzi[MZ_COUNT]={0000000111,0000001110,0000011100,0000111000,0001110000,0011100000,0111000000,
	0000000003,0000000030,0000000300,0000003000,0000030000,0000300000,0003000000,0030000000,0300000000};

ronia::ronia() {
	for (int i = 1; i <= 9; i++) 
		cardcount[i] = 0;
}

void ronia::addCard(int cardid) {
	cardcount[cardid]++;
}

bool ronia::dfs(int remain){
	for(int i=0;i<9;i++)
	if(remain==(2<<(3*i)))return 1;
	for(int i=0;i<MZ_COUNT;i++){
		bool ok=1;
		for(int j=0;j<9;j++)
		if((mianzi[i]&(7<<(3*j)))>(remain&(7<<(3*j))))
		{ok=0;break;}
		if(ok&&dfs(remain-mianzi[i]))return 1;
	}
	return 0;
}
int ronia::calcTing(){
	if (countCard() != 13)return 0;
	int remain=0;
	for(int i=0;i<9;i++)
		remain|=(cardcount[i+1]<<(3*i));
	int ans=0;
	for(int i=0;i<9;i++){
		if(cardcount[i+1]<4){
		remain+=(1<<(3*i));
		if (dfs(remain))ans = ans * 10 + (i + 1);
		remain-=(1<<(3*i));
		}
	}
	return ans;
}

int ronia::countCard() {
	int ans = 0;
	for (int i = 1; i <= 9; i++) {
		ans += cardcount[i];
		//printf("%d ", cardcount[i]);
	}
	//printf("\n");
	return ans;
}
int ronia::calcScore() {
	int cardTot = countCard(), keTot = 0;
	for (int i = 1; i <= 9; i++) 
		if (cardcount[i] >= 3)keTot++;
	//keTot不一定等于胡牌中的刻子数量，但keTot==4时，刻子数量一定为4
	int score = 4*(keTot==4);
	if (cardTot != 13) return score + cardTot;
	int ting = calcTing();
	int gpa = 4 * !(cardcount[2] || cardcount[3] || cardcount[5] || cardcount[9]);
	while (ting) {
		score++;
		int m = ting % 10;
		if (m == 1 || m == 4 || m == 6 || m == 7 || m == 8)score += gpa;
		ting /= 10;
	}
	return score + 13;
}