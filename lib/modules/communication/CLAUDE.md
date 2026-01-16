# CLAUDE.md - Communication Modules

## Directory Overview
Communication protocol implementations for wired and wireless connectivity in embedded systems.

## Structure
- **`wired/`** - Physical communication protocols (I2C, SPI, Serial, Modbus)
- **`wireless/`** - Wireless communication systems (LoRa, ESP-NOW)

## Wired Communication

### I2C Systems (`wired/i2c/`)
- **`i2c-scanner.h/cpp`** - I2C device detection and scanning
- **`i2c-expander.h/cpp`** - I2C expansion capabilities
- **`io-expander.h/cpp`** - General I/O expansion over I2C

### Serial Communication (`wired/serial/`)
- **`hard-serial.h/cpp`** - Hardware serial communication
- **`soft-serial.h/cpp`** - Software serial implementation
- **`swap-serial.h/cpp`** - Serial port swapping functionality
- **`enhanced-serial.h/cpp`** - Enhanced serial with advanced features
- **`nema-serial.h/cpp`** - NMEA protocol support for GPS/navigation

### SPI Communication (`wired/spi/`)
- **`spi-serial.h/cpp`** - SPI-based serial communication
- **`dot-matrix.h/cpp`** - SPI dot matrix display control

### Modbus Protocol (`wired/modbus/`)
Industrial communication protocol implementation:
- **`modbus.h`** - Main Modbus definitions
- **`modbusDevice.h/cpp`** - Modbus device abstraction
- **`modbusRegBank.h/cpp`** - Register bank management
- **`modbusSlave.h/cpp`** - Modbus slave implementation

## Wireless Communication

### LoRa Long-Range Communication (`wireless/lora/`)
Multiple LoRa implementations for different use cases:
- **`lora-base.h/cpp`** - Basic LoRa communication
- **`lora-com.h/cpp`** - Enhanced LoRa communication
- **`lora-comv2.h/cpp`** - Version 2 with improved features
- **`lora-ebyte-e220.h/cpp`** - EByte E220 specific implementation

### ESP-NOW (`wireless/now/`)
- **`esp-now.h/cpp`** - ESP-NOW mesh networking for ESP32/ESP8266

## Key Features

### I2C Scanner
```cpp
I2CScanner scanner;
scanner.beginTransmission();
scanner.scanI2CAddress();  // Detects connected I2C devices
scanner.endTransmission();
```

### Modbus Communication
- RTU and ASCII protocol support
- Master and slave implementations
- Register bank management
- Industrial-grade reliability

### LoRa Features
- Long-range communication (up to 10km)
- Multiple frequency bands
- Configurable power levels
- Packet acknowledgment
- EByte E220 module support

### ESP-NOW Features
- Peer-to-peer communication
- Mesh networking capabilities
- Low latency (<5ms)
- No WiFi infrastructure required

## Protocol Characteristics

| Protocol | Range | Speed | Power | Use Case |
|----------|-------|-------|-------|----------|
| I2C | <1m | 400kHz | Low | Sensors, displays |
| SPI | <1m | 10MHz+ | Low | High-speed peripherals |
| Serial | <100m | 115200+ | Low | Debug, GPS, modems |
| Modbus | <1000m | 9600-115200 | Medium | Industrial control |
| LoRa | 10km+ | 0.3-50kbps | Low | IoT, remote sensing |
| ESP-NOW | 200m | 1Mbps | Medium | Mesh networks |

## Platform Support
- **ESP32**: All protocols including WiFi-based and LoRa
- **ESP8266**: WiFi protocols, basic wired communication
- **AVR**: Wired protocols (I2C, SPI, Serial), basic LoRa

## Usage Patterns
```cpp
// Enable specific communication modules
#define ENABLE_MODULE_I2C_SCANNER
#define ENABLE_MODULE_LORA_COM_V2
#define ENABLE_MODULE_MODBUS_SLAVE
#include "Kinematrix.h"
```

## Applications
- Industrial automation (Modbus)
- IoT sensor networks (LoRa)
- Mesh networking (ESP-NOW)
- Device expansion (I2C/SPI)
- Data logging and debug (Serial)
- GPS navigation (NMEA)