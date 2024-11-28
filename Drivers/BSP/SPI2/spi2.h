#ifndef __SPI2_H
#define __SPI2_H

#include "./SYSTEM/sys/sys.h"

#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


void SPI2_Init(void);
uint8_t spi2_read_write_byte(uint8_t txdata);
void spi2_set_speed(uint8_t speed);

#endif

