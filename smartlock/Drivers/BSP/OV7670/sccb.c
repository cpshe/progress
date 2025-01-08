#include "./SYSTEM/sys/sys.h"
#include "./BSP/OV7670/sccb.h"
#include "./SYSTEM/delay/delay.h"

void SCCB_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_13;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOG,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_3;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	SCCB_SDA_OUT();
}

//SCCB��ʼ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĸߵ���,ΪSCCB��ʼ�ź�
//�ڼ���״̬��,SDA��SCL��Ϊ�͵�ƽ
void SCCB_Start(void)
{
	SCCB_SCL(1);
	SCCB_SDA(1);
	delay_us(50);
	SCCB_SDA(0);
	delay_us(50);
	SCCB_SCL(0);
}

//SCCBֹͣ�ź�
//��ʱ��Ϊ�ߵ�ʱ��,�����ߵĵ͵���,ΪSCCBֹͣ�ź�
//����״����,SDA,SCL��Ϊ�ߵ�ƽ
void SCCB_Stop(void)
{
	SCCB_SDA(0);
	delay_us(50);
	SCCB_SCL(1);
	delay_us(50);
	SCCB_SDA(1);
	delay_us(50);
}

//����ACKӦ��
void SCCB_ACK(void)
{
	SCCB_SDA(0);
	delay_us(50);
	SCCB_SCL(1);
	delay_us(50);
	SCCB_SCL(0);
	delay_us(50);
	SCCB_SDA(1);
}

//����NACK
void SCCB_No_ACK(void)
{
	delay_us(50);
	SCCB_SDA(1);
	delay_us(50);
	SCCB_SCL(1);
	delay_us(50);
	SCCB_SCL(0);
	delay_us(50);
	SCCB_SDA(0);
	delay_us(50);
}

//SCCB,д��һ���ֽ�
//����ֵ:0,�ɹ�;1,ʧ��. 
uint8_t SCCB_WR_Byte(uint8_t dat)
{
	uint8_t j,res;
	for(j=0;j<8;j++)
	{
		if(dat&0x80) 
		{
			SCCB_SDA(1);
		}
		else 
		{
			SCCB_SDA(0);
		}
		dat <<= 1;
		delay_us(50);
		SCCB_SCL(1);
		delay_us(50);
		SCCB_SCL(0);
	}
	SCCB_SDA_IN();
	SCCB_SDA(1);
	delay_us(50);
	SCCB_SCL(1);		//���յھ�λ�жϷ����Ƿ�ɹ�
	delay_us(50);
	if(SCCB_READ_SDA)res=1;	//SDA=1����ʧ�ܣ�����1
	else res = 0;						//SDA=0���ͳɹ�������0
	SCCB_SCL(0);
	SCCB_SDA_OUT();
	return res;
}

//SCCB ��ȡһ���ֽ�
//��SCL��������,��������
//����ֵ:����������
uint8_t SCCB_RD_Byte(void)
{
	uint8_t temp=0,j;
	SCCB_SDA_IN();
	for(j=8;j>0;j--)
	{
		delay_us(50);
		SCCB_SCL(1);
		temp=temp<<1;
		if(SCCB_READ_SDA)temp++;
		delay_us(50);
		SCCB_SCL(0);
	}
	SCCB_SDA_OUT();
	return temp;
}

//д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data)
{
	uint8_t res=0;
	SCCB_Start();
	if(SCCB_WR_Byte(SCCB_ID)) res = 1;//д����ID
	delay_us(100);
	if(SCCB_WR_Byte(reg)) res=1;//д�Ĵ���
	delay_us(100);
	if(SCCB_WR_Byte(data))res=1;//д����
	SCCB_Stop();
	return res;
}

//���Ĵ���
//����ֵ:�����ļĴ���ֵ
uint8_t SCCB_RD_Reg(uint8_t reg)
{
	uint8_t val = 0;
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID);
	delay_us(100);
	SCCB_WR_Byte(reg);
	delay_us(100);
	SCCB_Stop();
	delay_us(100);
	
	SCCB_Start();
	SCCB_WR_Byte(SCCB_ID|0x01);//���Ͷ�����
	delay_us(100);
	val = SCCB_RD_Byte();
	SCCB_No_ACK();
	SCCB_Stop();
	return val;
}

























