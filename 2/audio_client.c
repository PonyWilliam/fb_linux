/***********************
名称： 语音识别系统开发板客户端
平台： gec6818
作者： 化作尘
时间：2020年9月25日09:14:58
功能：语音识别
依赖： alsa库 iconv库 libfont.a
邮箱:2809786963@qq.com
bilibili项目展示视频：https://www.bilibili.com/video/BV1Dv411C7cZ/
**************************/




#include <stddef.h>	// size_t
#include <stdio.h>	// fopen()
#include <string.h>	// strlen()
#include <getopt.h>
#include "../include/libbase64.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <libbase64.h>
#include <sys/ioctl.h>
#include "lcd.h"
#include "ts.h"
#include "font.h"
#include "utf8_gb2312.h"

#define SERV_PORT 6666

#define head_data "head"
#define tail_data "tail"

//LED
#define TEST_MAGIC 'x'                           //定义幻数
#define LEDOP0 _IO(TEST_MAGIC, 0)
#define LEDOP1 _IO(TEST_MAGIC, 1)
#define LEDOP2 _IO(TEST_MAGIC, 2)
#define LEDOP3 _IO(TEST_MAGIC, 3)

#define BUFSIZE 1024 * 1024

static char buf[BUFSIZE];
static char out[(BUFSIZE * 5) / 3];	// Technically 4/3 of input, but take some margin
size_t nread;
size_t nout;
int x,y,push;


void sys_err(const char *str)
{
	perror(str);
	exit(1);
}

// 信号响应函数
void recvOOB(int sig);
//base64编码
static int
enc (FILE *fp,FILE *fd);
//base64解码
static int
dec (FILE *fp,FILE *fd);
//接收文件
int recv_file(void);
//文件发送
int send_file(char *filename);
//语音转文字
void yutowen(void);
//文字转语音
void wentoyu(void);
//文字回答
char *anwser(char *cmdbuf);
//整形转字符串
void itoa2(int m,unsigned char *a);
//清理网络数据
void clear_netdat(void);



//线程获取坐标
void *pthread_get_ts(void * arg)
{
    while(1)
    {
        get_xy(&x,&y,&push);
        
    }
    
}
//全局套接字
int cfd,ledfd;
int main (int argc, char **argv)
{
	if(argc != 2)
	{
		printf("input error:try './recv 192.168.18.66'\n");
		exit(0);
	}
    ledfd = open("/dev/Led",O_RDWR);//打开灯设备
    /* 连接服务器 */
    int conter = 10;
    char buf[BUFSIZ];
    
    struct sockaddr_in serv_addr;          //服务器地址结构

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    //inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
        sys_err("socket error");

    int ret = connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret != 0)
        sys_err("connect err");
    printf("connect sccess\n");
    //设置信号所属组
	signal(SIGURG, recvOOB);
    fcntl(cfd,F_SETOWN,getpid());
	/* 连接服务器成功 */


    //初始化触摸屏显示屏
    open_lcd();
    open_ts();
    Init_Font();//汉字库


    //显示初始化界面
    lcd_clear(0x0000f0);//清屏
    show_bmp("yutowen.bmp",130,140);
    show_bmp("wentoyu.bmp",460,140);

    //显示汉字到屏幕
    char s_buf[60] = "语音识别系统";
    char out_buf[60];
    int inlen = strlen(s_buf)+1;
    int outlen = sizeof(out_buf);
    utf8_gb2312(s_buf,inlen,out_buf,outlen);
    Display_characterX(260,20,out_buf,0x0f00f0,3);//语音识别系统

    //创建线程获取坐标
    pthread_t thr;
    pthread_create(&thr,NULL,pthread_get_ts,NULL);

    while(1)
    {
        if(push == 1)
        {
            if(y>140 && y<340)
            {
                if(x>130 && x<330)
                    yutowen();
                else if(x>460 && x<660)
                    wentoyu();

                lcd_clear(0x0000f0);//清屏
                show_bmp("yutowen.bmp",130,140);
                show_bmp("wentoyu.bmp",460,140);
            }
            clear_netdat();//清理网络数据
            
        }
        
        
    }
	//关闭lcd屏
    close_lcd();
    close_ts();
    close(ledfd);
    UnInit_Font();
	return 0;
}
//清理网络数据
void clear_netdat(void)
{
    char nullbuf[1000]; //读取数组，清网络垃圾
    int flags;  //设置套接字
    //设置套接字非阻塞
    flags = fcntl(cfd,F_GETFL,0);
    flags |= O_NONBLOCK;
    fcntl(cfd,F_SETFL,flags);
    //清理数据
    read(cfd,nullbuf,sizeof(nullbuf));
    //设置套接字阻塞
    flags = fcntl(cfd,F_GETFL,0);
    flags &=~ O_NONBLOCK;
    fcntl(cfd,F_SETFL,flags);
}

// 信号响应函数
void recvOOB(int sig)
{
    char oob;
    recv(cfd, &oob, 1, MSG_OOB);
    if(oob == 's')
    {
        printf("接收到紧急数据，接收\n");
        recv_file();
    }
    else if(oob == 'p')
    {
        printf("接收到紧急数据，播放\n");
        system("aplay tts_sample.wav");
    }
    else if(oob == 'r')
    {
        printf("接收到返回数据\n");
        char r_buf[100];
        read(cfd,r_buf,sizeof(r_buf));
        
        char out_buf[60];
        int inlen = strlen(r_buf)+1;
        int outlen = sizeof(out_buf);
        int re = utf8_gb2312(r_buf,inlen,out_buf,outlen);
        //开发板显示文字信息
        Clean_Area(300,380,400,48,0xffffff);
        Display_characterX(300,380,out_buf,0x00f000,3);



        //开发板回答你的语音
        char *ansbuf = anwser(r_buf);

        //发送紧急数据

        time_t tim,tim_last;
        tim = time(NULL);
        if(tim-tim_last>4)
        {
        char data = 'w';
        send(cfd, &data, 1, MSG_OOB);

        printf("发送文字：%s\n",ansbuf);
        write(cfd,ansbuf,strlen(ansbuf)+1);
        
        }
        tim_last = tim;
    }
}

//文字回答
char *anwser(char *cmdbuf)
{
    char buf[10][15]={
        "打开",
        "帮我",
        "运行",
        "启动",
        "我是",
        "你是",
        "开灯",
        "关灯",
	"床前",
	"哈喽"
    };
    static char retbuf[100];
    if(strncmp(cmdbuf,buf[0],strlen(buf[0]))==0)
        strcpy(retbuf,"好了帮你打开了");
    else if(strncmp(cmdbuf,buf[1],strlen(buf[1]))==0)
        strcpy(retbuf,"好的");
    else if(strncmp(cmdbuf,buf[2],strlen(buf[2]))==0)
        strcpy(retbuf,"已经帮你运行了");
    else if(strncmp(cmdbuf,buf[3],strlen(buf[3]))==0)
        strcpy(retbuf,"已经启动了");
    else if(strncmp(cmdbuf,buf[4],strlen(buf[4]))==0)
        strcpy(retbuf,"你是大沙叼");
    else if(strncmp(cmdbuf,buf[5],strlen(buf[5]))==0)
        strcpy(retbuf,"你是大傻逼");
    else if(strncmp(cmdbuf,buf[6],strlen(buf[5]))==0)
    {
        ioctl(ledfd,LEDOP0,0);
        ioctl(ledfd,LEDOP1,0);
        ioctl(ledfd,LEDOP2,0);
        ioctl(ledfd,LEDOP3,0);
        strcpy(retbuf,"开灯了，你看看你的板子上灯亮了没");
    }
        
    else if(strncmp(cmdbuf,buf[7],strlen(buf[5]))==0)
    {
        ioctl(ledfd,LEDOP0,1);
        ioctl(ledfd,LEDOP1,1);
        ioctl(ledfd,LEDOP2,1);
        ioctl(ledfd,LEDOP3,1);
        strcpy(retbuf,"关灯了，你看看灯关了没，要是没关你自己关一下");
    }
    else if(strncmp(cmdbuf,buf[8],strlen(buf[8]))==0)
        strcpy(retbuf,"疑似地上霜");
    else if(strncmp(cmdbuf,buf[9],strlen(buf[9]))==0)
        strcpy(retbuf,"哈喽哈喽，帅哥你好啊");
        
    else 
        strcpy(retbuf,"哎呀这个人家还没学会");

    return retbuf;
    
}

//接收文件
int recv_file(void)
{
    struct stat sta;
    char name[64];

	//校验
    char cheak = 0;
    read(cfd, &cheak, 1);
    printf("读校验...\n");
    if(cheak != ':'){
        printf("校验失败\n");
        cheak = 0;
        write(cfd, &cheak, 1);
        return -1;
    }
    printf("校验成功...\n");
    usleep(10000);
    cheak = '?';
    write(cfd, &cheak, 1);
    
    //读文件属性
    uint32_t file_size;
    read(cfd,name,sizeof(name));
    printf("filename: %s\n",name);

    char size_buf[20];
    read(cfd,size_buf,sizeof(size_buf));
    file_size = atol(size_buf);
    printf("filesize: %ld\n",file_size);

    //准备文件
    FILE *fd = fopen(name,"w+");
	FILE *fp = fopen("decode.txt","w+");


	//接收数组
    char *file_buf = (char *)malloc(file_size+100);


	int n_read;
    size_t total=0;

	sta.st_size = file_size;
    while(1)
    {
        if(sta.st_size-total>=10*1024)
        {
            n_read = read(cfd,file_buf+total,10*1024);
            total += n_read;
            printf("read %d byte：\n",n_read);
        }else if(sta.st_size-total<10*1024)
        {
            n_read = read(cfd,file_buf+total,sta.st_size-total);
            total += n_read;
            
            printf("read finish:all %ld byte\n",total);
            break;
        }
        if(n_read < 0)
        {
            printf("read error\n");
            exit(0);
        }
    }

	fwrite(file_buf,total,1,fp);
	fclose(fp);

	FILE *fp1 = fopen("decode.txt","r");
	//解码数据
	dec(fp1,fd);
    free(file_buf);
}

void itoa2(int m,unsigned char *a)
{
    int x=0;
    unsigned char *b=a;
    while(m>0)
    {
        x=m%10;
        *a=x+'0';
        m=m/10;
        a++;
    }
    *a='\0';
    a--;
    while(a>b)
    {
       x=*a; *a=*b;*b=x;
       a=a-1;  b=b+1;
    }

}


//文件发送
int send_file(char *filename)
{
    //发送紧急数据
    char data = 's';
    send(cfd, &data, 1, MSG_OOB);

    //校验
    char send_cheak = ':';
    char cheak = 0;
    usleep(5000);

    write(cfd, &send_cheak, 1);
    read(cfd, &cheak, 1);
    if(cheak != '?'){
        printf("对方不接受，发送失败\n");
        return -1;
    }printf("校验成功\n");

    //准备文件
    FILE *fp = fopen(filename,"r");
    if(fp == NULL){
        perror("fopen failed");
        exit(0);
    }
    FILE *fd = fopen("decode.txt","w+");
    if(fd == NULL){
        perror("fopen failed");
        exit(0);
    }

    //编码
    enc(fp,fd);

    //文件属性
    struct stat staf;
    if(stat(filename,&staf)==-1){
        perror("stat failed");
        exit(0);
    }

    printf("文件：%s\n",filename);
    printf("大小：%ld\n",staf.st_size);

    //文件属性
    struct stat sta;
    if(stat("decode.txt",&sta)==-1){
        perror("stat failed");
        exit(0);
    }
    printf("编码文件：%s\n","decode.txt");
    printf("编码大小：%ld\n",sta.st_size);

    char *file_buf = (char *)malloc(sta.st_size+10);
    bzero(file_buf,sizeof(file_buf));
    


    fd = fopen("decode.txt","r");
    if(fd == NULL){
        perror("fopen failed");
        exit(0);
    }

    //读取编码数据
    size_t read_total;

    read_total = fread(file_buf,1,sta.st_size,fd);
    printf("读出编码数据：%ld\n",read_total);

    
    //告诉对方文件名
    write(cfd,filename,strlen(filename)+1);
    usleep(50000);

    char size_buf[20];
    itoa2(read_total,size_buf);
    printf("size:%s\n",size_buf);
    //告诉对方要发多少字节过来
    write(cfd,size_buf,strlen(size_buf)+1);
    
    usleep(100000);
    

    int n_write=0;
    unsigned long w_total=0;
    while(1)
    { 
        if(read_total-w_total>=10*1024)
        {
            n_write = write(cfd,file_buf+w_total,10*1024);
            w_total += n_write;
            printf("发送%d字节：\n",n_write);usleep(1000);
        }
        else if(read_total-w_total<10*1024)
        {
            n_write = write(cfd,file_buf+w_total,read_total-w_total);
            w_total += n_write;
            printf("发完了:一共%ld字节\n",w_total);usleep(1000);
            break;
        }
        if(n_write < 0)
        {
            printf("发送出错\n");
            exit(0);
        }
    }

    free(file_buf);

    return 0;
}


static int
enc (FILE *fp,FILE *fd)
{
	int ret = 1;
	struct base64_state state;

	base64_stream_encode_init(&state, 0);

	while ((nread = fread(buf, 1, BUFSIZE, fp)) > 0) {
		base64_stream_encode(&state, buf, nread, out, &nout);
		if (nout) {
			fwrite(out, nout, 1, fd);
		}
		if (feof(fp)) {
			break;
		}
	}
	if (ferror(fp)) {
		fprintf(stderr, "read error\n");
		ret = 0;
		goto out;
	}
	base64_stream_encode_final(&state, out, &nout);

	if (nout) {
		fwrite(out, nout, 1, fd);
	}
out:	fclose(fp);
	fclose(fd);
	return ret;
}

static int
dec (FILE *fp,FILE *fd)
{
	int ret = 1;
	struct base64_state state;

	base64_stream_decode_init(&state, 0);

	while ((nread = fread(buf, 1, BUFSIZE, fp)) > 0) {
		if (!base64_stream_decode(&state, buf, nread, out, &nout)) {
			fprintf(stderr, "decoding error\n");
			ret = 0;
			goto out;
		}
		if (nout) {
			printf("write\n");
			fwrite(out, nout, 1, fd);
		}
		if (feof(fp)) {
			printf("write ok\n");
			break;
		}
	}
	if (ferror(fp)) {
		fprintf(stderr, "read error\n");
		ret = 0;
	}
out:	fclose(fp);
	fclose(fd);
	return ret;
}


//语音转文字
void yutowen(void)
{
    lcd_clear(0x0f0fff);//清屏
    show_bmp_center("yutowen.bmp");
    
    printf("语音转文字\n");
yu2wen:
    chdir("../audio");
    system("./a.out");

    //清除显示汉字
    Clean_Area(300,380,400,48,0xffffff);
    char s_buf[60] = "正在识别语音...";
    char out_buf[60];
    int inlen = strlen(s_buf)+1;
    int outlen = sizeof(out_buf);
    int re = utf8_gb2312(s_buf,inlen,out_buf,outlen);
    Clean_Area(300,380,400,48,0xffffff);
    Display_characterX(300,380,out_buf,0x00f000,3);//语音//正在识别语音

    //发送文件
    int ret = send_file("b.pcm");
    chdir("../base64_file");

    
    show_bmp("back.bmp",700,380);
    show_bmp("shuaxin.bmp",700,280);
    while(1)
    {
        if(push == 1)
        {
            if(x>700 && y>280 && y<380)
            {
                clear_netdat();//清理网络数据
                goto yu2wen;
            }
                
            else if(x>700 && y>380)
            {
                break;
            }usleep(100);
        }
    }
    
}


//文字转语音
void wentoyu(void)
{
    char buf[1000];
    bzero(buf,sizeof(buf));

    lcd_clear(0x3f0fff);//清屏
    show_bmp_center("wentoyu.bmp");
    
    printf("======文字转语音======\n");
    printf("请输入文字：\n");
    scanf("%s",buf);

    //发送紧急数据
    char data = 'w';
    send(cfd, &data, 1, MSG_OOB);
    usleep(10000);
    printf("发送文字：%s\n",buf);
    write(cfd,buf,strlen(buf)+1);
    sleep(2);
}
