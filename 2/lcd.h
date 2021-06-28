#ifndef _LCD_H
#define _LCD_H
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>	

static int lcd_fd;
static unsigned char *FB;
//初始化LCD
int open_lcd(void);
void close_lcd(void);
//获取LCD上的颜色
int lcd_get_color(int x,int y);
//显示EBM图片
int show_ebm(const char * argv);
//显示bmp图片
void show_bmp(char *bmp_patpname,int x_offset,int y_offset);
//缩放显示bmp图片
void show_bmp_shrink(char *bmp_patpname,int x_offset,int y_offset);
//显示bmp图片在中心
void show_bmp_center(char *bmp_patpname);
//清屏
void lcd_clear(int color);
#endif
