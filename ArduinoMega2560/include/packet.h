/**
 * @file packet.h
 * @brief Definición del protocolo SPI, estructura Packet y funciones asociadas.
 *
 * Este archivo contiene la definición del paquete SPI utilizado en el sistema,
 * así como los códigos de estado, constantes del protocolo y prototipos de
 * funciones para construir, validar y visualizar paquetes.
 */

#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>

/**
 * @def PAYLOAD_SIZE
 * @brief Número de bytes disponibles en el payload del paquete SPI.
 */
#define PAYLOAD_SIZE 8

/**
 * @def PACKET_SIZE
 * @brief Tamaño total del paquete SPI (ID + LEN + PAYLOAD + CRC).
 */
#define PACKET_SIZE (1 + 1 + PAYLOAD_SIZE + 1)

/**
 * @enum PacketStatus
 * @brief Códigos de estado para validación y diagnóstico de paquetes SPI.
 */
typedef enum {
    PKT_VALUE_NONE = -2,               /**< No se proporciona un valor asociado. */
    PKT_PAYLOAD_NONE = -1,             /**< No se especifica un índice de payload. */
    PKT_OK = 0,                        /**< El paquete es válido. */
    PKT_ERR_PAYLOAD_OUT_OF_RANGE = 1,  /**< Algún payload excede el rango permitido. */
    PKT_ERR_NULL_PTR = 2,              /**< El puntero al paquete es NULL. */
    PKT_ERR_PAYLOAD_SIZE_EXCEEDED = 3  /**< El tamaño del struct no coincide con PACKET_SIZE. */
} PacketStatus;

/**
 * @struct Packet
 * @brief Representa un paquete SPI completo.
 *
 * Esta estructura contiene todos los campos necesarios para transmitir
 * un mensaje SPI según el protocolo definido.
 */
typedef struct {
    uint8_t ID;                        /**< Identificador del comando. */
    uint8_t LEN = PACKET_SIZE;         /**< Longitud del payload (0–8). */
    uint8_t payload_0;                 /**< Byte 0 del payload. */
    uint8_t payload_1;                 /**< Byte 1 del payload. */
    uint8_t payload_2;                 /**< Byte 2 del payload. */
    uint8_t payload_3;                 /**< Byte 3 del payload. */
    uint8_t payload_4;                 /**< Byte 4 del payload. */
    uint8_t payload_5;                 /**< Byte 5 del payload. */
    uint8_t payload_6;                 /**< Byte 6 del payload. */
    uint8_t payload_7;                 /**< Byte 7 del payload. */
    uint8_t CRC;                       /**< CRC-8 del paquete. */
} Packet;

/**
 * @brief Verificación en tiempo de compilación del tamaño del paquete.
 */
static_assert(sizeof(Packet) == PACKET_SIZE,
              "Tamaño del struct Packet no coincide con PACKET_SIZE. Revisa la definición.");

/**
 * @brief Instancia global del paquete (opcional).
 */
extern Packet pkt0;

/**
 * @brief Construye un paquete SPI válido y calcula su CRC.
 *
 * @param pkt Puntero al paquete a construir.
 * @param msg Si es true, muestra el paquete por Serial.
 * @param delay_ms Retardo opcional tras mostrar el paquete.
 */
void build_packet(Packet *pkt, bool msg, uint32_t delay_ms);

/**
 * @brief Calcula el CRC-8 de un buffer.
 *
 * @param data Puntero al buffer.
 * @param len Longitud del buffer.
 * @return Valor CRC-8 calculado.
 */
uint8_t crc8(const uint8_t *data, size_t len);

/**
 * @brief Verifica la coherencia de un paquete mediante CRC8.
 *
 * Esta función recalcula el CRC8 del paquete recibido (excluyendo el byte
 * de CRC original) y permite validar si el contenido es íntegro o si ha
 * sufrido corrupción durante la transmisión.
 *
 * @param pkt Puntero al paquete recibido.
 */
void crc8_valido(Packet *pkt);

/**
 * @brief Muestra el contenido del paquete en formato tabla.
 *
 * @param pkt Puntero al paquete.
 */
void mostrar_packet_tabla(Packet *pkt);

/**
 * @brief Valida los campos del payload y la coherencia del paquete.
 *
 * @param pkt Puntero al paquete.
 * @return Código de estado PacketStatus.
 */
PacketStatus validate_payload(Packet *pkt);

/**
 * @brief Muestra información detallada del estado del paquete.
 *
 * @param st Código de estado.
 * @param payload_index Índice del payload afectado (si aplica).
 * @param value Valor asociado al error (si aplica).
 */
void mostrar_estado_completo(PacketStatus st, int8_t payload_index, uint16_t value);

/**
 * @brief Muestra por Serial el contenido de un paquete SPI recibido.
 *
 * Imprime:
 * - ID del paquete
 * - Longitud (LEN)
 * - Cada byte del payload con su posición
 * - CRC final
 *
 * @param pkt Puntero al paquete recibido. Debe ser válido.
 */
void mostrar_packet_recibido(const Packet *pkt);

/**
 * @brief Muestra en el monitor serie el resultado de la validación CRC8.
 *
 * Esta función imprime de forma clara y estructurada:
 *   - El CRC recibido dentro del paquete.
 *   - El CRC calculado a partir de los datos del paquete.
 *   - El resultado de la comparación entre ambos valores.
 *
 * Su propósito es facilitar la depuración del sistema de comunicación,
 * permitiendo verificar si el paquete recibido es coherente o presenta
 * posibles errores de transmisión.
 *
 * @param crc_recibido   Valor CRC incluido en el paquete recibido.
 * @param crc_calculado  Valor CRC generado localmente a partir del contenido del paquete.
 */
void mostrar_validacion_crc(uint8_t crc_recibido, uint8_t crc_calculado);


#endif  // PACKET_H
