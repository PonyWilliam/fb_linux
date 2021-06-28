#include "ts.h"
#include <linux/input.h>	//输入子系统头文件
static int ts_fd;
//触摸屏初始化 
int open_ts(void)
{
	// 1.打开触摸屏文�?	
	ts_fd = open("/dev/input/event0", O_RDONLY);
	if(ts_fd == -1)
	{
		perror("open ts failed");
		return -1;
	}
	return ts_fd;
}
//关闭触摸屏文�?
void close_ts(void)
{
	close(ts_fd);
}

//获取坐标
int get_xy(int *x,int *y,int *push)
{
	// 2.读取触摸屏数�?	
	struct input_event ts_buf;
	char x_flag=0,y_flag=0;
	int x_last,y_last;
	int press;
	int x_now,y_now;
	int i=1;
	while(1)
	{
		read(ts_fd, &ts_buf, sizeof(ts_buf));
		//判断事件类型
		if(ts_buf.type == EV_ABS)
		{
			//判断事件代号
			if(ts_buf.code == ABS_X)
			{
					x_now=ts_buf.value;
					x_flag=1;y_flag=0;
			}
			if(ts_buf.code == ABS_Y)
			{
					y_now=ts_buf.value;
					y_flag=1;
			}
		}
		else if(ts_buf.type==EV_KEY && ts_buf.code==BTN_TOUCH)
		{
			press = ts_buf.value;
			*push = press;
		}
		if(x_flag==1 && y_flag==1 && press==1){

					*x=x_now;
					*y=y_now;
					//printf("[%d,%d]%d\n",*x,*y,*push);
					x_flag=0;y_flag=0;
					break;
		}
	}

}