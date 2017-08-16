#ifndef COMMANDCUSTOMER_H
#define COMMANDCUSTOMER_H
#include "CThread.h"
#include <string>
#include "queue_safe.h"
//#include "JsonObjectBase.h"
#include "common.h"


#include "ConfigManager.h"
#include "LookUpManager.h"
#include "lookup.hpp"
using namespace  std;
class CommandCustomer:public CThread
{
	//����һ���߳��࣬
	//��ͣ������commandQueue�����е�����
	//commandQueue�����У�ÿһ��Ԫ�أ���ָ����Ҫ���������group�������е�module
	//�Լ�����ĸ�ʽ
	//Ԫ�ص�������Ҫ�����ﶨ��
	//feedback�ᱻfeedbackcustomer����

public:
	CommandCustomer(queue_safe<CommandGroupStruct>& command_struct_queue_,ConfigManager& cfg,Lookup& lm,int sleepTime_=DEFAULT_SLEEP_TIME):command_struct_queue(command_struct_queue_),cfgManager(cfg), lookup(lm),sleeptime(sleepTime_){


	}
	~CommandCustomer() {
	
	
	
	
	}
	CommandGroupStruct getFakeLedCommand(vector<string> fs_, vector<string> ns_, string groupName, int size);
	void run() override; //��дrun
	void init(){
		this->start();


	}//
	hebi::GroupCommand getGroupCommandStrut(CommandStruct ct){




	}
	hebi::Command getCommandStrut(CommandStruct ct){




	}
	bool customCommand(){
		
		printf("COMMAND_CUSTOMER : send led command\n");
		//shared_ptr<CommandGroupStruct> mapPtr  = this->command_struct_queue.wait_and_pop();
		CommandGroupStruct* mapPtr = new CommandGroupStruct();
	    *mapPtr = getFakeLedCommand({"SEA-Snake","Spare"}, {"SA011","SA035"},"testGroup",2);
		
		if(!mapPtr)return false;//û��ȡ����Ҫ���ĵ�gfd
		unique_ptr<hebi::Group> g= this->lookup.getGroupFromNames(mapPtr->names,mapPtr->familys);
		printf("COMMAND_CUSTOMER : g is not null:%d\n",g!=NULL);
		if (!g){
			printf("COMMAND_CUSTOMER : g is  null\n");
			return false;
		}
		//�������group��command
		hebi::GroupCommand command(g->size());
		switch (mapPtr->cmd)
		{
		case LED_CMD: {
			for (int i = 0; i < g->size();i++) {
				if (mapPtr->fd.size() <= i)break;
				hebi::Color color_(mapPtr->fd.at(i).led_fied.led_R, mapPtr->fd.at(i).led_fied.led_G, mapPtr->fd.at(i).led_fied.led_B);
				command[i].led().setOverrideColor(color_);
			
			}
			break;
		
		}
		case POSITION_CMD: {
			for (int i = 0; i < g->size(); i++) {
				if (mapPtr->fd.size() <= i)break;
				command[i].actuator().position().set(mapPtr->fd.at(i).actuator.position);

			}
			break;
		
		}
		case TURQUE_CMD:{
			for (int i = 0; i < g->size(); i++) {
				if (mapPtr->fd.size() <= i)break;
				command[i].actuator().torque().set(mapPtr->fd.at(i).actuator.torque);

			}
			break;
		
		}
		case VELOCITY_CMD: {
			for (int i = 0; i < g->size(); i++) {
				if (mapPtr->fd.size() <= i)break;
				command[i].actuator().velocity().set(mapPtr->fd.at(i).actuator.velocity);

			}
			break;

		}
		default:
			//do nothing
			break;
		}

		int timeout=100;
		if (g->sendCommandWithAcknowledgement(command, timeout))
		{
			
			printf("COMMAND_CUSTOMER : already sent!\n");
		}
		else
		{
		
			printf("COMMAND_CUSTOMER : send time out\n");
		}
		return true;
	}
private:
	queue_safe<CommandGroupStruct>& command_struct_queue;
	ConfigManager& cfgManager;
	Lookup& lookup;
	int sleeptime;
};

void CommandCustomer::run(){
	while (true)
	{
		printf("COMMAND_CUSTOMER : command customer working!!!!!\n");
		this->customCommand();
		if(this->sleeptime>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleeptime));
		}
		printf("COMMAND_CUSTOMER : command customer is ready for next!!!!!\n");
	}

}
CommandGroupStruct CommandCustomer::getFakeLedCommand(vector<string> fs_,vector<string> ns_,string groupName,int size) {
	Led_field l(181, 181, 181);
	Actuator_field a(0,0,0,0);
	vector<CommandStruct> v;
	for (int i = 0; i < size;i++) {
		CommandStruct c(a, l);
		v.push_back(c);
	}

	CommandGroupStruct* cg = new CommandGroupStruct(v,fs_,ns_);
	cg->groupName.assign(groupName);
	cg->cmd = LED_CMD;
	return *cg;

}

#endif // !
