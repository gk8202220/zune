
#include "read_date.h"
//读取一次的数据
unsigned char read_date(int read_fd)
{
	int ret;
	unsigned char char_data;
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0; //设置接收数据的等待时间2S
	fd_set rd_fd;
	FD_ZERO(&rd_fd);
	FD_SET(read_fd,&rd_fd);
	ret = select(read_fd + 1, &rd_fd, NULL, NULL, &timeout);
	switch(ret)
	{
		case -1:
			perror("select error\n");
			break;
		case  0:
			printf("Timeout\n");
			break;
		default:
			ret = read(read_fd, &char_data,1); //读取一个字节数据
			if (ret < 0)
			{
				printf("ret = %d, %m\n", ret, errno);
				break;

			}
	//	printf("sor:%#X\n",char_data);
			return char_data;
	}	
	return -1;
}
