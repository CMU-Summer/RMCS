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
// 	string ip; //ip��ַ
// 	int port; //�˿�
// 	io_service m_io;
// 	buffer_type m_buf;
// 	endpoint_type m_ep;
// 	sock_ptr socket;//socket
// 	bool connected;//Ĭ��Ϊ��
// public:
// 	bool putIntoCommandQueue(CommandGroupStruct gcmd);//���뵽commandqueue����ȥ
// 	string getCommandstrFromStream();//�����ж�ȡ����
// 	bool init();//��ʼ��
// 	bool connectToWebServer();//����Զ�����ݿ�
// 
// 	void async_conn_handler(const boost::system::error_code&ec, sock_ptr sock);//�첽���Ӵ�����
// 	void async_reader_handler(const boost::system::error_code&ec, sock_ptr sock);//�첽��ȡ������
// 	ServerApiManager(string ip_,int port_, queue_safe<CommandGroupStruct>& gcmd_queue);
// 	~ServerApiManager();//�ͷ�����
// 	void setIpAndPort(string ip,int port);//���ö˿�
// 	bool isconnected();//�Ƿ�������
// 
// };
// 
// 
// 
// #endif