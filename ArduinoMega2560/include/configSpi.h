/**
 * @file spi_config.h
 * @brief Definiciones, constantes y parámetros de configuración del sistema SPI.
 *
 * Este archivo contiene la configuración principal del módulo SPI utilizada
 * tanto por el maestro como por el esclavo. Incluye:
 *
 *  - Selección del modo de operación (MASTER, SLAVE, SLAVE_PRUEBA)
 *  - Definición de los pines hardware del bus SPI (SS, MOSI, MISO, SCK)
 *  - Códigos de error asociados a fallos en la comunicación SPI
 *
 * Su propósito es centralizar todos los parámetros críticos del sistema SPI
 * para garantizar coherencia entre módulos, facilitar la depuración y permitir
 * una configuración flexible del entorno de comunicación.
 *
 * @section CONFIGURACION_SPI Selección del modo SPI
 * Se definen macros para compilar el firmware en modo maestro, esclavo o
 * esclavo de prueba. Solo uno de ellos debe estar activo simultáneamente.
 *
 * @section PINES_SPI Pines hardware del bus SPI
 * Se especifican los pines utilizados por el microcontrolador para la interfaz
 * SPI estándar (SS, MOSI, MISO, SCK). Estos valores deben corresponder a los
 * pines físicos del hardware utilizado.
 *
 * @section CODIGOS_ERROR_SPI Códigos de error del sistema SPI
 * Se definen códigos de error de 8 bits que permiten identificar fallos
 * específicos durante la transferencia SPI, tales como timeouts, colisiones
 * de escritura, falta de inicialización o estados incorrectos del pin SS.
 *
 * @note Este archivo debe incluirse en todos los módulos que utilicen SPI
 *       para asegurar una configuración consistente del sistema.
 */

 
/**
 * @def MASTER
 * @brief Activa el modo maestro SPI.
 *
 * Colocar en 1 para compilar en modo maestro.
 */
#define MASTER 0
/**
 * @def SLAVE
 * @brief Activa el modo esclavo SPI.
 *
 * Colocar en 1 para compilar en modo esclavo.
 */
#define SLAVE 1

/**
 * @def SLAVE_PRUEBA
 * @brief Activa un modo esclavo especial para pruebas.
 */
#define SLAVE_PRUEBA 0

/* ----------------------------------------------------
 * @section PINES_SPI Pines hardware del bus SPI
 * ----------------------------------------------------
 */

/**
 * @brief Pin SS (Slave Select) usado por el maestro para seleccionar al esclavo.
 */
const uint8_t PIN_SS   = 53;

/**
 * @brief Pin MOSI (Master Out Slave In).  
 * Línea de datos desde el maestro hacia el esclavo.
 */
const uint8_t PIN_MOSI = 51;

/**
 * @brief Pin MISO (Master In Slave Out).  
 * Línea de datos desde el esclavo hacia el maestro.
 */
const uint8_t PIN_MISO = 50;

/**
 * @brief Pin SCK (Serial Clock).  
 * Reloj generado por el maestro.
 */
const uint8_t PIN_SCK  = 52;

/* ----------------------------------------------------
 * @section CODIGOS_ERROR_SPI Códigos de error del sistema SPI
 * ----------------------------------------------------
 */

/**
 * @def SPI_ERROR_TIMEOUT
 * @brief Error: la transferencia SPI excedió el tiempo máximo permitido.
 */
#define SPI_ERROR_TIMEOUT           0xFF

/**
 * @def SPI_ERROR_NO_INIT_SPE
 * @brief Error: el módulo SPI no está habilitado (bit SPE = 0).
 */
#define SPI_ERROR_NO_INIT_SPE       0xFE

/**
 * @def SPI_ERROR_NO_INIT_SS
 * @brief Error: el pin SS no estaba en LOW cuando se esperaba.
 */
#define SPI_ERROR_NO_INIT_SS        0xFD

/**
 * @def SPI_ERROR_WCOL
 * @brief Error: colisión de escritura (bit WCOL = 1).
 */
#define SPI_ERROR_WCOL              0xFC

/**
 * @def SPI_ERROR_NO_REQUEST_SLAVE
 * @brief Error: el esclavo no responde (recibido 0x00 o 0xFF).
 */
#define SPI_ERROR_NO_REQUEST_SLAVE  0xFB

/**
 * @def SPI_ERROR_PIN_SS_HIGH
 * @brief Error: el pin SS se encuentra en HIGH durante la transferencia.
 */
#define SPI_ERROR_PIN_SS_HIGH       0xFA


