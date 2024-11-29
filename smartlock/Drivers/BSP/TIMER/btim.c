/**
 ****************************************************************************************************
 * @file        btim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       基本定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211216
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "./BSP/TIMER/btim.h"
#include "./SYSTEM/usart/usart.h"

TIM_HandleTypeDef htim6;  /* 定时器句柄 */

extern volatile uint16_t USART2_RX_STA;

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
void tim6_int_init(uint16_t arr, uint16_t psc)
{
    htim6.Instance = TIM6_INT;                      /* 通用定时器X */
    htim6.Init.Prescaler = psc;                          /* 设置预分频系数 */
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 递增计数模式 */
    htim6.Init.Period = arr;                             /* 自动装载值 */
    HAL_TIM_Base_Init(&htim6);
	
    HAL_TIM_Base_Start_IT(&htim6);    /* 使能定时器x及其更新中断 */

}
/**
 * @brief       定时器底层驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6_INT)
    {
        TIM6_INT_CLK_ENABLE();                     /* 使能TIM时钟 */
        HAL_NVIC_SetPriority(TIM6_INT_IRQn, 0, 2); /* 抢占1，子优先级3，组2 */
        HAL_NVIC_EnableIRQ(TIM6_INT_IRQn);         /* 开启ITM3中断 */
    }
}

/**
 * @brief       定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void TIM6_INT_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&htim6, TIM_FLAG_UPDATE)!=RESET)
	{
		if(__HAL_TIM_GET_IT_SOURCE(&htim6,TIM_IT_UPDATE) != RESET)
		{
			 USART2_RX_STA |= 1 << 15;  // 标记接收完成
			__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
			HAL_TIM_Base_Stop(&htim6); 
		}
	}
}
