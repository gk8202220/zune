/*
	名称：设备的主程序，接收QT的命令和放回数据
	功能：1.检测设备
		2.打开设备
		3.关闭设备
 */

#include "money.h"
#include "head.h"
#include <pthread.h>
#include "print.h"
#include "readCarder.h"
#include "gpio.h"

#define FIFO_SERVER "/tmp/w_fifo"
#define FIFO_CUSTOM "/tmp/r_fifo"


int allMachine_status()
{
	money_status();
}
/*
//单独创建扫描仪的线程
void open_scanner()
{
	
	
	int ret;
	
	pthread_t scanner_pid;
	pthread_attr_t scanner_pthattr;
	pthread_t scannerGun_pid;
	pthread_attr_t scannerGun_pthattr;

	

	pthread_attr_init(&scanner_pthattr);
	pthread_attr_setdetachstate(&scanner_pthattr,PTHREAD_CREATE_DETACHED); 
	ret = pthread_create(&scanner_pid,&scanner_pthattr,scanner,(void*)Scanner_usbPort);
	if(ret != 0)printf("creat printer pthread error!\n");
	
		
	pthread_attr_init(&scannerGun_pthattr);
	pthread_attr_setdetachstate(&scannerGun_pthattr,PTHREAD_CREATE_DETACHED); 
	ret = pthread_create(&scannerGun_pid,&scannerGun_pthattr,scanner,(void*)ScannerGun_usbPort);
	if(ret != 0)printf("creat printer pthread error!\n");

}

*/
int allMachine_init()
{

	
	init_gpio();//1.GPIO初始化
	init_momey_dev();//现金设备初始化
	init_printer_dev();
	InitReadCarder();
	
}

//开机检测，把情况发送给QT界面
int main()
{
pthread_t money_pid;
pthread_attr_t money_pthattr;
pthread_t printer_pid;
pthread_attr_t printer_pthattr;

pthread_t readCarder_pid;
pthread_attr_t readCarder_pthattr;
pthread_t Keyboard_pid;
pthread_attr_t Keyboard_pthattr;
int ret;
float buyMoney;	
//char *event1;
char *str;
char *result;
char *readFifoBuf[256];
	//1.打开与QT通讯的管道
	if ((fifo_rd = open(FIFO_SERVER,O_RDONLY) )<0)
	{
		perror("open read fifo failed!");
		return FAILED;	
	}
	if((fifo_wr = open(FIFO_CUSTOM,O_WRONLY)) < 0)
	{
		perror("open write fifo failed!");
		return FAILED;
	}
	//2.进行设备的检测
	allMachine_init();

	set_one_gpio_level(GPIO_LEVEL_LOW,LED_GREEN);
	set_one_gpio_level(GPIO_LEVEL_LOW,LED_RED);
	set_one_gpio_level(GPIO_LEVEL_LOW,LED_YELLOW);
	

	while(1)
	{
		//循环接受QT的命令
		memset(readFifoBuf,0,sizeof(readFifoBuf));
		ret = read(fifo_rd,readFifoBuf,sizeof(readFifoBuf));
		if(ret >0)
		{
			printf("DEV>> Recv QT commemd:");
			printf("%s\n",readFifoBuf);
		}		
	//现金设备的线程
if((strstr(readFifoBuf,"money:") ) != NULL)
	{
	
		printf("DEV >> Money pthead creat!\n");
		str =  strdup(readFifoBuf);
        result = strsep(&str,":");       
        buyMoney = atof(str);
        printf("%f\n", buyMoney);
		pthread_attr_init(&money_pthattr);
		pthread_attr_setdetachstate(&money_pthattr,PTHREAD_CREATE_DETACHED);
		ret=pthread_create(&money_pid,&money_pthattr,money_sale,(void *)&buyMoney);
	    if(ret!=0)
	    printf("creat pthread error!\n");
 	}

	
	if((strstr(readFifoBuf,"cash_register_close") ) != NULL )
	{
		printf("DEV >> Money  pthead exit!\n");
		
		ret = pthread_cancel(money_pid);
		if(ret != 0)printf("exit Money pthread error!\n");
		closeMoneyDevice();
		
		
	}
	//打印机的线程
	
	if((strstr(readFifoBuf,"print_open") )!= NULL)
	{
	printf("DEV >> Print pthead creat!\n");
	pthread_attr_init(&printer_pthattr);
	pthread_attr_setdetachstate(&printer_pthattr,PTHREAD_CREATE_DETACHED); 
	ret = pthread_create(&printer_pid,&printer_pthattr,printer,NULL);
	if(ret != 0)printf("creat printer pthread error!\n");
	}


	//打开读卡器设备
	if((strstr(readFifoBuf,"member_card_open") ) != NULL )
	{
	 printf("DEV >> ReadCard pthead creat!\n");

	ret = pthread_create(&readCarder_pid,NULL,Card_Reader,NULL);
	if(ret != 0)printf("creat Card_Reader pthread error!\n");
	
	}

	if((strstr(readFifoBuf,"member_card_close") ) != NULL )
	{
		printf("DEV >> ReadCard pthead exit!\n");
		
		ret = pthread_cancel(readCarder_pid);
		if(ret != 0)printf("exit Card_Reader pthread error!\n");
		outCard();
		
	}
	


if((strstr(readFifoBuf,"yellow_lamp") ) != NULL )
	{

	set_one_gpio_level(GPIO_LEVEL_LOW,LED_GREEN);
	set_one_gpio_level(GPIO_LEVEL_LOW,LED_RED);
	set_one_gpio_level(GPIO_LEVEL_HIGHT,LED_YELLOW);
	}
if((strstr(readFifoBuf,"red_lamp") ) != NULL )
	{
	set_one_gpio_level(GPIO_LEVEL_LOW,LED_GREEN);
	set_one_gpio_level(GPIO_LEVEL_LOW,LED_YELLOW);
	set_one_gpio_level(GPIO_LEVEL_HIGHT,LED_RED );
	}
if((strstr(readFifoBuf,"green_lamp") ) != NULL )
	{
	set_one_gpio_level(GPIO_LEVEL_LOW,LED_YELLOW);
	set_one_gpio_level(GPIO_LEVEL_LOW,LED_RED);
	set_one_gpio_level(GPIO_LEVEL_HIGHT,LED_GREEN);
	}



	}

}
