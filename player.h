#pragma once

#include "const.h"
#include "map4plr.h"

class Player {
public:
	int playerRedAI(int mode, Map4Plr map);
	//mode==0表示选手需要进行移动操作，该函数应当返回一个0~（当前所在结点的度-1）之间的整数k
		//之后选手会移动到与当前结点相邻的编号第(k+1)小的结点
	//12<=mode<=89时，选手需要选择稀有精灵，该函数应当返回一个0~1之间的整数c
		//c为0表示选择左边的（即十位数），c为1表示选择右边的（即个位数）
	//若函数的返回值不在范围内，系统会自动将返回值对（最大合法值+1）取模，
		//若取模结果为负数，则再加上（最大合法值+1）。
	//目前map中只有一个数组nodeCDleft。对于每个结点i，nodeCDleft[i]:
		//值为负时，说明该实验室的当前价格为该值的相反数
		//值为零时，说明结点上有紫精灵
		//值为正时，说明结点上会在这么多个回合后出现紫精灵
	int playerBlueAI(int mode, Map4Plr map);
};