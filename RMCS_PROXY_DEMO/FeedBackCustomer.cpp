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

	this->start();//������
} //��ʼ��


bool FeedBackCustomer::customGfd(){
	printf("FEEDBACK_CUSTOMER : feedback customer is working!!!!\n");
	shared_ptr<GroupfeedbackCustomStruct> mapPtr  = this->gfd_quee.wait_and_pop();
	if(!mapPtr)return false;//û��ȡ����Ҫ���ĵ�gfd
	printGroupFeedBack(*mapPtr);//Feedback data display
	bool opt=this->insertCache(*mapPtr);
	bool opt1=this->insertDb(LOCAL,*mapPtr);
	bool opt2=this->insertDb(WEB,*mapPtr);
	
	return opt == opt2 == opt1 == true;
}

bool FeedBackCustomer:: insertCache(GroupfeedbackCustomStruct& gfd){
	printf("FEEDBACK_CUSTOMER : put into cache!!!\n");
	return this->cacheManager.updateGroupFeedBack(gfd);
	

}//�Ž���������
bool FeedBackCustomer:: insertDb(int type,GroupfeedbackCustomStruct& gfd){
	//cout<<"put into db!!!"<<endl;
	printf("FEEDBACK_CUSTOMER : put fd into local db or web db!!!\n");
	return true;


}//�Ž����ݿ�����