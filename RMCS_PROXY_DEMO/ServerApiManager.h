#ifndef SERVERAPIMANAGER_H
#define SERVERAPIMANAGER_H
#include "common.h"
#include "CThread.h"
#include <iostream>
#include<boost/asio/io_service.hpp>  
#include<boost/asio/ip/tcp.hpp>  
#include<boost/bind.hpp>  
#include<boost/shared_ptr.hpp>  
#include<boost/enable_shared_from_this.hpp>  
#include<string>  
#include<boost/asio/streambuf.hpp>  
#include<boost/asio/placeholders.hpp>  
#include<boost/asio.hpp>  

class ServerApiManager:public CThread {
public:
	bool putIntoCommandQueue(CommandGroupStruct gcmd);//���뵽commandqueue����ȥ
	string getCommandstrFromStream();//�����ж�ȡ����
	bool init();//��ʼ��
	bool connectToWebServer();//����Զ�����ݿ�

	virtual void run() override;



};



#endif