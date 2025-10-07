#define ENABLE_DEVELOPMENT_CERIA_DEV_OP
#include "Kinematrix.h"

using namespace CeriaDevOP;

CeriaModuleIO io(0x20);

void setup() {
  Serial.begin(115200);

  for (uint8_t i = 0; i < 8; i++) {
    io.setupButton(i, 50, IO_INPUT);
  }

  if (io.begin()) {
    Serial.println("CeriaModuleIO initialized successfully");

    Serial.println("Testing direct PCF8574 read:");
    for (int i = 0; i < 8; i++) {
      uint8_t state = io.digitalRead(i);
      Serial.println("P" + String(i) + ": " + String(state));
    }

    if (io.isConnected()) {
      Serial.println("PCF8574 connection: OK");
    } else {
      Serial.println("PCF8574 connection: FAILED");
    }

  } else {
    Serial.println("CeriaModuleIO initialization failed");
  }
}

void loop() {
  io.updateAll();

  if (io.hasNewData()) {
    for (int i = 0; i < 8; i++) {
      uint8_t directRead = io.digitalRead(i);
      bool buttonPressed = io.buttonPressed(i);

      if (buttonPressed) {
        Serial.println("Button " + String(i) + " pressed (wrapper detected)");
      }
    }
  }

  delay(50);
}