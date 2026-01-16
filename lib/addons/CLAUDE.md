# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is the **addons** directory of the Kinematrix framework, containing complete application systems built on top of the core Kinematrix modules. The primary addon is the **AutoLight** ecosystem - a comprehensive LED matrix control system for automotive and industrial applications.

## Build Commands

The addons system is built using PlatformIO as part of the larger Kinematrix project:

```bash
# Build the project (includes all enabled addons)
pio run

# Build for specific environments
pio run -e esp32doit-devkit-v1
pio run -e uno
pio run -e nodemcuv2

# Upload to device
pio run --target upload

# Clean build
pio run --target clean

# Monitor serial output
pio device monitor
```

## AutoLight Ecosystem Architecture

The AutoLight system represents **four generations** of LED control evolution, each building upon the previous while maintaining backward compatibility:

### Recent Fixes (Latest Update)

**Major Library Restructuring**: Comprehensive fixes for compilation and dependency issues:

1. **Duplicate Definition Fix**: Resolved `struct Credentials` redefinition
   - **Problem**: Duplicate definition between `ConfigData.h` and `WebManager.h`
   - **Solution**: Removed duplicate from `WebManager.h`, added forward declaration
   - **Impact**: Eliminated redefinition compilation errors

2. **Include System Overhaul**: Fixed circular dependencies and multiple definition issues
   - **Problem**: `index.h` included `.cpp` files causing multiple definitions
   - **Solution**: Removed `.cpp` includes from `index.h`, restored proper header/implementation separation
   - **Approach**: Headers contain only declarations, .cpp files contain implementations
   - **Files Modified**: All AutoLight V3 headers and implementation files

3. **Proper Header/Implementation Separation**: Standard C++ practices restored
   - **Problem**: Mixed header-only and separate compilation approaches
   - **Solution**: Clean separation - headers (.h) for declarations, implementation (.cpp) for definitions
   - **Pattern**: `.cpp` files include their corresponding `.h` files for class definitions

4. **Include Guard Standardization**: Consistent header protection
   - **Problem**: Mixed `#pragma once` and traditional guards 
   - **Solution**: Standardized to traditional `#ifndef/#define/#endif` guards
   - **Impact**: Universal compiler compatibility

**Architecture Changes**:
- **Standard C++ Design**: Proper header/implementation file separation
- **Clean Dependencies**: Strict hierarchical include structure
- **Forward Declarations**: Proper use to break circular dependencies
- **Build System Compatible**: Standard linking of .cpp files through build system

**Files Modified**: `index.h`, all `.h` and `.cpp` files in AutoLight V3
**Impact**: All AutoLight V3 examples now compile cleanly without errors

### Version Evolution

1. **AutoLight V1** (`AutoLight/`) - Foundation architecture with "Keed" naming convention
2. **AutoLight V2** (`AutoLightv2/`) - Web connectivity and namespace organization  
3. **AutoLight V3** (`AutoLightv3/`) - Dynamic PCF distribution and multi-modal buttons
4. **AutoLight V4** (`AutoLightv4/`) - Fully dynamic with zero-configuration capabilities
5. **AutoLight V5** (Concept) - Matrix spatial control with user-defined patterns

### Key Architectural Patterns

#### 1. Progressive Enhancement Architecture
Each version maintains **100% backward compatibility** while adding revolutionary features:
- **V1**: Established core patterns and I2C expansion
- **V2**: Added web interface and FreeRTOS task management
- **V3**: Introduced dynamic PCF distribution and 5 button modes
- **V4**: Full dynamic configuration with intelligent auto-detection

#### 2. Modular Component Design
All versions follow consistent component organization:
```
{Version}/
├── index.h                 # Component orchestration
├── Channel/               # LED control logic
├── Config/                # Configuration management
├── Cores/                 # System services (FileSystem, Task)
├── Log/                   # Debug logging
└── CLAUDE.md              # Version-specific documentation
```

#### 3. Hardware Abstraction System
- **I2C Expansion**: PCF8574 I/O expander support (8-128 channels)
- **Dynamic Distribution**: Intelligent channel allocation across multiple PCFs
- **Platform Support**: ESP32 primary, ESP8266/AVR compatibility
- **Button Systems**: 1-4 button configurations with 5 interaction modes

## Core Data Structures

### PCF Distribution System (V3/V4)
```cpp
struct PCFDistribution {
    uint8_t address;         // I2C address (0x20-0x27)
    uint8_t used_pins;       // Allocated pins (1-8)
    uint8_t start_channel;   // Starting channel number
    uint8_t pin_offset;      // Pin offset within PCF
    bool is_active;          // PCF status
};

typedef enum {
    DISTRIBUTE_BALANCED = 0,    // Even distribution
    DISTRIBUTE_SEQUENTIAL = 1,  // Fill sequentially
    DISTRIBUTE_OPTIMIZED = 2,   // Minimize PCF usage
    DISTRIBUTE_CUSTOM = 3       // Manual configuration
} distribution_strategy_t;
```

### Multi-Modal Button System
```cpp
typedef enum {
    BUTTON_MODE_4BUTTON = 0,  // ON/OFF/NEXT/PREV (V1-V3)
    BUTTON_MODE_1BUTTON = 1,  // Single: OFF→ON→Mode2-15→OFF
    BUTTON_MODE_2BUTTON = 2,  // Toggle + Next
    BUTTON_MODE_3BUTTON = 3,  // Separate ON/OFF/Next
    BUTTON_MODE_CUSTOM = 99   // User-defined handlers
} button_mode_t;
```

### Dynamic Configuration (V4)
```cpp
struct DynamicConfig {
    config_mode_t mode;                    // AUTO, MANUAL, HYBRID
    distribution_mode_t distribution_mode;  // Distribution strategy
    button_mode_v4_t button_mode;          // Button interaction
    PCFDevice pcf_devices[16];             // Up to 16 PCFs
    ChannelMap channel_map[128];           // Up to 128 channels
    bool auto_scan_enabled;                // Runtime detection
    bool auto_fix_enabled;                 // Automatic correction
    bool hot_swap_enabled;                 // Hot-swap PCF support
};
```

## Version-Specific Usage Patterns

### AutoLight V1 (Foundation)
```cpp
#define ENABLE_ADDONS_AUTOLIGHT
#include "Kinematrix.h"

KeedConfiguration cfg;
KeedAutoLight led;

void setup() {
    configuration_t config;
    config.sequence = 0;
    config.setDelay(50);
    cfg.setConfig(config);
    led.setConfiguration(&cfg);
    led.begin();
}
```

### AutoLight V2 (Web-Enabled)  
```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V2
#include "Kinematrix.h"
using namespace AutoLight;

BaseConfig config;
BaseChannel channel;

void setup() {
    config.setConfigIndex(I2C_CONFIG);
    channel.setConfig(&config);
    channel.begin();
    // Web server automatically initialized
}
```

### AutoLight V3 (Dynamic Distribution)
```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V3
#include "Kinematrix.h" 
using namespace AutoLight;

BaseConfig config;
BaseChannel channel;

void setup() {
    // Dynamic PCF distribution
    config.setDistributionStrategy(DISTRIBUTE_OPTIMIZED);
    config.setPCFCount(3);
    config.setPCFAddresses({0x27, 0x45, 0x67});
    config.distributeChannels(24); // 24 channels across 3 PCFs
    
    // Multi-modal button system
    channel.setButtonMode(BUTTON_MODE_1BUTTON);
    channel.setConfig(&config);
    channel.begin();
}
```

### AutoLight V4 (Fully Dynamic)
```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V4
#include "Kinematrix.h"
using namespace AutoLight;

AutoLightV4 led(true); // Enable debug

void setup() {
    // Zero-configuration automatic mode
    led.autoScan();       // Auto-detect hardware
    led.autoDistribute(); // Auto-distribute channels
    led.begin();          // Ready to use!
    
    // Or builder pattern manual configuration
    led.configure()
        .channels(12)
        .pcf(2)
        .addresses(0x38, 0x21)
        .distribute(6, 6)
        .buttonMode(BUTTON_MODE_SINGLE)
        .autoFix(true)
        .apply();
}
```

## LED Sequence System

All versions support **16 total lighting modes**:
- **Mode 0**: Complete OFF state
- **Mode 1**: Static ON (all channels)  
- **Modes 2-15**: 14 customizable LED sequence patterns

### Sequence Compatibility
- **V1-V3**: Hardcoded sequences with mathematical pattern algorithms
- **V4**: 100% V3 sequence compatibility + custom sequence support
- **V5** (Concept): User-defined patterns with matrix spatial control

## Development Guidelines

### Version Selection Strategy
- **New Projects**: Use AutoLight V4 for maximum capabilities
- **Legacy Systems**: V3 for dynamic features, V2 for web requirements, V1 for simple control
- **Educational**: Start with V1, progress through versions to understand evolution
- **Production**: V4 for robust validation and auto-recovery features

### Hardware Configuration
```cpp
// Maximum capabilities per version:
// V1: 24 channels, 3 PCFs, hardcoded sequences
// V2: 64 channels, 8 PCFs, web interface  
// V3: 64 channels, 8 PCFs, dynamic distribution
// V4: 128 channels, 16 PCFs, full automation
```

### Memory Optimization
- **Enable only required versions**: Each version adds ~50-100KB
- **Use conditional compilation**: Enable specific features only
- **Platform considerations**: ESP32 recommended for V3/V4, ESP8266 for V1/V2

### Error Handling Patterns
```cpp
// V1-V3: Manual error checking
cfg_error_t err = config.readChannel();
if (err != CFG_OK) { /* handle error */ }

// V4: Automatic validation and fixing
auto result = led.validate();
if (result != VALIDATION_OK) {
    led.autoFix(); // Automatic correction
}
```

## Integration with Kinematrix Framework

### Enable Flags
```cpp
// Individual version enables
#define ENABLE_ADDONS_AUTOLIGHT      // V1
#define ENABLE_ADDONS_AUTOLIGHT_V2   // V2  
#define ENABLE_ADDONS_AUTOLIGHT_V3   // V3
#define ENABLE_ADDONS_AUTOLIGHT_V4   // V4
```

### Include Patterns
```cpp
#include "addons/AutoLight.h"     // V1
#include "addons/AutoLightv2.h"   // V2
#include "addons/AutoLightv3.h"   // V3  
#include "addons/AutoLightv4.h"   // V4
```

### Namespace Usage
- **V1**: Global namespace with "Keed" prefix
- **V2-V4**: `AutoLight` namespace for clean organization

## Testing and Validation

### Hardware Testing
```bash
# Test individual channels
led.testAllChannels();

# Validate configuration  
auto result = led.validate();
led.printValidationReport();

# Runtime statistics
led.printRuntimeStats();
```

### Development Testing
- **Use V4 debug mode**: `AutoLightV4 led(true);`
- **Enable serial monitoring**: `pio device monitor`
- **Test on ESP32 first**: Full feature compatibility
- **Validate I2C addresses**: Use I2C scanner before deployment

## Key Architectural Concepts

- **Progressive Enhancement**: Each version builds upon previous capabilities
- **Backward Compatibility**: V4 can import/export V3 configurations  
- **Dynamic Adaptation**: Runtime hardware detection and configuration
- **Multi-Modal Interaction**: Flexible button systems for different user needs
- **Intelligent Automation**: Auto-scanning, auto-distribution, auto-fixing
- **Production Ready**: Comprehensive validation, error recovery, health monitoring

This addon system represents the evolution of embedded LED control from simple hardcoded patterns to intelligent, self-configuring systems suitable for both educational and production environments.