#ifndef  COMMON_H
#define  COMMON_H
//#include "JsonObjectBase.h"
#define PUT_LIST "put_list"
#define GET_LIST "get_list"
#define GET_STR  "get_str"
#define ADD_VALUE_TO_SET  "add_value_to_set"
#define DELETE_VALUE_TO_SET "delete_value_to_set"
#define SET_STR "set_str"
#define DEFAULT_SLEEP_TIME 100
#define DEFAULT_FD_FREQUENCY 10
#define LED_CMD 0
#define POSITION_CMD 1
#define TURQUE_CMD 2
#define VELOCITY_CMD 3

#include <iostream>
#include "JsonObjectBase.h"
#include <stdlib.h>
#include <stdio.h>
#include <mutex>

#include "deps/hiredis/hiredis.h"


//#include "src/Win32_Interop/win32fixes.h"
#include <vector>
#include <map>

using namespace std;
class NameStruct :public CJsonObjectBase  {
public:
	string name;
	bool connected;
	NameStruct(string name_, bool connect_) :name(name_), connected(connect_) {}
	~NameStruct() {}
	virtual void SetPropertys() {
		//
		this->SetProperty("name", CJsonObjectBase::asString, &(this->name));
		this->SetProperty("connected", CJsonObjectBase::asBool, &(this->connected));
	}
};
class FamilyStruct :public CJsonObjectBase  {
private:
	string name;
	vector<NameStruct*> nameList;

public:
	FamilyStruct() {

	}
	FamilyStruct(string name) { this->name = name; }
	~FamilyStruct() {

	}
	void freeStruct(){
	
		for (int i = 0; i < nameList.size(); i++) {
			delete nameList.at(i);
		}
	
	
	}


	void setName(string name) { this->name = name; }
	string getName() { return name; }
	void addName(string name) {
		NameStruct* name_ = new NameStruct(name,false);
		nameList.push_back(name_);
		// Get all the names of members in a string, with ';' as spliter
	}
	void setNameList(vector<NameStruct*> namelist) {
		this->nameList = namelist;
	}
	vector<NameStruct*>   getNameList() { return nameList; }

	virtual void SetPropertys() {
		//
		this->SetProperty("name", CJsonObjectBase::asString, &(this->name));
		this->SetProperty("nameList", CJsonObjectBase::asVectorArray, &(this->nameList),CJsonObjectBase::asJsonObj);
	}
};


class GroupStruct :public CJsonObjectBase  {
private:
	string name;
	vector<FamilyStruct*> familyList;
	//初始表初始化

public:
	GroupStruct() :familyList(), name("") {}
	GroupStruct(string name_) :familyList(), name(name_) {}
	~GroupStruct() {
	}
	void freeStruct() {
		for (int i = 0; i < familyList.size(); i++) {
			familyList.at(i)->freeStruct();
			delete familyList.at(i);
		}	
	}
	void setName(string name) { this->name = name; }
	string getName() { return name; }
	void addFamily(FamilyStruct* family) {
		familyList.push_back(family);
	}
	vector<FamilyStruct*> getFamilyList() { return familyList; }
	virtual void SetPropertys() {
		//
		this->SetProperty("name", CJsonObjectBase::asString, &(this->name));
		this->SetProperty("familyList", CJsonObjectBase::asVectorArray, &(this->familyList),CJsonObjectBase::asJsonObj);
	}

};

class Led_field:public CJsonObjectBase   {
public:
	int led_R;
	int led_G;
	int led_B;
	Led_field(uint8_t R, uint8_t G, uint8_t B) :led_R(R), led_G(G), led_B(B) {

	}
	virtual void SetPropertys() {
		this->SetProperty("led_R", asInt, &led_R);
		this->SetProperty("led_G", asInt, &led_G);
		this->SetProperty("led_B", asInt, &led_B);
	}
	~Led_field() {

	}
};
class Actuator_field :public CJsonObjectBase {

public:
	double position;
	double velocity;
	double torque;
	double voltage;
	double motoCurrent;
	Actuator_field(double p, double v, double t, double vol,double motoCurrent_) :position(p), velocity(v), torque(t), voltage(vol),motoCurrent(motoCurrent_) {}
	~Actuator_field() {



	}
	virtual void SetPropertys() {
		this->SetProperty("position", asDouble, &position);
		this->SetProperty("velocity", asDouble, &velocity);
		this->SetProperty("torque", asDouble, &torque);
		this->SetProperty("voltage", asDouble, &voltage);
		this->SetProperty("motoCurrent", asDouble, &motoCurrent);
	}

};



//group里面module的重置格式
//反正只要我们需要的就好
class FeedbackCustomStruct:public CJsonObjectBase   {

public:

	Led_field led_field;
	Actuator_field actuator_field;
	FeedbackCustomStruct(Led_field l_f, Actuator_field a_f) :led_field(l_f.led_R, l_f.led_G, l_f.led_B), actuator_field(a_f.position, a_f.velocity, a_f.torque, a_f.voltage,a_f.motoCurrent) {

	}
	virtual void SetPropertys() {
		this->SetProperty("led_field", asJsonObj, &led_field);
		this->SetProperty("actuator_field", asJsonObj, &actuator_field);

	}
	~FeedbackCustomStruct() {


	}
};
//groupFeedBack的重置格式
class GroupfeedbackCustomStruct:public CJsonObjectBase   {



public:
	vector<double> positionsVec;
	vector<double> velocitysVec;
	vector<double> torqueVec;
	string groupName;
	vector<FeedbackCustomStruct*> moduleFeedBackVec;

	GroupfeedbackCustomStruct(vector<double> pVec,
		vector<double> vVec, vector<double> tVec, string groupName_) :
		positionsVec(pVec),
		velocitysVec(vVec),
		torqueVec(tVec),
		moduleFeedBackVec(),
		groupName(groupName_)
	{

	}
	virtual void SetPropertys() {
		this->SetProperty("positionsVec", CJsonObjectBase::asVectorArray, &positionsVec,CJsonObjectBase::asDouble);
		this->SetProperty("velocitysVec", CJsonObjectBase::asVectorArray, &velocitysVec, CJsonObjectBase::asDouble);
		this->SetProperty("torqueVec", CJsonObjectBase::asVectorArray, &torqueVec, CJsonObjectBase::asDouble);
		this->SetProperty("groupName", CJsonObjectBase::asString, &groupName);
		this->SetProperty("moduleFeedBackVec", CJsonObjectBase::asVectorArray, &moduleFeedBackVec,CJsonObjectBase::asJsonObj);

	}
	~GroupfeedbackCustomStruct() {

	}
	void freeStruct() {
		for (int i = 0; i < moduleFeedBackVec.size(); i++) {
			delete moduleFeedBackVec.at(i);
		}	
	}

	bool putIntoModuleFeedBackVec(vector<FeedbackCustomStruct*>& fd_custom);//把模块的feedback放进去





};
class CommandStruct:public CJsonObjectBase {
public:
	Actuator_field actuator_field;
	Led_field led_field;
	CommandStruct(Actuator_field a, Led_field l) :actuator_field(a), led_field(l) {

	}
	~CommandStruct() {


	}
	virtual void SetPropertys() {
		this->SetProperty("led_field", asJsonObj, &led_field);
		this->SetProperty("actuator_field", asJsonObj, &actuator_field);

	}
};
class CommandGroupStruct :public CJsonObjectBase  {
public:

	string groupName;
	vector<string> familys;
	vector<string> names;
	vector<CommandStruct*> fd;
	int cmd;
	CommandGroupStruct() {};
	CommandGroupStruct(vector<CommandStruct*> fd_,vector<string> fs_,vector<string> names_) :fd(fd_),familys(fs_),names(names_) {

	}
	~CommandGroupStruct() {
	}
	void freeStruct() {
		for (int i = 0; i < fd.size();i++) {
			delete fd.at(i);
		}
	}

	virtual void SetPropertys() {
		this->SetProperty("groupName", asString, &groupName);
		this->SetProperty("familys", asVectorArray, &familys,asString);
		this->SetProperty("names", asVectorArray, &names,asString);
		this->SetProperty("fd", asVectorArray, &fd,asJsonObj);
		this->SetProperty("cmd", asInt, &cmd);
	}

};



class RedisCofig {
public:
	string ip;
	int port;
};
class DBconfig {
public:
	string user;
	string pwd;
	string ip;
	int port;
	string dbSchme;
	map<string, string> parmMap;
	DBconfig() :parmMap() {}
};
class CacheConnection {
	/*
	这个类管理底层对缓存的连接
	init连接缓存
	get/set 方法

	*/
private:
	string ip;
	int port;
	redisContext* rdc;

	mutable std::mutex mut;
	
public:
	CacheConnection(string ip_, int port_) :ip(ip_), port(port_), rdc(NULL) {}
	~CacheConnection() {
		this->disconnect();
	}



	bool connect() {
		printf("is going to connect redis [ip:%s,port:%d]",this->ip.data(),this->port);
		const string ip_ = this->ip;
		rdc = redisConnect(this->ip.data(), this->port);
		return rdc == NULL ? false : true;
	

	}//连接
	bool reconnect() {
		if (this->isConnected()) {
			return true;

		}
		else
		{
			const string ip_ = this->ip;
			rdc = redisConnect(this->ip.data(), this->port);
			return rdc == NULL ? false : true;
		}
	

	}//重新连接
	bool disconnect() {
		freeContext();
		rdc = NULL;
		return true;
	}//断开
	bool isConnected() {

		return rdc == NULL ? false : true;


	}//是否连接
	bool init() {
		return this->connect();

	}// 连接
	void setIpAndPort(string ip_, int port_) {
		this->ip = ip_;
		this->port = port_;
	}//重新设置

	void freeContext() {
	redisFree(this->rdc);

	}//清除context
	bool isContainKey(char*  key) {
		void* intP;
		int arg_nums = 2;
		const char* args[] = { "exists",key };
		this->setCommndWithArgs(arg_nums, args, "keyExist", intP);
		delete[] args;
		return (*(int*)intP == 0) ? false : true;
	}//是否包含Key
	bool deleteKey(char*  key) {
		void* intP;
		int arg_nums = 2;
		const char* args[] = { "del",key };
		this->setCommndWithArgs(arg_nums, args, "keyDelete", intP);
		delete[] args;
		return (*(int*)intP == 0) ? false : true;


	}//删除键
	bool setCommndWithArgs(int agr_nums, const char** args, string des, void* &res) {
		if (this->isConnected() == false) return false;
		std::unique_lock<std::mutex> lk(mut);
		redisCommandArgv(this->rdc, agr_nums, args, NULL);
		redisReply *reply;
		redisGetReply(rdc, (void**)&reply);
		switch (reply->type)
		{
		case REDIS_REPLY_ARRAY: {
			//是数组，那么就需要读数组
			vector<string>* vec = new vector<string>();

			for (int i = 0; i<reply->elements; i++) {
				vec->push_back((*(reply->element))[i].str);
			}
			res = (void*)vec;
			return true;

		}
		case REDIS_REPLY_INTEGER: {
			LONGLONG* a = new LONGLONG();
			(*a) = reply->integer;
			res = (void*)a;
			return true;
		}
		case REDIS_REPLY_STRING: {
			string* a = new string(reply->str);
			res = (void*)a;
			return true;

		}
		case REDIS_REPLY_NIL: {
			printf("error:get null from redis\n");

			return false;
		}
		case REDIS_REPLY_ERROR: {
			printf("error:executed error\n");
		
			return false;
		}
		case REDIS_REPLY_STATUS: {
			//某个操作的，比如DEL SET 放东西的操作,删除和查键就不算在里面了
			if (des == GET_LIST) {
				//获取列表,不会触发

			}
			else if (des == PUT_LIST) {
				//放列表的操作
				return  reply->integer <= 0 ? false : true;

			}
			else if (des == ADD_VALUE_TO_SET) {
				//创建或添加set
				return  reply->integer <= 0 ? false : true;

			}
			else if (des == DELETE_VALUE_TO_SET) {
				//删除某个set的成员
				return  reply->integer <= 0 ? false : true;
			}
			else if (des == GET_STR) {
				//获取字符串(序列化对象),不会到这里


			}
			else if (des == SET_STR) {
				//设置字符串（序列化对象）
				return strcmp(reply->str, "OK") == 0;
			}
			else {
				//奇怪的操作
				return false;
			}


			cout << reply->str << endl;
			return true;
		}
		default:
			cout << "unexcepted error" << endl;
			return false;
			break;
		}


	}//发送命令
	//	return true;
	
};
#endif