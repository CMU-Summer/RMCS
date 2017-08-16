#include "FeedBackCustomer.h"


void FeedBackCustomer::run(){
	while (true)
	{
		
		customGfd();
		if(this->sleeptime>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleeptime));
		}
		printf("FEEDBACK_CUSTOMER : feedback Customer is ready for next execute\n");
	}


}

void FeedBackCustomer::init(){

	this->start();//跑起来
} //初始化


bool FeedBackCustomer::customGfd(){
	printf("FEEDBACK_CUSTOMER : feedback customer is working!!!!\n");
	shared_ptr<GroupfeedbackCustomStruct> mapPtr  = this->gfd_quee.wait_and_pop();
	if(!mapPtr)return false;//没有取到需要消耗的gfd
	printGroupFeedBack(*mapPtr);//Feedback data display
	bool opt=this->insertCache(*mapPtr);
	bool opt1=this->insertDb(LOCAL,*mapPtr);
	bool opt2=this->insertDb(WEB,*mapPtr);
	
	return opt == opt2 == opt1 == true;
}

bool FeedBackCustomer:: insertCache(GroupfeedbackCustomStruct& gfd){
	printf("FEEDBACK_CUSTOMER : put into cache!!!\n");
	return this->cacheManager.updateGroupFeedBack(gfd);
	

}//放进缓存里面
bool FeedBackCustomer:: insertDb(int type,GroupfeedbackCustomStruct& gfd){
	//cout<<"put into db!!!"<<endl;
	printf("FEEDBACK_CUSTOMER : put fd into local db or web db!!!\n");
	return true;


}//放进数据库里面