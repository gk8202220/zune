/*
	1.要对数据操作进行时刻的保存
	2.清币信号的发送，设计清币按钮
	3.发送吐币命令后，如果要进行一个查询，要给个时间sleep(2)（至少两秒）让机器先去执行命令
	4.已设置为禁能按键
	故障处理：
	1.没钞
	2.传感器有异物
	3.双倍出钞
	4.机器故障

 */

#include "read_date.h"
#include "out_bill.h"
#include "head.h"
#include "read_date.h"

int  bill_total; //保存每一次放入的金额
unsigned char ret_date[10];
int i;



//最后一位的校验码
static char checkSum(char*cmd)
{
	int i;
	char ret = 0 ;
	for(i = 0;i<8;i++)
	{
		ret +=cmd[i];
	} 
	return ret&0xff;
}

//查询出钞机状态
int bill_status(int bill_fd)
{
	
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'S','0','0','0','0'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("coin_reset write error!");
		return -1;
	}
	memset(ret_date,0,sizeof(ret_date));
	for(i=0;i<sizeof(ret_date);i++)
	{
		ret_date[i] = read_date(bill_fd);
		if(ret_date[0] == 0xff)return NO_CONNECT;
		//printf("%x--%c\n", ret_date[i],ret_date[i]);	

	}
	printf("\nresult :");
	if(ret_date[4] == 'w')
	{
		printf("busy!\n");
	}else if(ret_date[4] == 'r')
	{
		printf("await!\n");
		return MACHINE_FINE;
	}else if(ret_date[4] == 'e')
	{
		
		if(ret_date[5] == 'o')
		{
			printf("No bill!\n");
			return NO_BILL;
		}else if(ret_date[5] == '7')
		{
			//双倍出钞
			printf("double out bill\n");
			return DOUBLE_OUTBILL;
		}else if(ret_date[5] == 'r' || ret_date[5] == 'L')
		{
			//传感器有异物
			printf("sersor foreign!\n");
			return SENSOR_FOREIGN;
		}else
		{
			printf("machine hitch!\n");
			return MACHINE_HITCH;
			//机器故障

		}
		printf("error:%c\n",ret_date[5]);
		
	}else if(ret_date[4] == 't')
	{
		printf("Test mode!\n");
	}else if(ret_date[3] == 'E')
	{
		if(ret_date[4] == 'o')
		{
			printf("No bill!\n");
			return NO_BILL;
		}else if(ret_date[4] == '7')
		{
			//双倍出钞
			printf("double out bill\n");
			return DOUBLE_OUTBILL;
		}else if(ret_date[4] == 'r' || ret_date[4] == 'L')
		{
			//传感器有异物
			printf("sersor foreign!\n");
			return SENSOR_FOREIGN;
		}else
		{
			printf("machine hitch!\n");
			return MACHINE_HITCH;
			//机器故障

		}
	}
	else
	{
		printf("没有连接!\n");
		return NO_CONNECT;
	}
	return OK;

}
//出钞50元
int out_bill50(int bill_fd)
{
	
	
	tcflush(bill_fd,TCIOFLUSH);
	int out_num;
	unsigned char cmd[10] = {0x02,0x30,0x30,'B','0','0','0','1'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	clear_count(bill_fd); //清除错误信息
	sleep(1);
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("coin_reset write error!");
		return -1;
	}
memset(ret_date,0,sizeof(ret_date));
	if( 0x06 ==read_date(bill_fd) )
	{

		for(i=0;i<sizeof(ret_date);i++)
		{
			ret_date[i] = read_date(bill_fd);		
		}
		
		if(ret_date[3] == 's' && ret_date[4] == 'e')
		{
					
			printf("出钞的异常情况:%c\n",ret_date[5] );
			printf("out 50 RMB ");
			if(ret_date[5] == '1')
			{
				printf("No bill!\n");
				//return NO_BILL;
			}else if(ret_date[5] == '7')
			{
				//双倍出钞
				printf("double out bill\n");
				//return DOUBLE_OUTBILL;
			}else if(ret_date[5] == 'r' || ret_date[5] == 'L')
			{
				//传感器有异物
				printf("sersor foreign!\n");
				//return SENSOR_FOREIGN;
			}else
			{
				printf("machine hitch!\n");
				//return MACHINE_HITCH;
				//机器故障

			}
		
			//查询出了多少张			
			out_num = total_count(bill_fd);
			printf("no bill and remain %d no out!\n",out_num);
			return out_num;			
		}else
		{
			printf("out 1 bill!\n");
			return 1;
		}
	}			
	return 0;

}
//出十元
int out_bill10(int bill_fd,int num)
{
	

	tcflush(bill_fd,TCIOFLUSH);
	int ret;
	unsigned char cmd[10] = {0x02,0x30,0x30,'B','0','0','0'};
	cmd[7] = num + 0x30;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
//	clear_count(bill_fd);
	
//	sleep(1);
	ret = write(bill_fd,cmd,10);
	int out_num;
	if(ret < 0)
	{
		perror("out bill10 write error!");
		return -1;
	}
memset(ret_date,0,sizeof(ret_date));
	if( 0x06 ==read_date(bill_fd) )
	{

		for(i=0;i<sizeof(ret_date);i++)
		{
			ret_date[i] = read_date(bill_fd);		
		}
		
		if(ret_date[3] == 's' && ret_date[4] == 'e')
		{
			//出钞中没有钞了，查询出了多少张			
				printf("出钞的异常情况:%c\n",ret_date[5] );
				printf("out 10 RMB ");
			if(ret_date[5] == '1')
			{
				printf("No bill!\n");
				
			}else if(ret_date[5] == '7')
			{
				//双倍出钞
				printf("double out bill\n");
				
			}else if(ret_date[5] == 'r' || ret_date[5] == 'L')
			{
				//传感器有异物
				printf("sersor foreign!\n");
				
			}else
			{
				printf("machine hitch!\n");
				//机器故障
			}
		
			out_num = total_count(bill_fd);
			printf("no bill and out %d Bill!\n",out_num);
			return out_num;
		}else if(ret_date[3] == 'b' && ret_date[7] == cmd[7])
		{
			//出钞完成
			printf("out %d bill!\n",(ret_date[7]-0x30));
			return num;
		}
	}			
	return 0;
}
//清除数据
int clear_errorAndCount(int bill_fd)
{
	
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'I','0','0','0','3'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("clear_errorAndCount write error!");
		return FAILED;
	}
	if( 0x06 ==read_date(bill_fd) )
	{
		printf("clear error and count!\n");
		return OK;
	}else
	{
		return FAILED;
	}
	
	
}
//清除错误信息
int clear_errorMsg(int bill_fd)
{
	
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'I','0','0','0','2'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("clear_errorMsg write error!");
		return -1;
	}
	if( 0x06 == read_date(bill_fd) )
	{
		printf("clear error and count!\n");
		return OK;
	}else
	{
		printf("clear error and count failed!\n");
		return FAILED;
	}
}
//清除总数
int clear_count(int bill_fd)
{
	
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'I','0','0','0','1'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("clear_errorMsg write error!");
		return -1;
	}
	if( 0x06 == read_date(bill_fd) )
	{
		printf("clear count succeed!\n");
		return OK;
	}else
	{
		printf("clear count failed!\n");
		return FAILED;
	}
	
}
//禁能按钮
int key_disable(int bill_fd)
{
	
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'K','1','0','1','0'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("key_disable write error!");
		return -1;
	}
	
	if( 0x06 == read_date(bill_fd) )
	{
		printf("key disable!\n");
		return OK;
	}else
	{
		printf("key disable failed!\n");
		return FAILED;
	}
}
//使能按钮
int key_eable(int bill_fd)
{
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'K','0','0','0','0'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("key_disable write error!");
		return -1;
	}
	
	if( 0x06 == read_date(bill_fd) )
	{
		printf("key eable!\n");
		return OK;
	}else
	{
		printf("key eable failed!\n");
		return FAILED;
	}
}
//清除错误记录
int clear_error(int bill_fd)
{
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'U','0','0','0','0'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("clear_error write error!");
		return -1;
	}

	if( 0x06 == read_date(bill_fd) )
	{
		printf("clear error record!\n");
		return OK;
	}else
	{
		printf("clear error record !\n");
		return FAILED;
	}
}
//取得错误信息
int error_record(int bill_fd)
{
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'R','0','0','0','0'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("error_record write error!");
		return -1;
	}
	memset(ret_date,0,sizeof(ret_date));
	for(i=0;i<sizeof(ret_date);i++)
	{
		ret_date[i] = read_date(bill_fd);
		printf("read_date : %c\n",ret_date[i]);

	}return 0;
}
//查询出钞数目
int total_count(int bill_fd)
{
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'C','0','0','0','0'};
	int ret;
	char result[3];
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("total_count write error!");
		return -1;
	}
	memset(ret_date,0,sizeof(ret_date));
	for(i=0;i<sizeof(ret_date);i++)
	{
		ret_date[i] = read_date(bill_fd);
	}
	result[0] = ret_date[5];
	result[1] = ret_date[6];
	result[2] = ret_date[7];
	printf("out bill count: %c%c%c\n",ret_date[5],ret_date[6],ret_date[7]);
	
	return ret_date[7] - 0x30;
}
//出钞机的总出钞数
int all_bill_count(int bill_fd)
{
	//需要读取两次
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'T','0','0','0','0'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("all_bill_count write error!");
		return -1;
	}
	memset(ret_date,0,sizeof(ret_date));
	for(i=0;i<sizeof(ret_date);i++)
	{
		ret_date[i] = read_date(bill_fd);
		printf("read_date : %x | %c\n",ret_date[i],ret_date[i]);

	}
	printf("\n");
	for(i=0;i<sizeof(ret_date);i++)
	{
		ret_date[i] = read_date(bill_fd);
		printf("read_date : %x | %c\n",ret_date[i],ret_date[i]);

	}
	printf("all bill count!\n");
	return 0;
}
//白卡矫正
//取得按键密码
int get_password(int bill_fd)
{
	//需要读取两次
	tcflush(bill_fd,TCIOFLUSH);
	unsigned char cmd[10] = {0x02,0x30,0x30,'P','0','0','0','0'};
	int ret;
	cmd[8] = checkSum(cmd);
	cmd[9] = 0x03;
	ret = write(bill_fd,cmd,10);
	if(ret < 0)
	{
		perror("get_password write error!");
		return -1;
	}

	for(i=0;i<sizeof(ret_date);i++)
	{
		ret_date[i] = read_date(bill_fd);
	//	printf("read_date : %c\n",ret_date[i]);

	}
	printf("password:%x%x%x\n",ret_date[5],ret_date[6],ret_date[7]); //前三位
	for(i=0;i<sizeof(ret_date);i++)
	{
		ret_date[i] = read_date(bill_fd);
		

	}
	printf("%x%x%x",ret_date[5],ret_date[6],ret_date[7]);//后三位
}

