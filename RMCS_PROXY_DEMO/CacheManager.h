#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "CThread.h"
#include "common.h"
#include <string>
#include "queue_safe.h"
#include <map>
#include <vector>
#include <mutex>
#include <condition_variable>


using namespace std;
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
	queue_safe<map<string,vector<string>>> family_name_queue;
	queue_safe<vector<GroupStruct>> group_struct_queue;//��Ҫ����ˢ
	queue_safe<GroupfeedbackCustomStruct> group_feedback_queue;
	
	bool canUseRedis;

	std::condition_variable data_cond;  
public:
	CacheManager(string ip_,int port_,int sleep_time=DEFAULT_SLEEP_TIME);
	~CacheManager();
	void initCacheManager();//���ӣ����̵߳ȵȲ���
	CacheConnection cacheConnect;
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
	void customGroupFeedBack(); //���Ķ��������groupFeedBack
	void setIpAndPort(string ip_,string port_);//��������
	void run() override;//�������к���
	bool reconnect();//��������

	bool flushCacheAndItNameList(const string& key ,vector<string>& list);//�Ѷ����Ž���������
	bool updateGroupFeedBack(GroupfeedbackCustomStruct gfd);//���»���feedbacklist,ֻ������
	 
	int sleep_time;
private:
	vector<GroupStruct> getGroupInCache_pri(string des);//��ȡcache�е�list
	bool flushCacheGroupState(GroupStruct gst);//ˢ�»��������groupstrut
	bool flushCacheGroupFeedBackList(GroupfeedbackCustomStruct gfd);//ˢ��Զ�˻��������groupFeedback
};


#endif // !