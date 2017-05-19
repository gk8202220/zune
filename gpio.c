#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#include "gpio.h"
#include "head.h"
int fd_gpio;
static GPIO_ATTR_ST gpios[] = {
	{"IMX_GPIO2_0", IMX_GPIO2_0}, {"IMX_GPIO2_1",IMX_GPIO2_1},
	{"IMX_GPIO2_2", IMX_GPIO2_2}, {"IMX_GPIO2_3",IMX_GPIO2_3},
	{"IMX_GPIO2_4", IMX_GPIO2_4}, {"IMX_GPIO2_5",IMX_GPIO2_5},
	{"IMX_GPIO2_6", IMX_GPIO2_6}, {"IMX_GPIO2_7",IMX_GPIO2_7},
	
	{"IMX_GPIO2_16", IMX_GPIO2_16}, {"IMX_GPIO2_17",IMX_GPIO2_17},
	{"IMX_GPIO2_18", IMX_GPIO2_18}, {"IMX_GPIO2_19",IMX_GPIO2_19},
	{"IMX_GPIO2_20", IMX_GPIO2_20}, {"IMX_GPIO2_21",IMX_GPIO2_21},
	{"IMX_GPIO2_22", IMX_GPIO2_22}, {"IMX_GPIO2_23",IMX_GPIO2_23},
};

//通过输入一个值来找到对应的gpio名字
static char *get_gpio_name(int gpio_index)
{
    int i;
    
    for (i = 0; i < ARRAY_SIZE(gpios); i++) 
    {
        if (gpio_index == gpios[i].gpio_index) 
        {
            return gpios[i].gpio_name;
        }
    }

    return NULL;
}

//设置电平的输出状态
static int gpio_set_level(int fd, 	IOCTL_GPIO_ARG_ST *parg)
{
    int i;
    int ret = RET_SUCCESS;

    if (NULL == parg)
	{
        DEBUG_ERROR("Invalid argument!\n"); 
        return RET_FAILED;
	}

	if (NULL == get_gpio_name(parg->gpio_index))
	{
        DEBUG_ERROR("Invalid argument!\n"); 
        return RET_FAILED;
	}

	if ((GPIO_LEVEL_LOW != parg->gpio_level) &&
	    (GPIO_LEVEL_HIGHT != parg->gpio_level))
    {
        DEBUG_ERROR("Invalid argument!\n"); 
        return RET_FAILED;
	}
	
    ret = ioctl(fd, IOCTL_GPIO_LEVEL_SET, parg);
    if (0 != ret)
    {
        ret = RET_FAILED;
    	//DEBUG_ERROR("gpio '%s' 'IOCTL_GPIO_LEVEL_SET' '%d' error!errno = %d\n", 
    //	            get_gpio_name(parg->gpio_index), parg->gpio_level, ret);
    }
    else
    {
      // DEBUG_INFO("set gpio '%s' level '%d'\n", get_gpio_name(parg->gpio_index), parg->gpio_level);
    }

   return ret;
}
//得到电平
static int gpio_get_level(int fd, 	IOCTL_GPIO_ARG_ST *parg)
{
    int i;
    int ret = RET_SUCCESS;
    
    if (NULL == parg)
	{
        DEBUG_ERROR("Invalid argument!\n"); 
        return RET_FAILED;
	}

	if (NULL == get_gpio_name(parg->gpio_index))
	{
        DEBUG_ERROR("Invalid argument!\n"); 
        return RET_FAILED;
	}

    ret = ioctl(fd, IOCTL_GPIO_LEVEL_GET, parg);
    if (0 != ret)
    {
        ret = RET_FAILED;
    	DEBUG_ERROR("gpio '%s' 'IOCTL_GPIO_LEVEL_GET' error!errno = %d\n", 
    	            get_gpio_name(parg->gpio_index), ret);
    }
    else
    {
       // DEBUG_INFO("get gpio '%s' level '%d'\n", 
                  // get_gpio_name(parg->gpio_index), parg->gpio_level);
    }

   return ret;
}
//设置输入
static int gpio_dir_set_input(int fd, IOCTL_GPIO_ARG_ST *parg)
{
    int ret = RET_SUCCESS;
    
    if (NULL == parg)
	{
        DEBUG_ERROR("Invalid argument!\n"); 
        return RET_FAILED;
	}

	if (NULL == get_gpio_name(parg->gpio_index))
	{
        DEBUG_ERROR("Invalid argument!\n"); 
        return RET_FAILED;
	} 

	ret = ioctl(fd, IOCTL_GPIO_DIR_INPUT_SET, parg);
    if (0 != ret)
    {
        ret = RET_FAILED;
    	DEBUG_ERROR("gpio '%s' 'IOCTL_GPIO_DIR_INPUT_SET' error!errno = %d\n", 
    	            get_gpio_name(parg->gpio_index), ret);
    }
    
    return ret;
}

//将所有的设置成输入模式，并得到电平
static int gpio_test_connect(int fd)
{
    int i;
    int ret;
    int final_ret = RET_SUCCESS;
	IOCTL_GPIO_ARG_ST arg;
	
    memset (&arg, 0, sizeof(arg));

    for (i = 0; i < ARRAY_SIZE(gpios); i++)
    {
        arg.gpio_index = gpios[i].gpio_index; 
        ret = gpio_dir_set_input(fd, &arg);
        if (RET_SUCCESS == ret)
        {
            memset (&arg, 0, sizeof(arg));
            arg.gpio_index = gpios[i].gpio_index; 
            ret = gpio_get_level(fd, &arg);
            if ((RET_SUCCESS == ret))
            {
               //DEBUG_INFO("get gpio '%s' level '%d'\n", 
                         //  gpios[i].gpio_name, arg.gpio_level);
            }
        }
        
        final_ret += ret;
    }
    return final_ret;
}

//设置指定GPIO口的电平
 int set_one_gpio_level(int gpio_level,int gpio_num)
{
  
    int ret;

    IOCTL_GPIO_ARG_ST arg;
    
    memset (&arg, 0, sizeof(arg));
  
        arg.gpio_index = gpios[gpio_num].gpio_index; 
        arg.gpio_level = gpio_level;
        ret = gpio_set_level(fd_gpio, &arg);     
   


    return ret;    
}

int init_gpio()
{
        fd_gpio = open(DEV_GPIO, O_RDWR);
    if (fd_gpio < 0) 
    {
        DEBUG_ERROR("open gpio device %s error!\n", DEV_GPIO);
        return -1;
    }
    return 0;
}

// int main(int argc, char *argv[])
// {
	
	
// 	int fd_gpio = -1;	
//     int ctrl;
// 	fd_gpio = open(DEV_GPIO, O_RDWR);
// 	if (fd_gpio < 0) 
//     {
// 		DEBUG_ERROR("open gpio device %s error!\n", DEV_GPIO);
// 		return RET_FAILED;
// 	}
//     while(1)
//     {
//         scanf("%d",&ctrl);
//         switch (ctrl)
//         {
//             case 1:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,6);
//             break;
//             case 2:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_HIGHT,6);
//             break;
//             case 3:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,7);
//             break;
//             case 4:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_HIGHT,7);
//             break;
//             case 5:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,8);
//             break;
//             case 6:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_HIGHT,8);
//             break;
//              case 0:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,6);
//               set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,7);
//               set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,8);
//             break;
//              case 7:
//                set_one_gpio_level(fd_gpio,GPIO_LEVEL_HIGHT,9);
//                sleep(1);
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,9);
//             break;
//             case 8:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_LOW,9);
//              break;
//             case 9:
//              set_one_gpio_level(fd_gpio,GPIO_LEVEL_HIGHT,9);
//              break;

//         }
//     }
  

// 	close(fd_gpio);

// 	return 0;
// }

