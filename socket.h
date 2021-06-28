#ifndef SOCKET_H
#define SOCKET_H
#include <sys/types.h>
#define MYPORT 8888
#define BUFFER_SIZE 1024
#define IP_ADDR "192.168.1.66"
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
#ifdef __cplusplus
extern "C"{
#endif
int sock_cli;
struct sockaddr_in servaddr;
char recvbuf[BUFFER_SIZE];
void socket_init();
int mysend(char *str);

#ifdef __cplusplus    
}
#endif
#endif