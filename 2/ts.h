#ifndef _TS_H
#define _TS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>	
#include <stdio.h>
#include <linux/input.h>	//输入子系统头文件

int open_ts(void);//触摸屏初始化 
void close_ts(void);//关闭触摸屏文件
//获取坐标
int get_xy(int *x,int *y,int *push);


#endif