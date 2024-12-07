#include "./BSP/TIM2/time2.h"
#include "./SYSTEM/delay/delay.h"

TIM_HandleTypeDef htim2;  /* 定时器句柄 */


uint32_t lock_time = 0;
#define LOCKUP_TIME 20

uint8_t LOCKUP_START = 0;

void time2_init(uint16_t arr, uint16_t psc)
{
	 __HAL_RCC_TIM2_CLK_ENABLE(); 
	htim2.Instance = TIM2;
	htim2.Init.Period = psc;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Prescaler = arr;
	HAL_TIM_Base_Init(&htim2);

	HAL_TIM_Base_Start_IT(&htim2);    /* 使能定时器x及其更新中断 */
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2); /* 定时器中断公共处理函数 */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		if(LOCKUP_START)
		{
			lock_time++;
			if(lock_time == LOCKUP_TIME)
			{
				lock_time = 0;
				LOCKUP_START = 0;
			}
		}
	}
}