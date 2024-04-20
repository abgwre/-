#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>  
#include <sys/socket.h> 
#include <string.h>
#include <arpa/inet.h>  
#include <pthread.h>    
#include <stdio.h>
#include "network.h"   
 
#define PORT 9999        //服务器端口
#define NOW_MAX 20 	 	 //最大允许并行客户端数量
 

 
int Socket_init (void); 
 
int Socket_Accept (int listen_socket); 
 
int Make_Server (void); 
 
void * handle_client (void* v);
 
int regis(int client_socket, Msg *msg);
 
int entry(int client_socket, Msg *msg);
 
void User_server(int client_socket, Msg *msg);   
 
void server_chatall(int client_socket, Msg * msg); 
 
void server_chatone(int client_socket, Msg * msg);
 
void server_transfer_file(int client_socket, Msg * msg);
 
void server_transfer_file_y(Msg * msg);
 
void server_transfer_file_n(Msg * msg);
 
void server_start_transfer_file(Msg * msg);

#endif
 