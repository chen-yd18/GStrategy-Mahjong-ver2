#ifndef RONIA_H
#define RONIA_H

//ά��ѡ��ӵ�е��ƣ�������������Ϣ�ͷ�ֵ
class ronia{
private:
	static const int MZ_COUNT = 16;//���ܳ��ֵġ����ӡ�����
	static const int mianzi[MZ_COUNT];
	//��һ��9λ�İ˽�����������һ���齫�ơ��ӵ͵������ĵ�iλ��ʾ����ֵΪi��ϡ�о����������0~4��ȡֵ����
	int cardcount[10];//ѡ��ӵ�еĸ���ϡ�о���ĸ���
public:
	ronia();
	bool addCard(int cardid);//ѡ�ֻ��������ֵΪcardid��ϡ�о���
	int countCard();//ѡ�ֵ�ϡ�о������
	bool dfs(int remain);//�жϰ˽�����remain��Ӧ�����Ƿ����
	int calcTing();//��������Щ��
	int calcScore();//����������Ϣ��ӵ�е�ϡ�о������������
	bool canBus();
	bool canSwim();
	bool full(int cardid);
	int* const getCards() { return cardcount; }
};

#endif