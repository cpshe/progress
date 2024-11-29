#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#define     OK		        0	    //接收完成标志
#define     OUTTIME	        1	    //接收未完成标志


void ESP8266_Clear(void);           //清空缓存

void ESP8266_Init(void);            //esp8266初始化


_Bool ESP8266_SendCmd(char *cmd, char *res);//发送数据

unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void ESP8266_SendData(unsigned char *data, unsigned short len);

#endif
