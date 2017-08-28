#ifndef  SERVERLISTENERAPI_H
#define  SERVERLISTENERAPI_H
#include "common.h"
#include "queue_safe.h"
#include "CThread.h"
#include "CacheManager.h"
class ServerListener :public CThread{
private:
	queue_safe<CommandGroupStruct>& group_command_queue;
	CacheManager& cacheManager;
	int sleepTime;
public:

	
	ServerListener(queue_safe<CommandGroupStruct>& group_command_queue_, CacheManager& cacheManager_, int sleepTime_=DEFAULT_SLEEP_TIME);
	void init();//��ʼ��
	virtual void run() override; //�̺߳���������
	void productCommand();//�ӻ�������ȡ����
	~ServerListener();//��������

};




#endif