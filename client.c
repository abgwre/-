#include "client.h"


char IP[20];    		//保存本地IP
int port;				//保存本地端口
char signname[40];  	//保存文件名
char myip[20]; 			//保存传输文件来源IP
int myport;				//保存传输文件来源端口
char fsignname[40]; 	//保存文件传输名字



int Connect_Server(void)
{
	//创建套接字
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket == -1)
	{
		perror("socket error");
		return -1;
	}
	//链接服务器
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	inet_aton("127.0.0.1", &(addr.sin_addr));
	
	int ret = connect(client_socket,(struct sockaddr *)&addr, sizeof(addr));
	if(ret == -1)
	{
		perror("connect error");
		return -1;
	}
	printf("成功连接到服务器 : %s\n", inet_ntoa(addr.sin_addr));
	
	return client_socket;
}

//主界面

void menu(void)
{
	system("clear");
	printf("\n\n\n\n\n\n\n");
	printf("++++++++++++++++++++++++++++++++++++++++\n");
	printf("+++                                  +++\n");
	printf("+++\t   欢迎使用聊天程序          +++\n");
	printf("+++                                  +++\n");
	printf("++++++++++++++++++++++++++++++++++++++++\n");
	printf("+++\t\t1,私聊               +++\n");
	printf("+++\t\t2,传输文件           +++\n");
	printf("++++++++++++++++++++++++++++++++++++++++\n");
}


void User_used(int client_socket)
{
	Msg msg;

	strcpy(msg.fromip, IP);
	msg.fromport = port;
	write(client_socket, &msg, sizeof(Msg));
	pthread_t read_id;
	pthread_create(&read_id, NULL, readMsg, (void *)client_socket);
	pthread_detach(read_id);   //等待线程分离
	
	char ch;
	int i = 1;
	while(i)
	{
		menu();
		printf("请输入您要做的操作\n");
		scanf("%c", &ch);
		while(getchar() != '\n');
		switch(ch)
		{
			case '1':        //私聊
				chat_one(client_socket);
				break;
			case '2':        //传输文件
				transfer_file(client_socket);
				break;
			case 'y':        //表示愿意接受文件
				transfer_file_y(client_socket);
				break;
			case 'n':        //表示不愿意接受文件
				transfer_file_n(client_socket);
				break;		
		}
	}
}

void * readMsg (void *v)
{
	int client_socket = (int)v;
	Msg buf;
	int i = 0;
	while(1)
	{
		bzero(&buf, sizeof(buf));
		int ret = read(client_socket, &buf, sizeof(Msg));
		if(ret == -1)
		{
			perror("read");
			break;
		}
		switch(buf.cmd)
		{
			
			case 1:   //私聊
				printf("%s:%d 给你发了一条消息: %s\n", buf.fromip, buf.fromport, buf.msg);
				break;
			case -1:  //私聊失败
				printf("私聊失败,用户不存在或下线\n");
				break;
			case 2:   //调用函数确认是否接受文件
				system("clear");
				printf("请问你是否接受来自 %s:%d 的文件 %s(y/n)\n", buf.fromip, buf.fromport, buf.signname);
				strcpy(myip,buf.fromip);   //保存传输文件来源名字
				myport = buf.fromport;
				sleep(1);
				break;
			case -2:   //表示传输文件失败,没有找到该人
				printf("发送文件失败,好友不在线或不存在\n");
				break;
			case 3:   //表示愿意接受文件,开始传输
				start_transfer_file(client_socket);
				break;
			case -3:  //表示不愿意接受文件
				printf("发送文件失败,后又拒绝接受文件\n");
				break;
			case 4:   //接受文件
				save_transfer_file(&buf);
				i++;
				break;
			default:break;
		}
 
	}
}

void chat_one(int client_socket)
 {
	Msg msg;
	msg.cmd = 1;

	printf("请输入聊天对象IP地址:\n");
	scanf ("%s", msg.localip);
	while(getchar() != '\n');

	printf("请输入聊天对象端口号:\n");
	scanf ("%d", &msg.localport);

	printf("请输入要发送的内容: \n");
	scanf("%s", msg.msg);
	while(getchar() != '\n');
	
	strcpy (msg.fromip, IP);
	msg.fromport = port;
	write(client_socket, &msg, sizeof(Msg));
 	
	sleep(2);
}

void transfer_file(int client_socket)
{
	Msg msg;
	msg.cmd = 2;
	printf("请输入你要传输文件的IP地址:");
	scanf ("%s", msg.localip);
	while(getchar() != '\n');

	printf("请输入你要传输文件的端口号:");
	scanf ("%d", &msg.localport);
	while(getchar() != '\n');
	
	printf("请输入你要传输的本地文件名:");
	scanf ("%s", msg.signname);
	while(getchar() != '\n');
	
	strcpy(msg.fromip, IP);
	msg.fromport = port;
	write(client_socket, &msg, sizeof(Msg));
	
	printf("等待验收中...\n");
	
	strcpy(fsignname, msg.signname);
	strcpy(myip, msg.localip);
	myport = msg.localport;
	
	sleep(2);
}

//表示愿意接受文件
void transfer_file_y(int client_socket)
{
	Msg msg;
	msg.cmd = 3;
	strcpy(msg.fromip, IP); 
	msg.fromport = port;
	strcpy(msg.localip, myip); 
	msg.localport = myport;
	strcpy(myip, "\0");    //用完后置零
	myport = 0;
	write(client_socket, &msg, sizeof(Msg));
}
 
//表示不愿意接受文件
void transfer_file_n(int client_socket)
{
	Msg msg;
	msg.cmd = -3;
	strcpy(msg.fromip, IP);
	msg.fromport = port;
	strcpy(msg.localip, myip); 
	strcpy(myip, "\0"); 
	write(client_socket, &msg, sizeof(Msg));
}
 
//传出文件来源开始传输文件
void start_transfer_file(int client_socket)
{
	Msg msg;
	msg.cmd = 4; 
	strcpy(msg.fromip, IP); 
	msg.fromport = port;
	strcpy(msg.signname, fsignname);
	strcpy(msg.localip, myip);
	msg.localport = myport;
	
	int fd = open(msg.signname, O_RDONLY); 
	if(fd == -1)
	{
		perror("open");
		printf("传输失败\n");
		return ;
	}
	
	int ret = 0;
	int i = 0;
	while(ret = read(fd, msg.msg, SIZE))
	{
		if(ret == -1)
		{
			perror("read");
			break;
		}
		if (ret == 0)
		{
			break;
		}
		msg.num = ret;
		write(client_socket, &msg, sizeof(Msg));
		usleep(10000);     //减缓传输速度,降低不可预祝错误出现
	}
	printf("文件复制完成传输\n");
	strcpy(fsignname, "\0");
	strcpy(myip, "\0");
	myport = 0;
	
	close(fd);
}
 
void save_transfer_file(Msg * buf)
{
	int fd = open(buf->signname, O_WRONLY|O_CREAT|O_APPEND, 0777);
	write(fd, buf->msg, buf->num);
	if(buf->num != SIZE)
	{
		printf ("文件接受完成\n");
	}
	close (fd);
} 





int isIP(char *IP){
	int n = strlen(IP);
	int np = 0;
	int num = 0;
	for(int i = 0; i <= n; i++){
		if(IP[i] == '.' || i == n){
			np++;
			if(num > 255) return 0;
			num = 0;
		}else if(IP[i]>='0' && IP[i]<='9')
			num = num * 10 + IP[i] - '0';
		else return 0;
	}
	if(np == 4) return 1;
	else return 0;
}





int main(int argc, char** argv){
	// 输入IP地址、端口，判断是否为正确IP地址和端口，直到输入正确
	char opt;

	while( (opt = getopt(argc, argv, "p:i:")) != EOF) {
		switch(opt) {
		case 'p':
			port = (int) atoi(optarg);
			break;
        case 'i':
            strcpy(IP, optarg);
            break;
		default:
			printf("Usage: %s -p port\n", argv[0]);
			return -1;
		}
	}

	if (port == 0) {
		printf("Invalid port number, try again. \n");
		printf("Usage: %s -p port -i IP\n", argv[0]);
		return -1;
	}

	if(!(isIP(IP))){
		printf("Invalid IP, try again. \n");
		printf("Usage: %s -p port -i IP\n", argv[0]);
		return -1;
	}
    
	int client_socket = Connect_Server();
	
	User_used(client_socket);    //调用函数表示用户界面
	
	
}