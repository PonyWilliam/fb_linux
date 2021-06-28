#include "socket.h"
void socket_init(){
    printf("对方IP:%s,PORT:%d\n",IP_ADDR,MYPORT);
    memset(&servaddr,0,sizeof(servaddr));
    sock_cli = socket(AF_INET,SOCK_STREAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);
    servaddr.sin_addr.s_addr = inet_addr(IP_ADDR);
    if(connect(sock_cli,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        perror("connect error");
        exit(1);
    }
    printf("socket connect success");
    memset(recvbuf,0,sizeof(recvbuf));
    printf("监听接收服务端\n");
    // while(recv(sock_cli,recvbuf,1024,0))
    //     printf("读取到服务端消息:%s\n",recvbuf);
    //     system("");
    //     memset(recvbuf,0,sizeof(recvbuf));
}
int mysend(char *str){
    return write(sock_cli,str,sizeof(str));
}