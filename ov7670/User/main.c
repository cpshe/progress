/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-21
 * @brief       TFTLCD(MCU屏) 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
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

const uint8_t*LMODE_TBL[6]={"Auto","Sunny","Cloudy","Office","Home","Light"};							//5种光照模式	    
const uint8_t*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
extern uint8_t ov_sta;	//在exit.c里 面定义
extern uint8_t ov_frame;	//在timer.c里面定义		 
//更新LCD显示
void camera_refresh(void)
{
	uint32_t j;
 	uint16_t color;	 
	uint32_t hd,x=0,y=0,flag=0;
	uint8_t r,g,b;
	if(ov_sta)//有帧中断更新？
	{
		lcd_scan_dir(U2D_L2R);		//从上到下,从左到右  
		if(lcddev.id==0X1963)lcd_set_window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//将显示区域设置到屏幕中央
		else if(lcddev.id==0X5510||lcddev.id==0X5310)lcd_set_window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//将显示区域设置到屏幕中央
		lcd_write_ram_prepare();     //开始写入GRAM	
		OV7670_RRST(0);				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST(1);				//复位读指针结束 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			if(0)
			{
				LCD->LCD_RAM=color;
			}
			else
			{
//				hd = (((color&0xF800)>> 8)*76+((color&0x7E0)>>3)*150+((color&0x001F)<<3)*30)>>8;//转换为灰度;
//				r = (color&0xF800) >> 8;
//				g = (color&0x7E0) >> 3;
//				b = (color&0x001F) << 3;
				if((r - g > 27) && (r - b > 27)) //红色>150
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
				//LCD->LCD_RAM = ((hd >> 3) << 11) | ((hd >> 3) << 5) | hd >> 3;//转换为灰度
			}
		}
 		ov_sta=0;					//清零帧中断标记
		ov_frame++; 
		lcd_scan_dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
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
	sys_stm32_clock_init(RCC_PLL_MUL9);    /* 设置时钟, 72Mhz */
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
	
	TIM5_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断
	EXTI8_Init();						//使能定时器捕获
	OV7670_Window_Set(12,176,240,320);	//设置窗口	  
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
		if(i==15)//DS0闪烁.
		{
			i=0;
			LED0_TOGGLE();
		}
	}
}	   

