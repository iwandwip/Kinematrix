# Kinematrix Beta v0.0.26

**🚀 Simplify Your Arduino Projects with Advanced Modular Framework**

Kinematrix is a comprehensive, modular Arduino library framework designed to dramatically simplify IoT and embedded systems development. Built for both beginners and professionals, it provides a unified API for sensors, communication protocols, displays, control algorithms, and hardware components across multiple microcontroller platforms.

---

## ✨ Key Features

### 🧩 **Modular Architecture**
- **168 Available Modules** - Include only what you need
- **Conditional Compilation** - Optimize memory usage (50KB - 500KB+)
- **Platform Agnostic** - ESP32, ESP8266, Arduino AVR support

### 🔬 **Advanced Sensor Framework**
- **Dual Framework Support** - Legacy V1 and Modern V2 sensor systems
- **40+ Sensor Types** - Environmental, motion, power, gas, and more
- **Real-time Filtering** - Moving Average, Kalman, Median, Exponential
- **Smart Alerts** - Threshold monitoring with custom callbacks
- **Interactive Calibration** - EEPROM persistence with serial interface

### 🌐 **Comprehensive Connectivity**
- **WiFi Management** - Auto-reconnect, dual mode, static IP
- **Cloud Integration** - Firebase (3 versions), Google Sheets, MQTT
- **Wireless Protocols** - LoRa, ESP-NOW mesh networking
- **Messaging** - Telegram/WhatsApp bot integration

### 🎛️ **Advanced Control Systems**
- **PID Controllers** - Auto-tuning with Ziegler-Nichols, Cohen-Coon
- **Machine Learning** - KNN classification with cross-validation
- **Fuzzy Logic** - Mamdani, Sugeno, Tsukamoto implementations
- **Decision Trees** - Classification with pruning support

### 🖥️ **Interactive Displays**
- **Menu Systems** - Multi-level navigation for LCD/OLED
- **Dynamic Content** - Real-time data visualization
- **Custom Rendering** - SH1106, SSD1306 support

---

## 🏗️ Platform Support

| Platform | Features | Memory | Use Cases |
|----------|----------|---------|-----------|
| **ESP32** | Full feature set, WiFi/BT, dual-core | 520KB SRAM | IoT, cloud connectivity, advanced algorithms |
| **ESP8266** | WiFi features, memory optimized | 80KB SRAM | IoT sensors, basic cloud integration |
| **Arduino AVR** | Core sensors & control, optimized | 2KB SRAM | Educational, basic automation |

---

## 🚀 Quick Start

### 1. **Installation (PlatformIO Recommended)**

```bash
# Clone the repository
git clone https://github.com/iwandwip/Kinematrix.git

# Or download as Arduino library
# Add to lib_deps in platformio.ini:
# https://github.com/iwandwip/Kinematrix.git
```

### 2. **Basic Usage - Environmental Monitoring**

```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#include "Kinematrix.h"

SensorModuleV2 sensors;
BME680SensV2 environmental;

void setup() {
    Serial.begin(115200);
    
    // Configure sensor
    environmental.setUpdateInterval(5000);
    sensors.addSensor("env", &environmental);
    
    // Set temperature alert
    sensors.setThreshold("env", "temperature", 18.0, 28.0, ALERT_OUTSIDE);
    sensors.setGlobalAlertCallback([](const char* sensor, const char* key, float value, AlertType type) {
        Serial.printf("🚨 %s.%s = %.1f (Alert!)\\n", sensor, key, value);
    });
    
    sensors.init();
    Serial.println("Environmental monitoring started...");
}

void loop() {
    sensors.update();
    
    if (sensors.isUpdated("env")) {
        float temp = sensors.getValue<float>("env", "temperature");
        float humidity = sensors.getValue<float>("env", "humidity");
        float pressure = sensors.getValue<float>("env", "pressure");
        
        Serial.printf("🌡️ %.1f°C | 💧 %.1f%% | 🌪️ %.1f hPa\\n", 
                      temp, humidity, pressure);
    }
    
    delay(100);
}
```

### 3. **Advanced Usage - IoT Data Logger with Cloud**

```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_MODULE_WIFI_HANDLER_V2
#define ENABLE_MODULE_MQTT_MANAGER
#include "Kinematrix.h"

SensorModuleV2 dataLogger;
WiFiHandlerV2 wifi;
MQTTManager mqtt;

void setup() {
    Serial.begin(115200);
    
    // Setup WiFi
    wifi.setCredentials("YourWiFi", "YourPassword");
    wifi.connect();
    
    // Setup MQTT
    mqtt.setServer("broker.hivemq.com", 1883);
    mqtt.setClientId("kinematrix-logger");
    mqtt.connect();
    
    // Add soil moisture sensor with filtering
    dataLogger.addSensor("soil", []() -> BaseSensV2* {
        auto sensor = new AnalogSensV2(A0, 3.3, 4095, [](BaseSensV2* s, int raw, float volt) {
            float moisture = map(raw, 0, 4095, 0, 100);
            s->updateValue("moisture", moisture);
            s->updateValue("conductivity", raw * 0.1);
        });
        
        sensor->addCustomValue("moisture", "Soil Moisture", "%", 1, true);
        sensor->addCustomValue("conductivity", "Conductivity", "μS/cm", 1, true);
        sensor->setUpdateInterval(30000); // 30 seconds
        return sensor;
    });
    
    dataLogger.init();
    
    // Add moving average filter
    FilterParams filter;
    filter.movingAverage.windowSize = 10;
    dataLogger.attachFilter("soil", "moisture", FILTER_MOVING_AVERAGE, filter);
    
    Serial.println("IoT Data Logger initialized!");
}

void loop() {
    wifi.handle();
    mqtt.handle();
    dataLogger.update();
    
    if (dataLogger.isUpdated("soil")) {
        float moisture = dataLogger.getFilteredValue("soil", "moisture");
        float conductivity = dataLogger.getValue<float>("soil", "conductivity");
        
        // Publish to MQTT
        String payload = "{\"moisture\":" + String(moisture) + 
                        ",\"conductivity\":" + String(conductivity) + 
                        ",\"timestamp\":" + String(millis()) + "}";
        
        mqtt.publish("kinematrix/sensors/soil", payload);
        
        Serial.printf("📡 Published: Moisture=%.1f%%, Conductivity=%.1f μS/cm\\n", 
                      moisture, conductivity);
    }
    
    delay(100);
}
```

---

## 📦 Available Modules

### 🔧 **Core Modules (168 total)**

#### Communication (15 modules)
```cpp
#define ENABLE_MODULE_I2C_EXPANDER         // I2C device expansion
#define ENABLE_MODULE_MODBUS               // Industrial communication
#define ENABLE_MODULE_LORA_COM_V2          // Long-range wireless
#define ENABLE_MODULE_ESP_NOW              // ESP mesh networking
#define ENABLE_MODULE_MQTT_MANAGER         // IoT messaging
#define ENABLE_MODULE_WIFI_HANDLER_V2      // Advanced WiFi management
```

#### Sensors V2 (14 modules)
```cpp
#define ENABLE_SENSOR_MODULE_V2            // Modern sensor framework
#define ENABLE_SENSOR_ANALOG_V2            // Analog sensors with processing
#define ENABLE_SENSOR_BME680_V2            // Environmental sensor
#define ENABLE_SENSOR_DHT_V2               // Temperature/humidity
#define ENABLE_SENSOR_INA219_V2            // Power monitoring
#define ENABLE_SENSOR_MLX90614_V2          // Infrared temperature
#define ENABLE_SENSOR_MQ_V2                // Gas sensors
#define ENABLE_SENSOR_RTC_DS3231_V2        // Real-time clock
```

#### Control Algorithms (7 modules)
```cpp
#define ENABLE_MODULE_PID_CONTROLLER       // Advanced PID with tuning
#define ENABLE_MODULE_FUZZY_MAMDANI        // Fuzzy logic control
#define ENABLE_MODULE_KNN                  // Machine learning classification
#define ENABLE_MODULE_DECISION_TREE        // Decision tree classifier
```

#### Cloud Services (21 modules)
```cpp
#define ENABLE_MODULE_FIREBASE_RTDB_V3     // Firebase Realtime Database
#define ENABLE_MODULE_FIREBASE_FIRESTORE_V3 // Cloud Firestore
#define ENABLE_MODULE_GOOGLE_SHEETS        // Google Sheets integration
#define ENABLE_MODULE_TELEGRAM_BOT         // Telegram messaging
```

#### Display Systems (3 modules)
```cpp
#define ENABLE_MODULE_LCD_MENU             // LCD menu system
#define ENABLE_MODULE_OLED_MENU            // OLED menu system
#define ENABLE_MODULE_SH1106_MENU          // SH1106 display support
```

### 📄 **Complete Module List**
For the complete list of all 168 available modules, see [Module Reference](#module-reference) below or check `lib/enable.h`.

---

## 🌟 Advanced Features

### 🔍 **Real-time Sensor Filtering**

```cpp
// Add Kalman filter for noisy sensors
FilterParams kalman;
kalman.kalman.processNoise = 0.01f;
kalman.kalman.measurementNoise = 0.1f;
kalman.kalman.estimateError = 0.1f;
sensorModule.attachFilter("temp", "celsius", FILTER_KALMAN, kalman);

// Get filtered value
float filteredTemp = sensorModule.getFilteredValue("temp", "celsius");
```

### 🚨 **Smart Alert System**

```cpp
// Set threshold alerts
sensorModule.setThreshold("temp", "celsius", 20.0, 30.0, ALERT_OUTSIDE);

// Custom alert handling
sensorModule.setGlobalAlertCallback([](const char* sensor, const char* key, float value, AlertType type) {
    // Send SMS, email, or trigger actuators
    if (type == ALERT_ABOVE) {
        // Turn on cooling system
    } else if (type == ALERT_BELOW) {
        // Turn on heating system
    }
});
```

### 🎯 **Interactive Calibration**

```cpp
// Start interactive calibration via serial
SensorCalibrationModuleV2 calibrator;
calibrator.addSensor("ph", sensorModule.getSensorByName("ph"));
calibrator.setEEPROMStartAddress(100);
calibrator.startInteractiveCalibration("ph", "value");

// Automatic EEPROM persistence
calibrator.setAutoSaveCalibration(true);
```

### 🤖 **Advanced Control Algorithms**

```cpp
// PID Controller with auto-tuning
PIDController pid(2.0, 0.1, 0.5, 0.1, 0.0, 255.0);
pid.setSetPoint(60.0);
pid.calculateOptimalIntegralLimit();

// Fuzzy Logic Control
FuzzyMamdani fuzzy;
fuzzy.addInputVariable("temperature", 0, 50);
fuzzy.addOutputVariable("fan_speed", 0, 100);
fuzzy.addRule("IF temperature IS high THEN fan_speed IS fast");
```

---

## 📖 Project Structure

```
KinematrixBeta/
├── src/                          # Main library headers
│   ├── Kinematrix.h             # Master include file
│   ├── KinematrixModules.h      # Module orchestration
│   └── KinematrixSensor.h       # Sensor framework
│
├── lib/
│   ├── enable.h                 # Module enable definitions (168 modules)
│   ├── modules/                 # Core functionality (168 modules)
│   │   ├── communication/       # Wired/wireless protocols
│   │   ├── control/            # PID, ML, fuzzy logic
│   │   ├── display/            # LCD/OLED menu systems
│   │   ├── wifi/               # WiFi & cloud services
│   │   └── ...                 # 10+ other categories
│   │
│   ├── sensors/                # Sensor implementations
│   │   ├── SensorModuleV1/     # Legacy framework (40+ sensors)
│   │   └── SensorModuleV2/     # Modern framework (advanced features)
│   │
│   └── addons/                 # Complete application systems
│       └── AutoLight/          # Lighting control systems
│
├── example/                    # 700+ example files
│   ├── modules/               # Module-specific examples
│   │   ├── control/          # PID, ML, fuzzy examples
│   │   ├── wifi/             # IoT & cloud examples
│   │   └── ...               # Examples for all categories
│   └── sensors/              # Sensor usage examples
│
├── platformio.ini             # Build configuration
└── library.properties         # Arduino library metadata
```

---

## 📚 Examples & Tutorials

### 🎓 **Learning Path**

1. **Beginner**: Start with basic sensor examples in `example/sensors/`
2. **Intermediate**: Explore module examples in `example/modules/`
3. **Advanced**: Study complete systems in `lib/addons/`

### 🔥 **Popular Examples**

#### Environmental Monitoring Station
- **Location**: `example/sensors/SensorModuleV2/`
- **Features**: BME680, filtering, alerts, data logging
- **Complexity**: Intermediate

#### IoT Smart Farm Controller
- **Location**: `example/modules/wifi/firebase/`
- **Features**: Sensor monitoring, cloud sync, remote control
- **Complexity**: Advanced

#### Fuzzy Logic HVAC Controller
- **Location**: `example/modules/control/EXAMPLE-FuzzyMamdani/`
- **Features**: Temperature/humidity control, energy optimization
- **Complexity**: Advanced

#### Machine Learning Gesture Recognition
- **Location**: `example/modules/control/EXAMPLE-KNN/`
- **Features**: Accelerometer data, pattern recognition
- **Complexity**: Expert

---

## 🛠️ Development Guide

### **PlatformIO Setup (Recommended)**

```ini
[env:esp32]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
lib_deps = 
    https://github.com/iwandwip/Kinematrix.git
    # Other dependencies managed automatically
```

### **Arduino IDE Setup**

1. Download library as ZIP
2. Install via Library Manager
3. Include required modules in your sketch

### **Memory Optimization**

```cpp
// Minimal setup (~50KB)
#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_ANALOG
#include "Kinematrix.h"

// Full-featured setup (~500KB)
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_MODULE_WIFI_HANDLER_V2
#define ENABLE_MODULE_FIREBASE_RTDB_V3
#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"
```

---

## 📊 Module Reference

<details>
<summary><strong>Communication Modules (15)</strong></summary>

| Module | Description | Platforms |
|--------|-------------|-----------|
| `ENABLE_MODULE_I2C_EXPANDER` | I2C device expansion | All |
| `ENABLE_MODULE_I2C_SCANNER` | I2C device detection | All |
| `ENABLE_MODULE_MODBUS` | Industrial communication | All |
| `ENABLE_MODULE_LORA_COM` | LoRa wireless communication | All |
| `ENABLE_MODULE_LORA_COM_V2` | Advanced LoRa with retry | All |
| `ENABLE_MODULE_LORA_EBYTE_E220` | EByte E220 LoRa module | All |
| `ENABLE_MODULE_ESP_NOW` | ESP mesh networking | ESP32/ESP8266 |
| `ENABLE_MODULE_MQTT_MANAGER` | MQTT client with reconnection | ESP32/ESP8266 |
| `ENABLE_MODULE_WIFI_HANDLER_V2` | Advanced WiFi management | ESP32/ESP8266 |
| `ENABLE_MODULE_TELEGRAM_BOT` | Telegram bot integration | ESP32/ESP8266 |
| `ENABLE_MODULE_WHATSAPP_BOT` | WhatsApp messaging | ESP32/ESP8266 |

</details>

<details>
<summary><strong>Sensor Framework V2 (14)</strong></summary>

| Module | Description | Features |
|--------|-------------|----------|
| `ENABLE_SENSOR_MODULE_V2` | Core sensor framework | Type-safe, JSON-based |
| `ENABLE_SENSOR_FILTER_V2` | Real-time filtering | 4 filter types |
| `ENABLE_SENSOR_ALERT_SYSTEM_V2` | Threshold monitoring | Custom callbacks |
| `ENABLE_SENSOR_CALIBRATION_MODULE_V2` | Interactive calibration | EEPROM persistence |
| `ENABLE_SENSOR_ANALOG_V2` | Analog sensors | Custom processing |
| `ENABLE_SENSOR_BME680_V2` | Environmental sensor | Temp/humidity/pressure/gas |
| `ENABLE_SENSOR_DHT_V2` | Temperature/humidity | DHT11/DHT22 support |
| `ENABLE_SENSOR_INA219_V2` | Power monitoring | Current/voltage/power |
| `ENABLE_SENSOR_MLX90614_V2` | Infrared temperature | Non-contact measurement |
| `ENABLE_SENSOR_MQ_V2` | Gas sensors | Multi-gas support |
| `ENABLE_SENSOR_RTC_DS3231_V2` | Real-time clock | High precision |

</details>

<details>
<summary><strong>Control Algorithms (7)</strong></summary>

| Module | Description | Features |
|--------|-------------|----------|
| `ENABLE_MODULE_PID_CONTROLLER` | Advanced PID control | Auto-tuning, limits |
| `ENABLE_MODULE_FUZZY_MAMDANI` | Mamdani fuzzy logic | Rule-based control |
| `ENABLE_MODULE_FUZZY_SUGENO` | Sugeno fuzzy logic | Mathematical functions |
| `ENABLE_MODULE_FUZZY_TSUKAMOTO` | Tsukamoto fuzzy logic | Monotonic membership |
| `ENABLE_MODULE_KNN` | K-Nearest Neighbors | Classification/regression |
| `ENABLE_MODULE_DECISION_TREE` | Decision tree classifier | Pruning, cross-validation |

</details>

<details>
<summary><strong>Cloud & IoT Services (21)</strong></summary>

| Module | Description | Platform |
|--------|-------------|----------|
| `ENABLE_MODULE_FIREBASE_RTDB_V3` | Realtime Database | ESP32/ESP8266 |
| `ENABLE_MODULE_FIREBASE_FIRESTORE_V3` | Cloud Firestore | ESP32/ESP8266 |
| `ENABLE_MODULE_FIREBASE_STORAGE_V3` | Cloud Storage | ESP32/ESP8266 |
| `ENABLE_MODULE_FIREBASE_MESSAGING_V3` | Push notifications | ESP32/ESP8266 |
| `ENABLE_MODULE_GOOGLE_SHEETS` | Spreadsheet integration | ESP32/ESP8266 |
| `ENABLE_MODULE_DATETIME_NTP_V2` | Network time sync | ESP32/ESP8266 |

</details>

<details>
<summary><strong>All 168 Modules</strong></summary>

See `lib/enable.h` for the complete list of all available modules organized by category.

</details>

---

## 🤝 Contributing

We welcome contributions! Here's how you can help:

### **For Developers**
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-sensor`)
3. Follow the existing code style and module structure
4. Add comprehensive examples in `example/`
5. Submit a pull request with detailed description

### **For Hardware Enthusiasts**
- Test modules on different platforms
- Report compatibility issues
- Share real-world project examples

### **For Educators**
- Create educational content and tutorials
- Improve documentation and examples
- Suggest beginner-friendly features

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](https://choosealicense.com/licenses/mit/) for details.

---

## 📞 Support & Community

- **📧 Email**: [mcclawsher369@gmail.com](mailto:mcclawsher369@gmail.com)
- **🐙 GitHub**: [iwandwip/Kinematrix](https://github.com/iwandwip/Kinematrix)
- **📖 Documentation**: Comprehensive examples and guides included
- **🔧 Issues**: Report bugs and request features on GitHub

---

## 🏆 Acknowledgments

- **Author**: [@iwandwip](https://github.com/iwandwip) (mcclawsher)
- **Community**: Thanks to all contributors and testers
- **Libraries**: Built on top of excellent open-source Arduino libraries
- **Inspiration**: Created for educational purposes and real-world applications

---

<div align="center">

**⭐ Star this repository if you find it useful!**

**🚀 Start building amazing IoT projects with Kinematrix today!**

</div>