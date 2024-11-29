#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/USART2/usart2.h"
#include "stdarg.h"
#include "stdio.h"	 	 
#include "string.h"	 

UART_HandleTypeDef USART2_InitStructure;
uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			  //发送缓冲,最大USART2_MAX_SEND_LEN字节

volatile uint16_t USART2_RX_STA = 0;

void USART2_IRQHandler(void)
{
	uint8_t res;
	if(__HAL_USART_GET_FLAG(&USART2_InitStructure,USART_FLAG_RXNE) != RESET)
	{
		 HAL_UART_Receive(&USART2_InitStructure, &res, 1, HAL_MAX_DELAY);
        if ((USART2_RX_STA & (1 << 15)) == 0) // 判断是否接收完的一批数据
        {
            if (USART2_RX_STA < USART2_MAX_RECV_LEN) // 还可以接收数据
            {
                __HAL_TIM_SET_COUNTER(&htim6, 0); // 清空计数器

                if (USART2_RX_STA == 0) // 如果是第一次接收，则使能定时器6
                {
                    HAL_TIM_Base_Start_IT(&htim6); // 启动定时器6
                }

                USART2_RX_BUF[USART2_RX_STA++] = res; // 记录接收到的值
            }
            else
            {
                USART2_RX_STA |= (1 << 15); // 强制标记接收完成
            }
        }
	}
	
}

void usart2_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART2_InitStructure.Instance = USART2;
	USART2_InitStructure.Init.BaudRate = bound;
	USART2_InitStructure.Init.StopBits = USART_STOPBITS_1;
	USART2_InitStructure.Init.WordLength = USART_WORDLENGTH_8B;
	USART2_InitStructure.Init.Parity = UART_PARITY_NONE;
	USART2_InitStructure.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	USART2_InitStructure.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&USART2_InitStructure);                                           /* HAL_UART_Init()会使能UART1 */
		
	__HAL_UART_ENABLE_IT(&USART2_InitStructure, UART_IT_RXNE);
	
	HAL_NVIC_EnableIRQ(USART2_IRQn);                      /* 使能USART1中断通道 */
	HAL_NVIC_SetPriority(USART2_IRQn, 2, 3);              /* 组2，最低优先级:抢占优先级3，子优先级3 */

	tim6_int_init(99,7199);
	USART2_RX_STA = 0;
	HAL_TIM_Base_Stop(&htim6);
}

//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)  
{  
	uint16_t i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(__HAL_USART_GET_FLAG(&USART2_InitStructure,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		HAL_UART_Transmit(&USART2_InitStructure,USART2_TX_BUF,i,HAL_MAX_DELAY); 
	} 
}











