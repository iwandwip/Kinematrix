#include "Arduino.h"
#include "PCF8574Module.h"

PCF8574Module pcfModule(0x20);

#define LED1 PCF_PIN0
#define LED2 PCF_PIN1
#define LED3 PCF_PIN2
#define LED4 PCF_PIN3
#define LED5 PCF_PIN4
#define LED6 PCF_PIN5
#define LED7 PCF_PIN6
#define LED8 PCF_PIN7

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!pcfModule.begin()) {
    Serial.println("PCF8574 Error!");
    while(1);
  }
  
  pcfModule.setupLED(LED1);
  pcfModule.setupLED(LED2);
  pcfModule.setupLED(LED3);
  pcfModule.setupLED(LED4);
  pcfModule.setupLED(LED5);
  pcfModule.setupLED(LED6);
  pcfModule.setupLED(LED7);
  pcfModule.setupLED(LED8);
  
  pcfModule.digitalWriteAll(0x00);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    pcfModule.turnOnLED(i);
    delay(200);
    pcfModule.turnOffLED(i);
    delay(50);
  }
  
  delay(500);
  
  for (int i = 0; i < 7; i++) {
    pcfModule.digitalWriteAll(1 << i);
    delay(150);
  }
  
  for (int i = 6; i >= 0; i--) {
    pcfModule.digitalWriteAll(1 << i);
    delay(150);
  }
  
  delay(500);
  
  for (int i = 0; i < 3; i++) {
    pcfModule.digitalWriteAll(0xFF);
    delay(300);
    pcfModule.digitalWriteAll(0x00);
    delay(300);
  }
  
  delay(500);
  
  for (int i = 0; i < 5; i++) {
    pcfModule.digitalWriteAll(0b01010101);
    delay(300);
    pcfModule.digitalWriteAll(0b10101010);
    delay(300);
  }
  
  delay(1000);
}