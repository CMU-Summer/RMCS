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
	this->start();//����

}//��ʼ��
void ServerListener::run(){
	while(true) {
	
		productCommand();
		if (this->sleepTime > 0) {
			this_thread::sleep_for(std::chrono::milliseconds(this->sleepTime));
		}
	
	}



} //�̺߳���������
void ServerListener::productCommand(){
	string s= this->cacheManager.getGroupCommandJsonStrFromCache();
	if (s == "") {
		
	}
	else {
		//�յ������ˣ�,����һ�£������������
		CommandGroupStruct c;
		c.DeSerialize(s.data());
		group_command_queue.push(c);//�Ž�ȥ
	}


}//�ӻ�������ȡ����


ServerListener::~ServerListener(){



}//��������