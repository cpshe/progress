#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/KEYBORD/keybord.h"

void Keybord_Init(void)
{
	/*��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIOF_CLK_ENABLE();
	GPIO_InitStructure.Pin= GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStructure.Mode= GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	/*��*/
	GPIO_InitTypeDef GPIO_InitStructure_1; 
	GPIO_InitStructure_1.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStructure_1.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure_1.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOF,&GPIO_InitStructure_1);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_PIN_SET);
}

uint16_t Keybord_GetNum(void)
{
	/*��һ�а���*/
	uint8_t KeyBord_Num = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
		{
			delay_ms(10);
			KeyBord_Num = 1;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
		{
			delay_ms(10);
			KeyBord_Num = 2;
		}
	}if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
		{
			delay_ms(10);
			KeyBord_Num = 3;
		}
	}if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
		{
			delay_ms(10);
			KeyBord_Num = 11;
		}
	}
	/*�ڶ��а���*/
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
		{
			delay_ms(10);
			KeyBord_Num = 4;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
		{
			delay_ms(10);
			KeyBord_Num = 5;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
		{
			delay_ms(10);
			KeyBord_Num = 6;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
		{
			delay_ms(10);
			KeyBord_Num = 12;
		}
	}
	/*�����а���*/
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
		{
			delay_ms(10);
			KeyBord_Num = 7;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
		{
			delay_ms(10);
			KeyBord_Num = 8;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
		{
			delay_ms(10);
			KeyBord_Num = 9;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
		{
			delay_ms(10);
			KeyBord_Num = 13;
		}
	}
	/*�����а���*/
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET);
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_4))
		{
			delay_ms(10);
			KeyBord_Num = 14;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_5))
		{
			delay_ms(10);
			KeyBord_Num = 10;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))
		{
			delay_ms(10);
			KeyBord_Num = 15;
		}
	}
	if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
	{
		//delay_ms(10);
		while(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7))
		{
			delay_ms(10);
			KeyBord_Num = 16;
		}
	}
	return KeyBord_Num;
}
