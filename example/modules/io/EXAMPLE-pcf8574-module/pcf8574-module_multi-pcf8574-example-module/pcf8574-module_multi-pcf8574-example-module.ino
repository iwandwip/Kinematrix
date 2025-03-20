#include "Arduino.h"
#include "PCF8574Module.h"

#if defined(ESP8266)
  #define INTERRUPT_PIN1 D7
  #define INTERRUPT_PIN2 D6
#elif defined(ESP32)
  #define INTERRUPT_PIN1 2
  #define INTERRUPT_PIN2 4
#else
  #define INTERRUPT_PIN1 2
  #define INTERRUPT_PIN2 3
#endif

volatile bool buttonChanged = false;

void IRAM_ATTR buttonInterrupt() {
  buttonChanged = true;
}

#define PCF_ADDR_1 0x20
#define PCF_ADDR_2 0x21

PCF8574Module pcfLed(PCF_ADDR_1);
PCF8574Module pcfButton(PCF_ADDR_2, INTERRUPT_PIN2, buttonInterrupt);

byte ledPattern = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!pcfLed.begin()) {
    Serial.println("PCF8574 #1 Error!");
    while(1);
  }
  
  if (!pcfButton.begin()) {
    Serial.println("PCF8574 #2 Error!");
    while(1);
  }
  
  for (int i = 0; i < 8; i++) {
    pcfLed.setupLED(i);
  }
  
  for (int i = 0; i < 8; i++) {
    pcfButton.setupButton(i);
  }
  
  pcfLed.digitalWriteAll(0x00);
}

void loop() {
  if (buttonChanged) {
    buttonChanged = false;
    
    byte buttonStatus = pcfButton.digitalReadAll();
    
    for (int i = 0; i < 8; i++) {
      if (pcfButton.readButton(i)) {
        switch (i) {
          case 0:
            runningLightPattern();
            break;
          case 1:
            chaserPattern();
            break;
          case 2:
            alternatePattern();
            break;
          case 3:
            toggleAllLEDs();
            break;
          case 4:
            binaryCounterPattern();
            break;
          case 5:
            pcfLed.toggleLED(5);
            break;
          case 6:
            pcfLed.toggleLED(6);
            break;
          case 7:
            pcfLed.toggleLED(7);
            break;
        }
      }
    }
    
    delay(50);
  }
}

void runningLightPattern() {
  byte prevStatus = pcfLed.digitalReadAll();
  
  for (int i = 0; i < 3; i++) {
    for (int pos = 0; pos < 8; pos++) {
      byte pattern = (1 << pos);
      pcfLed.digitalWriteAll(pattern);
      delay(100);
    }
  }
  
  pcfLed.digitalWriteAll(prevStatus);
}

void chaserPattern() {
  byte prevStatus = pcfLed.digitalReadAll();
  
  for (int i = 0; i < 3; i++) {
    for (int pos = 0; pos < 7; pos++) {
      byte pattern = (1 << pos);
      pcfLed.digitalWriteAll(pattern);
      delay(100);
    }
    
    for (int pos = 6; pos > 0; pos--) {
      byte pattern = (1 << pos);
      pcfLed.digitalWriteAll(pattern);
      delay(100);
    }
  }
  
  pcfLed.digitalWriteAll(prevStatus);
}

void alternatePattern() {
  byte prevStatus = pcfLed.digitalReadAll();
  
  for (int i = 0; i < 5; i++) {
    pcfLed.digitalWriteAll(0b01010101);
    delay(300);
    
    pcfLed.digitalWriteAll(0b10101010);
    delay(300);
  }
  
  pcfLed.digitalWriteAll(prevStatus);
}

void toggleAllLEDs() {
  byte currentStatus = pcfLed.digitalReadAll();
  
  if (currentStatus == 0) {
    pcfLed.digitalWriteAll(0xFF);
  } else {
    pcfLed.digitalWriteAll(0x00);
  }
}

void binaryCounterPattern() {
  byte prevStatus = pcfLed.digitalReadAll();
  
  for (int i = 0; i < 16; i++) {
    pcfLed.digitalWriteAll(i);
    delay(300);
  }
  
  pcfLed.digitalWriteAll(prevStatus);
}