#ifndef __SCCB_H
#define __SCCB_H

#include "./SYSTEM/sys/sys.h"

#define SCCB_SDA_IN()  {GPIOG->CRH&=0XFF0FFFFF;GPIOG->CRH|=0X00800000;}
#define SCCB_SDA_OUT() {GPIOG->CRH&=0XFF0FFFFF;GPIOG->CRH|=0X00300000;}

#define SCCB_SCL(x)		do{	x?\
													HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET):\
													HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);\
												}while(0)

#define SCCB_SDA(x)		do{	x?\
													HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_SET):\
													HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13,GPIO_PIN_RESET);\
												}while(0)

#define SCCB_READ_SDA		HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_13)
#define SCCB_ID					0x42
			
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_ACK(void);
void SCCB_No_ACK(void);
uint8_t SCCB_WR_Byte(uint8_t dat);
uint8_t SCCB_RD_Byte(void);
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data);
uint8_t SCCB_RD_Reg(uint8_t reg);

#endif


