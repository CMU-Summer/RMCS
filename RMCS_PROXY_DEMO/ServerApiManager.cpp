// #include "ServerApiManager.h"
// 
// 
// 
// bool ServerApiManager::putIntoCommandQueue(CommandGroupStruct gcmd){
// 	return false;
// 
// }//���뵽commandqueue����ȥ
// string ServerApiManager::getCommandstrFromStream(){
// 
// 	return "";
// }//�����ж�ȡ����
// bool ServerApiManager::init(){
// 	return this->connectToWebServer();
// 	
// 
// }//��ʼ��
// bool ServerApiManager::connectToWebServer(){
// 	ServerApiManager::sock_ptr sock(new socket_type(this->m_io));
// 	this->socket = sock;
// 	this->socket->async_connect(m_ep, boost::bind(&ServerApiManager::async_conn_handler, this, boost::asio::placeholders::error, sock));
// 	return true;
// 
// }//����Զ�����ݿ�
// 
// void ServerApiManager::async_conn_handler(const boost::system::error_code&ec, sock_ptr sock){
// 	if (ec)
// 	{
// 		//����ʧ��
// 		printf("SERVER__API__MANAGER: connect to Web Server [ip:%s,port:%d ] failed.msg: [ %s ]/n", this->ip, this->port, ec.message().data());
// 		this->connected = false;
// 		return ;
// 	}
// 	//���ӳɹ�
// 	printf("SERVER__API__MANAGER: connect to Web Server [ip:%s,port:%d ] successfully!!/n",this->ip,this->port);
// 	//�첽��ȡ
// 	sock->async_read_some(buffer(m_buf), boost::bind(&ServerApiManager::async_reader_handler, this, boost::asio::placeholders::error, sock));
// 	this->connected = true;
// }//�첽���Ӵ�����
// void ServerApiManager::async_reader_handler(const boost::system::error_code&ec, sock_ptr sock) {
// 	if (ec)
// 	{	//��ȡ��������
// 		printf("SERVER__API__MANAGER: some error when getting command,msg: [ %s ]/n",ec.message().data());
// 		return;
// 	}
// 	//��һ�ε��첽??���ǰ�����
// 	//sock->async_receive(buffer(m_buf), boost::bind(&ServerApiManager::async_reader_handler, this, boost::asio::placeholders::error, sock));
// 	string s;
// 	s.insert(s.begin(), m_buf.begin(), m_buf.end());
// 	printf("SERVER__API__MANAGER: revice cmd :[%s]",s);
// 
// 
// }
// ServerApiManager::ServerApiManager(string ip_, int port_, queue_safe<CommandGroupStruct>& gcmd_queue):
// 	ip(ip_),
// 	port(port_),
// 	group_command_queue(gcmd_queue),
// 	m_ep(address_type::from_string(ip_),port_)
// 
// {
// 
// 
// }
// ServerApiManager::~ServerApiManager(){
// 
// 
// }//�ͷ�����
// void ServerApiManager::setIpAndPort(string ip, int port){
// 	this->ip = ip;
// 	this->port = port;
// 
// 
// 
// }//���ö˿�
// bool ServerApiManager::isconnected() { return this->connected; }