/*
	1.要对数据操作进行时刻的保存
	2.清币信号的发送，设计清币按钮
	3.发送吐币命令后，如果要进行一个查询，要给个时间sleep(2)（至少两秒）让机器先去执行命令
	4.如果超时的次数超过一定的值，将该机器判为无法工作
 */


#include "uartApi.h"

#include "gpio.h"
#include "out_coin.h"
#include "head.h"
#include "read_date.h"

#define CLEANCOIN_GPIO 11

int i;


//最后一位的校验码
static char checkSum(char*cmd)
{
	int i;
	char retdata = 0 ;
	for(i = 0;i<5;i++)
	{
		retdata +=cmd[i];
	} 
	return retdata;
}
//出指定数量的币
int outCoin(int coin_fd,int num)
{
	char ret_date[6];
	//出币的数量要在1~9之间
	if(1>num || num >9)
	{
		printf("input coin number error!\n");
		return 0;
	}

	char cmd[7] = {0x05,0x10,0x00,0x10};
	int ret;
	unsigned char *data;
	cmd[4] = num;
	cmd[5] = checkSum(cmd);
	// while( machine_status(coin_fd) == MACHINE_BUSY);
	// if( machine_status(coin_fd)  != MACHINE_FINE)
	// {
	// 	//记录错误的信息
	// 	printf("the machine is no coins!\n");
	// 	return num;
	// }
	ret = write(coin_fd,cmd,7);
	
	if(ret < 0)
	{
		//记录错误的信息
		perror("playcoin error!");
		return num;
	}
	memset(ret_date,0,sizeof(ret_date));
	for(i=0;i<6;i++)
	{
		ret_date[i] = read_date(coin_fd);
		if(ret_date[2] ==  0xff)return 0;		
	}
	//发送命令之后还有一段缓冲的时间
	//币不足够
	if(ret_date[3] != 0xbb)
	{	
		//币前期足够,进行出币后的查询
		sleep(2);//保证在吐币机已经接收到了命令
		 while( machine_status(coin_fd) == MACHINE_BUSY);
	//	 sleep(1);
		 
		 remain_coin(coin_fd);
 		memset(ret_date,0,sizeof(ret_date));
		for(i=0;i<6;i++)
		{
			ret_date[i] = read_date(coin_fd);
			//printf("read_date : %x\n",ret_date[i]);
		}
		// printf("ret_date[3] = %x,ret_date[4] = %x\n",ret_date[3],ret_date[4]);
		if(ret_date[3] == 0xaa) 
		{
			
			printf("ret_datep[4] = %x\n",ret_date[4]);
			if(ret_date[4] == 0x00)
			{//出币成功
				printf("playout %d coin ok!\n",num);
				return num;
			}else 
			{
				//出币不完整，需要添加到笔记,机器进行一个重启，查询状态
				//如果实在没有币了，就得打印小票出来
				
				coin_reset(coin_fd);//重启；
				//记录，并打印小票
				printf("no coin!,still have %d coins no playout!\n",ret_date[4]);
				return num - ret_date[4];
				
			}			
		}		
		//出币失败
	}else
	{
		if(ret_date[4] == 0x00)
		{	
			//没有足够的币
			printf("machine little coins!\n");
			return 0;
		}else
		{
			//正在出币
			printf("playouting........\n");
			return 0;
		}
		
	}
	return 0;	
}

//硬币机重启
int coin_reset(int coin_fd)
{
	
	char cmd[7] = {0x05,0x10,0x00,0x12,0x00,0x27};
	int ret;
	ret = write(coin_fd,cmd,7);
	if(ret < 0)
	{
		perror("coin_reset write error!");
		return -1;
	}
	printf("reset coin machine... \n" );
	return 0;

}
//查询机器状态
int machine_status(int coin_fd)
{
	int ret;
	char ret_date[6];
	unsigned char cmd[7] = {0x05,0x10,0x00,0x11,0x00,0x26};	
	ret = write(coin_fd,cmd,7);
	if(ret < 0)
	{
		perror("machine_status write error!");
		return -1;
	}
	memset(ret_date,0,sizeof(ret_date));
	for(i=0;i<6;i++)
	{
		ret_date[i] = read_date(coin_fd);
		if(ret_date[1] == 0xff)return NO_CONNECT;
		//因为机器刚开机，得不到数据，所以要不断的发送查询的数据
		//printf("read_date : %x\n",ret_date[i]);		
		
	}
	
	return analyse(ret_date);
}

//查询剩余状态
int remain_coin(int coin_fd)
{
	
	tcflush(coin_fd,TCIOFLUSH);
	unsigned char cmd[7] = {0x05,0x10,0x00,0x13,0x00,0x28};
	int ret;
	
	ret = write(coin_fd,cmd,7);
	if(ret < 0)
	{
		perror("coin_reset write error!");
		return -1;
	}
	
	return 0;

}
//如果下面的数据解析中是错误的状态，将记录下来
int analyse(unsigned char *data)
{
	//只会返回一个状态，对于data[4]有数据的在此处不予处理
	//printf("data 3 = %x data 4 = %x\n",data[3],data[4]);
	if(data[3] == 0x04)
	{
		if(data[4] == 0x00)
		{
			//正常状态
			printf("machine is fine!\n");
			return MACHINE_FINE;

		}else if((data[4]&0x02) == 1)
		{
			//硬币不足
			printf("machine little coins!\n");
			return  LITTLE_COINS;
		}else if (data[4] == 0x20)
		{
			//出幣機於開機檢查未完成若接收到Master命令則回應忙
			printf("MACHINE_BUSY!\n");
			return MACHINE_BUSY;
		}
		else
		{
			//机器故障
			printf("machine have problem!\n");
			return MOTOR_PROBLEM;
		}	
		
	}else if(data[3] == 0x07 && data[4] == 0x00)
	{
		//出口一个币
		printf("playout one coin!\n");
		return PLAYOUT_ONE_COIN;
		
	}else if(data[3] == 0x08 && data[4] == 0x00)
	{
		//出币成功
		printf("playout succeed!\n");
		return PLAYOUT_SUCCEED;
	}else if(data[3] == 0x09  && data[4] == 0x00)
	{
		//收到清币的指令会返回的代码
		printf(" clearing coin....\n");
		return CLEAN_COIN;
	}else if(data[3] == 0xAA)
	{
		//当没有足够的币能出时会返回此代码，
		//1.当出币成功或者出币的途中没有币再出时data[4]代表本来全部要出的数
		//2.出币不成功如果此时进行查询，data[4]代表剩余未出的数
		//3.出币成功 如果此时进行查询，data[4]为0 。//这是查询到的数据05 01 00 AA 00 B0
		if(data[4] == 0x00)
		{
			
			return PLAYOUT_OK;
		}else
		{
			return RESPONSE;
		}
		
	}else if(data[3] == 0xBB)
	{
		//1.Data=Remain coin(s)當上一條出幣命令沒完成時又收到另一條出幣命令時回傳“NAK”，
		//2.剩余的币不多（一般低于35个币）data[4] == 0不允許執行
		return NO_EXECUTE;
	}else
	{
		printf("没有连接!,%d,%x\n",NO_CONNECT,NO_CONNECT);
		return NO_CONNECT;
	}
}
//清币操作
int clear_coin(int coin_fd)
{
	
	//int fd_gpio = -1;	
	int i;
	int coin_count = 0;
	char ret_date[6];
	// fd_gpio = open(DEV_GPIO, O_RDWR);
	// if (fd_gpio < 0) 
 //    {
	// 	DEBUG_ERROR("open gpio device %s error!\n", DEV_GPIO);
	// 	return RET_FAILED;
	// }
	//发送清币的电平
	tcflush(coin_fd,TCIOFLUSH);
	set_one_gpio_level(GPIO_LEVEL_HIGHT,CLEANCOIN_GPIO);
    sleep(1);
    set_one_gpio_level(GPIO_LEVEL_LOW,CLEANCOIN_GPIO);
	  memset(ret_date,0,sizeof(ret_date));
	  for(i=0;i<6;i++)
		{
			ret_date[i] = read_date(coin_fd);
			//因为机器刚开机，得不到数据，所以要不断的发送查询的数据		
			printf("read_date : %x\n",ret_date[i]);			
		}
		if(ret_date[3] == 0x09 && ret_date[4] == 0x00)
		{
			printf("clear coin!\n");
			coin_count = 0;
			while(1)
				{
					for(i=0;i<6;i++)
					{
						ret_date[i] = read_date(coin_fd);
						if(ret_date[i] == 0xff)i = -1;
						//因为机器刚开机，得不到数据，所以要不断的发送查询的数据		
						printf("read_date : %x\n",ret_date[i]);			
					}
					if(ret_date[3] == 07 && ret_date[4] == 00)
						printf("coin:%d\n",coin_count++);
					else if(ret_date[3] == 04 && ret_date[4] == 00)	
						return NO_COIN; //没有币
					else if(ret_date[3] == 04 && ret_date[4] == 02)					
						{
							printf("playout:%d coins\n",coin_count ); //清币结束
							break;

						}
				}
			
		}
	//close(fd_gpio);
    return 0;
  
}

