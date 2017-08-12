#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "CThread.h"

#include <string>
#include "queue_safe.h"
#include "LookUpManager.h"
#include <map>
#include <vector>
#include "hiredis/hiredis.h"
#include <mutex>
#include <condition_variable>
#define PUT_LIST "put_list"
#define GET_LIST "get_list"
#define GET_STR  "get_str"
#define ADD_VALUE_TO_SET  "add_value_to_set"
#define DELETE_VALUE_TO_SET "delete_value_to_set"
#define SET_STR "set_str"
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
	queue_safe<map<string,vector<string>>> family_name_queue;
	queue_safe<vector<GroupStruct>> group_struct_queue;//��Ҫ����ˢ
	queue_safe<GroupfeedbackCustomStruct> group_feedback_queue;
	mutable std::mutex mut;   
	bool canUseRedis;

	std::condition_variable data_cond;  
public:
	CacheManager(string ip_,int port_,int sleep_time=LookUpManager::DEAULT_SLEEP_TIME);
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
	bool isContainKey(char*  key);
	bool deleteKey();//ɾ����
	bool pushNewFamily_list_ToCache_mtx(const string& key ,vector<string>& list);//�Ѷ����Ž���������
	bool updateGroupFeedBack(GroupfeedbackCustomStruct gfd);//���»���feedbacklist,ֻ������
	 
	int sleep_time;
private:
	vector<GroupStruct> getGroupInCache_pri(string des);//��ȡcache�е�list
	bool flushCacheGroupState(GroupStruct gst);//ˢ�»��������groupstrut
	bool flushCacheGroupFeedBackList(GroupfeedbackCustomStruct gfd);//ˢ��Զ�˻��������groupFeedback
};
class CacheConnection{
	/*
	��������ײ�Ի��������
	init���ӻ���
	get/set ����
	
	*/
private:
	string ip;
	int port;
	redisContext* rdc;
public:
	CacheConnection(string ip_,int port_):ip(ip_),port(port_),rdc(NULL){}
	~CacheConnection();



	bool connect(){
		const string ip_=this->ip;
		rdc= redisConnect(this->ip.data(),this->port);
		return rdc==NULL?false:true;

	}//����
	bool reconnect(){
		if(this->isConnected()){
			return true;

		}else
		{
			const string ip_=this->ip;
			rdc= redisConnect(this->ip.data(),this->port);
			return rdc==NULL?false:true;
		}


	}//��������
	bool disconnect(){
		freeContext();
		rdc=NULL;

	}//�Ͽ�
	bool isConnected(){
		
		return rdc==NULL?false:true;


	}//�Ƿ�����
	bool init(){
		return this->connect();

	}// ����
	void setIpAndPort(string ip_,int port_){
		this->ip=ip_;
		this->port=port_;
	}//��������

	void freeContext(){
		redisFree(this->rdc);
	}//���context
	bool isContainKey(char*  key){
		void* intP;
		int arg_nums=2;
		char* args[]={"exists",key};
		this->setCommndWithArgs(arg_nums,args,"keyExist",intP);
		return (*(int*)intP == 0)?false:true;


	}//�Ƿ����Key
	bool deleteKey(char*  key){
		void* intP;
		int arg_nums=2;
		char* args[]={"del",key};
		this->setCommndWithArgs(arg_nums,args,"keyDelete",intP);
		return (*(int*)intP == 0)?false:true;

	}//ɾ����
	bool setCommndWithArgs(int agr_nums,char** args,string des,void* &res){
		if(this->isConnected() == false) return false;
		 redisCommandArgv(this->rdc,agr_nums,args,NULL);
		 redisReply *reply;
		 redisGetReply(rdc,&reply);
		 switch (reply->type)
		 {
		 case REDIS_REPLY_ARRAY:{
			 //�����飬��ô����Ҫ������
			 vector<string>* vec=new vector<string>();

			 for(int i=0;i<reply->elements;i++){
				 vec->push_back((*(reply->element))[i].str);
			 }
			 res=(void*)vec;
			 return true;
		
		  }
		 case REDIS_REPLY_INTEGER:{
			 LONGLONG* a= new LONGLONG();
			  (*a)=reply->integer;
			   res=(void*)a;
			   return true;
								}
		 case REDIS_REPLY_STRING:{
			 string* a= new string(reply->str);
			 res=(void*)a;
			 return true;

								}
		 case REDIS_REPLY_NIL:{
			 cout<<"error:get null from redis"<<endl;
			  return false;
								}
		 case REDIS_REPLY_ERROR:{
			  cout<<"error:executed error"<<endl;
			  return false;
								}
		 case REDIS_REPLY_STATUS:{
			 //ĳ�������ģ�����DEL SET �Ŷ����Ĳ���,ɾ���Ͳ���Ͳ�����������
			 if(des == GET_LIST){
				 //��ȡ�б�,���ᴥ��

			 }else if(des == PUT_LIST ){
				 //���б�Ĳ���
				 return  reply->integer<=0?false:true;

			 }else if(des == ADD_VALUE_TO_SET){
				 //���������set
				  return  reply->integer<=0?false:true;

			 }else if(des == DELETE_VALUE_TO_SET){
				 //ɾ��ĳ��set�ĳ�Ա
				 return  reply->integer<=0?false:true;
			 }else if(des == GET_STR){
				 //��ȡ�ַ���(���л�����),���ᵽ����


			 }else if(des == SET_STR){
				 //�����ַ��������л�����
				 return strcmp(reply->str,"OK")==0;
			 }else {
				 //��ֵĲ���
				 return false;
			 }


			  cout<<reply->str<<endl;
			  return true;
								}
		 default :
			 cout<<"unexcepted error"<<endl;
			 return false;
			 break;
		 }


	}//��������


};

#endif // !