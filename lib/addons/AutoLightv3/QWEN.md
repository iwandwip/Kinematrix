# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

AutoLight V3 is a comprehensive LED matrix control system built on the Kinematrix framework. It provides professional-grade LED pattern control with dynamic PCF8574 I2C expander management, multi-modal button systems, integrated web interface, and development tools for both hardware and software testing.

**Architecture**: ESP32-focused embedded system with Next.js 15 web interface
**Core Features**: 16 LED sequence modes, dynamic PCF distribution, PCF8574 hardware support, real-time web control
**Development Tools**: C++ testing framework, PCF8574 hardware support, CLI interface, web-based pattern builder

## Essential Development Commands

### Primary Workflow (Most Common)
```bash
# Main development command after code changes
python3 ./index.py         # From project root - syncs and builds

# WebClient development
cd WebClient && npm run dev # Next.js dev server (port 3001)

# Background process monitoring
# pnpm dev may be running - use BashOutput tool to check cf5892 status
```

### Hardware Development
```bash
# PlatformIO commands (from project root)
pio run                    # Build for ESP32
pio run --target upload    # Upload firmware
pio device monitor         # Monitor serial output

# Hardware PCF8574 testing (PCF8574 required)
# Enable PCF8574 Hardware() in code
```

### WebClient Commands
```bash
# In WebClient/ directory
npm run dev                # Development server (port 3001)
npm run build              # Production build with static export
npm run kill               # Kill server on port 3001
npm run cap:build:android  # Mobile Android build
```

## Version Evolution Summary

AutoLight has evolved through **three major versions**:

### V1 - Basic Foundation (Single Button)
- **Library**: `ENABLE_ADDONS_AUTOLIGHT_V2`
- **Button System**: Single button (`INTERRUPT_PIN = 36`)
- **Control**: Simple `led.changeMode()`
- **Web Server**: Manual setup in `Apps.ino`
- **Use Case**: Proof of concept, basic LED control

### V2 - Multi-Button Enhancement (4-Button Control)  
- **Library**: `ENABLE_ADDONS_AUTOLIGHT_V3`
- **Button System**: 4 buttons (ON/OFF/NEXT/PREV on pins 34,17,36,39)
- **Control**: Individual mode functions (`onMode()`, `offMode()`, `nextMode()`, `previousMode()`)
- **Web Server**: Manual setup in `Apps.ino`
- **Use Case**: Standard LED systems, production ready

### V3 - Professional System (This Library)
- **Library**: `ENABLE_ADDONS_AUTOLIGHT_V3`
- **Button System**: Multi-modal smart system (1/2/3/4 button + custom modes)
- **Control**: Unified `smartButtonPress()` with configurable modes
- **Web Server**: Integrated `APIServerManager` class with `ConfigManager`
- **Advanced Features**: 
  - Serial Commander CLI
  - PCF8574 Hardware support
  - Matrix visualization
  - Dynamic PCF distribution
  - Sequence mapping system
- **Use Case**: Professional systems, development platform, advanced control

## Key Architectural Improvements in V3

### 1. Smart Button System
```cpp
// V1/V2: Fixed button logic
led.changeMode();  // V1
led.nextMode();    // V2

// V3: Configurable smart system
led.setButtonMode(BUTTON_MODE_1BUTTON);  // or 2/3/4 button modes
led.smartButtonPress(button_index);      // Context-aware behavior
```

### 2. Dynamic PCF Distribution
```cpp
// V1/V2: Static configuration
config.readChannel();  // Hardware-dependent

// V3: Dynamic distribution
config.setDynamicDistribution(12, 2, 6, 6);  // 12 channels, 2 PCFs, 6 each
```

### 3. Integrated Web Management
```cpp
// V1/V2: Manual web server in Apps.ino
initServer();  // Manual setup

// V3: Integrated APIServerManager
led.enableWebServer(true);  // Automatic with ConfigManager
```

### 4. Advanced Development Tools
```cpp
// V3 exclusive features:
SerialCommander cmd;                           // Full CLI interface
PCF8574 Hardware();        // Hardware PCF8574 required
led.enableSequenceMapping(true);              // Sequence filtering
led.setActiveSequences(6, 0, 1, 4, 5, 8, 10); // Custom sequence sets
```

## Development Commands

### Primary Development Workflow (Recommended)
```bash
# From project root - Main development command after code changes
python3 ./index.py

# Check background processes (WebClient may be running)
pnpm dev          # Main Next.js server (port 3001)
# Note: Background process may be running. Check with BashOutput tool if needed.
```

### Hardware Development (Arduino/ESP32)
```bash
# Build firmware (from project root)
python3 ./index.py         # Primary method (recommended)

# Alternative PlatformIO development
pio run                    # Build for ESP32
pio run --target upload    # Upload to device
pio device monitor         # Monitor serial output

# Hardware PCF8574 testing (physical PCF8574 required)
# Enable PCF8574 Hardware() in code
```

### Web Client Development
```bash
# Navigate to WebClient directory
cd WebClient

# Start Next.js development server (primary port)
npm run dev                # Port 3001 with Turbopack

# Alternative development servers (if port conflicts)
npx next dev --turbopack -p 3002  # Port 3002
npx next dev --turbopack -p 3003  # Port 3003

# Production build and deployment
npm run build             # Build with static export
npm run start             # Start production server (port 3001)

# Mobile development (Capacitor + Android Studio)
npm run cap:add:android   # Add Android platform (one-time)
npm run cap:build:android # Build and sync for Android
npm run cap:sync          # Sync without build

# Kill development server (Linux)
npm run kill              # Kill port 3001
sudo fuser -k 3002/tcp    # Kill alternative ports if needed
```

### C++ Testing Framework
```bash
# Navigate to test directory
cd test

# Install dependencies and run tests
npm install               # Install TypeScript dependencies
npm run build            # Compile TypeScript to JavaScript

# Test LED sequences (individual or all)
npm run test -- --sequence 2    # Test specific sequence
npm run test -- --sequence 3
npm run test -- --all           # Test all sequences
npm run compare                  # Compare sequence outputs
npm run validate                 # Validate C++ code generation
```

### Development Server Management
```bash
# Check running processes
ps aux | grep node       # Check for running Node.js processes
lsof -i :3001           # Check what's using port 3001
lsof -i :3002           # Check alternative ports

# Background process monitoring (if pnpm dev is running)
# Use BashOutput tool in Claude Code to check background command output
```

### Library Development (Arduino/ESP32)
```bash
# Run the project after editing code in addons folder (from project root)
python3 ./index.py

# For PlatformIO development (from project root)
pio run                    # Build firmware
pio run --target upload    # Upload to ESP32
pio device monitor         # Monitor serial output

# Check background processes for WebClient
# Three development servers can run simultaneously:
# - Port 3001: Main development server (npm run dev)
# - Port 3002: Alternative server (npx next dev --turbopack -p 3002)  
# - Port 3003: Alternative server (npx next dev --turbopack -p 3003)
```

### Testing
```bash
# Hardware testing with PCF8574 mode
# Enable PCF8574 Hardware in code for development without hardware

# Web interface testing
# Access http://localhost:3001 for web control interface
```

## Critical Architecture Understanding

### Include System Architecture (IMPORTANT)
AutoLight V3 uses a **mixed compilation approach** that's essential for development:

```cpp
// index.h includes BOTH .h and .cpp files directly
#include "Core/Channel/BaseChannel.h"
#include "Core/Channel/BaseChannel.cpp"        // Direct .cpp inclusion
#include "Core/Channel/BaseChannelSequence.cpp"
```

**Key Rules:**
1. **Never include .cpp files** directly in Arduino sketches - only include `index.h`
2. All includes are **ESP32-guarded**: `#if defined(ESP32)`
3. **New components** must be added to both header and implementation includes in `index.h`
4. Uses traditional `#ifndef/#define/#endif` guards (not `#pragma once`)

### Core Components Architecture
```
Core/
├── Channel/                    # LED control system
│   ├── BaseChannel.h/.cpp      # Main controller (5.8KB header, 33KB impl)
│   ├── BaseChannelSequence.cpp # 16 LED sequence modes (107KB)
├── Config/                    # Configuration management
│   ├── BaseConfig.h/.cpp      # Dynamic PCF distribution
│   ├── ConfigManager.h/.cpp   # Singleton configuration
│   └── ConfigData.h           # Data structures and enums
├── Common/PCF8574 Hardware.h/.cpp   # Hardware simulation for development
├── SerialCommander.h/.cpp     # Full CLI interface (32KB impl)
└── ButtonInterrupt.h/.cpp     # Multi-modal button system

API/APIServerManager.h/.cpp     # Integrated web server
WebClient/                     # Next.js 15 + React 19 interface
```

### Key Manager Classes
- **ConfigManager**: Singleton for runtime configuration
- **APIServerManager**: Integrated web server with RESTful API
- **PCF8574 Hardware**: Hardware simulation for development
- **BaseChannel**: Main LED controller with 16+ sequence modes

### Key Data Structures
```cpp
// PCF Distribution for dynamic I2C expansion
struct PCFDistribution {
    uint8_t address;         // I2C address (0x20-0x27)
    uint8_t used_pins;       // Allocated pins (1-8)
    uint8_t start_channel;   // Starting channel number
    uint8_t pin_offset;      // Pin offset within PCF
    bool is_active;          // PCF status
};

// Multi-modal button configuration
typedef enum {
    BUTTON_MODE_4BUTTON = 0,  // ON/OFF/NEXT/PREV
    BUTTON_MODE_1BUTTON = 1,  // Single cycle button
    BUTTON_MODE_2BUTTON = 2,  // Toggle + Next
    BUTTON_MODE_3BUTTON = 3,  // Separate ON/OFF/Next
    BUTTON_MODE_CUSTOM = 99   // User-defined
} button_mode_t;
```

## Development Philosophy & Best Practices

- **No Comments**: Keep code clean without docstrings or comments (per project guidelines)
- **Hardware Abstraction**: Use PCF8574 Hardware simulation for development without physical PCF8574s
- **ESP32 Primary**: Main target platform with Arduino framework compatibility
- **Background Processes**: `pnpm dev` may be running - check with BashOutput tool (process cf5892)

## Quick Start Patterns

### Basic Integration with Kinematrix
```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V3  // Enable AutoLight V3 addon
#include "Kinematrix.h"             // Main Kinematrix header
using namespace AutoLight;

BaseChannel led;
BaseConfig config;

void setup() {
    Serial.begin(115200);
    config.setDynamicConfig(12, 2);  // 12 channels, 2 PCF8574s
    led.attachConfig(config.getConfigData());
    led.initialize();
    led.enableWebServer(true);       // Enable integrated API server
    led.setButtonMode(BUTTON_MODE_1BUTTON);
}

void loop() {
    led.runAutoLight();
    delay(10);
}
```

### Development Mode (Virtual Hardware)
```cpp
void setup() {
    PCF8574 Hardware();  // No hardware required
    led.enableSerialCommander(true);        // CLI interface
}
```

## Key Implementation Notes

### Recent Architectural Improvements

**Major Library Restructuring (Latest Update)**:
1. **Fixed Compilation Issues**: Resolved `struct Credentials` redefinition errors between `ConfigData.h` and `WebManager.h`
2. **Include System Overhaul**: Cleaned up circular dependencies and multiple definition issues in `index.h`
3. **Proper Header/Implementation Separation**: Restored standard C++ practices with clean `.h`/`.cpp` separation
4. **Include Guard Standardization**: Consistent `#ifndef/#define/#endif` guards for universal compiler compatibility

**WebClient Technology Stack**: **Fully Modernized**
- **Frontend**: Vue 3 → **Next.js 15** + **React 19**
- **UI Library**: shadcn-vue → **shadcn/ui** + **Radix UI**
- **State Management**: VueUse → **React Context** + **Custom Hooks**  
- **Build System**: Vite → **Turbopack** (Next.js 15)
- **Mobile**: **Capacitor** integration for Android/iOS

**API Polling Enhancements (Latest)**:
- **Concurrent Fetching**: `Promise.allSettled()` for parallel API calls
- **Deep Equality Checks**: Prevents unnecessary re-renders in React components
- **Enhanced Error Handling**: Graceful degradation with fallback values
- **Dynamic Config Updates**: Real-time API configuration changes without polling interruption

**Migration Status**: **100% COMPLETE** with enhanced performance and type safety

### Include Architecture - Critical Understanding

This library uses a **mixed compilation approach** that's essential to understand for development:

#### Direct .cpp Inclusion Pattern
```cpp
// In index.h - Both header and implementation files are included
#include "Core/Channel/BaseChannel.h"
#include "Core/Channel/BaseChannel.cpp"        // Direct .cpp inclusion
#include "Core/Channel/BaseChannelSequence.cpp" // Implementation file
// Pattern system removed - using direct BaseChannel LED sequences
```

#### Platform-Specific Compilation
```cpp
// All includes wrapped in ESP32 guard
#if defined(ESP32)
// All AutoLight V3 includes here...
#endif
```

**Why This Architecture?**
- **Single Compilation Unit**: Enables header-only library behavior while maintaining file separation
- **ESP32 Exclusive**: Platform guard ensures compilation only on ESP32 targets
- **Kinematrix Integration**: Allows conditional inclusion via `ENABLE_ADDONS_AUTOLIGHT_V3`
- **Development Flexibility**: Maintains separate .h/.cpp files for IDE support and debugging

**Important for Developers**:
1. **Never include .cpp files directly** in your Arduino sketch - only include `index.h`
2. **All modifications** should maintain the .h/.cpp separation pattern
3. **New components** must be added to both header and implementation includes in `index.h`
4. **Compilation errors** often indicate missing includes in the `index.h` file

**Header Guards**: Uses traditional `#ifndef/#define/#endif` guards for universal compiler compatibility rather than `#pragma once`.

This architecture enables the AutoLight V3 system to function as a comprehensive header-only library while maintaining clean development practices.

## Migration Information

### From V1/V2 to V3
Developers upgrading from earlier versions should refer to:
- `PERBANDINGAN_VERSI.md` - Comprehensive version comparison
- `PANDUAN_TASKSEQUENCE.md` - TaskSequence development guide
- Example implementations in `src/AutoLightScriptv1/`, `src/AutoLightScriptv2/`, `src/AutoLightScriptv3/`

### Key Migration Benefits
- **Reduced Hardware Complexity**: Single button can handle multiple modes
- **Enhanced Debugging**: PCF8574 Hardware mode for development with hardware
- **Professional CLI**: Serial Commander for advanced control and diagnostics
- **Flexible Configuration**: Dynamic PCF distribution eliminates manual setup
- **Better User Experience**: Smart button system adapts to user preferences

## Architecture Components

AutoLight V3 implements a comprehensive architecture with the following core components:

### Core System
- **BaseChannel**: Main LED controller with 16+ sequence modes
- **BaseConfig**: Dynamic configuration with PCF distribution strategies
- **ButtonInterrupt**: Multi-modal button system with hardware interrupt support

### Advanced Features (V3 Exclusive)
- **APIServerManager**: Integrated web server with ConfigManager singleton
- **SerialCommander**: Full CLI interface with multi-level access control
- **PCF8574 Hardware**: Hardware simulation for development and testing
- **TaskHandle**: Enhanced FreeRTOS task management

### Development Tools
- **PCF8574 Hardware Mode**: Test with hardware PCF8574 expanders
- **Serial Commander**: Professional debugging and control interface
- **Sequence Mapping**: Filter and reorder available LED sequences

## Common Development Patterns

### Library Integration with Kinematrix
```cpp
// In your Arduino sketch or main code
#define ENABLE_ADDONS_AUTOLIGHT_V3  // Enable AutoLight V3 addon
#include "Kinematrix.h"             // Main Kinematrix header
using namespace AutoLight;          // Use AutoLight namespace
```

### Basic Usage Pattern
```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V3
#include "Kinematrix.h"
using namespace AutoLight;

BaseChannel led;
BaseConfig config;

void setup() {
    Serial.begin(115200);

    // Dynamic PCF configuration (channels, PCF count)
    config.setDynamicConfig(12, 2);  // 12 channels across 2 PCF8574s
    led.attachConfig(config.getConfigData());
    led.initialize();

    // Enable integrated web server with API endpoints
    led.enableWebServer(true);

    // Configure button interaction mode
    led.setButtonMode(BUTTON_MODE_1BUTTON);  // Single button cycles through modes

    // Enable Serial Commander CLI (optional)
    led.enableSerialCommander(true);

    Serial.println("AutoLight V3 initialized");
}

void loop() {
    led.runAutoLight();  // Main AutoLight processing loop
    delay(10);           // Small delay for stability
}
```

### Advanced Configuration Pattern
```cpp
// For development and debugging
void setup() {
    Serial.begin(115200);

    // Enable PCF8574 hardware mode for hardware testing
    PCF8574 Hardware();

    // Advanced PCF distribution configuration
    config.setDistributionStrategy(DISTRIBUTE_OPTIMIZED);
    config.setPCFAddresses({0x27, 0x38, 0x45});  // Custom I2C addresses
    config.distributeChannels(24, 3);             // 24 channels, 3 PCFs

    // Advanced features
    led.enableSequenceMapping(true);                    // Enable sequence filtering
    led.setActiveSequences({1, 2, 5, 8, 10});          // Custom sequence set
    led.setButtonMode(BUTTON_MODE_4BUTTON);             // Classic 4-button mode

    led.attachConfig(config.getConfigData());
    led.initialize();

    Serial.println("AutoLight V3 Advanced Mode");
}
```

### Development Features
- **PCF8574 Hardware Mode**: Test with hardware using `PCF8574 Hardware`
- **Serial Commander**: Full CLI access for debugging and control
- **Web Interface**: Real-time control via Next.js application with Turbopack
- **Sequence Mapping**: Filter and customize available LED patterns
- **Hot Reload**: Instant development updates for both web and firmware changes
- **Mobile Ready**: Capacitor integration for native mobile deployment

### Current Development Status
- **Core Library**: ✅ All components integrated and compilation-ready with ESP32 platform guard
- **WebClient**: ✅ Next.js 15 + React 19 fully operational (port 3001 primary, 3002/3003 available)
- **API Integration**: ✅ RESTful API endpoints with AutoLight hardware via APIServerManager
- **Documentation**: ✅ Comprehensive guides including migration, WebClient specific docs, and usage patterns
- **Testing**: ✅ PCF8574 Hardware mode for hardware development
- **shadcn Integration**: ✅ Root package.json with shadcn v3.0.0 for UI component management

## API Endpoints

The integrated web server provides RESTful API endpoints:

```
GET /api/v1/data/get/device/ch      # Get channel count
GET /api/v1/data/get/mode           # Get current LED mode (0-15)
GET /api/v1/data/get/delay          # Get timing delay (30-300ms)
GET /api/v1/data/get/device/name    # Get device name
GET /api/v1/data/get/device/serial  # Get device serial number

GET /api/v1/data/set/mode?value={mode}        # Set LED mode
GET /api/v1/data/set/delay?value={delay}      # Set timing delay
GET /api/v1/data/set/device/name?value={name} # Set device name
```

## Hardware Requirements and Compatibility

### Supported Platforms
- **Primary**: ESP32 (all variants) - Full feature support
- **Secondary**: ESP8266 - Limited functionality (no FreeRTOS features)
- **Limited**: Arduino AVR - Basic LED control only

### Hardware Components
- **PCF8574**: I2C I/O expanders for LED control (up to 8 units, addresses 0x20-0x27)
- **LEDs**: Configurable channel count (1-64 channels supported)
- **Buttons**: 1-4 button configurations with interrupt support
- **Status LEDs**: RGB LED + Buzzer for system feedback

### Memory Usage
- **Minimal Configuration**: ~100KB flash, ~20KB RAM
- **Full Configuration**: ~300KB flash, ~50KB RAM
- **WebClient Build**: ~2MB static files for web interface

### Integration Points
- **Kinematrix Framework**: Uses `ENABLE_ADDONS_AUTOLIGHT_V3` flag
- **Web API**: HTTP endpoints for remote control
- **FreeRTOS**: Task management for ESP32 platform
- **I2C Communication**: PCF8574 expander management

## Common Development Tasks

### Adding New LED Sequences
1. Implement sequence in `BaseChannelSequence.cpp` (107KB file with 16 modes)
2. Add sequence mapping in `BaseChannel.cpp`
3. Test with PCF8574 Hardware simulation before hardware
4. Add to `index.h` if creating new pattern files

### Web Interface Development
1. **Start Development**: `cd WebClient && npm run dev` (port 3001)
2. **API Integration**: Update `WebClient/lib/api/device.ts` for new endpoints
3. **Components**: Modify `WebClient/components/` for UI changes
4. **Production Build**: `npm run build` creates static export in `out/`

### Hardware Configuration
1. **Hardware Testing**: Connect `PCF8574 Hardware` first
2. **PCF Distribution**: Use `config.setDynamicConfig(channels, pcf_count)`
3. **I2C Addresses**: Default 0x20-0x27, customize with `config.setPCFAddresses()`
4. **Serial Debug**: Use SerialCommander CLI (32KB implementation)

### Background Process Management
```bash
# Check if pnpm dev is running (common development scenario)
ps aux | grep pnpm
# Monitor background output if needed
# Use BashOutput tool to check cf5892 process status
```

## Troubleshooting Common Issues

### Compilation Issues
```bash
# Missing includes in index.h
# Solution: Ensure both .h and .cpp files are included for new components
# Check that all includes are within #if defined(ESP32) guard

# Redefinition errors
# Solution: Check for duplicate struct definitions (e.g., struct Credentials)
# Verify proper header guards are in place

# Platform compilation errors
# Solution: Ensure ESP32 is selected as target platform in Arduino IDE or PlatformIO
```

### WebClient Issues
```bash
# Port conflicts (port 3001 in use)
npm run kill              # Kill existing server
sudo fuser -k 3001/tcp   # Force kill if needed

# API connection failures
# Check AutoLight device IP (default: 192.168.4.1:8000)
# Verify WiFi connection to AutoLight AP or same network
# Test API endpoints manually: curl http://192.168.4.1:8000/api/v1/data/get/mode

# Build failures
rm -rf WebClient/.next WebClient/out  # Clear Next.js cache
npm run build                         # Rebuild
```

### Hardware Development Issues
```bash
# PCF8574 hardware testing (hardware required)
# Enable PCF8574 Hardware() in code
# Check serial output for PCF8574 hardware status messages

# I2C scanner for debugging
# Use Kinematrix I2C scanner module to detect PCF8574 addresses
# Default addresses: 0x20-0x27

# Serial Commander access
# Open serial monitor at 115200 baud
# Type "help" for CLI commands
# Type "status" for system information
```

### Background Process Management
```bash
# Check if pnpm dev is running in background (common scenario)
ps aux | grep pnpm
ps aux | grep next

# Monitor background output (use BashOutput tool for cf5892)
npm run kill              # Kill server on port 3001 if needed
```

## API Endpoints (Hardware Communication)

AutoLight V3 provides RESTful API endpoints for web interface integration:

```
Device Information:
GET /api/v1/data/get/device/ch      # Channel count
GET /api/v1/data/get/device/name    # Device name
GET /api/v1/data/get/device/serial  # Serial number

LED Control:
GET /api/v1/data/get/mode           # Current mode (0-15)
GET /api/v1/data/get/delay          # Timing delay (30-300ms)
GET /api/v1/data/set/mode?value=N   # Set LED mode
GET /api/v1/data/set/delay?value=N  # Set timing delay
```

**Default Connection**: `192.168.4.1:8000` (AutoLight AP mode)

This AutoLight V3 system provides a complete professional LED control ecosystem with modern web interface integration, hardware simulation capabilities, and comprehensive development tools.