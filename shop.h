#pragma once

class Shop {
private:
	int cardLeft[10];
	int left, right;
public:
	Shop();
	enum choice{LEFT, RIGHT};
	void generate();//��������̵��е�������
	void take(int ch);//����ѡ���Ӧ����
	int view();//�鿴��ǰ�̵��е��ƣ�����ֵ��ʮλΪ��ߵ��ƣ���λΪ�ұߵ���
};