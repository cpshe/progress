/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-06-05
 * @brief       串口初始化代码(一般是串口1)，支持printf
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211103
 * 第一次发布
 * V1.1 20230605
 * 删除USART_UX_IRQHandler()函数的超时处理和修改HAL_UART_RxCpltCallback()
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 和 串口 定义 
 * 默认是针对USART1的.
 * 注意: 通过修改这几个宏定义,可以支持USART1~UART5任意一个串口.
 */
#define USART1_MAX_RECV_LEN		1024					//最大接收缓存字节数
#define USART1_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART1_RX_EN 			1					//0,不接收;1,接收.

extern UART_HandleTypeDef UART1_Handler;

extern uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN];
extern uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN];
extern volatile uint16_t USART1_RX_STA ;

void usart1_init(uint32_t bound);
void USART1_IRQnHandle(void);
void u1_printf(char* fmt,...);

#endif


