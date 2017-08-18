// #include "ServerApiManager.h"
// 
// 
// 
// bool ServerApiManager::putIntoCommandQueue(CommandGroupStruct gcmd){
// 	return false;
// 
// }//加入到commandqueue里面去
// string ServerApiManager::getCommandstrFromStream(){
// 
// 	return "";
// }//从流中读取命令
// bool ServerApiManager::init(){
// 	return this->connectToWebServer();
// 	
// 
// }//初始化
// bool ServerApiManager::connectToWebServer(){
// 	ServerApiManager::sock_ptr sock(new socket_type(this->m_io));
// 	this->socket = sock;
// 	this->socket->async_connect(m_ep, boost::bind(&ServerApiManager::async_conn_handler, this, boost::asio::placeholders::error, sock));
// 	return true;
// 
// }//连接远程数据库
// 
// void ServerApiManager::async_conn_handler(const boost::system::error_code&ec, sock_ptr sock){
// 	if (ec)
// 	{
// 		//连接失败
// 		printf("SERVER__API__MANAGER: connect to Web Server [ip:%s,port:%d ] failed.msg: [ %s ]/n", this->ip, this->port, ec.message().data());
// 		this->connected = false;
// 		return ;
// 	}
// 	//连接成功
// 	printf("SERVER__API__MANAGER: connect to Web Server [ip:%s,port:%d ] successfully!!/n",this->ip,this->port);
// 	//异步读取
// 	sock->async_read_some(buffer(m_buf), boost::bind(&ServerApiManager::async_reader_handler, this, boost::asio::placeholders::error, sock));
// 	this->connected = true;
// }//异步连接处理函数
// void ServerApiManager::async_reader_handler(const boost::system::error_code&ec, sock_ptr sock) {
// 	if (ec)
// 	{	//读取发生错误
// 		printf("SERVER__API__MANAGER: some error when getting command,msg: [ %s ]/n",ec.message().data());
// 		return;
// 	}
// 	//下一次的异步??不是绑定了吗
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
// }//释放连接
// void ServerApiManager::setIpAndPort(string ip, int port){
// 	this->ip = ip;
// 	this->port = port;
// 
// 
// 
// }//设置端口
// bool ServerApiManager::isconnected() { return this->connected; }