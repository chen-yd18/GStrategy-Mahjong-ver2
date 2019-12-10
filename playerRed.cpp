#include "player.h"
#include <stdlib.h>
//你可以自由地包含C++自带的头文件，以及本项目里的其他头文件

//你可以任意定义全局变量
int vistime[NODE_COUNT + 1];

int Player::playerRedAI(int mode, Map4Plr map) {
	//你可以任意定义局部变量
	
	//你可以在这里任意发挥，
	//实现高明的策略，打造你的强力AI！
	if (mode == 0) {
		int mini = nodeU[map.pos];
		for (int i = nodeU[map.pos] + 1; i < nodeU[map.pos + 1]; i++)
			if (vistime[edgeV[i]] < vistime[edgeV[mini]])mini = i;
		vistime[edgeV[mini]]++;
		return mini - nodeU[map.pos];
	}
	if (mode >= 12 && mode <= 89) {
		int x = mode / 10, y = mode % 10;
		if (x > 5)x = 10 - x;
		if (y > 5)y = 10 - y;
		return x < y;
	}
	//千万别忘了返回值！
	//告诉你个小秘密，直接返回0可以得分哦~
}