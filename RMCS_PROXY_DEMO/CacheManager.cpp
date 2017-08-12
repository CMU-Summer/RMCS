#include "CacheManager.h"
#include "include/json/json.h"
#include <mutex>
#include <condition_variable>

CacheManager::CacheManager(string ip_,int port_,int sleep_time_=LookUpManager::DEAULT_SLEEP_TIME):cacheConnect(ip_,port_),canUseRedis(true),sleep_time(sleep_time_){




}
CacheManager::~CacheManager(){}
void CacheManager::initCacheManager(){
	this->isConnect=this->cacheConnect.init();
	this->start();//���߳�


}//���ӣ����̵߳ȵȲ���

vector<GroupStruct> CacheManager::getGroupInCache(){
	//����߳�����
		std::unique_lock<std::mutex> lk(mut);  
		return this->getGroupInCache_pri(GET_LIST);

}//��ȡcache�е�list,��Ҫ��

bool CacheManager::updateCacheFamilyAndItsNames(map<string,vector<string>> familyMap){
	//�Ѿ��ڶ��к����д�������
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
	if(this->isConnect == false)return false;
	else {
		this->group_struct_queue.push(groupStructVec);
		return true;
	}

}
//�������
//��
bool CacheManager::isConnected(){return this->isConnect;}//�Ƿ����������ϵ�
void CacheManager::customfamilyMap(){
	shared_ptr<map<string,vector<string>>> mapPtr  = this->family_name_queue.try_pop();
	if(!mapPtr)return ;//û��ȡ��
	map<string,vector<string>>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){
			
			//�Ž�family�ļ�������ȥ
			//����������family��set ��group �� set 
			this->pushNewFamily_list_ToCache_mtx(it->first,it->second);

	
	}


}//���Ķ��������familyMap����
void CacheManager::customGroupStateMap(){
	//
	shared_ptr<vector<GroupStruct>> mapPtr  = this->group_struct_queue.try_pop();
	if(!mapPtr)return ;//û��ȡ����Ҫ���ĵ�groupVec
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
void CacheManager::setIpAndPort(string ip_,string port_){}//��������
void CacheManager::run(){
	while (true)
	{
		customfamilyMap();//�����¶���
		customGroupStateMap();
		if(this->sleep_time>=0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time));
		}
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
		char* args[]={"smembers","group",};
		this->cacheConnect.setCommndWithArgs(arg_nums,args,des,groupP);//���ص�������
		vector<string>& g_strs = *(vector<string>*)groupP;
		vector<GroupStruct> groupV;
		for(int i=0;i<g_strs.size();i++){
			GroupStruct g;
			//ȡstr
			void* str=NULL;
			char* group_key_ = new char(g_strs.size()+1);
			int arg_nums_1=2;
			char* args_1[]={"get",strcpy(group_key_,g_strs.at(i).data())};
			this->cacheConnect.setCommndWithArgs(arg_nums_1,args_1,GET_STR,str);//str�Ѿ�����һ��str��
			g.DeSerialize((char*)str);
			delete str;
			delete group_key_;
			delete args_1;
			groupV.push_back(g);
			delete args;
			delete groupP;
			return groupV; //����

	}



}//ȡ����
//���û����࣬�Ž�ȥ
bool CacheManager::pushNewFamily_list_ToCache_mtx(const string& key ,vector<string>& list){
	//����set��Ҫadd 
	void* groupP;
	int arg_nums=list.size()+2;
	char* key_=new char(strlen( key.data())+1);

	char** args=new char*[arg_nums];
	args[0]="sadd";
	args[1]=strcpy(key_,key.data());
	for(int i=0;i<list.size();i++){
		args[2+i]=new char(strlen(list.at(i).data())+1);
		strcpy(args[2+i],list.at(i).data());
	}
	bool opt= this->cacheConnect.setCommndWithArgs(arg_nums,args,ADD_VALUE_TO_SET,groupP);

	char* key__=new char(strlen( key.data())+1);
	char* args_[]={"sadd","family",strcpy(key__,key.data())};//�ŵ�family set����ȥ
	int arg_nums_=2;
	void* intx;
	bool opt1= this->cacheConnect.setCommndWithArgs(arg_nums_,args_,ADD_VALUE_TO_SET,intx);
	delete intx;
	delete key_;
	delete key__;
	delete args_;
	delete groupP;
	for(int i=0;i<arg_nums;i++){
		delete args[i];
	}
	delete args;

	return opt==opt1==true;

}
bool CacheManager::flushCacheGroupState(GroupStruct gst){
	//�ж���key ��set str
	char* key_=new char(strlen( gst.getName().data())+1);
	string s(gst.Serialize());
	char* objStr=new char(strlen(s.data())+1);
	char* args_[]={"set",strcpy(key_,gst.getName().data()),strcpy(objStr,s.data())};//�ŵ�family set����ȥ
	int arg_num=3;
	void* res;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	delete res;
	delete key_;
	delete objStr;
	delete args_;
	return opt;


}
bool CacheManager::flushCacheGroupFeedBackList(GroupfeedbackCustomStruct gfd){
	//�ж���key ��set str
	string gfd_name_key=gfd.groupName+"_gfd";
	char* key_=new char(strlen(gfd_name_key.data())+1);
	string s(gfd.Serialize());
	char* objStr=new char(strlen(s.data())+1);
	char* args_[]={"rpush",strcpy(key_,gfd_name_key.data()),strcpy(objStr,s.data())};//�ŵ�groupfdlist set����ȥ
	int arg_num=3;
	void* res;
	bool opt=this->cacheConnect.setCommndWithArgs(arg_num,args_,SET_STR,res);
	delete res;
	delete key_;
	delete objStr;
	delete args_;
	return opt;



}//ˢ��Զ�˻��������groupFeedback
bool CacheManager::updateGroupFeedBack(GroupfeedbackCustomStruct gfd){
	if(this->isConnect == false)return false;
	else {
		this->group_feedback_queue.push(gfd);
		return true;
	}



}//���»���feedbacklist,ֻ������
void CacheManager::customGroupFeedBack(){
	shared_ptr<vector<GroupfeedbackCustomStruct>> mapPtr  = this->group_feedback_queue.try_pop();
	if(!mapPtr)return ;//û��ȡ����Ҫ���ĵ�groupVec
	vector<GroupfeedbackCustomStruct>::iterator it;
	for(it=mapPtr->begin();it!=mapPtr->end();it++){
		//��֮ǰҪ���ж��£����group���ڲ��ڻ��������ˣ���Ϊ�п��ܱ�ɾ��
		char* keyTemp=new char(strlen(it->groupName.data())+1);
		if(this->cacheConnect.isContainKey(strcpy(keyTemp,it->groupName.data()))){
			this->flushCacheGroupFeedBackList(*it);
		}


	}


} //���Ķ��������groupFeedBack