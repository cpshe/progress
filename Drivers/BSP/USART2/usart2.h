#ifndef __USART2_H
#define __USART2_H	 

#include "./SYSTEM/sys/sys.h"

#define USART2_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART2_RX_EN 			1					//0,不接收;1,接收.

extern uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN];
extern uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN];
extern volatile uint16_t USART2_RX_STA;

void usart2_init(uint32_t bound);				//串口2初始化 
void u2_printf(char* fmt,...);

#endif

