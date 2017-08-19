#include "CacheManager.h"
#include <mutex>
#include <condition_variable>
#include "ConfigManager.h"
CacheManager::CacheManager(ConfigManager& cofManager,int sleep_time_ ):cacheConnect(),canUseRedis(true),sleep_time(sleep_time_),cfgManager(cofManager){




}
CacheManager::~CacheManager(){}
void CacheManager::initCacheManager(){
	vector<RedisCofig> cogVec=this->cfgManager.getRedisList();
	if (cogVec.size() <= 0) {
		printf("CACHE_MANAGER_THREAD: no redis in config:%d\n");
		this->isConnect = false;
	}
	else {
		this->cacheConnect.setIpAndPort(cogVec.at(0).ip,cogVec.at(0).port);
		this->isConnect = this->cacheConnect.init();
		printf("CACHE_MANAGER_THREAD: redis connect status:%d\n", this->isConnect);
	}

	this->start();//���߳�


}//���ӣ����̵߳ȵȲ���

vector<GroupStruct> CacheManager::getGroupInCache(){
	//����߳�����
	//cout << "get user groups from cache" << endl;
	printf("LOOKUPMANAGER_THREAD: get user groups from cache\n");
	return this->getGroupInCache_pri(GET_LIST);

}//��ȡcache�е�list,��Ҫ��

bool CacheManager::updateCacheFamilyAndItsNames(map<string,vector<string>> familyMap){
	//�Ѿ��ڶ��к����д�������

	printf("LOOKUPMANAGER_THREAD: put family and its names into queue\n");
	if(this->cacheConnect.isConnected() == false)return false;
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
	printf("LOOKUPMANAGER_THREAD: put groups connect status into queue\n");
	if(this->cacheConnect.isConnected() == false)return false;
	else {
		this->group_struct_queue.push(groupStructVec);
		return true;
	}
	return true;
}
//�������
//��
bool CacheManager::isConnected(){return this->cacheConnect.isConnected();}//�Ƿ����������ϵ�
void CacheManager::customfamilyMap(){
	shared_ptr<map<string,vector<string>>> mapPtr  = this->family_name_queue.try_pop();
	if(!mapPtr)return ;//û��ȡ��

	printf("CACHE_MANAGER_THREAD: get family and names from queue\n");
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


	printf("CACHE_MANAGER_THREAD: get group status from queue\n");
	vector<GroupStruct>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){

		//��֮ǰҪ���ж��£����group���ڲ��ڻ��������ˣ���Ϊ�п��ܱ�ɾ��
		char* keyTemp=new char[strlen(it->getName().data())+1];
		if(this->cacheConnect.isContainKey(strncpy(keyTemp,it->getName().data(),it->getName().length()+1))){
			this->flushCacheGroupState(*it);
		}
		delete[] keyTemp;

	}

} // ���Ķ��������groupStructVec����
void CacheManager::setIpAndPort(string ip_,string port_){
	this->cacheConnect.setIpAndPort(ip_, std::atoi(port_.data()));
}//��������
void CacheManager::run(){
	while (true)
	{
		//cout<<"lookupManager thread work"<<endl;
		printf("CACHE_MANAGER_THREAD: cache manager thread work\n");
		customfamilyMap();//�����¶���
		customGroupStateMap();
		customGroupFeedBack();
		if(this->sleep_time>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time));
		}
		printf("CACHE_MANAGER_THREAD: cache manager is ready for next\n");
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
			printf("LOOKUPMANAGER_THREAD: can not get group list from cache\n");
		
		}
		else {
			printf("LOOKUPMANAGER_THREAD: get group list from cache\n");
		
		}
		vector<string>& g_strs = *(vector<string>*)groupP;
		vector<GroupStruct> groupV;
		for(int i=0;i<g_strs.size();i++){
			GroupStruct g;
			//ȡstr
			void* str=NULL;
			char* group_key_ = new char[strlen(g_strs.at(i).data())+1];
			int arg_nums_1=2;
			const char* args_1[]={"get",strncpy(group_key_,g_strs.at(i).data(),g_strs.at(i).length()+1)};
			this->cacheConnect.setCommndWithArgs(arg_nums_1,args_1,GET_STR,str);//str�Ѿ�����һ��str��
			if (str) {
				string* a = (string*)str;
				g.DeSerialize(a->data());
				delete str;
				delete[] group_key_;
				
				groupV.push_back(g);
			
			}
			
			
	}
		delete groupP;
		printf("LOOKUPMANAGER_THREAD: delete groupP\n");
		return groupV; //����

	


}//ȡ����
//���û����࣬�Ž�ȥ
bool CacheManager::flushCacheAndItNameList(const string& key ,vector<string>& list){
	//����set��Ҫadd 
	void* groupP;
	int arg_nums=list.size()+2;
	char* key_=new char[strlen( key.data())+1];

	const char** args=new const char*[arg_nums];
	args[0]="sadd";
	args[1]=strncpy(key_,key.data(),key.length()+1);
	for(int i=0;i<list.size();i++){
		args[2+i]=list.at(i).data();
		
	}
	bool opt= this->cacheConnect.setCommndWithArgs(arg_nums,args,ADD_VALUE_TO_SET,groupP);

	char* key__=new char[strlen( key.data())+1];
	const char* args_[]={"sadd","family",strncpy(key__,key.data(),key.length()+1)};//�ŵ�family set����ȥ
	int arg_nums_=3;
	void* intx;
	bool opt1= this->cacheConnect.setCommndWithArgs(arg_nums_,args_,ADD_VALUE_TO_SET,intx);
	delete intx, groupP;
	delete[] key_, key__;
	/*
	for (int i = 0; i<list.size(); i++) {
		delete[] args[2 + i];
	}*/
	if (opt == false) {
		printf("CACHE_MANAGER_THREAD: cache error:cannot flush some family in cache \n");
	}
	if (opt1 == false) {
		printf("CACHE_MANAGER_THREAD: cache error:cannot flush  family list in cache \n");
	}
	return opt==opt1==true;

	printf("CACHE_MANAGER_THREAD: send newest family and names map to cache\n");
	return true;
}
bool CacheManager::flushCacheGroupState(GroupStruct gst){
	//�ж���key ��set str
	 char* key_=new char[strlen( gst.getName().data())+1];
	 gst.SetPropertys();//����һ��
	 string s= gst.Serialize();

	char* objStr=new char[strlen(s.data())+1];
	const char* args_[]={"set",strncpy(key_,gst.getName().data(),gst.getName().length()+1),strncpy(objStr,s.data(),s.length()+1)};//�ŵ�family set����ȥ
	int arg_num=3;
	void* res=NULL;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	if (opt == false) {
		printf("CACHE_MANAGER_THREAD: cache error:cannot flush some group state  in cache \n");
	}
	if (res != NULL)delete res;
	delete[] key_, objStr;
	gst.freeStruct();

	return opt;



}
bool CacheManager::flushCacheGroupFeedBackList(GroupfeedbackCustomStruct gfd){
	//�ж���key ��set str
	string gfd_name_key=gfd.groupName+"_gfd";//group��fdĬ�ϼӸ���׺
	char* key_=new char[strlen(gfd_name_key.data())+1];
	gfd.SetPropertys();
	string s(gfd.Serialize());
	char* objStr=new char[strlen(s.data())+1];
	const char* args_[]={"rpush",strncpy(key_,gfd_name_key.data(),gfd_name_key.length()+1),strncpy(objStr,s.data(),s.length()+1)};//�ŵ�groupfdlist set����ȥ
	int arg_num=3;
	void* res=NULL;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	if(res!=NULL)delete res,
	delete[] key_, objStr;
	if (opt == false) {
		printf("CACHE_MANAGER_THREAD: cache error:cannot add some group 's feedback  in cache \n");
	}
	gfd.freeStruct();
	return opt;



}//ˢ��Զ�˻��������groupFeedback
bool CacheManager::updateGroupFeedBack(GroupfeedbackCustomStruct gfd){
	printf("FDBACKCUSTOMER__THREAD: put group feed back into queue\n");
	//cout<<"put group feed back into queue"<<endl;
	if(this->cacheConnect.isConnected() == false)return false;
	else {
		this->group_feedback_queue.push(gfd);
		return true;
	}

}//���»���feedbacklist,ֻ������
void CacheManager::customGroupFeedBack(){
	
	shared_ptr<GroupfeedbackCustomStruct> mapPtr  = this->group_feedback_queue.try_pop();
	
	if(!mapPtr)return ;//û��ȡ����Ҫ���ĵ�groupVec
	printf("CACHE_MANAGER_THREAD: get one feedback from queue\n");

	this->flushCacheGroupFeedBackList(*mapPtr);

}
	


 //���Ķ��������groupFeedBack