#ifndef SPI_H
#define SPI_H

#include <Arduino.h>
#include "msg.h"
#include <avr/io.h>

//Code de error para transferencias SPI
#define SPI_ERROR_TIMEOUT 0xFFFF
#define SPI_ERROR_NO_INIT_SPE 0xFFFE 
#define SPI_ERROR_NO_INIT_SS 0xFFFD
#define SPI_ERROR_WCOL 0xFFFC
#define SPI_ERROR_NO_REQUEST_SLAVE 0xFFFB

void spi_init_master();
void spi_init_slave();
uint16_t spi_master_transfer(uint8_t data, uint32_t delay_ms);
char spi_master_Valid_Init_SPE();
char spi_master_Valid_Init_SS();
char spi_master_Valid_Init_WCOL();
void clean_SPIF();
#endif  // SPI_H