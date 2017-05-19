#ifndef ATM_H_
#define ATM_H_ 
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

unsigned char read_date(int read_fd);
#endif