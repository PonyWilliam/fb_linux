#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_examples/lv_examples.h"
#include "lv_freetype/lv_freetype.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "sb.c"
#include "update_client.h"
#include "led.h"
#define DISP_BUF_SIZE (80 * LV_HOR_RES_MAX)
lv_obj_t *scr,*scr2,*scr3;
lv_obj_t *s2_img1;
lv_anim_t a;//动画的变量
static int fd; // 是mplayer的通信文件/home/china/1.fifo的文件描述符
pthread_t tid,ptid,pptid,ppptid;
int now = 1;
void file_open(){
    fd = open("/home/china/1.fifo", O_RDWR);
    if (fd == -1)
    {
        perror("open fifo error");
        exit(1);
    }
}
void* play(void *args)
{
    system("killall -9 mplayer");
    char str[1024];
    // sprintf(str,"mplayer -slave -quiet -input  file=/home/china/1.fifo -zoom -x 800 -y 380  -geometry  0:0  0%d.mp4",now);
    sprintf(str,"mplayer -input file=/home/china/1.fifo -slave -quiet -zoom -x 800 -y 380  -geometry  0:0  0%d.mp4",now);
    system(str); 
}
void mypause(){
    
    const char* cmd = "pause\n";
    int ret = write(fd, cmd, strlen(cmd));
    printf("write cmd %d bytes \n", ret);
}
static void btn1_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED) {
        system("killall -9 mplayer");
        printf("btn1 clicked\n");
        //上一首
        if(now < 2){
            now = 4;
        }else{
            now -= 1;
        }
        pthread_create(&tid, NULL, play, NULL);
    }
}
static void btn2_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED) {
        //暂停
        printf("btn2 clicked\n");
        mypause();
    }
}
static void btn3_event_cb(lv_obj_t * btn,lv_event_t e){
    if(e == LV_EVENT_CLICKED){
        system("killall -9 mplayer");
        printf("btn3 clicked");
        if(now >= 4){
            now = 1;
        }else{
            now += 1;
        }
        pthread_create(&tid, NULL, play, NULL);
    }
}
static void btn4_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_CLICKED) {
        system("killall -9 mplayer");
        printf("test4\n");
        lv_scr_load_anim(scr2,LV_SCR_LOAD_ANIM_MOVE_LEFT,500,0,false);
    }
}
void del_obj_thread(){
    usleep(3000 * 1000);
    lv_obj_fade_out(s2_img1,750,0);
    lv_img_set_zoom(s2_img1,1);
    
}
static void s2_btn1_cb(lv_obj_t *btn,lv_event_t e){
    if(e == LV_EVENT_CLICKED){
        //CLICKD我是傻逼
        //弹出图片你就是傻逼
        LV_IMG_DECLARE(sb);
        lv_anim_set_var(&a,s2_img1);
        lv_img_set_src(s2_img1,&sb);
        lv_obj_align(s2_img1,scr2,LV_ALIGN_CENTER,0,0);
        lv_obj_set_size(s2_img1,8,5);
        lv_img_set_zoom(s2_img1,100);
        lv_obj_fade_in(s2_img1,1000,0);
        pthread_create(&tid,NULL,del_obj_thread,2000);
    }
}
static void s2_btn2_cb(lv_obj_t *btn,lv_event_t e){
    if(e == LV_EVENT_CLICKED){
        LV_IMG_DECLARE(sb);
        lv_anim_set_var(&a,s2_img1);
        lv_img_set_src(s2_img1,&sb);
        lv_obj_align(s2_img1,scr2,LV_ALIGN_CENTER,0,0);
        lv_obj_set_size(s2_img1,8,5);
        lv_img_set_zoom(s2_img1,100);
        lv_obj_fade_in(s2_img1,1000,0);
        pthread_create(&tid,NULL,del_obj_thread,NULL);
    }
}
static void back_window(lv_obj_t *btn,lv_event_t e){
    if(e == LV_EVENT_CLICKED){
        //返回上一个窗口
        lv_scr_load_anim(scr,LV_SCR_LOAD_ANIM_MOVE_RIGHT,600,0,false);
    }
}
static void record(lv_obj_t *btn,lv_event_t e){
    if(e == LV_EVENT_CLICKED){
        //开始录音
        // arecord -D hw:0,0 -d3 -c1 -r16000 -traw -fS16_LE cmd.pcm
        system("arecord -d2 -c1 -r16000 -traw -fS16_LE 1.pcm");
        //录音完成，开始使用socket传输到服务端
        SendFile("1.pcm");
        //发送完成，可以选择删除文件
        system("rm -f ./1.pcm");
        sleep(3);//等待翻译
    }
}

void anim_init(void){
    lv_anim_init(&a);//初始化动画函数
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) (lv_img_set_zoom)); 
    lv_anim_set_time(&a,600);
    s2_img1 = lv_img_create(scr2,NULL);
}
void font_init(void){
    lv_freetype_init(64, 1, 0);
    lv_ft_info_t info;
    info.name = "./arial.ttf";
    info.weight = 16;
    info.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&info);
}
void file_init(){
    lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);
}
void init(){
    lv_init();
    /*Linux frame buffer device init*/
    fbdev_init();
    evdev_init();
    file_open();
    font_init();
    lv_fs_if_init();
    Socket_init();
    //对所有初始化后初始化链表
}
void layout(){
    scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);
    scr2 = lv_obj_create(NULL,NULL);
    scr3 = lv_obj_create(NULL,NULL);//这是一个屏幕
    lv_obj_t * btn1 = lv_btn_create(scr, NULL);         /*Create a button on the screen*/
    lv_obj_t *label1 = lv_label_create(btn1,NULL);
    lv_obj_t *btn2 = lv_btn_create(scr,NULL);
    lv_obj_t *label2 = lv_label_create(btn2,NULL);
    lv_obj_t *btn3 = lv_btn_create(scr,NULL);
    lv_obj_t *label3 = lv_label_create(btn3,NULL);
    lv_obj_t *btn4 = lv_btn_create(scr,NULL);
    lv_obj_t *label4 = lv_label_create(btn4,NULL);
    lv_obj_t *s2_btn1 = lv_btn_create(scr2,NULL);
    lv_obj_t *s2_btn2 = lv_btn_create(scr2,NULL);
    lv_obj_t *s2_label1 = lv_label_create(s2_btn1,NULL);
    lv_obj_t *s2_label2 = lv_label_create(s2_btn2,NULL);
    lv_obj_t *s2_btn3 = lv_btn_create(scr2,NULL);
    lv_obj_t *s2_label3 = lv_label_create(s2_btn3,NULL);
    lv_label_set_text(label1,"prev");
    lv_label_set_text(label2,"pause");
    lv_label_set_text(label3,"next");
    lv_label_set_text(label4,"record");
    lv_obj_set_size(btn1,100,50);
    lv_obj_set_size(btn2,100,50);
    lv_obj_set_size(btn3,100,50);
    lv_obj_set_size(btn4,150,50);
    lv_obj_set_pos(btn1,100,405);
    lv_obj_set_pos(btn2,250,405);
    lv_obj_set_pos(btn3,400,405);
    lv_obj_set_pos(btn4,550,405);
    lv_obj_set_event_cb(btn1,btn1_event_cb);
    lv_obj_set_event_cb(btn2,btn2_event_cb);
    lv_obj_set_event_cb(btn3,btn3_event_cb);
    lv_obj_set_event_cb(btn4,record);
    //第二个屏幕用来愉快的玩耍
    lv_obj_set_size(s2_btn1,100,50);
    lv_obj_set_size(s2_btn2,100,50);
    lv_obj_set_pos(s2_btn1,250,215);
    lv_obj_set_pos(s2_btn2,250+100+100,215);
    lv_label_set_text(s2_label3,"back to first");
    lv_obj_set_size(s2_btn3,300,100);
    lv_obj_set_pos(s2_btn3,250,330);
    lv_obj_set_event_cb(s2_btn3,back_window);
    lv_label_set_text(s2_label1,"yes");
    lv_label_set_text(s2_label2,"no");
    lv_obj_set_event_cb(s2_btn1,s2_btn1_cb);
    lv_obj_set_event_cb(s2_btn2,s2_btn2_cb);
    //做第三个屏幕，是用来做语音识别的界面。 
    lv_obj_t *s3_btn1 = lv_btn_create(scr3,NULL);
    lv_obj_t *s3_label1 = lv_label_create(s3_btn1,NULL);
    lv_label_set_text(s3_label1,"record");
    lv_obj_set_size(s3_btn1,200,120);
    lv_obj_align(s3_btn1,lv_scr_act(),LV_ALIGN_CENTER,0,0);
    lv_obj_set_event_cb(s3_btn1,record);

}
void* myrecv(){
    int n = 0;
    char recvbuf[1024];
    memset(recvbuf,0,1024);
    while(n = recv(client_socket,recvbuf,1024,0)){
        printf("继续向下执行\n");
        if(atoi(recvbuf)==0){
            //上一首
            system("killall -9 mplayer");
            printf("btn1 clicked\n");
            //上一首
            if(now < 2){
                now = 4;
            }else{
                now -= 1;
            }
            pthread_create(&tid, NULL, play, NULL);
        }else if(atoi(recvbuf)==1){
            //下一首
            system("killall -9 mplayer");
            printf("btn1 clicked\n");
            //上一首
            if(now >= 4){
                now = 1;
            }else{
                now += 1;
            }
            pthread_create(&tid, NULL, play, NULL);
            

        }else if(atoi(recvbuf)==2){
            printf("关灯\n");
            close_led();
        }else if(atoi(recvbuf)==3){
            printf("开灯\n");
            open_led();
        }else if(atoi(recvbuf)==4){
            printf("重启\n");
            system("reboot");
        }else if(atoi(recvbuf)==5){
            printf("暂停播放\n");
            system("killall -9 mplayer");
        }else if(atoi(recvbuf)==6){
            printf("继续播放\n");
            pthread_create(&tid, NULL, play, NULL);
        }
        memset(recvbuf,0,1024);
    }
}
void first(void){
    init();
    static lv_color_t buf[DISP_BUF_SIZE];
    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer   = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
    layout();
    pthread_create(&ptid,NULL,myrecv,NULL);
    anim_init();
    while(1) {
        lv_task_handler();
        usleep(5000);
    }
    return 0;
}
int main(void)
{
    first();
}
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}