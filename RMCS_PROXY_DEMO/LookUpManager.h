#ifndef	LOOKUPMANAGER_H
#define LOOKUPMANAGER_H
#include "CThread.h"
#include <map>
#include <vector>
#include <string>
#include "src/lookup.hpp"
using namespace std;
#include "CacheManager.h"
#include <queue>
#include <list>

/*
class LookUpManager:public CThread{
	//������lLookUpManager������
	//���Ǹ��߳���
	//��Ҫ�����Եĸ���family��names
	//���»����family and name map
	//���������ԵĴӻ����ж�ȡ�û������group
	//����group������״̬
	//��Ҫ���������
	//��Ҫ���建�������gruopStateMap
	/*
	����group
	key:{
		family:
		name:[]
	}
	value:{
		isConnected:
		list:[
			
		]



	}
	
	
	*/
/*
private:
	//------------------------
	hebi::Lookup& lookup; //hebi�ӿڶ���
	queue<hebi::GroupFeedback*>& groupFeedbackQueue;//�յ����첽groupfeedback��Ӧ�÷ŵ�����
	//group�����Ӻͼ���,�᲻��Ӱ��ʲô�����Ʋ���
	map<GroupStruct,unique_ptr<hebi::Group>> cacheGroupMap;//��Ҫ���µ�map,�ӻ�������ȡ�����keyû�о�ȡһ��addFh,�оͲ���
	vector<FeedBackManager> feedbackManagerVec;
	vector<GroupStruct> fixedGroup;
	int sleep_time; //����ʱ�䣬ȡĬ��ֵ
	//----------------------
	


	//----------------------
	void updateFamilyAndNamesMap(map<string,vector<string>> newFandNMap);//���»����е�map,ָ���ں����ڽ�������
	map<string,vector<string>> getNewestMapFromHibi();//������µ�family map������,���ش��ĺ����н�������
	bool isFirstRun;//�Ƿ��һ�����У��ǵĻ�Ϊtrue������Ϊfalse,��ʼ��Ϊfalse,��һ��run��ʱ��ȥ������sleepʱ��,Ȼ��getEntryList

	vector<GroupStruct> getGroupListFromCache();//�ӻ������ȡ��ǰ�û������group����Ϣ,��mapȥ��
	void updateGroupConncetState(vector<GroupStruct> groupInCache);//ˢ��group������״̬,�������
	bool updateOneGroupState(GroupStruct groupStrut);//����ŵ�cacheManager�Ķ�������ȥ���ܷŽ�ȥ����true
	vector<GroupStruct> getGroupsStateFromHeibi(vector<GroupStruct> groupVec);//��heibi������cahce���涨���group������״̬
	GroupStruct	getGroupStateFromHeibi(GroupStruct groupVec);
	
	

	vector<GroupStruct> getGroupListFromConfig();//�������ļ������ȡ

	



	//-----------------------
public:
	LookUpManager(CacheManager& cacheManager,queue<hebi::GroupFeedback>& groupFeedbackQueue);
	//��Ҫʲô����,��������࣬�򵥣�����Ҫ��Ϣ��ͨ,û��Ҫ������������ģ��,
	//���������������󣬶��ж���
	//����Ҫ�ڳ�ʼ���б��и�ֵ
	//lookup�����洴��
	~LookUpManager();
	void run() override; // override run function,to find 
	void init() ;//init the lookupManager,����������
	//-------------`-`-`-`-`-`-`-----------
	static const int  DEAULT_SLEEP_TIME=100; //Ĭ�ϵ�����ʱ��


}; */

class FamilyStruct {
private:
	string name;
	vector<string> nameList;
	string namesWithinFamily;

public:
	FamilyStruct() {}
	FamilyStruct(string name) { this->name = name; }
	~FamilyStruct() {}
	void setName(string name) { this->name = name; }
	string getName() { return name; }
	void addName(string name) {
		nameList.push_back(name);
		namesWithinFamily = string("");
		// Get all the names of members in a string, with ';' as spliter
		for (int i = 0;i < nameList.size();i++) {
			namesWithinFamily.append(nameList[i] + ";");
		}
	}
	void setNameList(vector<string> namelist) { 
		this->nameList = namelist;		
	}
	string getNamesWithinFamily() {
		return namesWithinFamily;
	}
	vector<string> getNameList() { return nameList; }

};


class GroupStruct{
private:
	string name;
	vector<FamilyStruct> familyList;
	//��ʼ���ʼ��
	bool isConnected;
public:
	GroupStruct() {}
	GroupStruct(string name) { this->name = name; }
	~GroupStruct() {}
	void setName(string name) { this->name = name; }
	string getName() { return name; }
	void addFamily(FamilyStruct family) {
		familyList.push_back(family);
	}
	vector<FamilyStruct> getFamilyList() { return familyList; }
	void setConnectStatus() {
	
		// ����isConnected�Ĳ���ֵ ������Ҫ���nʱ����������isConnected��ֵ
	}
	bool getConnectedStatus(){return isConnected;}

};




#endif