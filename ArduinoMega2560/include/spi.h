/**
 * @file spi.h
 * @brief API del módulo SPI para el ATmega2560 (maestro y esclavo).
 *
 * Este archivo declara todas las funciones públicas utilizadas para la
 * inicialización, transferencia y diagnóstico del bus SPI. Proporciona
 * una interfaz de alto nivel para:
 *
 *  - Configurar el módulo SPI en modo maestro o esclavo.
 *  - Enviar y recibir bytes mediante transferencia SPI síncrona.
 *  - Enviar bloques completos de datos.
 *  - Validar estados internos del hardware SPI (SPE, SS, WCOL).
 *  - Limpiar el flag SPIF según el procedimiento del datasheet.
 *  - Mostrar información de depuración y errores SPI.
 *
 * Las implementaciones correspondientes se encuentran en spi.c.
 *
 * @note Este módulo está diseñado específicamente para el microcontrolador
 *       ATmega2560 y utiliza registros SPCR, SPSR y SPDR directamente.
 */

#ifndef SPI_H
#define SPI_H

#include <Arduino.h>
#include "msg.h"
#include "configSpi.h"
#include "packet.h"
#include <avr/io.h>

/**
 * @brief Inicializa el módulo SPI en modo maestro.
 *
 * Configura los pines MOSI, MISO, SCK y SS, habilita SPI en modo maestro,
 * selecciona velocidad Fosc/16, modo SPI 0 (CPOL=0, CPHA=0) y limpia SPI2X.
 *
 * @note Debe llamarse antes de cualquier transferencia SPI.
 */
void spi_init_master();

/**
 * @brief Inicializa el módulo SPI en modo esclavo.
 *
 * Configura los pines SPI en modo esclavo, habilita SPI y la interrupción SPIE,
 * establece modo SPI 0 y limpia SPI2X.
 *
 * @note El maestro debe colocar SS en LOW para activar al esclavo.
 */
void spi_init_slave();

/**
 * @brief Envía un byte por SPI como maestro y recibe simultáneamente otro.
 *
 * Gestiona timeout, validación de SPE, estado del pin SS y colisión WCOL.
 * Devuelve el byte recibido o un código de error SPI_ERROR_XXXX.
 *
 * @param data Byte a enviar.
 * @param delay_ms Retardo opcional tras la transferencia.
 * @return uint8_t Byte recibido o código de error.
 */
uint8_t spi_master_transfer(uint8_t data, uint32_t delay_ms);

/**
 * @brief Envía un bloque completo de bytes mediante SPI maestro.
 *
 * Realiza una transferencia por cada byte del buffer, validando errores
 * en cada iteración (timeout, SS, SPE, WCOL). Muestra TX/RX por consola.
 *
 * @param data Puntero al buffer de datos a enviar.
 * @param len Número de bytes a transmitir.
 * @param delay_ms Retardo opcional tras cada transferencia.
 */
void spi_master_send_block(const uint8_t *data, size_t len, uint32_t delay_ms);

/**
 * @brief Imprime por consola la descripción del error SPI recibido.
 *
 * @param error_code Código de error SPI_ERROR_XXXX.
 */
void spi_master_print_error(uint8_t error_code);

/**
 * @brief Muestra el byte enviado y recibido durante una transferencia SPI.
 *
 * Incluye advertencias si el valor recibido es 0x00 o 0xFF.
 *
 * @param enviado Byte transmitido.
 * @param recibido Byte recibido.
 */
void spi_master_print_tx_rx(uint8_t enviado, uint8_t recibido);

/**
 * @brief Verifica si el módulo SPI está habilitado (bit SPE).
 *
 * @return true Si SPI está habilitado.
 * @return false Si SPE está en 0.
 */
char spi_master_Valid_Init_SPE();

/**
 * @brief Verifica si el pin SS está en LOW (esclavo activo).
 *
 * @return true Si SS está en LOW.
 * @return false Si SS está en HIGH.
 */
char spi_master_Valid_Init_SS();

/**
 * @brief Verifica si ocurrió una colisión de escritura (WCOL).
 *
 * @return true Si no hubo colisión.
 * @return false Si WCOL está activo.
 */
char spi_master_Valid_Init_WCOL();

/**
 * @brief Limpia el flag SPIF leyendo SPSR y luego SPDR.
 *
 * Procedimiento obligatorio según el datasheet del ATmega2560.
 */
void clean_SPIF();

#endif  // SPI_H
