#ifndef UARTAPI_H_
#define UARTAPI_H_
//设置
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);
//打开设备
int open_port(char *comport);

#endif