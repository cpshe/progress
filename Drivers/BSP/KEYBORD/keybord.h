#ifndef __KEYBORD_H
#define __KEYBORD_H

#include "./SYSTEM/sys/sys.h"

#define GPIOF_CLK_ENABLE(); do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)

void Keybord_Init(void);

uint16_t Keybord_GetNum(void);

#endif
