#define ENABLE_DEVELOPMENT_CERIA_DEV_OP
#include "Kinematrix.h"

using namespace CeriaDevOP;

CeriaModuleIO io(0x20);

void setup() {
  Serial.begin(115200);
  Serial.println("=== CeriaModuleIO Smart Button Demo ===");

  for (uint8_t i = 0; i < 8; i++) {
    io.setupButton(i, 50, IO_INPUT);
  }

  if (io.begin()) {
    Serial.println("✅ CeriaModuleIO initialized successfully");

    // Test basic connection first
    Serial.println("Testing basic connection:");
    for (int i = 0; i < 8; i++) {
      uint8_t state = io.digitalRead(i);
      Serial.println("P" + String(i) + ": " + String(state));
    }

    if (io.isConnected()) {
      Serial.println("PCF8574 connection: OK");
    } else {
      Serial.println("PCF8574 connection: FAILED");
      while (1) {
      }
    }

    ButtonTimings fastTiming;
    fastTiming.debounceTime = 30;
    fastTiming.doubleClickWindow = 250;
    fastTiming.tripleClickWindow = 350;
    fastTiming.longPressThreshold = 800;
    fastTiming.veryLongPressThreshold = 1500;
    fastTiming.extremelyLongPressThreshold = 3000;
    fastTiming.rapidFireInterval = 80;

    io.setButtonTimings(0, fastTiming);
    Serial.println("Smart button system ready!");
  } else {
    Serial.println("❌ CeriaModuleIO initialization failed");
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

      // Now test smart button features
      // handleSmartButton(i);
    }
  }

  delay(50);
}

void handleSmartButton(int pin) {
  String pinName = "P" + String(pin);

  if (io.buttonSingleClick(pin)) {
    Serial.println("🖱️  " + pinName + " → SINGLE CLICK");
  }

  if (io.buttonDoubleClick(pin)) {
    Serial.println("🖱️🖱️  " + pinName + " → DOUBLE CLICK");
  }

  if (io.buttonTripleClick(pin)) {
    Serial.println("🖱️🖱️🖱️ " + pinName + " → TRIPLE CLICK");
  }

  if (io.buttonLongPress(pin)) {
    Serial.println("⏳ " + pinName + " → LONG PRESS (1s)");
  }

  if (io.buttonVeryLongPress(pin)) {
    Serial.println("⏳⏳ " + pinName + " → VERY LONG PRESS (2s)");
  }

  if (io.buttonExtremelyLongPress(pin)) {
    Serial.println("⏳⏳⏳ " + pinName + " → EXTREMELY LONG PRESS (5s)");
  }

  if (io.buttonClickAndHold(pin)) {
    Serial.println("🔒 " + pinName + " → CLICK AND HOLD");
  }

  if (io.buttonRapidFire(pin)) {
    Serial.print("🔥 " + pinName + " → RAPID FIRE ");
    static int rapidCount = 0;
    rapidCount++;
    Serial.println("#" + String(rapidCount));

    if (rapidCount % 10 == 0) {
      Serial.println("💥 Rapid fire burst: " + String(rapidCount) + " shots!");
    }
  }

  if (io.buttonPressed(pin)) {
    static unsigned long pressCount = 0;
    pressCount++;

    if (pressCount % 50 == 0) {
      Serial.println("📈 Total button presses: " + String(pressCount));
    }
  }

  if (io.buttonReleased(pin)) {
  }
}