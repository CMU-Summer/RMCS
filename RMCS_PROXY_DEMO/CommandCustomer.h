#ifndef COMMANDCUSTOMER_H
#define COMMANDCUSTOMER_H
#include "CThread.h"
#include <string>
#include "LookUpManager.h"
#include "FeedBackManager.h"
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
	void init();//
	void getCommandStrut(string ct);
	bool customCommand();
private:

};
class CommandStruct{

	string cmd;
	GroupfeedbackCustomStruct gf;
	


};
#endif // !
