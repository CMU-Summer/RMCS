#include "ServerApiManager.h"

bool ServerApiManager::getCommandstrFromStream(){
	bool lastFinish = true;
	while (true)
	{
		char* tempBuf = new char[this->buf_size];
		int num = recv(this->m_sock,tempBuf,this->buf_size,0);
		if (num == 0)return false;//������
		else {
			string s1;
			s1.insert(s1.end(), tempBuf, tempBuf + num - 1);
			//��$,û�еĻ��Ͷ��������
			size_t d = s1.find('$');
			if (d != s1.npos) {
				//��,��ȡ֮ǰ��
				string str=s1.substr(0, d);
				CommandGroupStruct gfd;
				gfd.SetPropertys();
				bool opt=gfd.DeSerialize(str.data());
				this->group_command_queue.push(gfd);
				printf("SERVERMANAGER_THREAD: get command !\n");

			}
			else {
				//ʲô������
				printf("SERVERMANAGER_THREAD: illeagl command\n", this->ip.data(), this->port);
			}
		}
		if (this->sleepTime > 0) {
			//����
			this_thread::sleep_for(std::chrono::milliseconds(this->sleepTime));
		}

	}
	return false;


}//�����ж�ȡ����
bool ServerApiManager::init(){
	this->connected=this->connectToWebServer();
	if (connected) {
		printf("SERVERMANAGER_THREAD: connect to web successfully [ip:%s,port:%d]\n", this->ip.data(), this->port);
		this->start();//���ӳɹ��Ϳ�ʼstart	
		return true;
		
	}
	else {
		
		printf("SERVERMANAGER_THREAD: connect to web failed [ip:%s,port:%d]\n", this->ip.data(), this->port);
		return false;
	
	}


}//��ʼ��
bool ServerApiManager::connectToWebServer(){
	

	//���ڼ�¼������Ϣ�����
	int iErrMsg;
	//����WinSock
// 	WSAData wsaData;
// 	iErrMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
// 	if (iErrMsg != NO_ERROR)
// 		//�д���
// 	{
// 		printf("SERVERMANAGER_THREAD: failed with wsaStartup error : %d\n��msg:%s", iErrMsg,WSAGetLastError());
// 
// 		
// 		return false;
// 	}
	//����Socket
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//����Socketʧ��
	{
		printf("SERVERMANAGER_THREAD: socket failed with error : %d\n", WSAGetLastError());
		WSACleanup();//�ͷ��׽�����Դ 
		return false;
	}
	//Ŀ�����������
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	sockaddrServer.sin_port = htons((short)this->port);
	sockaddrServer.sin_addr.s_addr = inet_addr(this->ip.data());

	//����,sock��Ŀ�����������
	iErrMsg = connect(m_sock, (LPSOCKADDR)&sockaddrServer, sizeof(sockaddrServer));
	if (iErrMsg < 0)
	{
		printf("SERVERMANAGER_THREAD: connect failed with error : %d,msg:%d\n", iErrMsg, WSAGetLastError());

	
		return false;
	}
	printf("SERVERMANAGER_THREAD: connect successfully! \n");
	return true;
}//����Զ�����ݿ�
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
	closesocket(m_sock); //�ر��׽���  
	WSACleanup(); //�ͷ��׽�����Դ  
}//�ͷ�����
void ServerApiManager::setIpAndPort(string ip_, int port) { 
	this->ip = ip_; 
	this->port = port; 
}//���ö˿�
bool ServerApiManager::isconnected() { return this->connected; }//�Ƿ�������
void ServerApiManager::run() {
	try
	{
		getCommandstrFromStream();//���õ�ѭ����
	}
	catch (const std::exception&)
	{

	}
	
	printf("SERVERMANAGER_THREAD: error while getting command from web\n");
	return;
}