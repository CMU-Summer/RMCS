#ifndef INITMANAGER_H
#define INITMANAGER_H

#include "queue_safe.h"
#include "LookUpManager.h"
#include "CacheManager.h"
#include "ConfigManager.h"
#include "CommandCustomer.h"
#include "FeedBackCustomer.h"
#include "FeedBackManager.h"

#include <src/lookup.hpp>
#include <thread>
#include "common.h"
#define NO_QFORKIMPL //��һ�б���Ӳ�������ʹ��
#include "src/Win32_Interop/win32fixes.h"
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
		//hebi::Lookup lookup1;
		//��������
		int sleepTime;
		int fd_hz;
		printf("--------init configManager--------\n");
		ConfigManager cfgManager("resource/config.xml");
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
		//�������ݿ����
		printf("---------init databaseManager ---------\n");
		DataBaseManager db;
		printf("---------init lookupManager ---------\n");
		//����lookup�߳�
		LookUpManager lkManager(cacheManger,gfd_queue,lookup,cfgManager,cacheGroupMap_,fixedGroupMap_,sleepTime,fd_hz);
		printf("---------init CommandCustomer ---------\n");
		//��������������
		CommandCustomer cmdCusrom(cmd_queue,cfgManager,cacheGroupMap_,fixedGroupMap_, sleepTime);
		printf("---------init FeedBackCustomer ---------\n");
		//���л���������
		FeedBackCustomer fdCustomer(gfd_queue,cacheManger,db, sleepTime);
		//``````````````````````````````````
		printf("---------run cache thread ---------\n");

		cacheManger.initCacheManager();
		printf("---------run lookupManager thread ---------\n");
		lkManager.init();
		printf("---------run commandCustomer thread ---------\n");
		cmdCusrom.init();
		printf("---------run feedbackCustomer thread ---------\n");
		fdCustomer.init();
		printf("---------all working!---------\n");
		cacheManger.join();
		printf("---------cacheManger join!---------\n");
		lkManager.join();
		printf("---------lkManager join!---------\n");
		cmdCusrom.join();
		printf("---------cmdCusrom join!---------\n");
		fdCustomer.join();
		printf("---------fdCustomer join!---------\n");
		int i;
		cin >> i;
		return 0;
		
	}
	~InitManager() {
	
	
	
	}

private:

};




#endif