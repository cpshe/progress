/**
 ****************************************************************************************************
 * @file        usart.c
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/TIMER7/timer7.h"
#include "stdarg.h"
#include "stdio.h"	 	 
#include "string.h"	 
/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h" /* os 使用 */
#endif

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */

    USART1->DR = (uint8_t)ch;             /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/

/* 接收缓冲, 最大USART_REC_LEN个字节. */
UART_HandleTypeDef UART1_Handler;

uint8_t USART1_TX_BUF[USART1_MAX_SEND_LEN];

uint8_t USART1_RX_BUF[USART1_MAX_RECV_LEN];

volatile uint16_t USART1_RX_STA = 0;
void USART1_IRQHandler(void)
{
	uint8_t res;
	if((__HAL_USART_GET_FLAG(&UART1_Handler, USART_FLAG_RXNE)!= RESET))
	{
		HAL_UART_Receive(&UART1_Handler, &res,1,HAL_MAX_DELAY);
		if((USART1_RX_STA&(1<<15)) == 0)
		{
			if(USART1_RX_STA < USART1_MAX_RECV_LEN)
			{
				__HAL_TIM_SET_COUNTER(&htim7, 0);
				if(USART1_RX_STA==0)
				{
					HAL_TIM_Base_Start_IT(&htim7);
				}
				USART1_RX_BUF[USART1_RX_STA++] = res;
			}
			else
			{
				USART1_RX_STA |= 1<<15;
			}
		}
	}
}

//void USART3_IRQHandler(void)
//{
//	uint8_t res;
//	if((__HAL_USART_GET_FLAG(&UART1_Handler, USART_FLAG_RXNE)!= RESET))
//	{
//		HAL_UART_Receive(&UART1_Handler, &res,1,HAL_MAX_DELAY);
//		if((USART1_RX_STA & 0x8000)==0)
//		{
//			if(USART1_RX_STA & 0x4000)//接收到了0x0d
//			{
//				if(res != 0x0a)USART1_RX_STA = 0;
//				else USART1_RX_STA |= 0x8000;
//			}
//			else
//			{
//				USART3_RX_BUF[USART1_RX_STA & 0x3FFF] = res;
//				USART1_RX_STA++;
//				if(USART1_RX_STA>(USART3_MAX_RECV_LEN-1))USART1_RX_STA = 0;
//			}
//		}
//	}
//}

void usart1_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_10;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	UART1_Handler.Instance = USART1;
	UART1_Handler.Init.BaudRate = bound;
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;
	UART1_Handler.Init.Parity = UART_PARITY_NONE;
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&UART1_Handler);
	
	__HAL_UART_ENABLE_IT(&UART1_Handler, USART_IT_RXNE);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 2, 3);	
	
	tim7_int_init(99, 7199);
	USART1_RX_STA = 0;
	HAL_TIM_Base_Stop(&htim7);
}

void u1_printf(char* fmt,...)  
{  
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART1_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART1_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(__HAL_USART_GET_FLAG(&UART1_Handler,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		HAL_UART_Transmit(&UART1_Handler,USART1_TX_BUF,i,HAL_MAX_DELAY); 
	} 
}
