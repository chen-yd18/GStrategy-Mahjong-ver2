#include "player.h"
#include <stdlib.h>
//你可以自由地包含C++自带的头文件，以及本项目里的其他头文件
#include <queue>
#include <string.h>

//你可以任意定义全局变量
int remaining[10];
int vistime[NODE_COUNT + 1];

namespace sp {
	int d[NODE_COUNT + 1];
	void getD(int t, int po) {// 技能激活状态为po时，求出t到各点的最短路长度
		std::queue<int> Q;
		Q.push(t);
		memset(d, -1, sizeof(d));
		d[t] = 0;
		while (!Q.empty()) {
			int h = Q.front();
			Q.pop();
			bool power3 = false;
			for(int i=0;i<RIVERSIDE_COUNT;i++)
				if (riversides[i] == h) {
					power3 = true;
					break;
				}
			power3 &= (po == 3);
			for (int i = nodeU[h]; i < nodeU[h + 1]; i++) {
				int j = edgeV[i];
				if (d[j] == -1) {
					d[j] = d[h] + 1;
					Q.push(j);
				}
			}
			if (power3) {
				for (int i = 0; i < RIVERSIDE_COUNT; i++) {
					int j = riversides[i];
					if (j != h && d[j] == -1) {
						d[j] = d[h] + 1;
						Q.push(j);
					}
				}
			}
		}
	}
	int dist(int s, int t, int po) {// 技能激活状态为po时，s到t的最短路长度
		getD(t, po);
		if (d[s] == -1)return 19260817;
		return d[s];
	}
	int dir(int s, int t, int po) {// 技能激活状态为po时，从s出发，目标点为t，下一步应该走的方向（AI返回值）
		if (s == t)return rand();
		getD(t, po);
		for (int i = nodeU[s]; i < nodeU[s + 1]; i++) {
			int j = edgeV[i];
			if (d[j] != -1 && d[j] == d[s] - 1) {
				return i-nodeU[s];
			}
		}
		bool power3 = false;
		for (int i = 0; i < RIVERSIDE_COUNT; i++)
			if (riversides[i] == s) {
				power3 = true;
				break;
			}
		power3 &= (po == 3);
		if (power3) {
			for (int i = 0; i < RIVERSIDE_COUNT; i++) {
				int j = riversides[i];
				if (d[j] != -1 && d[j] == d[s] - 1) {
					return nodeU[s + 1] - nodeU[s] + i;
				}
			}
		}
		exit(233);//程序异常
		return -1;
	}
	int goal = 0, bestd = 19260817;
}

namespace dp {
	int shops[15], shopcnt;
	int dp[15][32768], fa[2][15][32768];
	int path[15];//依次经过的商店编号
	void initdp() {
		for (int i = 1; i <= NODE_COUNT; i++)
			if (nodeCD[i] < 0) {
				shops[shopcnt++] = i;
			}
		memset(dp, 0x3f, sizeof(dp));
		dp[1][2] = 0;
		
				for (int j = 0; j < (1 << shopcnt); j++)
					for (int i = 0; i < shopcnt; i++)
						for (int k = 0; k < shopcnt; k++)
					if (((1 << k) & j) != 0 && ((1 << i) & j) == 0) {
						int z;
						/*if (i == k)z = 2;
						else*/ z = sp::dist(shops[k], shops[i], 0);
						if (dp[k][j] + z < dp[i][j | (1 << i)]) {
							dp[i][j | (1 << i)] = dp[k][j] + z;
							fa[0][i][j | (1 << i)] = k;
							fa[1][i][j | (1 << i)] = j;
						}
					}
		int x = 0, y = (1 << shopcnt) - 1;
		for (int i = 1; i < shopcnt; i++)
			if (dp[i][y] < dp[x][y])x = i;
		for(int i=shopcnt;y;i--) {
			path[i] = shops[x];
			int xx = x;
			x = fa[0][xx][y];
			y = fa[1][xx][y];
		}
		for (int i = 1; i <= shopcnt; i++)
			printf("%d ", path[i]);
		printf("\n");
	}
}
int Player::playerRedAI(int mode, Map4Plr map) {
	//你可以任意定义局部变量
	
	//你可以在这里任意发挥，
	//实现高明的策略，打造你的强力AI！
	if (dp::shopcnt == 0) {
		dp::initdp();
		sp::goal = dp::path[1];
		sp::bestd = sp::dist(map.pos, sp::goal, map.power);
	}
	if (mode == 0) {
		if (map.pos == sp::goal) {
			sp::goal = 0;
			sp::bestd = 19260817;
			if (map.holding[0] + 2 <= dp::shopcnt 
				&& map.purple >= -map.nodeCDleft[dp::path[map.holding[0] + 2]]) {
				sp::goal = dp::path[map.holding[0]+2];
				printf("%d\n", sp::goal);
				sp::bestd = sp::dist(map.pos, sp::goal, map.power);
			}
		}
		//try labs first
		if (sp::goal == 0) {
			for (int i = 1; i <= NODE_COUNT; i++) {
				if (map.nodeCDleft[i] < 0) {
					if (i != map.pos && map.purple >= -map.nodeCDleft[i]) {
						int dis = sp::dist(i, map.pos, map.power);
						if (dis < sp::bestd) {
							sp::bestd = dis;
							sp::goal = i;
						}
					}
				}
			}
		}
		//try purples
		if (sp::goal == 0) {
			for (int i = 1; i <= NODE_COUNT; i++) {
				if (map.nodeCDleft[i] == 0) {
					if (i != map.pos) {
						int dis = sp::dist(i, map.pos, map.power);
						if (dis < sp::bestd) {
							sp::bestd = dis;
							sp::goal = i;
						}
					}
				}
			}
		}
		//printf("%d %d %d\n", sp::goal, sp::bestd, map.pos);
		if (sp::goal != 0) {
			//printf("%d\n", sp::dir(map.pos, sp::goal, map.power));
			return sp::dir(map.pos,sp::goal, map.power);
		}
		else return 0;
	}
	if (mode >= 12 && mode <= 89) {
		int x = mode / 10, y = mode % 10;
		int weightx = 1, weighty = 1;
		if (x == 5)weightx = 5;
		if (x == 3 || x == 7)weightx = 4;
		if (x == 4 || x == 6)weightx = 3;
		if (x == 2 || x == 8)weightx = 2;
		if (y == 5)weighty = 5;
		if (y == 3 || y == 7)weighty = 4;
		if (y == 4 || y == 6)weighty = 3;
		if (y == 2 || y == 8)weighty = 2;
		return weightx < weighty;
	}
	//千万别忘了返回值！
	//告诉你个小秘密，直接返回0可以得分哦~
}