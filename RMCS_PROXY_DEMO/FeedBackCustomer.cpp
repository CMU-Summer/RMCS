#include "FeedBackCustomer.h"


void FeedBackCustomer::run(){
	while (true)
	{
		
		customGfd();

		if(this->sleeptime>0){
			this_thread::sleep_for(std::chrono::milliseconds(this->sleeptime));
		}
	}


}

void FeedBackCustomer::init(){

	this->start();//������
} //��ʼ��


bool FeedBackCustomer::customGfd(){

	shared_ptr<GroupfeedbackCustomStruct> mapPtr  = this->gfd_quee.try_pop();
	if(!mapPtr)return false;//û��ȡ����Ҫ���ĵ�gfd
	printGroupFeedBack(*mapPtr);
	bool opt=this->insertCache(*mapPtr);
	bool opt1=this->insertDb(LOCAL,*mapPtr);
	bool opt2=this->insertDb(WEB,*mapPtr);
	return opt == opt2 == opt1 == true;
}

bool FeedBackCustomer:: insertCache(GroupfeedbackCustomStruct& gfd){
	return this->cacheManager.updateGroupFeedBack(gfd);
	

}//�Ž���������
bool FeedBackCustomer:: insertDb(int type,GroupfeedbackCustomStruct& gfd){
	cout<<"put into db!!!"<<endl;
	return true;


}//�Ž����ݿ�����