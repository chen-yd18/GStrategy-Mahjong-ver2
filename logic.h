#pragma once

#include "const.h"
#include "ronia.h"
#include "shop.h"
#include "player.h"

class Logic {
private:
	int pos[PLAYER_COUNT];//双方位置
	ronia army[PLAYER_COUNT];//双方稀有精灵战队
	int coin[PLAYER_COUNT];//双方紫精灵数量
	Shop shop;
	Player players;
	int nodeCDleft[NODE_COUNT+1];
	Logic();
	static Logic* logic;
	void SinglePlayerRun(int pl);
	void updateMap();
public:
	static Logic* getInstance();
	void GameRun();
};