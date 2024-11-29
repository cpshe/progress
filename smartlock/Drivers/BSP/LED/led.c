/**
 ****************************************************************************************************
 * @file        led.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       LED ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200417
 * ��һ�η���
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
 * @brief       ��ʼ��LED���IO��, ��ʹ��ʱ��
 * @param       ��
 * @retval      ��
 */
void led_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    LED0_GPIO_CLK_ENABLE();                                 /* LED0ʱ��ʹ�� */
    LED1_GPIO_CLK_ENABLE();                                 /* LED1ʱ��ʹ�� */

    gpio_init_struct.Pin = LED0_GPIO_PIN;                   /* LED0���� */
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* ������� */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* ���� */
    HAL_GPIO_Init(LED0_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED0���� */

    gpio_init_struct.Pin = LED1_GPIO_PIN;                   /* LED1���� */
    HAL_GPIO_Init(LED1_GPIO_PORT, &gpio_init_struct);       /* ��ʼ��LED1���� */
    

    LED0(1);                                                /* �ر� LED0 */
    LED1(1);                                                /* �ر� LED1 */
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
		text_show_string_middle(0,80,"�����ɹ�",16,240, BLACK);
		delay_ms(500);
		BEEP(0);
		delay_ms(2000);
		Servo_SetAngle(20);
	}
	else
	{
		lcd_fill(0,80,240,140,WHITE);
		text_show_string_middle(0,80,"�����ɹ�",16,240, BLACK);
		Servo_SetAngle(20);
	}
	led_info.Led_Status = status;
}
