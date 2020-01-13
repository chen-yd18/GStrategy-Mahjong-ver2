#include "bot.h"
#include <stdio.h>

//游戏用到的常量 
#ifndef CONST
#define CONST 
const int NODE_COUNT = 64;//结点数
const int EDGE_COUNT = 111;//双向道路数
const int NODE_START = 6;//起点编号
const int CHOICE_COUNT = 2;//实验室内选项个数
const int TURN_COUNT = 100;//回合数
const int PLAYER_COUNT = 2;//玩家数量
const int GATE_COUNT = 8;//校门数量
const int RIVERSIDE_COUNT = 11;//校河边的节点数量
enum player { RED, BLUE };//玩家代号

//每个结点每隔多少回合会刷出一个紫精灵。-1表示实验室
const int nodeCD[NODE_COUNT + 1] = {0,
	1, -1, 999, 3, -1, 999, 999, 999,
	999, 2, 999, 999, 999, 4, 999, 4,
	-1, 999, 2, 999, 999, 5, -1, 999,
	999, 3, 999, 999, -1, 999, 999, 6,
	999, -1, 999, 999, 999, 6, 6, 999,
	999, 999, 999, 2, 999, 999, 999, 999,
	-1, -1, -1, 999, 999, 2, 999, 999,
	999, 3, 999, 1, 999, 999, 999, -1
};
//校门列表
const int gates[GATE_COUNT] = {
	2, 17, 19, 41, 42, 60, 63, 64 
};
//校河边的节点列表
const int riversides[RIVERSIDE_COUNT] = {
	1, 4, 6, 12, 22, 26, 27, 28, 29, 30, 42 
};

//地图的正向表形式
const int nodeU[NODE_COUNT + 2] = {0,
	1, 5, 6, 9, 13, 19, 25, 29,
	33, 37, 40, 44, 48, 52, 56, 59, 
	62, 65, 67, 69, 73, 78, 82, 86, 
	89, 93, 96, 98, 102, 105, 110, 113, 
	117, 121, 124, 126, 129, 132, 135, 139,
	142, 146, 149, 153, 157, 160, 164, 168, 
	172, 175, 178, 182, 185, 189, 193, 197, 
	201, 204, 208, 210, 212, 215, 218, 220,
	223
};
const int edgeV[2*EDGE_COUNT + 1] = {0,
	4, 5, 6, 8, 
	3, 
	2, 4, 11, 
	1, 3, 6, 21,
	1, 6, 7, 8, 13, 14, 
	1, 4, 5, 7, 12, 14, 
	5, 6, 9, 14, 
	1, 5, 9, 10, 

	7, 8, 10, 15, 
	8, 9, 17, 
	3, 12, 20, 21, 
	6, 11, 13, 22, 
	5, 12, 15, 23, 
	5, 6, 7, 17, 
	9, 13, 24,
	17, 24, 25, 

	10, 14, 16,
	19, 26, 
	18, 20, 
	11, 19, 21, 28, 
	4, 11, 20, 22, 30, 
	12, 21, 23, 30, 
	13, 22, 24, 31, 
	15, 16, 23, 32,

	16, 33, 41, 
	18, 27, 42, 
	26, 28, 
	20, 27, 29, 34, 
	28, 30, 36, 
	21, 22, 29, 31, 45, 
	23, 30, 37, 
	24, 33, 38, 39,

	25, 32, 40, 41, 
	28, 35, 43, 
	34, 36, 
	29, 35, 44, 
	31, 38, 46, 
	32, 37, 39,
	32, 38, 40, 47, 
	33, 39, 41, 

	25, 33, 40, 49, 
	26, 43, 50, 
	34, 42, 44, 55,
	36, 43, 45, 51, 
	30, 44, 46, 
	37, 45, 47, 52, 
	39, 46, 48, 53, 
	47, 49, 53, 54, 

	41, 48, 54, 
	42, 51, 55, 
	44, 50, 52, 56,
	46, 51, 58, 
	47, 48, 54, 64, 
	48, 49, 53, 59, 
	43, 50, 56, 60, 
	51, 55, 57, 61,

	56, 58, 62, 
	52, 57, 63, 64, 
	54, 64, 
	55, 61, 
	56, 60, 62, 
	57, 61, 63,
	58, 62, 
	53, 58, 59
};
#endif
//给选手的信息 
typedef struct{
	int nodeCDleft[NODE_COUNT + 1];
	int holding[10];
	int pos;
	int purple;
	int power;
}GameMap;
int side;

#include <queue>
#include <string.h>
#include <stdlib.h>

int remaining[10];
int vistime[NODE_COUNT + 1];

namespace sp {
	int d[NODE_COUNT + 1];
	void getD(int t, int po) {
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
	int dist(int s, int t, int po) {
		getD(t, po);
		if (d[s] == -1)return 19260817;
		return d[s];
	}
	int dir(int s, int t, int po) {
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
		exit(233);
		return -1;
	}
	int goal = 0, bestd = 19260817;
}

namespace dp {
	int shops[15], shopcnt;
	int dp[15][32768], fa[2][15][32768];
	int path[15];
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
		//for (int i = 1; i <= shopcnt; i++)
		//	printf("%d ", path[i]);
		//printf("\n");
	}
}
int playerAI(int mode, GameMap map) {


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
				//printf("%d\n", sp::goal);
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
}

//主函数，无需选手修改 
int main(){
	char *s;
	s = bot_recv();
	sscanf(s,"%d",&side); 
	while(1){
	s = bot_recv();
	int mode;
	GameMap M;
	sscanf(s,"%d%d%d%d%d%d%d%d%d%d%d%d%d"
"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",&mode, &M.pos,&M.purple,&M.power,
	&M.holding[1],&M.holding[2],&M.holding[3],&M.holding[4],&M.holding[5],
	&M.holding[6],&M.holding[7],&M.holding[8],&M.holding[9],
	&M.nodeCDleft[1],&M.nodeCDleft[2],&M.nodeCDleft[3],&M.nodeCDleft[4],
	&M.nodeCDleft[5],&M.nodeCDleft[6],&M.nodeCDleft[7],&M.nodeCDleft[8],
	&M.nodeCDleft[9],&M.nodeCDleft[10],&M.nodeCDleft[11],&M.nodeCDleft[12],
	&M.nodeCDleft[13],&M.nodeCDleft[14],&M.nodeCDleft[15],&M.nodeCDleft[16],
	&M.nodeCDleft[17],&M.nodeCDleft[18],&M.nodeCDleft[19],&M.nodeCDleft[20],
	&M.nodeCDleft[21],&M.nodeCDleft[22],&M.nodeCDleft[23],&M.nodeCDleft[24],
	&M.nodeCDleft[25],&M.nodeCDleft[26],&M.nodeCDleft[27],&M.nodeCDleft[28],
	&M.nodeCDleft[29],&M.nodeCDleft[30],&M.nodeCDleft[31],&M.nodeCDleft[32],
	&M.nodeCDleft[33],&M.nodeCDleft[34],&M.nodeCDleft[35],&M.nodeCDleft[36],
	&M.nodeCDleft[37],&M.nodeCDleft[38],&M.nodeCDleft[39],&M.nodeCDleft[40],
	&M.nodeCDleft[41],&M.nodeCDleft[42],&M.nodeCDleft[43],&M.nodeCDleft[44],
	&M.nodeCDleft[45],&M.nodeCDleft[46],&M.nodeCDleft[47],&M.nodeCDleft[48],
	&M.nodeCDleft[49],&M.nodeCDleft[50],&M.nodeCDleft[51],&M.nodeCDleft[52],
	&M.nodeCDleft[53],&M.nodeCDleft[54],&M.nodeCDleft[55],&M.nodeCDleft[56],
	&M.nodeCDleft[57],&M.nodeCDleft[58],&M.nodeCDleft[59],&M.nodeCDleft[60],
	&M.nodeCDleft[61],&M.nodeCDleft[62],&M.nodeCDleft[63],&M.nodeCDleft[64]
);
	/*for(int i = 1; i <= NODE_COUNT; i++){
		fprintf(stderr,"%d ",M.nodeCDleft[i]);
	}
fprintf(stderr,"\n");*/
	char res[12];
	sprintf(res,"%d",playerAI(mode, M));
	bot_send(res);
	}
	return 0;
}
