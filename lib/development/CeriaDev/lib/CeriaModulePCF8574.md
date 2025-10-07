# CeriaModulePCF8574

## Overview
CeriaModulePCF8574 adalah wrapper sederhana dan powerful untuk PCF8574 library yang menyediakan interface yang clean untuk I2C GPIO expansion. Dirancang untuk kemudahan penggunaan dengan tetap mempertahankan fleksibilitas dan kontrol penuh dari base library.

## Features
- **Simple API**: Methods yang intuitif untuk GPIO operations
- **Full Platform Support**: ESP32, ESP8266, AVR, SAMD, RP2040, STM32
- **Multiple Constructors**: Flexible initialization untuk berbagai platform
- **Interrupt Support**: Hardware interrupt dengan callback management
- **Encoder Support**: Built-in rotary encoder dengan multiple algorithms
- **State Management**: Intelligent pin state tracking dan change detection
- **Bulk Operations**: Efficient multi-pin read/write operations
- **String Output**: Human-readable status dan diagnostic strings

## Hardware Specifications
- **I2C Interface**: Standard I2C/TWI communication
- **Operating Voltage**: 2.5V - 6V
- **Current per Pin**: 25mA maximum
- **I2C Address Range**: 0x20-0x27 (PCF8574), 0x38-0x3F (PCF8574A)
- **Pin Count**: 8 bidirectional GPIO pins (P0-P7)
- **Interrupt**: Single interrupt output untuk pin change notifications

## Pin Modes
```cpp
enum PCF8574PinMode {
    PCF8574_PIN_INPUT = INPUT,
    PCF8574_PIN_INPUT_PULLUP = INPUT_PULLUP,
    PCF8574_PIN_OUTPUT = OUTPUT
};
```

## Basic Usage

### Initialization
```cpp
#include "CeriaModulePCF8574.h"
using namespace CeriaDevOP;

CeriaModulePCF8574 pcf(0x38);  // Default I2C address

void setup() {
    Serial.begin(115200);
    
    if (pcf.begin()) {
        Serial.println("PCF8574 initialized successfully");
        Serial.println(pcf.getModuleStatusString());
    } else {
        Serial.println("PCF8574 initialization failed");
    }
}
```

### Basic GPIO Operations
```cpp
void setup() {
    Serial.begin(115200);
    pcf.begin();
    
    // Setup pins
    pcf.setPinMode(P0, PCF8574_PIN_OUTPUT);     // LED output
    pcf.setPinMode(P1, PCF8574_PIN_INPUT_PULLUP); // Button input
    pcf.setPinMode(P2, PCF8574_PIN_INPUT);      // Sensor input
}

void loop() {
    pcf.update();
    
    // Read button state
    uint8_t button = pcf.readPin(P1);
    if (button == LOW) {
        pcf.writePin(P0, HIGH);  // Turn on LED
        Serial.println("Button pressed, LED ON");
    } else {
        pcf.writePin(P0, LOW);   // Turn off LED
    }
    
    delay(50);
}
```

## API Reference

### Constructor Options

#### Basic Constructor
```cpp
CeriaModulePCF8574(uint8_t address = 0x38);     // Simple I2C address
```

#### With Interrupt Support
```cpp
CeriaModulePCF8574(uint8_t address, uint8_t interruptPin, void (*interruptFunction)());
```

#### Platform-Specific Constructors
```cpp
// ESP32, ESP8266, etc. - Custom I2C pins
CeriaModulePCF8574(uint8_t address, int sda, int scl);

// Advanced platforms - Custom I2C bus
CeriaModulePCF8574(TwoWire *pWire, uint8_t address);

// ESP32 - Full custom configuration
CeriaModulePCF8574(TwoWire *pWire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)());
```

### Core Methods
```cpp
bool begin();                                    // Initialize PCF8574
bool begin(uint8_t address);                     // Initialize with new address
void update();                                   // Update pin states
bool hasNewData();                               // Check for state changes
bool isConnected(uint32_t timeoutMs = 5000);    // Check I2C connection
```

### GPIO Methods
```cpp
void setPinMode(uint8_t pin, PCF8574PinMode mode, uint8_t output_start = HIGH);
uint8_t readPin(uint8_t pin);                    // Read single pin
bool writePin(uint8_t pin, uint8_t value);       // Write single pin
uint8_t readAllPins();                           // Read all pins as byte
bool writeAllPins(uint8_t pinValues);            // Write all pins as byte
```

### Encoder Methods
```cpp
void setupEncoder(uint8_t pinA, uint8_t pinB);   // Setup rotary encoder
bool readEncoder(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);
int8_t readEncoderStep(uint8_t pinA, uint8_t pinB); // Read single step
```

### Interrupt Methods
```cpp
void enableInterrupt();                          // Enable hardware interrupt
void disableInterrupt();                         // Disable hardware interrupt
```

### Configuration Methods
```cpp
void setLatency(int latencyMs);                  // Set I2C read latency
int getLatency();                                // Get current latency
```

### Status Methods
```cpp
String getPinStatusString();                     // Get pin states as string
String getModuleStatusString();                  // Get complete module status
bool waitForChange(uint32_t timeoutMs = 5000);   // Wait for pin state change
```

## Usage Examples

### LED Control Panel
```cpp
void setup() {
    Serial.begin(115200);
    pcf.begin();
    
    // Setup 4 LEDs as outputs
    for (int i = 0; i < 4; i++) {
        pcf.setPinMode(i, PCF8574_PIN_OUTPUT);
        pcf.writePin(i, LOW);  // Start with LEDs off
    }
}

void loop() {
    // Sequential LED pattern
    for (int i = 0; i < 4; i++) {
        pcf.writePin(i, HIGH);
        delay(200);
        pcf.writePin(i, LOW);
        delay(200);
    }
    
    // All LEDs blink
    pcf.writeAllPins(0x0F);  // Turn on P0-P3
    delay(500);
    pcf.writeAllPins(0x00);  // Turn off all
    delay(500);
}
```

### Button Matrix Input
```cpp
void setup() {
    Serial.begin(115200);
    pcf.begin();
    
    // Setup pins P0-P3 as inputs with pullup
    for (int i = 0; i < 4; i++) {
        pcf.setPinMode(i, PCF8574_PIN_INPUT_PULLUP);
    }
    
    Serial.println("Button matrix ready");
}

void loop() {
    pcf.update();
    
    if (pcf.hasNewData()) {
        uint8_t buttons = pcf.readAllPins();
        
        Serial.print("Button states: ");
        for (int i = 0; i < 4; i++) {
            bool pressed = ((buttons & (1 << i)) == 0);  // Active low
            Serial.print("P" + String(i) + ":" + (pressed ? "PRESSED" : "released") + " ");
        }
        Serial.println();
        
        // Individual button handling
        for (int i = 0; i < 4; i++) {
            if (pcf.readPin(i) == LOW) {
                Serial.println("Button " + String(i) + " action!");
                // Handle button press
            }
        }
    }
    
    delay(50);
}
```

### Rotary Encoder with Menu
```cpp
volatile long encoderPosition = 0;
int currentMenu = 0;
String menuItems[] = {"Option 1", "Option 2", "Option 3", "Option 4", "Exit"};

void encoderISR() {
    // Interrupt handler - keep minimal
}

void setup() {
    Serial.begin(115200);
    
    // Initialize with interrupt support
    CeriaModulePCF8574 pcf(0x38, 2, encoderISR);  // Interrupt on pin 2
    pcf.begin();
    
    // Setup encoder on P0, P1 and button on P2
    pcf.setupEncoder(P0, P1);
    pcf.setPinMode(P2, PCF8574_PIN_INPUT_PULLUP);
    
    pcf.setLatency(0);  // Low latency for encoder
    
    Serial.println("Rotary Encoder Menu System");
    displayMenu();
}

void loop() {
    pcf.update();
    
    // Read encoder changes
    if (pcf.readEncoder(P0, P1, &encoderPosition)) {
        currentMenu = constrain(encoderPosition, 0, 4);
        displayMenu();
    }
    
    // Check button press
    if (pcf.readPin(P2) == LOW) {
        delay(50);  // Debounce
        if (pcf.readPin(P2) == LOW) {
            Serial.println("Selected: " + menuItems[currentMenu]);
            
            if (currentMenu == 4) {  // Exit option
                Serial.println("Exiting menu...");
                delay(1000);
            }
            
            // Wait for button release
            while (pcf.readPin(P2) == LOW) {
                pcf.update();
                delay(10);
            }
        }
    }
    
    delay(10);
}

void displayMenu() {
    Serial.println("\n=== Menu System ===");
    for (int i = 0; i < 5; i++) {
        if (i == currentMenu) {
            Serial.println("> " + menuItems[i] + " <");
        } else {
            Serial.println("  " + menuItems[i]);
        }
    }
    Serial.println("Encoder position: " + String(encoderPosition));
}
```

### I/O Expander for Arduino Uno
```cpp
// Expand Arduino Uno from 14 to 22 digital I/O pins

void setup() {
    Serial.begin(115200);
    pcf.begin();
    
    // Setup expanded I/O
    pcf.setPinMode(P0, PCF8574_PIN_OUTPUT);     // Extended LED
    pcf.setPinMode(P1, PCF8574_PIN_OUTPUT);     // Extended relay
    pcf.setPinMode(P2, PCF8574_PIN_INPUT_PULLUP); // Extended button
    pcf.setPinMode(P3, PCF8574_PIN_INPUT);      // Extended sensor
    
    Serial.println("Arduino Uno I/O expansion ready");
    Serial.println("Original pins: 14, Extended pins: +8 = 22 total");
}

void loop() {
    // Use native Arduino pins
    digitalWrite(13, HIGH);  // Built-in LED
    
    // Use expanded PCF8574 pins
    pcf.writePin(P0, HIGH);  // Extended LED
    
    // Read both native and expanded inputs
    bool nativeButton = (digitalRead(2) == LOW);
    bool expandedButton = (pcf.readPin(P2) == LOW);
    
    if (nativeButton || expandedButton) {
        Serial.println("Button pressed on " + 
                      String(nativeButton ? "native" : "expanded") + " pins");
        
        // Control relay via expanded pins
        pcf.writePin(P1, HIGH);
        delay(1000);
        pcf.writePin(P1, LOW);
    }
    
    delay(100);
}
```

### Multi-PCF8574 System
```cpp
CeriaModulePCF8574 pcf1(0x38);  // First PCF8574
CeriaModulePCF8574 pcf2(0x39);  // Second PCF8574

void setup() {
    Serial.begin(115200);
    
    // Initialize both modules
    if (pcf1.begin() && pcf2.begin()) {
        Serial.println("Multi-PCF8574 system ready");
        Serial.println("Total I/O pins: 16");
    }
    
    // Setup PCF1 for outputs (LEDs)
    for (int i = 0; i < 8; i++) {
        pcf1.setPinMode(i, PCF8574_PIN_OUTPUT);
        pcf1.writePin(i, LOW);
    }
    
    // Setup PCF2 for inputs (buttons)
    for (int i = 0; i < 8; i++) {
        pcf2.setPinMode(i, PCF8574_PIN_INPUT_PULLUP);
    }
}

void loop() {
    pcf1.update();
    pcf2.update();
    
    // Read all buttons from PCF2
    uint8_t buttons = pcf2.readAllPins();
    
    // Mirror button states to LEDs on PCF1
    pcf1.writeAllPins(~buttons);  // Invert for active-low buttons
    
    // Status reporting
    if (pcf1.hasNewData() || pcf2.hasNewData()) {
        Serial.println("PCF1 (LEDs): " + pcf1.getPinStatusString());
        Serial.println("PCF2 (Buttons): " + pcf2.getPinStatusString());
    }
    
    delay(50);
}
```

## Hardware Setup

### Basic PCF8574 Connection
```
PCF8574      →    ESP32/Arduino
VCC          →    3.3V or 5V
GND          →    GND
SDA          →    SDA (I2C Data)
SCL          →    SCL (I2C Clock)
INT          →    GPIO pin (optional, for interrupts)
A0, A1, A2   →    GND or VCC (address selection)
```

### I2C Address Configuration
```
A2   A1   A0   Address (PCF8574)   Address (PCF8574A)
L    L    L    0x20               0x38
L    L    H    0x21               0x39
L    H    L    0x22               0x3A
L    H    H    0x23               0x3B
H    L    L    0x24               0x3C
H    L    H    0x25               0x3D
H    H    L    0x26               0x3E
H    H    H    0x27               0x3F
```

### Pull-up Resistors
- **I2C Bus**: 4.7kΩ pull-ups on SDA and SCL
- **Input Pins**: PCF8574 has weak internal pull-ups (~100kΩ)
- **External Pull-ups**: 10kΩ recommended untuk reliable input reading

## Performance Notes

### Update Frequency
- Call `update()` regularly untuk state change detection
- I2C communication takes ~100μs untuk single read/write
- Recommended update interval: 10-50ms untuk responsive applications

### Memory Usage
- Minimal memory overhead (~20 bytes state variables)
- Inherits dari PCF8574 class, semua original methods available
- State tracking adds negligible overhead

### I2C Considerations
- PCF8574 supports up to 400kHz I2C clock
- Multiple PCF8574s can share same I2C bus
- Use `setLatency()` untuk optimize read timing
- Lower latency = more responsive, higher CPU usage

## Troubleshooting

### No Communication
1. Check I2C connections (SDA, SCL, power, ground)
2. Verify I2C address dengan I2C scanner
3. Check pull-up resistors on I2C bus
4. Test dengan simple I2C device scan

### Incorrect Pin States
1. Verify pin mode setup (INPUT vs OUTPUT)
2. Check external pull-up/pull-down resistors
3. Measure voltage levels pada PCF8574 pins
4. Consider loading effects untuk output pins

### Interrupt Issues
1. Check interrupt pin connection
2. Verify interrupt function is properly defined
3. Use `enableInterrupt()` after initialization
4. Keep interrupt handler minimal (set flags only)

### Encoder Problems
1. Use `setLatency(0)` untuk encoder applications
2. Check encoder wiring (A, B phases)
3. Try different encoder algorithms
4. Consider hardware debouncing untuk mechanical encoders

## Integration with CeriaDev

### Enable in Project
```cpp
// In your enable.h or main file
#define ENABLE_CERIA_MODULE_PCF8574
#include "CeriaModulePCF8574.h"
```

### Platform Compatibility
- **ESP32**: Full support, multiple I2C buses, all features
- **ESP8266**: Full support, single I2C bus, all features  
- **Arduino Uno**: Full support, single I2C bus, limited interrupt pins
- **Arduino Mega**: Full support, single I2C bus, multiple interrupt pins
- **SAMD/RP2040/STM32**: Full support dengan platform-specific optimizations

## PCF8574 Base Class Reference

CeriaModulePCF8574 inherits dari PCF8574 library, sehingga semua methods original tetap tersedia:

### Original PCF8574 Methods
```cpp
// Pin operations
void pinMode(uint8_t pin, uint8_t mode, uint8_t output_start = HIGH);
uint8_t digitalRead(uint8_t pin, bool forceReadNow = false);
bool digitalWrite(uint8_t pin, uint8_t value);

// Bulk operations
PCF8574::DigitalInput digitalReadAll();          // Struct-based read
bool digitalWriteAll(PCF8574::DigitalInput);     // Struct-based write
byte digitalReadAll();                           // Byte-based read (LOW_MEMORY mode)
bool digitalWriteAll(byte);                      // Byte-based write

// Encoder support (multiple algorithms)
void encoder(uint8_t pinA, uint8_t pinB);
bool readEncoderValue(uint8_t pinA, uint8_t pinB, volatile long *value, bool reverse = false);

// Interrupt management
void attachInterrupt();
void detachInterrupt();

// Configuration
void setLatency(int latencyMs);
int getLatency();
bool isLastTransmissionSuccess();
```

### Advanced Usage with Base Class
```cpp
void advancedExample() {
    pcf.update();
    
    // Use wrapper methods
    bool buttonPressed = (pcf.readPin(P0) == LOW);
    
    // Mix with original PCF8574 methods
    PCF8574::DigitalInput allPins = pcf.digitalReadAll();
    if (allPins.p1 == HIGH) {
        // Handle P1 high state
    }
    
    // Access transmission status
    if (!pcf.isLastTransmissionSuccess()) {
        Serial.println("I2C communication error");
    }
    
    // Manual latency control
    pcf.setLatency(5);  // 5ms latency
    Serial.println("Current latency: " + String(pcf.getLatency()) + "ms");
}
```

CeriaModulePCF8574 provides clean, intuitive access to PCF8574 GPIO expansion while maintaining full compatibility dengan underlying PCF8574 library. Mix and match wrapper methods dengan original PCF8574 methods sesuai kebutuhan aplikasi untuk maximum flexibility dan control.