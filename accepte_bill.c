/*
	功能描述：实现收银机的初始化initIct,使能与禁能ict_enble，
					收银 querymoney()	查看的状态 ict_status()
	作者：朱振展
	联系：442446835@qq.com
	日期：2017.03.23
 */

/*
	3月23号问题描述：接收纸币是，如果出现时间超时，会一直接收钞票
	3.23问题：顾客中途取消付款的处理
 */
#include "accepte_bill.h"
#include "head.h"
#include "read_date.h"
/*
	注意点：
	1.当有数据要接受时，不要过快的发送数据过去，
	不然数据没接收完，就接收到新的数据而产生失误
	2.上电之后会返回0x0,0xf，接着就得向其回应0x02，进行捂手，如果是其他的命令会返回0x21
	3.握手成功在之后会进入使能模式，即查询返回0x3e
	4.适当的使用tcflush来清除旧数据的影响

 */
#define BUF_SIZE 1
int accpetBill_fd;
//使能与禁能#define ICT_NOENBLE 	 0x5e //禁能状态  ICT_ENBLE 		 0x3e //使能状态
int ict_enble(int rcv_command)
{
		
	char  buf;//接收的是16位的数据
	tcflush(accpetBill_fd,TCIOFLUSH);//清空缓存区非常重要，防止旧数据影响正常读取				 		
	//ict_status(accpetBill_fd);
				
	write(accpetBill_fd,&rcv_command,BUF_SIZE);
	buf = read_date(accpetBill_fd);
	if(buf != rcv_command)
	{
		printf("Enble is error!\n");
		return FAILED;
	}

	return OK;
}
int CloseAcceptBill()
{
	ict_enble(ICT_NOENBLE);
	//close(accpetBill_fd);
}

//查询纸币
int getmoney()
{
		/*
			1.没有钱时打印的是3e使能状态
			2.如果拒收
			buf = 1
		buf = f
		buf = 42
		buf = 29
		buf = 2f
		 */
	char  buf;//接收的是16位的数据
	char Money;
	int commemt;
	while(1)
	{
	
		buf =  read_date(accpetBill_fd);		 			 
		//接收到纸币指令0x1
		if( buf== 0x81)
		{
				buf =  read_date(accpetBill_fd);	
			if( buf== 0x8f)
			{	
					Money =  read_date(accpetBill_fd);	
				//发送接收的指令															
				 	printf("buf = %x\n",buf);
				 	commemt = 0x02;
					write(accpetBill_fd,&commemt,BUF_SIZE);
					
					//接收完毕	
					buf =  read_date(accpetBill_fd);	
					if(buf == 0x10)
					{					
						switch(Money)
						{
							case 0x40:
							printf("getmoney : %d\n",1);
							return 1;
							case 0x41:
							printf("getmoney : %d\n",5);
							return 5;
							case 0x42:
							printf("getmoney : %d\n",10);
							return 10;
							case 0x43:
							printf("getmoney : %d\n",20);
							return 20;
							case 0x44:
							printf("getmoney : %d\n",50);
							return 50;
							case 0x45:
							printf("getmoney : %d\n",100);
							return 100;
							default:
							printf("getmoney : %d\n",0);
							return 0;
						}
					}
				
			}		
		}
	}
	return 0;
}

//查看收银机的状态
char ict_status()
{
	
	tcflush(accpetBill_fd,TCIOFLUSH);
	int ret;
	int commemt;
	char  buf;//接收的是16位的数据
	commemt = 0x0c;		 	
	write(accpetBill_fd,&commemt,BUF_SIZE);	
	buf =  read_date(accpetBill_fd);	
	 //	捂手操作
	printf("ict status read date: %x\n",buf);
	
	 	if(buf == 0xa1)
		{
			//捂手
			commemt = 0x02;
 			ret = 	write(accpetBill_fd,&commemt,BUF_SIZE);
 			printf("connecting....%d\n",ret);
 			commemt = 0x0c;		 	
			write(accpetBill_fd,&commemt,BUF_SIZE);	
		buf =  read_date(accpetBill_fd);
 			
		}else if(buf == 0x80)
		{				
		
			//	捂手操作
			buf =  read_date(accpetBill_fd);			
			if( buf== 0x8f)
			{	
					printf("woshou\n");
					commemt = 0x02;
	 			 	write(accpetBill_fd,&commemt,BUF_SIZE);
	 			 	
			}
		}else if(buf== 0x3e)
		{
			printf("status:Enble \n");
		}
		else if(buf== 0x5e)
		{
			printf("status:NoEnble \n");
		}
		else if(buf== 0x81)
		{
			buf =  read_date(accpetBill_fd);
			//接收纸币
			commemt = 0x02;
			write(accpetBill_fd,&commemt,BUF_SIZE);	
			printf("status:have Bill \n",buf);
		}else
		{
			printf("status:The accepte bill cannot work!\n");
		}		
		return buf;			
}

//打开收银设备
int OpenAcceptBill()
{

	printf("open AcceeptBill dev!\n");
	//close(uart1v5_fd);
	//selectUart(5);
	
	if((accpetBill_fd = open_port(accepteBill_path) )< 0)
	{
		perror("open AcceeptBill dev error...");
		return -1;
	}
	//3,2波特率：9600,数据位：8位，偶数校验，1位停止位
	if(set_opt(accpetBill_fd,9600,8,'E',1)<0)
	{
		perror("set_opt AcceeptBill error...");
		return -1;
	}

}
int initIct()
{
		
	printf("init accepte bill buf:%x\n", ict_status());
	//如果是禁能状态，初始化成功	
	return ict_enble(ICT_NOENBLE);				 	

}
