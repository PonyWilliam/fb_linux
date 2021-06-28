#include "led.h"
void close_led(){
	int fd;
	fd = open("/dev/Led",O_RDWR);                //打开设备下的LED，成功返回0
	if(fd<0)
	{
		perror("Can not open /dev/LED\n");
		return ;
	}

	ioctl(fd, LED1, 1); 
	ioctl(fd, LED2, 1);
	ioctl(fd, LED3, 1); 
	ioctl(fd, LED4, 1);
}
void open_led(){
	int fd;
	fd = open("/dev/Led",O_RDWR);                //打开设备下的LED，成功返回0
	if(fd<0)
	{
		perror("Can not open /dev/LED\n");
		return ;
	}

	ioctl(fd, LED1, 0); 
	ioctl(fd, LED2, 0);
	ioctl(fd, LED3, 0); 
	ioctl(fd, LED4, 0);
}