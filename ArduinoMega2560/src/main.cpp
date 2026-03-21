#include "main.h"

   #define MASTER 1
   #define SLAVE  0

void setup() {
    Serial.begin(115200);
    Serial.println("Setup iniciado");

    if (MASTER == true)spi_init_master();
    if (SLAVE == true)spi_init_slave();
}


void loop() {
    if (MASTER == true) {
        uint16_t respuesta = spi_master_transfer(0xA6, 4000);
        Serial.print("Respuesta válida: 0x"); Serial.println(respuesta, HEX);
    }
    
} 

/* ------------------ ISR DEL ESCLAVO ------------------ */
ISR(SPI_STC_vect) {
    uint8_t recibido = SPDR;   // lo que mandó el maestro
    SPDR = recibido + 3;       // respuesta del esclavo
}