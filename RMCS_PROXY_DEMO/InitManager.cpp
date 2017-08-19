#include "InitManager.h"
using namespace std::chrono;
int main() {
#ifdef _WIN32
	//! Windows netword DLL init
	WORD version = MAKEWORD(2, 2);
	WSADATA data;

	if (WSAStartup(version, &data) != 0) {
		std::cerr << "WSAStartup() failure" << std::endl;
		return -1;
	}
#endif /* _WIN32 */
	return InitManager::main();

//-----------------test code for put feedbcak to cache---------------
// 	ConfigManager cManager("resource/config.xml");
// 	CacheManager c(cManager,300);
// 	c.initCacheManager();
// 	int defalut_sleeptime=2000;
// 	for (int k = 0; k < 100;k++) {
// 		GroupfeedbackCustomStruct gfd({ 0,0 }, { 0,0 }, { 0,0 }, "testGroupName");
// 		vector<FeedbackCustomStruct*> fdVec;
// 		for (int i = 0; i < 2; i++) {
// 			FeedbackCustomStruct* fdPtr = new FeedbackCustomStruct(Led_field(0, 0, 0), Actuator_field(0, 0, 0, 0, 0));
// 			fdVec.push_back(fdPtr);
// 
// 		}
// 		gfd.putIntoModuleFeedBackVec(fdVec);
// 		time_point<system_clock> now_time = system_clock::now(); //��ǰʱ��  
// 		time_t now_c = system_clock::to_time_t(now_time);
// 		gfd.timeStamp = (INT64)now_c;
// 		bool opt=c.updateGroupFeedBack(gfd);
// 		if (!opt) {
// 			cout << "put to queue false" << endl;
// 		}
// 		this_thread::sleep_for(std::chrono::milliseconds(defalut_sleeptime));
// 	
// 	
// 	}
//-----------------test code for put feedbcak to cache---------------


//-------------------test code for Serialize---------------------
// 	gfd.SetPropertys();
// 	string s(gfd.Serialize());
// 	vector<CommandStruct*> cmdVec;
// 	for (int i = 0; i < 2;i++) {
// 		CommandStruct* cmdPtr = new CommandStruct(Actuator_field(0,0,0,0,0),Led_field(0,0,0));
// 		cmdVec.push_back(cmdPtr);
// 	
// 	}
// 	CommandGroupStruct g_cmd(1, "testGroup", cmdVec, { "f1","f2" }, {"n1","n2"});
// 	g_cmd.SetPropertys();
// 	string s1(gfd.Serialize());
// 	printf("%s\n%s\n",s.data(),s1.data());
//-------------------test code for Serialize---------------------


}