// #ifndef  SERVERLISTENERAPI_H
// #define  SERVERLISTENERAPI_H
// #include <ws_client.h>
// #include "common.h"
// #include "queue_safe.h"
// using namespace web::websockets::client;
// class ServerListener {
// private:
// 	queue_safe<CommandGroupStruct>& group_command_queue;
// 	websocket_client  client;
// public:
// 
// 	
// 	ServerListener(queue_safe<CommandGroupStruct>& group_command_queue);
// 	ServerListener(queue_safe<CommandGroupStruct>& group_command_queue, websocket_client_config config);
// 	void init();//��ʼ��
// 	bool connect();//���Ӻ���
// 	void async_read_handler();//�첽������
// 	void close();
// 	~ServerListener();//��������
// 	CommandGroupStruct getCommandGroupFromJson(string jsonStr);//���jsonstr
// };
// 
// 
// 
// 
// #endif