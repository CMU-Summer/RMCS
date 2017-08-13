#ifndef INITMANAGER_H
#define INITMANAGER_H

#include "queue_safe.h"
#include "LookUpManager.h"
#include "CacheManager.h"
#include "ConfigManager.h"
#include "CommandCustomer.h"
#include "FeedBackCustomer.h"
#include "FeedBackManager.h"

#include "src/lookup.hpp"
#include <thread>
#include "common.h"
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
		cout << "----init queue!----" << endl;
	
		queue_safe<GroupfeedbackCustomStruct> gfd_queue;
		queue_safe<GroupStruct> gs_queue;
		queue_safe<CommandStruct> cmd_queue;
		
		hebi::Lookup lookup;
		//��������
		cout << "----loader config!" << endl;
		ConfigManager cfgManager("resource/config.xml");
		//���л������
		cout << " config loaded ! init cacheManager" << endl;
		CacheManager cacheManger(cfgManager.getRedisList().at(0).ip,cfgManager.getRedisList().at(0).port);
		//�������ݿ����
		cout << " ----cacheManager inited ! init databaseManager" << endl;
		DataBaseManager db;
		cout << " ----databaseManager inited ! init lookupManager" << endl;
		//����lookup�߳�
		LookUpManager lkManager(cacheManger,gfd_queue,lookup,cfgManager);
		cout << " ----lookupManager inited ! init CommandCustomer" << endl;
		//��������������
		CommandCustomer cmdCusrom(cmd_queue,cfgManager,lkManager);
		cout << " ----CommandCustomer inited ! init FeedBackCustomer" << endl;
		//���л���������
		FeedBackCustomer fdCustomer(gfd_queue,cacheManger,db);
		//``````````````````````````````````
		cacheManger.initCacheManager();
		lkManager.init();
		cmdCusrom.init();
		fdCustomer.init();
		cout<<"all run !!!"<<endl;
		cacheManger.join();
		lkManager.join();
		cmdCusrom.join();
		fdCustomer.join();
		return 0;
		
	}
	~InitManager() {
	
	
	
	}

private:

};




#endif