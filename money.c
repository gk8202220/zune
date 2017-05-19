#include "out_bill.h"
#include "out_coin.h"
#include "head.h"
#include "accepte_bill.h"
#include "uart_1V5.h"
/*
	1.故障状态的上报
	2.有变量来保存是否还有纸币
	3.现金设备不能使用的条件：10元或者1毛的面额低于9张将不再使用现金设备
	3.1 50元的面额没有，不能找零50元的
	3.2 5毛的面额没有，不能找零50元的
	#define uart1V5_path  "/dev/ttymxc3"
	#define outBill_path     "/dev/ttymxc2"
	#define outCoin_path     "/dev/ttymxc1"
	#define accepteBill_path "/dev/ttymxc4"
	4.收银机的设备在uart1扩5板子上不能正常使用,
	5.收银机放在其他设备读取数据会发生数据读错（fd传错的原因）
	6.找零设备成功找零的结果统一为已经找零多少钱，防止某个机器没有找零，而结果已经找零成功
	7.出钞机如果没有钞，第一次重启将失败
 */


//所有的面额剩余的数量，在操作的过程中随时上报情况，保存在文本或者数据库里
int remain_b50;
int remain_b10;
int remain_c10;
int remain_c5;
int remain_c1;

#define TRUE 1
#define FALSE 0
#define uart1v5_10Bill 5  //10元串口
#define uart1v5_50Bill 1  //50元串口
#define uart1v5_1Coin 3   //0.1元串口
#define uart1v5_5Coin 4   //0.5元串口 
#define uart1v5_10Coin 2  //1元串口
//int outbill_fd; 
//int outcoin_fd;

int uart1v5_fd;
int alac(float val);


//打开50元的串口
int openOut50Bill_dev()
{
	printf("open out 50 bill dev!\n");
	close(uart1v5_fd);
	selectUart(uart1v5_50Bill);
	uart1v5_fd = open_port(uart1V5_path);
	if(uart1v5_fd  < 0)
	{
		perror("open out 50 bill dev error...");
		return -1;
	}
	// //波特率：9600,数据位：8位，无校验，1位停止位
	if(set_opt(uart1v5_fd,9600,8,'N',1)<0)
	{
		perror("set_opt out 50 bill error...");
		return -1;
	}
	

}
//打开10元的串口
int openOut10Bill_dev()
{
	printf("open out 10 bill dev!\n");
	close(uart1v5_fd);
	selectUart(uart1v5_10Bill);
	uart1v5_fd = open_port(uart1V5_path);
	if(uart1v5_fd  < 0)
	{
		perror("open out 10 bill dev error...");
		return -1;
	}
	// //波特率：9600,数据位：8位，无校验，1位停止位
	if(set_opt(uart1v5_fd,9600,8,'N',1)<0)
	{
		perror("set_opt  out 10 bill error...");
		return -1;
	}
	

}
//打开1元的硬币机
int openOut10Coin_dev()
{
	printf("open out 10 Coin dev!\n");
	close(uart1v5_fd);
	selectUart(uart1v5_10Coin);
	if((uart1v5_fd = open_port(uart1V5_path) )< 0)
	{
		perror("open out 10 Coin dev error...");
		return -1;
	}
	//2.2波特率：9600,数据位：8位，偶数校验，1位停止位
	if(set_opt(uart1v5_fd,9600,8,'E',1)<0)
	{
		perror("set_opt out 10 Coin error...");
		return -1;
	}


}
//打开5毛的硬币机
int openOut5Coin_dev()
{
	printf("open out 5 Coin dev!\n");
	close(uart1v5_fd);
	selectUart(uart1v5_5Coin);
	if((uart1v5_fd = open_port(uart1V5_path) )< 0)
	{
		perror("open out 5 Coin dev error...");
		return -1;
	}
	//2.2波特率：9600,数据位：8位，偶数校验，1位停止位
	if(set_opt(uart1v5_fd,9600,8,'E',1)<0)
	{
		perror("set_opt out 5 Coin error...");
		return -1;
	}


}

//打开1毛的硬币
int openOut1Coin_dev()
{
	printf("open out 1 Coin dev!\n");
	close(uart1v5_fd);
	selectUart(uart1v5_1Coin);
	if((uart1v5_fd = open_port(uart1V5_path) )< 0)
	{
		perror("open out 1 Coin dev error...");
		return -1;
	}
	//2.2波特率：9600,数据位：8位，偶数校验，1位停止位
	if(set_opt(uart1v5_fd,9600,8,'E',1)<0)
	{
		perror("set_opt out 1 Coin error...");
		return -1;
	}


}


int init_momey_dev()

{	
	char ret;
	
	char sendDate[256] = {"moneyStatus:"};
	
	
	OpenAcceptBill();//1打开收银设备
	//3.3初始化,进入禁能状态
	if(FAILED == initIct())
	{
		printf("init accepte bill failed!\n");
		//记录到文件里面
	}else
	{
			printf("Init \"accepte_bill\" succeed!!\n");
	}

	openOut1Coin_dev();//打开0.1元设备

	sendDate[strlen(sendDate)]  =  ret = machine_status(uart1v5_fd);
	if(ret == NO_CONNECT)
	{
		printf("0.1元的机器没有连接，请检查线路！\n");

	}else
	{

		while(machine_status(uart1v5_fd) == MACHINE_BUSY);
			
		if(machine_status(uart1v5_fd)!= MACHINE_FINE)
		{				
			//故障处理				 
			//如果不是正常状态且不是忙状态				
			printf("The 0.1 coin machine have problem!\n");
			//return MOTOR_PROBLEM;
		}
		printf("Open and init out 1 coin succeed!\n");
		//2.3开机检测,检测太快的话，发送的指令不能被吐币机收到
	}

	openOut5Coin_dev();//打开0.5元设备
	
	sendDate[strlen(sendDate)]  = ret = machine_status(uart1v5_fd);
	if(ret == NO_CONNECT)
	{
		printf("0.5元的机器没有连接，请检查线路！\n");

	}else
	{

		while(( machine_status(uart1v5_fd)) == MACHINE_BUSY);

		if(machine_status(uart1v5_fd)!= MACHINE_FINE)
		{				
		//故障处理				 
		//如果不是正常状态且不是忙状态				
		printf("The 0.5 coin machine have problem!\n");
		//return MOTOR_PROBLEM;
		}

		printf("Open and init out 0.5 coin succeed!\n");
	}
	
openOut10Coin_dev();//打开1元设备
	sendDate[strlen(sendDate)]  =  ret = machine_status(uart1v5_fd);

	if(ret == NO_CONNECT)
	{
		printf("1元的机器没有连接，请检查线路！\n");

	}else
	{
		while(machine_status(uart1v5_fd) ==  MACHINE_BUSY);
		//2.3开机检测,检测太快的话，发送的指令不能被吐币机收到
		if(machine_status(uart1v5_fd)!= MACHINE_FINE)
		{				
			//故障处理				 
			//如果不是正常状态且不是忙状态				
			printf("The coin machine have problem!\n");
			//return MOTOR_PROBLEM;
		}
		printf("Open and init out 10 coin succeed!\n");
	}

	openOut10Bill_dev();//打开10元设备
	sendDate[strlen(sendDate)] =  ret = bill_status(uart1v5_fd);
	if(ret == NO_CONNECT)
	{
		printf("10元的机器没有连接，请检查线路！\n");
	}
	if(ret != MACHINE_FINE)
	{
		sleep(1);
		//上交错误信息
		ret = clear_errorAndCount(uart1v5_fd);
		if(ret != OK)
		{
			//上交错误信息
			//clear_errorAndCount(outbill_fd);
			printf("The out 10 bill mechine cannot work!\n");
			//return ret;
		}
		
	}else
	{
		 printf("Open and init out 10 bill succeed!\n");
	}

	openOut50Bill_dev();//打开50元设备
	//strcat(statusBuf,statusStr);
	sendDate[strlen(sendDate)]  = ret = bill_status(uart1v5_fd);
	//strcat(statusBuf,statusStr);
	if(ret == NO_CONNECT)
	{
		printf("50元的机器没有连接，请检查线路！\n");
	}
	if(ret != MACHINE_FINE)
	{
		//上交错误信息
		sleep(1);
		ret = clear_errorAndCount(uart1v5_fd);	
		if(ret != OK)
		{
			//上交错误信息			
			printf("The out 50 bill mechine cannot work!\n");
			//return ret;
		}

		
	}else
	{
			printf("Open and init out 50 bill succeed!\n");
	}

/*
	sendDate[strlen(sendDate)]  = '#';
ret = write(fifo_wr, sendDate, strlen (sendDate));// 写管道 
	if(ret < 0)
	{
		printf("card send to QT  error:%s!\n",sendDate);
	}
*/
	return 0;
}
//关闭现金设备
int closeMoneyDevice()
{
	CloseAcceptBill();
	close(uart1v5_fd);
}
void *money_sale(void *arg)
{
	float goods_price = *(float *)arg;
	int getmoney_count = 0;
	int ret;
	char wrfiBuf[254]  = {0};
	char flo[20];
	char *getmoneystr = "money:";
	float remain_money;
	//char *remainMoneyStr = "outmoney:";
	char *remainMoneyStr = "cash_unregister_close#";
	int recveMoney = 0;

	//1，接收来自管理员放入的纸币和硬币数量 
	//money_status();
	
	//得到购买的金额
		
	//开始接收纸币
	ict_enble(ICT_ENBLE);
	
	
	while(1)
	{
		memset(wrfiBuf,0,sizeof(wrfiBuf));
		recveMoney = getmoney();
		getmoney_count += recveMoney;
		//发送给QT放入的金额
		sprintf(flo,"%d", recveMoney );
		strcat(wrfiBuf,getmoneystr);
		strcat(wrfiBuf,flo);
		wrfiBuf[strlen(wrfiBuf)] = '#';
		ret = write(fifo_wr, wrfiBuf, strlen(wrfiBuf));	// 写管道 	
		if(ret < 0)
		{
			printf("card send to QT  error:%s!\n",wrfiBuf);
		}

		//实时汇报数据
		if(getmoney_count >=goods_price)
		{
			//关闭收银
			printf("已放入 = %d\n",getmoney_count );			
			ict_enble(ICT_NOENBLE);
			break;
		}

	}
	memset(flo,0,sizeof(flo));
	memset(wrfiBuf,0,sizeof(wrfiBuf));

	printf("需找零金额:%f\n",getmoney_count - goods_price );
	remain_money  = alac( getmoney_count - goods_price);
	//发送给QT找零的金额
//	sprintf(flo,"%f", remain_money );
//	printf("float to char%s\n", flo);
//	strcat(wrfiBuf,remainMoneyStr);
//	strcat(wrfiBuf,flo);
	ret = write(fifo_wr, remainMoneyStr, strlen (remainMoneyStr));	// 写管道 /	
	if(ret < 0)
	{
		printf("card send to QT  error:%s!\n",wrfiBuf);
	}
	
	//money_status();
	return;
	//找零完成，提交完成的命令

}
//机器的工作状态查询
int money_status()
{
	
	int ret;
	char sendDate[256] = {"moneyStatus:"};
	//收银机的状态
	ret = ict_status();	
	if(ret == ICT_NOENBLE)
	{
		printf("收银机一切正常!\n");
		//exist_acceptbill = TRUE;
	}
	else
	{			
		//exist_acceptbill = FALSE;
		printf("收银机不能正常使用!\n");
	}
		
	openOut1Coin_dev();
	while(machine_status(uart1v5_fd) == MACHINE_BUSY);
	//1毛的状态查询
	sendDate[strlen(sendDate)] = ret = machine_status(uart1v5_fd);	
	if(ret  == MACHINE_FINE)
	{	
		printf("1毛硬币机正常!\n");
		//exist_c1 = TRUE;
	}else if (ret == LITTLE_COINS)
	{
		printf("1毛的币不足\n");
		//exist_c1 = FALSE;
	}
	else
	{
		printf("1毛的硬币机出现问题！\n");
		//exist_c1 = FALSE;
	}
	openOut5Coin_dev();
	//5毛的状态查询
	sendDate[strlen(sendDate)] = ret = machine_status(uart1v5_fd);	
	if(ret  == MACHINE_FINE)
	{	
		
		printf("5毛硬币机正常!\n");
		//exist_c5 = TRUE;
	}else if (ret == LITTLE_COINS)
	{
		printf("5毛的币不足\n");
		//exist_c5 = FALSE;
	}
	else
	{
		printf("5毛的硬币机出现问题！\n");
		//exist_c5 = FALSE;
	}
	openOut10Coin_dev();
	//1元的状态查询
	sendDate[strlen(sendDate)] = ret = machine_status(uart1v5_fd);	
	if(ret  == MACHINE_FINE)
	{	
		printf("1元硬币机正常!\n");
		//exist_c10 = TRUE;
	}
	else if (ret == LITTLE_COINS)
	{
		printf("1元的币不足\n");
		//exist_c10 = FALSE;
	}else
	{
		printf("1元的硬币机出现问题！\n");
		//exist_c10 = FALSE;
	}
	openOut10Bill_dev();
	//10元纸币状态查询
	sendDate[strlen(sendDate)] = ret = bill_status(uart1v5_fd);	
	if(ret  == MACHINE_FINE)
	{	
		printf("10元出钞机正常!\n");
		//exist_b10 = TRUE;
	}
	else
	{
		printf("10元的出钞机出现问题！\n");
		//exist_b10 = FALSE;
	}
	openOut50Bill_dev();
	//50元纸币状态查询
	sendDate[strlen(sendDate)] = ret = bill_status(uart1v5_fd);	
	if(ret  == MACHINE_FINE)
	{	
		printf("50元出钞机正常!\n");
		
		//exist_b50 = TRUE;
	}
	else
	{
		printf("50元的出钞机出现问题！\n");
		//exist_b50 = FALSE;
	}
	
	sendDate[strlen(sendDate)] = '#';
	printf(sendDate);
	ret = write(fifo_wr, sendDate, strlen (sendDate));	// 写管道 		
	if(ret < 0)
	{
		printf("card send to QT  error:%s!\n",sendDate);
	}

	return OK;

}

int alac(float val)
{
	
	int bill = 0;
	int single = 0 ;
	int playout[5] = {0};
	int outB10_remain = 0;
	int outC10_remain = 0;
	int outC5_remain  = 0;
	int outC1_remain  = 0;		
	int outB10_neet = 0;
	int outC10_neet = 0;
	int outC5_neet  = 0;
	int outC1_neet  = 0;		
	//是否大于50 且 是否有50的面额找零
	bill = (int)val; 
	//50元面额找零
	if(val >50 )
	{
		openOut50Bill_dev();
		//出50元的
		printf("DEV>> Need 50 RMB number:1 \n");
		if(out_bill50(uart1v5_fd) == 1)
		{
			bill -=  50;
			printf("DEV>> Layout 50 RMB number:1 \n");
			
		}else
		{
			printf("DEV>> No layout 50 RMB number:1 \n");
		}
			
	}
	//10元面额找零
	outB10_neet = bill / 10;
	if(outB10_neet >9) outB10_neet = 9;
	if(outB10_neet >0 )
	{
		openOut10Bill_dev();
		printf("DEV>> Need 10 RMB number:%d\n",outB10_neet);
		outB10_remain = out_bill10(uart1v5_fd,outB10_neet); //已经出了多少张
		printf("DEV>> Layout 10 RMB number:%d\n",outB10_remain);
	}
	bill -= outB10_remain*10; //从未找零的总额中减去已找零额
	outC10_neet = bill % 10;
	if(outC10_neet >5)
	{
		single  = (outC10_neet - 5)*10;
		bill -=(outC10_neet - 5);
		outC10_neet = 5;	
	}
	//1元面额找零
	if((outC10_neet > 0))
	{
		openOut10Coin_dev();
		printf("DEV>> Need 1 RMB number:%d\n",outC10_neet);
		outC10_remain = outCoin(uart1v5_fd,outC10_neet); //已经吐出
		printf("DEV>> Layout 1 RMB number:%d\n", outC10_remain);
	}
	bill -=outC10_remain;

	
	//分解出小数位并使其成为整数	
	single += (val - ((int)val)) *11;
	//将1元的换成5毛的找	
	single += (outC10_neet-outC10_remain)*10;
	bill -= (outC10_neet-outC10_remain); 
	
	outC5_neet += (single/5);
	if(outC5_neet >9) outC5_neet = 9;
	//5毛面额找零
	if(outC5_neet > 0)
	{
		openOut5Coin_dev();
		printf("DEV>> Need 0.5 RMB number:%d\n",outC5_neet);
		outC5_remain = outCoin(uart1v5_fd,outC5_neet);
		printf("DEV>> Layout 0.5 RMB number:%d\n",outC5_remain);
	}
	single  -= outC5_remain*5;
	if(single >9)outC1_neet = 9;
	else 
	{
		outC1_neet = single;
	}

	//1毛面额找零
	if(outC1_neet > 0 )
	{
		openOut1Coin_dev();
		printf("DEV>> Need 0.1 RMB number:%d\n", outC1_neet);
		outC1_remain  = outCoin(uart1v5_fd,outC1_neet);
		printf("DEV>> Layout 0.1 RMB number:%d\n", outC1_remain);
	}

	single  -= outC1_remain;
	printf("No layout money:%f\n",bill+(float)single/10);
	return bill+(float)single/10;	
}
