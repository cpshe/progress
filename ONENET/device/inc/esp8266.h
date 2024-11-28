#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#define     OK		        0	    //������ɱ�־
#define     OUTTIME	        1	    //����δ��ɱ�־


void ESP8266_Clear(void);           //��ջ���

void ESP8266_Init(void);            //esp8266��ʼ��


_Bool ESP8266_SendCmd(char *cmd, char *res);//��������

unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void ESP8266_SendData(unsigned char *data, unsigned short len);

#endif
