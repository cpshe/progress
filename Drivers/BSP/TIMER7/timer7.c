#include "./BSP/LED/led.h"
#include "./BSP/TIMER7/timer7.h"
#include "./SYSTEM/usart/usart.h"

TIM_HandleTypeDef	htim7;

extern volatile uint16_t USART1_RX_STA;

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
void tim7_int_init(uint16_t arr, uint16_t psc)
{
		TIM7_INT_CLK_ENABLE();

		htim7.Instance = TIM7_INT;
		htim7.Init.Period = arr;
		htim7.Init.Prescaler = psc;
		htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	
		if(HAL_TIM_Base_Init(&htim7) != HAL_OK)
		{
			Error_Handler();
		}
	
		HAL_TIM_Base_Start_IT(&htim7);
	
		HAL_NVIC_SetPriority(TIM7_INT_IRQn, 0, 2); /* ��ռ1�������ȼ�3����2 */
		HAL_NVIC_EnableIRQ(TIM7_INT_IRQn);         /* ����ITM3�ж� */


}
/**
 * @brief       ��ʱ��TIMX�жϷ�����
 * @param       ��
 * @retval      ��
 */
void TIM7_INT_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)!=RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(&htim7,TIM_IT_UPDATE) != RESET)
		{
			 USART1_RX_STA |= 1 << 15;  // ��ǽ������
			__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
			HAL_TIM_Base_Stop(&htim7);  // ֹͣ TIM7
		}
	}
}

void Error_Handler(void)
{
	while(1)
	{
	}
}
