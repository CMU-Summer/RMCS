#ifndef INITMANAGER_H
#define INITMANAGER_H

#include "queue_safe.h"
#include "LookUpManager.h"
#include "CacheManager.h"
#include "ConfigManager.h"
#include "CommandCustomer.h"
#include "FeedBackCustomer.h"
#include "FeedBackManager.h"
#include "ServerApiManager.h"
#include <src/lookup.hpp>
#include <thread>
#include "common.h"
#include <chrono>
#include "ServerListenerApi.h"
class InitManager
	/*
	�������һ��ʼ��ʼ����
	��Ҫ�������ļ���ȡһϵ�в���
	���籾�����ݿ�/web���ݿ��ַ
		Ĭ����Ҫ���ӵ�group
	��ʼ������manager/customer�����߳������������������ʼ���������в����Ļ����Ͳ����ֶ�start��
	�ݶ�
	*/


{
public:



	InitManager() {
	
	
	
	}
	static int main(){
		printf("--------init queue!--------\n");
		queue_safe<GroupfeedbackCustomStruct> gfd_queue;
		queue_safe<GroupStruct> gs_queue;
		queue_safe<CommandGroupStruct> cmd_queue;
		map<string, unique_ptr<hebi::Group>> cacheGroupMap_;
		map<string, unique_ptr<hebi::Group>> fixedGroupMap_;
		printf("--------init hebi lookup--------\n");
		hebi::Lookup lookup;
		//��������
		int sleepTime;
		int fd_hz;
		printf("--------init configManager--------\n");
		ConfigManager cfgManager("resource/config.xml");
		vector<ServerConfig> sCof = cfgManager.getServersConfig();
		vector<RedisCofig>  rCof = cfgManager.getRedisList();
		vector<DBconfig>  dbCof=cfgManager.getDbConfig();
		CacheManager* cacheMangerPtr=NULL;
		LookUpManager* lkManagerPtr = NULL;
		CommandCustomer* cmdCusromPtr = NULL;
		FeedBackCustomer* fdCustomerPtr = NULL;

		if (cfgManager.getSleepTime() <= DEFAULT_SLEEP_TIME) {
			sleepTime = DEFAULT_SLEEP_TIME;

		}else {
			sleepTime = cfgManager.getSleepTime();
		
		}
		if (cfgManager.getFeedbackFrequency() <= DEFAULT_FD_FREQUENCY) {
			fd_hz = DEFAULT_FD_FREQUENCY;

		}
		else {
			fd_hz = cfgManager.getFeedbackFrequency();

		}
		//���л������
		printf("---------init cacheManager--------\n");
		CacheManager cacheManger(cfgManager, sleepTime);
		//��ʼ�����ݿ����
		printf("---------init databaseManager ---------\n");
		DataBaseManager db;
		printf("---------init lookupManager ---------\n");
		//��ʼ��lookup�߳�
		LookUpManager lkManager(cacheManger,gfd_queue,lookup,cfgManager,cacheGroupMap_,fixedGroupMap_,sleepTime,fd_hz);
		printf("---------init CommandCustomer ---------\n");
		//��ʼ������������
		CommandCustomer cmdCusrom(cmd_queue,cfgManager,cacheGroupMap_,fixedGroupMap_, sleepTime);
		printf("---------init FeedBackCustomer ---------\n");
		//��ʼ������������
		FeedBackCustomer fdCustomer(gfd_queue,cacheManger,db, sleepTime);
		printf("---------init ServerListener ---------\n");
		//��ʼ��Զ�̷���������
		ServerListener serverListener(cmd_queue, cacheManger, sleepTime);
// 		ServerApiManager* sMptr=NULL;
// 		if (sCof.size() > 0) {
// 			 
// 			sMptr =new  ServerApiManager(sCof.at(0).ip, sCof.at(0).port, cmd_queue, BUF_SIZE, sleepTime);
// 		}
// 		else {
// 			printf("---------fail to init ServerApiManager ---------\n");
// 		
// 		}
		//``````````````````````````````````
		printf("---------run cache thread ---------\n");

		cacheManger.initCacheManager();
		printf("---------run lookupManager thread ---------\n");
		lkManager.init();
		printf("---------run commandCustomer thread ---------\n");
		cmdCusrom.init();
		printf("---------run feedbackCustomer thread ---------\n");
		fdCustomer.init();
		printf("---------run feedbackCustomer thread ---------\n");
		serverListener.init();
		printf("--------- working!---------\n");
		cacheManger.join();
		printf("---------cacheManger join!---------\n");
		lkManager.join();
		printf("---------lkManager join!---------\n");
		cmdCusrom.join();
		printf("---------cmdCusrom join!---------\n");
		fdCustomer.join();
		printf("---------fdCustomer join!---------\n");
		//sMptr->join();
		printf("---------serverListener join!---------\n");
		int i;
		cin >> i;
		return 0;
		
	}
	~InitManager() {
	
	
	
	}

private:

};




#endif