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
	//初始化
	//获取值



}
//需要什么参数,缓存管理类，简单，不需要信息沟通,没必要生产者消费者模型,
//传进来缓存管理对象，队列对象，
//引用要在初始化列表中赋值
//lookup在里面创建
LookUpManager::~LookUpManager(){
	//没有确定需要释放什么东西


}
void LookUpManager::run() {
	//逻辑运行的地方
	while(true){
		//再次确认，抽取cache里面的group，更新状态；更新cahce里面的family和name的关系表
		//1.给没有异步处理方法的group添加handler
		this->addHandlerFromGroups();


		//2.刷新f and n
		this->updateFamilyAndNamesMap(this->getNewestMapFromHibi());


		//3.刷新group里面的name的连接状态
		this->updateGroupConncetState(this->cacheManager.getGroupInCache());

		//4。线程暂停
		if(this->sleep_time>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleep_time));
		}
		printf("LOOKUPMANAGER_THREAD: lookupManager thread next executed\n");
	}


} // override run function,to find 
void LookUpManager::init() {
	//获取
	vector<GroupStruct> grpList=this->getGroupListFromConfig();


	this->fixedGroup.assign(grpList.begin(),grpList.end()); 
	
	this->start();//启动线程,还有其他的操作？

}//init the lookupManager,比如跑起来
//----------------------
void LookUpManager::updateFamilyAndNamesMap(map<string,vector<string>> newFandNMap){
	
	this->cacheManager.updateCacheFamilyAndItsNames(newFandNMap);
	
}//更新缓存中的map,指针在函数内进行销毁
map<string,vector<string>> LookUpManager::getNewestMapFromHibi(){
	//从hebi里面获取
	map<string,vector<string>> fMap;
	unique_ptr<hebi::Lookup::EntryList> entrylist = this->lookup.getEntryList();
	if (!entrylist) {
		printf("LOOKUPMANAGER_THREAD: entrylist is null !!\n");
		return fMap;
	}
	for(int i=0; i<entrylist->size();i++){
		auto entry = entrylist->getEntry(i);
		if(fMap.count(entry.family_)){
			//包含，就放进去
		
			(fMap[entry.family_]).push_back(entry.name_);

		}else {
			//不包含
			fMap[entry.family_]=vector<string>();//
			fMap[entry.family_].push_back(entry.name_);

		}

	}
	return fMap;
}//获得最新的family map的值


vector<GroupStruct> LookUpManager::getGroupListFromCache(){
	return this->cacheManager.getGroupInCache();

}//从缓存里获取当前用户定义的group的信息,到map去查


void LookUpManager::updateGroupConncetState(vector<GroupStruct> groupInCache,int default_timeout){
	//根据这些group，去判断是否连接了
	if( groupInCache.size()>0 && this->updateGroupsStateInCache(getGroupsStateFromHeibi(groupInCache,default_timeout))){
		//成功刷新缓存
		
		printf("LOOKUPMANAGER_THREAD: group connection status flush successfully\n");

	}else {
		if(cacheManager.isConnected()){
			//连接上了，就抱个错
		}else{
			//没有连接上，既然是刷新状态，没必要干什么了
	
			printf("LOOKUPMANAGER_THREAD: disconnected,can not flush\n");
		}
	}

}//刷新group的连接状态,这里面的


bool LookUpManager::updateGroupsStateInCache(vector<GroupStruct> groupStrut){
	return this->cacheManager.updateCacheGroupStateList(groupStrut);
}//这个放到cacheManager的队列里面去，能放进去就是true

vector<GroupStruct> LookUpManager::getGroupsStateFromHeibi(vector<GroupStruct> groupVec,int default_int){
	for(int i=0;i<groupVec.size();i++){
		for(int j=0;j<groupVec.at(i).getFamilyList().size();j++){
			//每个family
			string familyName=groupVec.at(i).getFamilyList().at(j).getName();
			//遍历Map
			vector<NameStruct>& nameListMap=groupVec.at(i).getFamilyList().at(j).getNameList();
			vector<NameStruct>::iterator it;
			for(it=nameListMap.begin();it!=nameListMap.end();it++){
				unique_ptr<Group> grp=this->lookup.getConnectedGroupFromName(it->name,familyName);
				if(grp){
					it->connected=true;//更新状态为true
				}

			}	
		}
	}	
	return groupVec;//更新完返回




}//从heibi里面获得cahce里面定义的group的连接状态


vector<GroupStruct> LookUpManager::getGroupListFromConfig(){
	//获取从配置文件读的group
	 return this->configManager.getGroupList();




}//从配置文件里面获取
void LookUpManager::addHandlerFromGroups(){
	//给fixedgroup添加
	if(fixedGroup.size()>0 && this->fixedAdded == false){
		printf("LOOKUPMANAGER_THREAD:add FeedBack Handle to Group\n");
		for(int i=0;i<fixedGroup.size();i++){

			vector<string>* familyVec=new vector<string>(),*nameVec = new vector<string>();
			//获取names和familys
			string gname=fixedGroup.at(i).getName();
			this->getFamilyAndNamesFromGroupStruct(fixedGroup.at(i),familyVec,nameVec);
			//给找到的group添加feedbackManager
			this->addHandlerForOneGroup(familyVec,nameVec,gname);
			delete familyVec,nameVec;
		}
	}
	vector<GroupStruct> gstVec= this->cacheManager.getGroupInCache();//没有的话会返回个空的
	for(int i=0;i<gstVec.size();i++){
		printf("LOOKUPMANAGER_THREAD:add FeedBack Handle to Group\n");
		if(this->cacheGroupMap.count(gstVec.at(i).getName())){
			//有，就不加了
		}else {
			//没有，这个group还没有一个feedbackManager来处理
			vector<string>* familyVec=new vector<string>(),*nameVec = new vector<string>();
			//获取names和familys
			this->getFamilyAndNamesFromGroupStruct(gstVec.at(i),familyVec,nameVec);
			//添加feedbackManager
			string gname= gstVec.at(i).getName();
			this->addHandlerForOneGroup(familyVec,nameVec,gname);
			//释放指针
			delete familyVec,nameVec;
		}

	}


}//为fixed和缓存的group加处理函数

void LookUpManager::addHandlerForOneGroup(vector<string>* &familyVec,vector<string>* &nameVec,string groupName){
	FeedBackManager* fdbManager=new FeedBackManager(this->groupFeedbackQueue); 
	unique_ptr<Group> grp;
	grp = this->lookup.getGroupFromNames(*nameVec, *familyVec, DEAULT_SLEEP_TIME);
	if (!grp){
		printf("LOOKUPMANAGER_THREAD: group from hebi is null!s\n");
		return;//null不用加
	}
	LookUpManager* this_ = this;

	/*8/16/2017 Pittsburgh Luyao Li Debug： 用同步方式获取feedback*/

	GroupFeedback group_fbk(grp->size());
	if (grp->requestFeedback(&group_fbk)){
		this_->showGroupFeedBackInfo(&group_fbk);
		GroupfeedbackCustomStruct gfb_custom = fdbManager->toGroupFbCustomStruct(&group_fbk, groupName);
		fdbManager->putToQueue(gfb_custom);
	}

	/*
	grp->addFeedbackHandler([&fdbManager, &groupName, &this_](const GroupFeedback* const group_fbk)->void
	{
		//用fdbManager里面的函数
		this_->showGroupFeedBackInfo(group_fbk);
		GroupfeedbackCustomStruct gfb_custom = fdbManager->toGroupFbCustomStruct(group_fbk, groupName);
		fdbManager->putToQueue(gfb_custom);
	});
	*/

	grp->setFeedbackFrequencyHz(this->default_frequency);
	this->feedbackManagerVec.push_back(*fdbManager); //放进vec里面管理
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
void LookUpManager::showGroupFeedBackInfo(const GroupFeedback* group_fbk) {
	printf("LOOKUPMANAGER_THREAD:[-------GroupFeedBack------]\n[size:%d]\n",group_fbk->size());
	for (int i = 0; i < group_fbk->size();i++) {
		printf("LOOKUPMANAGER_THREAD: module[%d] feedback info:",i);
		printf("voltage %f", (*group_fbk)[i].voltage().get());
		printf("Motor current %f",(*group_fbk)[i].actuator().motorWindingCurrent().get());
		printf("position %f", (*group_fbk)[i].actuator().position().get());
		printf("velocity %f", (*group_fbk)[i].actuator().velocity().get());
		printf("torque %f\n", (*group_fbk)[i].actuator().torque().get());
	}
	printf("LOOKUPMANAGER_THREAD:[-------END------]\n", group_fbk->size());



}
