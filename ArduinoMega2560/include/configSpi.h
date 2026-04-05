/* ----------------------------------------------------
 * @section CONFIGURACION_SPI Configuración del sistema SPI
 * ----------------------------------------------------
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


