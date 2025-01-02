#include "./SYSTEM/usart/usart.h"
#include "./BSP/OV7670/ov7670.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/EXTI/exti.h"

uint8_t ov_sta;

// �ⲿ�ж����ų�ʼ��
void EXTI8_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    EXTI_HandleTypeDef EXTI_Handle = {0};

    // ���� GPIOA ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // ���� PA8 ����Ϊ�ⲿ�ж�ģʽ
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;    // ����Ϊ�����ش����ж�
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // ����������������
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);         // ��ʼ�� PA8 ����

    // ���� NVIC �ж�
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);  // �����ж����ȼ�
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);           // ʹ���ⲿ�ж�ͨ�� EXTI9_5_IRQn

    // ���� EXTI �ж���
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);       // �����ⲿ�жϴ�����
}

// �ⲿ�жϴ����� (EXTI9_5_IRQn �жϴ���)
void EXTI9_5_IRQHandler(void)
{
    // ���� HAL ���ṩ���жϴ�����
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8); // ���� PA8 ���ŵ��ж�
}

// �ⲿ�ж������־λ
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_8)  // ����� PA8 ���Ŵ������ж�
    {
        OV7670_WRST(0);    // ��λдָ��
        OV7670_WRST(1);    // �ָ�дָ��
        OV7670_WREN(1);    // ����д�� FIFO
        ov_sta++;           // ֡�жϼ�����1
    }
}
