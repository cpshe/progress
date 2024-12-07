#include "./BSP/PASSWORD/password.h"
#include "./TEXT/text.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/LED/led.h"
#include "./BSP/24CXX/24cxx.h"
#include "./BSP/AS608/as608.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/beep/beep.h"
#include "./BSP/LCD/lcd.h"
#include	"./BSP/SERVO/servo.h"
#include "./BSP/RC522/RC522.h"
#include "./BSP/TIM2/time2.h"
#include <stdio.h>

uint8_t ** PW_kbd_tbl;
const uint8_t * kbd_passwordmenu[15]={"指纹"," : ","设置","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//按键表
extern uint8_t Card_OK;
extern int LOCKUP_START ;
uint8_t error_cnt = 0;

void SET_PW(void)	//设置新的密码
{
	uint16_t num;
	uint8_t i;	
	uint8_t temp[4];
	uint16_t convert[4];
	uint16_t change[4];
	lcd_fill(0,120,240,140,WHITE);
	text_show_string_middle(0,120,"请输入密码,按Enter验证",16,240,BLACK);
		
	for(i=0;i<4;i++)
	{
		temp[i] = at24cxx_read_one_byte(55+(i*1));
	}
	num=PW_GET_NUM();
	if(num == 0xff00)
	{
		MAIN_MENU();
	}
  if((num != 0xffff)&&(num != 0xff00))	
	{
		 NUM_DISPLAY(num);
	//将num写入EEPROM
		convert[0]=num/1000;
		convert[1]=(num/100)%10;
		convert[2]=(num/10)%10;
		convert[3]=num%10;
		if(convert[0]==temp[0]&&
			convert[1]==temp[1]&&
			convert[2]==temp[2]&&
			convert[3]==temp[3])
		{
			LCD_PW();
			lcd_fill(0,120,240,140,WHITE);
			text_show_string_middle(0,120,"请输入密码,按Enter修改",16,240,BLACK);
			num=PW_GET_NUM();
			if((num != 0xffff)&&(num != 0xff00))	
			{
				NUM_DISPLAY(num);
				change[0]=num/1000;
				change[1]=(num/100)%10;
				change[2]=(num/10)%10;
				change[3]=num%10;

				for(i=0;i<4;i++)
				{
					at24cxx_write_one_byte(55+(i*1),change[i]);
				}	
				delay_ms(1000);
				MAIN_MENU();//加载主页面	
			}
		}
	}	
}

void VERIFY_PW(uint16_t num)//密码验证
{
	uint8_t temp[4];
	uint8_t i;
	uint16_t convert[4];
//	uint8_t error_cnt = 0;
	NUM_DISPLAY(num);	
	for(i=0;i<4;i++)
	{
		temp[i] = at24cxx_read_one_byte(55+(i*1));//读出EEPROM的密码
	}
	convert[0]=num/1000;
	convert[1]=(num/100)%10;
	convert[2]=(num/10)%10;
	convert[3]=num%10;	

	if(convert[0]==temp[0]&&
		convert[1]==temp[1]&&
		convert[2]==temp[2]&&
		convert[3]==temp[3])
	{
		LED1(0);		//DS1绿灯亮
		BEEP(1);		//BEEP响
		Servo_SetAngle(180);
		delay_ms(500);
		LED1(1);
		BEEP(0);		//BEEP不响
		delay_ms(2000);
		Servo_SetAngle(20);
		lcd_fill(0,120,240,140,WHITE);
		text_show_string_middle(0,120,"密码正确",16,240, BLACK);
		error_cnt = 0;
		delay_ms(1000);
	}
	else
	{
		if(error_cnt<=3)
		{
			error_cnt++;
			lcd_fill(0,120,240,140,WHITE);
			text_show_string_middle(0,120,"密码错误",16,240,BLACK);
		}
		else if(error_cnt >3)
		{
			lcd_fill(0,120,240,140,WHITE);
			text_show_string_middle(0,120,"设备上锁20s",16,240,BLACK);
			LOCKUP_START = 1;
			while(LOCKUP_START)
			{
			}
			error_cnt = 0;
			delay_ms(200);
		}
	}
	delay_ms(1000);
	MAIN_MENU();//加载主页面
	
}

void PW_load_keyboard(uint16_t x,uint16_t y,uint8_t **kbtbl)//加载虚拟键盘
{
	uint16_t i;
	g_point_color=RED;
	PW_kbd_tbl=kbtbl;
	lcd_fill(x,y,x+240,y+150,WHITE);
	lcd_draw_rectangle(x,y,x+240,y+150,BLACK);						   
	lcd_draw_rectangle(x+80,y,x+160,y+150,BLACK);	 
	lcd_draw_rectangle(x,y+30,x+240,y+60,BLACK);
	lcd_draw_rectangle(x,y+90,x+240,y+120,BLACK);
	g_point_color=BLUE;
	for(i=0;i<15;i++)
	{
		if(i==1)//按键表第2个‘:’不需要中间显示
			text_show_string(x+(i%3)*80+2,y+7+30*(i/3),80,30,(uint8_t*)PW_kbd_tbl[i],16,0,BLACK);	
		else
			text_show_string_middle(x+(i%3)*80,y+7+30*(i/3),(uint8_t*)PW_kbd_tbl[i],16,80,BLACK);
	} 
}

uint8_t  PW_get_keynum(uint16_t x,uint16_t y)//获取键盘数
{
	uint16_t i,j;	
	static uint8_t key_x=0;//0,没有任何按键按下	
	uint8_t key=0;
	tp_dev.scan(0);
	if(tp_dev.sta&TP_PRES_DOWN)//触摸屏被按下
	{	
		for(i=0;i<5;i++)
		{
			for(j=0;j<3;j++)
			{
			 	if(tp_dev.x[0]<(x+j*80+80)&&tp_dev.x[0]>(x+j*80)&&tp_dev.y[0]<(y+i*30+30)&&tp_dev.y[0]>(y+i*30))
				{	
					key=i*3+j+1;	 
					break;	 		   
				}
			}
			if(key)
			{	   
				if(key_x==key)key=0;
				else 
				{
					PW_key_staset(x,y,key_x-1,0);
					key_x=key;
					PW_key_staset(x,y,key_x-1,1);
				}
				break;
			}
	  }  
	}else if(key_x) 
	{
		PW_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	
	return key;
}

uint16_t PW_GET_NUM(void)//获取键盘数值
{
	uint8_t  key_num=0;
	uint16_t num=0;
//	uint8_t cnt;
	while(1)
	{
		key_num=PW_get_keynum(0,170);	
		if(key_num)
		{
			if(key_num==1)return 0xFFFF;//‘跳转指纹解锁’键
			if(key_num==3)return 0xFF00;//	‘设置’键	
			if(key_num>3&&key_num<13&&num<999)//‘1-9’键(限制输入4位数)
				num =num*10+key_num-3;		
			if(key_num==13)num =num/10;//‘Del’键			
			if(key_num==14&&num<999)num =num*10;//‘0’键
			if(key_num==15)return num;  //‘Enter’键			
		}	
		lcd_show_num(80+15,170+7,num,6,16,BLACK);
		RC522_Handle();
		delay_ms(200);
		if(Card_OK == 0)
		{
			Card_OK = 2;
			printf("解锁失败");
			delay_ms(500);
			Servo_SetAngle(20);
			delay_ms(3000);
		}
		if(Card_OK == 1)
		{
			Card_OK = 2;
			printf("解锁成功");
			BEEP(1);
			Servo_SetAngle(180);
			delay_ms(1000);
			BEEP(0);
			Servo_SetAngle(20);
			delay_ms(3000);
		}	
	}
}

//按键状态设置
//x,y:键盘坐标
//key:键值(0~14)
//sta:状态，0，松开；1，按下；
void PW_key_staset(uint16_t x,uint16_t y,uint8_t keyx,uint8_t sta)
{		  
	uint16_t i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//按键表第2个‘:’不需要清除
		lcd_fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		lcd_fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)//不是‘：’
		text_show_string_middle(x+j*80,y+7+30*i,(uint8_t*)PW_kbd_tbl[keyx],16,80,BLACK);	
}

//在密码框显示
void NUM_DISPLAY(uint16_t num)
{
	uint8_t i;
	uint8_t str[2];
	uint8_t convert[4];
	convert[0]=num/1000;
	convert[1]=(num/100)%10;
	convert[2]=(num/10)%10;
	convert[3]=num%10;
	LCD_PW();
	for(i=0;i<4;i++)
	{
		sprintf((char*)str,"%d",convert[i]);
		text_show_string_middle(60+30*i,90,str,16,30,BLACK);
	}
	
	
}

//密码显示框图(清空键)
void LCD_PW(void)
{
	lcd_fill(60,80,180,110,WHITE);
	lcd_draw_rectangle(60,80,180,110,BLACK);
	lcd_draw_line(90,80,90,110,BLACK);
	lcd_draw_line(120,80,120,110,BLACK);	
	lcd_draw_line(150,80,150,110,BLACK);		
}

//密码锁主页面
void MAIN_MENU(void)
{
	/*加载指纹识别实验界面*/
	lcd_clear(WHITE);
	delay_ms(50);
 	g_point_color=BLUE;
	text_show_string_middle(0,40,"指纹密码锁",16,240,BLACK);
	
		/*与指纹模块通信*/
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		delay_ms(400);
		lcd_fill(0,60,240,80,WHITE);
		text_show_string_middle(0,60,"未检测到指纹模块!!!",16,240, BLACK);
		delay_ms(800);
		lcd_fill(0,60,240,80,WHITE);
		text_show_string_middle(0,60,"尝试连接指纹模块...",16,240, BLACK);		  
	}
	lcd_fill(0,60,240,80,WHITE);	
	text_show_string_middle(0,60,"通讯成功",16,240,BLACK);
	
	LCD_PW();
	lcd_fill(0,120,240,140,WHITE);
	text_show_string_middle(0,120,"请输入密码,按Enter确认",16,240,BLACK);
	PW_load_keyboard(0,170,(uint8_t**)kbd_passwordmenu);//加载虚拟键盘,密码解锁界面
}





































