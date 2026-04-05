#ifndef MSG_H
#define MSG_H

#include <Arduino.h>
#include "spi.h"
#include "packet.h"

bool verificar_configuracion_spi_PIN_Master();
bool verificar_configuracion_spi_PIN_Slave();
void mostrar_configuracion_SPCR();

#endif  // MSG_H