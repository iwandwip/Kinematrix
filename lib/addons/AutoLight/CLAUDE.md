# CLAUDE.md - AutoLight V1 Library

This file provides guidance to Claude Code (claude.ai/code) when working with the AutoLight V1 library code.

## Library Overview

AutoLight V1 is the original LED matrix control library within the Kinematrix framework. This is the foundational addon that established the core architecture for automotive lighting control systems using the "Keed" naming convention.

## Library Structure

### Core Components

- **KeedAutoLight.h/.cpp**: Main LED control and sequencing engine
- **KeedBase.h**: Base definitions and common structures
- **AutoLight.h**: Library entry point and include orchestration

### Module Organization

#### AutoLightChannel/
- **KeedBaseChannel.h/.cpp**: Core channel management and LED control
- **KeedBaseChannelTask.cpp**: Task-based LED sequencing implementation

#### AutoLightConfig/
- **KeedConfig.h/.cpp**: Configuration management system
- **KeedConfigExt.cpp**: External configuration extensions
- **KeedConfigWiFi.cpp**: WiFi-specific configuration handling
- **KeedCustom.h**: Custom configuration definitions
- **KeedDef.h**: Core definitions and constants
- **KeedDisplay.h/.cpp**: Display management for configuration
- **KeedDisplayPos.h**: Display positioning utilities
- **KeedIndex.h**: Index and enumeration definitions

#### AutoLightStrobe/
- **Keed3ChannelStrobe.h/.cpp**: 3-channel strobe light implementation

## Architecture Patterns

### Naming Convention
The library uses the "Keed" prefix throughout, establishing a consistent naming pattern:
- **KeedAutoLight**: Main control class
- **KeedConfiguration**: Configuration management
- **KeedBaseChannel**: Channel control base class

### Configuration System
- **Hardware Detection**: Pin-based board version and channel detection
- **EEPROM Storage**: Persistent configuration storage
- **Custom Sequences**: Flexible pin mapping for different board layouts
- **WiFi Integration**: Network configuration capabilities

### Channel Management
- **I2C Expansion**: PCF8574 I/O expander support
- **Sequence Control**: Mode-based LED pattern management
- **Task-Based Execution**: FreeRTOS task integration for LED control
- **Strobe Support**: Specialized 3-channel strobe functionality

## Key Features

### 1. Modular Design
- **Conditional Compilation**: Include only required components
- **Extensible Architecture**: Easy addition of new features
- **Platform Abstraction**: ESP32-focused with portable design

### 2. Hardware Abstraction
- **I2C Communication**: Standardized I2C expander interface
- **Pin Mapping**: Flexible channel-to-pin assignment
- **Board Detection**: Automatic hardware configuration

### 3. Configuration Management
- **Multiple Sources**: Hardware detection, EEPROM, manual configuration
- **Validation**: Configuration validation and error handling
- **Display Integration**: LCD/OLED configuration display support

## Usage Patterns

### Basic Initialization
```cpp
#include "AutoLight.h"

KeedConfiguration cfg;
KeedAutoLight led;

void setup() {
    configuration_t config;
    config.sequence = 0;
    config.setDelay(50);
    config.reverse = true;
    
    cfg.setConfig(config);
    cfg_error_t err_ch = cfg.readChannel();
    cfg_error_t err_ver = cfg.readVersion();
    
    led.setConfiguration(&cfg);
    led.begin();
}
```

### Custom Configuration
```cpp
// Custom pin sequence
config.channel = 24;
config.setPinSequence(24, 
    1,2,4,5,6,7,8,9,           // Row 1
    10,11,12,3,22,13,14,15,    // Row 2  
    16,17,18,19,20,21,23,24    // Row 3
);
```

### I2C Expansion
```cpp
// I2C mode with automatic detection
#define KEED_I2C
config.reverse = true;

// Custom I2C configuration
#define KEED_I2C_CUSTOM
config.custom = true;
```

## Development Guidelines

### Code Organization
- **Separation of Concerns**: Each module handles specific functionality
- **Clear Dependencies**: Minimal coupling between modules
- **Consistent Interfaces**: Standardized method signatures

### Memory Management
- **Static Allocation**: Prefer static allocation for embedded systems
- **EEPROM Efficiency**: Optimized EEPROM usage patterns
- **Resource Cleanup**: Proper resource management

### Error Handling
- **Error Codes**: Standardized error reporting via `cfg_error_t`
- **Validation**: Input validation at configuration boundaries
- **Graceful Degradation**: Fallback behaviors for error conditions

## Integration Points

### Kinematrix Framework
- **Enable Flag**: `ENABLE_ADDONS_AUTOLIGHT`
- **Include Path**: `lib/addons/AutoLight.h`
- **Namespace**: Uses global namespace with Keed prefix

### Hardware Support
- **ESP32 Primary**: Optimized for ESP32 platform
- **I2C Protocol**: PCF8574 I/O expander standard
- **GPIO Interface**: Direct GPIO pin control support

### Extension Points
- **Custom Sequences**: User-defined LED patterns
- **Display Integration**: LCD/OLED configuration display
- **WiFi Configuration**: Network-based configuration
- **Strobe Modules**: Specialized lighting effects

## Legacy Status

This library represents the foundation of the AutoLight ecosystem:
- **Historical Significance**: First version of AutoLight architecture
- **Pattern Establishment**: Established naming and architectural patterns
- **Backward Compatibility**: Maintained compatibility in later versions

The patterns and architecture established in this library directly influenced the development of AutoLightv2 and AutoLightv3, making it a crucial component of the Kinematrix lighting ecosystem.

## Key Architectural Concepts

- **Keed Ecosystem**: Consistent naming and architectural patterns
- **Configuration-Driven**: Hardware-agnostic configuration system
- **Modular Expansion**: Easy addition of new lighting modules
- **Task Integration**: FreeRTOS task-based execution model
- **I2C Standardization**: Consistent I2C expansion interface

This library serves as the foundation for all subsequent AutoLight development and establishes the core patterns used throughout the Kinematrix lighting system.