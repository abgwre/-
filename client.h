#ifndef __CLIENT_H__

#define __CLIENT_H__

 

#define PORT 9999 

 

#include <sys/types.h>  
#include <sys/socket.h> 
#include <string.h>
#include <arpa/inet.h> 
#include <stdio.h>
#include <sys/stat.h>  
#include <fcntl.h>		
#include <unistd.h>   
#include <stdlib.h>
#include <strings.h>
#include "network.h" 


#define BACKSPACE 127 


int Connect_Server(void);

void handle_password(char *pass);
 
int Ask_server(int client_socket);

void menu(void);

void User_used(int client_socket);
 
void * readMsg (void *v);

void chat_one(int client_socket);

void transfer_file(int client_socket);

void transfer_file_y(int client_socket);
 
void transfer_file_n(int client_socket);

void start_transfer_file(int client_socket);
 
void save_transfer_file(Msg * buf);

int isIP(char *IP);


#endif

 

 