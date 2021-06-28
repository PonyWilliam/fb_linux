#include "update_client.h"
void Socket_init(){
    //初始化socket连接，等待连接
    bzero(&client_addr, sizeof(client_addr)); //把一段内存区的内容全部设置为0
    client_addr.sin_family = AF_INET; //internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY); //INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0); //0表示让系统自动分配一个空闲端口
    //创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
    //把客户机的socket和客户机的socket地址结构联系起来
    if (bind(client_socket, (struct sockaddr*) &client_addr,
            sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }
    //设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (inet_aton(IP_ADDR, &server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
    {
        printf("Server IP Address Error! \n");
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);
    // 向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if (connect(client_socket, (struct sockaddr*) &server_addr,
            server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", IP_ADDR);
        exit(1);
    }
}
void SendFile(char* filename){
    char buffer[BUFFER_SIZE];
    int n;
        FILE * fp = fopen(filename, "r");
        if (NULL == fp)
        {
            printf("File: %s NOT FOUND! \n", "1.pcm");
            exit(1);
        }
        int file_block_length = 0;

        while ((file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp))
                > 0)
        {
            if (send(client_socket, buffer, file_block_length, 0) < 0)
            {
                printf("Send File:\t%s Failed\n", "1.pcm");
                break;
            }
            bzero(buffer, BUFFER_SIZE);
        }
        send(client_socket,"myend666",sizeof("myend666"),0);
        
        printf("File:\t%s Transfer Finished\n", "1.pcm");
        fclose(fp);
}