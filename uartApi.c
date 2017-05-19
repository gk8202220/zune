/*
    功能：linux串口编程通用设置
    作者：朱振展
    联系：442446835@qq.com
    日期：2017.03.13
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    if  ( tcgetattr( fd,&oldtio)  !=  0) 
    { 
        perror("SetupSerial 1");
        return -1;
    }
    bzero( &newtio, sizeof( newtio ) );
    //激活选项有CLOCAL和CREAD，用于本地连接和接收使用
    newtio.c_cflag  |=  CLOCAL | CREAD; 
    //设置数据位
    newtio.c_cflag &= ~CSIZE; 
    switch( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    //设置奇偶校验，'O':奇校验 'E':偶校验'N':无校验
    switch( nEvent )
    {
    case 'O':                     //奇校验
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':                     //偶校验
        //newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_iflag |= INPCK;
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':                    //无校验
        newtio.c_cflag &= ~PARENB;
        break;
    }
//设置波特率 2400、4800、9600、 115200
switch( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    //设置停止位 1或者2
    if( nStop == 1 )
    {
        newtio.c_cflag &=  ~CSTOPB;
    }
    else if ( nStop == 2 )
    {
        newtio.c_cflag |=  CSTOPB;
    }
    //设置超时等待时间
    newtio.c_cc[VTIME]  = 200;
    newtio.c_cc[VMIN] = 0;
    //清空输入缓冲
    tcflush(fd,TCIFLUSH);
    //生效
    if((tcsetattr(fd,TCSANOW,&newtio))!=0)
    {
        perror("com set error");
        return -1;
    }
 
    return 0;
}

int open_port(char *comport)
{
    int fd;
    //char *dev[]={"/dev/ttymxc0","/dev/ttymxc1","/dev/ttymxc2","/dev/ttymxc3","/dev/ttymxc4"};
   
   fd = open( comport, O_RDWR|O_NOCTTY|O_NDELAY);
   //fd = open( dev[comport], O_RDWR);
        if (-1 == fd)
        {
            perror("Can't Open Serial Port");
            return(-1);
        }
        // else 
        // {
        //     printf("open %s .....\n",comport);
        // }

    return fd;
}
