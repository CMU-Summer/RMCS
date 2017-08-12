#ifndef	FEEDBACKCUSTOMER_H
#define FEEDBACKCUSTOMER_H
#include "queue_safe.h"
#include "FeedBackManager.h"
#include "CacheManager.h"
#include "DataBaseManager.h"
#include "LookUpManager.h"
#include "CThread.h"
#define  LOCAL 1
#define  WEB 2
class FeedBackCustomer :public CThread{
	//������������feedback��������
	//��һ���߳���
	//�ɶ���feedback�Ѿ���feedbackmanager���涨�����
	//�ɶ���feedback�����Ѿ���һ�������group��Ϣ�����õ����޷�ȷ�����Ǹ�group��feedback
	//���û���ӿڣ�д�뵽��������������ȥ
private: 
	queue_safe<GroupfeedbackCustomStruct>& gfd_quee; 
	CacheManager& cacheManager;
	DataBaseManager& dbManager;
	int sleeptime;
public:
	FeedBackCustomer(queue_safe<GroupfeedbackCustomStruct>& q,CacheManager& cacheManager_,DataBaseManager dbManager_,int sleeptime_=LookUpManager::DEAULT_SLEEP_TIME):
	cacheManager(cacheManager_),
	dbManager(dbManager_),
	sleeptime(sleeptime_),
	gfd_quee(q)
	{


	}
	  void run() override; //���غ���
	  void init(); //��ʼ��

	  bool insertCache(GroupfeedbackCustomStruct& gfd);//�Ž���������
	  bool insertDb(int type,GroupfeedbackCustomStruct& gfd);//�Ž����ݿ�����
	  bool customGfd();//����
	  void printGroupFeedBack(GroupfeedbackCustomStruct& gfd){
		  cout<<"-------------"<<endl;
		  cout<<"feedback from:"+gfd.groupName<<endl;
		  cout<<"size:"<<gfd.velocitysVec.size()<<endl;
		  cout<<"+++"<<endl;
		  for(int i=0;i<gfd.moduleFeedBackVec.size();i++){
			  cout<<"module["<<i<<"]'s:��"<<gfd.moduleFeedBackVec.at(i).led_field.led_R<<","<<gfd.moduleFeedBackVec.at(i).led_field.led_G<<","<<gfd.moduleFeedBackVec.at(i).led_field.led_B<<")"<<endl;
		  }
		  cout<<"+++"<<endl;
	  }
};



#endif