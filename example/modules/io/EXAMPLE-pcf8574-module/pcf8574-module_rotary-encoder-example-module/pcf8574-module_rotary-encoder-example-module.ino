#include "Arduino.h"
#include "PCF8574Module.h"

#if defined(ESP8266)
  #define INTERRUPT_PIN D7
#elif defined(ESP32)
  #define INTERRUPT_PIN 2
#else
  #define INTERRUPT_PIN 2
#endif

volatile bool encoderChanged = false;

void IRAM_ATTR encoderInterrupt() {
  encoderChanged = true;
}

PCF8574Module pcfModule(0x20, INTERRUPT_PIN, encoderInterrupt);

#define ENCODER_PIN_A PCF_PIN0
#define ENCODER_PIN_B PCF_PIN1
#define ENCODER_BTN   PCF_PIN2
#define LED1          PCF_PIN3
#define LED2          PCF_PIN4
#define LED3          PCF_PIN5
#define LED4          PCF_PIN6
#define LED5          PCF_PIN7

volatile long encoderValue = 0;
long lastEncoderValue = 0;
int displayLevel = 0;
bool lastButtonState = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!pcfModule.begin()) {
    Serial.println("PCF8574 Error!");
    while(1);
  }
  
  pcfModule.setupEncoder(ENCODER_PIN_A, ENCODER_PIN_B);
  pcfModule.setupButton(ENCODER_BTN);
  
  pcfModule.setupLED(LED1);
  pcfModule.setupLED(LED2);
  pcfModule.setupLED(LED3);
  pcfModule.setupLED(LED4);
  pcfModule.setupLED(LED5);
  
  pcfModule.digitalWriteAll(0x00);
}

void loop() {
  if (encoderChanged) {
    encoderChanged = false;
    
    bool changed = pcfModule.readEncoder(ENCODER_PIN_A, ENCODER_PIN_B, &encoderValue);
    
    if (changed) {
      if (encoderValue < 0) encoderValue = 0;
      if (encoderValue > 5) encoderValue = 5;
      
      if (encoderValue != lastEncoderValue) {
        displayLevel = encoderValue;
        updateLEDBar(displayLevel);
        lastEncoderValue = encoderValue;
      }
    }
    
    bool buttonPressed = pcfModule.readButton(ENCODER_BTN);
    
    if (buttonPressed && !lastButtonState) {
      encoderValue = 0;
      displayLevel = 0;
      updateLEDBar(displayLevel);
    }
    
    lastButtonState = buttonPressed;
  }
  
  delay(10);
}

void updateLEDBar(int level) {
  pcfModule.turnOffLED(LED1);
  pcfModule.turnOffLED(LED2);
  pcfModule.turnOffLED(LED3);
  pcfModule.turnOffLED(LED4);
  pcfModule.turnOffLED(LED5);
  
  switch (level) {
    case 5:
      pcfModule.turnOnLED(LED5);
    case 4:
      pcfModule.turnOnLED(LED4);
    case 3:
      pcfModule.turnOnLED(LED3);
    case 2:
      pcfModule.turnOnLED(LED2);
    case 1:
      pcfModule.turnOnLED(LED1);
      break;
    case 0:
    default:
      break;
  }
}