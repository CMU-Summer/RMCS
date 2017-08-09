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


};

class GroupStruct{
private:
	string family;
	string namesWithinFamily;
	//��ʼ���ʼ��
	bool isConnected();
public:
	GroupStruct();
	~GroupStruct();
	string getFamily(){return family;}
	string getNamesWithinFamily(){return namesWithinFamily;}
	bool isConnected(){return isConnected;}
	void setFamily(string family_){this->family=family_;}
	void setNames(vector<string> names){
		string s("");
		for(int i=0;i<names.size();i++){
			s.append(names[i]+";");
		}
		this->namesWithinFamily.clear();
		this->namesWithinFamily.append(s);
	}

};


#endif