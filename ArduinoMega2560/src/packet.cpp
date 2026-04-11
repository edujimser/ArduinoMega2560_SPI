
#include "packet.h"

/**
 * @file packet.c
 * @brief Implementación del sistema de construcción, validación y diagnóstico de paquetes SPI.
 *
 * Este módulo proporciona todas las funciones necesarias para gestionar un
 * paquete SPI completo, incluyendo:
 *
 * - Construcción del paquete (`build_packet`)
 * - Cálculo del CRC-8 (`crc8`)
 * - Validación de payloads (`validate_payload`)
 * - Visualización del paquete en formato tabla (`mostrar_packet_tabla`)
 * - Reporte detallado de estados y errores (`mostrar_estado_completo`)
 *
 * También define las estructuras y enumeraciones asociadas al protocolo:
 *
 * - `Packet`: estructura que representa un paquete SPI completo
 * - `PacketStatus`: códigos de estado y error del sistema
 *
 * El objetivo de este módulo es ofrecer una interfaz clara, robusta y
 * fácilmente depurable para la comunicación SPI entre dispositivos.
 *
 * @note Este archivo forma parte del sistema de comunicación SPI diseñado
 *       para proyectos embebidos basados en microcontroladores AVR/Arduino.
 */



/* Instancia del paquete SPI */
Packet pkt0;

/**
 * @brief Construye un paquete SPI válido y calcula su CRC.
 *
 * Esta función valida el contenido del paquete antes de construirlo mediante
 * validate_payload(). Si la validación falla (por ejemplo, por un payload
 * fuera de rango o un puntero inválido), el paquete se limpia completamente
 * con memset() y no se calcula ningún CRC.
 *
 * En caso de que el paquete sea válido, se genera el CRC8 a partir de los
 * campos ID, LEN y los 8 payloads, y se almacena en el campo CRC del paquete.
 * Opcionalmente, puede mostrarse una tabla con el contenido final del paquete.
 *
 * @param pkt      Puntero al paquete a construir y actualizar.
 * @param msg      Si es true, se mostrará una tabla con el contenido del paquete.
 * @param delay_ms Parámetro reservado para uso futuro (por ejemplo, temporización
 *                 asociada al envío del paquete).
 *
 * @note Si la validación falla, el contenido del paquete se resetea a cero
 *       mediante memset() y la función retorna sin modificar el CRC.
 */
void build_packet(Packet *pkt, bool msg, uint32_t delay_ms) {
  
    // Validar el contenido del paquete antes de construirlo
    // Si la validación falla, se muestra el paquete (si msg == true),
    // se limpia completamente la estructura y se aborta la construcción.

    PacketStatus st = validate_payload(pkt);

    // Calcular el checksum de 16 bits a partir del payload y almacenarlo en el paquete
    checksum8_calculo(pkt); 

    if (st != PKT_OK) {
        if (msg) mostrar_packet_tabla(pkt);
        memset(pkt, 0x00, sizeof(Packet));   // Reset completo del paquete
        return;
    }

    // Construir un buffer temporal con los campos que se usarán para el CRC
    uint8_t buffer[PAYLOAD_SIZE];

    buffer[0] = pkt->payload_0;
    buffer[1] = pkt->payload_1;
    buffer[2] = pkt->payload_2;
    buffer[3] = pkt->payload_3;
    buffer[4] = pkt->payload_4;
    buffer[5] = pkt->payload_5;
    buffer[6] = pkt->payload_6;
    buffer[7] = pkt->payload_7;

  
    // Cálculo del CRC8 sobre el buffer construido
    pkt->CRC = crc8(buffer, sizeof(buffer));
   
    // Mostrar el paquete final si se solicita
    if (msg) mostrar_packet_tabla(pkt);
}




/**
 * @brief Calcula el CRC-8 (polinomio 0x07) de un bloque de datos.
 *
 * Esta función implementa el algoritmo CRC-8 estándar basado en el polinomio
 * 0x07 (CRC-8/SMBUS). El cálculo se realiza byte a byte y bit a bit, aplicando
 * desplazamientos y XOR según corresponda. No utiliza XOR final ni valor
 * inicial distinto de cero.
 *
 * @param data Puntero al bloque de datos de entrada.
 * @param len  Número de bytes a procesar.
 *
 * @retval uint8_t Valor CRC-8 resultante del cálculo.
 *
 * @note Este CRC corresponde a la variante CRC-8/SMBUS:
 *       - Polinomio: 0x07
 *       - Valor inicial: 0x00
 *       - Sin XOR final
 *       - Procesamiento MSB-first
 */
uint8_t crc8(const uint8_t *data, size_t len) {

    uint8_t crc = 0x00;  // Valor inicial del CRC

    // Procesar cada byte del buffer
    for (size_t i = 0; i < len; ++i) {

        crc ^= data[i];  // XOR inicial con el byte actual

        // Procesar cada uno de los 8 bits del byte
        for (uint8_t b = 0; b < 8; ++b) {

            // Si el bit más significativo está a 1, aplicar polinomio
            if (crc & 0x80)
                crc = (uint8_t)((crc << 1) ^ 0x07);
            else
                crc = (uint8_t)(crc << 1);
        }
    }

    return crc;  // Resultado final del CRC-8
}




/**
 * @brief Verifica la coherencia de un paquete mediante CRC8.
 *
 * Esta función toma un paquete recibido, extrae su CRC original y calcula
 * nuevamente el CRC8 sobre todos los campos excepto el propio CRC. El cálculo
 * utiliza el polinomio estándar 0x07, sin reflexión y con valor inicial 0x00.
 *
 * El objetivo es determinar si el paquete ha sido recibido correctamente o si
 * presenta corrupción en alguno de sus bytes.
 *
 * @param pkt Puntero al paquete recibido que contiene ID, longitud, payload y CRC.
 *
 * @note El cálculo del CRC se realiza sobre (PACKET_SIZE - 1) bytes, es decir,
 *       todos los campos excepto el byte final que contiene el CRC original.
 *
 * @details
 * Algoritmo CRC8 utilizado:
 *   - Polinomio: 0x07
 *   - Valor inicial: 0x00
 *   - Sin reflexión de entrada
 *   - Sin reflexión de salida
 *   - Sin XOR final
 *
 * El proceso consiste en:
 *   1. Copiar el paquete y poner su CRC a 0.
 *   2. Convertir la estructura a un arreglo de bytes.
 *   3. Construir un buffer con todos los bytes excepto el CRC.
 *   4. Aplicar el algoritmo CRC8 bit a bit.
 *   5. Comparar el CRC calculado con el CRC recibido.
 */
void crc8_valido(Packet *pkt) {

    // 1. Guardar el CRC original del paquete
    uint8_t crc_paquete = pkt->CRC;

    // 2. Crear copia temporal del paquete y poner CRC = 0
    Packet temp_pkt = *pkt;
    temp_pkt.CRC = 0x00;

    // 3. Convertir la estructura a un arreglo de bytes
   uint8_t buffer[8];

    buffer[0] = pkt->payload_0;
    buffer[1] = pkt->payload_1;
    buffer[2] = pkt->payload_2;
    buffer[3] = pkt->payload_3;
    buffer[4] = pkt->payload_4;
    buffer[5] = pkt->payload_5;
    buffer[6] = pkt->payload_6;
    buffer[7] = pkt->payload_7;

    // 4. Calcular CRC8 usando polinomio 0x07
    uint8_t crc = 0x00;

    for (size_t i = 0; i < sizeof(buffer); ++i) {

        crc ^= buffer[i];  // Mezclar el byte actual

        // Procesar cada uno de los 8 bits
        for (uint8_t b = 0; b < 8; ++b) {

            // Si el bit más significativo está a 1, aplicar polinomio
            if (crc & 0x80)
                crc = (uint8_t)((crc << 1) ^ 0x07);
            else
                crc = (uint8_t)(crc << 1);
        }
    }

    mostrar_validacion_crc(crc_paquete, crc);
}



/**
 * @brief Calcula el checksum de 16 bits correspondiente al payload del paquete.
 *
 * Esta función recorre secuencialmente los bytes del payload del paquete,
 * comenzando en POSITION_PAYLOAD_BYTE y procesando exactamente PAYLOAD_SIZE
 * bytes consecutivos. Cada byte se acumula en la variable interna `sum`,
 * cuyo tamaño de 16 bits permite almacenar el resultado completo de la suma.
 *
 * Una vez finalizado el cálculo, el valor de 16 bits se divide en dos partes:
 *   - chechSumByte_0 : Byte menos significativo (LSB).
 *   - chechSumByte_1 : Byte más significativo (MSB).
 *
 * Ambos bytes se almacenan en la estructura Packet siguiendo el formato
 * little-endian, habitual en microcontroladores AVR/ARM, lo que garantiza
 * compatibilidad con protocolos binarios que transmiten valores de 16 bits
 * descompuestos en dos octetos.
 *
 * @note La función no valida el contenido del payload ni comprueba límites.
 *       Se asume que POSITION_PAYLOAD_BYTE y PAYLOAD_SIZE describen un rango
 *       de memoria válido dentro de la estructura Packet.
 *
 * @param pkt Puntero al paquete sobre el que se calcula y almacena el checksum.
 *
 * @pre  `pkt` debe apuntar a una estructura Packet correctamente inicializada.
 * @post Los campos `chechSumByte_0` y `chechSumByte_1` contendrán el checksum
 *       de 16 bits generado a partir del payload.
 */
void checksum8_calculo(Packet *pkt) {
    int16_t sum = 0;

    const uint8_t *raw = (const uint8_t *)pkt;

   for (size_t i = POSITION_PAYLOAD_BYTE; i < POSITION_PAYLOAD_BYTE + PAYLOAD_SIZE; i++) {
    sum += raw[i];
    }

    // Guardar el checksum en dos bytes dentro del paquete
    pkt->chechSumByte_0  = (uint8_t)(sum & 0xFF);        // LSB
    pkt->chechSumByte_1 = (uint8_t)((sum >> 8) & 0xFF);  // MSB
}




/**
 * @brief Calcula el checksum de 16 bits del payload y valida el recibido.
 *
 * Recorre los bytes del payload del paquete recibido y acumula su suma
 * en un entero de 16 bits. Luego extrae el LSB y MSB de dicha suma y los
 * compara con los valores almacenados en el paquete (`chechSumByte_0` y
 * `chechSumByte_1`). Finalmente, muestra el resultado de la validación.
 *
 * @param pkt Puntero al paquete recibido que contiene el payload y los
 *            bytes de checksum enviados por el maestro.
 *
 * @note Esta función no modifica el contenido del paquete; únicamente
 *       calcula y verifica el checksum.
 */
void checksum8_calculo_slave(Packet *pkt) {
    int16_t sum = 0;
    int8_t  sum_Byte_0 = 0;
    int8_t  sum_Byte_1 = 0;
    bool checksum_valido = false;
    
    const uint8_t *raw = (const uint8_t *)pkt;

   for (size_t i = POSITION_PAYLOAD_BYTE; i < POSITION_PAYLOAD_BYTE + PAYLOAD_SIZE; i++) {
    sum += raw[i];
    }

    // Datos del checksum de 16 bits almacenados en el paquete
    sum_Byte_0  = (uint8_t)(sum & 0xFF);        // LSB
    sum_Byte_1 = (uint8_t)((sum >> 8) & 0xFF);  // MSB

    if (pkt->chechSumByte_0 == sum_Byte_0 && pkt->chechSumByte_1 == sum_Byte_1)
    {
        checksum_valido = true;
    }
    
    mostrar_datos_checksum_slave(pkt, sum_Byte_0, sum_Byte_1, checksum_valido);
}




/**
 * @brief Imprime en el monitor serie una tabla detallada con el contenido del paquete.
 *
 * Esta implementación recorre la estructura Packet siguiendo estrictamente el
 * orden real de sus campos en memoria. La salida se organiza en una tabla
 * legible que incluye:
 *
 *   - ID y LEN (cabecera del paquete)
 *   - chechSumByte_0 y chechSumByte_1 (checksum de 16 bits, LSB → MSB)
 *   - payload_0 ... payload_7 (contenido del mensaje)
 *   - CRC final del paquete
 *
 * Cada campo se muestra en formato decimal y hexadecimal para facilitar la
 * inspección del protocolo durante la depuración. La tabla mantiene separadores
 * consistentes y alineación fija para mejorar la legibilidad.
 *
 * @param pkt Puntero a la estructura Packet que se desea visualizar.
 *
 * @pre  `pkt` debe apuntar a una estructura Packet válida.
 * @post Se envía al monitor serie una tabla completa con todos los campos del paquete.
 *
 * @warning Esta función utiliza Serial.println() y no debe ejecutarse dentro
 *          de interrupciones (ISR) ni en contextos donde la salida por UART
 *          pueda bloquear la ejecución.
 */
void mostrar_packet_tabla(Packet *pkt) {

    Serial.println(F("\n========================================="));
    Serial.println(F("              PACKET TABLE"));
    Serial.println(F("========================================="));
    Serial.println(F("| FIELD          |   DEC   |   HEX       |"));
    Serial.println(F("-----------------------------------------"));

    // ---- CAMPOS PRINCIPALES ----
    Serial.print(F("| ID             | "));
    Serial.print(pkt->ID);
    Serial.print(F("      | 0x"));
    Serial.println(pkt->ID, HEX);

    Serial.print(F("| LEN            | "));
    Serial.print(pkt->LEN);
    Serial.print(F("      | 0x"));
    Serial.println(pkt->LEN, HEX);

    Serial.println(F("-----------------------------------------"));
    Serial.println(F("| CHECKSUM 16b   |   DEC   |   HEX       |"));
    Serial.println(F("-----------------------------------------"));

    Serial.print(F("| CHK LSB (0)    | "));
    Serial.print(pkt->chechSumByte_0);
    Serial.print(F("      | 0x"));
    Serial.println(pkt->chechSumByte_0, HEX);

    Serial.print(F("| CHK MSB (1)    | "));
    Serial.print(pkt->chechSumByte_1);
    Serial.print(F("      | 0x"));
    Serial.println(pkt->chechSumByte_1, HEX);

    Serial.println(F("-----------------------------------------"));
    Serial.println(F("| PAYLOAD        |   DEC   |   HEX       |"));
    Serial.println(F("-----------------------------------------"));

    Serial.print(F("| P0             | ")); Serial.print(pkt->payload_0); Serial.print(F("      | 0x")); Serial.println(pkt->payload_0, HEX);
    Serial.print(F("| P1             | ")); Serial.print(pkt->payload_1); Serial.print(F("      | 0x")); Serial.println(pkt->payload_1, HEX);
    Serial.print(F("| P2             | ")); Serial.print(pkt->payload_2); Serial.print(F("      | 0x")); Serial.println(pkt->payload_2, HEX);
    Serial.print(F("| P3             | ")); Serial.print(pkt->payload_3); Serial.print(F("      | 0x")); Serial.println(pkt->payload_3, HEX);
    Serial.print(F("| P4             | ")); Serial.print(pkt->payload_4); Serial.print(F("      | 0x")); Serial.println(pkt->payload_4, HEX);
    Serial.print(F("| P5             | ")); Serial.print(pkt->payload_5); Serial.print(F("      | 0x")); Serial.println(pkt->payload_5, HEX);
    Serial.print(F("| P6             | ")); Serial.print(pkt->payload_6); Serial.print(F("      | 0x")); Serial.println(pkt->payload_6, HEX);
    Serial.print(F("| P7             | ")); Serial.print(pkt->payload_7); Serial.print(F("      | 0x")); Serial.println(pkt->payload_7, HEX);

    Serial.println(F("-----------------------------------------"));

    // ---- CRC ----
    Serial.print(F("| CRC            | "));
    Serial.print(pkt->CRC);
    Serial.print(F("      | 0x"));
    Serial.println(pkt->CRC, HEX);

    Serial.println(F("=========================================\n"));
}





/**
 * @brief Valida todos los campos de payload dentro de un paquete SPI.
 *
 * Esta función verifica que el puntero al paquete sea válido y que cada uno
 * de los 8 payloads se encuentre dentro del rango permitido (0–255).  
 * Si se detecta un error, se informa mediante mostrar_estado_completo() y
 * se devuelve el código de error correspondiente.
 *
 * @param pkt Puntero al paquete que se desea validar.
 *
 * @retval PKT_VALUE_NONE               No se especifica un valor de payload afectado (usado cuando el error no está relacionado con un payload específico).
 * @retval PKT_PAYLOAD_NONE             El paquete no contiene payloads válidos (no se especifica un payload afectado).
 * @retval PKT_OK                       Todos los payloads son válidos.
 * @retval PKT_ERR_NULL_PTR             El puntero al paquete es NULL.
 * @retval PKT_ERR_PAYLOAD_OUT_OF_RANGE Algún payload excede el rango permitido.
 *
 * @note Esta función no modifica el contenido del paquete; solo valida.
 */
PacketStatus validate_payload(Packet *pkt) {
    // Verificar que el tamaño del paquete coincida con PACKET_SIZE
    if (sizeof(Packet) != PACKET_SIZE) {
        mostrar_estado_completo(PKT_ERR_PAYLOAD_SIZE_EXCEEDED, PKT_PAYLOAD_NONE, PKT_VALUE_NONE);
        return PKT_ERR_PAYLOAD_SIZE_EXCEEDED;
    }
    
    // Verificar que el puntero al paquete sea válido
    if (!pkt) {
        mostrar_estado_completo(PKT_ERR_NULL_PTR, PKT_PAYLOAD_NONE, PKT_VALUE_NONE);
        return PKT_ERR_NULL_PTR;
    }

    // Crear un arreglo de punteros a los 8 payloads para recorrerlos de forma compacta
    uint8_t *payloads[8] = {
        &pkt->payload_0, &pkt->payload_1, &pkt->payload_2, &pkt->payload_3,
        &pkt->payload_4, &pkt->payload_5, &pkt->payload_6, &pkt->payload_7
    };

    // Validar cada payload individualmente
    for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {

        // Si el valor excede el rango permitido (0–255), reportar el error
        if (*payloads[i] > 0xFF) {
            mostrar_estado_completo(PKT_ERR_PAYLOAD_OUT_OF_RANGE, i, *payloads[i]);
            return PKT_ERR_PAYLOAD_OUT_OF_RANGE;
        }
    }

    // Si todos los payloads son válidos, reportar estado OK
    mostrar_estado_completo(PKT_OK, PKT_PAYLOAD_NONE, PKT_VALUE_NONE);
    return PKT_OK;
}




/**
 * @brief Muestra en el monitor serie el estado del paquete SPI.
 *
 * Esta función imprime un informe detallado del estado del paquete,
 * indicando si es válido (PKT_OK) o si se ha producido algún error,
 * como un puntero nulo (PKT_ERR_NULL_PTR) o un payload fuera de rango
 * (PKT_ERR_PAYLOAD_OUT_OF_RANGE).
 *
 * Cuando el error está relacionado con un payload, también se muestran
 * el índice del payload afectado y el valor recibido, siempre que dichos
 * parámetros no sean PKT_PAYLOAD_NONE o PKT_VALUE_NONE.
 *
 * @param st            Código de estado del paquete.
 * @param payload_index Índice del payload asociado al error, o
 *                      PKT_PAYLOAD_NONE si no aplica.
 * @param value         Valor del payload asociado al error, o
 *                      PKT_VALUE_NONE si no aplica.
 */

void mostrar_estado_completo(PacketStatus st, int8_t payload_index, uint16_t value) {
    Serial.println(F("\n========================================="));
    Serial.println(F("            PACKET STATUS"));
    Serial.println(F("========================================="));

    switch (st) {

        case PKT_OK:
            Serial.println(F("✔️ Estado: PKT_OK"));
            Serial.println(F("El paquete es válido y no se detectaron errores."));
            break;

        case PKT_ERR_NULL_PTR:
            Serial.println(F("❌ Estado: PKT_ERR_NULL_PTR"));
            Serial.println(F("Se recibió un puntero NULL. No es posible procesar el paquete."));
            break;

        case PKT_ERR_PAYLOAD_OUT_OF_RANGE:
            Serial.println(F("❌ Estado: PKT_ERR_PAYLOAD_OUT_OF_RANGE"));
            Serial.println(F("Uno de los valores del payload está fuera del rango permitido.\n"));

            // Solo mostramos detalles si payload_index es válido
            if (payload_index != PKT_PAYLOAD_NONE) {
                Serial.print(F("Payload afectado: P"));
                Serial.println(payload_index);
            } else {
                Serial.println(F("Payload afectado: (no especificado)"));
            }

            if (value != PKT_VALUE_NONE) {
                Serial.print(F("Valor recibido: "));
                Serial.print(value);
                Serial.print(F(" (0x"));
                Serial.print(value, HEX);
                Serial.println(F(")"));
            } else {
                Serial.println(F("Valor recibido: (no especificado)"));
            }

            Serial.println(F("\nRango permitido: 0 - 255 (0x00 - 0xFF)"));
            break;
        
        case PKT_ERR_PAYLOAD_SIZE_EXCEEDED:
            Serial.println(F("❌ Estado: PKT_ERR_PAYLOAD_SIZE_EXCEEDED"));
            Serial.println(F("El tamaño del paquete no coincide con PACKET_SIZE. Revisa la definición del struct Packet."));
            break;

        default:
            Serial.println(F("⚠️ Estado desconocido."));
            break;
    }

    Serial.println(F("=========================================\n"));
}

/**
 * @brief Muestra por Serial el contenido de un paquete SPI recibido,
 *        indicando la posición de cada byte del payload.
 *
 * @param pkt Puntero al paquete recibido.
 */
void mostrar_packet_recibido(const Packet *pkt) {
    if (!pkt) {
        Serial.println(F("Error: puntero NULL en mostrar_packet_recibido()"));
        return;
    }

    Serial.println(F("\n========================================="));
    Serial.println(F("        PAQUETE RECIBIDO (Contenido Detallado)"));
    Serial.println(F("========================================="));

    // --- ID ---
    Serial.print(F("ID:   0x"));
    if (pkt->ID < 0x10) Serial.print('0');
    Serial.println(pkt->ID, HEX);

    // --- LEN ---
    Serial.print(F("LEN:  "));
    Serial.println(pkt->LEN);

    // --- CHECKSUM 16 bits ---
    Serial.println(F("CHECKSUM 16 bits:"));

    Serial.print(F("  LSB (chechSumByte_0): 0x"));
    if (pkt->chechSumByte_0 < 0x10) Serial.print('0');
    Serial.println(pkt->chechSumByte_0, HEX);

    Serial.print(F("  MSB (chechSumByte_1): 0x"));
    if (pkt->chechSumByte_1 < 0x10) Serial.print('0');
    Serial.println(pkt->chechSumByte_1, HEX);

    // --- PAYLOAD ---
    Serial.println(F("\nPAYLOAD (posición : valor):"));

    const uint8_t *payload = &pkt->payload_0;

    for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {
        Serial.print(F("  ["));
        Serial.print(i);
        Serial.print(F("] = 0x"));
        if (payload[i] < 0x10) Serial.print('0');
        Serial.println(payload[i], HEX);
    }

    // --- CRC ---
    Serial.print(F("\nCRC:  0x"));
    if (pkt->CRC < 0x10) Serial.print('0');
    Serial.println(pkt->CRC, HEX);

    Serial.println(F("=========================================\n"));
}






/**
 * @brief Muestra el resultado de la validación CRC8 entre el CRC recibido y el calculado.
 *
 * Esta función imprime:
 *   - El CRC recibido en el paquete.
 *   - El CRC calculado internamente.
 *   - El resultado de la comparación.
 *
 * @param crc_recibido  CRC original incluido en el paquete.
 * @param crc_calculado CRC obtenido tras procesar los datos del paquete.
 */
void mostrar_validacion_crc(uint8_t crc_recibido, uint8_t crc_calculado)
{
    Serial.println(F("\n========================================="));
    Serial.println(F("        VALIDACIÓN DE CRC DEL PAQUETE"));
    Serial.println(F("========================================="));

    Serial.print(F("CRC recibido Maestro:   0x"));
    Serial.println(crc_recibido, HEX);

    Serial.print(F("CRC calculado Esclavo:  0x"));
    Serial.println(crc_calculado, HEX);

    Serial.println();

    if (crc_recibido == crc_calculado) {
        Serial.println(F("✔ CRC válido. El paquete es coherente."));
    } else {
        Serial.println(F("✘ CRC inválido. El paquete puede estar corrupto."));
    }

    Serial.println(F("=========================================\n"));
}





/**
 * @brief Muestra el resultado de la validación del checksum de 16 bits.
 *
 * Esta función imprime:
 *   - Los bytes de checksum recibidos en el paquete (LSB y MSB).
 *   - Los bytes de checksum calculados por el esclavo.
 *   - El resultado de la comparación.
 *
 * @param pkt            Puntero al paquete recibido.
 * @param sum_Byte_0     Byte LSB del checksum calculado.
 * @param sum_Byte_1     Byte MSB del checksum calculado.
 * @param valido         true si el checksum coincide, false en caso contrario.
 */
void mostrar_datos_checksum_slave(const Packet *pkt,
                                  uint8_t sum_Byte_0,
                                  uint8_t sum_Byte_1,
                                  bool valido)
{
    Serial.println(F("\n========================================="));
    Serial.println(F("        VALIDACIÓN DE CHECKSUM 16b"));
    Serial.println(F("========================================="));

    Serial.print(F("Checksum recibido (LSB): 0x"));
    Serial.println(pkt->chechSumByte_0, HEX);

    Serial.print(F("Checksum recibido (MSB): 0x"));
    Serial.println(pkt->chechSumByte_1, HEX);

    Serial.print(F("Checksum calculado (LSB): 0x"));
    Serial.println(sum_Byte_0, HEX);

    Serial.print(F("Checksum calculado (MSB): 0x"));
    Serial.println(sum_Byte_1, HEX);

    Serial.println();

    if (valido) {
        Serial.println(F("✔ Checksum válido. El paquete es coherente."));
    } else {
        Serial.println(F("✘ Checksum inválido. El paquete puede estar corrupto."));
    }

    Serial.println(F("=========================================\n"));
}
