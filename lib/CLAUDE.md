# CLAUDE.md - Library Core

## Directory Overview
Main library directory containing all Kinematrix modules, sensors, and add-on systems.

## Structure
- **`modules/`** - Core functionality modules (168 available modules)
- **`sensors/`** - Dual sensor framework (V1 legacy + V2 modern)
- **`addons/`** - Complete application systems (AutoLight variants)
- **`enable.h`** - Master module enable definitions

## Module Categories

### Communication Modules (15)
- **Wired**: I2C expansion, Modbus RTU/ASCII, enhanced Serial variants, SPI
- **Wireless**: LoRa (multiple variants), ESP-NOW mesh networking

### Control Algorithms (7)
- PID with auto-tuning (Ziegler-Nichols, Cohen-Coon)
- Fuzzy Logic (Mamdani, Sugeno, Tsukamoto)
- Decision Trees with pruning
- KNN classification with cross-validation

### WiFi/Cloud Services (21)
- Connection management with WiFiManager
- Firebase (3 versions): Realtime DB, Firestore, Storage, Messaging
- Cloud integrations: Google Sheets, MQTT, Telegram/WhatsApp bots
- NTP time synchronization

### Hardware Interfaces
- **Display Systems**: LCD/OLED menu systems, SH1106 support
- **I/O Expansion**: PCF8574 variants, rotary encoders, 7-segment displays
- **Hardware Drivers**: Motor and servo control
- **File Systems**: EEPROM, SD card operations

### Development Tools
- **Debug**: Serial debuggers with different verbosity levels
- **Task Management**: FreeRTOS integration, timer systems
- **Utilities**: Logic conditions, state management, variable watching

## Key Files
- **`enable.h`** - Central configuration for all 168 modules
- Module implementations in respective subdirectories
- Header files providing unified API access

## Usage Pattern
```cpp
// Enable specific modules before including main header
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_MODULE_WIFI_HANDLER_V2
#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"
```

## Architecture
- Conditional compilation system for memory optimization
- Platform abstraction supporting ESP32, ESP8266, AVR
- Hierarchical module organization by functionality
- Unified API design across all components