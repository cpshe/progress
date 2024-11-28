/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ������ʱ�� ��������
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
 * V1.0 20211216
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "./BSP/TIMER/btim.h"
#include "./SYSTEM/usart/usart.h"

TIM_HandleTypeDef htim6;  /* ��ʱ����� */

extern volatile uint16_t USART2_RX_STA;

/**
 * @brief       ������ʱ��TIMX��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ36M, ���Զ�ʱ��ʱ�� = 72Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void tim6_int_init(uint16_t arr, uint16_t psc)
{
    htim6.Instance = TIM6_INT;                      /* ͨ�ö�ʱ��X */
    htim6.Init.Prescaler = psc;                          /* ����Ԥ��Ƶϵ�� */
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;         /* ��������ģʽ */
    htim6.Init.Period = arr;                             /* �Զ�װ��ֵ */
    HAL_TIM_Base_Init(&htim6);
	
    HAL_TIM_Base_Start_IT(&htim6);    /* ʹ�ܶ�ʱ��x��������ж� */

}
/**
 * @brief       ��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
                �˺����ᱻHAL_TIM_Base_Init()��������
 * @param       htim:��ʱ�����
 * @retval      ��
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6_INT)
    {
        TIM6_INT_CLK_ENABLE();                     /* ʹ��TIMʱ�� */
        HAL_NVIC_SetPriority(TIM6_INT_IRQn, 0, 2); /* ��ռ1�������ȼ�3����2 */
        HAL_NVIC_EnableIRQ(TIM6_INT_IRQn);         /* ����ITM3�ж� */
    }
}

/**
 * @brief       ��ʱ��TIMX�жϷ�����
 * @param       ��
 * @retval      ��
 */
void TIM6_INT_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&htim6, TIM_FLAG_UPDATE)!=RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(&htim6,TIM_IT_UPDATE) != RESET)
		{
			 USART2_RX_STA |= 1 << 15;  // ��ǽ������
			__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
			HAL_TIM_Base_Stop(&htim6); 
		}
	}
}
