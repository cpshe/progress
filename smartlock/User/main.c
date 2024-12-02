#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/KEYBORD/keybord.h"
#include "./BSP/LCD/lcd.h" 
#include "./BSP/LED/led.h"
#include "./BSP/SPI2/spi2.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/TOUCH/touch.h"
#include "./BSP/PASSWORD/password.h"
#include "./BSP/AS608/as608.h"
#include "./BSP/24CXX/24cxx.h"
#include "./BSP/USART2/usart2.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./FATFS/source/ff.h"			
#include "./TEXT/fonts.h"
#include "./TEXT/text.h"
#include "./MALLOC/malloc.h"
#include "./BSP/EXTI/exti.h"
#include "./BSP/KEY/key.h"
#include "./BSP/beep/beep.h"
#include "./BSP/TIMER/btim.h"
#include "./BSP/TIMER7/timer7.h"
#include "./BSP/PWMTIMER/pwmtimer.h"
#include "./BSP/RC522/RC522.h"
#include "./BSP/STMFLASH/stmflash.h"
#include "./BSP/SPI/spi.h"
#include	"./BSP/SERVO/servo.h"
#include "./BSP/USART3/usart3.h"
#include "./BSP/HC05/hc05.h"
#include "esp8266.h"
#include "onenet.h"
#include "MqttKit.h"
#include <string.h>

#define usart2_baund  57600

uint8_t ID1,ID2,ID3,ID4,ID5;
extern uint8_t Card_OK;

extern unsigned char Card_ID1[8];
extern unsigned char Card_ID2[8];
extern unsigned char Card_ID3[8];
extern unsigned char Card_ID4[8];
extern unsigned char Card_ID5[8];


unsigned char Card_ID_TEST[8] = {1,2,3,4,5,6,7,8};
unsigned char Card_ID_READ[8] = {0};

extern unsigned char a_esp8266_buf;

uint8_t processsum = 0;
uint8_t hc_pwd[4];
uint8_t initial_pwd[4] = {1,2,3,4};
int main(void)
 {
	uint8_t i;
	uint8_t reclen = 0;
	uint8_t key_num;
	uint8_t key;
	uint16_t num;
	uint16_t cnt;
	uint8_t processsum = 0;
	uint8_t times;
	HAL_Init();   
	sys_stm32_clock_init(RCC_PLL_MUL9); 
	delay_init(72);                      
	//HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
	usart1_init(9600);
	usart2_init(usart2_baund);
	usart3_init(115200);
	beep_init();
	lcd_init();
	PS_StaGPIO_Init();
	led_init();
	key_init();
	RC522_Init();
	Servo_Init();
	extix_init();
	Servo_SetAngle(20);
	norflash_init();
	my_mem_init(SRAMIN);
	tp_dev.init();
	f_mount(fs[1],"1",1);//����flash
//	for(i=0;i<4;i++)//��ʼ����Ϊ1234
//	{
//		at24cxx_write_one_byte(55+(i*1),initial_pwd[i]);
//	}
	
	while(fonts_init())
	{
		lcd_show_string(60, 50, 240, 16, 16, "Font Error!",BLACK);
		delay_ms(200);
		lcd_fill(60,50,240,66,WHITE);//�����ʾ
		delay_ms(200);	
	}
	LED0(0);
	
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&a_esp8266_buf, 1);
	ESP8266_Init();
	
	while(HC05_Init())
	{
		lcd_show_string(60, 50, 240, 16, 16, "HC05 Error!",BLACK);
		delay_ms(200);
		lcd_fill(60,50,240,66,WHITE);//�����ʾ
		delay_ms(200);	
	}
	delay_ms(100);
		
	while(1)
	{
		text_show_string_middle(0,40,"��������",16,240,BLACK);
		text_show_string_middle(0,60,"��ѡ��ģʽ",16,240,BLACK);
		text_show_string(0,80,200,16,"����+0xFE��KEYUP:��ͨģʽ",16,0,BLACK);
		text_show_string(0,100,200,16,"����+0xFF��KEY1:����ģʽ",16,0,BLACK);
		text_show_string(0,120,200,16,"����+0xEE��KEY0:WIFEģʽ",16,0,BLACK);
		
		key = key_scan(0);
		if(USART1_RX_BUF[4]==0xFE||key == WKUP_PRES )
		{
			while(1)
			{
				//��ʱ״̬�л�		
				switch(processsum)
				{
					case 0:
						MAIN_MENU();//�л���ҳ��
						processsum=2;
						break;
					case 1:
						PS_MAINMENU();				
							while(1)
							{
								key_num=AS608_get_keynum(0,170);
								if(key_num)
								{
									if(key_num==1)Del_FR();		//ɾָ��
									if(key_num==3)Add_FR();		//¼ָ��									
								}
								if(PS_Sta)//���PS_Sta״̬���������ָ������תָ�ƽ�������ȶ�ָ��
								{
									press_FR();//ˢָ��
								}	
								delay_ms(20);
								cnt++;
								if(cnt%500==0)break;//�ӳ�10s����ת������
							}
							processsum=0;
							break;
					case 2:			
						num=PW_GET_NUM();
						if(num==0xff00)
						{
							SET_PW();		//����������
						}
						if((num!=0xffff)&&(num!=0xff00))
						{
							VERIFY_PW(num);	//������֤		
						}
						if(num==0xffff)//��תָ�ƽ���
						{
							delay_ms(500);
							processsum=1;
							break;
						}
				}
			}
		}	
		if(USART1_RX_BUF[4] == 0xFF || key == KEY1_PRES)
		{
			lcd_clear(WHITE);
			text_show_string_middle(0,40,"����ģʽ",16,240,BLACK);
			text_show_string_middle(0,60,"�����룺",16,240,BLACK);
			text_show_string(0,80,200,16,"����+0x0E:����",16,0,BLACK);
			text_show_string(0,100,200,16,"����+0x0F:�޸�����",16,0,BLACK);
			while(1)
			{
				if(USART1_RX_STA & 0x8000)
				{
					reclen = USART1_RX_STA & 0x7FFF;
					USART1_RX_BUF[reclen] = 0;
					if(USART1_RX_BUF[5]==0x0E&&USART1_RX_BUF[6]==0x0A)
					{
						uint8_t i;
						for(i=0;i<4;i++)
						{
							hc_pwd[i] = at24cxx_read_one_byte(55+(i*1));
						}
						lcd_clear(WHITE);
						text_show_string_middle(0,40,"����ģʽ",16,240,BLACK);
						text_show_string_middle(0,60,"����������",16,240,BLACK);
						delay_ms(1000);
						if(USART1_RX_BUF[0] == hc_pwd[0]&&
							 USART1_RX_BUF[1]==hc_pwd[1]&&
							 USART1_RX_BUF[2]==hc_pwd[2]&&
							 USART1_RX_BUF[3]==hc_pwd[3])
						{
							LED1(0);		//DS1�̵���
							BEEP(1);		//BEEP��
							Servo_SetAngle(180);
							lcd_fill(0,120,240,140,WHITE);
							text_show_string_middle(0,120,"������ȷ",16,240, BLACK);
							delay_ms(500);
							LED1(1);
							BEEP(0);		//BEEP����
							delay_ms(2000);
							Servo_SetAngle(20);
						}
						else
						{
							lcd_fill(0,120,240,140,WHITE);
							text_show_string_middle(0,120,"�������",16,240,BLACK);
						}
					}
					else if(USART1_RX_BUF[5]==0x0F&&USART1_RX_BUF[6]==0x0A)
					{
						lcd_clear(WHITE);
						text_show_string_middle(0,40,"����ģʽ",16,240,BLACK);
						text_show_string_middle(0,80,"����������",16,240,BLACK);
						uint8_t i;
						for(i=0;i<4;i++)
						{
							at24cxx_write_one_byte(55+(i*1),USART1_RX_BUF[i]);
						}
						text_show_string_middle(0,100,"�޸ĳɹ�",16,240,BLACK);
					}
					USART1_RX_STA = 0;
				}
			}
		}
		if(USART1_RX_BUF[4] == 0xEE || key == KEY0_PRES)
		{
			unsigned char *dataPtr = NULL;
			lcd_clear(WHITE);
			text_show_string_middle(0,40,"WIFEģʽ",16,240,BLACK);
			text_show_string_middle(0,60,"�����룺",16,240,BLACK);
			text_show_string(0,80,200,16,"TRUE:����",16,0,BLACK);
			text_show_string(0,100,200,16,"FALSE:����",16,0,BLACK);
			
			while(OneNet_DevLink())
			HAL_Delay(500);
			OneNET_Subscribe();
			while(1)
			{
				if(++times>=10)
				{
					OneNet_SendData();
					times = 0;
					ESP8266_Clear();
				}
				
				delay_ms(200);
				
				dataPtr = ESP8266_GetIPD(3);
				if(dataPtr != NULL)
					OneNet_RevPro(dataPtr);	
			}
		}
	}
}
