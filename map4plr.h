#pragma once

#include "const.h"

class Map4Plr {
public:
	int nodeCDleft[NODE_COUNT + 1];
	int pos;
	int purple;
	int power;
	Map4Plr(int* const logicNCL, int po, int pu, int pw);
};