#ifndef OUTCOIN_H_
#define OUTCOIN_H_ 

//出指定数量的币
int outCoin(int coin_fd,int num);
//硬币机重启
int coin_reset(int coin_fd);
//查询机器状态
int machine_status(int coin_fd);
//查询剩余状态
int remain_coin(int coin_fd);
//清币操作
int clear_coin(int coin_fd);

#endif