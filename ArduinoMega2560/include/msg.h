/**
 * @file msg.h
 * @brief Funciones de diagnóstico y verificación de la configuración SPI en el Arduino Mega 2560.
 *
 * Este archivo declara funciones utilizadas para comprobar el estado de los pines SPI,
 * validar la configuración del registro SPCR y mostrar información útil para depuración.
 *
 * @note Forma parte del sistema de comunicación SPI entre Maestro y Esclavo.
 */

#ifndef MSG_H
#define MSG_H

#include <Arduino.h>
#include "spi.h"
#include "packet.h"

/**
 * @brief Verifica si la configuración de pines SPI corresponde al modo Maestro.
 *
 * Comprueba que los pines MOSI, MISO, SCK y SS están configurados correctamente
 * para operar como Maestro SPI.
 *
 * @return true si la configuración es correcta, false en caso contrario.
 */
bool verificar_configuracion_spi_PIN_Master();

/**
 * @brief Verifica si la configuración de pines SPI corresponde al modo Esclavo.
 *
 * Comprueba que los pines SPI están configurados adecuadamente para operar como Esclavo.
 *
 * @return true si la configuración es correcta, false en caso contrario.
 */
bool verificar_configuracion_spi_PIN_Slave();

/**
 * @brief Muestra el contenido del registro SPCR para depuración.
 *
 * Imprime en el monitor serie los bits relevantes del registro SPCR,
 * permitiendo verificar la configuración del módulo SPI.
 */
void mostrar_configuracion_SPCR();

#endif  // MSG_H
