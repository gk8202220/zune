#ifndef ATM_H_
#define ATM_H_ 


//机器状态
int bill_status(int bill_fd);
//出钞
int out_bill50(int bill_fd);
int out_bill10(int bill_fd,int num);
//清除数据
int clear_errorAndCount(int bill_fd);
//禁能按钮
int key_disable(int bill_fd);
//清除错误记录
int clear_error(int bill_fd);
//取得错误信息
int error_record(int bill_fd);
//查询出钞数目
int total_count(int bill_fd);
//出钞机的总出钞数
int all_bill_count(int bill_fd);
//取得按键密码
int get_password(int bill_fd);
//清除错误信息
int clear_errorMsg(int bill_fd);
//清除总数
int clear_count(int bill_fd);
#endif