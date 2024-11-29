#ifndef __BTIM_H
#define __BTIM_H

#include "./SYSTEM/sys/sys.h"
extern TIM_HandleTypeDef htim7;  /* 定时器句柄 */

/******************************************************************************************/
/* 基本定时器 定义 */

/* TIMX 中断定义 
 * 默认是针对TIM6/TIM7
 * 注意: 通过修改这4个宏定义,可以支持TIM1~TIM8任意一个定时器.
 */
 
#define TIM7_INT                       TIM7
#define TIM7_INT_IRQn                  TIM7_IRQn
#define TIM7_INT_IRQHandler            TIM7_IRQHandler
#define TIM7_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM7_CLK_ENABLE(); }while(0)   /* TIM7 时钟使能 */


/******************************************************************************************/

void tim7_int_init(uint16_t arr, uint16_t psc);    /* 基本定时器 定时中断初始化函数 */
void Error_Handler(void);

#endif
