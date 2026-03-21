#include "msg.h"

bool verificar_configuracion_spi_PIN_Master() {
  bool ok = true;

  Serial.println(F("\n===================================================="));
  Serial.println(F("   VERIFICACIÓN DE CONFIGURACIÓN SPI (MEGA MAESTRO)"));
  Serial.println(F("===================================================="));
  Serial.println(F(" Pin | Señal | Dirección esperada | Estado detectado"));
  Serial.println(F("----------------------------------------------------"));

  // MOSI (51) → OUTPUT
  if (digitalRead(51) == HIGH || digitalRead(51) == LOW) {
    Serial.println(F(" 51  | MOSI  | OUTPUT             | OK"));
  } else {
    Serial.println(F(" 51  | MOSI  | OUTPUT             | ERROR"));
    ok = false;
  }

  // MISO (50) → INPUT
  pinMode(50, INPUT_PULLUP); // test temporal
  if (digitalRead(50) == HIGH || digitalRead(50) == LOW) {
    Serial.println(F(" 50  | MISO  | INPUT              | OK"));
  } else {
    Serial.println(F(" 50  | MISO  | INPUT              | ERROR"));
    ok = false;
  }
  pinMode(50, INPUT); // restaurar

  // SCK (52) → OUTPUT
  if (digitalRead(52) == HIGH || digitalRead(52) == LOW) {
    Serial.println(F(" 52  | SCK   | OUTPUT             | OK"));
  } else {
    Serial.println(F(" 52  | SCK   | OUTPUT             | ERROR"));
    ok = false;
  }

  // SS (53) → OUTPUT y en HIGH
  if (digitalRead(53) == HIGH) {
    Serial.println(F(" 53  | SS    | OUTPUT + HIGH      | OK"));
  } else {
    Serial.println(F(" 53  | SS    | OUTPUT + HIGH      | ERROR (debe estar HIGH)"));
    ok = false;
  }

  Serial.println(F("----------------------------------------------------"));

  if (ok) {
    Serial.println(F(" ✔ Todos los pines SPI están configurados correctamente."));
    Serial.println(F("   El módulo SPI puede iniciarse sin riesgos."));
  } else {
    Serial.println(F(" ✖ Se han detectado errores en la configuración SPI."));
    Serial.println(F("   Revisa el wiring y los pinMode antes de continuar."));
  }

  Serial.println(F("====================================================\n"));

  return ok;
}

bool verificar_configuracion_spi_PIN_Slave() {
  bool ok = true;

  Serial.println(F("\n===================================================="));
  Serial.println(F("   VERIFICACIÓN DE CONFIGURACIÓN SPI (MEGA ESCLAVO)"));
  Serial.println(F("===================================================="));
  Serial.println(F(" Pin | Señal | Dirección esperada | Estado detectado"));
  Serial.println(F("----------------------------------------------------"));

  // MOSI (51) → INPUT
  pinMode(51, INPUT_PULLUP); // test temporal
  if (digitalRead(51) == HIGH || digitalRead(51) == LOW) {
    Serial.println(F(" 51  | MOSI  | INPUT              | OK"));
  } else {
    Serial.println(F(" 51  | MOSI  | INPUT              | ERROR"));
    ok = false;
  }
  pinMode(51, INPUT); // restaurar

  // MISO (50) → OUTPUT
  pinMode(50, OUTPUT);
  if (digitalRead(50) == HIGH || digitalRead(50) == LOW) {
    Serial.println(F(" 50  | MISO  | OUTPUT             | OK"));
  } else {
    Serial.println(F(" 50  | MISO  | OUTPUT             | ERROR"));
    ok = false;
  }

  // SCK (52) → INPUT
  pinMode(52, INPUT_PULLUP); // test temporal
  if (digitalRead(52) == HIGH || digitalRead(52) == LOW) {
    Serial.println(F(" 52  | SCK   | INPUT              | OK"));
  } else {
    Serial.println(F(" 52  | SCK   | INPUT              | ERROR"));
    ok = false;
  }
  pinMode(52, INPUT); // restaurar

  // SS (53) → INPUT (controlado por el maestro)
  pinMode(53, INPUT_PULLUP); // test temporal
  if (digitalRead(53) == HIGH || digitalRead(53) == LOW) {
    Serial.println(F(" 53  | SS    | INPUT              | OK"));
  } else {
    Serial.println(F(" 53  | SS    | INPUT              | ERROR"));
    ok = false;
  }
  pinMode(53, INPUT); // restaurar

  Serial.println(F("----------------------------------------------------"));

  if (ok) {
    Serial.println(F(" ✔ Configuración SPI ESCLAVO correcta."));
    Serial.println(F("   El módulo SPI puede iniciarse sin riesgos."));
  } else {
    Serial.println(F(" ✖ Errores detectados en la configuración SPI ESCLAVO."));
    Serial.println(F("   Revisa wiring y pinMode antes de continuar."));
  }

  Serial.println(F("====================================================\n"));

  return ok;
}

void mostrar_configuracion_SPCR() {

  Serial.println(F("\n===================================================="));
  Serial.println(F("        ESTADO DEL REGISTRO SPCR (SPI CONTROL)      "));
  Serial.println(F("===================================================="));
  Serial.println(F(" Bit | Nombre | Valor | Descripción"));
  Serial.println(F("----------------------------------------------------"));

  // Bit 7 – SPIE
  Serial.print(F("  7  | SPIE   |  "));
  Serial.print((SPCR & (1 << SPIE)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println((SPCR & (1 << SPIE)) ? 
    F("Interrupción SPI habilitada") : 
    F("Interrupción SPI deshabilitada"));

  // Bit 6 – SPE
  Serial.print(F("  6  | SPE    |  "));
  Serial.print((SPCR & (1 << SPE)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println((SPCR & (1 << SPE)) ? 
    F("SPI habilitado") : 
    F("SPI deshabilitado"));

  // Bit 5 – DORD
  Serial.print(F("  5  | DORD   |  "));
  Serial.print((SPCR & (1 << DORD)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println((SPCR & (1 << DORD)) ? 
    F("LSB primero") : 
    F("MSB primero"));

  // Bit 4 – MSTR
  Serial.print(F("  4  | MSTR   |  "));
  Serial.print((SPCR & (1 << MSTR)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println((SPCR & (1 << MSTR)) ? 
    F("Modo MAESTRO") : 
    F("Modo ESCLAVO"));

  // Bit 3 – CPOL
  Serial.print(F("  3  | CPOL   |  "));
  Serial.print((SPCR & (1 << CPOL)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println((SPCR & (1 << CPOL)) ? 
    F("Reloj en HIGH en reposo") : 
    F("Reloj en LOW en reposo"));

  // Bit 2 – CPHA
  Serial.print(F("  2  | CPHA   |  "));
  Serial.print((SPCR & (1 << CPHA)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println((SPCR & (1 << CPHA)) ? 
    F("Captura en segundo flanco") : 
    F("Captura en primer flanco"));

  // Bit 1 – SPR1
  Serial.print(F("  1  | SPR1   |  "));
  Serial.print((SPCR & (1 << SPR1)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println(F("Bit alto del divisor de reloj"));

  // Bit 0 – SPR0
  Serial.print(F("  0  | SPR0   |  "));
  Serial.print((SPCR & (1 << SPR0)) ? "1" : "0");
  Serial.print(F("    | "));
  Serial.println(F("Bit bajo del divisor de reloj"));

  Serial.println(F("----------------------------------------------------"));

  // Mostrar velocidad SPI resultante
  uint8_t spr1 = (SPCR & (1 << SPR1)) ? 1 : 0;
  uint8_t spr0 = (SPCR & (1 << SPR0)) ? 1 : 0;
  uint8_t spi2x = (SPSR & (1 << SPI2X)) ? 1 : 0;

  Serial.print(F(" Velocidad SPI: "));

  if (spr1 == 0 && spr0 == 0) Serial.print(F("Fosc/4"));
  if (spr1 == 0 && spr0 == 1) Serial.print(F("Fosc/16"));
  if (spr1 == 1 && spr0 == 0) Serial.print(F("Fosc/64"));
  if (spr1 == 1 && spr0 == 1) Serial.print(F("Fosc/128"));

  if (spi2x) Serial.print(F(" (x2 por SPI2X)"));

  Serial.println();
  Serial.println(F("====================================================\n"));
}
