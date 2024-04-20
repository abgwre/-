#include "server.h"
NowClient user[NOW_MAX];    //表示当前在线人的资料

int Socket_init (void)
{
	//创建
	int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_socket == -1)
	{
		perror("socket error");
		printf("套接字创建失败.\n");
		return -1;
	}
	//绑定
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  			
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);      			
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int ret = bind(listen_socket, (struct sockaddr*)&(addr), sizeof(addr));
	if(ret == -1)
	{
		perror("bind error");
		printf("绑定套接字失败.\n");
		return -1;
	}
	//监听
	ret = listen(listen_socket, 3);      
	if(ret == -1)
	{
		perror("listen error");
		printf("监听套接字失败.\n");
		return -1;
	}
	
	printf("等待用户连接.....\n");
	
	return listen_socket;
}

int Socket_Accept (int listen_socket)
{
	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	int client_socket = accept(listen_socket, 
				(struct sockaddr*)&(client_addr), &len);
	if(client_socket == -1)
	{
		perror("accept error");
		printf("链接客户端失败\n");
		return -1;
	}
	
	printf("成功链接了一个客户端 : %s\n"), inet_ntoa(client_addr.sin_addr);
	
	return client_socket;
}

void * handle_client (void* v) 
{
	int client_socket = (int)v;
	Msg msg;
	//从客户端读取数据
	int ret = read(client_socket, &msg, sizeof(msg));
	if (ret == -1)
	{
		perror ("read error ");
		return;
	}
	// 客户端退出
	if (ret == 0)
	{
		printf ("客户端退出\n");
		return;
	}

	int i;
	for (i=0; i<NOW_MAX; i++)
	{
		if(user[i].socket == 0)
		{
			strcpy(user[i].IP, msg.fromip);
			user[i].port = msg.fromport;
			user[i].socket = client_socket;
			printf("客户端在线人数加一\n");
			break;
		}
	}
	//用户界面
	User_server(client_socket, &msg);  
	
		
	
}

void User_server(int client_socket, Msg *msg)
{
	int j = 1;      //表示循环退出条件
	while(j)
	{
		//从客户端读取数据
		int ret = read(client_socket, msg, sizeof(Msg));
		
		if (ret == -1)
		{
			perror ("read");
			break;
		}
		// 代表客户端退出
		if (ret == 0)
		{
			//printf ("客户端退出\n");
			break;
		}
		
		switch (msg->cmd)
		{
			case 1 :    //私聊
				server_chatone(client_socket, msg);	
				break;
			
			case 2 :   //请求传输文件
				server_transfer_file(client_socket, msg);
				break;
			case 3:   //接受传输文件
				server_transfer_file_y(msg);
				break;
			case -3 : //拒绝传输文件
				server_transfer_file_n(msg);
				break;
			case 4:   //一切条件都已成立,直接开始传输
				server_start_transfer_file(msg);
		}
	}
	
	//用户下线
	int i ;
	for(i=0; i<NOW_MAX; i++)
	{
		if(user[i].socket == client_socket)
		{
			user[i].socket = 0;
			printf("客户端在线人数减一\n");
			break;
		}
	}
}

void server_chatone(int client_socket, Msg * msg)
{
	
	int i;
	for(i=0; i<NOW_MAX; i++)
	{
		
		if(user[i].socket != 0 && strncmp(user[i].IP, msg->localip, strlen(msg->localip)) == 0 && user[i].port == msg->localport)
		{
			write(user[i].socket, msg, sizeof(Msg));
			printf("私聊成功\n");
			break;
		}
	}
	if (i == NOW_MAX)
	{
		msg->cmd = -1;    //表示私聊失败
		write(client_socket, msg, sizeof(Msg));
		printf("私聊失败\n");
	}
}

void server_transfer_file(int client_socket, Msg * msg)
{
	printf ("%s:%d 请求发送%s文件给 %s:%d\n", msg->fromip, msg->fromport, msg->signname, msg->localip, msg->localport);
	int i;
	for(i=0; i<NOW_MAX; i++)
	{
		if(user[i].socket != 0 && strncmp(user[i].IP, msg->localip, strlen(msg->localip)) == 0 && user[i].port == msg->localport)
		{
			write(user[i].socket, msg, sizeof(Msg));
			printf("发送给%s:%d信息进行判断是否接受\n", msg->localip, msg->localport);
			break;
		}
	}
	if (i == NOW_MAX)
	{
		msg->cmd = -2;    //表示传输文件失败
		write(client_socket, msg, sizeof(Msg));
		printf("发送文件失败,好友不在线或不存在\n");
	}
}
 
//接受文件
void server_transfer_file_y(Msg * msg)
{
	printf("%s:%d 接受文件传输.\n", msg->fromip, msg->fromport);
	
	int i;
	for(i=0; i<NOW_MAX; i++)
	{
		if(user[i].socket != 0 && strncmp(user[i].IP, msg->localip, strlen(msg->localip)) == 0 && user[i].port == msg->localport)
		{
			write(user[i].socket, msg, sizeof(Msg));
			break;
		}
	}
}
 
//拒绝文件
void server_transfer_file_n(Msg * msg)
{
	printf("%s:%d 不愿意接受文件传输.\n", msg->fromip, msg->fromport);
	
	int i;
	for(i=0; i<NOW_MAX; i++)
	{
		if(user[i].socket != 0 && strncmp(user[i].IP, msg->localip, strlen(msg->localip)) == 0 && user[i].port == msg->localport)
		{
			write(user[i].socket, msg, sizeof(Msg));
			break;
		}
	}
}
 
//一切条件都已成立,直接开始传输
void server_start_transfer_file(Msg * msg)
{
	//printf("文件传输中\n");
	int i;
	for(i=0; i<NOW_MAX; i++)
	{
		if(user[i].socket != 0 && strncmp(user[i].IP, msg->localip, strlen(msg->localip)) == 0 && user[i].port == msg->localport)
		{
			write(user[i].socket, msg, sizeof(Msg));    //写文件数据
			break;
		}
	}
	if(msg->num != 1024)
	{
		printf("文件传输完成\n");
	}
}

int main(int argc, char** argv)
{
	
	int listen_socket = Socket_init();
	if(listen_socket == -1)
	{
		printf("%s", "Socket_init error");
		return -1;
	}
	
	while(1)
	{
		//链接客户端
		int client_socket = Socket_Accept(listen_socket);
		if(listen_socket == -1)
		{
			printf("%s", "Socket_Accept error");
			return -1;
		}
		
		//创建进程处理客户端
		pthread_t client_id;
		int ret = pthread_create(&client_id, NULL, handle_client, (void*)client_socket);
		if(ret != 0)
		{
			perror("pthread_create error");
			return -1;
		}
		pthread_detach(client_id); // 线程分离
	}
	
	close (listen_socket);
	return 0;
}