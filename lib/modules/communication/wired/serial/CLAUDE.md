# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Directory Overview
Serial communication module implementations providing various types of serial communication for embedded systems development.

## Build and Development Commands

### PlatformIO Commands
```bash
# Build for active environment (ESP32 DevKit v1)
pio run

# Build for specific platforms
pio run -e esp32doit-devkit-v1
pio run -e uno                    # Arduino Uno (comment out in platformio.ini)
pio run -e nodemcuv2             # ESP8266 (comment out in platformio.ini)

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor

# Clean build artifacts
pio run --target clean
```

### Module Enable System
Enable specific serial modules via `lib/enable.h` definitions before including main header:
```cpp
#define ENABLE_MODULE_SERIAL_ENHANCED  // Enhanced serial with buffering and error handling
#define ENABLE_MODULE_SERIAL_HARD      // Hardware serial wrapper
#define ENABLE_MODULE_SERIAL_NEMA      // NMEA-like packet protocol with CRC
#define ENABLE_MODULE_SERIAL_SOFT      // Software serial (not available on ESP32)
#define ENABLE_MODULE_SERIAL_SWAP      // Hardware serial with custom pin mapping (ESP32/ESP8266)
#include "Kinematrix.h"
```

## Architecture Overview

### Serial Module Hierarchy
Five distinct serial communication implementations, each optimized for specific use cases:

#### 1. **EnhancedSerial** (`enhanced-serial.h/cpp`)
- **Purpose**: Production-grade serial communication with advanced features
- **Key Features**:
  - Stream interface inheritance for compatibility
  - Circular buffer implementation with configurable size
  - Comprehensive error handling (timeout, buffer overflow, invalid data)
  - Echo mode for debugging
  - Asynchronous operations with callbacks
  - Template-based data formatting
  - String parsing utilities with data extraction
- **Memory**: Dynamic buffer allocation (default 256 bytes, configurable)
- **Platform**: All platforms (ESP32, ESP8266, AVR)

#### 2. **HardSerial** (`hard-serial.h/cpp`)
- **Purpose**: Simple hardware serial wrapper with basic functionality
- **Key Features**:
  - Direct HardwareSerial pointer wrapping
  - Template-based data appending with separators
  - Synchronous and asynchronous send/receive operations
  - String parsing with separator-based indexing
- **Memory**: Minimal overhead, String-based data accumulation
- **Platform**: All platforms with hardware serial support

#### 3. **NemaSerial** (`nema-serial.h/cpp`)
- **Purpose**: Robust packet-based communication protocol with NMEA-like structure
- **Key Features**:
  - Binary packet protocol with start/end markers and byte stuffing
  - CRC16 error detection and acknowledgment system
  - JSON data support with ArduinoJson integration
  - Type-safe data serialization (int8/16/32, uint8/16/32, float, string, bool)
  - Platform-specific buffer sizing (AVR: 64B, ESP8266: 256B, ESP32: 512B)
  - Comprehensive debugging with configurable verbosity levels
  - Statistical tracking (packets sent/received, errors, retries)
- **Memory**: Adaptive buffer management based on platform capabilities
- **Platform**: All platforms with platform-optimized memory usage

#### 4. **SoftSerial** (`soft-serial.h/cpp`)
- **Purpose**: Software-based serial communication for additional ports
- **Key Features**:
  - SoftwareSerial library wrapper
  - Compatible API with HardSerial for easy switching
  - Pin-based initialization support
  - **Platform Limitation**: Not compiled on ESP32 (conditional compilation)
- **Memory**: Minimal overhead
- **Platform**: AVR, ESP8266 (ESP32 excluded via preprocessor)

#### 5. **SwapSerial** (`swap-serial.h/cpp`)
- **Purpose**: Hardware serial with custom pin assignment
- **Key Features**:
  - Custom RX/TX pin mapping for ESP32/ESP8266
  - GPIO table documentation for pin selection
  - Configurable serial parameters (baud, config, pins)
  - Extended timeout control
  - **Platform Limitation**: ESP32/ESP8266 only (conditional compilation)
- **Memory**: Standard HardwareSerial overhead
- **Platform**: ESP32, ESP8266 only

### Platform-Specific Architecture

#### ESP32 Platform
- **Available Modules**: EnhancedSerial, HardSerial, NemaSerial, SwapSerial
- **Memory**: 512KB buffer for NemaSerial, 128-byte chunks
- **Hardware**: Multiple hardware serial ports (Serial, Serial1, Serial2)
- **GPIO Flexibility**: Custom pin mapping via SwapSerial

#### ESP8266 Platform  
- **Available Modules**: EnhancedSerial, HardSerial, NemaSerial, SoftSerial, SwapSerial
- **Memory**: 256KB buffer for NemaSerial, 64-byte chunks
- **Hardware**: Limited hardware serial ports
- **Software Serial**: Available for additional ports

#### AVR (Arduino Uno/Nano) Platform
- **Available Modules**: EnhancedSerial, HardSerial, NemaSerial, SoftSerial
- **Memory**: 64KB buffer for NemaSerial, 16-byte chunks (highly optimized)
- **Hardware**: Single hardware serial port (Serial)
- **Software Serial**: Essential for multiple serial communications

### Common Design Patterns

#### Template-Based Data Handling
All modules use template methods for type-agnostic data operations:
```cpp
template<typename T>
void addData(T newData, const char* separator = ";");

template<typename T>
void sendString(T newData);
```

#### Callback-Driven Architecture
Asynchronous operations use function pointer callbacks:
```cpp
void sendDataCb(void (*onReceive)(const String&));
void receiveAsync(uint32_t interval, void (*onReceive)(const String&));
```

#### String-Based Data Parsing
Common parsing utilities across all modules:
```cpp
String parseStr(String data, char separator[], int index);
float getData(String data, uint8_t index, char separator[]);
String getStrData(String data, uint8_t index, char separator[]);
```

## Key Implementation Details

### Buffer Management (EnhancedSerial)
- Circular buffer implementation prevents data loss
- Dynamic allocation with error handling
- Buffer overflow detection and recovery
- Configurable buffer sizes based on application needs

### Packet Protocol (NemaSerial) 
- Frame structure: `[START][SEQUENCE][COMMAND][LENGTH][DATA...][CRC][END]`
- Byte stuffing for data transparency (0x7E/0x7F markers, 0x7D escape)
- Command set includes: ACK/NAK, PING, DATA, REGISTER_READ/WRITE, JSON_DATA
- Automatic retry mechanism with configurable limits

### Platform Detection
Conditional compilation ensures optimal resource usage:
```cpp
#if defined(ESP32)
    // ESP32-specific implementations
#elif defined(ESP8266) 
    // ESP8266-specific implementations
#elif defined(ARDUINO_AVR_UNO)
    // AVR-specific optimizations
#endif
```

### Error Handling Hierarchy
- **EnhancedSerial**: Enum-based error codes with string conversion
- **NemaSerial**: Comprehensive error tracking with statistics
- **Other Modules**: Basic error handling via return values

## Usage Patterns

### Basic Communication (HardSerial)
```cpp
HardSerial serial;
serial.begin(&Serial, 9600);
serial.addData(sensorValue);
serial.addData(timestamp);
serial.sendData();
```

### Advanced Communication (EnhancedSerial)
```cpp
EnhancedSerial enhanced(512);  // 512-byte buffer
enhanced.begin(&Serial1, 2000); // 2-second timeout
enhanced.setEcho(true);
enhanced.sendDataCbWaitDataWithTimeout(nullptr, 5000, 3);
```

### Packet Communication (NemaSerial)
```cpp
NemaSerial nema;
nema.begin(&Serial2, 256);
nema.beginPacket()
    .writeFloat(temperature)
    .writeString("sensor_01")
    .writeBool(alertStatus)
    .endPacket(CMD_DATA);
```

### Platform-Specific Usage (SwapSerial - ESP32 only)
```cpp
SwapSerial swap;
swap.begin(&Serial2, 115200, SERIAL_8N1, 16, 17); // Custom RX/TX pins
```

## Development Workflow

### Testing Serial Modules
1. **Hardware Setup**: Connect appropriate serial devices or use loopback for testing
2. **Platform Selection**: Uncomment target platform in `platformio.ini`
3. **Module Enable**: Define required modules in test sketch or `lib/enable.h`
4. **Monitor Output**: Use `pio device monitor` to observe serial communication

### Debugging Serial Communication
- **EnhancedSerial**: Enable echo mode and error checking
- **NemaSerial**: Set debug level and use statistics tracking
- **All Modules**: Use callback functions to monitor data flow

### Memory Optimization
- Choose appropriate buffer sizes for target platform
- Use SoftSerial on AVR when multiple ports needed
- Leverage NemaSerial's adaptive buffering for complex protocols

### Cross-Platform Development
- Test on multiple platforms (ESP32 for full features, AVR for resource constraints) 
- Use conditional compilation for platform-specific features
- Validate buffer sizes don't exceed platform limitations

## Integration with Kinematrix Framework

These serial modules integrate with the broader Kinematrix ecosystem:
- **Sensor Framework**: Serial data acquisition from remote sensors
- **Control Modules**: Serial communication for actuator control
- **WiFi Modules**: Serial-to-WiFi gateways and data forwarding
- **Debug Modules**: Enhanced serial debugging capabilities
- **File Modules**: Serial data logging to EEPROM/SD cards

The modular enable system allows precise selection of required serial functionality while maintaining optimal memory usage across different microcontroller platforms.