#ifndef	FEEDBACKMANAGER_H
#define FEEDBACKMANAGER_H
#include "src/group_feedback.hpp"
#include "src/feedback.hpp"
#include "queue_safe.h"
#include <vector>
#include "JsonObjectBase.h"
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
	queue_safe<GroupfeedbackCustomStruct>& group_feedback_queue;
	
public:
	 FeedBackManager(queue_safe<GroupfeedbackCustomStruct>& group_feedback_queue_);
	 //��������
	~FeedBackManager();
	GroupfeedbackCustomStruct toGroupFbCustomStruct(const GroupFeedback* group_fdb,string groupName);//ת������
	FeedbackCustomStruct toFeedBackCustomStruct(const Feedback& fdb); //��ÿ��ģ���fbת���ɿɶ�����
	void putToQueue(GroupfeedbackCustomStruct group_fdb_custom);//�Ž����У��������Ҫ��Щ������
	


};
//groupFeedBack�����ø�ʽ
class GroupfeedbackCustomStruct:public CJsonObjectBase{
	


public:
	vector<double> positionsVec;
	vector<double> velocitysVec;
	vector<double> torqueVec;
	string groupName;
	vector<FeedbackCustomStruct> moduleFeedBackVec;

	GroupfeedbackCustomStruct(vector<double> pVec,
		vector<double> vVec,vector<double> tVec,string groupName_):
		positionsVec(pVec),
		velocitysVec(vVec),
		torqueVec(tVec),
		moduleFeedBackVec(),
		groupName(groupName_)
		{
		
	}
	virtual void SetPropertys(){
			this->SetProperty("positionsVec",CJsonObjectBase::asVectorArray,&positionsVec);
			this->SetProperty("velocitysVec",CJsonObjectBase::asVectorArray,&velocitysVec);
			this->SetProperty("torqueVec",CJsonObjectBase::asVectorArray,&torqueVec);
			this->SetProperty("groupName",CJsonObjectBase::asString,&groupName);
			this->SetProperty("moduleFeedBackVec",CJsonObjectBase::asVectorArray,&moduleFeedBackVec);

		}
	~GroupfeedbackCustomStruct();
	bool putIntoModuleFeedBackVec(vector<FeedbackCustomStruct>& fd_custom);//��ģ���feedback�Ž�ȥ





};
//group����module�����ø�ʽ
//����ֻҪ������Ҫ�ľͺ�
class FeedbackCustomStruct:public CJsonObjectBase{

	public:
	
		Led_field led_field;
		Actuator_field actuator_field;
		FeedbackCustomStruct(Led_field l_f,Actuator_field a_f):led_field(l_f.led_R,l_f.led_G,l_f.led_B),actuator_field(a_f.position,a_f.velocity,a_f.torque,a_f.voltage){

		}
		virtual void SetPropertys(){
			this->SetProperty("led_field",asJsonObj,&led_field);
			this->SetProperty("actuator_field",asJsonObj,&actuator_field);
			
		}
		~FeedbackCustomStruct();
};
class Led_field:public CJsonObjectBase{
public:
	int led_R;
	int led_G;
	int led_B;
	Led_field(uint8_t R,uint8_t G,uint8_t B):led_R(R),led_G(G),led_B(B){

	}
	virtual void SetPropertys(){
		this->SetProperty("led_R",asInt,&led_R);
		this->SetProperty("led_G",asInt,&led_G);
		this->SetProperty("led_B",asInt,&led_B);
	}
	~Led_field();
};
class Actuator_field:public CJsonObjectBase{

public:
	double position;
	double velocity;
	double torque;
	double voltage;
	Actuator_field(double p,double v ,double t,double vol):position(p),velocity(v),torque(t),voltage(vol){}
	~Actuator_field();
	virtual void SetPropertys(){
		this->SetProperty("position",asDouble,&position);
		this->SetProperty("velocity",asDouble,&velocity);
		this->SetProperty("torque",asDouble,&torque);
		this->SetProperty("voltage",asDouble,&voltage);
	}

};


#endif