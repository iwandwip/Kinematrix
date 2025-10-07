# SensorModuleV2 Documentation

## Overview

SensorModuleV2 is a comprehensive, modular sensor management framework for Arduino and ESP32 platforms. It provides a unified API for managing multiple sensors with advanced features including real-time filtering, alert systems, calibration management, and extensive debugging capabilities.

## Table of Contents

1. [Features](#features)
2. [Architecture](#architecture)
3. [Quick Start](#quick-start)
4. [Core Classes](#core-classes)
5. [Sensor Types](#sensor-types)
6. [Advanced Features](#advanced-features)
7. [Examples](#examples)
8. [API Reference](#api-reference)
9. [Best Practices](#best-practices)
10. [Troubleshooting](#troubleshooting)

## Features

### Core Functionality
- **Template-based Type Safety**: Type-safe data retrieval with compile-time checking
- **JSON Data Storage**: Efficient data management using ArduinoJson
- **Dynamic Sensor Management**: Runtime sensor registration and management
- **Real-time Updates**: Configurable update intervals with status tracking
- **Memory Management**: Automatic memory allocation and cleanup

### Advanced Systems
- **Multi-level Filtering**: Moving Average, Kalman, Median, and Exponential filters
- **Comprehensive Alert System**: Threshold-based alerts with customizable callbacks
- **Calibration Framework**: Interactive calibration with EEPROM persistence
- **Debug & Monitoring**: Extensive debugging and data visualization tools
- **Modular Architecture**: Easy extension with custom sensor types

### Platform Support
- **Arduino**: Uno, Nano, Mega, and compatible boards
- **ESP32**: All variants with WiFi and Bluetooth support
- **ESP8266**: NodeMCU, Wemos D1 Mini, and compatible boards

## Architecture

```
SensorModuleV2/
├── SensorModule/              # Core framework
│   ├── SensorModuleV2         # Main sensor management class
│   ├── SensorCalibrationModuleV2 # Calibration system
│   ├── Systems/               # Supporting systems
│   │   ├── SensorAlertSystemV2    # Alert management
│   │   └── SensorFilterV2         # Signal filtering
│   ├── Tools/                 # Development tools  
│   │   └── InteractiveSerialGeneralSensorCalibratorV2
│   └── Utils/                 # Utilities
│       ├── SensorUtilityV2        # Helper functions
│       └── SensorImplementationGuide.md
└── SensorList/                # Available sensor types
    ├── AbstractSensV2         # Virtual test sensor
    ├── AnalogSensV2          # Analog input sensor
    ├── BME680SensV2          # Environmental sensor
    ├── DHTSensV2             # Temperature/humidity
    ├── GP2YDustSensV2        # Dust particle sensor
    ├── INA219SensV2          # Current/power sensor
    ├── MLX90614SensV2        # Infrared temperature
    ├── MQSensV2              # Gas sensor family
    ├── RTCSensV2/            # Real-time clock sensors
    └── CustomSensorTemplateV2 # Template for new sensors
```

## Quick Start

### 1. Basic Setup

```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;
AnalogSensV2 lightSensor(A0, 5.0, 1023);

void setup() {
    Serial.begin(115200);
    
    // Configure sensor
    lightSensor.setUpdateInterval(1000);
    
    // Add to module
    sensorModule.addSensor("light", &lightSensor);
    
    // Initialize
    sensorModule.init();
}

void loop() {
    sensorModule.update();
    
    // Get sensor data
    if (sensorModule.isUpdated("light")) {
        float voltage = sensorModule.getValue<float>("light", "volt");
        int rawValue = sensorModule.getValue<int>("light", "raw");
        
        Serial.print("Light: ");
        Serial.print(voltage);
        Serial.println(" V");
    }
    
    delay(100);
}
```

### 2. Advanced Setup with Filtering and Alerts

```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#include "Kinematrix.h"

SensorModuleV2 sensorModule;
AnalogSensV2 temperatureSensor(A0, 5.0, 1023);

void setup() {
    Serial.begin(115200);
    
    // Add sensor with custom processing
    sensorModule.addSensor("temp", []() -> BaseSensV2* {
        auto sensor = new AnalogSensV2(A0, 5.0, 1023, 
            [](BaseSensV2* s, int raw, float volt) {
                float tempC = (volt - 0.5) * 100; // LM35 conversion
                s->updateValue("celsius", tempC);
                s->updateValue("fahrenheit", tempC * 9.0/5.0 + 32);
            });
        
        sensor->addCustomValue("celsius", "Temperature", "°C", 1, true);
        sensor->addCustomValue("fahrenheit", "Temperature", "°F", 1, true);
        sensor->setUpdateInterval(2000);
        return sensor;
    });
    
    sensorModule.init();
    
    // Add moving average filter
    FilterParams filterParams;
    filterParams.movingAverage.windowSize = 10;
    sensorModule.attachFilter("temp", "celsius", FILTER_MOVING_AVERAGE, filterParams);
    
    // Set temperature alerts
    sensorModule.setThreshold("temp", "celsius", 20.0, 30.0, ALERT_OUTSIDE);
    sensorModule.setGlobalAlertCallback([](const char* sensor, const char* key, float value, AlertType type) {
        Serial.print("ALERT: Temperature ");
        Serial.print(value);
        Serial.println(type == ALERT_ABOVE ? " too high!" : " too low!");
    });
}

void loop() {
    sensorModule.update();
    
    if (sensorModule.isUpdated("temp")) {
        float rawTemp = sensorModule.getValue<float>("temp", "celsius");
        float filteredTemp = sensorModule.getFilteredValue("temp", "celsius");
        
        Serial.print("Temperature: Raw=");
        Serial.print(rawTemp);
        Serial.print("°C, Filtered=");
        Serial.print(filteredTemp);
        Serial.println("°C");
    }
    
    delay(100);
}
```

## Core Classes

### BaseSensV2

Base class for all sensors providing core functionality:

```cpp
class BaseSensV2 {
public:
    // Core methods
    virtual bool init() = 0;
    virtual bool update() = 0;
    virtual bool isUpdated() const;
    
    // Data access
    template<typename T> T getValue(const char* key) const;
    float getFloatValue(const char* key) const;
    int getIntValue(const char* key) const;
    const char* getStringValue(const char* key) const;
    
    // Configuration
    void addValueInfo(const char* key, const char* label, 
                     const char* unit, uint8_t precision, bool calibrable);
    void updateValue(const char* key, float value);
    void updateValue(const char* key, int value);
    void updateValue(const char* key, const char* value);
    
    // Calibration
    void setCalibration(const char* key, float offset, float scale);
    bool hasValue(const char* key) const;
};
```

### SensorModuleV2

Main sensor management class:

```cpp
class SensorModuleV2 {
public:
    // Sensor management
    void addSensor(const char* name, BaseSensV2* sensor);
    void addSensor(const char* name, SensorCreateCallback callback);
    
    // System control
    void init();
    void update();
    
    // Data access
    template<typename T> T getValue(const char* sensorName, const char* valueKey) const;
    bool isUpdated(const char* sensorName) const;
    BaseSensV2* getSensorByName(const char* name) const;
    
    // Alert system
    bool setThreshold(const char* sensorName, const char* valueKey,
                     float lowThreshold, float highThreshold, AlertType type);
    void setGlobalAlertCallback(AlertCallback callback);
    
    // Filter system  
    bool attachFilter(const char* sensorName, const char* valueKey,
                     FilterType type, FilterParams params);
    float getFilteredValue(const char* sensorName, const char* valueKey);
    
    // Debugging
    void debug(const char* sensorName, const char* valueKey, bool showHeap = false);
    void debugAll(bool showHeap = false);
};
```

## Sensor Types

### AnalogSensV2

General-purpose analog sensor with custom data processing:

```cpp
// Basic usage
AnalogSensV2 sensor(A0, 5.0, 1023);

// With custom data callback
AnalogSensV2 sensor(A0, 5.0, 1023, [](BaseSensV2* s, int raw, float volt) {
    float processed = volt * 100; // Custom processing
    s->updateValue("processed", processed);
});

// Configuration
sensor.setUpdateInterval(1000);
sensor.addCustomValue("processed", "Processed Value", "units", 2, true);
```

**Default Values:**
- `raw` - Raw ADC reading (0-1023)
- `volt` - Voltage value (calculated from raw)

### BME680SensV2

Environmental sensor (temperature, humidity, pressure, gas):

```cpp
BME680SensV2 bme680;
bme680.setUpdateInterval(3000);

// Access environmental data
float temp = bme680.getValue<float>("temperature");     // °C
float humidity = bme680.getValue<float>("humidity");    // %
float pressure = bme680.getValue<float>("pressure");    // hPa
float altitude = bme680.getValue<float>("altitude");    // m
float gas = bme680.getValue<float>("gas");              // Ohms
```

### DHTSensV2  

Temperature and humidity sensor:

```cpp
DHTSensV2 dht(2, DHT22);  // Pin 2, DHT22 type
dht.setUpdateInterval(2000);

float temp = dht.getValue<float>("temperature");    // °C
float humidity = dht.getValue<float>("humidity");   // %
```

### MQSensV2

Gas sensor with multiple gas detection:

```cpp
MQSensV2 mq("ESP32", 3.3, 12, A0, "MQ-135");

// Configure
mq.setRL(1.0);
mq.setR0(9.83);
mq.enableMultipleGases(true);

// Add custom gas profiles
mq.addCustomGas("CO2", 110.47, -2.862, 1, "ppm", 2);

// Calibration
mq.calibrateInCleanAir(9.83);

// Reading
float concentration = mq.getValue<float>("ppm");
float ratio = mq.getValue<float>("ratio");
```

### AbstractSensV2

Virtual sensor for testing and development:

```cpp
AbstractSensV2 testSensor(0);  // Random mode
testSensor.setUpdateInterval(500);

// Available test signals
float value = testSensor.getValue<float>("value");      // Main value
float random = testSensor.getValue<float>("random");    // Random data
float sine = testSensor.getValue<float>("sine");        // Sine wave
float cosine = testSensor.getValue<float>("cosine");    // Cosine wave
int square = testSensor.getValue<int>("square");        // Square wave
float triangle = testSensor.getValue<float>("triangle"); // Triangle wave
int counter = testSensor.getValue<int>("counter");      // Counter
```

### RTCSensV2

Real-time clock sensors (DS1307, DS3231, PCF8523, PCF8563):

```cpp
RTCDS3231SensV2 rtc;
rtc.setUpdateInterval(1000);

// Time values
int hour = rtc.getValue<int>("hour");
int minute = rtc.getValue<int>("minute");
int second = rtc.getValue<int>("second");
int day = rtc.getValue<int>("day");
int month = rtc.getValue<int>("month");
int year = rtc.getValue<int>("year");
```

## Advanced Features

### Filtering System

The filtering system provides real-time signal processing:

#### Moving Average Filter
```cpp
FilterParams params;
params.movingAverage.windowSize = 10;
sensorModule.attachFilter("sensor", "value", FILTER_MOVING_AVERAGE, params);
```

#### Kalman Filter
```cpp
FilterParams params;
params.kalman.processNoise = 0.01f;
params.kalman.measurementNoise = 0.1f;
params.kalman.estimateError = 0.1f;
sensorModule.attachFilter("sensor", "value", FILTER_KALMAN, params);
```

#### Median Filter
```cpp
FilterParams params;
params.median.windowSize = 5;
sensorModule.attachFilter("sensor", "value", FILTER_MEDIAN, params);
```

#### Exponential Filter
```cpp
FilterParams params;
params.exponential.alpha = 0.1f;
sensorModule.attachFilter("sensor", "value", FILTER_EXPONENTIAL, params);
```

### Alert System

Comprehensive alerting with threshold monitoring:

#### Basic Threshold Alert
```cpp
// Alert when temperature is outside 20-30°C range
sensorModule.setThreshold("temp", "celsius", 20.0, 30.0, ALERT_OUTSIDE);
```

#### Alert Types
- `ALERT_ABOVE` - Trigger when value exceeds high threshold
- `ALERT_BELOW` - Trigger when value falls below low threshold  
- `ALERT_OUTSIDE` - Trigger when value is outside range
- `ALERT_INSIDE` - Trigger when value is inside range

#### Alert Callbacks
```cpp
// Global callback for all alerts
sensorModule.setGlobalAlertCallback([](const char* sensor, const char* key, float value, AlertType type) {
    Serial.print("Alert: ");
    Serial.print(sensor);
    Serial.print(".");
    Serial.print(key);
    Serial.print(" = ");
    Serial.println(value);
});

// Sensor-specific callback
sensorModule.setSensorAlertCallback("temp", [](const char* sensor, const char* key, float value, AlertType type) {
    // Handle temperature-specific alerts
});
```

#### Alert Management
```cpp
// Check alert status
bool isActive = sensorModule.isAlertActive("temp", "celsius");
AlertState state = sensorModule.getAlertState("temp", "celsius");

// Acknowledge alerts
sensorModule.acknowledgeAlert("temp", "celsius");
sensorModule.acknowledgeAllAlerts();

// Reset alerts
sensorModule.resetAlert("temp", "celsius");
sensorModule.resetAllAlerts();
```

### Calibration System

Interactive calibration with EEPROM persistence:

#### Basic Calibration
```cpp
// Set manual calibration
sensor.setCalibration("temperature", 0.5, 1.02); // offset, scale

// Interactive calibration
SensorCalibrationModuleV2 calibrator;
calibrator.addSensor("temp", sensorModule.getSensorByName("temp"));
calibrator.startInteractiveCalibration("temp", "celsius");
```

#### EEPROM Persistence
```cpp
calibrator.setEEPROMStartAddress(100);
calibrator.setAutoSaveCalibration(true);
calibrator.saveCalibrationToEEPROM();
calibrator.loadCalibrationFromEEPROM();
```

### Debugging and Monitoring

Comprehensive debugging tools:

```cpp
// Debug single sensor value
sensorModule.debug("temp", "celsius", true); // Show heap memory

// Debug all sensors
sensorModule.debugAll(true);

// Horizontal format debug
sensorModule.debugHorizontal();

// Pretty formatted debug with timing
sensorModule.debugPretty(1000);

// Custom debug using SensorUtilityV2
SensorUtilityV2::debugNested(&sensorModule, "temp", true);
```

## Examples

### Environmental Monitoring Station

```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2

#include "Kinematrix.h"

SensorModuleV2 environment;
BME680SensV2 bme680;
AnalogSensV2 lightSensor(A0, 3.3, 4095);

void setup() {
    Serial.begin(115200);
    
    // Configure sensors
    bme680.setUpdateInterval(5000);
    lightSensor.setUpdateInterval(1000);
    
    // Add sensors
    environment.addSensor("bme680", &bme680);
    environment.addSensor("light", &lightSensor);
    
    // Initialize
    environment.init();
    
    // Add filters
    FilterParams tempFilter;
    tempFilter.movingAverage.windowSize = 5;
    environment.attachFilter("bme680", "temperature", FILTER_MOVING_AVERAGE, tempFilter);
    
    // Set alerts
    environment.setThreshold("bme680", "temperature", 18.0, 28.0, ALERT_OUTSIDE);
    environment.setThreshold("bme680", "humidity", 30.0, 70.0, ALERT_OUTSIDE);
    environment.setThreshold("light", "volt", 1.0, 3.0, ALERT_OUTSIDE);
    
    // Alert callback
    environment.setGlobalAlertCallback([](const char* sensor, const char* key, float value, AlertType type) {
        Serial.print("[ENVIRONMENTAL ALERT] ");
        Serial.print(sensor);
        Serial.print(".");
        Serial.print(key);
        Serial.print(" = ");
        Serial.print(value);
        Serial.println(type == ALERT_ABOVE ? " (HIGH)" : " (LOW)");
    });
    
    Serial.println("Environmental monitoring started...");
}

void loop() {
    environment.update();
    
    // Check for BME680 updates
    if (environment.isUpdated("bme680")) {
        float temp = environment.getValue<float>("bme680", "temperature");
        float humidity = environment.getValue<float>("bme680", "humidity");
        float pressure = environment.getValue<float>("bme680", "pressure");
        
        Serial.println("=== Environmental Data ===");
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.println("°C");
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println("%");
        Serial.print("Pressure: ");
        Serial.print(pressure);
        Serial.println(" hPa");
    }
    
    // Check for light sensor updates
    if (environment.isUpdated("light")) {
        float lightVolt = environment.getValue<float>("light", "volt");
        int lightRaw = environment.getValue<int>("light", "raw");
        
        Serial.print("Light: ");
        Serial.print(lightVolt);
        Serial.print("V (");
        Serial.print(lightRaw);
        Serial.println(")");
    }
    
    delay(100);
}
```

### Multi-Sensor Data Logger

```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2

#include "Kinematrix.h"

SensorModuleV2 dataLogger;
SensorCalibrationModuleV2 calibrator;

void setup() {
    Serial.begin(115200);
    
    // Add temperature/humidity sensor
    dataLogger.addSensor("dht", []() -> BaseSensV2* {
        auto dht = new DHTSensV2(2, DHT22);
        dht->setUpdateInterval(2000);
        return dht;
    });
    
    // Add soil moisture sensor with calibration
    dataLogger.addSensor("soil", []() -> BaseSensV2* {
        auto soil = new AnalogSensV2(A1, 5.0, 1023, [](BaseSensV2* s, int raw, float volt) {
            float moisture = map(raw, 0, 1023, 0, 100);
            s->updateValue("moisture", moisture);
            s->updateValue("conductivity", raw * 0.1); // Custom processing
        });
        
        soil->addCustomValue("moisture", "Soil Moisture", "%", 1, true);
        soil->addCustomValue("conductivity", "Soil Conductivity", "µS/cm", 1, true);
        soil->setUpdateInterval(5000);
        return soil;
    });
    
    // Initialize systems
    dataLogger.init();
    
    // Setup calibration
    calibrator.addSensor("soil", dataLogger.getSensorByName("soil"));
    calibrator.setEEPROMStartAddress(100);
    calibrator.setAutoSaveCalibration(true);
    calibrator.loadCalibrationFromEEPROM();
    
    Serial.println("Data logger initialized");
    Serial.println("Commands: 'c' = calibrate, 'd' = debug, 'r' = reset");
}

void loop() {
    dataLogger.update();
    
    // Log data every 10 seconds
    static unsigned long lastLog = 0;
    if (millis() - lastLog >= 10000) {
        logSensorData();
        lastLog = millis();
    }
    
    // Handle commands
    if (Serial.available()) {
        char cmd = Serial.read();
        handleCommand(cmd);
        while (Serial.available()) Serial.read(); // Clear buffer
    }
    
    delay(100);
}

void logSensorData() {
    unsigned long timestamp = millis();
    
    Serial.print(timestamp);
    Serial.print(",");
    
    if (dataLogger.getSensorByName("dht")) {
        Serial.print(dataLogger.getValue<float>("dht", "temperature"));
        Serial.print(",");
        Serial.print(dataLogger.getValue<float>("dht", "humidity"));
        Serial.print(",");
    }
    
    if (dataLogger.getSensorByName("soil")) {
        Serial.print(dataLogger.getValue<float>("soil", "moisture"));
        Serial.print(",");
        Serial.print(dataLogger.getValue<float>("soil", "conductivity"));
    }
    
    Serial.println();
}

void handleCommand(char cmd) {
    switch (cmd) {
        case 'c':
            Serial.println("Starting soil moisture calibration...");
            calibrator.startInteractiveCalibration("soil", "moisture");
            break;
            
        case 'd':
            Serial.println("=== Debug Information ===");
            dataLogger.debugAll(true);
            break;
            
        case 'r':
            Serial.println("Resetting calibration...");
            calibrator.resetCalibration("soil", "moisture");
            break;
            
        default:
            Serial.println("Unknown command");
            break;
    }
}
```

## API Reference

### SensorModuleV2 Methods

#### Sensor Management
```cpp
void addSensor(const char* name, BaseSensV2* sensor);
void addSensor(const char* name, SensorCreateCallback callback);
BaseSensV2* getSensorByName(const char* name) const;
BaseSensV2* getSensor(uint8_t index) const;
const char* getSensorName(uint8_t index) const;
uint8_t getSensorCount() const;
```

#### Data Access
```cpp
template<typename T> T getValue(const char* sensorName, const char* valueKey) const;
bool isUpdated(const char* sensorName) const;
void printValues(const char* sensorName) const;
SensorProxy operator[](const char* sensorName);
```

#### Alert System
```cpp
bool setThreshold(const char* sensorName, const char* valueKey,
                 float lowThreshold, float highThreshold, AlertType type);
bool removeThreshold(const char* sensorName, const char* valueKey);
void removeAllThresholds();
AlertState getAlertState(const char* sensorName, const char* valueKey);
bool isAlertActive(const char* sensorName, const char* valueKey);
void acknowledgeAlert(const char* sensorName, const char* valueKey);
void acknowledgeAllAlerts();
void resetAlert(const char* sensorName, const char* valueKey);
void resetAllAlerts();
void setGlobalAlertCallback(AlertCallback callback);
bool setSensorAlertCallback(const char* sensorName, AlertCallback callback);
```

#### Filter System
```cpp
bool attachFilter(const char* sensorName, const char* valueKey,
                 FilterType type, FilterParams params);
bool updateFilter(const char* sensorName, const char* valueKey,
                 FilterType type, FilterParams params);
bool detachFilter(const char* sensorName, const char* valueKey);
void detachAllFilters();
float getFilteredValue(const char* sensorName, const char* valueKey);
bool hasFilter(const char* sensorName, const char* valueKey);
FilterType getFilterType(const char* sensorName, const char* valueKey);
```

### BaseSensV2 Methods

#### Core Interface
```cpp
virtual bool init() = 0;
virtual bool update() = 0;
virtual bool isUpdated() const;
```

#### Data Management
```cpp
template<typename T> T getValue(const char* key) const;
float getFloatValue(const char* key) const;
int getIntValue(const char* key) const;
const char* getStringValue(const char* key) const;
void updateValue(const char* key, float value);
void updateValue(const char* key, int value);
void updateValue(const char* key, const char* value);
bool hasValue(const char* key) const;
```

#### Configuration
```cpp
void addValueInfo(const char* key, const char* label, const char* unit, 
                 uint8_t precision, bool calibrable);
void setCalibration(const char* key, float offset, float scale);
```

## Best Practices

### 1. Sensor Initialization

Always initialize sensors in setup() and check return values:

```cpp
void setup() {
    sensorModule.addSensor("temp", &tempSensor);
    
    if (!sensorModule.init()) {
        Serial.println("Sensor initialization failed!");
        while(1); // Stop execution
    }
}
```

### 2. Update Intervals

Set appropriate update intervals for different sensor types:

```cpp
// Fast sensors (analog, digital)
analogSensor.setUpdateInterval(100);

// Medium sensors (I2C sensors)  
bme680.setUpdateInterval(2000);

// Slow sensors (complex calculations)
complexSensor.setUpdateInterval(5000);
```

### 3. Memory Management

Use sensor creation callbacks for automatic memory management:

```cpp
// Preferred: automatic management
sensorModule.addSensor("temp", []() -> BaseSensV2* {
    return new DHTSensV2(2, DHT22);
});

// Avoid: manual management
DHTSensV2* dht = new DHTSensV2(2, DHT22);
sensorModule.addSensor("temp", dht);
// Remember to delete dht somewhere!
```

### 4. Error Handling

Always check sensor availability before accessing data:

```cpp
BaseSensV2* sensor = sensorModule.getSensorByName("temp");
if (sensor && sensor->isUpdated()) {
    float temperature = sensor->getValue<float>("temperature");
    // Process temperature...
}
```

### 5. Filtering Guidelines

Choose appropriate filters for different signal types:

```cpp
// Noisy analog signals
FilterParams params;
params.movingAverage.windowSize = 10;
sensorModule.attachFilter("sensor", "value", FILTER_MOVING_AVERAGE, params);

// Signals with measurement uncertainty
params.kalman.processNoise = 0.01f;
params.kalman.measurementNoise = 0.1f;
sensorModule.attachFilter("sensor", "value", FILTER_KALMAN, params);

// Signals with outliers
params.median.windowSize = 5;
sensorModule.attachFilter("sensor", "value", FILTER_MEDIAN, params);
```

### 6. Alert Configuration

Set meaningful thresholds with appropriate hysteresis:

```cpp
// Temperature with 2°C hysteresis
sensorModule.setThreshold("temp", "celsius", 18.0, 25.0, ALERT_OUTSIDE);
sensorModule.setDefaultHysteresis(2000); // 2°C * 1000

// Quick response for critical values
sensorModule.setDefaultDebounceTime(100);
```

## Troubleshooting

### Common Issues

#### 1. Compilation Errors

**Issue**: `undefined reference to template function`
**Solution**: Ensure template methods are in header files, not implementation files.

**Issue**: `missing include files`
**Solution**: Check that all required `#define` statements are present:
```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"
```

#### 2. Runtime Issues

**Issue**: Sensor not updating
**Solution**: Check update intervals and initialization:
```cpp
if (!sensor.init()) {
    Serial.println("Sensor init failed");
}
sensor.setUpdateInterval(1000); // Ensure reasonable interval
```

**Issue**: Memory allocation failures
**Solution**: Monitor free heap and optimize sensor count:
```cpp
Serial.print("Free heap: ");
Serial.println(ESP.getFreeHeap()); // ESP32/ESP8266
```

#### 3. Data Issues

**Issue**: Incorrect sensor readings
**Solution**: Verify pin connections and voltage levels:
```cpp
// Check raw ADC values
int raw = sensorModule.getValue<int>("sensor", "raw");
float volt = sensorModule.getValue<float>("sensor", "volt");
Serial.print("Raw: "); Serial.print(raw);
Serial.print(", Volt: "); Serial.println(volt);
```

**Issue**: Filter not working
**Solution**: Verify filter attachment and parameters:
```cpp
if (sensorModule.hasFilter("sensor", "value")) {
    FilterType type = sensorModule.getFilterType("sensor", "value");
    Serial.print("Filter type: "); Serial.println(type);
}
```

### Debugging Tools

#### 1. Serial Debug Output

```cpp
// Enable verbose debugging
sensorModule.debugAll(true); // Show heap memory

// Debug specific sensor
sensorModule.debug("temp", "celsius", true);

// Pretty formatted output
sensorModule.debugPretty(1000);
```

#### 2. Memory Monitoring

```cpp
void checkMemory() {
    #ifdef ESP32
    Serial.print("Free heap: "); Serial.println(ESP.getFreeHeap());
    Serial.print("Min free heap: "); Serial.println(ESP.getMinFreeHeap());
    #endif
}
```

#### 3. Sensor Status Check

```cpp
void checkSensorStatus() {
    for (int i = 0; i < sensorModule.getSensorCount(); i++) {
        const char* name = sensorModule.getSensorName(i);
        BaseSensV2* sensor = sensorModule.getSensor(i);
        
        Serial.print("Sensor "); Serial.print(name);
        Serial.print(": Updated="); Serial.println(sensor->isUpdated());
    }
}
```

## Version Information

- **Version**: 2.0.0
- **Last Updated**: 2024
- **Compatibility**: Arduino IDE 1.8.0+, PlatformIO
- **Dependencies**: ArduinoJson 6.x, sensor-specific libraries

## Support

For issues, feature requests, or contributions:

1. Check the examples directory for reference implementations
2. Review the SensorImplementationGuide.md for custom sensor development
3. Use the debugging tools for troubleshooting
4. Check memory usage if experiencing stability issues

## License

This documentation and the SensorModuleV2 framework are part of the Kinematrix library system.