#include "Arduino.h"
#include "PCF8574Module.h"

#if defined(ESP8266)
  #define INTERRUPT_PIN D7
#elif defined(ESP32)
  #define INTERRUPT_PIN 2
#else
  #define INTERRUPT_PIN 2
#endif

volatile bool buttonChanged = false;

void IRAM_ATTR buttonInterrupt() {
  buttonChanged = true;
}

PCF8574Module pcfModule(0x20, INTERRUPT_PIN, buttonInterrupt);

#define BUTTON1 PCF_PIN0
#define BUTTON2 PCF_PIN1
#define BUTTON3 PCF_PIN2
#define BUTTON4 PCF_PIN3
#define LED1    PCF_PIN4
#define LED2    PCF_PIN5
#define LED3    PCF_PIN6
#define LED4    PCF_PIN7

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!pcfModule.begin()) {
    Serial.println("PCF8574 Error!");
    while(1);
  }
  
  pcfModule.setupButton(BUTTON1);
  pcfModule.setupButton(BUTTON2);
  pcfModule.setupButton(BUTTON3);
  pcfModule.setupButton(BUTTON4);
  
  pcfModule.setupLED(LED1);
  pcfModule.setupLED(LED2);
  pcfModule.setupLED(LED3);
  pcfModule.setupLED(LED4);
  
  pcfModule.turnOffLED(LED1);
  pcfModule.turnOffLED(LED2);
  pcfModule.turnOffLED(LED3);
  pcfModule.turnOffLED(LED4);
}

void loop() {
  if (buttonChanged) {
    buttonChanged = false;
    
    if (pcfModule.readButton(BUTTON1)) {
      pcfModule.toggleLED(LED1);
    }
    
    if (pcfModule.readButton(BUTTON2)) {
      pcfModule.toggleLED(LED2);
    }
    
    if (pcfModule.readButton(BUTTON3)) {
      pcfModule.toggleLED(LED3);
    }
    
    if (pcfModule.readButton(BUTTON4)) {
      pcfModule.toggleLED(LED4);
    }
    
    delay(50);
  }
}