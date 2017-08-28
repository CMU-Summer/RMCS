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
	void init();//初始化
	virtual void run() override; //线程函数跑起来
	void productCommand();//从缓存里面取命令
	~ServerListener();//析构函数

};




#endif