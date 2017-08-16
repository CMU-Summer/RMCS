#include "CacheManager.h"
#include <mutex>
#include <condition_variable>

CacheManager::CacheManager(string ip_,int port_,
						   int sleep_time_):cacheConnect(ip_,port_),canUseRedis(true),sleep_time(sleep_time_){




}
CacheManager::~CacheManager(){}
void CacheManager::initCacheManager(){
	this->isConnect=this->cacheConnect.init();

	printf("CACHE_MANAGER_THREAD : redis connect status:%d\n",this->isConnect);
	this->start();//���߳�


}//���ӣ����̵߳ȵȲ���

vector<GroupStruct> CacheManager::getGroupInCache(){
	//����߳�����
	//cout << "get user groups from cache" << endl;
	printf("LOOKUPMANAGER_THREAD : get user groups from cache\n");
	return this->getGroupInCache_pri(GET_LIST);

}//��ȡcache�е�list,��Ҫ��

bool CacheManager::updateCacheFamilyAndItsNames(map<string,vector<string>> familyMap){
	//�Ѿ��ڶ��к����д�������

	printf("LOOKUP_MANAGER_THREAD : put family and its names into queue\n");
	if(this->isConnect == false)return false;
	else {
		this->family_name_queue.push(familyMap);
		return true;
	}

}
//�����ڲ��������
//����������ϵģ���ô���ص���true
//�������ͨ���ڱ���̵߳�

bool CacheManager::updateCacheGroupStateList(vector<GroupStruct> groupStructVec){
	//cout<<"put groups connect status into queue"<<endl;
	printf("LOOKUPMANAGER_THREAD : put groups connect status into queue\n");
	if(this->isConnect == false)return false;
	else {
		this->group_struct_queue.push(groupStructVec);
		return true;
	}
	return true;
}
//�������
//��
bool CacheManager::isConnected(){return this->isConnect;}//�Ƿ����������ϵ�
void CacheManager::customfamilyMap(){
	shared_ptr<map<string,vector<string>>> mapPtr  = this->family_name_queue.try_pop();
	if(!mapPtr)return ;//û��ȡ��

	printf("CACHE_MANAGER_THREAD : get family and names from queue\n");
	map<string,vector<string>>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){
			//�Ž�family�ļ�������ȥ
			//����������family��set ��group �� set 
			this->flushCacheAndItNameList(it->first,it->second);
	}

}//���Ķ��������familyMap����
void CacheManager::customGroupStateMap(){
	//
	shared_ptr<vector<GroupStruct>> mapPtr  = this->group_struct_queue.try_pop();
	if(!mapPtr)return ;//û��ȡ����Ҫ���ĵ�groupVec


	printf("CACHE_MANAGER_THREAD : get group status from queue\n");
	vector<GroupStruct>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){

		//��֮ǰҪ���ж��£����group���ڲ��ڻ��������ˣ���Ϊ�п��ܱ�ɾ��
		char* keyTemp=new char(strlen(it->getName().data())+1);
		if(this->cacheConnect.isContainKey(strcpy(keyTemp,it->getName().data()))){
			this->flushCacheGroupState(*it);
		}
		delete keyTemp;

	}

} // ���Ķ��������groupStructVec����
void CacheManager::setIpAndPort(string ip_,string port_){
	this->cacheConnect.setIpAndPort(ip_, std::atoi(port_.data()));
}//��������
void CacheManager::run(){
	while (true)
	{
		//cout<<"lookupManager thread work"<<endl;
		printf("CACHE_MANAGER_THREAD : cache manager thread work\n");
		customfamilyMap();//�����¶���
		customGroupStateMap();
		customGroupFeedBack();
		if(this->sleep_time>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time));
		}
		printf("CACHE_MANAGER_THREAD : cache manager is ready for next\n");
	}
}//�������к���

bool CacheManager::reconnect(){
	if(this->cacheConnect.isConnected()==false){
		return this->isConnect=this->cacheConnect.reconnect();
	}else
	{
		return this->isConnect = true;
		
	}
}//��������




vector<GroupStruct> CacheManager::getGroupInCache_pri(string des){
		//��ȡ group��list,��ȡstr
		void* groupP=NULL;
		int arg_nums=2;
		const char* args[]={"smembers","group",};
		bool opt = this->cacheConnect.setCommndWithArgs(arg_nums,args,des,groupP);//���ص�������
			
		if (opt == false) {
			printf("LOOKUP_MANAGER_THREAD: can not get group list from cache\n");
		
		}
		else {
			printf("LOOKUP_MANAGER_THREAD: get group list from cache\n");
		
		}
		vector<string>& g_strs = *(vector<string>*)groupP;
		vector<GroupStruct> groupV;
		for(int i=0;i<g_strs.size();i++){
			GroupStruct g;
			//ȡstr
			void* str=NULL;
			char* group_key_ = new char(strlen(g_strs.at(i).data())+1);
			int arg_nums_1=2;
			const char* args_1[]={"get",strcpy(group_key_,g_strs.at(i).data())};
			this->cacheConnect.setCommndWithArgs(arg_nums_1,args_1,GET_STR,str);//str�Ѿ�����һ��str��
			g.DeSerialize((char*)str);
			delete str, group_key_, args_1;
			groupV.push_back(g);
			delete[] args;
	}
		delete groupP;
		return groupV; //����

	


}//ȡ����
//���û����࣬�Ž�ȥ
bool CacheManager::flushCacheAndItNameList(const string& key ,vector<string>& list){
	//����set��Ҫadd 
	void* groupP;
	int arg_nums=list.size()+2;
	char* key_=new char(strlen( key.data())+1);

	const char** args=new const char*[arg_nums];
	args[0]="sadd";
	args[1]=strcpy(key_,key.data());
	for(int i=0;i<list.size();i++){
		args[2+i]=list.at(i).data();
		
	}
	bool opt= this->cacheConnect.setCommndWithArgs(arg_nums,args,ADD_VALUE_TO_SET,groupP);

	char* key__=new char(strlen( key.data())+1);
	const char* args_[]={"sadd","family",strcpy(key__,key.data())};//�ŵ�family set����ȥ
	int arg_nums_=3;
	void* intx;
	bool opt1= this->cacheConnect.setCommndWithArgs(arg_nums_,args_,ADD_VALUE_TO_SET,intx);
	delete intx, key_, key_, groupP;
	delete[] args_;
	delete[] args;
	if (opt == false) {
		printf("CACHE_MANAGER_THREAD:cache error:cannot flush some family in cache \n");
	}
	if (opt1 == false) {
		printf("CACHE_MANAGER_THREAD:cache error:cannot flush  family list in cache \n");
	}
	return opt==opt1==true;

	printf("CACHE_MANAGER_THREAD : send newest family and names map to cache\n");
	return true;
}
bool CacheManager::flushCacheGroupState(GroupStruct gst){
	//�ж���key ��set str
	char* key_=new char(strlen( gst.getName().data())+1);
	string s(gst.Serialize());
	char* objStr=new char(strlen(s.data())+1);
	const char* args_[]={"set",strcpy(key_,gst.getName().data()),strcpy(objStr,s.data())};//�ŵ�family set����ȥ
	int arg_num=3;
	void* res;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	if (opt == false) {
		printf("CACHE_MANAGER_THREAD:cache error:cannot flush some group state  in cache \n");
	}
	delete res, key_, objStr;
	delete[] args_;
	gst.freeStruct();

	return opt;



}
bool CacheManager::flushCacheGroupFeedBackList(GroupfeedbackCustomStruct gfd){
	//�ж���key ��set str
	string gfd_name_key=gfd.groupName+"_gfd";//group��fdĬ�ϼӸ���׺
	char* key_=new char(strlen(gfd_name_key.data())+1);
	string s(gfd.Serialize());
	char* objStr=new char(strlen(s.data())+1);
	const char* args_[]={"rpush",strcpy(key_,gfd_name_key.data()),strcpy(objStr,s.data())};//�ŵ�groupfdlist set����ȥ
	int arg_num=3;
	void* res;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	delete res, key_, objStr;
	delete[] args_;
	if (opt == false) {
		printf("CACHE_MANAGER_THREAD:cache error:cannot add some group 's feedback  in cache \n");
	}
	gfd.freeStruct();
	return opt;



}//ˢ��Զ�˻��������groupFeedback
bool CacheManager::updateGroupFeedBack(GroupfeedbackCustomStruct gfd){
	printf("FEEDBACK_CUSTOMER : put group feed back into queue\n");
	//cout<<"put group feed back into queue"<<endl;
	if(this->isConnect == false)return false;
	else {
		this->group_feedback_queue.push(gfd);
		return true;
	}

}//���»���feedbacklist,ֻ������
void CacheManager::customGroupFeedBack(){
	
	shared_ptr<GroupfeedbackCustomStruct> mapPtr  = this->group_feedback_queue.try_pop();
	
	if(!mapPtr)return ;//û��ȡ����Ҫ���ĵ�groupVec
	printf("CACHE_MANAGER_THREAD : get one feedback from queue\n");

	this->flushCacheGroupFeedBackList(*mapPtr);

}
	


 //���Ķ��������groupFeedBack