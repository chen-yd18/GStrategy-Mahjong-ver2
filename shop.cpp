#include "shop.h"
#include <stdlib.h>
#include <time.h>
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