#include "LookUpManager.h"
#include "src/lookup.hpp"
#include "ConfigManager.h"
#include "FeedBackManager.h"
using namespace std;
using namespace hebi;
LookUpManager::LookUpManager(CacheManager& cacheManager_,
							 queue_safe<GroupfeedbackCustomStruct>& groupFeedbackQueue_,
							 Lookup lookup_,
							 int sleep_time_=DEAULT_SLEEP_TIME,
							 ConfigManager& configManager_
							 )
	:cacheManager(cacheManager_)
	,groupFeedbackQueue(groupFeedbackQueue_)
	,lookup(lookup_)
	,sleep_time(sleep_time_)
	,configManager(configManager_)
	,fixedGroup()
	,cacheGroupMap()
	,feedbackManagerVec()
	,fixedAdded(false)
{
	//��ʼ��
	//��ȡֵ



}
//��Ҫʲô����,��������࣬�򵥣�����Ҫ��Ϣ��ͨ,û��Ҫ������������ģ��,
//���������������󣬶��ж���
//����Ҫ�ڳ�ʼ���б��и�ֵ
//lookup�����洴��
LookUpManager::~LookUpManager(){
	//û��ȷ����Ҫ�ͷ�ʲô����


}
void LookUpManager::run() {
	//�߼����еĵط�
	while(true){
		//�ٴ�ȷ�ϣ���ȡcache�����group������״̬������cahce�����family��name�Ĺ�ϵ��
		//1.��û���첽��������group���handler
		this->addHandlerFromGroups();


		//2.ˢ��f and n
		this->updateFamilyAndNamesMap(this->getNewestMapFromHibi());

		//3.ˢ��group�����name������״̬

		this->updateGroupConncetState(this->cacheManager.getGroupInCache());
		//4���߳���ͣ
		if(this->sleep_time>=0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time));
		}
	}


} // override run function,to find 
void LookUpManager::init() {
	//��ȡ
	vector<GroupStruct> grpList=this->getGroupListFromConfig();
	this->fixedGroup.assign(grpList.begin(),grpList.end()); 

	this->start();//�����߳�,���������Ĳ�����

}//init the lookupManager,����������
//----------------------
void LookUpManager::updateFamilyAndNamesMap(map<string,vector<string>> newFandNMap){
	
	this->cacheManager.updateCacheFamilyAndItsNames(newFandNMap);
	
}//���»����е�map,ָ���ں����ڽ�������
map<string,vector<string>> LookUpManager::getNewestMapFromHibi(){
	//��hebi�����ȡ
	map<string,vector<string>> fMap;
	unique_ptr<hebi::Lookup::EntryList> entrylist = this->lookup.getEntryList();
	for(int i=0; i<entrylist->size();i++){
		auto entry = entrylist->getEntry(i);
		if(fMap.count(entry.family_)){
			//�������ͷŽ�ȥ
			(fMap[entry.family_]).push_back(entry.name_);

		}else {
			//������
			fMap[entry.family_]=vector<string>();//
			fMap[entry.family_].push_back(entry.name_);

		}

	}
	return fMap;
}//������µ�family map��ֵ


vector<GroupStruct> LookUpManager::getGroupListFromCache(){
	return this->cacheManager.getGroupInCache();

}//�ӻ������ȡ��ǰ�û������group����Ϣ,��mapȥ��


void LookUpManager::updateGroupConncetState(vector<GroupStruct> groupInCache,int default_timeout){
	//������Щgroup��ȥ�ж��Ƿ�������
	if( groupInCache.size()>0 && this->updateGroupsStateInCache(getGroupsStateFromHeibi(groupInCache,default_timeout))){
		//�ɹ�ˢ�»���
		cout<<"group connection status flush successfully"<<endl;

	}else {
		if(cacheManager.isConnected()){
			//�������ˣ��ͱ�����
		}else{
			//û�������ϣ���Ȼ��ˢ��״̬��û��Ҫ��ʲô��
			cout<<"disconnected,can not flush "<<endl;
		}
	}

}//ˢ��group������״̬,�������


bool LookUpManager::updateGroupsStateInCache(vector<GroupStruct> groupStrut){
	return this->cacheManager.updateCacheGroupStateList(groupStrut);
}//����ŵ�cacheManager�Ķ�������ȥ���ܷŽ�ȥ����true

vector<GroupStruct> LookUpManager::getGroupsStateFromHeibi(vector<GroupStruct> groupVec,int default_int){
	for(int i=0;i<groupVec.size();i++){
		for(int j=0;j<groupVec.at(i).getFamilyList().size();j++){
			//ÿ��family
			string familyName=groupVec.at(i).getFamilyList().at(j).getName();
			//����Map
			vector<NameStruct>& nameListMap=groupVec.at(i).getFamilyList().at(j).getNameList();
			vector<NameStruct>::iterator it;
			for(it=nameListMap.begin;it!=nameListMap.end();it++){
				unique_ptr<Group> grp=this->lookup.getConnectedGroupFromName(it->name,familyName);
				if(grp){
					it->connected=true;//����״̬Ϊtrue
				}

			}	
		}
	}	
	return groupVec;//�����귵��




}//��heibi������cahce���涨���group������״̬


vector<GroupStruct> LookUpManager::getGroupListFromConfig(){
	//��ȡ�������ļ�����group
	 return this->configManager.getGroupList();




}//�������ļ������ȡ
void LookUpManager::addHandlerFromGroups(){
	//��fixedgroup���
	if(fixedGroup.size()>0 && this->fixedAdded == false){
		for(int i=0;i<fixedGroup.size();i++){
			vector<string>* familyVec=new vector<string>(),*nameVec = new vector<string>();
			//��ȡnames��familys
			string gname=fixedGroup.at(i).getName();
			this->getFamilyAndNamesFromGroupStruct(fixedGroup.at(i),familyVec,nameVec);
			//���ҵ���group���feedbackManager
			this->addHandlerForOneGroup(familyVec,nameVec,gname);
			delete familyVec,nameVec;
		}
	}
	vector<GroupStruct> gstVec= this->cacheManager.getGroupInCache();//û�еĻ��᷵�ظ��յ�
	for(int i=0;i<gstVec.size();i++){
		if(this->cacheGroupMap.count(gstVec.at(i).getName())){
			//�У��Ͳ�����
		}else {
			//û�У����group��û��һ��feedbackManager������
			vector<string>* familyVec=new vector<string>(),*nameVec = new vector<string>();
			//��ȡnames��familys
			this->getFamilyAndNamesFromGroupStruct(gstVec.at(i),familyVec,nameVec);
			//���feedbackManager
			string gname=fixedGroup.at(i).getName();
			this->addHandlerForOneGroup(familyVec,nameVec,gname);
			//�ͷ�ָ��
			delete familyVec,nameVec;
		}

	}


}//Ϊfixed�ͻ����group�Ӵ�����

void LookUpManager::addHandlerForOneGroup(vector<string>* &familyVec,vector<string>* &nameVec,string groupName){
	FeedBackManager* fdbManager=new FeedBackManager(this->groupFeedbackQueue); 
	unique_ptr<Group> grp= this->lookup.getGroupFromNames(*familyVec,*nameVec,DEAULT_SLEEP_TIME);
	grp->addFeedbackHandler([&fdbManager,&groupName](const GroupFeedback* group_fbk){
		//��fdbManager����ĺ���
		GroupfeedbackCustomStruct gfb_custom= fdbManager->toGroupFbCustomStruct(group_fbk,groupName);
		fdbManager->putToQueue(gfb_custom);
	});
	this->feedbackManagerVec.push_back(*fdbManager); //�Ž�vec�������
}
void LookUpManager::getFamilyAndNamesFromGroupStruct(GroupStruct& thisGroup,vector<string>* &familysVec,vector<string>* &namesVec){
	for(int j=0;j<thisGroup.getFamilyList().size();j++){
		familysVec->insert(familysVec->end(),thisGroup.getFamilyList().at(j).getName());
		vector<NameStruct>& nameListMap=thisGroup.getFamilyList().at(j).getNameList();
		vector<NameStruct>::iterator it;
		for(it=nameListMap.begin();it!=nameListMap.end();it++){
			namesVec->insert(namesVec->end(),it->name);
		}
	}

}
