#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero

#define SERVER_PORT 6666
#define LISTEN_QUEUE  20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

int main(int argc, char **argv)
{
    //设置一个socket地址结构server_addr,代表服务器internet地址, 端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr)); //把一段内存区的内容全部设置为0
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    //创建用于internet的流协议(TCP)socket,用server_socket代表服务器socket
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        printf("Create Socket Failed!");
        exit(1);
    }

    //把socket和socket地址结构联系起来
    if (bind(server_socket, (struct sockaddr*) &server_addr,
            sizeof(server_addr)))
    {
        printf("Server Bind Port: %d Failed!\n", SERVER_PORT);
        exit(1);
    }

    //server_socket用于监听
    if (listen(server_socket, LISTEN_QUEUE))
    {
        printf("Server Listen Failed!");
        exit(1);
    }

    while (1)
    {
        //定义客户端的socket地址结构client_addr
        char buffer[BUFFER_SIZE];
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        int client_socket = accept(server_socket,
                (struct sockaddr*) &client_addr, &length);
        if (client_socket < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }
        bzero(buffer, BUFFER_SIZE);
        // 获取客户端要传输的文件名
        length = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (length < 0)
        {
            printf("Server Recieve Data Failed!\n");
            break;
        }
        char file_name[FILE_NAME_MAX_SIZE + 1];
        bzero(file_name, FILE_NAME_MAX_SIZE + 1);
        strncpy(
                file_name,
                buffer,
                strlen(buffer) > FILE_NAME_MAX_SIZE? FILE_NAME_MAX_SIZE : strlen(buffer));

        // 新建文件
        FILE * fp = fopen(file_name, "w");
        if (NULL == fp)
        {
            printf("File: %s CAN NOT WRITE!\n", file_name);
        }
        else
        {
            bzero(buffer, BUFFER_SIZE);
            int file_block_length = 0;
            while ((file_block_length = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
            {
                if (file_block_length < 0)
                {
                    printf("Recieve Data From Client Failed!\n");
                    break;
                }
                int write_length = fwrite(buffer, sizeof(char), file_block_length, fp);
                if (write_length < file_block_length)
                {
                    printf("File: %s Write Failed\n", file_name);
                    break;
                }
                bzero(buffer, BUFFER_SIZE);
            }
            fclose(fp);
            printf("File: %s Transfer Finished\n\n", file_name);
        }
        //关闭与客户端的连接
        close(client_socket);
    }
    //关闭监听用的socket
    close(server_socket);
    return 0;
}