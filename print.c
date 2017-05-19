/*
功能实现：打印小票，支持中文和条形码的打印
作者：朱振展
联系：442446835@qq.com
时间：2017.03.12
修改内容：1.串口统一设置的方式：115200 ，8，‘N’,1
		2.增加对无纸的检测

注：在打印的过程中无法发现没有纸，除非重新按一下打印按钮才能判断。此时可以提示
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "print.h"
#include "head.h"
#include "read_date.h"
#include "uartApi.h"

static int fd_printer;

int openPrinter(char *dev)
{
	
	printf("open printer dev!\n");
	if((fd_printer = open_port(dev) )< 0)
	{
		perror("open printer dev error...");
		return -1;
	}
	//3,2波特率：9600,数据位：8位，偶数校验，1位停止位
	if(set_opt(fd_printer,115200,8,'N',1)<0)
	{
		perror("set_opt printer error...");
		return -1;
	}

}

int closePrinter(void)
{
	int ret = 0;
	ret = close(fd_printer);
	return ret;
}

int writePrinter(const char *str, int len)
{
	int ret = 0;

	if(str == NULL || len <= 0 ) return -1;

	ret = write(fd_printer, str, len);

	return ret;
}

int setPrinterModeChinese(void)
{
	int ret = 0;
	unsigned char cmd[2] = {0x1C, 0x26};

	ret = write(fd_printer, cmd,  sizeof(cmd));

	return (ret == 2) ? 0 : -1 ;
}
//设置汉字模式打印组合
int setPrinterMoudle(void)
{
	int ret = 0;
	unsigned char cmd[3] = {0x1B, 0x21,0x30};

	ret = write(fd_printer, cmd, sizeof(cmd));

	return (ret == 3) ? 0 : -1 ;
}
int setPriterModeEncode(int mode)
{
	int ret = 0;
	char cmd[3] = {0x1B, 0x39};
	cmd[2] = mode;

	//if(mode < 0)

	ret = write(fd_printer, cmd,  sizeof(cmd));
	return (ret == 3) ? 0 : -1 ;
	
}

int printertest(void)
{
	int ret = 0;
	char cmd[] = {0x1D, 0x6B, 72, 7, 67, 111, 100, 101,\
		13, 57, 51};
	ret = write(fd_printer, cmd,  sizeof(cmd));

	return ret;
}

int getPrinterState()
{

	int ret;
	char sendDate[256] = {"printerStatus:"};	
	char buf;
	char cmd[] = {0x10,0x04,0x04};
	

	writePrinter(cmd,sizeof(cmd));
	buf =  read_date(fd_printer);
	if(buf == 0x12)
	{
		sendDate[strlen(sendDate)] = MACHINE_FINE;
	}
	else if (buf == 0xff)
	{
		sendDate[strlen(sendDate)] = NO_CONNECT;
	}else
	{
		sendDate[strlen(sendDate)] = NO_PAPER;
	}
	sendDate[strlen(sendDate)] = '#';
	ret = write(fifo_wr, sendDate, strlen (sendDate));	// 写管道 	
	if(ret < 0)
	{
		printf("card send to QT  error:%s!\n",sendDate);
	}
	

	//此处写入管道
	return ret;
	
}

int setPriterStrong(int mode)
{
	int ret = 0;
	char cmd[3] = {0x1B, 0x45};
	cmd[2] = mode;
	ret = write(fd_printer, cmd,  sizeof(cmd));

	return ret;
	
}

int resetPrinter(void)
{
	int ret = 0;
	char cmd[2] = {0x1B, 0x40};
	ret = write(fd_printer, cmd,  sizeof(cmd));

	return ret;
	
}

//设置正常字符大小
int setCharSizeNormal()
{
	int ret = 0;
	unsigned char cmd[3] = {0x1D, 0x21,0x00};

	ret = write(fd_printer, cmd,  sizeof(cmd));

	return ret;
}
//设置大字符
int setCharSizeBig()
{
	int ret = 0;
	unsigned char cmd[3] = {0x1D, 0x21,0x10};

	ret = write(fd_printer, cmd,  sizeof(cmd));

	return ret;
}


int writePrinterDraw(unsigned char width, unsigned char high, unsigned char *dat)
{
	int ret = 0, n = 0;
	unsigned char cmd[8] = {0x1D, 0x2A};
	
	if(width <= 0 || width > 7 ||
		high <= 0 || high > 7 ||
		dat == NULL) return -1;

	cmd[2] = width;
	cmd[3] = high;

	n = width * high * 8;
	
	//setPrinterModeChinese();
	//resetPrinter();
	write(fd_printer, cmd, 4);

	ret = write(fd_printer, dat, n);
	
	sleep(1);
	//usleep(1000);
	cmd[0] = 0x1D;
	cmd[1] = 0x2F;
	cmd[2] = 48;

	write(fd_printer, cmd, 3);

	return ret;
}

//打印条形码
int print_bar(char *numBar)
{
	
	int ret;
	char displayNum[3] = {0x1D,0x48,2}; //条形码显示字符
	char bar_commem[3] = {0x1D, 0x6B, 2};//打印条形码命令	
	writePrinter(displayNum,3);
	writePrinter(bar_commem,3);
	ret = writePrinter(numBar,strlen(numBar)+1);
	return ret;
}

int init_printer_dev()
{
	openPrinter(Print_Path);

	getPrinterState();
	resetPrinter();	

	
	setPriterModeEncode(1);//GBK232模式
	setPrinterModeChinese();//打印中文模式
	//setPrinterMoudle();
	//setPrinterCharSize(2, 2);//设置打印的尺寸
	//setPriterStrong(1);
	
}
//确定打印缓冲区并进纸N行  ，num：行数（1~255）
int  printAndNewLineN(int num)
{
	int ret;
	char data[3] = {0x1B,0x64,num}; 
	ret = writePrinter(data, sizeof(data));
	return ret;
}
//确定打印缓冲区并换行
int printAndNewLine()
{
	int ret = 0;
	unsigned char cmd[1] = {0x0A};

	

	ret = writePrinter( cmd, sizeof(cmd));

	return ret;
}
//切割纸张
int  cutPaper()
{
	int ret;
	char data[3]={ 0x1D, 0x56, 0x00};
	ret =  writePrinter(data, sizeof(data));
	
	return ret;
}

void  *printer(void *args)
{	
	char fileBUff[1024]; 
	int ret;	
	char *str;
	char *serialNum;
	int fd_printerFile;	
	char wannengju[] = " 万能居购物广场";
	//打开小票信息
	init_printer_dev();
	//打印标题，大字体
	setCharSizeBig();
	writePrinter(wannengju, strlen(wannengju));
	printAndNewLine();
	//打印内容
	setCharSizeNormal();
	fd_printerFile = open("print_utf8.txt",O_RDONLY);
	if (fd_printerFile < 0)
	{ 
	    perror("open print file failed!"); 
	    exit(1); 
	}	
	
	while((ret = read(fd_printerFile,fileBUff, sizeof(fileBUff) ))> 0)
	{
	 writePrinter(fileBUff, ret);
	//  	
		 
          // 读流水号进行切割，用于打条形码  
          // if( strstr(read_data,"流水号") != NULL)
          // {
          // 	 str =  strdup(read_data);
          // 	 strsep(&str,":");
          // 	 strsep(&str,":");
       		 // serialNum = strsep(&str," ");
     		
          // }             	 	
	 }
//printf("%s\n",serialNum );
	//判断是挂单模式，	
	
	printAndNewLine();
	cutPaper();
	getPrinterState();	
	close(fd_printerFile);
	close(fd_printer);
	return;
}
