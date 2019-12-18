#include "map4plr.h"

Map4Plr::Map4Plr(int* const logicNCL, int* const logicHold, int po, int pu, int pw)
			:pos(po),purple(pu),power(pw)
{
	nodeCDleft[0] = 0;
	for (int i = 1; i <= NODE_COUNT; i++)
		nodeCDleft[i] = logicNCL[i];
	holding[0] = 0;
	for (int i = 1; i <= 9; i++) {
		holding[i] = logicHold[i];
		holding[0] += holding[i];
	}
}