	#ifndef __OV7670_H
	#define __OV7670_H

	#include "./SYSTEM/sys/sys.h"
	#include "./BSP/OV7670/sccb.h"

	#define OV7670_VSYNC 	HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)		//֡ͬ���ź�
	#define OV7670_WRST(x) 		do{x?\
																HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET):\
																HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET);\
																}while(0)		//дָ�븴λ

	#define OV7670_WREN(x)		do{x?\
																HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET):\
																HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);\
																}while(0)		//дFIFOʹ��

	#define OV7670_RCK_H			GPIOB->BSRR = 1<<4	//���ö�����ʱ�Ӹߵ�ƽ
	#define OV7670_RCK_L 	 		GPIOB->BRR = 1<<4		//���ö�����ʱ�ӵ͵�ƽ
	#define OV7670_RRST(x) 		do{x?\
																HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,GPIO_PIN_SET):\
																HAL_GPIO_WritePin(GPIOG,GPIO_PIN_14,GPIO_PIN_RESET);\
																}while(0)		//��ָ�븴λ

	#define OV7670_CS(x)			do{x?\
																HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,GPIO_PIN_SET):\
																HAL_GPIO_WritePin(GPIOG,GPIO_PIN_15,GPIO_PIN_RESET);\
																}while(0)		//Ƭѡ�źţ�OE��

	#define OV7670_DATA		GPIOC->IDR&0x00FF

	uint8_t OV7670_Init(void);
	void OV7670_Light_Mode(uint8_t mode);
	void OV7670_Color_Saturation(uint8_t sat);
	void OV7670_Brightness(uint8_t bright);
	void OV7670_Brightness(uint8_t bright);
	void OV7670_Contrast(uint8_t contrast);
	void OV7670_Special_Effects(uint8_t eft);
	void OV7670_Window_Set(uint16_t sx,uint16_t sy, uint16_t width, uint16_t height);

	#endif

														