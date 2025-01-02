#include "./SYSTEM/usart/usart.h"
#include "./BSP/OV7670/ov7670.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/EXTI/exti.h"

uint8_t ov_sta;

// 外部中断引脚初始化
void EXTI8_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    EXTI_HandleTypeDef EXTI_Handle = {0};

    // 启用 GPIOA 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 配置 PA8 引脚为外部中断模式
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;    // 设置为上升沿触发中断
    GPIO_InitStruct.Pull = GPIO_NOPULL;             // 不启用上下拉电阻
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);         // 初始化 PA8 引脚

    // 配置 NVIC 中断
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);  // 设置中断优先级
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);           // 使能外部中断通道 EXTI9_5_IRQn

    // 配置 EXTI 中断线
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);       // 配置外部中断处理函数
}

// 外部中断处理函数 (EXTI9_5_IRQn 中断处理)
void EXTI9_5_IRQHandler(void)
{
    // 调用 HAL 库提供的中断处理函数
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8); // 处理 PA8 引脚的中断
}

// 外部中断清除标志位
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_8)  // 如果是 PA8 引脚触发的中断
    {
        OV7670_WRST(0);    // 复位写指针
        OV7670_WRST(1);    // 恢复写指针
        OV7670_WREN(1);    // 允许写入 FIFO
        ov_sta++;           // 帧中断计数加1
    }
}
