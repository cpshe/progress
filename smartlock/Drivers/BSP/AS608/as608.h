#ifndef __AS608_H
#define __AS608_H

#include "./SYSTEM/sys/sys.h"

#define PS_Sta        		HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)
#define CharBuffer1 0x01
#define CharBuffer2 0x02

extern uint32_t AS608Addr;


typedef struct  
{
	uint16_t pageID;//ָ��ID
	uint16_t mathscore;//ƥ��÷�
}SearchResult;

typedef struct
{
	uint16_t PS_max;//ָ���������
	uint8_t  PS_level;//��ȫ�ȼ�
	uint32_t PS_addr;
	uint8_t  PS_size;//ͨѶ���ݰ���С
	uint8_t  PS_N;//�����ʻ���N
}SysPara;

void PS_StaGPIO_Init(void);//��ʼ��PA6��״̬����

uint8_t PS_GetImage(void); //¼��ͼ�� 
 
uint8_t PS_GenChar(uint8_t BufferID);//�������� 

uint8_t PS_Match(void);//��ȷ�ȶ���öָ������ 

uint8_t PS_Search(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);//����ָ�� 
 
uint8_t PS_RegModel(void);//�ϲ�����������ģ�壩 
 
uint8_t PS_StoreChar(uint8_t BufferID,uint16_t PageID);//����ģ�� 

uint8_t PS_DeletChar(uint16_t PageID,uint16_t N);//ɾ��ģ�� 

uint8_t PS_Empty(void);//���ָ�ƿ� 

uint8_t PS_WriteReg(uint8_t RegNum,uint8_t DATA);//дϵͳ�Ĵ��� 
 
uint8_t PS_ReadSysPara(SysPara *p); //��ϵͳ�������� 

uint8_t PS_SetAddr(uint32_t addr);  //����ģ���ַ 

uint8_t PS_WriteNotepad(uint8_t NotePageNum,uint8_t *content);//д���±� 

uint8_t PS_ReadNotepad(uint8_t NotePageNum,uint8_t *note);//������ 

uint8_t PS_HighSpeedSearch(uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,SearchResult *p);//�������� 
  
uint8_t PS_ValidTempleteNum(uint16_t *ValidN);//����Чģ����� 

uint8_t PS_HandShake(uint32_t *PS_Addr); //��AS608ģ������


void PS_MAINMENU(void);//ָ�ƽ���������
void AS608_load_keyboard(uint16_t x,uint16_t y,uint8_t **kbtbl);//�����������
void AS608_key_staset(uint16_t x,uint16_t y,uint8_t keyx,uint8_t sta);
uint8_t AS608_get_keynum(uint16_t x,uint16_t y);//��ȡ������
uint16_t GET_NUM(void);//��ȡ��ֵ
void ShowErrMessage(uint8_t ensure);//��ʾȷ���������Ϣ
void Add_FR(void);//¼ָ��
void press_FR(void);//ɾ��ָ��
void Del_FR(void);//ˢָ��	
const char *EnsureMessage(uint8_t ensure) ;
void ShowErrMessage(uint8_t ensure);


#endif
