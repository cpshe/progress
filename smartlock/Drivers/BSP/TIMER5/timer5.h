#ifndef __TIMER5_H
#define __TIMER5_H


#include "./SYSTEM/sys/sys.h"

void TIM5_Int_Init(uint16_t arr ,uint16_t psc);
void TIM5_IRQHandler(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif

