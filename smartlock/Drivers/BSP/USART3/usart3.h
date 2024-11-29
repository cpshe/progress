#ifndef __USART3_H
#define __USART3_H

#include "./SYSTEM/sys/sys.h"

extern UART_HandleTypeDef huart3;

void usart3_init(uint32_t baudrate);
void USART3_IRQnHandle(void);

#endif
