#include "LookUpManager.h"
#include "src/lookup.hpp"
#include "ConfigManager.h"
#include "FeedBackManager.h"
#include <chrono>
#include <ctime>
#define D_S_T 50
using namespace std::chrono;
using namespace std;
using namespace hebi;
LookUpManager::LookUpManager(CacheManager& cacheManager_,
							 queue_safe<GroupfeedbackCustomStruct>& groupFeedbackQueue_,
							 Lookup& lookup_,
							 ConfigManager& configManager_,
							 map<string, unique_ptr<hebi::Group>>& cacheGroupMap_,
							 map<string, unique_ptr<hebi::Group>>& fixedGroupMap_,
							 int sleep_time_,
							 int default_frequency_
)
	:cacheManager(cacheManager_)
	,groupFeedbackQueue(groupFeedbackQueue_)
	,lookup(lookup_)
	,sleep_time(sleep_time_)
	,configManager(configManager_)
	,fixedGroup()
	,cacheGroupMap(cacheGroupMap_)
	,feedbackManagerVec()
	,fixedAdded(false)
	,default_frequency(default_frequency_)
	,fixedGroupMap(fixedGroupMap_)
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
		vector<GroupStruct> gst_vec = this->getGroupListFromCache();//ȡ����

		//1.��û���첽��������group���handler
		this->addHandlerFromGroups(gst_vec);

		
		//2.ˢ��f and n 
		this->updateFamilyAndNamesMap(this->getNewestMapFromHibi());


		//3.ˢ��group�����name������״̬���ŵ������Ǹ���������ȥ����
		this->updateGroupConncetState(gst_vec);


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
				else {
					(*it)->connected = false;//����״̬Ϊfalse
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
void LookUpManager::addHandlerFromGroups(vector<GroupStruct> gstVec){
	//��fixedgroup���
	if(fixedGroup.size()>0 && this->fixedAdded == false){
		printf("LOOKUPMANAGER_THREAD: add FeedBack Handle to Fixed Group\n");
		for(int i=0;i<fixedGroup.size();i++){
			if (this->fixedGroupMap.count(this->fixedGroup.at(i).getName())) { 
				
				continue; }//����Ҫ��������Ĳ�����
			vector<string>* familyVec=new vector<string>(),*nameVec = new vector<string>();
			//��ȡnames��familys
			string gname=fixedGroup.at(i).getName();
			this->getFamilyAndNamesFromGroupStruct(fixedGroup.at(i),familyVec,nameVec);
			//���ҵ���group���feedbackManager
			this->addHandlerForOneGroup(familyVec,nameVec,gname,FIXED_TYPE);
			//�ŵ�Map����
			
			//ֻ��һ��
			delete familyVec,nameVec;
		}
	}
	
	for(int i=0;i<gstVec.size();i++){
		printf("LOOKUPMANAGER_THREAD: add FeedBack Handle to Cache Group\n");
		if(this->cacheGroupMap.count(gstVec.at(i).getName())){
			//�������ʲôҲ����
			printf("LOOKUPMANAGER_THREAD: this group is already handled!\n");
		}else {
			//û�У����group��û��һ��feedbackManager������
			vector<string>* familyVec=new vector<string>(),*nameVec = new vector<string>();
			//��ȡnames��familys
			this->getFamilyAndNamesFromGroupStruct(gstVec.at(i),familyVec,nameVec);
			//���feedbackManager
			string gname= gstVec.at(i).getName();
			this->addHandlerForOneGroup(familyVec,nameVec,gname,CACHE_TYPE);
			//�ͷ�ָ��
			delete familyVec,nameVec;
			//������ַ�����Ӧ��cacheMap����ȥ
			
		}

	}


}//Ϊfixed�ͻ����group�Ӵ�����

void LookUpManager::addHandlerForOneGroup(vector<string>* &familyVec,vector<string>* &nameVec,string groupName,int type){
	//�������ģ�����group�����֣����cache�����У��Ͳ�����
	if (this->cacheGroupMap.count(groupName) > 0)return;
	else if (this->fixedGroupMap.count(groupName) > 0)return;
	
	
	FeedBackManager* fdbManager=new FeedBackManager(this->groupFeedbackQueue); 
	unique_ptr<Group> grp= this->lookup.getGroupFromNames(*nameVec,*familyVec,DEAULT_SLEEP_TIME);
	if (!grp){
		printf("LOOKUPMANAGER_THREAD: group from hebi is null!s\n");
		return;//null���ü�
	}
	LookUpManager* this_ = this;
	string* groupName_ = new string(groupName);
	grp->addFeedbackHandler([fdbManager,groupName_,&this_](const GroupFeedback* group_fbk)->void{
		//��fdbManager����ĺ���
		this_->showGroupFeedBackInfo(group_fbk);
		GroupfeedbackCustomStruct gfb_custom= fdbManager->toGroupFbCustomStruct(group_fbk, groupName_->data());
		time_point<system_clock> now_time = system_clock::now(); //��ǰʱ��  
		time_t now_c = system_clock::to_time_t(now_time);
		gfb_custom.timeStamp = (long)now_c;
		fdbManager->putToQueue(gfb_custom);
	});
	grp->setFeedbackFrequencyHz(this->default_frequency);
	if (type == FIXED_TYPE) {
		this->fixedGroupMap.insert(map<string, unique_ptr<Group>>::value_type(groupName, std::move(grp))); //�����ڴ�
	}
	else {
		this->cacheGroupMap.insert(map<string, unique_ptr<Group>>::value_type(groupName, std::move(grp)));//�����ڴ�
		this->feedbackManagerVec.push_back(*fdbManager); //�Ž�vec�������
	}
	
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
	printf("LOOKUPMANAGER_THREAD: [-------GroupFeedBack------]\n[size:%d]\n", group_fbk->size());
	for (int i = 0; i < group_fbk->size(); i++) {
		printf("LOOKUPMANAGER_THREAD: module[%d] feedback info:", i);
		printf("voltage %f", (*group_fbk)[i].voltage().get());
		printf("Motor current %f", (*group_fbk)[i].actuator().motorCurrent().get());
		printf("position %f", (*group_fbk)[i].actuator().position().get());
		printf("velocity %f", (*group_fbk)[i].actuator().velocity().get());
		printf("torque %f\n", (*group_fbk)[i].actuator().torque().get());
	}
	printf("LOOKUPMANAGER_THREAD: [-------END------]\n");



}
