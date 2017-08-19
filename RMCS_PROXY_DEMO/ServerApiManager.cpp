#include "ServerApiManager.h"

bool ServerApiManager::getCommandstrFromStream(){
	bool lastFinish = true;
	while (true)
	{
		char* tempBuf = new char[this->buf_size];
		int num = recv(this->m_sock,tempBuf,this->buf_size,0);
		if (num == 0)return false;//出错了
		else {
			string s1;
			s1.insert(s1.end(), tempBuf, tempBuf + num - 1);
			//有$,没有的话就丢弃命令吧
			size_t d = s1.find('$');
			if (d != s1.npos) {
				//有,截取之前的
				string str=s1.substr(0, d);
				CommandGroupStruct gfd;
				gfd.SetPropertys();
				bool opt=gfd.DeSerialize(str.data());
				this->group_command_queue.push(gfd);
				printf("SERVERMANAGER_THREAD: get command !\n");

			}
			else {
				//什么都不做
				printf("SERVERMANAGER_THREAD: illeagl command\n", this->ip.data(), this->port);
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
		printf("SERVERMANAGER_THREAD: connect to web successfully [ip:%s,port:%d]\n", this->ip.data(), this->port);
		this->start();//连接成功就开始start	
		return true;
		
	}
	else {
		
		printf("SERVERMANAGER_THREAD: connect to web failed [ip:%s,port:%d]\n", this->ip.data(), this->port);
		return false;
	
	}


}//初始化
bool ServerApiManager::connectToWebServer(){
	

	//用于记录错误信息并输出
	int iErrMsg;
	//启动WinSock
// 	WSAData wsaData;
// 	iErrMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
// 	if (iErrMsg != NO_ERROR)
// 		//有错误
// 	{
// 		printf("SERVERMANAGER_THREAD: failed with wsaStartup error : %d\n，msg:%s", iErrMsg,WSAGetLastError());
// 
// 		
// 		return false;
// 	}
	//创建Socket
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//创建Socket失败
	{
		printf("SERVERMANAGER_THREAD: socket failed with error : %d\n", WSAGetLastError());
		WSACleanup();//释放套接字资源 
		return false;
	}
	//目标服务器数据
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	sockaddrServer.sin_port = htons((short)this->port);
	sockaddrServer.sin_addr.s_addr = inet_addr(this->ip.data());

	//连接,sock与目标服务器连接
	iErrMsg = connect(m_sock, (LPSOCKADDR)&sockaddrServer, sizeof(sockaddrServer));
	if (iErrMsg < 0)
	{
		printf("SERVERMANAGER_THREAD: connect failed with error : %d,msg:%d\n", iErrMsg, WSAGetLastError());

	
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
ServerApiManager::~ServerApiManager(){
	closesocket(m_sock); //关闭套接字  
	WSACleanup(); //释放套接字资源  
}//释放连接
void ServerApiManager::setIpAndPort(string ip_, int port) { 
	this->ip = ip_; 
	this->port = port; 
}//设置端口
bool ServerApiManager::isconnected() { return this->connected; }//是否连接上
void ServerApiManager::run() {
	try
	{
		getCommandstrFromStream();//永久的循环的
	}
	catch (const std::exception&)
	{

	}
	
	printf("SERVERMANAGER_THREAD: error while getting command from web\n");
	return;
}