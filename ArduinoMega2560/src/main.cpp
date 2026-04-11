#include "main.h"


bool receiving = false;                               /** Estado de recepción del esclavo. */
bool packet_ready = false;                            /** Indica que el paquete recibido está listo para ser procesar. */
uint8_t rx_index = 0;                                 /** Índice de escritura en el buffer de recepción del esclavo. */
uint8_t rx_buffer[PACKET_SIZE];                       /** Buffer para almacenar un paquete SPI (13 bytes). */

/**
 * @brief Inicializa el sistema según el modo SPI seleccionado.
 *
 * Configura:
 * - Serial
 * - Validación de modo (MASTER/SLAVE/PRUEBA)
 * - Inicialización SPI
 * - Interrupciones (solo esclavo)
 */
void setup() {
    Serial.begin(115200);
    delay(200); /**< Espera a que Serial esté listo. */

    /* Validación de modo único */
    #if (MASTER + SLAVE + SLAVE_PRUEBA) != 1
        #error "Debes seleccionar exactamente un modo: MASTER, SLAVE o SLAVE_PRUEBA"
    #endif

     /* Modo maestro */
    #if MASTER
        spi_init_master();
    #endif

    /* Modo esclavo */
    #if SLAVE
        spi_init_slave();
        PCICR  |= (1 << PCIE0);  /**< Habilita PCINT0..7 */
        PCMSK0 |= (1 << PCINT0); /**< Interrupción en PB0 (SS) */
    #endif

    /* Modo esclavo de prueba */
    #if SLAVE_PRUEBA
        spi_init_slave();
    #endif

    sei(); /**< Activa interrupciones globales. */

    /* Mensaje de diagnóstico */
    Serial.println(F("Sistema SPI iniciado. Modo: "
        #if MASTER
            "MAESTRO"
        #elif SLAVE
            "ESCLAVO"
        #elif SLAVE_PRUEBA
            "ESCLAVO_PRUEBA"
        #endif
    ));
}


void loop() {

    #if MASTER
        pkt0.ID = 0x01;
        pkt0.payload_0 = 0xAA;
        pkt0.payload_1 = 0xBB;
        pkt0.payload_2 = 0xCC;
        pkt0.payload_3 = 0xDD;
        pkt0.payload_4 = 0xEE;
        pkt0.payload_5 = 0xFF;
        pkt0.payload_6 = 0x11;
        pkt0.payload_7 = 0x22;

        build_packet(&pkt0, true, 4000);
        spi_master_send_block((uint8_t*)&pkt0, PACKET_SIZE, 1000);
    #endif


    #if SLAVE
        if (packet_ready)
        {
            
            Packet received_pkt = {
                .ID = rx_buffer[0],
                .LEN = rx_buffer[1],
                .chechSumByte_0 = rx_buffer[2],  
                .chechSumByte_1 = rx_buffer[3],  
                .payload_0 = rx_buffer[4],
                .payload_1 = rx_buffer[5],
                .payload_2 = rx_buffer[6],
                .payload_3 = rx_buffer[7],
                .payload_4 = rx_buffer[8],
                .payload_5 = rx_buffer[9],
                .payload_6 = rx_buffer[10],
                .payload_7 = rx_buffer[11],
                .CRC = rx_buffer[12]
            };
            
            crc8_valido(&received_pkt);
            checksum8_calculo_slave(&received_pkt);
            mostrar_packet_recibido(&received_pkt);
            
            switch (received_pkt.ID){
                case 0x01:
                    Serial.println(F("Comando 0x01 recibido:"));
                    Serial.print(F("Payload: "));

                    for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {
                        Serial.print(F("0x"));
                        uint8_t val = ((uint8_t*)&received_pkt.payload_0)[i];
                        if (val < 0x10) Serial.print('0');
                        Serial.print(val, HEX);
                        Serial.print(' ');
                    }
                    Serial.println();
                    break;

                case 0x02:
                    Serial.println(F("Comando 0x02 recibido (pendiente de implementar)."));
                    break;

                case 0x03:
                    Serial.println(F("Comando 0x03 recibido (pendiente de implementar)."));
                    break;

                default:
                    Serial.print(F("ID desconocido: 0x"));
                    if (received_pkt.ID < 0x10) Serial.print('0');
                    Serial.println(received_pkt.ID, HEX);
                    break;
        }
            packet_ready = false;
   
        }
    #endif
} 

/* ------------------ ISR DEL ESCLAVO ------------------ */

ISR(PCINT0_vect)
{   
    if (SLAVE == true && SLAVE_PRUEBA == false)
    {
         bool ss_state = PINB & (1 << PB0);

        if (!ss_state) {
            // SS BAJO → INICIO DE PAQUETE
            Serial.println("SS BAJO → INICIO PAQUETE");
            rx_index = 0;
            receiving = true;
            packet_ready = false;   // ← CORRECTO
        } else {
            // SS ALTO → FIN DE PAQUETE
            Serial.print("SS ALTO → FIN PAQUETE. Bytes recibidos: ");
            Serial.println(rx_index);
            receiving = false;
            packet_ready = true;    // ← CORRECTO
        
        }
    }
}


ISR(SPI_STC_vect)
{   
    if (SLAVE == true && SLAVE_PRUEBA == true)
    {
        uint8_t recibido = SPDR;   // lo que mandó el maestro
        SPDR = recibido + 1;       // preparar respuesta para el SIGUIENTE byte
    }

    if (SLAVE == true && SLAVE_PRUEBA == false)
    {
         if (receiving && rx_index < PACKET_SIZE) {
            rx_buffer[rx_index++] = SPDR;
        }
    }
    
}


