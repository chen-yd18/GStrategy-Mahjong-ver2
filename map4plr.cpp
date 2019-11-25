#include "map4plr.h"

Map4Plr::Map4Plr(int* const logicNCL) {
	nodeCDleft[0] = 0;
	for (int i = 1; i <= NODE_COUNT; i++)
		nodeCDleft[i] = logicNCL[i];
}