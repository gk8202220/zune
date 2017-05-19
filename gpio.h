/*

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef __ASM_ARCH_GPIO_H
#define __ASM_ARCH_GPIO_H

#define MAX_GPIO_BANKS		5
#define NR_BUILTIN_GPIO		(MAX_GPIO_BANKS * 32)

#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))

#define DEBUG 1

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#ifdef DEBUG
  #define DEBUG_INFO(fmt, args...)  fprintf(stderr, fmt, ##args) 
  #define DEBUG_ERROR(fmt, args...) fprintf(stderr, fmt"%s\t%s\t%d\n", ##args, __FILE__,__FUNCTION__,__LINE__)
#else
  #define DEBUG_INFO(fmt, args...) 
  #define DEBUG_ERROR(fmt, args...)
#endif        

#define RET_SUCCESS                 0
#define RET_FAILED                  -1
#define BUF_SIZE                    512

/* ioctl cmd */
#define	IOCTL_GPIO_LEVEL_GET	        _IOW('Q', 0x02, int)   	//get gpio state
#define	IOCTL_GPIO_LEVEL_SET            _IOR('Q', 0x03, int)	//set gpio output state
#define	IOCTL_GPIO_DIR_INPUT_SET	    _IOW('Q', 0x04, int)	//set gpio as a input
  

#define GPIO_LEVEL_LOW                  0
#define GPIO_LEVEL_HIGHT                1

typedef struct {
	char *gpio_name;
	int  gpio_index;
} GPIO_ATTR_ST;
 
typedef struct {
	int  gpio_index;
	int  gpio_level;
} IOCTL_GPIO_ARG_ST;


#define DEV_GPIO                    "/dev/qiyang_imx6_gpio"


#define	IMX_GPIO2_0	    IMX_GPIO_NR(2 , 0)
#define	IMX_GPIO2_1	    IMX_GPIO_NR(2 , 1)
#define	IMX_GPIO2_2	    IMX_GPIO_NR(2 , 2)
#define	IMX_GPIO2_3	    IMX_GPIO_NR(2 , 3)
#define	IMX_GPIO2_4	    IMX_GPIO_NR(2 , 4)
#define	IMX_GPIO2_5	    IMX_GPIO_NR(2 , 5)
#define	IMX_GPIO2_6	    IMX_GPIO_NR(2 , 6)
#define	IMX_GPIO2_7	    IMX_GPIO_NR(2 , 7)

#define	IMX_GPIO2_16	IMX_GPIO_NR(2 , 16)
#define	IMX_GPIO2_17	IMX_GPIO_NR(2 , 17)
#define	IMX_GPIO2_18	IMX_GPIO_NR(2 , 18)
#define	IMX_GPIO2_19	IMX_GPIO_NR(2 , 19)
#define	IMX_GPIO2_20	IMX_GPIO_NR(2 , 20)
#define	IMX_GPIO2_21	IMX_GPIO_NR(2 , 21)
#define	IMX_GPIO2_22	IMX_GPIO_NR(2 , 22)
#define	IMX_GPIO2_23	IMX_GPIO_NR(2 , 23)

 int set_one_gpio_level( int gpio_level,int gpio_num);

int init_gpio();
#endif

