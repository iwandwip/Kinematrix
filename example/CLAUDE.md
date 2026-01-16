# CLAUDE.md - Examples Directory

## Directory Overview
Comprehensive collection of 700+ example files demonstrating every module and sensor in the Kinematrix Beta library.

## Structure
- **`modules/`** - Module-specific examples with progressive difficulty
- **`sensors/`** - Sensor framework examples (V1 and V2)

## Example Organization

### Module Examples (`modules/`)
Examples organized by functional category with progressive complexity:

#### Control Algorithm Examples
- **`control/EXAMPLE-PIDController/`** - 6 PID examples
  - Basic PID usage
  - Auto-tuning (Ziegler-Nichols, Cohen-Coon)
  - EEPROM parameter storage
  - Advanced features demonstration

- **`control/EXAMPLE-FuzzyMamdani/`** - 6 Fuzzy Logic examples
  - Simplified usage patterns
  - HVAC control systems
  - Irrigation automation
  - Robot navigation
  - Temperature control

- **`control/EXAMPLE-DecisionTree/`** - 7 Machine Learning examples
  - Basic classification
  - Cross-validation techniques
  - Model save/load functionality
  - Real-world applications (soil, weather)

- **`control/EXAMPLE-KNN/`** - 10 KNN examples
  - Distance metrics comparison
  - Gesture recognition
  - Currency detection
  - Weighted KNN implementation

#### Communication Examples
- **`communication/wireless/lora/`** - 12 LoRa examples
  - Basic point-to-point communication
  - Multi-node networks
  - Long-range applications
  - EByte E220 specific features

#### Display System Examples
- **`display/EXAMPLE-lcd-menu/`** - 5 LCD menu examples
  - Basic menu navigation
  - Dynamic content menus
  - Multi-level menu systems
  - WiFi configuration menus

- **`display/EXAPMLE-oled-menu/`** - 5 OLED menu examples
  - OLED-specific implementations
  - Graphical menu elements
  - Real-time data display

#### I/O Expansion Examples
- **`io/EXAMPLE-pcf8574-module/`** - 10+ PCF8574 examples
  - Basic LED control
  - Button input handling
  - Multi-PCF8574 configurations
  - Rotary encoder integration

#### Time Management Examples
- **`time/EXAMPLE-timer-manager/`** - 9 Timer examples
  - Basic intervals and one-shots
  - Multiple timer coordination
  - Button debouncing
  - Long-duration timers

#### Utility System Examples
- **`utils/EXAMPLE-LogicConditionManager/`** - 27 Logic examples
  - Boolean conditions
  - Value-based conditions
  - Composite logic systems
  - Real-world applications (smart home, industrial)

- **`utils/EXAMPLE-StateActionManager/`** - 9 State examples
  - State change monitoring
  - Range-based control
  - Multi-sensor coordination

#### WiFi/Cloud Examples
- **`wifi/EXAMPLE-WiFiModule/`** - Basic WiFi connectivity
- **`wifi/EXAMPLE-mqtt-manager/`** - MQTT client examples
- **`wifi/EXAMPLE-google-sheets/`** - Google Sheets integration
- **`wifi/firebase/`** - Firebase examples (3 versions)

#### File System Examples
- **`file/EXAMPLE-SDCardModuleESP32/`** - 8 SD card examples
  - Basic file operations
  - Data logging patterns
  - Performance testing
  - Directory management

### Sensor Examples (`sensors/`)
- **`SensorModuleV1/`** - Legacy sensor examples
- **`SensorModuleV2/`** - Modern sensor framework examples

## Example Naming Convention
```
EXAMPLE-ModuleName/
├── module-feature-basic/
│   └── module-feature-basic.ino
├── module-feature-advanced/
│   └── module-feature-advanced.ino
└── module-real-world-application/
    └── module-real-world-application.ino
```

## Complexity Levels

### Basic Examples
- Minimal setup and usage
- Single feature demonstration
- Clear, commented code
- Ideal for beginners

### Intermediate Examples
- Multiple feature combinations
- Error handling
- Configuration options
- Real-world scenarios

### Advanced Examples
- Complex integrations
- Performance optimization
- Multiple module coordination
- Production-ready patterns

## Educational Focus
Examples designed for:
- **Beginners**: Step-by-step learning progression
- **Intermediate**: Practical application patterns
- **Advanced**: Complex system integration
- **Educators**: Teaching embedded systems concepts

## Usage Pattern
Each example includes:
- Header comments explaining purpose
- Hardware requirements
- Wiring diagrams (where applicable)
- Expected output description
- Troubleshooting notes

## Platform Compatibility
Examples tagged with platform requirements:
- **[ESP32]** - ESP32-specific features
- **[ESP8266]** - ESP8266 compatibility
- **[AVR]** - Arduino Uno/Nano compatibility
- **[ALL]** - Universal compatibility

## Quick Start
1. Navigate to desired module category
2. Start with basic examples
3. Progress to advanced implementations
4. Adapt examples for your specific use case

## Example Applications
- Smart home automation
- Industrial monitoring
- Agricultural systems
- Educational projects
- Robotics control
- IoT sensor networks
- Environmental monitoring
- Data logging systems