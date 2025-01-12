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
#include "./BSP/TIM2/time2.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/OV7670/sccb.h"
#include "./BSP/OV7670/ov7670.h"
#include "./PICTURE/piclib.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "esp8266.h"
#include "onenet.h"
#include "MqttKit.h"
#include <string.h>
#include "math.h"

#define usart2_baund  57600

uint8_t ID1,ID2,ID3,ID4,ID5;
extern uint8_t Card_OK;
extern uint8_t ov_sta;

extern unsigned char ESP8266_Buf[512];                         //定义一个数组作为esp8266的数据缓冲区
extern unsigned char Card_ID1[8];
extern unsigned char Card_ID2[8];
extern unsigned char Card_ID3[8];
extern unsigned char Card_ID4[8];
extern unsigned char Card_ID5[8];
extern unsigned char a_esp8266_buf;
extern uint8_t error_cnt;
extern uint8_t LOCKUP_START;
uint8_t processsum = 0;
uint8_t hc_pwd[4];
uint8_t initial_pwd[4] = {1,2,3,4};
uint8_t wifi_pwd_leng = 0;
uint32_t real_time ;
uint8_t creation_times[4];
uint8_t sd_ok = 1;
char *photoname; //文件名

void BLE_TEXT(void)
{
	lcd_clear(WHITE);
	text_show_string_middle(0,40,"解锁模式",16,240,BLACK);
	text_show_string_middle(0,60,"请输入密码",16,240,BLACK);
}

int main(void)
 {
	uint8_t res;
	uint8_t i;
	uint8_t reclen = 0;
	uint8_t key_num;
	uint8_t key;
	uint8_t modify_key;
	uint8_t initial_key;
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
	time2_init(5000-1, 7200-1);
	EXTI8_Init();
	beep_init();
	lcd_init();
	PS_StaGPIO_Init();
	led_init();
	key_init();
	RC522_Init();
	Servo_Init();
	Servo_SetAngle(20);
	norflash_init();
	rtc_init();
	my_mem_init(SRAMIN);
	tp_dev.init();
	
	exfuns_init();                              /* 为fatfs相关变量申请内存 */
	f_mount(fs[0],"0:",1);                    /* 挂载SD卡 */
	f_mount(fs[1],"1:",1);//挂载flash
	
	piclib_init();
	
	//取出临时密码生成的时间
	at24cxx_read(120,creation_times,4);
	real_time = (creation_times[0] << 24) | (creation_times[1] << 16) | (creation_times[2] << 8) | creation_times[3];		
	
	while(fonts_init())
	{
		lcd_show_string(60, 50, 240, 16, 16, "Font Error!",BLACK);
		delay_ms(200);
		lcd_fill(60,50,240,66,WHITE);//清除显示
		delay_ms(200);	
	}

	res = f_mkdir("0:/PHOTO");                  /* 创建PHOTO文件夹 */

	if(res != FR_EXIST && res != FR_OK)        /* 发生了错误 */
	{
			res = f_mkdir("0:/PHOTO");              /* 创建PHOTO文件夹 */
			text_show_string(30, 110, 240, 16, "SD卡错误!", 16, 0, RED);
			delay_ms(200);
			text_show_string(30, 110, 240, 16, "拍照功能将不可用!", 16, 0, RED);
			delay_ms(200);
			sd_ok = 0;
	}
	
	while(HC05_Init())
	{
		lcd_show_string(60, 50, 240, 16, 16, "HC05 Error!",BLACK);
		delay_ms(200);
		lcd_fill(60,50,240,66,WHITE);//清除显示
		delay_ms(200);	
	}
	delay_ms(100);
		
	modify_key = key_scan(0); 
	if(modify_key == KEY1_PRES)
	{
		WIFI_MODIFY();
	}

	HAL_UART_Receive_IT(&huart3, (uint8_t *)&a_esp8266_buf, 1);
	ESP8266_Init();

	while (OV7670_Init() != 0)
	{
		lcd_show_string(30, 130, 200, 16, 16, "OV7670 Error!!", RED);
		delay_ms(200);
		lcd_fill(30, 130, 239, 246, WHITE);
		delay_ms(200);
	}

	OV7670_Window_Set(12,176,240,320);
		
	OV7670_Light_Mode(0);                   /* 自动 灯光模式 */
	OV7670_Color_Saturation(2);             /* 默认 色彩饱和度 */
	OV7670_Brightness(2);                   /* 默认 亮度 */
	OV7670_Contrast(2);                     /* 默认 对比度 */
	OV7670_Special_Effects(0);              /* 默认 特效 */

	OV7670_CS(0);                           /* 使能OV7670 FIFO数据输出 */

	photoname = mymalloc(SRAMIN, 30);           /* 为带路径的文件名分配30个字节的内存 */

	/*初始密码*/
	initial_key = key_scan(1);
	if(initial_key == KEY0_PRES)
	{
		for(i=0;i<4;i++)//初始密码为1234
		{
			at24cxx_write_one_byte(55+(i*1),initial_pwd[i]);
		}
	}
	
	
	while(1)
	{				
		lcd_clear(WHITE);
		text_show_string_middle(0,40,"智能门锁",16,240,BLACK);
		text_show_string_middle(0,60,"请选择模式",16,240,BLACK);
		text_show_string(0,80,200,16,"密码+0xFE或KEYUP:普通模式",16,0,BLACK);
		text_show_string(0,100,200,16,"密码+0xFF或KEY1:蓝牙模式",16,0,BLACK);
		text_show_string(0,120,200,16,"密码+0xEE或KEY0:WIFE模式",16,0,BLACK);
		while(1)
		{
			key = key_scan(0);
			if(USART1_RX_BUF[4]==0xFE||key == WKUP_PRES )
			{
				while(1)
				{
					//延时状态切换		
					switch(processsum)
					{
						case 0:
							MAIN_MENU();//切回主页面
							processsum=2;
							break;
						case 1:
							PS_MAINMENU();				
								while(1)
								{
									key_num=AS608_get_keynum(0,170);
									if(key_num)
									{
										if(key_num==1)Del_FR();		//删指纹
										if(key_num==3)Add_FR();		//录指纹									
									}
									if(PS_Sta)//检测PS_Sta状态，如果有手指按下跳转指纹解锁界面比对指纹
									{
										press_FR();//刷指纹
									}	
									delay_ms(20);
									cnt++;
									if(cnt%500==0)break;//延迟10s后跳转主界面
								}
								processsum=0;
								break;
						case 2:			
							num=PW_GET_NUM();
							if(num==0xff00)
							{
								SET_PW();		//设置新密码
							}
							if((num!=0xffff)&&(num!=0xff00))
							{
								VERIFY_PW(num);	//密码验证		
							}
							if(num==0xffff)//跳转指纹解锁
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
				text_show_string_middle(0,40,"蓝牙模式",16,240,BLACK);
				text_show_string_middle(0,60,"请输入：",16,240,BLACK);
				text_show_string(0,80,200,16,"密码+FF0x0E:解锁",16,0,BLACK);
				text_show_string(0,100,200,16,"密码+FF0x0F:修改密码",16,0,BLACK);
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
							BLE_TEXT();
							delay_ms(1000);
							if(USART1_RX_BUF[0] == hc_pwd[0]&&
								 USART1_RX_BUF[1]==hc_pwd[1]&&
								 USART1_RX_BUF[2]==hc_pwd[2]&&
								 USART1_RX_BUF[3]==hc_pwd[3])
							{
								LED1(0);		//DS1绿灯亮
								BEEP(1);		//BEEP响
								Servo_SetAngle(180);
								lcd_fill(0,120,240,140,WHITE);
								text_show_string_middle(0,120,"密码正确",16,240, BLACK);
								delay_ms(500);
								LED1(1);
								BEEP(0);		//BEEP不响
								delay_ms(2000);
								Servo_SetAngle(20);
								BLE_TEXT();
								error_cnt = 0;
							}
							else
							{
								if(error_cnt <= 2)
								{
									error_cnt++;
									lcd_fill(0,120,240,140,WHITE);
									text_show_string_middle(0,120,"密码错误",16,240,BLACK);
									delay_ms(1000);
									BLE_TEXT();
								}
								else if(error_cnt > 2)
								{
									lcd_fill(0,120,240,140,WHITE);
									text_show_string_middle(0,120,"设备上锁20s",16,240,BLACK);
									printf("设备上锁20s\r\n");
									LOCKUP_START = 1;
									while(LOCKUP_START)
									{
									}
									error_cnt = 0;
									delay_ms(1000);
									BLE_TEXT();
								}
							}
						}
						else if(USART1_RX_BUF[5]==0x0F&&USART1_RX_BUF[6]==0x0A)
						{
							lcd_clear(WHITE);
							text_show_string_middle(0,40,"改密模式",16,240,BLACK);
							text_show_string_middle(0,80,"请输入密码",16,240,BLACK);
							uint8_t i;

							for(i=0;i<4;i++)
							{
								at24cxx_write_one_byte(55+(i*1),USART1_RX_BUF[i]);
							}
							delay_ms(1000);
							text_show_string_middle(0,100,"修改成功",16,240,BLACK);
						}
						USART1_RX_STA = 0;
					}
				}
			}
			if(USART1_RX_BUF[4] == 0xEE || key == KEY0_PRES)
			{
				unsigned char *dataPtr = NULL;
				lcd_clear(WHITE);
				WIFI_TEXT();
				while(OneNet_DevLink())
				HAL_Delay(500);
				OneNET_Subscribe();
				while(1)
				{
					if(++times>=200)
					{
						OneNet_SendData();
						times = 0;
						ESP8266_Clear();
					}
					delay_ms(10);
					
					dataPtr = ESP8266_GetIPD(3);
					if(dataPtr != NULL)
					{
						OneNet_RevPro(dataPtr);	
					}
				}
			}
			USART1_RX_STA = 0;
		}	
	}
}
