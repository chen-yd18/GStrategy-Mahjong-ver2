#include "player.h"
#include <stdlib.h>
//��������ɵذ���C++�Դ���ͷ�ļ����Լ�����Ŀ�������ͷ�ļ�

//��������ⶨ��ȫ�ֱ���
int remaining[10];
int vistime[NODE_COUNT + 1];

int Player::playerRedAI(int mode, Map4Plr map) {
	//��������ⶨ��ֲ�����
	
	//��������������ⷢ�ӣ�
	//ʵ�ָ����Ĳ��ԣ��������ǿ��AI��
	if (remaining[0] == 0) {
		for (int i = 0; i <= 9; i++)remaining[i] = 1;
		remaining[1] = remaining[9] = 3;
	}
	if (mode == 0) {
		int mini = nodeU[map.pos];
		for (int i = nodeU[map.pos] + 1; i < nodeU[map.pos + 1]; i++)
			if (vistime[edgeV[i]] < vistime[edgeV[mini]])mini = i;
		vistime[edgeV[mini]]++;
		return mini - nodeU[map.pos];
	}
	if (mode >= 12 && mode <= 89) {
		int x = mode / 10, y = mode % 10;
		if (remaining[x] > remaining[y]) {
			remaining[x]--;
			return 0;
		}
		else {
			remaining[y]--;
			return 1;
		}
	}
	//ǧ������˷���ֵ��
	//�������С���ܣ�ֱ�ӷ���0���Ե÷�Ŷ~
}