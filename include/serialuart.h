#ifndef SERIALUART_H
#define SERIALUART_H

#include     "../include/data.h"


#define FALSE 1
#define TRUE 0

extern AppStruct App;

void set_speed(int fd, int speed);
int set_Parity(int fd,int databits,int stopbits,int parity);
void *Uartthread(void *arg);



#endif
