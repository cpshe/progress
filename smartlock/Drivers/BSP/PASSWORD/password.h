#ifndef __PASSWORD_H
#define __PASSWORD_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/LCD/lcd.h"

extern _Bool generate_pwd_status;

void SET_PW(void);
void VERIFY_PW(uint16_t num);
void PW_load_keyboard(uint16_t x,uint16_t y,uint8_t **kbtbl);
uint8_t  PW_get_keynum(uint16_t x,uint16_t y);
uint16_t PW_GET_NUM(void);
void PW_key_staset(uint16_t x,uint16_t y,uint8_t keyx,uint8_t sta);

uint8_t* NUM_CONVERT(uint16_t num);
void NUM_DISPLAY(uint16_t num);
void LCD_PW(void);
void MAIN_MENU(void);
void generate_password(char* password, uint8_t length);

#endif
