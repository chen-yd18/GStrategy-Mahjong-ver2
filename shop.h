#pragma once

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