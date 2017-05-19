#ifndef LIBPRINTER_H_
#define LIBPRINTER_H_



// 打开打印机设备
// 参数 ： dev   打印机驱动名称
int openPrinter(char *dev);
//初始化，支持中文,GBK232模式
int init_printer_dev();

//关闭打印机设备
int closePrinter(void);

//打印内容
// 参数 ： str   打印内容
// 参数 ： len   字符串长度
int writePrinter(const char *str, int len);
//打印条形码
int print_bar(char *numBar);
//确定打印缓冲区并进纸
int  confirmPrint();
//切割纸张
int  cutPaper();
//设置中文模式
int setPrinterModeChinese(void);

//复位打印机
int resetPriter(void);

//设置正常字符大小
int setCharSizeNormal();
//设置大字字符
int setCharSizeBig();
//打印位图
// 参数 ： width   位图宽度
// 参数 ： high    位图高度
// 参数 ： dat     位图数据
int writePrinterDraw(unsigned char width, unsigned char high, unsigned char *dat);
//确定打印缓冲区并进纸N行  ，num：行数（1~255）
int  printAndNewLineN(int num);
//确定打印缓冲区并换行
int printAndNewLine();
void  *printer(void *args);
int  getPrinterState();
#endif

