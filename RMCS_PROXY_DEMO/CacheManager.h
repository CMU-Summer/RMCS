#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "CThread.h"
class CacheManager:public CThread
{
	/**
	������Ǹ��߳��࣬����ά���������У�
	��feedback�����߷ŵ�feedback�����������
	lookup����»��������familyAndNames map
	�ṩ�����е��б����map�Ĳ����ӿ�
	*/
public:
	CacheManager();
	~CacheManager();
	void run() override;
private:

};
class CacheConnection{
	/*
	��������ײ�Ի��������
	init���ӻ���
	get/set ����
	
	*/




};






#endif // !
