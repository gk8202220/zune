//////////////////////////////////////////////////////////////////////////
//	文件名：TYRMCOMMU10.H
//	功能：银联商务多媒体自助终端硬件驱动接口规范读卡器部分
//	时间：2017-5-03
//////////////////////////////////////////////////////////////////////////

#ifndef TYRMCOMMU_H_
#define TYRMCOMMU_H_

#ifdef __cplusplus
extern "C" {
#endif

//磁卡结构
typedef struct _TRACKINFO
{
	char	Contents[3][256];
	int		Lengths[3];
	int 	Status[3];
} TRACKINFO, *PTRACKINFO;

	//获取驱动版本函数
	//返回版本信息长度
	//出口参数：VersMes-版本信息
	 int GetDllVersion(char* VersMes);

	//打开读卡器函数
	//成功返回0，错误返回状态码
	//入口参数：nPort-端口号; -1表示从配置文件(.ini)中获取,COM1填1，以此类推,USB1填1，以此类推
	//出口参数：sMes-错误信息
	int OpenDevice(char * nPort, char* sMes);

	//关闭读卡器函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int CloseDevice(char* sMes);

    //发送无参数指令
    //chCMD:命令
    //chPARA:参数
    //pszReadData:收到的数据
    //nReadLen:收到的数据长度
     int SendData(unsigned char chCMD, unsigned char chPARA,unsigned char * pszReadData,int* nReadLen);

    //发送多数指令
     int SendParaData(char command , char para,unsigned char paraData[],int paraDataLen,unsigned char * pszReadData,int *nReadLen);

	//读卡器初始化函数
	//成功返回0，错误返回状态码
	//入口参数：Option-卡操作选项;如果读卡器里（含卡机口）有卡时:1 弹卡;2 吞卡;3 保持
	//出口参数：sMes-错误信息
	 int InitDevice (int Option,char* sMes);

	//获取读卡器固件版本函数始化函数
	//返回版本信息长度
	//出口参数：VerMessage-版本信息
	//			sMes-错误信息
	 int GetFirewareVersion(char* VerMessage, char* sMes);

	//读卡器功能能力函数
	//成功返回0，错误返回状态码
	//出口参数：CardOption-卡操作选项;位1  1支持磁卡;
	//										0不支持磁卡;
	//								  位2  1支持CPU卡;
	//										0不支持CPU卡;
	//								  位3  1支持RF卡;
	//										0不支持RF卡;
	//								  位4  1支持SIM卡;
	//										0不支持SIM卡;
	//			iDeviceType-读卡器类型;1 手动读卡器；2 电动读卡器；3 模块(纯电路板，无电机、锁卡等装置)
	//			sMes-错误信息
	 int ReaderCapabilities(int* CardOption,int* iDeviceType,char* sMes);

	//获得读卡器状态函数版本函数始化函数
	//成功返回0，错误返回状态码
	//出口参数：iCardStatus-有无卡信息；0 无卡；1 前段有卡；2 内部有卡
	//			sMes-错误信息
	 int GetDeviceStatus(int *iCardStatus,char * sMes);

	//允许插卡函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int CallEntry(char* sMes);

	//禁止插卡函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int DisEntry (char* sMes);

	//退卡函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int Eject(char* sMes);

	//吞卡函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int Capture(char* sMes);


	//读磁卡函数
	//成功返回0，错误返回状态码
	//入口参数：track_id-磁道号;1 读1轨;2 读2轨;3 读3轨
	//出口参数：track_datas-磁道的数据
	//			sMes-错误信息
	 int ReadTracks(int track_id, char *track_datas, char* sMes);

    //读磁卡函数
	//成功返回0，错误返回状态码
	//入口参数：track_id-磁道号;1 读1轨;2 读2轨;3 读3轨, 4 读12轨;5 读23轨；6 读13轨；7读123轨
	//出口参数：track_datas-磁道的数据
     int ReadTracksR1(	int	track_id,	PTRACKINFO track_datas, char* sMes);

	//CPU卡上电激活函数
	//成功返回0，错误返回状态码
	//出口参数：AtrData-上电复位后返回的卡片信息
	//			AtrDataLen-上电复位后返回的卡片信息长度
	//			sMes-错误信息
	 int CPUPowerOn(unsigned char* AtrData, int* AtrDataLen,char* sMes);

	//CPU卡芯片数据交换函数
	//成功返回0，错误返回状态码
	//入口参数：InDataBuf-存放输入数据
	//			InLen-存放输入的数据长度
	//出口参数：OutDataBuf-存放输出的数据
	//			OutLen-存放输出的数据长度
	//			sMes-错误信息
	 int CPUChipIo(unsigned char* InDataBuf, int InLen, unsigned char* OutDataBuf, int* OutLen, char* sMes);

	//CPU卡下电函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int CPUPowerOff(char* sMes);


	//RF卡上电激活函数
	//成功返回0，错误返回状态码
	//出口参数：AtrData-上电复位后返回的卡片信息
	//			AtrDataLen-上电复位后返回的卡片信息长度
	//			sMes-错误信息
	 int RFPowerOn(unsigned char* AtrData, int* AtrDataLen,char* sMes);

	//RF卡芯片数据交换函数
	//成功返回0，错误返回状态码
	//入口参数：InDataBuf-存放输入数据
	//			InLen-存放输入的数据长度
	//出口参数：OutDataBuf-存放输出的数据
	//			OutLen-存放输出的数据长度
	//			sMes-错误信息
	 int RFChipIo(unsigned char* InDataBuf, int InLen, unsigned char* OutDataBuf, int *OutLen, char* sMes);

	//RF卡下电函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int RFPowerOff(char* sMes);


	//SIM卡座选择函数
	//成功返回0，错误返回状态码
	//入口参数：SlotNumber-选择将要操作的SIM卡座序号，序号从1开始
	//出口参数：sMes-错误信息
	 int SIMSelectSlot(int SlotNumber, char* sMes);

	//SIM卡上电激活函数
	//成功返回0，错误返回状态码
	//出口参数：AtrData-上电复位后返回的卡片信息
	//			AtrDataLen-上电复位后返回的卡片信息长度
	//			sMes-错误信息
	 int SIMPowerOn(unsigned char* AtrData,int *AtrDataLen, char* sMes);

	//SIM卡芯片数据交换函数
	//成功返回0，错误返回状态码
	//入口参数：InDataBuf-存放输入数据
	//			InLen-存放输入的数据长度
	//出口参数：OutDataBuf-存放输出的数据
	//			OutLen-存放输出的数据长度
	//			sMes-错误信息
	 int SIMChipIo(unsigned char* InDataBuf, int InLen, unsigned char* OutDataBuf, int *OutLen, char* sMes);

	//SIM卡下电函数
	//成功返回0，错误返回状态码
	//出口参数：sMes-错误信息
	 int SIMPowerOff(char* sMes);

     int TD3SendData(unsigned char tdID,unsigned char chCMD, unsigned char chPARA,unsigned char * pszReadData,int* nReadLen);

     int TD3SendParaData(char command , char para,unsigned char paraData[],int paraDataLen,unsigned char * pszReadData,int *nReadLen);

#ifdef __cplusplus
}
#endif

#endif /*TYRMCOMMU_H_*/

