/**
 * @file main.h
 * @brief Declaraciones globales y variables compartidas para el sistema SPI del Arduino Mega 2560.
 *
 * Este archivo contiene las variables externas utilizadas por el módulo principal
 * y el manejador de recepción SPI. Define los flags de estado, el índice de recepción
 * y el buffer donde se almacena el paquete recibido.
 *
 * @note Este archivo forma parte del sistema de comunicación SPI entre el Maestro y el Esclavo.
 */

#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "spi.h"

/**
 * @brief Indica si actualmente se está recibiendo un paquete SPI.
 */
extern bool receiving;

/**
 * @brief Señala que un paquete completo ha sido recibido y está listo para procesarse.
 */
extern bool packet_ready;

/**
 * @brief Índice actual de escritura dentro del buffer de recepción.
 */
extern uint8_t rx_index;

/**
 * @brief Buffer donde se almacena el paquete recibido vía SPI.
 */
extern uint8_t rx_buffer[PACKET_SIZE];

#endif  // MAIN_H
