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
	int nodeCDleft[NODE_COUNT+1];//这个数组非常重要！
		//值为负时，说明该实验室的当前价格为该值的相反数
		//值为零时，说明结点上有紫精灵
		//值为正时，说明结点上会在这么多个回合后出现紫精灵
		//该数组就是地图状态，会被传递给选手的AI函数。
	Logic();
	static Logic* logic;
	void SinglePlayerRun(int pl);
	void updateMap();
public:
	static Logic* getInstance();
	void GameRun();
};