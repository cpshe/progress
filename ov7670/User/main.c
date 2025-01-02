/**
 ****************************************************************************************************
 * @file        main.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-21
 * @brief       TFTLCD(MCU��) ʵ��
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

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/TIMER5/timer5.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/OV7670/ov7670.h"

const uint8_t*LMODE_TBL[6]={"Auto","Sunny","Cloudy","Office","Home","Light"};							//5�ֹ���ģʽ	    
const uint8_t*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
extern uint8_t ov_sta;	//��exit.c�� �涨��
extern uint8_t ov_frame;	//��timer.c���涨��		 
//����LCD��ʾ
void camera_refresh(void)
{
	uint32_t j;
 	uint16_t color;	 
	uint32_t hd,x=0,y=0,flag=0;
	uint8_t r,g,b;
	if(ov_sta)//��֡�жϸ��£�
	{
		lcd_scan_dir(U2D_L2R);		//���ϵ���,������  
		if(lcddev.id==0X1963)lcd_set_window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//����ʾ�������õ���Ļ����
		else if(lcddev.id==0X5510||lcddev.id==0X5310)lcd_set_window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//����ʾ�������õ���Ļ����
		lcd_write_ram_prepare();     //��ʼд��GRAM	
		OV7670_RRST(0);				//��ʼ��λ��ָ�� 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST(1);				//��λ��ָ����� 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			if(0)
			{
				LCD->LCD_RAM=color;
			}
			else
			{
//				hd = (((color&0xF800)>> 8)*76+((color&0x7E0)>>3)*150+((color&0x001F)<<3)*30)>>8;//ת��Ϊ�Ҷ�;
//				r = (color&0xF800) >> 8;
//				g = (color&0x7E0) >> 3;
//				b = (color&0x001F) << 3;
				if((r - g > 27) && (r - b > 27)) //��ɫ>150
				{
					LCD->LCD_RAM = 0xF800;
				}
				if((g - r > 37) && (g - b > 37))
				{
					LCD->LCD_RAM = 0x07E0;
					x += j/320;
					y += (j+1)%320 == 0 ? 0 : (j+1)%320;
					flag++;
				}
				if((b - r > 15) && (b - g > 15))
				{
					LCD->LCD_RAM = 0x001F;
					x += j/320;
					y += (j+1)%320 == 0 ? 0 : (j+1)%320;
					flag++;
				}
				else
				{
					LCD->LCD_RAM=color;
				}
				//LCD->LCD_RAM = ((hd >> 3) << 11) | ((hd >> 3) << 5) | hd >> 3;//ת��Ϊ�Ҷ�
			}
		}
 		ov_sta=0;					//����֡�жϱ��
		ov_frame++; 
		lcd_scan_dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
	}
}

int main(void)
{
	uint8_t key;
	uint8_t lightmode=0,saturation=2,contrast=2;
	uint8_t effect=0;
	uint8_t i = 0;
	uint8_t msgbuf[15];
	uint8_t tm=0;
	
	HAL_Init();
	sys_stm32_clock_init(RCC_PLL_MUL9);    /* ����ʱ��, 72Mhz */
	delay_init(72);
	usart_init(115200);
	led_init();
	lcd_init();
	lcd_show_string(30,50,200,16,16,"ELITE STM32F103 ^_^",BLACK);	
	lcd_show_string(30,70,200,16,16,"OV7670 TEST",BLACK);	
	lcd_show_string(30,90,200,16,16,"ATOM@ALIENTEK",BLACK);
	lcd_show_string(30,110,200,16,16,"2015/1/18",BLACK); 
	lcd_show_string(30,130,200,16,16,"KEY0:Light Mode",BLACK);
	lcd_show_string(30,150,200,16,16,"KEY1:Saturation",BLACK);
	lcd_show_string(30,170,200,16,16,"KEY_UP:Contrast",BLACK);
	lcd_show_string(30,190,200,16,16,"TPAD:Effects",BLACK);	 
  lcd_show_string(30,210,200,16,16,"OV7670 Init...",BLACK);

	while(OV7670_Init())
	{
		lcd_show_string(30,210,200,16,16,"OV7670 Error!!",BLACK);
		printf("ov7670 error\r\n");
		delay_ms(200);
		lcd_fill(30,210,239,246,WHITE);
		delay_ms(200);
	}
	lcd_show_string(30,210,200,16,16,"OV7670 Init OK",BLACK);
	delay_ms(1500);
	OV7670_Light_Mode(lightmode);
	OV7670_Color_Saturation(saturation);
	OV7670_Contrast(contrast);
 	OV7670_Special_Effects(effect);	 
	
	TIM5_Int_Init(10000,7199);			//10Khz����Ƶ��,1�����ж�
	EXTI8_Init();						//ʹ�ܶ�ʱ������
	OV7670_Window_Set(12,176,240,320);	//���ô���	  
  OV7670_CS(0);
	lcd_clear(BLACK);		
	while(1)
	{
		camera_refresh();
		if(tm)
		{
			lcd_show_string((lcddev.width-240)/2+30,(lcddev.height-320)/2+60,200,16,16,(char*)msgbuf,BLACK);
			tm--;
		}
		i++;
		if(i==15)//DS0��˸.
		{
			i=0;
			LED0_TOGGLE();
		}
	}
}	   

