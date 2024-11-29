#include "./SYSTEM/sys/sys.h"
#include "./BSP/SPI2/spi2.h"

SPI_HandleTypeDef SPI_InitStructure;


void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_SPI2_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
	
	SPI_InitStructure.Instance = SPI2;
	SPI_InitStructure.Init.Mode = SPI_MODE_MASTER;
	SPI_InitStructure.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_InitStructure.Init.CLKPolarity =	SPI_POLARITY_HIGH;
	SPI_InitStructure.Init.CLKPhase = SPI_PHASE_2EDGE;
	SPI_InitStructure.Init.NSS = SPI_NSS_SOFT;
	SPI_InitStructure.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SPI_InitStructure.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_InitStructure.Init.CRCPolynomial = 7;

	HAL_SPI_Init(&SPI_InitStructure);                                     /* ��ʼ�� */
	__HAL_SPI_ENABLE(&SPI_InitStructure);
}

void spi2_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* �ж���Ч�� */
    __HAL_SPI_DISABLE(&SPI_InitStructure);             /* �ر�SPI */
    SPI_InitStructure.Instance->CR1 &= 0XFFC7;         /* λ3-5���㣬�������ò����� */
    SPI_InitStructure.Instance->CR1 |= speed << 3;     /* ����SPI�ٶ� */
    __HAL_SPI_ENABLE(&SPI_InitStructure);              /* ʹ��SPI */
}

/**
 * @brief       SPI2��дһ���ֽ�����
 * @param       txdata  : Ҫ���͵�����(1�ֽ�)
 * @retval      ���յ�������(1�ֽ�)
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&SPI_InitStructure, &txdata, &rxdata, 1, 1000);
    return rxdata; /* �����յ������� */
}
