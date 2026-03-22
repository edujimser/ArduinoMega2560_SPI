#include "main.h"



void setup() {
    Serial.begin(115200);
    Serial.println("Setup iniciado");

    if (MASTER == true)spi_init_master();
    if (SLAVE == true)spi_init_slave();

    sei();
}


void loop() {
    /* Ejemplo de transferencia SPI maestro-esclavo:
     * El maestro envía 0xA6 y espera una respuesta del esclavo.
     * El esclavo, al recibir un byte, responde con el valor recibido + 1.
     * Se valida la respuesta y se imprimen los resultados por consola.
     */
    /**
    if (MASTER == true) {
        uint16_t respuesta = spi_master_transfer(0xA6, 4000);
        Serial.print("Respuesta válida: 0x"); Serial.println(respuesta, HEX);
    }
    */


     if (MASTER == true) {
        // -------------------------
        // 1) Datos a enviar
        // -------------------------

        uint8_t id = 0x10;                 // ID del comando
        uint8_t dato = 0x42;               // Dato a enviar
        uint8_t dato2 = 0x37;              // Dato a enviar
        uint8_t dato3 = 0x99;              // Dato a enviar
        uint8_t dato4 = 0xAB;              // Dato a enviar

        // -------------------------
        // 2) Construir paquete
        // -------------------------

        uint8_t paquete[7];
        size_t pos = 0;

         
        // -------------------------
        // 3) construir paquete dato
        // -------------------------
        paquete[pos++] = id;    // byte 0: ID del comando
        paquete[pos++] = dato;  // byte 1: dato
        paquete[pos++] = dato2;  // byte 2: dato2
        paquete[pos++] = dato3;  // byte 3: dato3
        paquete[pos++] = dato4;  // byte 4: dato4
        
        // -------------------------
        // 4) Enviar paquete
        // -------------------------

        Serial.println(F("\n--- Enviando paquete SPI ---"));
        spi_master_send_block(paquete, pos, 2);
        Serial.println(F("--- Fin de envío SPI ---\n"));

        delay(3000);

     }

} 

/* ------------------ ISR DEL ESCLAVO ------------------ */
ISR(SPI_STC_vect) {
    uint8_t recibido = SPDR;   // lo que mandó el maestro
    SPDR = recibido + 1;       // preparar respuesta para el SIGUIENTE byte
}