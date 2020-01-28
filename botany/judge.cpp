#include "bot.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

class ronia{
private:
	static const int MZ_COUNT = 16;//可能出现的“面子”种数
	static const int mianzi[MZ_COUNT];
	//用一个9位的八进制数来描述一组麻将牌。从低到高数的第i位表示属性值为i的稀有精灵个数（在0~4间取值）。
	int cardcount[10];//选手拥有的各种稀有精灵的个数
public:
	ronia();
	bool addCard(int cardid);//选手获得了属性值为cardid的稀有精灵
	int countCard();//选手的稀有精灵个数
	bool dfs(int remain);//判断八进制数remain对应的牌是否胡牌
	int calcTing();//计算听哪些牌
	int calcScore();//根据听牌信息和拥有的稀有精灵来计算分数
	bool canBus();
	bool canSwim();
	bool full(int cardid);
	int* const getCards() { return cardcount; }
};

const int ronia::mianzi[MZ_COUNT]={0000000111,0000001110,0000011100,0000111000,0001110000,0011100000,0111000000,
	0000000003,0000000030,0000000300,0000003000,0000030000,0000300000,0003000000,0030000000,0300000000};

ronia::ronia() {
	for (int i = 1; i <= 9; i++) 
		cardcount[i] = 0;
}

bool ronia::addCard(int cardid) {
	if (full(cardid))return false;
	cardcount[cardid]++;
	return true;
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

bool ronia::canBus() {
	bool ok = 1;
	for (int i = 1; i <= 8; i++)
		if (cardcount[i] == 0)ok = 0;
	if (ok)return 1;
	for (int i = 2; i <= 9; i++)
		if (cardcount[i] == 0)ok = 0;
	return ok;
}

bool ronia::canSwim() {
	return cardcount[5] >= 3;
}

bool ronia::full(int cardid) {
	if (cardid < 1 || cardid>9)return true;
	return cardcount[cardid] >= 4;
}

class Shop {
private:
	int cardLeft[10];
	int left, right;
public:
	Shop();
	enum choice{LEFT, RIGHT};
	void generate();//随机产生商店中的两张牌
	void take(int ch);//拿走选项对应的牌
	int view();//查看当前商店中的牌，返回值的十位为左边的牌，个位为右边的牌
};

Shop::Shop() { 
	srand(time(0));
	for (int i = 1; i <= 9; i++)
		cardLeft[i] = 4; 
}

void Shop::generate() {
	int cardlist[36], cnt = 0;
	for (int i = 1; i <= 9; i++)
		for (int j = 1; j <= cardLeft[i]; j++)
			cardlist[cnt++] = i;
	int x, y;
	do {
		x = rand() % cnt;
		y = rand() % cnt;
	} while (cardlist[x] == cardlist[y]);
	left = cardlist[x];
	right = cardlist[y];
}

void Shop::take(int ch) {
	if (ch == LEFT) cardLeft[left]--;
	if (ch == RIGHT) cardLeft[right]--;
}

int Shop::view(){
	return left * 10 + right;
}
int pos[PLAYER_COUNT];//双方位置
	ronia army[PLAYER_COUNT];//双方稀有精灵战队
	int coin[PLAYER_COUNT];//双方紫精灵数量
	int finishTurn[PLAYER_COUNT];//双方各自收集齐13个稀有精灵的回合
	Shop shop;
	int nodeCDleft[NODE_COUNT+1];//这个数组非常重要！
		//值为负时，说明该实验室的当前价格为该值的相反数
		//值为零时，说明结点上有紫精灵
		//值为正时，说明结点上会在这么多个回合后出现紫精灵
		//该数组就是地图状态，会被传递给选手的AI函数。
		

//给玩家的信息 
typedef struct{
	int nodeCDleft[NODE_COUNT + 1];
	int holding[10];
	int pos;
	int purple;
	int power;
}GameMap;

//一堆库函数 
#define child_pause(__cp)   kill((__cp).pid, SIGSTOP)
#define child_resume(__cp)  kill((__cp).pid, SIGCONT)
#define child_kill(__cp)    kill((__cp).pid, SIGKILL)

childproc par[2];

//正片开始 
int win=-1;
void init() {
	pos[RED] = pos[BLUE] = NODE_START;
	coin[RED] = coin[BLUE] = 0;
	finishTurn[RED] = finishTurn[BLUE] = 0;
	for (int i = 1; i <= NODE_COUNT; i++)
		nodeCDleft[i] = nodeCD[i];
	srand(time(NULL));
}

int callBot(int mode, int pl, int power){
	char buf[500];
	sprintf(buf,"%d\n%d %d %d\n%d %d %d %d %d %d %d %d %d\n"
	"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
	"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
	"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d "
	"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",
	mode, pos[pl], coin[pl], power,
	army[pl].getCards()[1],army[pl].getCards()[2],army[pl].getCards()[3],
	army[pl].getCards()[4],army[pl].getCards()[5],army[pl].getCards()[6],
	army[pl].getCards()[7],army[pl].getCards()[8],army[pl].getCards()[9],
	nodeCDleft[1],nodeCDleft[2],nodeCDleft[3],nodeCDleft[4],
	nodeCDleft[5],nodeCDleft[6],nodeCDleft[7],nodeCDleft[8],
	nodeCDleft[9],nodeCDleft[10],nodeCDleft[11],nodeCDleft[12],
	nodeCDleft[13],nodeCDleft[14],nodeCDleft[15],nodeCDleft[16],
	nodeCDleft[17],nodeCDleft[18],nodeCDleft[19],nodeCDleft[20],
	nodeCDleft[21],nodeCDleft[22],nodeCDleft[23],nodeCDleft[24],
	nodeCDleft[25],nodeCDleft[26],nodeCDleft[27],nodeCDleft[28],
	nodeCDleft[29],nodeCDleft[30],nodeCDleft[31],nodeCDleft[32],
	nodeCDleft[33],nodeCDleft[34],nodeCDleft[35],nodeCDleft[36],
	nodeCDleft[37],nodeCDleft[38],nodeCDleft[39],nodeCDleft[40],
	nodeCDleft[41],nodeCDleft[42],nodeCDleft[43],nodeCDleft[44],
	nodeCDleft[45],nodeCDleft[46],nodeCDleft[47],nodeCDleft[48],
	nodeCDleft[49],nodeCDleft[50],nodeCDleft[51],nodeCDleft[52],
	nodeCDleft[53],nodeCDleft[54],nodeCDleft[55],nodeCDleft[56],
	nodeCDleft[57],nodeCDleft[58],nodeCDleft[59],nodeCDleft[60],
	nodeCDleft[61],nodeCDleft[62],nodeCDleft[63],nodeCDleft[64]);
	
	child_send(par[pl], buf);
size_t len;
    char *resp = child_recv(par[pl], &len, 1000);
    
    if (resp == NULL) {
        fprintf(stderr, "Side #%d errors with %d, considered resignation\n",
            pl, (int)len);
         win=pl^1;
         return 0; 
	} else {
		int ret=233;
		sscanf(resp,"%d",&ret);
		fprintf(stderr, "Side #%d moves at %d\n", pl, ret);
		return ret;
	}
}
void updateMap() {
	printf("COIN %d %d\n", coin[RED], coin[BLUE]);
	for (int i = 1; i <= NODE_COUNT; i++) 
		if (nodeCDleft[i] > 0) {
			nodeCDleft[i]--;
			if (nodeCDleft[i] == 0) {
				printf("SPAWN %d\n", i);
			}
		}
}

void SinglePlayerRun(int pl) {
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
	int op = callBot(0, pl, power);
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
				
				int op2 = callBot(shop.view(), pl, 0);
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
			printf("OUTERRING %d %d %d\n", pl, oldpos, pos[pl]);//OUTERRING
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
			printf("WATERWAY %d %d %d\n", pl, oldpos, pos[pl]);//WATERWAY
			goto aftermove;
		}
	}
}

int main(int argc, char *argv[])
{
    if (argc < 5) return 1;

    
    par[0] = child_create(argv[1], argv[3]);
    par[1] = child_create(argv[2], argv[4]);

    child_send(par[0], "0");
    child_send(par[1], "1");

    init();
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
		if (win != -1) {
            fprintf(stderr, "Side #%d wins!\n", win);
            break;
        }
	}
int rs,bs;
	printf("RED %d %d %d\n", army[RED].calcTing(), rs=army[RED].calcScore(), finishTurn[RED]);
	printf("BLUE %d %d %d\n", army[BLUE].calcTing(), bs=army[BLUE].calcScore(), finishTurn[BLUE]);
if(rs>bs)win=0;
if(rs<bs)win=1;
    printf("winner: %d\n", win);

    child_finish(par[0]);
    child_finish(par[1]);

    return 0;
}
