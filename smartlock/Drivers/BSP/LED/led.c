/**
 ****************************************************************************************************
 * @file        led.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       LED 驱动代码
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
 * V1.0 20200417
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include	"./BSP/SERVO/servo.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/beep/beep.h"
#include "./TEXT/text.h"
#include "./BSP/LCD/lcd.h" 
LED_INFO led_info;
/**
 * @brief       初始化LED相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    LED0_GPIO_CLK_ENABLE();                                 /* LED0时钟使能 */
    LED1_GPIO_CLK_ENABLE();                                 /* LED1时钟使能 */

    gpio_init_struct.Pin = LED0_GPIO_PIN;                   /* LED0引脚 */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
    HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct);       /* 初始化LED0引脚 */

    gpio_init_struct.Pin = LED1_GPIO_PIN;                   /* LED1引脚 */
    HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct);       /* 初始化LED1引脚 */
    

    LED0(1);                                                /* 关闭 LED0 */
    LED1(1);                                                /* 关闭 LED1 */
}

void Led_Set(_Bool status)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5,status == LED_ON?GPIO_PIN_RESET : GPIO_PIN_SET);
	if(status == LED_ON)
	{
		BEEP(1);
		Servo_SetAngle(180);
		lcd_fill(0,80,240,140,WHITE);
		lcd_fill(0,100,240,140,WHITE);
		text_show_string_middle(0,80,"开锁成功",16,240, BLACK);
		delay_ms(500);
		BEEP(0);
		delay_ms(2000);
		Servo_SetAngle(20);
	}
	else
	{
		lcd_fill(0,80,240,140,WHITE);
		text_show_string_middle(0,80,"关锁成功",16,240, BLACK);
		Servo_SetAngle(20);
	}
	led_info.Led_Status = status;
}
