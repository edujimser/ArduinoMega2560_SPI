/**
 * @file main.cpp
 * @brief Lógica principal del sistema SPI para el Arduino Mega 2560.
 *
 * Este archivo implementa la inicialización del sistema según el modo seleccionado
 * (MAESTRO, ESCLAVO o ESCLAVO_PRUEBA), el bucle principal de ejecución y las rutinas
 * de interrupción asociadas al SPI y al pin SS.
 *
 * Maneja:
 * - Recepción de paquetes SPI
 * - Procesamiento de comandos
 * - Validación CRC8 y checksum
 * - Respuesta automática en modo SLAVE_PRUEBA
 */

#include "main.h"

/** @brief Estado de recepción del esclavo. */
bool receiving = false;

/** @brief Indica que el paquete recibido está listo para procesarse. */
bool packet_ready = false;

/** @brief Índice de escritura en el buffer de recepción del esclavo. */
uint8_t rx_index = 0;

/** @brief Buffer para almacenar un paquete SPI completo. */
uint8_t rx_buffer[PACKET_SIZE];

/**
 * @brief Inicializa el sistema según el modo SPI seleccionado.
 *
 * Configura:
 * - Comunicación Serial
 * - Validación de modo (MASTER / SLAVE / SLAVE_PRUEBA)
 * - Inicialización del módulo SPI
 * - Interrupciones (solo en modo esclavo)
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

/**
 * @brief Bucle principal del sistema.
 *
 * - En modo MAESTRO: construye y envía un paquete SPI.
 * - En modo ESCLAVO: procesa el paquete recibido cuando está completo.
 */
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

/* -------------------------------------------------------------------------- */
/*                               RUTINAS ISR                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief ISR para cambios en el pin SS (PCINT0).
 *
 * - SS BAJO  → inicio de paquete
 * - SS ALTO  → fin de paquete
 */
ISR(PCINT0_vect)
{
    if (SLAVE == true && SLAVE_PRUEBA == false)
    {
        bool ss_state = PINB & (1 << PB0);

        if (!ss_state) {
            Serial.println("SS BAJO → INICIO PAQUETE");
            rx_index = 0;
            receiving = true;
            packet_ready = false;
        } else {
            Serial.print("SS ALTO → FIN PAQUETE. Bytes recibidos: ");
            Serial.println(rx_index);
            receiving = false;
            packet_ready = true;
        }
    }
}

/**
 * @brief ISR de transferencia SPI (SPI_STC_vect).
 *
 * - En modo SLAVE_PRUEBA: responde automáticamente incrementando el byte recibido.
 * - En modo SLAVE normal: almacena los bytes recibidos en el buffer.
 */
ISR(SPI_STC_vect)
{
    if (SLAVE == true && SLAVE_PRUEBA == true)
    {
        uint8_t recibido = SPDR;
        SPDR = recibido + 1;
    }

    if (SLAVE == true && SLAVE_PRUEBA == false)
    {
        if (receiving && rx_index < PACKET_SIZE) {
            rx_buffer[rx_index++] = SPDR;
        }
    }
}
