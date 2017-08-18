// #ifndef SERVERAPIMANAGER_H
// #define SERVERAPIMANAGER_H
// #include "common.h"
// 
#include <string>  
#include <iostream>
/*#include <boost/shared_ptr.hpp>*/
//#include <boost/asio.hpp>
// #include <boost/asio/placeholders.hpp>
// #include <boost/system/error_code.hpp>
// #include <boost/bind/bind.hpp>
// #include "queue_safe.h"
// using  boost::asio::ip::tcp;
// using  boost::asio::ip::address;
// using  namespace boost::asio;
// 
// class ServerApiManager {
// 	typedef ServerApiManager this_type;
// 	typedef ip::tcp::acceptor acceptor_type;
// 	typedef ip::tcp::endpoint endpoint_type;
// 	typedef ip::tcp::socket socket_type;
// 	typedef ip::address address_type;
// 	typedef boost::shared_ptr<socket_type> sock_ptr;
// 	typedef vector<char> buffer_type;
// private:
// 	queue_safe<CommandGroupStruct>& group_command_queue;
// 	string ip; //ip地址
// 	int port; //端口
// 	io_service m_io;
// 	buffer_type m_buf;
// 	endpoint_type m_ep;
// 	sock_ptr socket;//socket
// 	bool connected;//默认为假
// public:
// 	bool putIntoCommandQueue(CommandGroupStruct gcmd);//加入到commandqueue里面去
// 	string getCommandstrFromStream();//从流中读取命令
// 	bool init();//初始化
// 	bool connectToWebServer();//连接远程数据库
// 
// 	void async_conn_handler(const boost::system::error_code&ec, sock_ptr sock);//异步连接处理函数
// 	void async_reader_handler(const boost::system::error_code&ec, sock_ptr sock);//异步读取处理函数
// 	ServerApiManager(string ip_,int port_, queue_safe<CommandGroupStruct>& gcmd_queue);
// 	~ServerApiManager();//释放连接
// 	void setIpAndPort(string ip,int port);//设置端口
// 	bool isconnected();//是否连接上
// 
// };
// 
// 
// 
// #endif