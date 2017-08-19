#include "ServerApiManager.h"

bool ServerApiManager::getCommandstrFromStream(){
	bool lastFinish = true;
	while (true)
	{
		char* tempBuf = new char[this->buf_size];
		int num = recv(this->m_sock,tempBuf,this->buf_size,0);
		if (num == 0)return false;//出错了
		else {
			string s1(tempBuf);
			//有$
			if (s1.find('$') != s1.npos ) {
				//有，判断下上次传完没有
				if (lastFinish) {
					//传完了，那么这次的直到$之前都是新的
					int syntx_index = s1.find('$');//记录第一个$的位置
					int index = 0; //记录每次搜索的起点
					string cStr;//一个完整的cmd字符串
					while (syntx_index !=s1.npos && index < s1.length() ) //第一次完美执行
					{
						//从起点到'$'的位置之前
						 cStr.clear();//清除之前的内容
						 cStr=s1.substr(index, index);//把内容付给cStr
						 CommandGroupStruct gcmd;
						 gcmd.DeSerialize(cStr.data());//反序列化
						 this->group_command_queue.push(gcmd);//压入队列
						 //查找下一个$的位置,从上一个+1的位置开始
						 syntx_index = s1.find('$', syntx_index + 1);
						 //让下次起始的位置是上一个$的+1位置
						 index= syntx_index+1;
					}
					if (syntx_index == s1.npos) {
						//没找到$了，判断下index是不是在最后一个位置,是在最后一个位置，说明接到的数据是完整的n个cmd字符串
						if (index == s1.length() - 1) {
							//完整的，没跑了
							lastFinish = true;//完成！
						}
						else {
							//没找到了，但是index不是最后一个，说明命令不完整，cStr应该放在buf里面
							for (int i = 0; i < cStr.length();i++) {
								buf.push_back(cStr.at(i));
							}
							lastFinish = false;//没有传完！
						}
					}

				}
				//上次没有传完
				else {
					//那就是说，找到的第一个位置的$之前的，是完整的
					int syntx_index = s1.find('$');//记录第一个$的位置
					int index = 0; //记录每次搜索的起点
					string cStr;//一个完整的cmd字符串
					cStr = s1.substr(index, syntx_index);//到第一个$之前的所有
					for (int i = 0; i < cStr.length();i++) {
						buf.push_back(cStr.at(i));
					}
					//buf是一个完整的命令了
					CommandGroupStruct gcmd;
					gcmd.DeSerialize(cStr.data());//反序列化
					this->group_command_queue.push(gcmd);//压入队列
					buf.clear();//清空之前的内容
					//然后接下来和处理之前是一样的了
					//查找下一个$的位置,从上一个+1的位置开始
					syntx_index = s1.find('$', syntx_index + 1);
					//让下次起始的位置是上一个的加1
					index= syntx_index+1;
					//开始找第二个
					while (syntx_index != s1.npos && index < s1.length()) //第一次完美执行
					{
						//从起点到'$'的位置之前
						cStr.clear();//清除之前的内容
						cStr = s1.substr(index, index);//把内容付给cStr
						CommandGroupStruct gcmd;
						gcmd.DeSerialize(cStr.data());//反序列化
						this->group_command_queue.push(gcmd);//压入队列
						//查找下一个$的位置,从上一个+1的位置开始
						syntx_index = s1.find('$', syntx_index + 1);
						//让下次起始的位置是上一个$的+1位置
						index = syntx_index + 1;
					}
					if (syntx_index == s1.npos) {
						//没找到$了，判断下index是不是在最后一个位置,是在最后一个位置，说明接到的数据是完整的n个cmd字符串
						if (index == s1.length() - 1) {
							//完整的，没跑了
							lastFinish = true;//完成！
						}
						else {
							//没找到了，但是index不是最后一个，说明命令不完整，cStr应该放在buf里面
							for (int i = 0; i < cStr.length(); i++) {
								buf.push_back(cStr.at(i));
							}
							lastFinish = false;//没有传完！
						}
					}
				}
			}
			//没有$
			else {
				//不管上次穿没穿玩，都应该放进去
				//这次应该放完整的命令串了
				for (int i = 0; i < s1.length();i++) {
					buf.push_back(s1.at(i));
				}
				lastFinish = false;//没有完成
			}
		}
		if (this->sleepTime > 0) {
			//休眠
			this_thread::sleep_for(std::chrono::milliseconds(this->sleepTime));
		}

	}
	return false;


}//从流中读取命令
bool ServerApiManager::init(){
	this->connected=this->connectToWebServer();
	if (connected) {
		printf("SERVERMANAGER_THREAD: connect to web successfully [ip:%s,port:%d]", this->ip.data(), this->port);
		this->start();//连接成功就开始start	
		return true;
		
	}
	else {
		
		printf("SERVERMANAGER_THREAD: connect to web failed [ip:%s,port:%d]", this->ip.data(), this->port);
		return false;
	
	}


}//初始化
bool ServerApiManager::connectToWebServer(){
	

	//用于记录错误信息并输出
	int iErrMsg;
	//启动WinSock
	WSAData wsaData;
	iErrMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (iErrMsg != NO_ERROR)
		//有错误
	{
		printf("SERVERMANAGER_THREAD: failed with wsaStartup error : %d\n", iErrMsg);

		
		return false;
	}
	//创建Socket
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//创建Socket失败
	{
		printf("SERVERMANAGER_THREAD: socket failed with error : %d\n", WSAGetLastError());

		return false;
	}
	//目标服务器数据
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	sockaddrServer.sin_port = this->port;
	sockaddrServer.sin_addr.s_addr = inet_addr(this->ip.data());

	//连接,sock与目标服务器连接
	iErrMsg = connect(m_sock, (sockaddr*)&sockaddrServer, sizeof(sockaddrServer));
	if (iErrMsg < 0)
	{
		printf("SERVERMANAGER_THREAD: connect failed with error : %d\n", iErrMsg);

	
		return false;
	}
	printf("SERVERMANAGER_THREAD: connect successfully! \n");
	return true;
}//连接远程数据库
ServerApiManager::ServerApiManager(string ip_, int port_, queue_safe<CommandGroupStruct>& gcmd_queue, int buf_size_,int sleepTime_):
	group_command_queue(gcmd_queue),
	ip(ip_),
	port(port_),
	buf_size(buf_size_),
	buf(),
	buf_vec(),
	sleepTime(sleepTime_)
	{




}
ServerApiManager::~ServerApiManager(){}//释放连接
void ServerApiManager::setIpAndPort(string ip_, int port) { 
	this->ip = ip_; 
	this->port = port; 
}//设置端口
bool ServerApiManager::isconnected() { return this->connected; }//是否连接上
void ServerApiManager::run() {
	getCommandstrFromStream();//永久的循环的
	printf("SERVERMANAGER_THREAD: error while getting command from web\n");
	return;
}