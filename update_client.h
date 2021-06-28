#ifndef UPDATE_H
#define UPDATE_H
#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
/*
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>
 */

#define SERVER_PORT 6666
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
#define IP_ADDR "192.168.0.1"
int client_socket;

struct sockaddr_in client_addr;
#ifdef __cplusplus
extern "C"{
#endif
void Socket_init();
void SendFile(char* filename);
#ifdef __cplusplus    
}
#endif

#endif