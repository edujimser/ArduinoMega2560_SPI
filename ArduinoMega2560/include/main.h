#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "spi.h"

extern bool receiving;
extern bool packet_ready;
extern uint8_t rx_index;
extern uint8_t rx_buffer[PACKET_SIZE]; 


#endif  // MAIN_H