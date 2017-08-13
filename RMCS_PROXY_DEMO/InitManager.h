#ifndef INITMANAGER_H
#define INITMANAGER_H
#include "LookUpManager.h"
#include "CacheManager.h"
#include "ConfigManager.h"
#include "CommandCustomer.h"
#include "FeedBackCustomer.h"
#include "FeedBackManager.h"
#include "queue_safe.h"
#include "src/lookup.hpp"
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
		queue_safe<GroupfeedbackCustomStruct> gfd_queue;
		queue_safe<GroupStruct> gs_queue;
		queue_safe<CommandStruct> cmd_queue;
		hebi::Lookup lookup;
		//��������
		ConfigManager cfgManager("resource/config.xml");
		//���л������
		CacheManager cacheManger(cfgManager.getRedisList().at(0).ip,cfgManager.getRedisList.at(0).port);
		//�������ݿ����
		DataBaseManager db;
		//����lookup�߳�
		LookUpManager lkManager(cacheManger,gfd_queue,lookup,cfgManager);
		//��������������
		CommandCustomer cmdCusrom(cmd_queue,cfgManager,lkManager);
		//���л���������
		FeedBackCustomer fdCustomer(gfd_queue,cacheManger,db);
		//``````````````````````````````````
		cacheManger.initCacheManager();
		lkManager.init();
		cmdCusrom.init();
		fdCustomer.init();
		cout<<"all run !!!"<<endl;



		
	}
	~InitManager() {
	
	
	
	}

private:

};




#endif