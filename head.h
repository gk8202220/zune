#ifndef HEAD_H_
#define HEAD_H_ 
#define OK 0
#define FAILED -1
#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>



//管道通讯
#define WRDATE_FIFO "/tmp/writeDate_fifo"
#define RDDATE_FIFO "/tmp/readDate_fifo"
#define WRCMD_FIFO "/tmp/writecommand_fifo"
#define RDCMD_FIFO "/tmp/readcommand_fifo"

/*
  *QT通信命令和应用通信命令
*/
#define KeyBoardOpen       0x01
#define KeyBoardClose      0x02
#define CameraOpen  	   0x03
#define CameraClose        0x04
#define Scanner_Open       0x05
#define Scanner_Close	   0x06
#define Print_Open         0x07
#define Member_Card_Open   0x08
#define Member_Card_Close  0x09



/*
  *设备节点
*/
#define Scanner_usbPort     0
#define ScannerGun_usbPort  1
#define KeyBoard_usbPort    2
#define display_usbPort     5
#define Print_Path 	     "/dev/ttymxc1"

#define uart1V5_path  "/dev/ttymxc3"
#define accepteBill_path "/dev/ttymxc2"
#define readCard_path  "/dev/ttymxc4"

/*
	出币机的命令
 */
#define MACHINE_BUSY	 0x15
#define LITTLE_COINS 	 0x16
#define MOTOR_PROBLEM	 0x17
#define PLAYOUT_ONE_COIN 0x18
#define PLAYOUT_SUCCEED  0x19
#define CLEAN_COIN		 0x20
#define RESPONSE 		 0x21
#define NO_EXECUTE 		 0x22
#define NO_COIN 		 0x23
#define INPUT_ERROR		 0x24
#define PLAYOUT_OK 		 0x25


/*
	收银设备命令
 */
#define ICT_QURTY 		 100 //查询收银机的状态
#define ICT_NOENBLE 	 0x5e //禁能状态
#define ICT_ENBLE 		 0x3e //使能状态

#define MACHINE_HITCH    0x32 // 机器故障
#define OUTBILL_OK 		 0x33 //找零
#define MACHINE_FINE 	 0x34  //机器正常
#define NO_BILL 		 0x35 //没有钞票
#define SENSOR_FOREIGN	 0x36 //传感器有异物
#define DOUBLE_OUTBILL 	 0x37 //双倍出钞
#define NO_CONNECT	     0x39 //设备未连接


//打印机命令
#define NO_PAPER 0x40
#define LED_RED 10
#define LED_YELLOW 9
#define LED_GREEN 8
#define OFF 1
#define ON 0

int getEvent(int usbPort_num,char *retevent);
int fifo_rd;
int fifo_wr;

#endif
