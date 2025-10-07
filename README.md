# Kinematrix Beta

**v0.0.28** - Advanced Modular Arduino Library Framework for IoT and Embedded Systems Development

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-ESP32%20%7C%20ESP8266%20%7C%20AVR-blue.svg)](https://platformio.org/)
[![Version](https://img.shields.io/badge/version-0.0.28%20beta-orange.svg)](https://github.com/iwandwip/Kinematrix)

## 🚀 Overview

Kinematrix Beta is a sophisticated, modular Arduino library framework designed to dramatically simplify IoT and embedded systems development. It provides a unified, abstracted API for managing sensors, communication protocols, displays, control algorithms, file systems, and hardware components across multiple microcontroller platforms.

### Core Philosophy
- **🔧 Modular Architecture**: Conditional compilation system allowing selective inclusion of only required components
- **🌐 Platform Agnostic**: Unified API supporting ESP32, ESP8266, and AVR-based Arduino boards
- **📚 Educational Focus**: Designed for both beginners and professionals with comprehensive examples
- **⚡ Production Ready**: Includes advanced features like calibration, filtering, alerts, and data persistence

## ✨ Key Features

### 🎯 AutoLight Ecosystem (v1-v4)
Four generations of LED control evolution with backward compatibility:
- **AutoLight V3** (Current): Dynamic PCF8574 distribution, multi-modal buttons, web integration
- **JavaScript Simulator Mode**: Revolutionary microsecond-accurate browser-based LED pattern development
- **Pattern Builder**: Professional desktop-only LED development tool with 4 simulation modes
- **WebClient**: Next.js 15 + React 19 interface with REST API

### 🔬 Dual Sensor Framework
- **SensorModuleV1**: Traditional callback-based system (40+ sensors)
- **SensorModuleV2**: Modern type-safe framework with real-time filtering, alerts, and calibration (14+ sensors)

### 📡 Communication Modules (15+)
- **Wired**: I2C expansion, Modbus RTU/ASCII, enhanced Serial variants, SPI
- **Wireless**: LoRa (multiple variants), ESP-NOW mesh networking
- **Internet**: WiFi management, MQTT, Firebase (3 versions), cloud integrations

### 🎛️ Control Algorithms (7+)
- PID controllers with auto-tuning (Ziegler-Nichols, Cohen-Coon)
- Fuzzy Logic implementations (Mamdani, Sugeno, Tsukamoto)
- Machine Learning (KNN classification, Decision Trees)

## 🏗️ Architecture

### Modular Conditional Compilation
Optimize memory usage by including only required components:

```cpp
// Enable specific modules before including main header
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_MODULE_WIFI_HANDLER_V2
#define ENABLE_MODULE_PID_CONTROLLER
#define ENABLE_MODULE_MQTT_MANAGER
#include "Kinematrix.h"
```

### Memory Optimization Strategy
- **Minimal setup**: ~50KB (basic sensors only)
- **Standard setup**: ~200KB (sensors + basic modules)
- **Full-featured setup**: ~500KB+ (all modules enabled)

### Triple Include System
- **KinematrixModules.h**: Production modules (.h + .cpp)
- **KinematrixModulesHelper.h**: Development helpers (.h + .cpp)
- **KinematrixModulesNoDef.h**: Header-only modules (.h only)

## 🛠️ Quick Start

### Installation
1. Clone this repository to your Arduino libraries folder
2. Open PlatformIO or Arduino IDE
3. Include the library in your project

### Basic Usage Examples

#### Environmental Monitoring

```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_BME680_V2
#include "Kinematrix.h"

SensorModuleV2 sensors;
BME680SensV2 environmental;

void setup() {
    Serial.begin(115200);
    environmental.setUpdateInterval(5000);
    sensors.addSensor("env", &environmental);
    sensors.init();
}

void loop() {
    sensors.update();
    Serial.printf("Temperature: %.2f°C\n", environmental.getTemperature());
    delay(1000);
}
```

#### AutoLight V3 LED Control

```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V3
#include "Kinematrix.h"
using namespace AutoLight;

BaseChannel led;
BaseConfig config;
SerialCommander cmd;

void setup() {
    Serial.begin(115200);

    // Dynamic PCF configuration
    config.setDynamicConfig(12, 2);  // 12 channels, 2 PCF8574s
    led.attachConfig(config.getConfigData());
    led.initialize();

    // Enable integrated systems
    led.enableWebServer(true);       // REST API on port 8000
    led.setButtonMode(BUTTON_MODE_1BUTTON);

    // CLI interface for development
    cmd.init(led, config, visualizer, button);
}

void loop() {
    led.runAutoLight();
    cmd.handleSerialInput();
    delay(10);
}
```

#### Advanced IoT Data Logger

## 🎯 Platform Support

### ESP32 Family (Primary Target)
- Full feature set with WiFi/Bluetooth capabilities
- 240MHz dual-core processor, 520KB SRAM
- Advanced features: Firebase, MQTT, NTP, Google Sheets integration

### ESP8266 Family (Secondary)
- WiFi capabilities optimized for 80KB SRAM
- Internet connectivity features with memory optimization

### AVR Family (Limited)
- Arduino Uno/Nano/Mega compatibility
- Basic I/O and sensor operations optimized for limited resources

## 🔧 Development Workflow

### Build System
```bash
# Primary development workflow
python3 ./index.py              # Main development command

# PlatformIO commands
pio run                         # Build for active environment (ESP32)
pio run --target upload         # Upload to device
pio device monitor             # Monitor serial output
pio run --target clean          # Clean build artifacts
```

### AutoLight V3 WebClient Development
```bash
# Navigate to AutoLight V3 WebClient
cd lib/addons/AutoLightv3/WebClient

# Next.js development commands
npm run dev                    # Start development server (port 3001)
npm run build                  # Production build with static export
npm run kill                  # Kill server on port 3001

# Background process management
ps aux | grep next             # Check for running Next.js servers
sudo fuser -k 3001/tcp         # Force kill port 3001 if needed
```

### C++ Testing Framework
```bash
# Navigate to test directory and run sequence tests
cd lib/addons/AutoLightv3/WebClient/test
npm install && npm run build   # Install dependencies and compile TypeScript

# Test individual sequences
npm run test -- --sequence 2   # Test specific sequence mode
npm run test -- --all          # Test all 16 sequence modes
npm run compare                # Compare test outputs
npm run validate               # Validate sequence implementations
```

## 📁 Project Structure

```
KinematrixBeta/
├── src/                          # Main library headers
│   ├── Kinematrix.h             # Master include file
│   ├── KinematrixModules.h      # Production module orchestration
│   ├── KinematrixModulesHelper.h # Development module orchestration
│   └── KinematrixModulesNoDef.h # Header-only module orchestration
├── lib/
│   ├── enable.h                 # Master module control (156+ modules)
│   ├── modules/                 # Core functionality modules
│   │   ├── communication/       # Wired/Wireless protocols
│   │   ├── control/             # Control algorithms
│   │   ├── wifi/               # Internet/Cloud services
│   │   ├── display/            # Display systems
│   │   ├── driver/             # Hardware drivers
│   │   └── utils/              # Utility functions
│   ├── sensors/                 # Dual sensor framework
│   │   ├── SensorModule/       # V1 sensors (40+ types)
│   │   ├── SensorList/         # V2 sensors (14+ types)
│   │   └── calibration/        # Calibration systems
│   └── addons/                  # Complete application systems
│       └── AutoLightv3/         # LED control ecosystem
│           ├── WebClient/       # Next.js web interface
│           └── Core/           # Core AutoLight components
├── example/                     # 254+ example files
│   ├── basic/                  # Basic usage examples
│   ├── advanced/               # Advanced feature examples
│   ├── modules/                # Module-specific examples
│   └── addons/                 # AutoLight examples
├── platformio.ini              # Build system configuration
├── CLAUDE.md                   # Claude Code development guide
└── index.py                    # Development workflow script
```

## 🚀 Recent Achievements (2025)

### High-Precision Timing Revolution (Latest)
- **±1-4% timing accuracy** in browser-based JavaScript LED pattern simulation
- Eliminated fundamental web browser timing limitations
- Hardware-compatible simulation with exact ESP32 deployment timing
- Hybrid timing strategy using `performance.now()` + `requestAnimationFrame`

### JavaScript Simulator Mode - Production Ready
- Complete compatibility with BaseChannelSequence.cpp
- Professional LED pattern development in web browsers
- Hardware-accurate timing with zero double-timing conflicts
- Advanced pattern matrix support for complex LED coordination

## 📊 Module Categories

### Core Modules (156+ available)
- **Communication (15)**: I2C, Modbus, Serial variants, LoRa, ESP-NOW
- **Control Algorithms (7)**: PID, Fuzzy Logic variants, KNN, Decision Trees
- **Sensor Framework (78)**: V1 (42) + V2 (14) + Calibration (5) + Add-ons (4)
- **WiFi/Cloud Services (21)**: Connection handling, Firebase, MQTT, cloud integrations
- **Display Systems**: OLED, LCD, TFT display support
- **Hardware Interfaces**: I/O expansion, drivers, file systems

## 🌐 WebClient Features

### Pattern Builder System
- **4 Development Modes**: Visual, Expression, C++, JavaScript
- **Real-time Preview**: Live LED pattern visualization
- **Frame Timeline**: Professional hover preview with UI optimization
- **Export Capabilities**: Generate Arduino-ready code

### REST API Integration
```bash
# AutoLight device endpoints
GET  /api/v1/data/get/mode       # Get current mode
POST /api/v1/data/set/mode       # Set LED mode
GET  /api/v1/data/get/status     # Get device status
```

## 🔧 Configuration

### Primary Configuration Files
- **`platformio.ini`**: Build system configuration (ESP32 DevKit v1 active)
- **`lib/enable.h`**: Master module control (all 156+ modules enabled for development)

### AutoLight V3 Configuration
- **`lib/addons/AutoLightv3/Core/Config/ConfigData.h`**: Data structures and enums
- **`lib/addons/AutoLightv3/WebClient/next.config.ts`**: Next.js static export configuration

## 📚 Examples

The library includes **254+ example files** organized by category and complexity:

- **Basic Examples**: Simple sensor reading and LED control
- **Advanced Examples**: Multi-sensor systems with web integration
- **Module Examples**: Individual module demonstrations
- **AutoLight Examples**: Complete LED control systems

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Arduino Platform**: Foundation for embedded development
- **PlatformIO**: Advanced build system and library management
- **Next.js/React**: Modern web framework for WebClient
- **ESP32/ESP8266**: Powerful microcontroller platforms
- **Open Source Community**: Inspiration and collaboration

## 📞 Support

- **Documentation**: Check the `example/` directory for comprehensive usage examples
- **Issues**: Report bugs and request features via GitHub Issues
- **Development Guide**: See `CLAUDE.md` for advanced development workflows

---

**Kinematrix Beta v0.0.28** - Bridging the gap between simple Arduino sketches and professional embedded systems development.

*Built with ❤️ for the IoT community*