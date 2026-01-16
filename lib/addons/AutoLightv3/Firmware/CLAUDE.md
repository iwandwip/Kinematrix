# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is the **Firmware** directory for AutoLight V3, containing the embedded C++ codebase for ESP32-based LED control systems. The firmware provides comprehensive LED matrix control with dynamic PCF8574 I2C expander management, multi-modal button systems, integrated REST API web server, and CLI interface.

**Platform**: ESP32 (primary), ESP8266 (limited support)
**Architecture**: Modular component design with singleton configuration management
**Key Features**: 16 LED sequence modes, dynamic PCF distribution, hardware I2C expansion, FreeRTOS task management

## Build Commands

All build commands should be run from the **project root** (`/home/iwandwp/catkin_ws/KinematrixBeta/`):

```bash
# Primary development workflow (from project root)
python3 ./index.py              # Main development command (builds and starts WebClient)

# PlatformIO firmware commands (from project root)
pio run                         # Build for ESP32
pio run --target upload         # Upload firmware to device
pio device monitor             # Monitor serial output at 115200 baud
pio run --target clean          # Clean build artifacts

# Combined workflow
pio run --target upload && pio device monitor
```

**Important**: Never run build commands from the Firmware directory itself. Always use the project root.

## Core Architecture

### Component Organization

```
Firmware/
├── Core/                          # Core system components
│   ├── Channel/                   # LED control system
│   │   ├── BaseChannel/           # Main LED controller
│   │   │   ├── BaseChannel.h/.cpp      # LED management (33KB implementation)
│   │   │   └── BaseChannelSequence.cpp # 16 sequence modes (107KB)
│   │   └── Indicator.h/.cpp       # Status indicators (RGB LED + buzzer)
│   ├── Config/                    # Configuration management
│   │   ├── BaseConfig.h/.cpp      # Dynamic PCF distribution
│   │   ├── ConfigManager.h/.cpp   # Singleton configuration manager
│   │   └── ConfigData.h           # Data structures and enums
│   ├── Interaction/               # User interaction systems
│   │   ├── ButtonInterrupt.h/.cpp      # Multi-modal button handler
│   │   └── SerialCommander.h/.cpp      # CLI interface
│   ├── Cores/                     # System services
│   │   └── Task.h/.cpp            # FreeRTOS task wrapper
│   └── Common/
│       └── Constants.h            # System-wide constants
└── API/                           # REST API server
    └── APIServerManager.h/.cpp    # Integrated web server (port 8000)
```

### Key Manager Classes

**ConfigManager** (Singleton)
- Runtime configuration with EEPROM persistence
- Thread-safe access to configuration data
- Manages WiFi credentials and device settings
- Located: `Core/Config/ConfigManager.h`

**APIServerManager**
- Integrated AsyncWebServer on port 8000
- RESTful API endpoints for LED control
- FreeRTOS task-based execution
- Automatic WiFi AP/STA mode handling
- Located: `API/APIServerManager.h`

**BaseChannel**
- Main LED controller with 16+ sequence modes
- PCF8574 dynamic distribution support
- Multi-modal button system integration
- Sequence mapping and filtering
- Located: `Core/Channel/BaseChannel/BaseChannel.h`

**SerialCommander**
- Full CLI interface at 115200 baud
- Multi-level access control (4 permission levels)
- WiFi, LED, PCF, button, and sequence management
- Hardware I2C debugging commands
- Located: `Core/Interaction/SerialCommander.h`

### Core Data Structures

```cpp
// Dynamic PCF8574 distribution
struct PCFDistribution {
    uint8_t address;         // I2C address (0x20-0x27)
    uint8_t used_pins;       // Allocated pins (1-8)
    uint8_t start_channel;   // Starting channel number
    uint8_t pin_offset;      // Pin offset within PCF
    bool is_active;          // PCF status
};

// Distribution strategies
typedef enum {
    DISTRIBUTE_BALANCED = 0,    // Even distribution across PCFs
    DISTRIBUTE_SEQUENTIAL = 1,  // Fill PCFs sequentially
    DISTRIBUTE_OPTIMIZED = 2,   // Minimize PCF usage
    DISTRIBUTE_CUSTOM = 3       // Manual configuration
} distribution_strategy_t;

// Button interaction modes
typedef enum {
    BUTTON_MODE_4BUTTON = 0,  // ON/OFF/NEXT/PREV (legacy)
    BUTTON_MODE_1BUTTON = 1,  // Single cycle: OFF→ON→Mode2-15→OFF
    BUTTON_MODE_2BUTTON = 2,  // Toggle + Next
    BUTTON_MODE_3BUTTON = 3,  // Separate ON/OFF/Next
    BUTTON_MODE_CUSTOM = 99   // User-defined handlers
} button_mode_t;

// Configuration container
typedef struct {
    ConfigHeader header;     // Channel count, pin pointers, I2C addresses
    ConfigTable table;       // Address and version tables
} ConfigData;
```

## Common Development Patterns

### Basic Setup Pattern

```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V3
#include "Kinematrix.h"
using namespace AutoLight;

BaseChannel led;
BaseConfig config;

void setup() {
    Serial.begin(115200);

    // Dynamic PCF configuration
    config.setDynamicConfig(12, 2);  // 12 channels, 2 PCF8574s
    led.attachConfig(config.getConfigData());
    led.initialize();

    // Enable integrated systems
    led.enableWebServer(true);       // REST API on port 8000
    led.setButtonMode(BUTTON_MODE_1BUTTON);
}

void loop() {
    led.runAutoLight();
    delay(10);
}
```

### Development Mode with Full Features

```cpp
void setup() {
    Serial.begin(115200);

    // SerialCommander for CLI debugging
    SerialCommander cmd;
    cmd.init(led, config, button);

    // Advanced PCF configuration
    config.setDistributionStrategy(DISTRIBUTE_OPTIMIZED);
    config.setDynamicConfig(24, 3);  // 24 channels across 3 PCFs

    // Sequence mapping (filter available modes)
    led.enableSequenceMapping(true);
    led.setActiveSequences(5, 2, 5, 7, 10, 12);  // Only modes 2,5,7,10,12

    // Multi-modal button
    led.setButtonMode(BUTTON_MODE_1BUTTON);

    led.initialize();
    led.enableWebServer(true);
}

void loop() {
    led.runAutoLight();
    cmd.process();  // Handle serial commands
    delay(10);
}
```

### Dynamic Distribution Strategies

```cpp
// Balanced: Even distribution across all PCFs
config.setDynamicConfig(24, 3, DISTRIBUTE_BALANCED);  // 8 channels per PCF

// Sequential: Fill first PCF fully before next
config.setDynamicConfig(24, 3, DISTRIBUTE_SEQUENTIAL);  // PCF1:8, PCF2:8, PCF3:8

// Optimized: Minimize number of PCFs used
config.setDynamicConfig(24, 8, DISTRIBUTE_OPTIMIZED);  // Uses only 3 PCFs

// Custom: Manual address and channel specification
PCFDistribution custom[3] = {
    {0x27, 8, 0, 0, true},   // First PCF: channels 0-7
    {0x38, 8, 8, 0, true},   // Second PCF: channels 8-15
    {0x45, 8, 16, 0, true}   // Third PCF: channels 16-23
};
config.setCustomDistribution(custom, 3);
```

## SerialCommander CLI Interface

Available at 115200 baud via serial monitor. Commands are organized into categories:

### Command Categories

**WiFi Management**
```
wifi ap <ssid> <password>     # Start access point mode
wifi sta <ssid> <password>    # Connect to WiFi network
wifi scan                     # Scan available networks
wifi status                   # Show connection status
wifi restart                  # Restart WiFi AP
wifi reset                    # Reset to default credentials
wifi clients                  # Show connected clients
```

**LED Control**
```
led on                        # Turn all LEDs on
led off                       # Turn all LEDs off
led test <channel>            # Test specific channel
led status                    # Show all channel states
led next                      # Next sequence mode
led prev                      # Previous sequence mode
```

**Sequence Management**
```
sequence set <mode>           # Set LED sequence (0-15)
sequence map enable           # Enable sequence filtering
sequence map set 2,5,7,10     # Set active sequences
sequence map show             # Display current mapping
sequence delay <ms>           # Set timing delay (30-300ms)
```

**PCF8574 I2C Management**
```
pcf scan                      # Scan I2C bus for devices
pcf test <address>            # Test PCF at hex address (e.g., 0x27)
pcf status                    # Show all PCF states
pcf reset <address>           # Reset specific PCF
pcf write <address> <data>    # Write byte to PCF
pcf read <address>            # Read byte from PCF
```

**Button Configuration**
```
button mode <0-3>             # Set button mode
button test <index>           # Test button action
button config                 # Show current config
```

**Access Control**
```
serial activate <level>       # Activate permission level (0-3)
serial deactivate             # Deactivate elevated permissions
serial mode                   # Show current access level
```

## LED Sequence System

### Sequence Modes (0-15)

- **Mode 0**: Complete OFF state
- **Mode 1**: Static ON (all channels high)
- **Mode 2**: Blink all channels simultaneously
- **Mode 3**: Fill from two points (center outward)
- **Mode 4**: Fill from right to left
- **Mode 5**: Fill from both ends inward
- **Mode 6**: Blink one LED at a time sequentially
- **Mode 7**: Blink two LEDs while filling
- **Mode 8**: Snake pattern with reverse
- **Mode 9**: Random LED activation
- **Mode 10**: Wave propagation effect
- **Mode 11**: Complex multi-pattern coordination
- **Mode 12**: Advanced pattern matrix
- **Mode 13**: Custom blink patterns
- **Mode 14**: Advanced pattern combinations
- **Mode 15**: All sequences in rotation

### Sequence Implementation Location

All sequence implementations are in: `Core/Channel/BaseChannel/BaseChannelSequence.cpp`

Each sequence is a member function: `BaseChannel::taskSequence2BlinkAll()` through `taskSequence15AllSequences()`

### Sequence Mapping Feature

Filter available sequences to subset:

```cpp
led.enableSequenceMapping(true);
led.setActiveSequences(3, 2, 7, 10);  // Only modes 2, 7, 10 accessible
led.reorderActiveSequences(new_order, 3);  // Reorder filtered list
led.printSequenceMapping();  // Debug output
```

## REST API Endpoints

APIServerManager provides REST API on port 8000:

### Data Retrieval
```
GET /api/v1/data/get/device/ch      # Get channel count
GET /api/v1/data/get/device/name    # Get device name
GET /api/v1/data/get/device/serial  # Get device serial
GET /api/v1/data/get/mode           # Get current sequence mode (0-15)
GET /api/v1/data/get/delay          # Get timing delay (30-300ms)
GET /api/v1/data/get/status         # Get device status
```

### Control Commands
```
GET /api/v1/data/set/mode?value=N   # Set sequence mode (0-15)
GET /api/v1/data/set/delay?value=N  # Set timing delay (ms)
GET /api/v1/control/on              # Turn system on
GET /api/v1/control/off             # Turn system off
GET /api/v1/control/next            # Next sequence
GET /api/v1/control/prev            # Previous sequence
```

### Testing Endpoints
```bash
# From terminal (default AP mode IP: 192.168.4.1)
curl http://192.168.4.1:8000/api/v1/data/get/mode
curl "http://192.168.4.1:8000/api/v1/data/set/mode?value=5"
```

## Hardware Configuration

### PCF8574 I2C Expanders

- **Supported**: Up to 8 PCF8574 units (addresses 0x20-0x27)
- **Channels**: 1-64 total LED channels with dynamic distribution
- **I2C Bus**: Hardware I2C (ESP32 default SDA/SCL pins)

### Status Indicators

Defined in `Constants.h`:
- **RGB LED**: Pins 14 (red), 12 (green), 13 (blue)
- **Buzzer**: Pin 15
- **Max Buttons**: 4 configurable GPIO pins

### System Limits

```cpp
const int MAXNUM_CHANNEL = 64;              // Maximum LED channels
const int MAXNUM_IO_EXPANDER = 8;           // Maximum PCF8574 units
const int MAXNUM_TOTAL_TASK_SEQUENCE = 16;  // Total sequence modes
const int DEFAULT_DELAY_TIME = 50;          // Default timing (ms)
```

## Troubleshooting

### Compilation Issues

```bash
# Missing Kinematrix modules
# Solution: Ensure ENABLE_ADDONS_AUTOLIGHT_V3 is defined before including Kinematrix.h

# PCF8574 library errors
# Solution: Check platformio.ini includes required I2C expander library
```

### Runtime Issues

```bash
# I2C communication failures
# Use SerialCommander: "pcf scan" to detect connected devices
# Verify physical I2C connections (SDA/SCL pullup resistors)

# Web server not responding
# Check Serial output for WiFi AP initialization
# Default AP: AutoLight-XXXX (check serial for password)
# Default IP: 192.168.4.1:8000

# Button not responding
# Use SerialCommander: "button test <index>" to debug
# Verify GPIO pin configuration and button wiring
```

### Serial Debugging

```bash
# Enable full debug output at 115200 baud
pio device monitor

# Common debug commands
help                  # Show all available commands
wifi status          # Check network connection
pcf scan             # Verify I2C devices
led status           # Check channel states
sequence map show    # Display active sequences
```

## Memory Footprint

- **Minimal Configuration**: ~100KB flash, ~20KB RAM (basic LED control)
- **Full Configuration**: ~300KB flash, ~50KB RAM (all features enabled)
- **BaseChannelSequence.cpp**: ~107KB (16 sequence implementations)

## Platform Support

- **Primary**: ESP32 (all variants) - Full feature support including FreeRTOS
- **Secondary**: ESP8266 - Limited (no FreeRTOS task support)
- **Not Supported**: Arduino AVR (insufficient memory)

All code is ESP32-guarded with `#if defined(ESP32)` preprocessor directives.
