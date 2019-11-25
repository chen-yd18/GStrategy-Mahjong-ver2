#pragma once

#include "const.h"
#include "ronia.h"
#include "shop.h"
#include "player.h"

class Logic {
private:
	int pos[PLAYER_COUNT];//˫��λ��
	ronia army[PLAYER_COUNT];//˫��ϡ�о���ս��
	int coin[PLAYER_COUNT];//˫���Ͼ�������
	Shop shop;
	Player players;
	int nodeCDleft[NODE_COUNT+1];//�������ǳ���Ҫ��
		//ֵΪ��ʱ��˵����ʵ���ҵĵ�ǰ�۸�Ϊ��ֵ���෴��
		//ֵΪ��ʱ��˵����������Ͼ���
		//ֵΪ��ʱ��˵������ϻ�����ô����غϺ�����Ͼ���
		//��������ǵ�ͼ״̬���ᱻ���ݸ�ѡ�ֵ�AI������
	Logic();
	static Logic* logic;
	void SinglePlayerRun(int pl);
	void updateMap();
public:
	static Logic* getInstance();
	void GameRun();
};