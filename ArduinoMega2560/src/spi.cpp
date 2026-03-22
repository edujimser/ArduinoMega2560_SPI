#include "spi.h"

/**
 * @brief Inicializa el módulo SPI en modo MAESTRO.
 *
 * Configura los pines físicos del bus SPI:
 *  - MOSI (51) como salida
 *  - MISO (50) como entrada
 *  - SCK  (52) como salida
 *  - SS   (53) como salida
 *
 * Además:
 *  - Coloca SS en HIGH para desactivar al esclavo.
 *  - Verifica que los pines estén correctamente configurados.
 *  - Configura el registro SPCR para habilitar SPI en modo maestro.
 *  - Ajusta la velocidad SPI a Fosc/16.
 *  - Configura el modo SPI 0 (CPOL=0, CPHA=0).
 *  - Asegura que SPSR no use doble velocidad (SPI2X=0).
 *  - Muestra la configuración final de SPCR.
 */
void spi_init_master() {

    pinMode(PIN_MOSI, OUTPUT); /*< MOSI: el maestro envía datos */
    pinMode(PIN_MISO, INPUT_PULLUP);  /*< MISO: el maestro recibe datos */
    pinMode(PIN_SCK, OUTPUT); /*< SCK: reloj generado por el maestro */
    pinMode(PIN_SS, OUTPUT); /*< SS: selección del esclavo */

    digitalWrite(PIN_SS, HIGH); /*< SS en HIGH → esclavo desactivado */

    if (!verificar_configuracion_spi_PIN_Master()) return;

    /**
     * Configuración del registro SPCR:
     *  SPE  = 1 → SPI habilitado
     *  MSTR = 1 → Modo maestro
     *  SPR0 = 1 → Velocidad Fosc/16
     *  CPOL = 0 → Reloj en LOW en reposo
     *  CPHA = 0 → Captura en flanco de subida
     *  DORD = 0 → MSB primero
     */
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    /**
     *  SPI2X = 0 → velocidad normal (sin multiplicar por 2)
     */
    SPSR &= ~(1 << SPI2X);

    mostrar_configuracion_SPCR();
}


/**
 * @brief Inicializa el módulo SPI en modo ESCLAVO.
 *
 * Configura los pines físicos del bus SPI:
 *  - MOSI (51) como entrada
 *  - MISO (50) como salida
 *  - SCK  (52) como entrada
 *  - SS   (53) como entrada
 *
 * Además:
 *  - Coloca SS en HIGH (el maestro lo pondrá en LOW cuando quiera hablar).
 *  - Verifica que los pines estén correctamente configurados.
 *  - Configura SPCR para habilitar SPI en modo esclavo.
 *  - Habilita la interrupción SPI (SPIE) para reaccionar cuando llega un byte.
 *  - Configura el modo SPI 0 (CPOL=0, CPHA=0).
 *  - Limpia SPI2X en SPSR (no afecta al esclavo).
 *  - Muestra la configuración final de SPCR.
 */
void spi_init_slave() {

    pinMode(PIN_MOSI, INPUT);   /*< MOSI: el esclavo recibe datos */
    pinMode(PIN_MISO, OUTPUT);  /*< MISO: el esclavo envía datos */
    pinMode(PIN_SCK, INPUT);   /*< SCK: reloj recibido del maestro */
    pinMode(PIN_SS, INPUT);   /*< SS: activado por el maestro */

    digitalWrite(PIN_SS, HIGH); /*< SS en HIGH → esclavo desactivado */

    if (!verificar_configuracion_spi_PIN_Slave()) return;

    /**
     * Configuración del registro SPCR:
     *  SPIE = 1 → interrupción SPI habilitada
     *  SPE  = 1 → SPI habilitado
     *  MSTR = 0 → modo esclavo
     *  CPOL = 0 → reloj en LOW en reposo
     *  CPHA = 0 → captura en flanco de subida
     *  DORD = 0 → MSB primero
     *
     * SPR1 y SPR0 NO afectan al esclavo.
     */
    SPCR = (1 << SPE) | (1 << SPIE);

    /**
     * SPSR:
     *  SPI2X = 0 → sin efecto en esclavo, pero se limpia por coherencia
     */
    SPSR &= ~(1 << SPI2X);

    mostrar_configuracion_SPCR();
}


/**
 * @brief Realiza una transferencia SPI como maestro (envía y recibe un byte).
 *
 * Esta función escribe un byte en SPDR, espera a que la transferencia SPI
 * finalice (SPIF = 1), verifica errores como timeout, SPI deshabilitado,
 * pin SS incorrecto o colisión de escritura (WCOL), y finalmente devuelve
 * el byte recibido desde el esclavo.
 *
 * Durante la transferencia se desactivan las interrupciones (cli) para evitar
 * que una ISR interfiera con SPDR, SPSR o SPIF, lo cual podría corromper
 * la comunicación. Al finalizar, se restaura el estado original de SREG.
 *
 * @param data Byte a enviar al esclavo SPI.
 * @return uint8_t Byte recibido desde el esclavo. Devuelve 0xFF en caso de error.
 */
uint16_t spi_master_transfer(uint8_t data, uint32_t delay_ms) {
    /* Variables */
    uint32_t timeout = micros();
   

    /* Interrupcion */
    clean_SPIF();                                                           /*< Asegura que SPIF esté limpio antes de iniciar la transferencia */
    uint8_t sreg = SREG;                                                    /*< Guarda el estado de interrupciones */
    cli();                                                                  /*< Desactiva interrupciones para proteger la transferencia */

    /* Iniciar transferencia */
    digitalWrite(PIN_SS, LOW);                                                  /*< Activar esclavo */
    SPDR = data;                                                            /*< Inicia la transferencia cargando el dato en SPDR */

    /* Esperar a que SPIF indique que la transferencia terminó */
    while (!(SPSR & (1 << SPIF))) {
        if (micros() - timeout > 10000) {                                   /* Timeout de 10 ms */
            Serial.println(F("Error: Timeout en transferencia SPI"));
            clean_SPIF();                                                   /* Limpia SPIF para evitar estados inconsistentes */
            SREG = sreg;                                                    /* Restaura interrupciones */
            digitalWrite(53, HIGH);                                         /* Desactivar esclavo */
            return SPI_ERROR_TIMEOUT;
        }
    }

    /* Errores */
                                                                           /* Validar que el SPI está habilitado */
    if (!spi_master_Valid_Init_SPE()) {
        Serial.println(F("Error: SPI no está correctamente inicializado"));
        clean_SPIF();                                                       /* Limpia SPIF para evitar estados inconsistentes */
        SREG = sreg;                                                       /* Restaura el estado original de interrupciones */
        digitalWrite(53, HIGH);                                            /* Desactivar esclavo */
        delay(delay_ms);                                                   /* Retardo opcional después de la transferencia */
        return SPI_ERROR_NO_INIT_SPE;
    }

                                                                           /* Validar que el pin SS está en LOW (esclavo seleccionado) */
    if (!spi_master_Valid_Init_SS()) {
        Serial.println(F("Error: Pin SS no está en LOW, esclavo no activo"));
        clean_SPIF();                                                      /* Limpia SPIF para evitar estados inconsistentes */
        SREG = sreg;                                                       /* Restaura el estado original de interrupciones */
        digitalWrite(53, HIGH);                                            /* Desactivar esclavo */
        delay(delay_ms);                                                   /* Retardo opcional después de la transferencia */
        return SPI_ERROR_NO_INIT_SS;
    }

                                                                           /* Validar que no hubo colisión de escritura */
    if (!spi_master_Valid_Init_WCOL()) {
        Serial.println(F("Error: Colisión de escritura detectada"));
        clean_SPIF();                                                     /* Limpia SPIF para evitar estados inconsistentes */
        SREG = sreg;                                                      /* Restaura el estado original de interrupciones */
        digitalWrite(53, HIGH);                                           /* Desactivar esclavo */
        delay(delay_ms);                                                  /* Retardo opcional después de la transferencia */
        return SPI_ERROR_WCOL;
    }

    /* Recepcion */
    uint8_t recibido = SPDR;                                              /* Lee el byte recibido del esclavo */
    SREG = sreg;                                                          /* Restaura el estado original de interrupciones */
    clean_SPIF();                                                         /* Limpia SPIF para evitar estados inconsistentes */
    digitalWrite(53, HIGH);                                               /* Desactivar esclavo */

    /* Validar el byte recibido */
    if (recibido == 0xFF || recibido == 0x00) { 
        Serial.println(F("Aviso: No hay esclavo conectado o no responde"));                                  
        delay(delay_ms);                                                   /* Retardo opcional después de la transferencia */
        return SPI_ERROR_NO_REQUEST_SLAVE;
    }


    delay(delay_ms);                                                       /* Retardo opcional después de la transferencia */
    return recibido;
}


void spi_master_send_block(const uint8_t *data, size_t len, uint32_t delay_ms) {

    clean_SPIF();               
    uint8_t sreg = SREG;        
    cli();                      

    digitalWrite(PIN_SS, LOW);  

    for (size_t i = 0; i < len; i++) {

        clean_SPIF();           
        SPDR = data[i];         

        uint32_t t0 = micros();
        while (!(SPSR & (1 << SPIF))) {
            if (micros() - t0 > 10000) {
                spi_master_print_error(SPI_ERROR_TIMEOUT);
                clean_SPIF();
                SREG = sreg;
                digitalWrite(PIN_SS, HIGH);
                return;
            }
        }

        if (!spi_master_Valid_Init_SPE()) {
            delay(delay_ms);
            spi_master_print_error(SPI_ERROR_NO_INIT_SPE);
            clean_SPIF();
            SREG = sreg;
            digitalWrite(PIN_SS, HIGH);
            return;
        }

        if (!spi_master_Valid_Init_SS()) {
            delay(delay_ms);
            spi_master_print_error(SPI_ERROR_NO_INIT_SS);
            clean_SPIF();
            SREG = sreg;
            digitalWrite(PIN_SS, HIGH);
            return;
        }

        if (!spi_master_Valid_Init_WCOL()) {
            delay(delay_ms);
            spi_master_print_error(SPI_ERROR_WCOL);
            clean_SPIF();
            SREG = sreg;
            digitalWrite(PIN_SS, HIGH);
            return;
        }

        uint8_t recibido = SPDR;
        spi_master_print_tx_rx(data[i], recibido);
    }

    SREG = sreg;                                                             
    clean_SPIF();                                                            
    digitalWrite(PIN_SS, HIGH);                                              
}


/**
 * @brief Verifica si el módulo SPI está habilitado (bit SPE en SPCR).
 *
 * @return true Si SPI está habilitado.
 * @return false Si SPI está deshabilitado.
 */
char spi_master_Valid_Init_SPE() {
    if (SPCR & (1 << SPE)) return true;
    return false;
}


/**
 * @brief Verifica si el pin SS (Slave Select) está en LOW.
 *
 * El pin SS debe estar en LOW para que el esclavo SPI esté activo y responda.
 *
 * @return true Si SS está en LOW.
 * @return false Si SS está en HIGH.
 */
char spi_master_Valid_Init_SS() {
    if (digitalRead(53) == LOW) return true;
    return false;
}


/**
 * @brief Verifica si ocurrió una colisión de escritura (WCOL).
 *
 * WCOL se activa si se intenta escribir en SPDR mientras una transferencia
 * anterior aún no ha terminado. Esto indica un error grave en la secuencia SPI.
 *
 * @return true Si ocurrió colisión de escritura.
 * @return false Si no hubo colisión.
 */
char spi_master_Valid_Init_WCOL() {
    if (SPSR & (1 << WCOL)) return false;
    return true;
}


/**
 * @brief Limpia el flag SPIF leyendo SPSR y luego SPDR.
 *
 * Según el datasheet del ATmega2560, el flag SPIF solo se limpia si:
 *  1. Se lee SPSR.
 *  2. Se lee SPDR inmediatamente después.
 *
 * Esta función ejecuta ese procedimiento para dejar el estado SPI limpio.
 */
void clean_SPIF() {
    volatile uint8_t dummy;
    dummy = SPSR; /*< Lectura obligatoria de SPSR */
    dummy = SPDR; /*< Lectura obligatoria de SPDR para limpiar SPIF */
}


/**
 * @brief Imprime por consola el tipo de error SPI y su explicación.
 *
 * @param error_code Código de error SPI_ERROR_XXXX.
 */
void spi_master_print_error(uint16_t error_code) {


    Serial.println(F("========================================"));
    Serial.print  (F("[SPI ERROR] Código: 0x"));
    Serial.println(error_code, HEX);

    switch (error_code) {

        case SPI_ERROR_TIMEOUT:
            Serial.println(F("Tipo: TIMEOUT"));
            Serial.println(F("Descripción: El esclavo no respondió a tiempo."));
            Serial.println(F("Causa posible: El esclavo está bloqueado o no hay reloj SPI."));
            break;

        case SPI_ERROR_NO_INIT_SPE:
            Serial.println(F("Tipo: SPI NO INICIALIZADO"));
            Serial.println(F("Descripción: El bit SPE está en 0."));
            Serial.println(F("Causa posible: SPCR no fue configurado correctamente."));
            break;

        case SPI_ERROR_NO_INIT_SS:
            Serial.println(F("Tipo: SS NO ACTIVO"));
            Serial.println(F("Descripción: El pin SS no está en LOW durante la transferencia."));
            Serial.println(F("Causa posible: El esclavo no está seleccionado o hay ruido en la línea SS."));
            break;

        case SPI_ERROR_WCOL:
            Serial.println(F("Tipo: COLISIÓN DE ESCRITURA (WCOL)"));
            Serial.println(F("Descripción: Se escribió en SPDR antes de que terminara la transferencia anterior."));
            Serial.println(F("Causa posible: Código maestro demasiado rápido o interrupciones interfiriendo."));
            break;

        case SPI_ERROR_NO_REQUEST_SLAVE:
            Serial.println(F("Tipo: ESCLAVO NO RESPONDE"));
            Serial.println(F("Descripción: El esclavo devolvió 0x00 o 0xFF."));
            Serial.println(F("Causa posible: El esclavo no está conectado, no está inicializado o no tiene datos preparados."));
            break;

        default:
            Serial.println(F("Tipo: ERROR DESCONOCIDO"));
            Serial.println(F("Descripción: Código no reconocido."));
            break;
    }

    Serial.println(F("========================================"));
}


/**
 * @brief Muestra por consola el byte enviado y recibido durante la transferencia SPI.
 *
 * Esta función NO considera automáticamente 0x00 o 0xFF como errores,
 * porque pueden ser datos válidos. Sin embargo, muestra una advertencia
 * para ayudar a depurar si esos valores no eran esperados.
 *
 * @param enviado   Byte enviado por el maestro.
 * @param recibido  Byte recibido desde el esclavo.
 */
void spi_master_print_tx_rx(uint8_t enviado, uint8_t recibido) {

    Serial.print(F("[SPI] Enviado: 0x"));
    Serial.print(enviado, HEX);

    Serial.print(F(" | Recibido: 0x"));
    Serial.print(recibido, HEX);

    // --- Detección inteligente de valores sospechosos ---
    if (recibido == 0x00) {
        Serial.print(F("  (Aviso: 0x00 recibido. Puede ser dato válido o esclavo sin respuesta)"));
    }
    else if (recibido == 0xFF) {
        Serial.print(F("  (Aviso: 0xFF recibido. Puede indicar línea MISO flotante o sin esclavo)"));
    }

    Serial.println();
}