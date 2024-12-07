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
const uint8_t * kbd_passwordmenu[15]={"ָ��"," : ","����","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//������
extern uint8_t Card_OK;
extern int LOCKUP_START ;
uint8_t error_cnt = 0;

void SET_PW(void)	//�����µ�����
{
	uint16_t num;
	uint8_t i;	
	uint8_t temp[4];
	uint16_t convert[4];
	uint16_t change[4];
	lcd_fill(0,120,240,140,WHITE);
	text_show_string_middle(0,120,"����������,��Enter��֤",16,240,BLACK);
		
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
	//��numд��EEPROM
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
			text_show_string_middle(0,120,"����������,��Enter�޸�",16,240,BLACK);
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
				MAIN_MENU();//������ҳ��	
			}
		}
	}	
}

void VERIFY_PW(uint16_t num)//������֤
{
	uint8_t temp[4];
	uint8_t i;
	uint16_t convert[4];
//	uint8_t error_cnt = 0;
	NUM_DISPLAY(num);	
	for(i=0;i<4;i++)
	{
		temp[i] = at24cxx_read_one_byte(55+(i*1));//����EEPROM������
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
		LED1(0);		//DS1�̵���
		BEEP(1);		//BEEP��
		Servo_SetAngle(180);
		delay_ms(500);
		LED1(1);
		BEEP(0);		//BEEP����
		delay_ms(2000);
		Servo_SetAngle(20);
		lcd_fill(0,120,240,140,WHITE);
		text_show_string_middle(0,120,"������ȷ",16,240, BLACK);
		error_cnt = 0;
		delay_ms(1000);
	}
	else
	{
		if(error_cnt<=3)
		{
			error_cnt++;
			lcd_fill(0,120,240,140,WHITE);
			text_show_string_middle(0,120,"�������",16,240,BLACK);
		}
		else if(error_cnt >3)
		{
			lcd_fill(0,120,240,140,WHITE);
			text_show_string_middle(0,120,"�豸����20s",16,240,BLACK);
			LOCKUP_START = 1;
			while(LOCKUP_START)
			{
			}
			error_cnt = 0;
			delay_ms(200);
		}
	}
	delay_ms(1000);
	MAIN_MENU();//������ҳ��
	
}

void PW_load_keyboard(uint16_t x,uint16_t y,uint8_t **kbtbl)//�����������
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
		if(i==1)//�������2����:������Ҫ�м���ʾ
			text_show_string(x+(i%3)*80+2,y+7+30*(i/3),80,30,(uint8_t*)PW_kbd_tbl[i],16,0,BLACK);	
		else
			text_show_string_middle(x+(i%3)*80,y+7+30*(i/3),(uint8_t*)PW_kbd_tbl[i],16,80,BLACK);
	} 
}

uint8_t  PW_get_keynum(uint16_t x,uint16_t y)//��ȡ������
{
	uint16_t i,j;	
	static uint8_t key_x=0;//0,û���κΰ�������	
	uint8_t key=0;
	tp_dev.scan(0);
	if(tp_dev.sta&TP_PRES_DOWN)//������������
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

uint16_t PW_GET_NUM(void)//��ȡ������ֵ
{
	uint8_t  key_num=0;
	uint16_t num=0;
//	uint8_t cnt;
	while(1)
	{
		key_num=PW_get_keynum(0,170);	
		if(key_num)
		{
			if(key_num==1)return 0xFFFF;//����תָ�ƽ�������
			if(key_num==3)return 0xFF00;//	�����á���	
			if(key_num>3&&key_num<13&&num<999)//��1-9����(��������4λ��)
				num =num*10+key_num-3;		
			if(key_num==13)num =num/10;//��Del����			
			if(key_num==14&&num<999)num =num*10;//��0����
			if(key_num==15)return num;  //��Enter����			
		}	
		lcd_show_num(80+15,170+7,num,6,16,BLACK);
		RC522_Handle();
		delay_ms(200);
		if(Card_OK == 0)
		{
			Card_OK = 2;
			printf("����ʧ��");
			delay_ms(500);
			Servo_SetAngle(20);
			delay_ms(3000);
		}
		if(Card_OK == 1)
		{
			Card_OK = 2;
			printf("�����ɹ�");
			BEEP(1);
			Servo_SetAngle(180);
			delay_ms(1000);
			BEEP(0);
			Servo_SetAngle(20);
			delay_ms(3000);
		}	
	}
}

//����״̬����
//x,y:��������
//key:��ֵ(0~14)
//sta:״̬��0���ɿ���1�����£�
void PW_key_staset(uint16_t x,uint16_t y,uint8_t keyx,uint8_t sta)
{		  
	uint16_t i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//�������2����:������Ҫ���
		lcd_fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		lcd_fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)//���ǡ�����
		text_show_string_middle(x+j*80,y+7+30*i,(uint8_t*)PW_kbd_tbl[keyx],16,80,BLACK);	
}

//���������ʾ
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

//������ʾ��ͼ(��ռ�)
void LCD_PW(void)
{
	lcd_fill(60,80,180,110,WHITE);
	lcd_draw_rectangle(60,80,180,110,BLACK);
	lcd_draw_line(90,80,90,110,BLACK);
	lcd_draw_line(120,80,120,110,BLACK);	
	lcd_draw_line(150,80,150,110,BLACK);		
}

//��������ҳ��
void MAIN_MENU(void)
{
	/*����ָ��ʶ��ʵ�����*/
	lcd_clear(WHITE);
	delay_ms(50);
 	g_point_color=BLUE;
	text_show_string_middle(0,40,"ָ��������",16,240,BLACK);
	
		/*��ָ��ģ��ͨ��*/
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		delay_ms(400);
		lcd_fill(0,60,240,80,WHITE);
		text_show_string_middle(0,60,"δ��⵽ָ��ģ��!!!",16,240, BLACK);
		delay_ms(800);
		lcd_fill(0,60,240,80,WHITE);
		text_show_string_middle(0,60,"��������ָ��ģ��...",16,240, BLACK);		  
	}
	lcd_fill(0,60,240,80,WHITE);	
	text_show_string_middle(0,60,"ͨѶ�ɹ�",16,240,BLACK);
	
	LCD_PW();
	lcd_fill(0,120,240,140,WHITE);
	text_show_string_middle(0,120,"����������,��Enterȷ��",16,240,BLACK);
	PW_load_keyboard(0,170,(uint8_t**)kbd_passwordmenu);//�����������,�����������
}





































