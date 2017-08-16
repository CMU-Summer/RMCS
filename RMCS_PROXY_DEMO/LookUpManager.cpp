#include "LookUpManager.h"
#include "src/lookup.hpp"
#include "ConfigManager.h"
#include "FeedBackManager.h"
#define D_S_T 50
using namespace std;
using namespace hebi;
LookUpManager::LookUpManager(CacheManager& cacheManager_,
							 queue_safe<GroupfeedbackCustomStruct>& groupFeedbackQueue_,
							 Lookup& lookup_,
							 ConfigManager& configManager_,
							 int sleep_time_,
							 int default_frequency_
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
	,default_frequency(default_frequency_)
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
		if(this->sleep_time>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time));
		}
		printf("LOOKUPMANAGER_THREAD: lookupManager thread next executed\n");
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
	if (!entrylist) {
		printf("LOOKUPMANAGER_THREAD: entrylist is null !!\n");
		return fMap;
	}
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
		
		printf("LOOKUPMANAGER_THREAD: group connection status flush successfully\n");

	}else {
		if(cacheManager.isConnected()){
			//�������ˣ��ͱ�����
		}else{
			//û�������ϣ���Ȼ��ˢ��״̬��û��Ҫ��ʲô��
	
			printf("LOOKUPMANAGER_THREAD: disconnected,can not flush\n");
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
			string familyName=groupVec.at(i).getFamilyList().at(j)->getName();
			//����Map
			vector<NameStruct*>& nameListMap=groupVec.at(i).getFamilyList().at(j)->getNameList();
			vector<NameStruct*>::iterator it;
			for(it=nameListMap.begin();it!=nameListMap.end();it++){
				unique_ptr<Group> grp=this->lookup.getConnectedGroupFromName((*it)->name,familyName);
				if(grp){
					(*it)->connected=true;//����״̬Ϊtrue
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
		printf("LOOKUPMANAGER_THREAD:add FeedBack Handle to Group\n");
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
		printf("LOOKUPMANAGER_THREAD:add FeedBack Handle to Group\n");
		if(this->cacheGroupMap.count(gstVec.at(i).getName())){
			//�У��Ͳ�����
		}else {
			//û�У����group��û��һ��feedbackManager������
			vector<string>* familyVec=new vector<string>(),*nameVec = new vector<string>();
			//��ȡnames��familys
			this->getFamilyAndNamesFromGroupStruct(gstVec.at(i),familyVec,nameVec);
			//���feedbackManager
			string gname= gstVec.at(i).getName();
			this->addHandlerForOneGroup(familyVec,nameVec,gname);
			//�ͷ�ָ��
			delete familyVec,nameVec;
		}

	}


}//Ϊfixed�ͻ����group�Ӵ�����

void LookUpManager::addHandlerForOneGroup(vector<string>* &familyVec,vector<string>* &nameVec,string groupName){
	FeedBackManager* fdbManager=new FeedBackManager(this->groupFeedbackQueue); 
	unique_ptr<Group> grp= this->lookup.getGroupFromNames(*nameVec,*familyVec,DEAULT_SLEEP_TIME);
	if (!grp){
		printf("LOOKUPMANAGER_THREAD: group from hebi is null!s\n");
		return;//null���ü�
	}
	LookUpManager* this_ = this;
	grp->addFeedbackHandler([&fdbManager,&groupName,&this_](const GroupFeedback* group_fbk){
		//��fdbManager����ĺ���
		this_->showGroupFeedBackInfo(group_fbk);
		GroupfeedbackCustomStruct gfb_custom= fdbManager->toGroupFbCustomStruct(group_fbk,groupName);
		fdbManager->putToQueue(gfb_custom);
	});
	grp->setFeedbackFrequencyHz(this->default_frequency);
	this->feedbackManagerVec.push_back(*fdbManager); //�Ž�vec�������
}
void LookUpManager::getFamilyAndNamesFromGroupStruct(GroupStruct& thisGroup,vector<string>* &familysVec,vector<string>* &namesVec){
	for(int j=0;j<thisGroup.getFamilyList().size();j++){
		familysVec->insert(familysVec->end(),thisGroup.getFamilyList().at(j)->getName());
		vector<NameStruct*>& nameListMap=thisGroup.getFamilyList().at(j)->getNameList();
		vector<NameStruct*>::iterator it;
		for(it=nameListMap.begin();it!=nameListMap.end();it++){
			namesVec->insert(namesVec->end(),(*it)->name);
		}
	}

}
void LookUpManager::showGroupFeedBackInfo(const GroupFeedback* group_fbk) {
	printf("LOOKUPMANAGER_THREAD:[-------GropFeedBack------]\n[------size:%d]\n",group_fbk->size());
	for (int i = 0; i < group_fbk->size();i++) {
		printf("LOOKUPMANAGER_THREAD:moudule[%d][motorCurrent:%f,motorWindingCurrent:%f,voltage:%f,motorSensorTemperature:%f]\n",i,(*group_fbk)[i].actuator().motorCurrent().get(), (*group_fbk)[i].actuator().motorWindingCurrent().get(), (*group_fbk)[i].voltage().get(), (*group_fbk)[i].actuator().motorSensorTemperature().get());
	
	}
	printf("LOOKUPMANAGER_THREAD:[-------END------]\n", group_fbk->size());



}
