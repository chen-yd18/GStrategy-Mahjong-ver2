#ifndef RONIA_H
#define RONIA_H

//维护选手拥有的牌，并计算听牌信息和分值
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

#endif