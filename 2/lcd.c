#include "lcd.h"

#include "stdio.h"
static int lcd_fd;
static unsigned char *FB;

/**==宏定义、结构体定义、全局变量定义===========*/
#define LCD_SIZE 800*480*4

/*bmp图片的文件头信息定义*/
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#pragma pack(1)   			//取消字节对齐 
typedef struct {			//bmp图片文件头信息封装 
	// 位图文件头 
	u8  bit_file_type[2];	//位图文件类型:'BM'->0x4d42 
	u32 file_size;	  		//整个文件大小 
	u16 reserved1;	  		//保留 
	u16 reserved2;	  		//保留 
	u32 offset;		  		//文件头到位图数据之间的偏移量 

	// 位图信息头 
	u32 head_size;			//位图信息头长度 
	u32 width;		  		//位图宽度 
	u32 height;		  		//位图高度 
	u16 bit_planes;	  		//位图位面数 
	u16 bits_per_pixel; 	//每个像素的位数 
	u32 compression;		//压缩说明 
	u32 image_size;			//位图数据大小 
	u32 h_res;				//水平分辨率 
	u32 v_res;				//垂直分辨率 
	u32 color_palette;		//位图使用的颜色索引数 
	u32 vip_color;			//重要的颜色索引数目 

}bmp_head;
#pragma pack() 	//恢复字节对齐

//初始化LCD
int open_lcd(void)
{
	// 1.打开LCD

	lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		perror("open lcd failed");
		return -1;
	}
		//内存映射

	FB = mmap(	NULL, 		
				4*800*480, 	//
				PROT_READ | PROT_WRITE,	//
				MAP_SHARED,	//
				lcd_fd, 	//
				0);			//
	if(FB == MAP_FAILED)
	{
		perror("mmap failed");
		return -1;
	}
}
//关闭LCD
void close_lcd(void)
{
	close(lcd_fd);
	munmap(FB,4*800*480);
}

//获取屏幕颜色
int lcd_get_color(int x,int y)
{
	int color;
	color=*(FB+y*800*3+x)*4096
	+*(FB+y*800*3+x+1)*256
	+*(FB+y*800*3+x+2);

	return color;
}
//显示EBM图
int show_ebm(const char * argv)
{
	int ebm_fd;
	ebm_fd = open(argv, O_RDONLY);
	if(ebm_fd == -1)
	{
		printf("%s\n", argv);
		perror("open ebm failed");
		return -1;
	}
	unsigned char ebm_buf[4*800*480];
	bzero(ebm_buf, sizeof(ebm_buf));
	read(ebm_fd, ebm_buf, sizeof(ebm_buf));
	close(ebm_fd);
	
	int x;
	for(x=0; x<800*4*480; x++)
	{
		*(FB+x) = ebm_buf[x];
	}
	
	close(lcd_fd);
}
//显示bmp
void show_bmp(char *bmp_patpname,int x_offset,int y_offset)
{
	/*向lcd文件中写入像素点数据*/
	//打开bmp图片
	int  bmp_fd = open(bmp_patpname, O_RDONLY);//完整路径 /mnt/hgfs/shear/day004/1.bmp
	if(bmp_fd == -1)
	{
		perror("打开bmp图片失败");
		return ;
	}
	int n;//多出来的字节数

	//读取图片头部信息
	bmp_head myhead;
	read(bmp_fd, &myhead, sizeof(bmp_head));


	//如果每一行字节数能被4整除，n为0，否则是多出来的字节数
	n = ((myhead.width*3)%4==0) ? 0 : 4 -(myhead.width*3)%4;	


	//申请缓冲区
	char tmp_buf[(myhead.width*3+n)*myhead.height];
	//清空缓冲区
	bzero(tmp_buf,sizeof(tmp_buf));
	
	//读取图片数据
	read(bmp_fd, tmp_buf, sizeof(tmp_buf));
	//关闭图片文件
	close(bmp_fd);
	//将数据存入映射内存
	//将数据存入映射内存
	int i,j;
	for(i=0; i<myhead.height; i++)
		for(j=0; j<myhead.width; j++)
		{
			*(FB+((i+y_offset)*800+j+x_offset)*4) = tmp_buf[((myhead.height-1-i)*myhead.width+j)*3-i*n];
			*(FB+((i+y_offset)*800+j+x_offset)*4+1) = tmp_buf[((myhead.height-1-i)*myhead.width+j)*3+1-i*n];
			*(FB+((i+y_offset)*800+j+x_offset)*4+2) = tmp_buf[((myhead.height-1-i)*myhead.width+j)*3+2-i*n];
			*(FB+((i+y_offset)*800+j+x_offset)*4+3) = 0x00;
		}
	
}
//缩放显示bmp
void show_bmp_shrink(char *bmp_patpname,int x_offset,int y_offset)
{
	/*向lcd文件中写入像素点数据*/
	//打开bmp图片
	int  bmp_fd = open(bmp_patpname, O_RDONLY);//完整路径 /mnt/hgfs/shear/day004/1.bmp
	if(bmp_fd == -1)
	{
		perror("打开bmp图片失败");
		return ;
	}
	
	//读取图片头部信息
	bmp_head myhead;
	read(bmp_fd, &myhead, sizeof(bmp_head));
	
	//申请缓冲区
	char tmp_buf[myhead.width*myhead.height*3];
	char Tmp_buf[myhead.width*myhead.height*3/2];
	//清空缓冲区
	bzero(tmp_buf,sizeof(tmp_buf));
	
	//读取图片数据
	read(bmp_fd, tmp_buf, sizeof(tmp_buf));
	int n=0;
	for(int i=0; i<sizeof(tmp_buf); i++)
	{
		
		if((i/(800*3))%2==0)
		{
			if(i%6==0)
			{
				for(int j=3*n; j<3*n+3; j++)
				Tmp_buf[j]=tmp_buf[i+j-3*n];
				n++;
			}	
		}
		
	}
	//关闭图片文件
	close(bmp_fd);
	//将数据存入映射内存
	//将数据存入映射内存
	myhead.height=myhead.height/2;
	myhead.width=myhead.width/2;
	int i,j;
	for(i=0; i<myhead.height; i++)
		for(j=0; j<myhead.width; j++)
		{
			*(FB+((i+y_offset)*800+j+x_offset)*4) = Tmp_buf[((myhead.height-1-i)*myhead.width+j)*3];
			*(FB+((i+y_offset)*800+j+x_offset)*4+1) = Tmp_buf[((myhead.height-1-i)*myhead.width+j)*3+1];
			*(FB+((i+y_offset)*800+j+x_offset)*4+2) = Tmp_buf[((myhead.height-1-i)*myhead.width+j)*3+2];
			*(FB+((i+y_offset)*800+j+x_offset)*4+3) = 0x00<<24;
		}
	
}
//居中显示
void show_bmp_center(char *bmp_patpname)
{
	int x_offset,y_offset;
	/*向lcd文件中写入像素点数据*/
	//打开bmp图片
	int  bmp_fd = open(bmp_patpname, O_RDONLY);//完整路径 /mnt/hgfs/shear/day004/1.bmp
	if(bmp_fd == -1)
	{
		perror("打开bmp图片失败");
		return ;
	}
	
	//读取图片头部信息
	bmp_head myhead;
	read(bmp_fd, &myhead, sizeof(bmp_head));
	
	//申请缓冲区
	char tmp_buf[myhead.width*myhead.height*3];
	//清空缓冲区
	bzero(tmp_buf,sizeof(tmp_buf));
	
	//读取图片数据
	read(bmp_fd, tmp_buf, sizeof(tmp_buf));
	//关闭图片文件
	close(bmp_fd);
	//将数据存入映射内存
	//将数据存入映射内存
	int i,j;

	x_offset=400-myhead.width/2;
	y_offset=240-myhead.height/2;
	for(i=0; i<myhead.height; i++)
		for(j=0; j<myhead.width; j++)
		{
			*(FB+((i+y_offset)*800+j+x_offset)*4) = tmp_buf[((myhead.height-1-i)*myhead.width+j)*3];
			*(FB+((i+y_offset)*800+j+x_offset)*4+1) = tmp_buf[((myhead.height-1-i)*myhead.width+j)*3+1];
			*(FB+((i+y_offset)*800+j+x_offset)*4+2) = tmp_buf[((myhead.height-1-i)*myhead.width+j)*3+2];
			*(FB+((i+y_offset)*800+j+x_offset)*4+3) = 0x00<<24;
		}
	
}

//清屏
void lcd_clear(int color)
{
	long n;
	for(n=0; n<480*800*4;n++)
	{
		*(FB+n) = color;
	}

}