#include "./SYSTEM/sys/sys.h"

#ifndef __PWMTIMER_H
#define __PWMTIMER_H

#define TIM4_PWM_CHY_GPIO_PORT 					GPIOB
#define TIM4_PWM_CHY_GPIO_PIN  					GPIO_PIN_9
#define TIM4_PWM_CHY_GPIO_CLK_ENABLE()	do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)

#define TIM4_PWM												TIM4
#define TIM4_PWM_CHY 										TIM_CHANNEL_4
#define TIM4_PWM_CHY_CCRX 							TIM4->CCR2
#define TIM4_PWM_CHY_CLK_ENABLE() 			do{ __HAL_RCC_TIM4_CLK_ENABLE(); }while(0)

void PWM_Init(void);
void PWM_SetCompare1(uint16_t compare);

#endif
