#include "esp8266.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/USART3/usart3.h"
#include "./BSP/24CXX/24cxx.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LCD/lcd.h" 
#include "./TEXT/text.h"
#include "./BSP/KEY/key.h"

//#define     ESP8266_WIFI_INFO		"AT+CWJAP=\"508\",\"www.791358426\"\r\n"          //�������Լ���wifi�ȵ㣺WiFi��������
#define     ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n" //������OneNet��MQTT


unsigned char ESP8266_Buf[512];                         //����һ��������Ϊesp8266�����ݻ�����
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;     //������������ֵ���˴κ���һ��
unsigned char a_esp8266_buf;

unsigned char WIFI_BUF[512];
char wifi_str[512];

uint8_t modify_wifi_str[512];

/**
  * @brief esp8266��ʼ��
  * @param ��
  * @retval ��
  */
void ESP8266_Init(void)
{
	char* result = 0;
	uint8_t correct_len = 0;
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_Initure.Pin = GPIO_PIN_4;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	HAL_Delay(250);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_Delay(500);
	
  ESP8266_Clear();
	
	at24cxx_read(75,modify_wifi_str,40);

	printf("1. ����AT����\r\n");            //AT������AT����
	while(ESP8266_SendCmd("AT\r\n", "OK"))
	{
		lcd_show_string(30,40,240,16,16,"AT\r\n",BLACK);
		HAL_Delay(500);
		lcd_clear(WHITE);
	}
		
	printf("2. ����WiFiģʽ��CWMODE��\r\n");        //��ѯ/���� Wi-Fi ģʽ������WiFiģʽΪStationģʽ
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
	{
		lcd_show_string(30,40,240,16,16,"AT+CWMODE=1\r\n",BLACK);
		HAL_Delay(500);
		lcd_clear(WHITE);
	}
	
	printf("3. AT+CWDHCP\r\n");     //����/���� DHCP
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
	{
		lcd_show_string(30,40,240,16,16,"AT+CWDHCP=1,1\r\n",BLACK);
		HAL_Delay(500);
		lcd_clear(WHITE);
	}
	
	printf("4. ����WiFi�ȵ㣨CWJAP��\r\n");        
	while(ESP8266_SendCmd((char*)modify_wifi_str, "GOT IP"))
	{
		lcd_show_string(30,40,240,16,16,"ESP8266_WIFI_INFO",BLACK);
		HAL_Delay(500);
		lcd_clear(WHITE);
	}
	
	printf("5. ����TCP���ӣ�CIPSTART��\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
	{
		lcd_show_string(30,40,240,16,16,"ESP8266_ONENET_INFO",BLACK);
		HAL_Delay(500);
		lcd_clear(WHITE);
	}
	
	printf("6. ESP8266 Init OK\r\n");
	lcd_show_string(30,40,240,16,16,"ESP8266 Init OK",BLACK);
	lcd_clear(WHITE);
}    


/**
  * @brief  ��ջ���
  * @param  ��
  * @retval ��
  */
void ESP8266_Clear(void)
{
    memset(ESP8266_Buf, 0, sizeof(ESP8266_Buf));    //�������е�Ԫ��ȫ����ʼ��Ϊ0��
}    
 
/**
  * @brief  �ȴ��������
  * @param  ��
  * @retval OK����ʾ������ɣ�OUTTIME����ʾ���ճ�ʱ���
  *         ����ѭ�����ã��������Ƿ����
  */
_Bool ESP8266_WaitRecive(void)
{
    if(esp8266_cnt == 0) 							//�����ǰ���ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return OUTTIME;
		
	if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		esp8266_cnt = 0;							//��0���ռ���
			
		return OK;								    //���ؽ�����ɱ�־
	}
	else                                            //�������ͬ���򽫴˴θ�ֵ����һ�Σ������ؽ���δ��ɱ�־
    {        
        esp8266_cntPre = esp8266_cnt;				
        
        return OUTTIME;								
    }
}

/**
  * @brief ��������
  * @param cmd����ʾ���res����Ҫ���ķ���ָ��
  * @retval 0����ʾ�ɹ���1����ʾʧ��
  */
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	HAL_UART_Transmit(&huart3, (unsigned char *)cmd, strlen((const char *)cmd),0xffff);

	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == OK)							//����յ�����
		{
			printf("%s",ESP8266_Buf);
			if(strstr((const char *)ESP8266_Buf, res) != NULL)		//����������ؼ���,��ջ���
			{
				ESP8266_Clear();							
				
				return 0;
			}
		}
		HAL_Delay(10);
	}
	return 1;
}

/**
  * @brief ���ݷ���
  * @param data�������͵����ݣ�len�������͵����ݳ���
  * @retval ��
  */
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������,sprintf()�������ڽ���ʽ��������д���ַ���
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		HAL_UART_Transmit(&huart3, data, len,0xffff);		//�����豸������������
	}
}

/**
  * @brief ��ȡƽ̨���ص�����
  * @param �ȴ���ʱ��
  * @retval ƽ̨���ص����ݣ���ͬ�����豸���صĸ�ʽ��ͬ����Ҫ���е��ԣ��磺ESP8266�ķ��ظ�ʽΪ��"+IPD,x:yyy",x��ʾ���ݳ��ȣ�yyy��ʾ��������
  */
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP8266_WaitRecive() == OK)								//����������
		{
			ptrIPD = strstr((char *)ESP8266_Buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
				//printf("\"IPD\" not founf\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		HAL_Delay(5);													//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;		//��ʱ��δ�ҵ������ؿ�ָ��
}

void WIFI_TEXT(void)
{
		text_show_string_middle(0,40,"WIFIģʽ",16,240,BLACK);
		text_show_string_middle(0,60,"�����룺",16,240,BLACK);
		text_show_string_middle(0,80,"LED:TRUE:����",16,240,BLACK);
		text_show_string_middle(0,100,"LED:FALSE:����",16,240,BLACK);
}

void WIFI_MODIFY(void)
{
	uint8_t wifi_reclen = 0;
	uint8_t len =0;
	text_show_string_middle(0,40,"�޸�WIFI",16,240,BLACK);
	while(1)
	{
		if(USART1_RX_STA & 0x8000)
		{
			wifi_reclen = USART1_RX_STA & 0x7FFF;
			USART1_RX_BUF[wifi_reclen] = 0;
			strcpy((char*)WIFI_BUF,(char*)USART1_RX_BUF);
			sprintf(wifi_str, "AT+CWJAP=%s\r\n",WIFI_BUF);
			len = strlen(wifi_str);
			wifi_str[len] = '\0';
			at24cxx_write(75,(uint8_t*)wifi_str,len);
			USART1_RX_STA = 0;
		}
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		if(esp8266_cnt >= 255)  //����жϣ�����һ���ֽ�
		{
			esp8266_cnt = 0;
			memset(ESP8266_Buf,0x00,sizeof(ESP8266_Buf));
			HAL_UART_Transmit(&huart3, (uint8_t *)"�������", 10,0xFFFF); 	
					
		}
		else
		{
			ESP8266_Buf[esp8266_cnt++] = a_esp8266_buf;   //��������ת��
		
		}	
		HAL_UART_Receive_IT(&huart3, (uint8_t *)&a_esp8266_buf, 1);   //�ٿ��������ж�
	}	
}
