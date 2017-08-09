#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "CThread.h"
#include <queue>
#include <string>
#include <queue>
#include "LookUpManager.h"
#include <map>
#include <vector>
using namespace std;
using namespace hebi;
class CacheManager:public CThread
{
	/**
	������Ǹ��߳��࣬����ά���������У�
	��feedback�����߷ŵ�feedback�����������
	lookup����»��������familyAndNames map
	�ṩ�����е��б����map�Ĳ����ӿ�
	*/
private:
	bool isConnect;//�Ƿ��Ѿ�����
	queue<map<string,vector<string>>> family_name_queue;
	queue<vector<GroupStruct>> group_struct_queue;//��Ҫ����ˢ
public:
	CacheManager(string ip_,string port_);
	~CacheManager();
	void initCacheManager();//���ӣ����̵߳ȵȲ���
	
	vector<GroupStruct> getGroupInCache();//��ȡcache�е�list
	bool updateCacheFamilyAndItsNames(map<string,vector<string>> familyMap);
	//�����ڲ��������
	//����������ϵģ���ô���ص���true
	//�������ͨ���ڱ���̵߳�
	//����
	bool updateCacheGroupStateList(vector<GroupStruct> groupStructVec);
	//�������
	//��
	bool isConnected();//�Ƿ����������ϵ�
	void customfamilyMap();//���Ķ��������familyMap����
	void customGroupStateMap(); // ���Ķ��������groupStructVec����
	void setIpAndPort(string ip_,string port_);//��������
	void run() override;
private:

};
class CacheConnection{
	/*
	��������ײ�Ի��������
	init���ӻ���
	get/set ����
	
	*/
private:
	string ip;
	string port;
public:
	CacheConnection(string ip_,string port_);
	~CacheConnection();
	bool connect();//����
	bool reconnect();//��������
	bool disconnect();//�Ͽ�
	bool isConnected();//�Ƿ�����
	void setIpAndPort(string ip_,string port_);//��������

};

#endif // !
