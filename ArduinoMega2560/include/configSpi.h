/* ------------------ CONFIGURACIÓN ------------------ */
// Selecciona modo: pon 1 en el que quieras compilar
#define MASTER 1
#define SLAVE  0

/* Pines hardware SPI (Arduino Mega) */
const uint8_t PIN_SS   = 53;  ///< Pin SS usado por el maestro para seleccionar esclavo
const uint8_t PIN_MOSI = 51;  ///< Pin MOSI (salida del maestro)
const uint8_t PIN_MISO = 50;  ///< Pin MISO (entrada del maestro)
const uint8_t PIN_SCK  = 52;  ///< Pin SCK (reloj generado por el maestro)

/* Códigos de error (uint16_t) */
#define SPI_ERROR_TIMEOUT           0xFFFE  ///< Timeout en la transferencia
#define SPI_ERROR_NO_INIT_SPE       0xFFFD  ///< SPI no habilitado (SPE=0)
#define SPI_ERROR_NO_INIT_SS        0xFFFC  ///< Pin SS no en LOW cuando se esperaba
#define SPI_ERROR_WCOL              0xFFFB  ///< Colisión de escritura (WCOL)
#define SPI_ERROR_NO_REQUEST_SLAVE  0xFFFA  ///< El esclavo no responde (0x00/0xFF)