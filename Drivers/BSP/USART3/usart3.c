#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/USART3/usart3.h"
#include "stdarg.h"
#include "stdio.h"	 	 
#include "string.h"	 

UART_HandleTypeDef huart3;

void usart3_init(uint32_t baudrate)
{
	huart3.Instance = USART3;
	huart3.Init.BaudRate = baudrate;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&huart3);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;
		
    if (huart->Instance == USART3)                            /* 如果是串口1，进行串口1 MSP初始化 */
    {
				__HAL_RCC_GPIOB_CLK_ENABLE();
				__HAL_RCC_USART3_CLK_ENABLE();
			
        gpio_init_struct.Pin = GPIO_PIN_10;               /* 串口发送引脚号 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* 复用推挽输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO速度设置为高速 */
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);
                
        gpio_init_struct.Pin = GPIO_PIN_11;               /* 串口RX脚 模式设置 */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);   /* 串口RX脚 必须设置成输入模式 */

				HAL_NVIC_EnableIRQ(USART3_IRQn);
				HAL_NVIC_SetPriority(USART3_IRQn,2,3);
		}
}

void USART3_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart3);
}
