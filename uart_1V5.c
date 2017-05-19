/*
	功能：串口1扩5
	作者：朱振展
	联系：424446835@qq.com
	日期：2017.3.25

	说明：单通道模式
		对应的GPIO控制口
		 T1 :GPIO2_6
		 T2 :GPIO2_4
		 T3 :GPIO2_2 
		 R1 :GPIO2_0  
		 R2 :GPIO2_5
		 R3 :GPIO2_3
		 串口连接：UART3

 */
#include "gpio.h"
#include "head.h"
int selectUart(int num)
{
	// int fd_gpio = -1;	
	// fd_gpio = open(DEV_GPIO, O_RDWR);
	// if (fd_gpio < 0) 
 //    {
	// 	DEBUG_ERROR("open gpio device %s error!\n", DEV_GPIO);
	// 	return RET_FAILED;
	// }
	

		// set_one_gpio_level(GPIO_LEVEL_LOW,1);
		// sleep(1);
		// set_one_gpio_level(GPIO_LEVEL_HIGHT,1);
		

	switch(num)
	{
		case 1:
		set_one_gpio_level(GPIO_LEVEL_LOW,2);
		set_one_gpio_level(GPIO_LEVEL_LOW,4);		
		set_one_gpio_level(GPIO_LEVEL_HIGHT,6);
		set_one_gpio_level(GPIO_LEVEL_LOW,3);
		set_one_gpio_level(GPIO_LEVEL_LOW,5);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,0);
		//printf("select Uart 5-----1\n");
		break;
		case 2:
		set_one_gpio_level(GPIO_LEVEL_LOW,2);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,4);		
		set_one_gpio_level(GPIO_LEVEL_LOW,6);
		set_one_gpio_level(GPIO_LEVEL_LOW,3);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,5);
		set_one_gpio_level(GPIO_LEVEL_LOW,0);
		//printf("select Uart 5-----2\n");
		break;
		case 3:
	
		set_one_gpio_level(GPIO_LEVEL_LOW,2);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,4);		
		set_one_gpio_level(GPIO_LEVEL_HIGHT,6);
		set_one_gpio_level(GPIO_LEVEL_LOW,3);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,5);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,0);
		//printf("select Uart 5-----3\n");
		break;
		case 4:
	
		set_one_gpio_level(GPIO_LEVEL_HIGHT,2);
		set_one_gpio_level(GPIO_LEVEL_LOW,4);		
		set_one_gpio_level(GPIO_LEVEL_LOW,6);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,3);
		set_one_gpio_level(GPIO_LEVEL_LOW,5);
		set_one_gpio_level(GPIO_LEVEL_LOW,0);
		//printf("select Uart 5-----4\n");
		break;
		case 5:
		
		set_one_gpio_level(GPIO_LEVEL_HIGHT,2);
		set_one_gpio_level(GPIO_LEVEL_LOW,4);		
		set_one_gpio_level(GPIO_LEVEL_HIGHT,6);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,3);
		set_one_gpio_level(GPIO_LEVEL_LOW,5);
		set_one_gpio_level(GPIO_LEVEL_HIGHT,0);
		//printf("select Uart 5-----5\n");
		break;
   		

	}

	
}

