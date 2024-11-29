#include "./BSP/LED/led.h"
#include "./BSP/TIMER7/timer7.h"
#include "./SYSTEM/usart/usart.h"

TIM_HandleTypeDef	htim7;

extern volatile uint16_t USART1_RX_STA;

/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
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
	
		HAL_NVIC_SetPriority(TIM7_INT_IRQn, 0, 2); /* 抢占1，子优先级3，组2 */
		HAL_NVIC_EnableIRQ(TIM7_INT_IRQn);         /* 开启ITM3中断 */


}
/**
 * @brief       定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void TIM7_INT_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE)!=RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(&htim7,TIM_IT_UPDATE) != RESET)
		{
			 USART1_RX_STA |= 1 << 15;  // 标记接收完成
			__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
			HAL_TIM_Base_Stop(&htim7);  // 停止 TIM7
		}
	}
}

void Error_Handler(void)
{
	while(1)
	{
	}
}
