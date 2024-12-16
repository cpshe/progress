#include "./SYSTEM/sys/sys.h"
#include "./BSP/PWMTIMER/pwmtimer.h"
#include "./SYSTEM/delay/delay.h"

TIM_HandleTypeDef htim4;
GPIO_InitTypeDef 	gpio_init_struct;

void PWM_Init(void)
{
	GPIO_InitTypeDef 	gpio_init_struct;

	TIM4_PWM_CHY_GPIO_CLK_ENABLE();
	TIM4_PWM_CHY_CLK_ENABLE();
	
	gpio_init_struct.Pin = TIM4_PWM_CHY_GPIO_PIN;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Pull = GPIO_NOPULL;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TIM4_PWM_CHY_GPIO_PORT, &gpio_init_struct);
	
	htim4.Instance = TIM4_PWM;
	htim4.Init.Prescaler = 72 - 1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 20000 - 1;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&htim4);
	
	TIM_OC_InitTypeDef sConfigOC;
	sConfigOC.OCMode = TIM_OCMODE_PWM1;  // PWM 模式 1
	//sConfigOC.Pulse = TIM4_PWM_CHY_CCRX;  // CCR，占空比初始值
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;  // 输出极性高
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;  // 快速模式禁用
	sConfigOC.Pulse = 500;
	HAL_TIM_PWM_Init(&htim4);
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM4_PWM_CHY);
 
	HAL_TIM_PWM_Start(&htim4, TIM4_PWM_CHY);
}

void PWM_SetCompare1(uint16_t compare)
{
	__HAL_TIM_SET_COMPARE(&htim4, TIM4_PWM_CHY, compare);
}
