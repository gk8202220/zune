/*
	2017/4/14 bug:进卡模式：开关+磁信号 （已修复）
 */

#include "uartApi.h"
#include <dlfcn.h>
#include "head.h"

#include "TYRMCOMMU10.H"
char msg[1024] = {0};
int  InitReadCarder()
{
	int ret;
	ret = OpenDevice(readCard_path,msg);//打开设备
	if(ret == 0)
	{
		printf("open readCardDevice succees!\n");
	}
	else
	{
		printf("open readCardDevice ERROR!\n");
	}
	ret = InitDevice(2,msg);//初始化
	if(ret < 0)
	{
		//如果初始化失败，
		printf("readCard initDeice error !\n");
		return -1;
	}
	return 0;
}
int outCard()
{
	int ret;
	int cardStatus;
	ret = GetDeviceStatus(&cardStatus,msg);//卡的状态
	if(cardStatus == 2)
	{
	ret = Eject(msg);//退卡
	}
	ret = DisEntry (msg);//禁止进卡
	if(ret <0) 
	{
		printf("%s\n",msg );
	}
	printf("out card ret = %d\n",ret );
	return ret;
}
void *Card_Reader(void *args)
{

	int cardStatus;
	int ret;
	char ReData[1024] = {0};
	char sendQtDate[1024] = {"member:"};
	ret = InitDevice(2,msg);//初始化
	if(ret < 0)
	{
		//如果初始化失败，
		printf("readCard initDeice error !\n");
	}
	
	ret = CallEntry(msg);//允许插卡
	do
	{
		pthread_testcancel();
	ret = GetDeviceStatus(&cardStatus,msg);//卡的状态
	}while(cardStatus != 2);
	ret = ReadTracks(2,ReData,msg); //读取第二磁道
	ret = Eject(msg);//退卡
	ret = DisEntry (msg);//禁止进卡

	strcat(sendQtDate,ReData);
	sendQtDate[strlen(sendQtDate)] = '#';
	printf("DEV>> ReadcardId :  %s\n",sendQtDate);
	ret = write(fifo_wr, sendQtDate, strlen (sendQtDate));	/* 写管道 */	
	if(ret < 0)
	{
		printf("card send to QT  error:%s!\n",sendQtDate);
	}
	return;

}



