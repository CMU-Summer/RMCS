#include "ServerApiManager.h"

bool ServerApiManager::getCommandstrFromStream(){
	bool lastFinish = true;
	while (true)
	{
		char* tempBuf = new char[this->buf_size];
		int num = recv(this->m_sock,tempBuf,this->buf_size,0);
		if (num == 0)return false;//������
		else {
			string s1(tempBuf);
			//��$
			if (s1.find('$') != s1.npos ) {
				//�У��ж����ϴδ���û��
				if (lastFinish) {
					//�����ˣ���ô��ε�ֱ��$֮ǰ�����µ�
					int syntx_index = s1.find('$');//��¼��һ��$��λ��
					int index = 0; //��¼ÿ�����������
					string cStr;//һ��������cmd�ַ���
					while (syntx_index !=s1.npos && index < s1.length() ) //��һ������ִ��
					{
						//����㵽'$'��λ��֮ǰ
						 cStr.clear();//���֮ǰ������
						 cStr=s1.substr(index, index);//�����ݸ���cStr
						 CommandGroupStruct gcmd;
						 gcmd.DeSerialize(cStr.data());//�����л�
						 this->group_command_queue.push(gcmd);//ѹ�����
						 //������һ��$��λ��,����һ��+1��λ�ÿ�ʼ
						 syntx_index = s1.find('$', syntx_index + 1);
						 //���´���ʼ��λ������һ��$��+1λ��
						 index= syntx_index+1;
					}
					if (syntx_index == s1.npos) {
						//û�ҵ�$�ˣ��ж���index�ǲ��������һ��λ��,�������һ��λ�ã�˵���ӵ���������������n��cmd�ַ���
						if (index == s1.length() - 1) {
							//�����ģ�û����
							lastFinish = true;//��ɣ�
						}
						else {
							//û�ҵ��ˣ�����index�������һ����˵�����������cStrӦ�÷���buf����
							for (int i = 0; i < cStr.length();i++) {
								buf.push_back(cStr.at(i));
							}
							lastFinish = false;//û�д��꣡
						}
					}

				}
				//�ϴ�û�д���
				else {
					//�Ǿ���˵���ҵ��ĵ�һ��λ�õ�$֮ǰ�ģ���������
					int syntx_index = s1.find('$');//��¼��һ��$��λ��
					int index = 0; //��¼ÿ�����������
					string cStr;//һ��������cmd�ַ���
					cStr = s1.substr(index, syntx_index);//����һ��$֮ǰ������
					for (int i = 0; i < cStr.length();i++) {
						buf.push_back(cStr.at(i));
					}
					//buf��һ��������������
					CommandGroupStruct gcmd;
					gcmd.DeSerialize(cStr.data());//�����л�
					this->group_command_queue.push(gcmd);//ѹ�����
					buf.clear();//���֮ǰ������
					//Ȼ��������ʹ���֮ǰ��һ������
					//������һ��$��λ��,����һ��+1��λ�ÿ�ʼ
					syntx_index = s1.find('$', syntx_index + 1);
					//���´���ʼ��λ������һ���ļ�1
					index= syntx_index+1;
					//��ʼ�ҵڶ���
					while (syntx_index != s1.npos && index < s1.length()) //��һ������ִ��
					{
						//����㵽'$'��λ��֮ǰ
						cStr.clear();//���֮ǰ������
						cStr = s1.substr(index, index);//�����ݸ���cStr
						CommandGroupStruct gcmd;
						gcmd.DeSerialize(cStr.data());//�����л�
						this->group_command_queue.push(gcmd);//ѹ�����
						//������һ��$��λ��,����һ��+1��λ�ÿ�ʼ
						syntx_index = s1.find('$', syntx_index + 1);
						//���´���ʼ��λ������һ��$��+1λ��
						index = syntx_index + 1;
					}
					if (syntx_index == s1.npos) {
						//û�ҵ�$�ˣ��ж���index�ǲ��������һ��λ��,�������һ��λ�ã�˵���ӵ���������������n��cmd�ַ���
						if (index == s1.length() - 1) {
							//�����ģ�û����
							lastFinish = true;//��ɣ�
						}
						else {
							//û�ҵ��ˣ�����index�������һ����˵�����������cStrӦ�÷���buf����
							for (int i = 0; i < cStr.length(); i++) {
								buf.push_back(cStr.at(i));
							}
							lastFinish = false;//û�д��꣡
						}
					}
				}
			}
			//û��$
			else {
				//�����ϴδ�û���棬��Ӧ�÷Ž�ȥ
				//���Ӧ�÷������������
				for (int i = 0; i < s1.length();i++) {
					buf.push_back(s1.at(i));
				}
				lastFinish = false;//û�����
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
		printf("SERVERMANAGER_THREAD: connect to web successfully [ip:%s,port:%d]", this->ip.data(), this->port);
		this->start();//���ӳɹ��Ϳ�ʼstart	
		return true;
		
	}
	else {
		
		printf("SERVERMANAGER_THREAD: connect to web failed [ip:%s,port:%d]", this->ip.data(), this->port);
		return false;
	
	}


}//��ʼ��
bool ServerApiManager::connectToWebServer(){
	

	//���ڼ�¼������Ϣ�����
	int iErrMsg;
	//����WinSock
	WSAData wsaData;
	iErrMsg = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (iErrMsg != NO_ERROR)
		//�д���
	{
		printf("SERVERMANAGER_THREAD: failed with wsaStartup error : %d\n", iErrMsg);

		
		return false;
	}
	//����Socket
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//����Socketʧ��
	{
		printf("SERVERMANAGER_THREAD: socket failed with error : %d\n", WSAGetLastError());

		return false;
	}
	//Ŀ�����������
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	sockaddrServer.sin_port = this->port;
	sockaddrServer.sin_addr.s_addr = inet_addr(this->ip.data());

	//����,sock��Ŀ�����������
	iErrMsg = connect(m_sock, (sockaddr*)&sockaddrServer, sizeof(sockaddrServer));
	if (iErrMsg < 0)
	{
		printf("SERVERMANAGER_THREAD: connect failed with error : %d\n", iErrMsg);

	
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
ServerApiManager::~ServerApiManager(){}//�ͷ�����
void ServerApiManager::setIpAndPort(string ip_, int port) { 
	this->ip = ip_; 
	this->port = port; 
}//���ö˿�
bool ServerApiManager::isconnected() { return this->connected; }//�Ƿ�������
void ServerApiManager::run() {
	getCommandstrFromStream();//���õ�ѭ����
	printf("SERVERMANAGER_THREAD: error while getting command from web\n");
	return;
}