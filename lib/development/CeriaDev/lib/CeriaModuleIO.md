# CeriaModuleIO

## Overview
CeriaModuleIO adalah advanced I/O management system yang menggabungkan simplicity dari CeriaDev wrapper pattern dengan industrial-grade features dari Kinematrix I/O modules. Dirancang sebagai complete solution untuk GPIO expansion dengan PCF8574 yang dilengkapi debouncing, async operations, dan comprehensive state management.

## Features
- **Triple-Class Architecture**: `CeriaDigitalInput`, `CeriaDigitalOutput`, dan `CeriaModuleIO`
- **Smart Button Detection**: Advanced multi-click, long press levels, rapid fire, dan click patterns
- **Industrial Input Processing**: Configurable debouncing, edge detection, event counting
- **Advanced Output Control**: Async operations, pattern generation, callback support, reverse logic
- **Smart Object Management**: Dynamic creation/destruction dengan automatic memory management
- **Batch Processing**: Efficient multi-pin updates dalam single operation
- **Platform Support**: ESP32, ESP8266, AVR, SAMD, RP2040, STM32 dengan optimizations
- **Full Base Access**: Semua PCF8574 methods tetap available

## 🎮 Smart Button Features (v2.2)

### **Advanced Button Detection**
CeriaModuleIO v2.2 introduces sophisticated button interaction patterns inspired by modern user interfaces:

- **Single Click** - Standard button press detection
- **Double Click** - Two quick presses within configurable window (300ms default)
- **Triple Click** - Three quick presses within configurable window (400ms default)  
- **Long Press (3 Levels)** - Multi-level long press detection:
  - Long Press: 1000ms threshold
  - Very Long Press: 2000ms threshold
  - Extremely Long Press: 5000ms threshold
- **Click-and-Hold** - Long press followed by release detection
- **Rapid Fire** - Continuous event generation during long press (100ms intervals)

### **Smart Timing Configuration**
```cpp
struct ButtonTimings {
    unsigned long debounceTime = 50;
    unsigned long doubleClickWindow = 300;
    unsigned long tripleClickWindow = 400;
    unsigned long longPressThreshold = 1000;
    unsigned long veryLongPressThreshold = 2000;
    unsigned long extremelyLongPressThreshold = 5000;
    unsigned long rapidFireInterval = 100;
};
```

## Architecture Overview

### Class Hierarchy
```
CeriaModuleIO (inherits directly dari PCF8574)
├── CeriaDigitalInput[8]  - Smart input management
└── CeriaDigitalOutput[8] - Advanced output control
```

### Direct PCF8574 Integration
- **Direct Inheritance**: CeriaModuleIO extends PCF8574 directly for optimal performance
- **Native PCF8574 API**: Uses `pinMode()`, `digitalRead()`, `digitalWrite()` methods directly
- **Full Compatibility**: All PCF8574 methods and features remain accessible
- **Optimal Performance**: No wrapper overhead, direct hardware access

### Input Processing Features
- **Debounce Management**: Configurable per-pin debounce timing
- **Edge Detection**: `isPressed()`, `isReleased()` dengan state machine  
- **Long Press Detection**: Configurable time thresholds
- **Event Counting**: `COUNT_FALLING`, `COUNT_RISING`, `COUNT_BOTH`
- **State Tracking**: Complete button lifecycle monitoring

### Output Control Features
- **Async Operations**: Non-blocking delays dan pattern generation
- **Pattern Support**: Blink patterns, sequential effects, timed operations
- **Callback System**: Function callbacks untuk toggle events
- **Reverse Logic**: Support untuk active-low outputs
- **State Management**: Internal state tracking dengan external sync

## Basic Usage

### Initialization (Correct Pattern)
```cpp
#include "CeriaModuleIO.h"
using namespace CeriaDevOP;

CeriaModuleIO io(0x38);

void setup() {
    Serial.begin(115200);
    
    // Setup pins BEFORE begin() - following PCF8574 pattern
    io.setupButton(P0, 50, IO_INPUT_PULLUP);  // Button with pullup
    io.setupButton(P1, 30, IO_INPUT);         // Button without pullup
    io.setupLED(P4, false);                   // LED starts OFF
    io.setupLED(P5, true);                    // LED starts ON
    
    // Now initialize the module
    if (io.begin()) {
        Serial.println("CeriaModuleIO initialized successfully");
    } else {
        Serial.println("CeriaModuleIO initialization failed");
    }
}
```

### Simple Button and LED Control (Corrected)
```cpp
void setup() {
    Serial.begin(115200);
    
    // Setup I/O pins BEFORE begin()
    io.setupButton(P0, 50, IO_INPUT_PULLUP);  // Button with pullup, 50ms debounce
    io.setupLED(P1, false, false);            // LED, start OFF, normal polarity
    
    // Initialize after pin setup
    if (io.begin()) {
        Serial.println("Button/LED system ready");
    }
}

void loop() {
    io.updateAll();
    
    // Simple button -> LED control
    if (io.buttonPressed(P0)) {
        io.ledToggle(P1);
        Serial.println("Button pressed, LED toggled");
    }
    
    delay(10);
}
```

## API Reference

### Constructor Options
```cpp
// Basic constructor
CeriaModuleIO(uint8_t address = 0x38);

// With interrupt support  
CeriaModuleIO(uint8_t address, uint8_t interruptPin, void (*interruptFunction)());

// Platform-specific variants (sama seperti CeriaModulePCF8574)
CeriaModuleIO(uint8_t address, int sda, int scl);
CeriaModuleIO(TwoWire *pWire, uint8_t address);
// ... dll sesuai platform support
```

### Core Management Methods
```cpp
bool begin();                                    // Initialize module with auto pin setup
bool begin(bool setupPins);                      // Initialize with optional pin setup
void setupPins();                                // Apply all pre-configured pin modes
void updateAll();                                // Update all I/O objects
void setBatchUpdateInterval(uint8_t intervalMs); // Set update timing
void enableAutoUpdate(bool enable = true);       // Enable automatic updates
```

### **IMPORTANT: Native PCF8574 Pattern**
CeriaModuleIO now follows the **exact** PCF8574 library pattern with direct method calls:

```cpp
// ✅ CORRECT Pattern - Direct PCF8574 Usage
io.setupButton(P0, 50, IO_INPUT_PULLUP);  // Calls PCF8574::pinMode(P0, INPUT_PULLUP) internally
io.setupLED(P1, false);                   // Calls PCF8574::pinMode(P1, OUTPUT) internally
if (io.begin()) {                         // Calls PCF8574::begin() directly
    // Now ready to use - all methods are native PCF8574
    uint8_t state = io.digitalRead(P0);    // Direct PCF8574::digitalRead()
    io.digitalWrite(P1, HIGH);             // Direct PCF8574::digitalWrite()
    
    // Connection monitoring
    if (io.isConnected()) {
        Serial.println("PCF8574 connected successfully");
    }
}

// ❌ WRONG Pattern - Will not work properly
if (io.begin()) {                         // Initialize first
    io.setupButton(P0, 50);               // Configure pin after - TOO LATE!
}
```

### Native PCF8574 Methods Available
Since CeriaModuleIO inherits directly from PCF8574, all original methods are available:
```cpp
// All these methods work exactly like the original PCF8574 library:
io.pinMode(P0, INPUT_PULLUP);
io.digitalWrite(P1, HIGH);
uint8_t value = io.digitalRead(P2);
io.setupEncoder(P3, P4);                           // CeriaModuleIO wrapper method
bool success = io.readEncoderValue(P3, P4, &encoderCount);
uint8_t errorCode = io.getTransmissionStatusCode();
bool connected = io.isConnected();                 // CeriaModuleIO wrapper method
```

### Technical Implementation Details
CeriaModuleIO implements a **pre-setup system** with **direct PCF8574 integration**:

1. **Pre-Setup Storage**: Pin modes stored in `uint8_t _prePinModes[8]` using Arduino constants
2. **Setup Tracking**: Boolean flags in `bool _prePinSetup[8]` track configured pins
3. **Auto-Apply**: `begin()` automatically applies all stored pin modes using native `pinMode()`
4. **Direct API**: Uses Arduino standard constants (INPUT, INPUT_PULLUP, OUTPUT)
5. **Zero Overhead**: No wrapper methods, direct PCF8574 hardware access

```cpp
// Internal flow when you call:
io.setupButton(P0, 50, IO_INPUT_PULLUP);  // Stores INPUT_PULLUP
io.setupLED(P1, false);                   // Stores OUTPUT
io.begin();                               // Applies: PCF8574::pinMode(P0, INPUT_PULLUP)
                                         //          PCF8574::pinMode(P1, OUTPUT)
                                         // Then calls PCF8574::begin()

// All I/O operations are direct PCF8574 calls:
io.buttonPressed(P0);                     // Uses PCF8574::digitalRead(P0) directly
io.ledToggle(P1);                         // Uses PCF8574::digitalWrite(P1, value) directly
```

### Object Creation Methods
```cpp
CeriaDigitalInput* createInput(uint8_t pin, unsigned long debounceTime = 50, IOPinMode mode = IO_INPUT_PULLUP);
CeriaDigitalOutput* createOutput(uint8_t pin, bool isReverse = false);
```

### High-Level Setup Methods
```cpp
void setupButton(uint8_t pin, unsigned long debounceTime = 50, IOPinMode mode = IO_INPUT_PULLUP);
void setupLED(uint8_t pin, bool initialState = false, bool isReverse = false);
```

### Input Mode Options
```cpp
IO_INPUT        // No internal pullup (external pullup required)
IO_INPUT_PULLUP // Internal pullup enabled (default)
IO_OUTPUT       // Output mode
```

### Button Control Methods
```cpp
bool buttonPressed(uint8_t pin);                 // Edge detection - pressed
bool buttonReleased(uint8_t pin);                // Edge detection - released  
bool buttonLongPressed(uint8_t pin, unsigned long time = 1000);
unsigned long buttonCount(uint8_t pin);          // Get event count
void resetButtonCount(uint8_t pin);              // Reset event counter
void setInputCountMode(uint8_t pin, int mode);   // Set counting mode
```

### LED Control Methods
```cpp
void ledOn(uint8_t pin);                         // Turn LED on
void ledOff(uint8_t pin);                        // Turn LED off
void ledToggle(uint8_t pin);                     // Toggle LED state
void ledSet(uint8_t pin, bool state);            // Set LED to specific state
void ledBlink(uint8_t pin, unsigned long interval = 500);     // Async blink
void ledBlinkPattern(uint8_t pin, unsigned long onTime = 100, int count = 3); // Pattern blink
bool ledState(uint8_t pin);                      // Get current LED state
```

### Status and Monitoring Methods
```cpp
String getInputStatusString();                   // Get input status summary
String getOutputStatusString();                  // Get output status summary  
String getIOStatusString();                      // Get complete I/O status
bool waitForButtonPress(uint8_t pin, uint32_t timeoutMs = 5000);
bool waitForAnyButtonPress(uint32_t timeoutMs = 5000);

bool isConnected(uint32_t timeoutMs = 5000);           // Check PCF8574 connection status
void setupEncoder(uint8_t pinA, uint8_t pinB);        // Setup encoder pins
bool hasNewData();                                     // Check for new data since last update

// Smart Button Detection Methods
ButtonEvent getButtonEvent(uint8_t pin);              // Get last button event type
bool buttonSingleClick(uint8_t pin);                  // Single click detection
bool buttonDoubleClick(uint8_t pin);                  // Double click detection (300ms window)
bool buttonTripleClick(uint8_t pin);                  // Triple click detection (400ms window)
bool buttonLongPress(uint8_t pin);                    // Long press detection (1000ms)
bool buttonVeryLongPress(uint8_t pin);                // Very long press detection (2000ms)
bool buttonExtremelyLongPress(uint8_t pin);           // Extremely long press (5000ms)
bool buttonClickAndHold(uint8_t pin);                 // Click-and-hold pattern
bool buttonRapidFire(uint8_t pin);                    // Rapid fire during long press

// Smart Button Configuration
void setButtonTimings(uint8_t pin, const ButtonTimings& timings);
ButtonTimings getButtonTimings(uint8_t pin);
void resetButtonPattern(uint8_t pin);
```

## Usage Examples

### 🎮 Smart Button Features Demo
```cpp
void setup() {
    Serial.begin(115200);
    
    // Setup button dengan default timings
    io.setupButton(P0, 50, IO_INPUT_PULLUP);
    
    // Custom timing configuration untuk P1
    ButtonTimings fastTiming;
    fastTiming.debounceTime = 30;
    fastTiming.doubleClickWindow = 250;
    fastTiming.longPressThreshold = 800;
    fastTiming.rapidFireInterval = 80;
    
    if (io.begin()) {
        io.setButtonTimings(P1, fastTiming);
        Serial.println("Smart button system ready!");
    }
}

void loop() {
    io.updateAll();
    
    // Smart button detection
    if (io.buttonSingleClick(P0)) {
        Serial.println("Single click detected!");
        performAction(ACTION_SINGLE);
    }
    
    if (io.buttonDoubleClick(P0)) {
        Serial.println("Double click detected!");
        performAction(ACTION_DOUBLE);
    }
    
    if (io.buttonTripleClick(P0)) {
        Serial.println("Triple click detected!");
        performAction(ACTION_TRIPLE);
    }
    
    if (io.buttonLongPress(P0)) {
        Serial.println("Long press detected (1s)!");
        performAction(ACTION_LONG);
    }
    
    if (io.buttonVeryLongPress(P0)) {
        Serial.println("Very long press detected (2s)!");
        performAction(ACTION_VERY_LONG);
    }
    
    if (io.buttonExtremelyLongPress(P0)) {
        Serial.println("Extremely long press (5s)!");
        performAction(ACTION_EXTREME);
    }
    
    if (io.buttonClickAndHold(P0)) {
        Serial.println("Click and hold pattern!");
        performAction(ACTION_HOLD);
    }
    
    if (io.buttonRapidFire(P0)) {
        Serial.print("Rapid fire! ");
        static int rapidCount = 0;
        Serial.println("Shot #" + String(++rapidCount));
    }
    
    // Advanced event handling
    ButtonEvent event = io.getButtonEvent(P1);
    if (event != BUTTON_NONE) {
        handleAdvancedEvent(event, P1);
    }
    
    delay(10);
}

void performAction(ActionType action) {
    switch (action) {
        case ACTION_SINGLE:
            io.ledBlink(P4, 200);
            break;
        case ACTION_DOUBLE:
            io.ledBlinkPattern(P4, 100, 2);
            break;
        case ACTION_TRIPLE:
            io.ledBlinkPattern(P4, 100, 3);
            break;
        case ACTION_LONG:
            Serial.println("Entering config mode...");
            break;
        case ACTION_VERY_LONG:
            Serial.println("Factory reset initiated...");
            break;
        case ACTION_EXTREME:
            Serial.println("Emergency shutdown!");
            break;
    }
}
```

### Advanced Button Processing (Legacy Support)
```cpp
void setup() {
    Serial.begin(115200);
    
    // Setup ALL pins BEFORE begin() - Critical!
    io.setupButton(P0, 30, IO_INPUT_PULLUP);   // Fast response button with pullup
    io.setupButton(P1, 100, IO_INPUT);         // Mechanical switch without pullup
    io.setupButton(P2, 50, IO_INPUT_PULLUP);   // Standard button with pullup
    
    // Setup LEDs untuk feedback
    io.setupLED(P4, false);   // Status LED
    io.setupLED(P5, false);   // Activity LED
    io.setupLED(P6, false);   // Counter LED
    
    // Initialize module after pin setup
    if (io.begin()) {
        Serial.println("CeriaModuleIO initialized successfully");
        
        // Configure counting modes AFTER begin()
        io.setInputCountMode(P0, COUNT_FALLING);  // Count presses only
        io.setInputCountMode(P1, COUNT_BOTH);     // Count press and release
        io.setInputCountMode(P2, COUNT_RISING);   // Count releases only
        
        Serial.println("Advanced button processing ready");
    } else {
        Serial.println("CeriaModuleIO initialization failed!");
    }
}

void loop() {
    io.updateAll();
    
    // Handle button press events
    if (io.buttonPressed(P0)) {
        Serial.println("Fast button pressed! Count: " + String(io.buttonCount(P0)));
        io.ledToggle(P4);
        io.ledBlinkPattern(P5, 100, 2);  // Double blink feedback
    }
    
    if (io.buttonPressed(P1)) {
        Serial.println("Mechanical switch pressed! Count: " + String(io.buttonCount(P1)));
        io.ledToggle(P6);
    }
    
    // Handle long press
    if (io.buttonLongPressed(P0, 2000)) {
        Serial.println("Long press detected - resetting all counters");
        io.resetButtonCount(P0);
        io.resetButtonCount(P1);
        io.resetButtonCount(P2);
        
        // Visual feedback untuk reset
        io.ledBlinkPattern(P4, 50, 10);
    }
    
    // Handle button release
    if (io.buttonReleased(P1)) {
        Serial.println("Mechanical switch released");
    }
    
    // Periodic status report
    static unsigned long lastReport = 0;
    if (millis() - lastReport > 5000) {
        Serial.println("\n=== Status Report ===");
        Serial.println(io.getIOStatusString());
        Serial.println("Button counts - P0:" + String(io.buttonCount(P0)) + 
                      " P1:" + String(io.buttonCount(P1)) + 
                      " P2:" + String(io.buttonCount(P2)));
        lastReport = millis();
    }
    
    delay(10);
}
```

### LED Effects and Patterns
```cpp
void setup() {
    Serial.begin(115200);
    io.begin();
    
    // Setup 4 LEDs untuk pattern display
    for (int i = 0; i < 4; i++) {
        io.setupLED(i, false);
    }
    
    // Setup control button
    io.setupButton(P4, 50);
    
    Serial.println("LED pattern system ready");
}

int currentPattern = 0;
const int maxPatterns = 5;

void loop() {
    io.updateAll();
    
    // Change pattern on button press
    if (io.buttonPressed(P4)) {
        currentPattern = (currentPattern + 1) % maxPatterns;
        Serial.println("Switching to pattern " + String(currentPattern));
        
        // Turn off all LEDs untuk clean transition
        for (int i = 0; i < 4; i++) {
            io.ledOff(i);
        }
    }
    
    // Execute current pattern
    executePattern(currentPattern);
    
    delay(10);
}

void executePattern(int pattern) {
    static unsigned long lastPatternUpdate = 0;
    static int patternStep = 0;
    
    switch (pattern) {
        case 0: // Sequential chase
            if (millis() - lastPatternUpdate > 200) {
                for (int i = 0; i < 4; i++) {
                    io.ledSet(i, i == patternStep);
                }
                patternStep = (patternStep + 1) % 4;
                lastPatternUpdate = millis();
            }
            break;
            
        case 1: // Blink all
            io.ledBlink(0, 500);
            io.ledBlink(1, 500);
            io.ledBlink(2, 500);
            io.ledBlink(3, 500);
            break;
            
        case 2: // Binary counter
            if (millis() - lastPatternUpdate > 1000) {
                for (int i = 0; i < 4; i++) {
                    io.ledSet(i, (patternStep & (1 << i)) != 0);
                }
                patternStep = (patternStep + 1) % 16;
                lastPatternUpdate = millis();
            }
            break;
            
        case 3: // Breathing effect (simulated)
            if (millis() - lastPatternUpdate > 100) {
                bool state = ((patternStep % 20) < 10);
                for (int i = 0; i < 4; i++) {
                    io.ledSet(i, state);
                }
                patternStep++;
                lastPatternUpdate = millis();
            }
            break;
            
        case 4: // Random pattern
            if (millis() - lastPatternUpdate > 300) {
                for (int i = 0; i < 4; i++) {
                    io.ledSet(i, random(2));
                }
                lastPatternUpdate = millis();
            }
            break;
    }
}
```

### Multi-Function Control Panel
```cpp
struct ControlPanel {
    // Input assignments
    static const uint8_t BUTTON_START = P0;
    static const uint8_t BUTTON_STOP = P1;
    static const uint8_t BUTTON_RESET = P2;
    static const uint8_t ENCODER_A = P3;
    static const uint8_t ENCODER_B = P4;
    
    // Output assignments  
    static const uint8_t LED_POWER = P5;
    static const uint8_t LED_STATUS = P6;
    static const uint8_t LED_ERROR = P7;
};

enum SystemState {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_ERROR
};

SystemState currentState = STATE_IDLE;
int systemValue = 0;

void setup() {
    Serial.begin(115200);
    io.begin();
    
    // Setup control inputs
    io.setupButton(ControlPanel::BUTTON_START, 50);
    io.setupButton(ControlPanel::BUTTON_STOP, 50);
    io.setupButton(ControlPanel::BUTTON_RESET, 50);
    
    // Setup encoder (using low-level PCF8574 methods)
    io.setupEncoder(ControlPanel::ENCODER_A, ControlPanel::ENCODER_B);
    
    // Setup status LEDs
    io.setupLED(ControlPanel::LED_POWER, true);   // Power LED always on
    io.setupLED(ControlPanel::LED_STATUS, false);
    io.setupLED(ControlPanel::LED_ERROR, false);
    
    Serial.println("Multi-function control panel ready");
    displaySystemStatus();
}

void loop() {
    io.updateAll();
    
    // Handle control buttons
    if (io.buttonPressed(ControlPanel::BUTTON_START)) {
        handleStartButton();
    }
    
    if (io.buttonPressed(ControlPanel::BUTTON_STOP)) {
        handleStopButton();
    }
    
    if (io.buttonPressed(ControlPanel::BUTTON_RESET)) {
        handleResetButton();
    }
    
    // Handle encoder input
    volatile long encoderValue = systemValue;
    if (io.readEncoder(ControlPanel::ENCODER_A, ControlPanel::ENCODER_B, &encoderValue)) {
        systemValue = constrain(encoderValue, 0, 100);
        Serial.println("System value: " + String(systemValue));
    }
    
    // Update status LEDs based on state
    updateStatusLEDs();
    
    // System processing simulation
    processSystem();
    
    delay(10);
}

void handleStartButton() {
    Serial.println("START button pressed");
    
    switch (currentState) {
        case STATE_IDLE:
        case STATE_PAUSED:
            currentState = STATE_RUNNING;
            Serial.println("System started");
            break;
            
        case STATE_ERROR:
            Serial.println("Cannot start - system in error state");
            io.ledBlinkPattern(ControlPanel::LED_ERROR, 100, 3);
            break;
            
        default:
            Serial.println("System already running");
            break;
    }
    
    displaySystemStatus();
}

void handleStopButton() {
    Serial.println("STOP button pressed");
    
    if (currentState == STATE_RUNNING) {
        currentState = STATE_PAUSED;
        Serial.println("System paused");
    } else {
        Serial.println("System stopped");
        currentState = STATE_IDLE;
    }
    
    displaySystemStatus();
}

void handleResetButton() {
    Serial.println("RESET button pressed");
    
    // Long press untuk full reset
    if (io.buttonLongPressed(ControlPanel::BUTTON_RESET, 3000)) {
        Serial.println("Full system reset");
        currentState = STATE_IDLE;
        systemValue = 0;
        
        // Visual feedback
        io.ledBlinkPattern(ControlPanel::LED_STATUS, 50, 5);
    } else {
        // Short press untuk error clear
        if (currentState == STATE_ERROR) {
            currentState = STATE_IDLE;
            Serial.println("Error cleared");
        }
    }
    
    displaySystemStatus();
}

void updateStatusLEDs() {
    // Power LED always on (handled in setup)
    
    // Status LED based on system state
    switch (currentState) {
        case STATE_IDLE:
            io.ledOff(ControlPanel::LED_STATUS);
            break;
            
        case STATE_RUNNING:
            io.ledBlink(ControlPanel::LED_STATUS, 500);  // Slow blink
            break;
            
        case STATE_PAUSED:
            io.ledBlink(ControlPanel::LED_STATUS, 200);  // Fast blink
            break;
            
        case STATE_ERROR:
            io.ledOn(ControlPanel::LED_STATUS);  // Solid on
            break;
    }
    
    // Error LED
    if (currentState == STATE_ERROR) {
        io.ledBlink(ControlPanel::LED_ERROR, 300);
    } else {
        io.ledOff(ControlPanel::LED_ERROR);
    }
}

void processSystem() {
    static unsigned long lastProcess = 0;
    
    if (currentState == STATE_RUNNING && millis() - lastProcess > 1000) {
        // Simulate system processing
        Serial.println("Processing... Value: " + String(systemValue));
        
        // Simulate random error condition
        if (random(100) < 2) {  // 2% chance of error
            currentState = STATE_ERROR;
            Serial.println("System error occurred!");
            displaySystemStatus();
        }
        
        lastProcess = millis();
    }
}

void displaySystemStatus() {
    Serial.println("\n=== System Status ===");
    Serial.println("State: " + getStateString(currentState));
    Serial.println("Value: " + String(systemValue));
    Serial.println(io.getIOStatusString());
    Serial.println("Button counts - START:" + String(io.buttonCount(ControlPanel::BUTTON_START)) +
                  " STOP:" + String(io.buttonCount(ControlPanel::BUTTON_STOP)) +
                  " RESET:" + String(io.buttonCount(ControlPanel::BUTTON_RESET)));
    Serial.println("====================\n");
}

String getStateString(SystemState state) {
    switch (state) {
        case STATE_IDLE: return "IDLE";
        case STATE_RUNNING: return "RUNNING";
        case STATE_PAUSED: return "PAUSED";
        case STATE_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
```

### Real-Time Monitoring System
```cpp
struct MonitoringData {
    uint32_t timestamp;
    uint8_t inputStates;
    uint8_t outputStates;
    uint32_t buttonCounts[8];
    bool valid;
};

MonitoringData history[50];  // Circular buffer
int historyIndex = 0;

void setup() {
    Serial.begin(115200);
    io.begin();
    
    // Setup comprehensive I/O monitoring
    for (int i = 0; i < 4; i++) {
        io.setupButton(i, 50);
        io.setupLED(i + 4, false);
        io.setInputCountMode(i, COUNT_BOTH);
    }
    
    Serial.println("Real-time monitoring system active");
}

void loop() {
    io.updateAll();
    
    // Continuous data logging
    logSystemState();
    
    // Handle any button presses
    for (int i = 0; i < 4; i++) {
        if (io.buttonPressed(i)) {
            Serial.println("Button " + String(i) + " pressed - Count: " + String(io.buttonCount(i)));
            io.ledToggle(i + 4);  // Mirror to LED
        }
        
        if (io.buttonLongPressed(i, 2000)) {
            Serial.println("Button " + String(i) + " long pressed - triggering action");
            performButtonAction(i);
        }
    }
    
    // Periodic analysis
    static unsigned long lastAnalysis = 0;
    if (millis() - lastAnalysis > 10000) {  // Every 10 seconds
        analyzeHistory();
        lastAnalysis = millis();
    }
    
    // Real-time status display
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 2000) {  // Every 2 seconds
        displayRealTimeStatus();
        lastStatus = millis();
    }
    
    delay(50);  // 20Hz monitoring rate
}

void logSystemState() {
    MonitoringData& data = history[historyIndex];
    
    data.timestamp = millis();
    data.inputStates = 0;
    data.outputStates = 0;
    
    // Capture input states
    for (int i = 0; i < 4; i++) {
        if (io.digitalRead(i) == HIGH) {
            data.inputStates |= (1 << i);
        }
        data.buttonCounts[i] = io.buttonCount(i);
    }
    
    // Capture output states
    for (int i = 4; i < 8; i++) {
        if (io.ledState(i)) {
            data.outputStates |= (1 << (i - 4));
        }
    }
    
    data.valid = true;
    historyIndex = (historyIndex + 1) % 50;
}

void analyzeHistory() {
    Serial.println("\n=== Historical Analysis ===");
    
    uint32_t totalActivity = 0;
    uint32_t buttonTotals[4] = {0};
    int validSamples = 0;
    
    for (int i = 0; i < 50; i++) {
        if (history[i].valid) {
            validSamples++;
            
            // Count state changes
            for (int j = 0; j < 4; j++) {
                totalActivity += history[i].buttonCounts[j];
                buttonTotals[j] += history[i].buttonCounts[j];
            }
        }
    }
    
    if (validSamples > 0) {
        Serial.println("Valid samples: " + String(validSamples) + "/50");
        Serial.println("Total button activity: " + String(totalActivity));
        
        for (int i = 0; i < 4; i++) {
            Serial.println("Button " + String(i) + " total: " + String(buttonTotals[i]));
        }
        
        // Calculate activity rate
        uint32_t timeSpan = history[(historyIndex - 1 + 50) % 50].timestamp - history[historyIndex].timestamp;
        if (timeSpan > 0) {
            float activityRate = (totalActivity * 60000.0) / timeSpan;  // Actions per minute
            Serial.println("Activity rate: " + String(activityRate, 2) + " actions/min");
        }
    }
    
    Serial.println("========================\n");
}

void displayRealTimeStatus() {
    Serial.println("=== Real-Time Status ===");
    Serial.println("Timestamp: " + String(millis()));
    Serial.println(io.getIOStatusString());
    
    // Show activity levels
    Serial.print("Activity: ");
    for (int i = 0; i < 4; i++) {
        uint32_t count = io.buttonCount(i);
        Serial.print("B" + String(i) + ":" + String(count) + " ");
    }
    Serial.println();
    
    // Show LED states
    Serial.print("LEDs: ");
    for (int i = 4; i < 8; i++) {
        Serial.print("L" + String(i-4) + ":" + (io.ledState(i) ? "ON" : "off") + " ");
    }
    Serial.println();
    
    // Connection health
    if (io.isConnected()) {
        Serial.println("PCF8574 connection: HEALTHY");
    } else {
        Serial.println("PCF8574 connection: ERROR");
    }
    
    Serial.println("=======================\n");
}

void performButtonAction(int button) {
    switch (button) {
        case 0:
            Serial.println("Action: Reset all counters");
            for (int i = 0; i < 4; i++) {
                io.resetButtonCount(i);
            }
            io.ledBlinkPattern(4, 100, 5);
            break;
            
        case 1:
            Serial.println("Action: Toggle all LEDs");
            for (int i = 4; i < 8; i++) {
                io.ledToggle(i);
            }
            break;
            
        case 2:
            Serial.println("Action: LED pattern demo");
            for (int i = 4; i < 8; i++) {
                io.ledBlinkPattern(i, 200, 3);
                delay(100);
            }
            break;
            
        case 3:
            Serial.println("Action: System diagnostic");
            analyzeHistory();
            break;
    }
}
```

## Hardware Setup

### Basic PCF8574 Connection
Sama seperti CeriaModulePCF8574:
```
PCF8574      →    ESP32/Arduino
VCC          →    3.3V or 5V
GND          →    GND
SDA          →    SDA (I2C Data)
SCL          →    SCL (I2C Clock)
INT          →    GPIO pin (optional)
```

### I/O Device Connections
```
// Input devices (buttons, switches)
Device       →    PCF8574 Pin    →    Configuration
Button       →    P0             →    One side to pin, other to GND
Switch       →    P1             →    Common to pin, others to GND/VCC
Sensor       →    P2             →    Signal to pin, use external pullup

// Output devices (LEDs, relays)
Device       →    PCF8574 Pin    →    Configuration  
LED          →    P4             →    Anode to pin, cathode via resistor to GND
Relay        →    P5             →    Coil control via transistor/driver
Buzzer       →    P6             →    Signal to pin, ground to GND
```

## Performance Considerations

### Update Strategies
- **updateAll()**: Comprehensive update for all objects
- **Batch Processing**: Efficient multi-pin operations
- **Selective Updates**: Update only changed pins untuk optimization
- **Auto-Update**: Background updating dengan configurable intervals

### Memory Management
- **Dynamic Allocation**: Objects created on-demand
- **Smart Cleanup**: Automatic memory management
- **Array Management**: Efficient pin-to-object mapping
- **State Compression**: Minimal memory footprint per object

### Timing Optimization
- **Debounce Tuning**: Per-pin debounce configuration
- **Update Frequency**: Configurable batch update intervals
- **Async Operations**: Non-blocking output effects
- **Interrupt Support**: Hardware interrupt untuk immediate response

## Integration Benefits

### Best of All Worlds
- **CeriaDev Simplicity**: Clean, intuitive high-level API
- **Native PCF8574 Performance**: Zero-overhead direct hardware access
- **Full PCF8574 Compatibility**: All original library features accessible
- **Advanced I/O Management**: Industrial-grade input/output processing
- **Modern Architecture**: Object-oriented design with smart management

### Full PCF8574 Compatibility
```cpp
// Use as native PCF8574 (all original methods work)
io.pinMode(P0, OUTPUT);
io.digitalWrite(P0, HIGH);
uint8_t state = io.digitalRead(P1);
io.encoder(P2, P3);
bool encoderChanged = io.readEncoderValue(P2, P3, &count);

// Use advanced CeriaModuleIO features
io.setupButton(P4, 50, IO_INPUT_PULLUP);
io.buttonPressed(P4);
io.setupLED(P5, false);
io.ledBlink(P5, 500);

// Mix both approaches seamlessly
if (io.digitalRead(P6) == LOW) {  // Native PCF8574
    io.ledToggle(P7);             // CeriaModuleIO feature
}
```

### Performance Advantages
- **Direct Hardware Access**: No wrapper method overhead
- **Native I2C Communication**: Uses PCF8574's optimized I2C handling
- **Memory Efficient**: Minimal additional memory footprint
- **Interrupt Support**: Full hardware interrupt capability from PCF8574
- **Encoder Support**: Access to all PCF8574 encoder algorithms

CeriaModuleIO represents the pinnacle of embedded I/O management, combining ease-of-use dengan professional-grade features untuk comprehensive GPIO expansion solutions.

## Summary of Major Changes (v2.1 - Fixed Version)

### ✅ **Direct PCF8574 Integration**
- **Removed**: CeriaModulePCF8574 wrapper layer
- **Added**: Direct inheritance from PCF8574 for optimal performance
- **Result**: Zero-overhead access to all PCF8574 features

### ✅ **Native Method Usage**
- **Fixed**: `applyPreSetupPins()` now uses `PCF8574::pinMode()` instead of Arduino `pinMode()`
- **Fixed**: All internal pin operations use PCF8574 methods directly
- **Result**: Proper PCF8574 hardware initialization and control

### ✅ **Missing Method Implementation**
- **Added**: `isConnected()` method using `PCF8574::isLastTransmissionSuccess()`
- **Added**: `setupEncoder()` method as wrapper for `PCF8574::encoder()`
- **Added**: `hasNewData()` method for data change detection
- **Result**: Complete API coverage as documented

### ✅ **Enhanced Data Management**
- **Added**: `_hasNewData` flag for update status tracking
- **Added**: `_lastUpdate` timestamp for timing control
- **Fixed**: `updateAll()` now properly tracks data changes
- **Result**: Reliable data synchronization and status monitoring

### ✅ **Standard Arduino Constants**
- **Confirmed**: Uses `INPUT_PULLUP`, `INPUT`, `OUTPUT` constants
- **Fixed**: Pre-setup system now properly applies these to PCF8574
- **Result**: Consistent with Arduino IDE and native PCF8574 library

### ✅ **Enhanced Error Handling**
- **Available**: `getTransmissionStatusCode()` from base PCF8574
- **Available**: `isConnected()` wrapper for connection monitoring  
- **Result**: Complete PCF8574 error handling capability

### ✅ **Full Feature Access**
- **Available**: All PCF8574 encoder methods via `readEncoderValue()`
- **Available**: All PCF8574 interrupt methods via inheritance
- **Available**: All PCF8574 configuration methods directly
- **Result**: No PCF8574 functionality is hidden or restricted

CeriaModuleIO v2.1 adalah **true PCF8574 extension** yang telah diperbaiki untuk menambahkan advanced I/O management tanpa mengurangi akses ke native PCF8574 functionality. Versi ini telah mengatasi masalah-masalah kritis pada implementasi sebelumnya dan sekarang fully compatible dengan native PCF8574 library.