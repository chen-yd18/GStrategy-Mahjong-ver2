#include "map4plr.h"

Map4Plr::Map4Plr(int* const logicNCL, int po, int pu, int pw)
			:pos(po),purple(pu),power(pw)
{
	nodeCDleft[0] = 0;
	for (int i = 1; i <= NODE_COUNT; i++)
		nodeCDleft[i] = logicNCL[i];
}