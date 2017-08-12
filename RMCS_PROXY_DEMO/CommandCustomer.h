#ifndef COMMANDCUSTOMER_H
#define COMMANDCUSTOMER_H
#include "CThread.h"
#include <string>
#include "LookUpManager.h"
#include "FeedBackManager.h"
#include "ConfigManager.h"
#include "JsonObjectBase.h"
class CommandStruct:public CJsonObjectBase{
public:
	string cmd;
	string groupName;
	vector<string> familys;
	vector<string> names;
	vector<float> vel;
	vector<float> t;
	vector<float> p;
	vector<float> vol;
	FeedbackCustomStruct fd;
	CommandStruct(FeedbackCustomStruct fd_):fd(fd_){

	}
	virtual void SetPropertys(){
		this->SetProperty("cmd",asString,&cmd);
		this->SetProperty("groupName",asString,&groupName);
		this->SetProperty("familys",asVectorArray,&familys);
		this->SetProperty("names",asVectorArray,&names);
		this->SetProperty("vel",asVectorArray,&vel);
		this->SetProperty("t",asVectorArray,&t);
		this->SetProperty("p",asVectorArray,&p);
		this->SetProperty("vol",asVectorArray,&vol);
		this->SetProperty("fd",asJsonObj,&fd);

	}

};
class CommandCustomer:public CThread
{

	//����һ���߳��࣬
	//��ͣ������commandQueue�����е�����
	//commandQueue�����У�ÿһ��Ԫ�أ���ָ����Ҫ���������group�������е�module
	//�Լ�����ĸ�ʽ
	//Ԫ�ص�������Ҫ�����ﶨ��
	//feedback�ᱻfeedbackcustomer����

public:
	CommandCustomer(queue_safe<CommandStruct>& command_struct_queue_,ConfigManager& cfg,LookUpManager& lm,int sleepTime_=LookUpManager::DEAULT_SLEEP_TIME):command_struct_queue(command_struct_queue_),cfgManager(cfg),lookupManager(lm),sleeptime(sleepTime_){


	}
	~CommandCustomer();
	void run() override; //��дrun
	void init(){
		this->start();


	}//
	hebi::GroupCommand getGroupCommandStrut(CommandStruct ct){




	}
	hebi::Command getCommandStrut(CommandStruct ct){




	}
	bool customCommand(){
		cout<<"send led command"<<endl;
		shared_ptr<CommandStruct> mapPtr  = this->command_struct_queue.try_pop();
		//if(!mapPtr)return false;//û��ȡ����Ҫ���ĵ�gfd
		unique_ptr<hebi::Group> g= this->lookupManager.getLookUp().getGroupFromNames(mapPtr->names,mapPtr->familys);
		hebi::GroupCommand command(g->size());
		uint8_t r=254;
		uint8_t g1=0;
		uint8_t b=0;
		for(int i=0;i<g->size();i++){
			hebi::Color c(r,g1,b);
			command[i].led().setOverrideColor(c);
		}
		int timeout=100;
		if (g->sendCommandWithAcknowledgement(command, timeout))
		{
			std::cout << "already sent!" << std::endl;
		}
		else
		{
			std::cout << "send time out" << std::endl;
		}

	}
private:
	queue_safe<CommandStruct>& command_struct_queue;
	ConfigManager& cfgManager;
	LookUpManager& lookupManager;
	int sleeptime;
};

void CommandCustomer::run(){
	while (true)
	{

		customCommand();
		if(this->sleeptime>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleeptime));
		}

	}

}


#endif // !
