# CLAUDE.md - Modules Core

## Directory Overview
Core functionality modules providing the essential building blocks for embedded systems development.

## Architecture
Modular system with conditional compilation allowing selective inclusion of only required components.

## Module Categories

### Communication (15 modules)
- **`communication/wired/`** - I2C, Modbus, Serial variants, SPI
- **`communication/wireless/`** - LoRa (multiple variants), ESP-NOW

### Control Systems (7 modules)
Advanced control algorithms for automation and robotics:
- **`control/PID.h`** - PID controller with auto-tuning
- **`control/PIDController.h`** - Enhanced PID with EEPROM persistence
- **`control/FuzzyMamdani.h`** - Mamdani fuzzy logic controller
- **`control/FuzzySugeno.h`** - Sugeno fuzzy logic system
- **`control/FuzzyTsukamoto.h`** - Tsukamoto fuzzy inference
- **`control/DecisionTree.h`** - Decision tree classifier with pruning
- **`control/KNN.h`** - K-Nearest Neighbors with cross-validation

### Display Systems (3 modules)
- **`display/lcd-menu.h`** - LCD menu system with navigation
- **`display/oled-menu.h`** - OLED menu system
- **`display/sh1106-menu.h`** - SH1106 display support with rendering

### Hardware Drivers (2 modules)
- **`driver/motor/`** - Motor control systems
- **`driver/servo/`** - Servo control with PCA9685 support

### File Systems (4 modules)
- **`file/EEPROMLib.h`** - EEPROM operations for AVR
- **`file/EEPROMLibESP8266.h`** - ESP8266-specific EEPROM
- **`file/SDArduino.h`** - SD card operations for Arduino
- **`file/SDCardModuleESP32.h`** - ESP32 SD card with advanced features

### I/O Expansion (8 modules)
- **`io/pcf8574-*.h`** - PCF8574 I2C expander variants
- **`io/rotary-module.h`** - Rotary encoder handling
- **`io/seven-segment*.h`** - 7-segment display drivers
- **`io/input-module.h`** - Input handling abstraction
- **`io/output-module.h`** - Output control abstraction

### Task Management (5 modules)
- **`task/FreeRTOSHandler.h`** - FreeRTOS integration
- **`task/Task.h`** - Task management abstraction
- **`time/timer-*.h`** - Timer and duration management

### Utilities (4 modules)
- **`utils/LogicConditionManager.h`** - Complex logic condition handling
- **`utils/StateActionManager.h`** - State machine management
- **`utils/VariableWatcher.h`** - Variable monitoring and alerts
- **`utils/EasyLogic.h`** - Simplified logic operations

### Debug Tools (3 modules)
- **`debug/SerialDebugger.h`** - Full-featured serial debugging
- **`debug/SerialDebuggerLite.h`** - Lightweight debugging
- **`debug/SerialDebuggerV2.h`** - Enhanced debugging with filtering

### WiFi/Cloud Services (21 modules)
- **`wifi/WiFiModule.h`** - Basic WiFi connection management
- **`wifi/wifi-handler*.h`** - Advanced WiFi handling with reconnection
- **`wifi/firebase/`** - Firebase integration (3 versions)
- **`wifi/mqtt-manager.h`** - MQTT client with auto-reconnection
- **`wifi/google-sheets.h`** - Google Sheets API integration
- **`wifi/telegram-bot.h`** - Telegram bot functionality
- **`wifi/whatsapp-bot.h`** - WhatsApp bot integration
- **`wifi/datetime-ntp*.h`** - NTP time synchronization

## Usage Pattern
Each module is enabled through `lib/enable.h` definitions:
```cpp
#define ENABLE_MODULE_PID_CONTROLLER
#define ENABLE_MODULE_WIFI_HANDLER_V2
#define ENABLE_MODULE_MQTT_MANAGER
```

## Platform Support
- **ESP32**: Full feature set including WiFi, advanced algorithms
- **ESP8266**: WiFi features with memory optimization
- **AVR (Arduino)**: Basic modules optimized for limited resources

## Integration
Modules are orchestrated through:
- `KinematrixModules.h` - Main module inclusion
- `KinematrixModulesHelper.h` - Helper functions
- `KinematrixModulesNoDef.h` - Alternative inclusion without defines