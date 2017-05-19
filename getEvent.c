/*
 *功能实现：打开/proc/bus/input下的输入设备信息，通过固定的设备物理地址来获取到对应的event
 *作者：朱振展
 *联系：442446835@qq.com
 *日期：2017.03.09
*/

#include "head.h" 


int getEvent(int usbPort_num,char *retevent)
{
    FILE *file_op;
    char *file_path = "./dev_path.txt";
    char readbuff[254];
    char *result;
    char *s;  
    int i;
   char *command="cat /proc/bus/input/devices > dev_path.txt" ;

    char *usbPort[] = {"usb-fsl-ehci.1-1.1/input0","usb-fsl-ehci.1-1.2.1/input0","usb-fsl-ehci.1-1.2.2/input0","usb-fsl-ehci.1-1.2.3/input0","usb-fsl-ehci.1-1.2.4/input0","eGalaxTouch Virtual Device for Single"};

     system(command); 
    file_op = fopen(file_path, "r");  
    if (file_op == NULL)  
    {  
    printf("can't open %s\n", file_path);  
    return -1;  
    } 
   
    while(fgets(readbuff,sizeof(readbuff),file_op) != NULL)
    {
        //如果找到该设备的物理地址，就获取对应的event
        //扫描枪
        
        if(strstr(readbuff,usbPort[usbPort_num]) != NULL)
        {
            //继续读取三次得到event所在的行      
            i = 3;
            while(i--)
            fgets(readbuff,sizeof(readbuff),file_op);
            //对得到的字符串进行分解 H: Handlers=sysrq kbd event3
             s =  strdup(readbuff);
             result = strsep(&s," ");
          while( strstr(result,"event") == NULL)
            result = strsep(&s," "); 
            break;     
         
        }
       
    }
if(result == NULL)
{
  printf("%s event is no connet!\n",usbPort[usbPort_num]);
  return FAILED;
}else
{  
    strcat(retevent,result);
}
   
    fclose(file_op);

 return OK;

}
