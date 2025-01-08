#include "./BSP/TIMER5/timer5.h"
#include "./SYSTEM/usart/usart.h"

TIM_HandleTypeDef htim5;
uint8_t ov_frame;

void TIM5_Int_Init(uint16_t arr ,uint16_t psc)
{
	__HAL_RCC_TIM5_CLK_ENABLE();
	
	htim5.Instance = TIM5;
	htim5.Init.Period = arr;
	htim5.Init.Prescaler = psc;
	htim5.Init.ClockDivision = 0;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	
	HAL_TIM_Base_Init(&htim5);
	
	HAL_TIM_Base_Start_IT(&htim5);
	
	HAL_NVIC_SetPriority(TIM5_IRQn, 1,3);
	HAL_NVIC_EnableIRQ(TIM5_IRQn);
}


void TIM5_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&htim5,TIM_FLAG_UPDATE)!=RESET)
	{
		__HAL_TIM_CLEAR_IT(&htim5,TIM_IT_UPDATE);
		printf("frame:%dfps\r\n",ov_frame);
		ov_frame = 0;
	}
//	HAL_TIM_IRQHandler(&htim5);
}

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	if(htim->Instance == TIM5)
//	{
//		printf("frame:%dfps\r\n",ov_frame);
//		ov_frame = 0;
//	}
//}






