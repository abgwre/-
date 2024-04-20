#ifndef __NETWORK_H__
#define __NETWORK_H__
 

#include <stdio.h>
#include <time.h>
#include <string.h>
 
#define SIZE 1024
 
typedef struct msg          
{
	char msg[SIZE];   		//消息内容
	char localip[20];		//消息目的IP
    int localport;        //消息目的端口
	char fromip[20];      //消息来源IP
    int fromport;        //消息目的端口
	char signname[40];      //传输文件名   
	int cmd;                //工作模式
	int num;                //用于保存文件复制的字节数
}Msg;
 
typedef struct now_client   //在线人数
{
	char IP[20];
    int port;
	int socket;
}NowClient; 
 
#endif
 