#ifndef COMMANDCUSTOMER_H
#define COMMANDCUSTOMER_H
#include "CThread.h"
class CommandCustomer:public CThread
{

	//����һ���߳��࣬
	//��ͣ������commandQueue�����е�����
	//commandQueue�����У�ÿһ��Ԫ�أ���ָ����Ҫ���������group�������е�module
	//�Լ�����ĸ�ʽ
	//Ԫ�ص�������Ҫ�����ﶨ��
	//feedback�ᱻfeedbackcustomer����

public:
	CommandCustomer();
	~CommandCustomer();
	void run() override; //��дrun

private:

};

#endif // !
