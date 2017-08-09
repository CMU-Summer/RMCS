#ifndef	FEEDBACKMANAGER_H
#define FEEDBACKMANAGER_H
#include "src/group_feedback.hpp"
#include "src/feedback.hpp"
#include <queue>
using namespace std;
using namespace hebi;
class FeedBackManager{
	//���е�feedback�Ѿ�������������
	//��Ϊ�����Ķ����������߳���,���������������߳�֮��
	//��Ҫ����ɶ�����ʽ
	//1.��ҪfeedBackQueue
	//2.��Ҫ�ǵ�����
	//3.���д�����

private:
	queue<GroupFeedback*>& group_feedback_queue;
	
public:
	 FeedBackManager(queue<GroupFeedback*>& group_feedback_queue_);
	 //��������
	~FeedBackManager();
	GroupfeedbackCustomStruct toGroupFbCustomStruct(const GroupFeedback* group_fdb);//ת������
	FeedbackCustomStruct toFeedBackCustomStruct(Feedback* fdb); //��ÿ��ģ���fbת���ɿɶ�����
	void putToQueue(GroupfeedbackCustomStruct group_fdb_custom);//�Ž����У��������Ҫ��Щ������
	


};
//groupFeedBack�����ø�ʽ
class GroupfeedbackCustomStruct{
	


public:
	vector<float> positionsVec;
	vector<float> velocitysVec;
	vector<float> torqueVec;
	vector<FeedbackCustomStruct> moduleFeedBackVec;
	GroupfeedbackCustomStruct(vector<float> pVec,vector<float> vVec,vector<float> tVec):positionsVec(pVec),velocitysVec(vVec),torqueVec(tVec),moduleFeedBackVec(){
		
	}
	~GroupfeedbackCustomStruct();
	bool putIntoModuleFeedBackVec(FeedbackCustomStruct fd_custom);//��ģ���feedback�Ž�ȥ





};
//group����module�����ø�ʽ
//����ֻҪ������Ҫ�ľͺ�
class FeedbackCustomStruct{

	public:
		Led_field led_field;
		Actuator_field actuator_field;
		FeedbackCustomStruct(Led_field l_f,Actuator_field a_f):led_field(l_f.led_R,l_f.led_G,l_f.led_B),actuator_field(a_f.position,a_f.velocity,a_f.torque){

		}
		~FeedbackCustomStruct();
};
class Led_field{
public:
	uint8_t led_R;
	uint8_t led_G;
	uint8_t led_B;
	Led_field(uint8_t R,uint8_t G,uint8_t B):led_R(R),led_G(G),led_B(B){

	}
	~Led_field();
};
class Actuator_field{

public:
	float position;
	float velocity;
	float torque;
	Actuator_field(float p,float v ,float t):position(p),velocity(v),torque(t){}
	~Actuator_field();

};


#endif