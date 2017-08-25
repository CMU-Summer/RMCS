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
// 	void init();//初始化
// 	bool connect();//连接函数
// 	void async_read_handler();//异步处理函数
// 	void close();
// 	~ServerListener();//析构函数
// 	CommandGroupStruct getCommandGroupFromJson(string jsonStr);//获得jsonstr
// };
// 
// 
// 
// 
// #endif