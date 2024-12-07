/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       ����� ʵ��
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
 ****************************************************************************************************
 */
#include "stdio.h"
#include "main.h"
#include "esp8266.h"
#include "onenet.h"
#include "MqttKit.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LED/led.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/LSENS/lsens.h"
#include "./BSP/KEY/key.h"
#include "./BSP/BEEP/beep.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/TIMER/btim.h"

extern unsigned char a_esp8266_buf;

uint8_t alarmFlag;
uint8_t alarmFlag_Free = 10;
uint8_t tempreture;
uint8_t humidity;
short light;
char PUB_BUF[256];
//const char *devSubTopic[] = {"$sys/7BWCaXV7rg/weather/thing/property/set"};
const char devPubTopic[] = "$sys/r5H174uJB6/dev/thing/property/post";

int main(void)
{		
		uint16_t times;
		uint8_t t = 10;
		unsigned char *dataPtr = NULL;
	
    HAL_Init();                                 /* ��ʼ��HAL�� */
    sys_stm32_clock_init(RCC_PLL_MUL9);         /* ����ʱ��,72M */
    delay_init(72);                             /* ��ʼ����ʱ���� */
    led_init();  																/* ��ʼ��LED */
		usart1_init(115200);
		usart3_init(115200);
		dht11_init();
		lcd_init();
		lsens_init();
		beep_init();
		extix_init();
		btim_timx_int_init(5000 - 1, 7200 - 1); 
	
		lcd_show_string(30, 30, 200, 16, 16, "Welcome To The Smart Home", BLACK);
		lcd_show_string(30, 50, 200, 16, 16, "DHT11 TEST", RED);
		lcd_show_string(30, 70, 200, 16, 16, "LSENS TEST", RED);
		
		
		/*��ʼ��dht11*/
		while(dht11_init())
		{
			lcd_show_string(30, 90, 200, 16, 16, "DHT11 Error", RED);
			delay_ms(200);
			lcd_fill(30, 90, 239, 130 + 16, WHITE);
			delay_ms(200);
		}
		
		lcd_show_string(30, 90, 200, 16, 16, "DHT11 OK", RED);
		lcd_show_string(30, 110, 200, 16, 16, "LSENS_VAL:", BLUE);
		lcd_show_string(30, 130, 200, 16, 16, "Temp:  C", BLUE);
		lcd_show_string(30, 150, 200, 16, 16, "Humi:  %", BLUE);
		
		printf("This is  mqtt text \r\n");
		HAL_UART_Receive_IT(&huart3, (uint8_t *)&a_esp8266_buf, 1);
		ESP8266_Init();

		while(OneNet_DevLink())			//����OneNET
		HAL_Delay(500);
		printf("����onenet�ɹ�");
		OneNET_Subscribe();

    while(1)
    {		
			if(++times >= 100)
			{
				dht11_read_data(&tempreture, &humidity);
				light = lsens_get_val();
//				sprintf(PUB_BUF,"{\"Temp\":%d,\"Humi\":%d,\"Light\":%d}",tempreture,humidity,light);
//				printf("OneNet_Publish\r\n");
//				OneNET_Publish(devPubTopic,PUB_BUF);
//				printf("OneNet_SendData\r\n");
				OneNet_SendData();									//��������
				times = 0;
				ESP8266_Clear();
			}
			
			dataPtr = ESP8266_GetIPD(3);
			if(dataPtr != NULL)
				OneNet_RevPro(dataPtr);
			
			light = lsens_get_val();
			lcd_show_xnum(30 + 10 * 8, 110, light, 3, 16, 0, BLUE); /* ��ʾADC��ֵ */
			
			if(t % 10 == 0)
			{
				dht11_read_data(&tempreture, &humidity);
				lcd_show_num(30 + 40, 130, tempreture, 2, 16, BLUE); /* ��ʾ�¶� */
				lcd_show_num(30 + 40, 150, humidity, 2, 16, BLUE); /* ��ʾʪ�� */
			}
			delay_ms(10);
			t++;
			
			if(t == 20)
			{
				t = 0;
				LED0_TOGGLE();
			}
			if(alarmFlag_Free == 10)
			{
				if(tempreture < 100 && humidity < 100 && light < 70)
				{
					alarmFlag = 0;
				}
				else
				{					
					alarmFlag = 1;
					BEEP(1);
					delay_ms(100);
					BEEP(0);
				}
			}	
			if(alarmFlag_Free < 10)alarmFlag_Free++ ;
			delay_ms(20);
    }	
}	

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

