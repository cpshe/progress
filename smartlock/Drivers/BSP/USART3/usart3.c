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
		
    if (huart->Instance == USART3)                            /* ����Ǵ���1�����д���1 MSP��ʼ�� */
    {
				__HAL_RCC_GPIOB_CLK_ENABLE();
				__HAL_RCC_USART3_CLK_ENABLE();
			
        gpio_init_struct.Pin = GPIO_PIN_10;               /* ���ڷ������ź� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* IO�ٶ�����Ϊ���� */
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);
                
        gpio_init_struct.Pin = GPIO_PIN_11;               /* ����RX�� ģʽ���� */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;    
        HAL_GPIO_Init(GPIOB, &gpio_init_struct);   /* ����RX�� �������ó�����ģʽ */

				HAL_NVIC_EnableIRQ(USART3_IRQn);
				HAL_NVIC_SetPriority(USART3_IRQn,2,3);
		}
}

void USART3_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart3);
}
