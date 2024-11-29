#ifndef __HC05_H
#define __HC05_H

#include "./SYSTEM/sys/sys.h"

#define HC05_KEY(x)		 do{ x ? HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0, GPIO_PIN_RESET);}while(0)
#define HC05_LED			HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9);
											
uint8_t HC05_Init(void);
uint8_t HC05_Get_Role(void);
uint8_t HC05_Set_Cmd(uint8_t* atstr);	     

												
#endif














