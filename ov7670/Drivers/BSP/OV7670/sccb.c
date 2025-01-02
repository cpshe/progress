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

//SCCB起始信号
//当时钟为高的时候,数据线的高到低,为SCCB起始信号
//在激活状态下,SDA和SCL均为低电平
void SCCB_Start(void)
{
	SCCB_SCL(1);
	SCCB_SDA(1);
	delay_us(50);
	SCCB_SDA(0);
	delay_us(50);
	SCCB_SCL(0);
}

//SCCB停止信号
//当时钟为高的时候,数据线的低到高,为SCCB停止信号
//空闲状况下,SDA,SCL均为高电平
void SCCB_Stop(void)
{
	SCCB_SDA(0);
	delay_us(50);
	SCCB_SCL(1);
	delay_us(50);
	SCCB_SDA(1);
	delay_us(50);
}

//产生ACK应答
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

//产生NACK
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

//SCCB,写入一个字节
//返回值:0,成功;1,失败. 
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
	SCCB_SCL(1);		//接收第九位判断发送是否成功
	delay_us(50);
	if(SCCB_READ_SDA)res=1;	//SDA=1发送失败，返回1
	else res = 0;						//SDA=0发送成功，返回0
	SCCB_SCL(0);
	SCCB_SDA_OUT();
	return res;
}

//SCCB 读取一个字节
//在SCL的上升沿,数据锁存
//返回值:读到的数据
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

//写寄存器
//返回值:0,成功;1,失败.
uint8_t SCCB_WR_Reg(uint8_t reg,uint8_t data)
{
	uint8_t res=0;
	SCCB_Start();
	if(SCCB_WR_Byte(SCCB_ID)) res = 1;//写器件ID
	delay_us(100);
	if(SCCB_WR_Byte(reg)) res=1;//写寄存器
	delay_us(100);
	if(SCCB_WR_Byte(data))res=1;//写数据
	SCCB_Stop();
	return res;
}

//读寄存器
//返回值:读到的寄存器值
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
	SCCB_WR_Byte(SCCB_ID|0x01);//发送读命令
	delay_us(100);
	val = SCCB_RD_Byte();
	SCCB_No_ACK();
	SCCB_Stop();
	return val;
}

























