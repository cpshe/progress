/**
 ****************************************************************************************************
 * @file        usart.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.1
 * @date        2023-06-05
 * @brief       ���ڳ�ʼ������(һ���Ǵ���1)��֧��printf
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211103
 * ��һ�η���
 * V1.1 20230605
 * ɾ��USART_UX_IRQHandler()�����ĳ�ʱ������޸�HAL_UART_RxCpltCallback()
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
/* ���ʹ��os,����������ͷ�ļ�����. */
#if SYS_SUPPORT_OS
#include "os.h" /* os ʹ�� */
#endif

/******************************************************************************************/
/* �������´���, ֧��printf����, ������Ҫѡ��use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* ʹ��AC6������ʱ */
__asm(".global __use_no_semihosting\n\t");  /* ������ʹ�ð�����ģʽ */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6����Ҫ����main����Ϊ�޲�����ʽ�����򲿷����̿��ܳ��ְ�����ģʽ */

#else
/* ʹ��AC5������ʱ, Ҫ�����ﶨ��__FILE �� ��ʹ�ð�����ģʽ */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* ��ʹ�ð�����ģʽ��������Ҫ�ض���_ttywrch\_sys_exit\_sys_command_string����,��ͬʱ����AC6��AC5ģʽ */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* ����_sys_exit()�Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE �� stdio.h���涨��. */
FILE __stdout;

/* MDK����Ҫ�ض���fputc����, printf�������ջ�ͨ������fputc����ַ��������� */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */

    USART1->DR = (uint8_t)ch;             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    return ch;
}
#endif
/******************************************************************************************/

/* ���ջ���, ���USART_REC_LEN���ֽ�. */
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
//			if(USART1_RX_STA & 0x4000)//���յ���0x0d
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
	i=strlen((const char*)USART1_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(__HAL_USART_GET_FLAG(&UART1_Handler,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		HAL_UART_Transmit(&UART1_Handler,USART1_TX_BUF,i,HAL_MAX_DELAY); 
	} 
}
