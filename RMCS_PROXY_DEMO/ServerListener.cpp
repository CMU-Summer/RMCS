#include "ServerListenerApi.h"

ServerListener::ServerListener(queue_safe<CommandGroupStruct>& group_command_queue_, 
	CacheManager& cacheManager_,
	int sleepTime_
	):
	group_command_queue(group_command_queue_),
	cacheManager(cacheManager_),
	sleepTime(sleepTime_)
{




}
void ServerListener::init(){
	this->start();//启动

}//初始化
void ServerListener::run(){
	while(true) {
	
		productCommand();
		if (this->sleepTime > 0) {
			this_thread::sleep_for(std::chrono::milliseconds(this->sleepTime));
		}
	
	}



} //线程函数跑起来
void ServerListener::productCommand(){
	string s= this->cacheManager.getGroupCommandJsonStrFromCache();
	if (s == "") {
		
	}
	else {
		//收到命令了，,解析一下，放入队列里面
		CommandGroupStruct c;
		c.DeSerialize(s.data());
		group_command_queue.push(c);//放进去
	}


}//从缓存里面取命令


ServerListener::~ServerListener(){



}//析构函数