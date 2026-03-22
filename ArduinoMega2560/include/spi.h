#ifndef SPI_H
#define SPI_H

#include <Arduino.h>
#include "msg.h"
#include "configSpi.h"
#include <avr/io.h>

void spi_init_master();
void spi_init_slave();
uint16_t spi_master_transfer(uint8_t data, uint32_t delay_ms);
void spi_master_send_block(const uint8_t *data, size_t len, uint32_t delay_ms);
void spi_master_print_error(uint16_t error_code);
void spi_master_print_tx_rx(uint8_t enviado, uint8_t recibido);
char spi_master_Valid_Init_SPE();
char spi_master_Valid_Init_SS();
char spi_master_Valid_Init_WCOL();
void clean_SPIF();
#endif  // SPI_H