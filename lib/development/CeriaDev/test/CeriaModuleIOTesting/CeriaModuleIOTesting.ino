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
  } else {
    Serial.println("CeriaModuleIO initialization failed");
  }
}

void loop() {
  io.updateAll();

  for (int i = 0; i < 8; i++) {
    if (io.buttonPressed(i)) {
      Serial.println("Button " + String(i) + ", LED toggled");
    }
  }
}
