#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/24CXX/24cxx.h"
#include "./BSP/HC05/hc05.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h" 
#include "./BSP/beep/beep.h"
#include	"./BSP/SERVO/servo.h"
#include "./TEXT/text.h"
#include "string.h"
#include "math.h"

uint8_t HC05_Set_Cmd(uint8_t* atstr);

uint8_t HC05_Init(void)
{
	uint8_t retry = 10,t;
	uint8_t temp = 1;
	
	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_0;
	GPIO_InitStructure.Mode= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	usart1_init(9600);

	while(retry--)
	{
		HC05_KEY(1); 	/*����ATģʽ*/
		delay_ms(20);
		u1_printf("AT\r\n");
		delay_ms(20);
		HC05_KEY(0);
		for(t=0;t<10;t++)
		{
			if(USART1_RX_STA & 0x8000)break;
			delay_ms(5);
		}
		if(USART1_RX_STA & 0x8000)
		{
			temp = USART1_RX_STA & 0x7FFF;
			USART1_RX_STA = 0;
			if(temp == 4&&USART1_RX_BUF[0] == 'O'&&USART1_RX_BUF[1] == 'K')
			{
				temp = 0;
				break;
			}
		}
	}
	if(retry == 0 )temp=1;
	return temp;
}

//��ȡATK-HC05ģ��Ľ�ɫ
//����ֵ:0,�ӻ�;1,����;0XFF,��ȡʧ��.							  
uint8_t	HC05_Get_Role(void)
{
	uint8_t retry = 0X0F;
	uint8_t temp,t;
	while(retry--)
	{
		HC05_KEY(1);
		delay_ms(10);
		u1_printf("AT+ROLE?\r\n");//��ѯ��ɫ
		for(t=0;t<20;t++)
		{
			delay_ms(10);
			if(USART1_RX_STA&0x8000)break;
		}
		HC05_KEY(0);
		if(USART1_RX_STA&0x8000)
		{
			temp = USART1_RX_STA&0x7FFF;
			USART1_RX_STA = 0;
			if(temp==13&&USART1_RX_BUF[0]=='+')
			{
				temp = USART1_RX_BUF[6]-'0';
				break;
			}
		}
	}
	if(retry == 0)temp = 0XFF;
	return temp;
}

//ATK-HC05��������
//�˺�����������ATK-HC05,�����ڽ�����OKӦ���ATָ��
//atstr:ATָ�.����:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"���ַ���
//����ֵ:0,���óɹ�;����,����ʧ��.							  
uint8_t HC05_Set_Cmd(uint8_t * atstr)
{
	uint8_t retry = 0X0F;
	uint8_t temp,t;
	while(retry--)
	{
		HC05_KEY(1);
		delay_ms(10);
		u1_printf("%s\r\n",atstr);
		HC05_KEY(0);
		for(t=0;t<20;t++)
		{
			if(USART1_RX_STA&0X8000)break;
			delay_ms(5);
		}
		if(USART1_RX_STA&0X8000)
		{
			temp = USART1_RX_STA&0X7FFF;
			USART1_RX_STA = 0;
			if(temp==4&USART1_RX_BUF[0]=='O')
			{
				temp = 0;
				break;
			}
		}
	}
	if(retry==0)temp = 0XFF;
	return temp;
}
