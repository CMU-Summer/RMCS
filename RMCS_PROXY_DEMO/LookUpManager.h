#ifndef	LOOKUPMANAGER_H
#define LOOKUPMANAGER_H
#include "CThread.h"
#include <map>
#include <vector>
#include <string>
#include "src/lookup.hpp"
using namespace std;
#include "CacheManager.h"

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
	
	typedef struct GroupKeyInfo
	{
		//groupMap��groupStateMap��key
		string* family;

	};
private:
	//------------------------
	map<string,vector<string>>& familyAndNames_last; //��һ�ε�mapָ�룬��ʼ��Ϊnull
	hebi::Lookup& lookup;
	//----------------------
	void updateFamilyAndNamesMap(map<string,vector<string>&>& newFandNMap);//���»����е�map
	map<string,vector<string>&>& getNewestMapFromHibi();//������µ�family map������
	bool isFirstRun;//�Ƿ��һ�����У��ǵĻ�Ϊtrue������Ϊfalse,��ʼ��Ϊfalse,��һ��run��ʱ��ȥ������sleepʱ��,Ȼ��getEntryList
	
	//-----------------------
public:
	LookUpManager(CacheManager& cacheManager);//��Ҫʲô����,��������࣬�򵥣�����Ҫ��Ϣ��ͨ,û��Ҫ������������ģ��,
	~LookUpManager();
	void run() override; // override run function,to find 
	void init() ;//init the lookupManager
 


};
#endif