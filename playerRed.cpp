#include "player.h"
#include <stdlib.h>
//��������ɵذ���C++�Դ���ͷ�ļ����Լ�����Ŀ�������ͷ�ļ�

//��������ⶨ��ȫ�ֱ���
int vistime[NODE_COUNT + 1];

int Player::playerRedAI(int mode, Map4Plr map) {
	//��������ⶨ��ֲ�����
	
	//��������������ⷢ�ӣ�
	//ʵ�ָ����Ĳ��ԣ��������ǿ��AI��
	if (mode == 0) {
		int mini = nodeU[map.pos];
		for (int i = nodeU[map.pos] + 1; i < nodeU[map.pos + 1]; i++)
			if (vistime[edgeV[i]] < vistime[edgeV[mini]])mini = i;
		vistime[edgeV[mini]]++;
		return mini - nodeU[map.pos];
	}
	if (mode >= 12 && mode <= 89) {
		int x = mode / 10, y = mode % 10;
		if (x > 5)x = 10 - x;
		if (y > 5)y = 10 - y;
		return x < y;
	}
	//ǧ������˷���ֵ��
	//�������С���ܣ�ֱ�ӷ���0���Ե÷�Ŷ~
}