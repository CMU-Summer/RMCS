#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include "CThread.h"
class DataBaseManager:public CThread
{
	//���Ǹ��߳���
	//����ά��һ������
	//run��������feedback��
	//�����뵽���غ�Զ�˵����ݿ�
	//�ṩһЩ�ӿ�
	//��feedback�������ܹ����������������
	//�����feedback�Ǹ�ʽ�ɶ������
	//������Ҫά������д���ݿ��������
	//һ�����ӱ��صģ�һ������Զ�̵�
	//��ȡ������Ľӿڣ��������ݿ��ʵ��д��
public:
	DataBaseManager(){}
	~DataBaseManager(){

	}
	void run() override;
	void init() ;
private:



};
void DataBaseManager::run(){




}
class DataBaseConnection{
	//������װ�˶����ݿ�����ӣ���ȡconnection����
	//init������������ݿ�����,�������
	//insert�������д��




};



#endif // !