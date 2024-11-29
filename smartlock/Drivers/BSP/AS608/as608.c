#include "./BSP/USART2/usart2.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/LED/led.h"
#include "./BSP/TOUCH/touch.h"
#include "./TEXT/text.h"
#include "./MALLOC/malloc.h"
#include "./BSP/beep/beep.h"
#include "./BSP/AS608/as608.h"
#include	"./BSP/SERVO/servo.h"
#include "./SYSTEM/usart/usart.h"
#include <string.h>
#include <stdio.h>

uint32_t AS608Addr = 0XFFFFFFFF;

#define usart2_baund  57600//����2�����ʣ�����ָ��ģ�鲨���ʸ���

SysPara AS608Para;//ָ��ģ��AS608����

uint16_t ValidN;
uint8_t ** PS_kbd_tbl;
const uint8_t* kbd_menu[15]={"ɾָ��"," : ","¼ָ��","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//������[]/
const uint8_t* kbd_delFR[15]={"����"," : ","���ָ��","1","2","3","4","5","6","7","8","9","DEL","0","Enter",};//������

void Add_FR(void);

void PS_StaGPIO_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;
  __HAL_RCC_GPIOA_CLK_ENABLE();//ʹ��GPIOAʱ��
  //��ʼ����״̬����GPIOA
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO	
}

//���ڷ���һ���ֽ�
static void MYUSART_SendData(uint8_t data)
{
	while((USART2->SR&0X40)==0); //SR״̬�Ĵ���������λΪһ�Ƿ���������
	USART2->DR = data;
}

//���Ͱ�ͷ
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}

//���͵�ַ
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}

//���Ͱ���ʶ,
static void SendFlag(uint8_t flag)
{
	MYUSART_SendData(flag);
}

//���Ͱ�����
static void SendLength(uint16_t length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}

//����ָ����
static void Sendcmd(uint8_t cmd)
{
	MYUSART_SendData(cmd);
}

//����У���
static void SendCheck(uint16_t check)
{
	MYUSART_SendData(check>>8);
	MYUSART_SendData(check);
}

//�ж��жϽ��յ�������û��Ӧ���
//waittimeΪ�ȴ��жϽ������ݵ�ʱ�䣨��λ1ms��
//����ֵ�����ݰ��׵�ַ
static uint8_t *JudgeStr(uint16_t waittime)
{
	char *data;
	uint8_t str[8];
	str[0]=0xef;str[1]=0x01;str[2]=AS608Addr>>24;
	str[3]=AS608Addr>>16;str[4]=AS608Addr>>8;
	str[5]=AS608Addr;str[6]=0x07;str[7]='\0';
	USART2_RX_STA=0;
	while(--waittime)
	{
		delay_ms(1);
		if(USART2_RX_STA&0X8000)//���յ�һ������
		{
			USART2_RX_STA=0;
			data=strstr((const char*)USART2_RX_BUF,(const char*)str);
			if(data)
				return (uint8_t*)data;	
		}
	}
	return 0;
}

//¼��ͼ�� PS_GetImage
//����:̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer�� 
//ģ�鷵��ȷ����
uint8_t PS_GetImage(void)
{
  uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x01);
  temp =  0x01+0x03+0x01;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}

//�������� PS_GenChar
//����:��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2			 
//����:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//ģ�鷵��ȷ����
uint8_t PS_GenChar(uint8_t BufferID)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}

//��ȷ�ȶ���öָ������ PS_Match
//����:��ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ� 
//ģ�鷵��ȷ����
uint8_t PS_Match(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}

//����ָ�� PS_Search
//����:��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣			
//����:  BufferID @ref CharBuffer1	CharBuffer2
//˵��:  ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x04);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x04+BufferID
	+(StartPage>>8)+(uint8_t)StartPage
	+(PageNum>>8)+(uint8_t)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
	{
		ensure = data[9];
		p->pageID   =(data[10]<<8)+data[11];
		p->mathscore=(data[12]<<8)+data[13];	
	}
	else
		ensure = 0xff;
	return ensure;	
}

//�ϲ�����������ģ�壩PS_RegModel
//����:��CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2	
//˵��:  ģ�鷵��ȷ����
uint8_t PS_RegModel(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;		
}

//����ģ�� PS_StoreChar
//����:�� CharBuffer1 �� CharBuffer2 �е�ģ���ļ��浽 PageID ��flash���ݿ�λ�á�			
//����:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID��ָ�ƿ�λ�úţ�
//˵��:  ģ�鷵��ȷ����
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x06);
	Sendcmd(0x06);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	temp = 0x01+0x06+0x06+BufferID
	+(PageID>>8)+(uint8_t)PageID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;	
}

//ɾ��ģ�� PS_DeletChar
//����:  ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
//����:  PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������
//˵��:  ģ�鷵��ȷ����
uint8_t PS_DeletChar(uint16_t PageID,uint16_t N)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x0C);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	MYUSART_SendData(N>>8);
	MYUSART_SendData(N);
	temp = 0x01+0x07+0x0C
	+(PageID>>8)+(uint8_t)PageID
	+(N>>8)+(uint8_t)N;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}

//���ָ�ƿ� PS_Empty
//����:  ɾ��flash���ݿ�������ָ��ģ��
//����:  ��
//˵��:  ģ�鷵��ȷ����
uint8_t PS_Empty(void)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}

//дϵͳ�Ĵ��� PS_WriteReg
//����:  дģ��Ĵ���
//����:  �Ĵ������RegNum:4\5\6
//˵��:  ģ�鷵��ȷ����
uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x05);
	Sendcmd(0x0E);
	MYUSART_SendData(RegNum);
	MYUSART_SendData(DATA);
	temp = RegNum+DATA+0x01+0x05+0x0E;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	if(ensure==0)
		printf("\r\n���ò����ɹ���");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}

//��ϵͳ�������� PS_ReadSysPara
//����:  ��ȡģ��Ļ��������������ʣ�����С��)
//����:  ��
//˵��:  ģ�鷵��ȷ���� + ����������16bytes��
uint8_t PS_ReadSysPara(SysPara *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01+0x03+0x0F;
	SendCheck(temp);
	data=JudgeStr(1000);
	if(data)
	{
		ensure = data[9];
		p->PS_max = (data[14]<<8)+data[15];
		p->PS_level = data[17];
		p->PS_addr=(data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
		p->PS_size = data[23];
		p->PS_N = data[25];
	}		
	else
		ensure=0xff;
	if(ensure==0x00)
	{
		printf("\r\nģ�����ָ������=%d",p->PS_max);
		printf("\r\n�Աȵȼ�=%d",p->PS_level);
		printf("\r\n��ַ=%x",p->PS_addr);
		printf("\r\n������=%d",p->PS_N*9600);
	}
	else 
			printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}

//����ģ���ַ PS_SetAddr
//����:  ����ģ���ַ
//����:  PS_addr
//˵��:  ģ�鷵��ȷ����
uint8_t PS_SetAddr(uint32_t PS_addr)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x15);
	MYUSART_SendData(PS_addr>>24);
	MYUSART_SendData(PS_addr>>16);
	MYUSART_SendData(PS_addr>>8);
	MYUSART_SendData(PS_addr);
	temp = 0x01+0x07+0x15
	+(uint8_t)(PS_addr>>24)+(uint8_t)(PS_addr>>16)
	+(uint8_t)(PS_addr>>8) +(uint8_t)PS_addr;				
	SendCheck(temp);
	AS608Addr=PS_addr;//������ָ�������ַ
  data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;	
		AS608Addr = PS_addr;
	if(ensure==0x00)
		printf("\r\n���õ�ַ�ɹ���");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}

//���ܣ� ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
//	�ü��±��߼��ϱ��ֳ� 16 ��ҳ��
//����:  NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
//˵��:  ģ�鷵��ȷ����
uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *Byte32)
{
	uint16_t temp;
  uint8_t  ensure,i;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(36);
	Sendcmd(0x18);
	MYUSART_SendData(NotePageNum);
	for(i=0;i<32;i++)
	 {
		 MYUSART_SendData(Byte32[i]);
		 temp += Byte32[i];
	 }
  temp =0x01+36+0x18+NotePageNum+temp;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}

//������PS_ReadNotepad
//���ܣ�  ��ȡFLASH�û�����128bytes����
//����:  NotePageNum(0~15)
//˵��:  ģ�鷵��ȷ����+�û���Ϣ
uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *Byte32)
{
	uint16_t temp;
  uint8_t  ensure,i;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x19);
	MYUSART_SendData(NotePageNum);
	temp = 0x01+0x04+0x19+NotePageNum;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
	{
		ensure=data[9];
		for(i=0;i<32;i++)
		{
			Byte32[i]=data[10+i];
		}
	}
	else
		ensure=0xff;
	return ensure;
}

//��������PS_HighSpeedSearch
//���ܣ��� CharBuffer1��CharBuffer2�е������ļ��������������򲿷�ָ�ƿ⡣
//		  �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ����
//		  �ܺõ�ָ�ƣ���ܿ�������������
//����:  BufferID�� StartPage(��ʼҳ)��PageNum��ҳ����
//˵��:  ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x1b);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x1b+BufferID
	+(StartPage>>8)+(uint8_t)StartPage
	+(PageNum>>8)+(uint8_t)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
 	if(data)
	{
		ensure=data[9];
		p->pageID 	=(data[10]<<8) +data[11];
		p->mathscore=(data[12]<<8) +data[13];
	}
	else
		ensure=0xff;
	return ensure;
}

//��AS608���� PS_HandShake
//����: PS_Addr��ַָ��
//˵��: ģ�鷵�µ�ַ����ȷ��ַ��	
uint8_t PS_HandShake(uint32_t *PS_Addr)
{
	SendHead();
	SendAddr();
	MYUSART_SendData(0X01);
	MYUSART_SendData(0X00);
	MYUSART_SendData(0X00);	
	delay_ms(200);
	if(USART2_RX_STA&0X8000)//���յ�����
	{		
		if(//�ж��ǲ���ģ�鷵�ص�Ӧ���				
					USART2_RX_BUF[0]==0XEF
				&&USART2_RX_BUF[1]==0X01
				&&USART2_RX_BUF[6]==0X07
			)
			{
				*PS_Addr=(USART2_RX_BUF[2]<<24) + (USART2_RX_BUF[3]<<16)
								+(USART2_RX_BUF[4]<<8) + (USART2_RX_BUF[5]);
				USART2_RX_STA=0;
				return 0;
			}
		USART2_RX_STA=0;					
	}
	return 1;		
}

//����Чģ����� PS_ValidTempleteNum
//���ܣ�����Чģ�����
//����: ��
//˵��: ģ�鷵��ȷ����+��Чģ�����ValidN
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{
	uint16_t temp;
  uint8_t  ensure;
	uint8_t  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
	{
		ensure=data[9];
		*ValidN = (data[10]<<8) +data[11];
	}		
	else
		ensure=0xff;
	
	if(ensure==0x00)
	{
		printf("\r\n��Чָ�Ƹ���=%d",(data[10]<<8)+data[11]);
	}
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}


//ģ��Ӧ���ȷ������Ϣ����
//���ܣ�����ȷ���������Ϣ������Ϣ
//����: ensure
const char *EnsureMessage(uint8_t ensure) 
{
	const char *p;
	switch(ensure)
	{
		case  0x00:
			p="OK";break;		
		case  0x01:
			p="���ݰ����մ���";break;
		case  0x02:
			p="��������û����ָ";break;
		case  0x03:
			p="¼��ָ��ͼ��ʧ��";break;
		case  0x04:
			p="ָ��ͼ��̫�ɡ�̫��������������";break;
		case  0x05:
			p="ָ��ͼ��̫ʪ��̫��������������";break;
		case  0x06:
			p="ָ��ͼ��̫�Ҷ�����������";break;
		case  0x07:
			p="ָ��ͼ����������������̫�٣������̫С��������������";break;
		case  0x08:
			p="ָ�Ʋ�ƥ��";break;
		case  0x09:
			p="û������ָ��";break;
		case  0x0a:
			p="�����ϲ�ʧ��";break;
		case  0x0b:
			p="����ָ�ƿ�ʱ��ַ��ų���ָ�ƿⷶΧ";
		case  0x10:
			p="ɾ��ģ��ʧ��";break;
		case  0x11:
			p="���ָ�ƿ�ʧ��";break;	
		case  0x15:
			p="��������û����Чԭʼͼ��������ͼ��";break;
		case  0x18:
			p="��д FLASH ����";break;
		case  0x19:
			p="δ�������";break;
		case  0x1a:
			p="��Ч�Ĵ�����";break;
		case  0x1b:
			p="�Ĵ����趨���ݴ���";break;
		case  0x1c:
			p="���±�ҳ��ָ������";break;
		case  0x1f:
			p="ָ�ƿ���";break;
		case  0x20:
			p="��ַ����";break;
		default :
			p="ģ�鷵��ȷ��������";break;
	}
 return p;	
}

void PS_MAINMENU(void)
{
	uint8_t ensure;
	char *str;
	lcd_clear(WHITE);
	delay_ms(50);
	g_point_color=BLUE;
	text_show_string_middle(0,40,"ָ��ʶ��",16,240,BLACK);	
	lcd_fill(0,120,240,140,WHITE);
	str=mymalloc(SRAMIN,30);
	sprintf(str,"������:%d   ��ַ:%x",usart2_baund,AS608Addr);
	text_show_string(0,60,240,16,str,16,0,BLACK);
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	if(ensure==0x00)
	{
		my_mem_set(str,0,50);
		sprintf(str,"������:%d     �Աȵȼ�: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
		text_show_string(0,80,240,16,str,16,0,BLACK);
	}
	else
		ShowErrMessage(ensure);	
	myfree(SRAMIN,str);
	AS608_load_keyboard(0,170,(uint8_t**)kbd_menu);//�����������	
		
}

//���ذ������棨�ߴ�x,yΪ240*150��
//x,y:������ʼ���꣨240*320�ֱ��ʵ�ʱ��x����Ϊ0��
void AS608_load_keyboard(uint16_t x,uint16_t y,uint8_t **kbtbl)
{
	uint16_t i;
	g_point_color=RED;
	PS_kbd_tbl=kbtbl;
	lcd_fill(x,y,x+240,y+150,WHITE);
	lcd_draw_rectangle(x,y,x+240,y+150,BLACK);						   
	lcd_draw_rectangle(x+80,y,x+160,y+150,BLACK);	 
	lcd_draw_rectangle(x,y+30,x+240,y+60,BLACK);
	lcd_draw_rectangle(x,y+90,x+240,y+120,BLACK);
	g_point_color=BLUE;
	for(i=0;i<15;i++)
	{
		if(i==1)//�������2����:������Ҫ�м���ʾ
			text_show_string(x+(i%3)*80+2,y+7+30*(i/3),80,30,(uint8_t*)PS_kbd_tbl[i],16,0,BLACK);	
		else
			text_show_string_middle(x+(i%3)*80,y+7+30*(i/3),(uint8_t*)PS_kbd_tbl[i],16,80,BLACK);
	} 
}

//����״̬����
//x,y:��������
//key:��ֵ(0~14)
//sta:״̬��0���ɿ���1�����£�
void AS608_key_staset(uint16_t x,uint16_t y,uint8_t keyx,uint8_t sta)
{		  
	uint16_t i=keyx/3,j=keyx%3;
	if(keyx>16)return;
	if(sta &&keyx!=1)//�������2����:������Ҫ���
		lcd_fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,GREEN);
	else if(keyx!=1)
		lcd_fill(x+j*80+1,y+i*30+1,x+j*80+78,y+i*30+28,WHITE);
	if(keyx!=1)//���ǡ�����
		text_show_string_middle(x+j*80,y+7+30*i,(uint8_t*)PS_kbd_tbl[keyx],16,80,BLACK);	
}

//�õ�������������
//x,y:��������
//����ֵ:��1~15,��Ӧ������
uint8_t AS608_get_keynum(uint16_t x,uint16_t y)
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
					AS608_key_staset(x,y,key_x-1,0);
					key_x=key;
					AS608_key_staset(x,y,key_x-1,1);
				}
				break;
			}
	  }  
	}else if(key_x) 
	{
		AS608_key_staset(x,y,key_x-1,0);
		key_x=0;
	} 
	return key; 
}

uint16_t GET_NUM(void)
{
	uint8_t  key_num=0;
	uint16_t num=0;
	while(1)
	{
		key_num=AS608_get_keynum(0,170);	
		if(key_num)
		{
			if(key_num==1)return 0xFFFF;//�����ء���
			if(key_num==3)return 0xFF00;//		
			if(key_num>3&&key_num<13&&num<99)//��1-9����(��������3λ��)
				num =num*10+key_num-3;		
			if(key_num==13)num =num/10;//��Del����			
			if(key_num==14&&num<99)num =num*10;//��0����
			if(key_num==15)return num;  //��Enter����
		}
		lcd_show_num(80+15,170+7,num,6,16,BLACK);
	}	
}

//��ʾȷ���������Ϣ
void ShowErrMessage(uint8_t ensure)
{
	lcd_fill(0,120,lcddev.width,160,WHITE);
	text_show_string_middle(0,120,(char*)EnsureMessage(ensure),16,240,BLACK);
}

//¼ָ��
void Add_FR(void)
{
	uint8_t i,ensure ,processnum=0;
	uint16_t ID;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				lcd_fill(0,100,lcddev.width,160,WHITE);
				text_show_string_middle(0,100,"�밴ָ��",16,240,BLACK);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP(1);
					ensure=PS_GenChar(CharBuffer1);//��������
					BEEP(0);
					if(ensure==0x00)
					{
						lcd_fill(0,120,lcddev.width,160,WHITE);
						text_show_string_middle(0,120,"ָ������",16,240,BLACK);
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				lcd_fill(0,100,lcddev.width,160,WHITE);
				text_show_string_middle(0,100,"�밴�ٰ�һ��ָ��",16,240,BLACK);
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					BEEP(1);
					ensure=PS_GenChar(CharBuffer2);//��������
					BEEP(0);
					if(ensure==0x00)
					{
						lcd_fill(0,120,lcddev.width,160,WHITE);
						text_show_string_middle(0,120,"ָ������",16,240,BLACK);
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				lcd_fill(0,100,lcddev.width,160,WHITE);
				text_show_string_middle(0,100,"�Ա�����ָ��",16,240,BLACK);
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					lcd_fill(0,120,lcddev.width,160,WHITE);
					text_show_string_middle(0,120,"�Աȳɹ�,����ָ��һ��",16,240,BLACK);
					processnum=3;//�������Ĳ�
				}
				else 
				{
					lcd_fill(0,100,lcddev.width,160,WHITE);
					text_show_string_middle(0,100,"�Ա�ʧ�ܣ�������¼��ָ��",16,240,BLACK);
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				delay_ms(1200);
				break;

			case 3:
				lcd_fill(0,100,lcddev.width,160,WHITE);
				text_show_string_middle(0,100,"����ָ��ģ��",16,240,BLACK);
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					lcd_fill(0,120,lcddev.width,160,WHITE);
					text_show_string_middle(0,120,"����ָ��ģ��ɹ�",16,240,BLACK);
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				lcd_fill(0,100,lcddev.width,160,WHITE);
				text_show_string_middle(0,100,"�����봢��ID,��Enter����",16,240,BLACK);
				text_show_string_middle(0,120,"0=< ID <=299",16,240,BLACK);
				do
					ID=GET_NUM();
				while(!(ID<AS608Para.PS_max));//����ID����С��ָ�����������ֵ
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{			
					lcd_fill(0,100,lcddev.width,160,WHITE);					
					text_show_string_middle(0,120,"¼��ָ�Ƴɹ�",16,240,BLACK);
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					lcd_show_num(56,80,AS608Para.PS_max-ValidN,3,16,BLACK);
					delay_ms(1500);
					lcd_fill(0,100,240,160,WHITE);
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(400);
		if(i==5)//����5��û�а���ָ���˳�
		{
			lcd_fill(0,100,lcddev.width,160,WHITE);
			break;	
		}				
	}
}

//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	uint8_t ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{		
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{			
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{	
				LED1(0);		//DS1�̵���
				BEEP(1);		//BEEP��
				delay_ms(500);
				Servo_SetAngle(180);
				LED1(1);
				BEEP(0);		//BEEP����
				delay_ms(2000);
				Servo_SetAngle(20);
				lcd_fill(0,100,lcddev.width,160,WHITE);
				text_show_string_middle(0,100,"ˢָ�Ƴɹ�",16,240,BLACK);				
				str=mymalloc(SRAMIN,50);
				sprintf(str,"ȷ�д���,ID:%d  ƥ��÷�:%d",seach.pageID,seach.mathscore);
				text_show_string_middle(0,140,str,16,240,BLACK);
				myfree(SRAMIN,str);
				delay_ms(2000);
				Servo_SetAngle(20);
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);
	 delay_ms(600);
	 lcd_fill(0,100,lcddev.width,160,WHITE);
	}
		
}

//ɾ��ָ��
void Del_FR(void)
{
	uint8_t  ensure;
	uint16_t num;
	lcd_fill(0,100,lcddev.width,160,WHITE);
	text_show_string_middle(0,100,"ɾ��ָ��",16,240,BLACK);
	text_show_string_middle(0,120,"������ָ��ID��Enter����",16,240,BLACK);
	text_show_string_middle(0,140,"0=< ID <=299",16,240,BLACK);
	delay_ms(50);
	AS608_load_keyboard(0,170,(uint8_t**)kbd_delFR);
	num=GET_NUM();//��ȡ���ص���ֵ
	if(num==0xFFFF)
		goto MENU ; //������ҳ��
	else if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
		lcd_fill(0,120,lcddev.width,160,WHITE);
		text_show_string_middle(0,140,"ɾ��ָ�Ƴɹ�",16,240,BLACK);		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	lcd_show_num(56,80,AS608Para.PS_max-ValidN,3,16,BLACK);
MENU:	
	lcd_fill(0,100,lcddev.width,160,WHITE);
	delay_ms(50);
	AS608_load_keyboard(0,170,(uint8_t**)kbd_menu);
}
