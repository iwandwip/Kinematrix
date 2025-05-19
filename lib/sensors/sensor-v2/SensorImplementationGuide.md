# Comprehensive Sensor Implementation Guide for SensorModuleV2

## Table of Contents

1. [Introduction](#1-introduction)
    - [Framework Overview](#framework-overview)
    - [Architecture Design](#architecture-design)
    - [Key Concepts](#key-concepts)

2. [Library Structure](#2-library-structure)
    - [Class Hierarchy](#class-hierarchy)
    - [Core Components](#core-components)
    - [Inheritance and Polymorphism](#inheritance-and-polymorphism)

3. [Creating a New Sensor](#3-creating-a-new-sensor)
    - [File Structure](#file-structure)
    - [Naming Conventions](#naming-conventions)
    - [Basic Template](#basic-template)
    - [Header File Implementation](#header-file-implementation)
    - [Source File Implementation](#source-file-implementation)

4. [Required Methods](#4-required-methods)
    - [Constructor and Destructor](#constructor-and-destructor)
    - [init() Method](#init-method)
    - [update() Method](#update-method)
    - [Custom Methods](#custom-methods)

5. [Working with Sensor Values](#5-working-with-sensor-values)
    - [Value Registration](#value-registration)
    - [Value Types](#value-types)
    - [Adding Values](#adding-values)
    - [Updating Values](#updating-values)
    - [Reading Values](#reading-values)
    - [Value Metadata](#value-metadata)

6. [Memory Management](#6-memory-management)
    - [Dynamic Memory Allocation](#dynamic-memory-allocation)
    - [String Handling](#string-handling)
    - [Memory Optimization](#memory-optimization)
    - [Avoiding Memory Leaks](#avoiding-memory-leaks)

7. [Sensor Calibration](#7-sensor-calibration)
    - [Calibration Types](#calibration-types)
    - [One-Point Calibration](#one-point-calibration)
    - [Two-Point Calibration](#two-point-calibration)
    - [Multi-Point Calibration](#multi-point-calibration)
    - [Custom Calibration Logic](#custom-calibration-logic)

8. [Integration with SensorCalibrationModuleV2](#8-integration-with-sensorcalibrationmodulev2)
    - [Registering Sensors](#registering-sensors)
    - [Discovery Process](#discovery-process)
    - [Calibration UI](#calibration-ui)
    - [Saving/Loading Calibration Data](#savingloading-calibration-data)

9. [Common Sensor Types Implementation](#9-common-sensor-types-implementation)
    - [Analog Sensors](#analog-sensors)
    - [Digital Sensors](#digital-sensors)
    - [I2C Sensors](#i2c-sensors)
    - [SPI Sensors](#spi-sensors)
    - [Serial Sensors](#serial-sensors)
    - [Custom Protocol Sensors](#custom-protocol-sensors)

10. [Advanced Features](#10-advanced-features)
    - [Custom Data Processing](#custom-data-processing)
    - [Filtering and Smoothing](#filtering-and-smoothing)
    - [Event Detection](#event-detection)
    - [Adaptive Sampling](#adaptive-sampling)
    - [Dynamic Value Management](#dynamic-value-management)

11. [Error Handling and Debugging](#11-error-handling-and-debugging)
    - [Error Detection](#error-detection)
    - [Status Flags](#status-flags)
    - [Debugging Techniques](#debugging-techniques)
    - [Serial Debug Output](#serial-debug-output)
    - [Common Issues and Solutions](#common-issues-and-solutions)

12. [Power Management](#12-power-management)
    - [Low-Power Operation](#low-power-operation)
    - [Sleep Modes](#sleep-modes)
    - [Power-Aware Sampling](#power-aware-sampling)

13. [Creating a Distributable Library](#13-creating-a-distributable-library)
    - [Library Structure](#library-structure-1)
    - [Documentation](#documentation)
    - [Examples](#examples)
    - [Versioning](#versioning)
    - [GitHub Repository Setup](#github-repository-setup)
    - [Arduino Library Manager Compatibility](#arduino-library-manager-compatibility)

14. [Complete Implementation Examples](#14-complete-implementation-examples)
    - [Temperature/Humidity Sensor](#temperaturehumidity-sensor)
    - [Soil Moisture Sensor](#soil-moisture-sensor)
    - [Accelerometer](#accelerometer)
    - [Distance Sensor](#distance-sensor)
    - [Light Sensor](#light-sensor)

15. [Testing and Validation](#15-testing-and-validation)
    - [Unit Testing](#unit-testing)
    - [Integration Testing](#integration-testing)
    - [Validation Strategies](#validation-strategies)
    - [Performance Testing](#performance-testing)

16. [Performance Optimization](#16-performance-optimization)
    - [CPU Efficiency](#cpu-efficiency)
    - [Memory Usage](#memory-usage)
    - [Bandwidth Optimization](#bandwidth-optimization)

17. [Troubleshooting Guide](#17-troubleshooting-guide)
    - [Common Problems](#common-problems)
    - [Debugging Process](#debugging-process)
    - [Error Codes](#error-codes)

18. [Appendix](#18-appendix)
    - [Glossary](#glossary)
    - [Reference Documentation](#reference-documentation)
    - [External Resources](#external-resources)

---

## 1. Introduction

### Framework Overview

The SensorModuleV2 framework is a comprehensive system for implementing, managing, and calibrating sensors in embedded
systems. It's designed to solve the common challenges faced when working with various sensors:

- **Standardization**: Provides a uniform interface for different sensor types
- **Metadata**: Rich metadata for each sensor value (units, precision, labels)
- **Calibration**: Built-in support for various calibration methods
- **Dynamic Values**: Support for multiple values per sensor with different types
- **Memory Efficiency**: Optimized for resource-constrained environments

This framework is particularly valuable for projects that:

- Utilize multiple sensors of different types
- Require calibration capabilities
- Need to manage and process various sensor values
- Have needs for data formatting and presentation

### Architecture Design

The framework follows a hierarchical design with several key components:

1. **BaseSensV2**: The abstract base class that all sensors inherit from
2. **SensorModuleV2**: Manager class for multiple sensors
3. **SensorCalibrationModuleV2**: Extension that adds calibration capabilities
4. **Sensor Implementations**: Custom sensors you create using this framework

The architecture uses a composition pattern where each sensor can have multiple values, each with associated metadata.
This enables flexible representation of complex sensor data.

### Key Concepts

Before implementing your first sensor, understanding these key concepts is essential:

1. **Sensor Value**: A specific measurement or state from a sensor (e.g., temperature, humidity)
2. **Value Metadata**: Information about a value (label, unit, precision, type)
3. **Calibration**: Process of adjusting raw values to match known references
4. **Update Cycle**: Regular process of reading sensor values
5. **Value Discovery**: Automatic detection of available sensor values

## 2. Library Structure

### Class Hierarchy

```
BaseSensV2                      (Abstract base class for all sensors)
├── YourCustomSensorV2          (Your sensor implementation)
├── AnalogSensV2                (Framework-provided analog sensor)
├── DHTSensV2                   (Framework-provided DHT sensor)
└── ...                         (Other sensor implementations)

SensorModuleV2                  (Manager for multiple sensors)
└── SensorCalibrationModuleV2   (Extended manager with calibration features)
```

### Core Components

1. **BaseSensV2**: Abstract base class defining the interface for all sensors
    - Manages value registration and metadata
    - Provides value access methods
    - Defines abstract methods that must be implemented

2. **SensorModuleV2**: Container and manager for multiple sensors
    - Manages sensor initialization and updates
    - Provides access to sensors and their values
    - Handles JSON document management

3. **SensorCalibrationModuleV2**: Extends SensorModuleV2 with calibration
    - Provides calibration interface and storage
    - Implements interactive calibration via serial
    - Manages calibration profiles

### Inheritance and Polymorphism

The framework leverages C++ inheritance and polymorphism:

- **Inheritance**: Your sensors inherit from BaseSensV2, gaining its capabilities
- **Polymorphism**: SensorModuleV2 can manage any sensor type derived from BaseSensV2
- **Abstract Methods**: init() and update() are abstract, requiring implementation in derived classes
- **Method Overrides**: You can override methods like getValueByKey() for custom behavior

## 3. Creating a New Sensor

### File Structure

For each new sensor, you typically need two files:

```
YourSensorV2.h     (Header file with class declaration)
YourSensorV2.cpp   (Source file with method implementations)
```

### Naming Conventions

Follow these naming conventions for consistency:

- **Class Names**: Use CamelCase and end with V2 (e.g., `TemperatureSensorV2`)
- **File Names**: Match class name (e.g., `TemperatureSensorV2.h`)
- **Value Keys**: Use camelCase or snake_case consistently (e.g., `temperature` or `temp`)
- **Method Names**: Use camelCase starting with a verb (e.g., `getValue`, `setUpdateInterval`)

### Basic Template

Start by copying the CustomSensorTemplateV2 files and renaming them to your sensor name.

### Header File Implementation

A typical header file has this structure:

```cpp
#pragma once

#ifndef YOUR_SENSOR_V2_H
#define YOUR_SENSOR_V2_H

#include "Arduino.h"
#include "SensorModuleV2.h"
#include "OtherRequiredLibraries.h"  // Any libraries your sensor needs

class YourSensorV2 : public BaseSensV2 {
private:
    // Private variables for your sensor
    uint8_t _pin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    
    // Sensor-specific variables
    float _calibrationFactor;
    
    // Private helper methods
    float calculateDerivedValue(float rawValue);

public:
    // Constructor and destructor
    YourSensorV2(uint8_t pin, float parameter2 = defaultValue);
    virtual ~YourSensorV2();
    
    // Required overrides from BaseSensV2
    bool init() override;
    bool update() override;
    
    // Getter/setter methods
    void setUpdateInterval(uint32_t interval);
    void setCalibrationFactor(float factor);
    
    // Custom methods for your sensor
    float calculateSpecialValue();
    void performSensorSpecificFunction();
};

#endif // YOUR_SENSOR_V2_H
```

### Source File Implementation

The source file implements all methods declared in the header:

```cpp
#include "YourSensorV2.h"

YourSensorV2::YourSensorV2(uint8_t pin, float parameter2)
    : _pin(pin),
      _updateTimer(0),
      _updateInterval(1000),
      _calibrationFactor(parameter2) {
    
    // Register sensor values (key, label, unit, precision, type)
    addValueInfo("raw", "Raw Value", "", 0, TYPE_INT);
    addValueInfo("processed", "Processed Value", "units", 1, TYPE_FLOAT);
    // Add more values as needed
}

YourSensorV2::~YourSensorV2() {
    // Clean up any resources (usually handled by parent class)
}

bool YourSensorV2::init() {
    // Set up hardware
    pinMode(_pin, INPUT);
    
    // Initialize sensor hardware if needed
    // Example: _sensorHardware.begin();
    
    // Initialize values
    updateValue("raw", 0);
    updateValue("processed", 0.0f);
    
    return true; // Return false if initialization fails
}

bool YourSensorV2::update() {
    // Check if it's time to update
    if (millis() - _updateTimer >= _updateInterval) {
        // Read raw value
        int rawValue = analogRead(_pin); // Example reading
        
        // Process value
        float processedValue = calculateDerivedValue(rawValue);
        
        // Update stored values
        updateValue("raw", rawValue);
        updateValue("processed", processedValue);
        
        _updateTimer = millis();
        return true; // Indicates values were updated
    }
    return false; // No update performed
}

float YourSensorV2::calculateDerivedValue(float rawValue) {
    // Example calculation
    return rawValue * _calibrationFactor;
}

void YourSensorV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void YourSensorV2::setCalibrationFactor(float factor) {
    _calibrationFactor = factor;
}

float YourSensorV2::calculateSpecialValue() {
    float processedValue = getFloatValue("processed");
    // Some special calculation
    return processedValue * 2;
}

void YourSensorV2::performSensorSpecificFunction() {
    // Implement sensor-specific functionality
}
```

## 4. Required Methods

### Constructor and Destructor

The constructor is where you:

1. Initialize sensor parameters
2. Set default values for internal variables
3. Register sensor values with addValueInfo()

```cpp
YourSensorV2::YourSensorV2(uint8_t pin)
    : _pin(pin),
      _updateTimer(0),
      _updateInterval(1000) {
    
    // Register all sensor values
    addValueInfo("value1", "Value One", "units", 2, TYPE_FLOAT);
    addValueInfo("value2", "Value Two", "", 0, TYPE_INT);
}
```

The destructor is usually minimal since BaseSensV2 handles most cleanup:

```cpp
YourSensorV2::~YourSensorV2() {
    // Any specific cleanup needed
}
```

### init() Method

The init() method is called once during system initialization and should:

1. Set up hardware (pins, communication buses)
2. Initialize sensor components
3. Set initial values
4. Return success/failure status

```cpp
bool YourSensorV2::init() {
    // Set pin modes
    pinMode(_pin, INPUT);
    
    // Initialize sensor-specific hardware
    bool sensorInitSuccess = _sensorHardware.begin();
    
    // Set initial values
    updateValue("value1", 0.0f);
    updateValue("value2", 0);
    
    // Return success/failure
    return sensorInitSuccess;
}
```

### update() Method

The update() method is called periodically to:

1. Read new sensor values
2. Process raw values into meaningful measurements
3. Update stored values
4. Return whether an update occurred

A good update method includes:

- Timing control to prevent excessive updates
- Error handling for sensor reads
- Processing of raw values
- Update of stored values

```cpp
bool YourSensorV2::update() {
    // Check update interval
    if (millis() - _updateTimer >= _updateInterval) {
        
        // Read sensor
        int rawReading = analogRead(_pin);
        
        // Process values
        float processedValue = convertRawToValue(rawReading);
        
        // Check for errors
        if (processedValue < 0) {
            // Handle error case
            updateValue("error", 1);
            return false;
        }
        
        // Update stored values
        updateValue("raw", rawReading);
        updateValue("value", processedValue);
        
        // Reset timer
        _updateTimer = millis();
        return true;
    }
    return false;
}
```

### Custom Methods

Beyond the required methods, you'll typically implement:

- Getter/setter methods for configuration
- Processing methods for calculations
- Sensor-specific utility functions

```cpp
// Setter for configuration
void YourSensorV2::setThreshold(float threshold) {
_threshold = threshold;
}

// Processing method
float YourSensorV2::calculateMovingAverage() {
// Implementation
}

// Utility function
bool YourSensorV2::isValueAboveThreshold() {
float currentValue = getFloatValue("value");
return currentValue > _threshold;
}
```

## 5. Working with Sensor Values

### Value Registration

Values must be registered before use with the addValueInfo() method:

```cpp
addValueInfo(key, label, unit, precision, type);
```

Parameters:

- **key**: String identifier used in code (e.g., "temperature")
- **label**: Human-readable name for UI display (e.g., "Temperature")
- **unit**: Measurement unit (e.g., "°C")
- **precision**: Decimal places to display for floats (0-6)
- **type**: Value type (TYPE_FLOAT, TYPE_INT, TYPE_STRING)

### Value Types

The framework supports three value types:

1. **TYPE_FLOAT**: For decimal values (temperatures, voltages)
   ```cpp
   addValueInfo("temperature", "Temperature", "°C", 1, TYPE_FLOAT);
   ```

2. **TYPE_INT**: For whole numbers (counts, digital states)
   ```cpp
   addValueInfo("counter", "Count", "", 0, TYPE_INT);
   ```

3. **TYPE_STRING**: For text values (status messages)
   ```cpp
   addValueInfo("status", "Sensor Status", "", 0, TYPE_STRING);
   ```

### Adding Values

Values are typically added in the constructor, but can be added dynamically:

```cpp
// In constructor
addValueInfo("basic", "Basic Value", "units", 2, TYPE_FLOAT);

// Dynamically based on configuration
if (_enableAdvancedMode) {
addValueInfo("advanced", "Advanced Value", "units", 2, TYPE_FLOAT);
}
```

### Updating Values

Update values using the appropriate type-specific method:

```cpp
// Float values
updateValue("temperature", 25.5f);

// Integer values
updateValue("counter", 42);

// String values
updateValue("status", "OK");
```

This is typically done in the update() method after reading sensor data.

### Reading Values

To read values within your sensor class:

```cpp
// Reading float values
float temp = getFloatValue("temperature");

// Reading integer values
int count = getIntValue("counter");

// Reading string values
const char* status = getStringValue("status");
```

From outside the sensor (via SensorModuleV2):

```cpp
// Raw values
float temperature = sensorModule.getRawValue("temperatureSensor", "temperature");

// Calibrated values
float calibratedTemp = sensorModule.getCalibratedValue("temperatureSensor", "temperature");
```

### Value Metadata

You can access value metadata to retrieve information about a value:

```cpp
// Get value info
const SensorValueInfo* info = getValueInfoByKey("temperature");

if (info) {
Serial.print("Label: ");
Serial.println(info->label);

Serial.print("Unit: ");
Serial.println(info->unit);

Serial.print("Precision: ");
Serial.println(info->precision);
}
```

## 6. Memory Management

### Dynamic Memory Allocation

The framework uses dynamic memory for:

- Storing value information
- Storing value keys and metadata
- JSON document representation

Best practices:

- Allocate memory in constructor when possible
- Avoid frequent allocation/deallocation during operation
- Be cautious with string operations

### String Handling

String handling is a common source of memory issues. Use these techniques:

- Use `strdup()` for making string copies:
  ```cpp
  char* nameCopy = strdup(name);
  ```

- Always check allocations:
  ```cpp
  char* nameCopy = strdup(name);
  if (nameCopy == nullptr) {
    // Handle allocation failure
  }
  ```

- Free memory when no longer needed:
  ```cpp
  if (oldString != nullptr) {
    free(oldString);
    oldString = nullptr;
  }
  ```

- Use static strings where possible:
  ```cpp
  // Instead of:
  addValueInfo("temp", "Temperature", "°C", 1, TYPE_FLOAT);
  
  // Consider:
  static const char KEY_TEMP[] = "temp";
  static const char LABEL_TEMP[] = "Temperature";
  static const char UNIT_TEMP[] = "°C";
  addValueInfo(KEY_TEMP, LABEL_TEMP, UNIT_TEMP, 1, TYPE_FLOAT);
  ```

### Memory Optimization

Techniques for reducing memory usage:

1. **Reuse Buffers**: For temporary string operations
   ```cpp
   char buffer[64]; // Reuse for different string operations
   ```

2. **Minimize String Duplication**: Use references where possible
   ```cpp
   // Instead of copying strings multiple times, 
   // keep references to original strings when safe
   ```

3. **Limit EEPROM Usage**: Only save essential calibration data

4. **Static Configuration**: Use compile-time configuration when possible
   ```cpp
   #define SENSOR_UPDATE_INTERVAL 1000
   ```

5. **Avoid String Concatenation**: Use separate print statements
   ```cpp
   // Instead of:
   String message = "Value: " + String(value) + " units";
   
   // Use:
   Serial.print("Value: ");
   Serial.print(value);
   Serial.println(" units");
   ```

### Avoiding Memory Leaks

Common sources of memory leaks:

1. **Failure to free allocated memory**:
   ```cpp
   // Allocate memory
   char* temp = strdup(name);
   
   // Later, when no longer needed:
   free(temp);
   temp = nullptr;
   ```

2. **Overwriting pointers without freeing**:
   ```cpp
   // Wrong:
   _name = strdup("NewName"); // Memory leak if _name already points to allocated memory
   
   // Correct:
   if (_name != nullptr) {
     free(_name);
   }
   _name = strdup("NewName");
   ```

3. **Not handling allocation failures**:
   ```cpp
   void* newBuffer = realloc(oldBuffer, newSize);
   if (newBuffer == nullptr) {
     // Keep using oldBuffer, don't overwrite it
     // Handle the allocation failure
     return false;
   }
   oldBuffer = newBuffer; // Only update if allocation succeeded
   ```

## 7. Sensor Calibration

### Calibration Types

The framework supports several calibration types:

1. **No Calibration (CALIBRATION_NONE)**: Raw values used directly
2. **One-Point Calibration (CALIBRATION_ONE_POINT)**: Simple scaling
3. **Two-Point Calibration (CALIBRATION_TWO_POINT)**: Linear equation (y = mx + b)
4. **Multi-Point Calibration (CALIBRATION_MULTI_POINT)**: Linear interpolation or polynomial fitting

### One-Point Calibration

One-point calibration applies a simple scaling factor:

```
calibrated_value = raw_value * scale_factor
```

Where scale_factor = reference_value / raw_value

Use cases:

- Simple offset adjustments
- Percentage-based measurements
- When the sensor response is linear through zero

### Two-Point Calibration

Two-point calibration fits a line between two known points:

```
calibrated_value = slope * raw_value + offset
```

Where:

- slope = (known_value2 - known_value1) / (raw_value2 - raw_value1)
- offset = known_value1 - slope * raw_value1

Use cases:

- Linear sensors with offset and gain
- Temperature sensors
- Pressure sensors

### Multi-Point Calibration

Multi-point calibration supports two methods:

1. **Linear Interpolation**: Connects calibration points with straight lines
    - Good for non-linear responses
    - Preserves exact calibration points
    - No smoothing between points

2. **Polynomial Fitting**: Fits a polynomial curve to all points
    - Smoother curve
    - May not exactly hit calibration points
    - Better for sensors with smooth non-linearity

### Custom Calibration Logic

For sensors requiring special calibration:

1. Create a custom calibration class:
   ```cpp
   class CustomCalibrator {
   public:
       float calibrate(float rawValue) {
           // Custom calibration logic
       }
   };
   ```

2. Integrate with your sensor:
   ```cpp
   float YourSensorV2::getCustomCalibratedValue() {
       float raw = getFloatValue("raw");
       return _customCalibrator.calibrate(raw);
   }
   ```

## 8. Integration with SensorCalibrationModuleV2

### Registering Sensors

To use a sensor with SensorCalibrationModuleV2:

```cpp
// Create sensor and calibration module
YourSensorV2 sensor(A0);
SensorCalibrationModuleV2 sensorModule;

// Add sensor to module with name
sensorModule.addSensor("sensorName", &sensor);

// Initialize the system
sensorModule.init();
```

### Discovery Process

The calibration module can auto-discover sensor values:

```cpp
// Discover all available values across all sensors
sensorModule.discoverSensorValues();
```

This creates calibration entries for each value automatically.

### Calibration UI

Enable interactive calibration via serial:

```cpp
// Start calibration mode with timeout
sensorModule.startCalibrationMode(&Serial, 300000); // 5 minute timeout
```

The calibration UI allows:

- Viewing all available sensor values
- Selecting values for calibration
- Performing different calibration types
- Saving/loading calibration profiles

### Saving/Loading Calibration Data

Save all calibration data to EEPROM:

```cpp
// Save to default location
sensorModule.saveAllCalibrations();

// Save to custom address
sensorModule.saveAllCalibrations(100); // Start at EEPROM address 100
```

Load previously saved calibration:

```cpp
// Load from default location
sensorModule.loadAllCalibrations();

// Load from custom address
sensorModule.loadAllCalibrations(100);
```

## 9. Common Sensor Types Implementation

### Analog Sensors

Analog sensors connect to ADC pins and provide voltage readings.

```cpp
class AnalogSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    float _vref;
    int _adcResolution;
    
public:
    AnalogSensorV2(uint8_t pin, float vref = 5.0, int adcResolution = 1023)
        : _pin(pin), _vref(vref), _adcResolution(adcResolution) {
        
        addValueInfo("raw", "Raw ADC", "", 0, TYPE_INT);
        addValueInfo("voltage", "Voltage", "V", 3, TYPE_FLOAT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        updateValue("raw", 0);
        updateValue("voltage", 0.0f);
        return true;
    }
    
    bool update() override {
        int rawValue = analogRead(_pin);
        float voltage = rawValue * (_vref / _adcResolution);
        
        updateValue("raw", rawValue);
        updateValue("voltage", voltage);
        
        return true;
    }
};
```

### Digital Sensors

Digital sensors typically use GPIO pins for binary or pulse-based data.

```cpp
class DigitalSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    bool _pullup;
    bool _invertLogic;
    
public:
    DigitalSensorV2(uint8_t pin, bool pullup = false, bool invertLogic = false)
        : _pin(pin), _pullup(pullup), _invertLogic(invertLogic) {
        
        addValueInfo("state", "State", "", 0, TYPE_INT);
        addValueInfo("pulseCount", "Pulse Count", "", 0, TYPE_INT);
    }
    
    bool init() override {
        pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
        updateValue("state", 0);
        updateValue("pulseCount", 0);
        return true;
    }
    
    bool update() override {
        int state = digitalRead(_pin);
        if (_invertLogic) state = !state;
        
        // Update state
        updateValue("state", state);
        
        // Example: count pulses
        static int lastState = 0;
        if (state == HIGH && lastState == LOW) {
            int count = getIntValue("pulseCount");
            updateValue("pulseCount", count + 1);
        }
        lastState = state;
        
        return true;
    }
};
```

### I2C Sensors

I2C sensors communicate over the I2C bus, often using existing libraries.

```cpp
#include <Wire.h>
#include "BME280Library.h" // Example I2C sensor library

class BME280SensorV2 : public BaseSensV2 {
private:
    BME280 _bme;
    uint8_t _address;
    
public:
    BME280SensorV2(uint8_t address = 0x76) 
        : _address(address) {
        
        addValueInfo("temperature", "Temperature", "°C", 1, TYPE_FLOAT);
        addValueInfo("humidity", "Humidity", "%", 0, TYPE_FLOAT);
        addValueInfo("pressure", "Pressure", "hPa", 1, TYPE_FLOAT);
    }
    
    bool init() override {
        Wire.begin();
        bool success = _bme.begin(_address);
        
        updateValue("temperature", 0.0f);
        updateValue("humidity", 0.0f);
        updateValue("pressure", 0.0f);
        
        return success;
    }
    
    bool update() override {
        float temp = _bme.readTemperature();
        float humidity = _bme.readHumidity();
        float pressure = _bme.readPressure() / 100.0F; // Convert Pa to hPa
        
        updateValue("temperature", temp);
        updateValue("humidity", humidity);
        updateValue("pressure", pressure);
        
        return true;
    }
};
```

### SPI Sensors

SPI sensors use the SPI bus and often require additional chip select pins.

```cpp
#include <SPI.h>
#include "MAX31855Library.h" // Example SPI sensor library

class MAX31855SensorV2 : public BaseSensV2 {
private:
    uint8_t _csPin;
    MAX31855 _thermocouple;
    
public:
    MAX31855SensorV2(uint8_t csPin) 
        : _csPin(csPin), _thermocouple(csPin) {
        
        addValueInfo("temperature", "Temperature", "°C", 1, TYPE_FLOAT);
        addValueInfo("internalTemp", "Internal Temp", "°C", 1, TYPE_FLOAT);
        addValueInfo("faultCode", "Fault Code", "", 0, TYPE_INT);
    }
    
    bool init() override {
        SPI.begin();
        pinMode(_csPin, OUTPUT);
        digitalWrite(_csPin, HIGH);
        
        _thermocouple.begin();
        
        updateValue("temperature", 0.0f);
        updateValue("internalTemp", 0.0f);
        updateValue("faultCode", 0);
        
        return true;
    }
    
    bool update() override {
        float temp = _thermocouple.readCelsius();
        float internalTemp = _thermocouple.readInternalC();
        int fault = _thermocouple.readError();
        
        updateValue("temperature", temp);
        updateValue("internalTemp", internalTemp);
        updateValue("faultCode", fault);
        
        return true;
    }
};
```

### Serial Sensors

Serial sensors communicate over UART/Serial ports.

```cpp
#include "SoftwareSerial.h"

class CO2SerialSensorV2 : public BaseSensV2 {
private:
    SoftwareSerial _serial;
    uint8_t _rxPin, _txPin;
    
public:
    CO2SerialSensorV2(uint8_t rxPin, uint8_t txPin) 
        : _rxPin(rxPin), _txPin(txPin), _serial(rxPin, txPin) {
        
        addValueInfo("co2", "CO2 Level", "ppm", 0, TYPE_INT);
        addValueInfo("temperature", "Temperature", "°C", 1, TYPE_FLOAT);
    }
    
    bool init() override {
        _serial.begin(9600);
        
        updateValue("co2", 0);
        updateValue("temperature", 0.0f);
        
        return true;
    }
    
    bool update() override {
        if (_serial.available() >= 9) {
            if (_serial.read() == 0xFF) {
                uint8_t buffer[8];
                _serial.readBytes(buffer, 8);
                
                if (buffer[0] == 0x86) {
                    int co2 = (buffer[1] << 8) | buffer[2];
                    float temp = buffer[3] - 40.0f;
                    
                    updateValue("co2", co2);
                    updateValue("temperature", temp);
                    
                    return true;
                }
            }
        }
        return false;
    }
};
```

### Custom Protocol Sensors

Some sensors use custom protocols or timing-critical communication.

```cpp
class OneWireTemperatureSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    
    // Function to implement the OneWire protocol
    float readOneWireTemp() {
        // Custom implementation of the 1-Wire protocol
        // ...
    }
    
public:
    OneWireTemperatureSensorV2(uint8_t pin) : _pin(pin) {
        addValueInfo("temperature", "Temperature", "°C", 1, TYPE_FLOAT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT); // Will be toggled as needed
        updateValue("temperature", 0.0f);
        return true;
    }
    
    bool update() override {
        float temp = readOneWireTemp();
        updateValue("temperature", temp);
        return true;
    }
};
```

## 10. Advanced Features

### Custom Data Processing

Example: Computing derived values from raw readings

```cpp
class PressureSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    float _seaLevelPressure;
    float _elevation;
    
    float calculateAltitude(float pressure) {
        return 44330.0 * (1.0 - pow(pressure / _seaLevelPressure, 0.1903));
    }
    
public:
    PressureSensorV2(uint8_t pin, float elevation = 0.0) 
        : _pin(pin), _seaLevelPressure(1013.25), _elevation(elevation) {
        
        addValueInfo("pressure", "Pressure", "hPa", 1, TYPE_FLOAT);
        addValueInfo("altitude", "Altitude", "m", 1, TYPE_FLOAT);
        addValueInfo("seaLevelEquiv", "Sea Level Pressure", "hPa", 1, TYPE_FLOAT);
    }
    
    bool update() override {
        // Read raw pressure value
        float pressure = readPressure();
        
        // Calculate derived values
        float altitude = calculateAltitude(pressure);
        float seaLevelEquiv = pressure / pow(1.0 - (_elevation/44330.0), 5.255);
        
        // Update all values
        updateValue("pressure", pressure);
        updateValue("altitude", altitude);
        updateValue("seaLevelEquiv", seaLevelEquiv);
        
        return true;
    }
    
    void setSeaLevelPressure(float pressure) {
        _seaLevelPressure = pressure;
    }
    
    void setElevation(float elevation) {
        _elevation = elevation;
    }
};
```

### Filtering and Smoothing

Example: Implementing a moving average filter

```cpp
class FilteredSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    
    // Moving average filter parameters
    static const int WINDOW_SIZE = 10;
    float _window[WINDOW_SIZE];
    int _windowIndex = 0;
    bool _windowFilled = false;
    
    float calculateAverage() {
        float sum = 0.0;
        int count = _windowFilled ? WINDOW_SIZE : _windowIndex;
        
        for (int i = 0; i < count; i++) {
            sum += _window[i];
        }
        
        return sum / count;
    }
    
public:
    FilteredSensorV2(uint8_t pin) : _pin(pin) {
        addValueInfo("raw", "Raw Value", "", 2, TYPE_FLOAT);
        addValueInfo("filtered", "Filtered Value", "", 2, TYPE_FLOAT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        
        // Initialize window
        for (int i = 0; i < WINDOW_SIZE; i++) {
            _window[i] = 0.0;
        }
        
        updateValue("raw", 0.0f);
        updateValue("filtered", 0.0f);
        
        return true;
    }
    
    bool update() override {
        // Read raw value
        float rawValue = analogRead(_pin) * (5.0 / 1023.0);
        
        // Update filter window
        _window[_windowIndex] = rawValue;
        _windowIndex = (_windowIndex + 1) % WINDOW_SIZE;
        
        if (_windowIndex == 0) {
            _windowFilled = true;
        }
        
        // Calculate filtered value
        float filteredValue = calculateAverage();
        
        // Update values
        updateValue("raw", rawValue);
        updateValue("filtered", filteredValue);
        
        return true;
    }
};
```

### Event Detection

Example: Threshold-based event detection

```cpp
class MotionSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    float _threshold;
    unsigned long _eventDuration;
    unsigned long _lastEventTime;
    
public:
    MotionSensorV2(uint8_t pin, float threshold = 0.5, unsigned long duration = 5000) 
        : _pin(pin), _threshold(threshold), _eventDuration(duration), _lastEventTime(0) {
        
        addValueInfo("level", "Motion Level", "", 2, TYPE_FLOAT);
        addValueInfo("detected", "Motion Detected", "", 0, TYPE_INT);
        addValueInfo("eventCount", "Event Count", "", 0, TYPE_INT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        updateValue("level", 0.0f);
        updateValue("detected", 0);
        updateValue("eventCount", 0);
        return true;
    }
    
    bool update() override {
        // Read motion level
        float level = analogRead(_pin) * (5.0 / 1023.0);
        
        // Update level value
        updateValue("level", level);
        
        // Detect events
        bool isActive = (level > _threshold);
        bool wasActive = (getIntValue("detected") > 0);
        
        if (isActive) {
            // Motion above threshold
            _lastEventTime = millis();
            
            if (!wasActive) {
                // This is a new event
                int count = getIntValue("eventCount");
                updateValue("eventCount", count + 1);
            }
            
            updateValue("detected", 1);
        } else {
            // Check if event has expired
            if (millis() - _lastEventTime > _eventDuration) {
                updateValue("detected", 0);
            }
        }
        
        return true;
    }
    
    void setThreshold(float threshold) {
        _threshold = threshold;
    }
    
    void setEventDuration(unsigned long duration) {
        _eventDuration = duration;
    }
};
```

### Adaptive Sampling

Example: Sensor with variable sampling rate based on activity

```cpp
class AdaptiveSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    unsigned long _baseInterval;
    unsigned long _fastInterval;
    float _activityThreshold;
    unsigned long _lastUpdateTime;
    
public:
    AdaptiveSensorV2(uint8_t pin) 
        : _pin(pin),
          _baseInterval(5000),     // Normal: 5 seconds
          _fastInterval(100),      // Active: 100 ms
          _activityThreshold(2.0), // Threshold for "activity"
          _lastUpdateTime(0) {
        
        addValueInfo("value", "Sensor Value", "", 2, TYPE_FLOAT);
        addValueInfo("delta", "Change Rate", "", 2, TYPE_FLOAT);
        addValueInfo("sampleRate", "Sample Rate", "Hz", 2, TYPE_FLOAT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        updateValue("value", 0.0f);
        updateValue("delta", 0.0f);
        updateValue("sampleRate", 1000.0f / _baseInterval);
        return true;
    }
    
    bool update() override {
        unsigned long now = millis();
        unsigned long elapsed = now - _lastUpdateTime;
        
        // Get current settings and values
        float lastValue = getFloatValue("value");
        float delta = getFloatValue("delta");
        float currentRate = getFloatValue("sampleRate");
        
        // Determine appropriate interval
        unsigned long interval = (abs(delta) > _activityThreshold) 
                               ? _fastInterval : _baseInterval;
        
        // Check if it's time to update
        if (elapsed >= interval) {
            // Read new value
            float newValue = analogRead(_pin) * (5.0 / 1023.0);
            
            // Calculate delta (change rate)
            delta = newValue - lastValue;
            
            // Calculate actual sample rate
            float actualRate = 1000.0f / elapsed;
            
            // Update all values
            updateValue("value", newValue);
            updateValue("delta", delta);
            updateValue("sampleRate", actualRate);
            
            _lastUpdateTime = now;
            return true;
        }
        
        return false;
    }
    
    void setIntervals(unsigned long baseInterval, unsigned long fastInterval) {
        _baseInterval = baseInterval;
        _fastInterval = fastInterval;
    }
    
    void setActivityThreshold(float threshold) {
        _activityThreshold = threshold;
    }
};
```

### Dynamic Value Management

Example: Sensor that changes available values based on mode

```cpp
class MultiModeSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    int _mode;
    bool _valuesRegistered[3];
    
public:
    enum Modes {
        BASIC_MODE = 0,
        ADVANCED_MODE = 1,
        DIAGNOSTIC_MODE = 2
    };
    
    MultiModeSensorV2(uint8_t pin, int initialMode = BASIC_MODE) 
        : _pin(pin), _mode(initialMode) {
        
        // Values always available
        addValueInfo("raw", "Raw Value", "", 0, TYPE_INT);
        _valuesRegistered[0] = true;
        
        // Register mode-specific values
        updateValueRegistration();
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        updateValue("raw", 0);
        
        // Initialize any enabled mode-specific values
        if (_valuesRegistered[1]) {
            updateValue("processed", 0.0f);
        }
        
        if (_valuesRegistered[2]) {
            updateValue("min", 0);
            updateValue("max", 0);
            updateValue("avg", 0.0f);
        }
        
        return true;
    }
    
    bool update() override {
        // Read basic value
        int rawValue = analogRead(_pin);
        updateValue("raw", rawValue);
        
        // Update mode-specific values
        if (_mode >= ADVANCED_MODE && _valuesRegistered[1]) {
            float processed = calculateProcessedValue(rawValue);
            updateValue("processed", processed);
        }
        
        if (_mode >= DIAGNOSTIC_MODE && _valuesRegistered[2]) {
            // Update diagnostic values
            int currentMin = getIntValue("min");
            int currentMax = getIntValue("max");
            
            if (rawValue < currentMin || currentMin == 0) {
                updateValue("min", rawValue);
            }
            
            if (rawValue > currentMax) {
                updateValue("max", rawValue);
            }
            
            // Simple running average
            float currentAvg = getFloatValue("avg");
            if (currentAvg == 0) {
                updateValue("avg", (float)rawValue);
            } else {
                updateValue("avg", currentAvg * 0.95f + rawValue * 0.05f);
            }
        }
        
        return true;
    }
    
    void setMode(int newMode) {
        if (newMode != _mode) {
            _mode = newMode;
            updateValueRegistration();
        }
    }
    
private:
    float calculateProcessedValue(int raw) {
        // Example processing
        return raw * 0.01f;
    }
    
    void updateValueRegistration() {
        // Register/unregister mode-specific values
        
        // Advanced mode values
        if (_mode >= ADVANCED_MODE && !_valuesRegistered[1]) {
            addValueInfo("processed", "Processed Value", "", 2, TYPE_FLOAT);
            _valuesRegistered[1] = true;
        }
        
        // Diagnostic mode values
        if (_mode >= DIAGNOSTIC_MODE && !_valuesRegistered[2]) {
            addValueInfo("min", "Minimum", "", 0, TYPE_INT);
            addValueInfo("max", "Maximum", "", 0, TYPE_INT);
            addValueInfo("avg", "Average", "", 2, TYPE_FLOAT);
            _valuesRegistered[2] = true;
        }
    }
};
```

## 11. Error Handling and Debugging

### Error Detection

Implement robust error detection in your sensors:

```cpp
class RobustSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    int _errorCount;
    unsigned long _lastSuccessTime;
    
public:
    RobustSensorV2(uint8_t pin) 
        : _pin(pin), _errorCount(0), _lastSuccessTime(0) {
        
        addValueInfo("value", "Sensor Value", "", 2, TYPE_FLOAT);
        addValueInfo("status", "Status Code", "", 0, TYPE_INT);
        addValueInfo("errorCount", "Error Count", "", 0, TYPE_INT);
        addValueInfo("uptime", "Uptime", "s", 0, TYPE_INT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        
        // Initialize with error state
        updateValue("value", 0.0f);
        updateValue("status", 0);     // 0 = initializing
        updateValue("errorCount", 0);
        updateValue("uptime", 0);
        
        // Attempt to initialize sensor hardware
        bool success = initializeSensorHardware();
        
        // Update status based on initialization result
        updateValue("status", success ? 1 : -1); // 1 = OK, -1 = error
        
        return success;
    }
    
    bool update() override {
        // Update uptime
        int uptime = (millis() / 1000);
        updateValue("uptime", uptime);
        
        // Attempt to read sensor
        float value;
        bool success = readSensorWithTimeout(value);
        
        if (success) {
            // Successful reading
            updateValue("value", value);
            updateValue("status", 1); // 1 = OK
            _lastSuccessTime = millis();
        } else {
            // Reading failed
            _errorCount++;
            updateValue("errorCount", _errorCount);
            
            // Determine appropriate error code
            int statusCode;
            
            if (_errorCount > 10) {
                statusCode = -3; // Critical failure
            } else if (millis() - _lastSuccessTime > 10000) {
                statusCode = -2; // Timeout
            } else {
                statusCode = -1; // Temporary error
            }
            
            updateValue("status", statusCode);
        }
        
        return true; // Always return true to let system know we updated values
    }
    
private:
    bool initializeSensorHardware() {
        // Example hardware initialization
        // Return success/failure
        return true;
    }
    
    bool readSensorWithTimeout(float &value) {
        // Example: attempt reading with timeout
        unsigned long startTime = millis();
        
        // Try for up to 500ms
        while (millis() - startTime < 500) {
            if (sensorReady()) {
                value = readSensor();
                return true;
            }
            delay(10);
        }
        
        return false;
    }
    
    bool sensorReady() {
        // Example check if sensor is ready
        return true;
    }
    
    float readSensor() {
        // Example sensor reading
        return analogRead(_pin) * 0.01f;
    }
};
```

### Status Flags

Use status flags to track sensor state:

```cpp
// Status codes in your sensor
#define SENSOR_STATUS_INIT    0   // Initializing
#define SENSOR_STATUS_OK      1   // Normal operation
#define SENSOR_STATUS_WARN    2   // Warning
#define SENSOR_STATUS_ERROR  -1   // General error
#define SENSOR_STATUS_TIMEOUT -2  // Timeout error
#define SENSOR_STATUS_CRITICAL -3 // Critical error

// Usage in code
updateValue("status", SENSOR_STATUS_OK);

// Check status from outside
if (sensorModule.getRawValue("sensorName", "status") < 0) {
    Serial.println("Sensor error detected!");
}
```

### Debugging Techniques

Implement debug methods in your sensors:

```cpp
class DebuggableSensorV2 : public BaseSensV2 {
private:
    // ... normal sensor implementation ...
    
    // Debug buffer
    char _debugBuffer[64];
    bool _debugEnabled;
    Stream* _debugStream;
    
public:
    // ... normal sensor methods ...
    
    void enableDebug(Stream* stream) {
        _debugEnabled = true;
        _debugStream = stream;
    }
    
    void disableDebug() {
        _debugEnabled = false;
    }
    
    void setDebugLevel(int level) {
        // Set debug verbosity
    }
    
    void debugPrint(const char* message) {
        if (_debugEnabled && _debugStream) {
            _debugStream->print(message);
        }
    }
    
    bool update() override {
        // Get previous values for comparison
        float oldValue = getFloatValue("value");
        
        // Normal update code
        // ...
        
        // Debug output
        if (_debugEnabled && _debugStream) {
            float newValue = getFloatValue("value");
            snprintf(_debugBuffer, sizeof(_debugBuffer), 
                    "Sensor update: %0.2f -> %0.2f (delta: %0.2f)\n", 
                    oldValue, newValue, newValue - oldValue);
            _debugStream->print(_debugBuffer);
        }
        
        return true;
    }
};
```

### Serial Debug Output

Standard debug patterns:

1. **Prefixed messages**:
   ```cpp
   Serial.print("[MySensor] ");
   Serial.println("Initialized");
   ```

2. **Timed messages**:
   ```cpp
   Serial.print(millis());
   Serial.print(": ");
   Serial.println("Sensor value updated");
   ```

3. **Formatted values**:
   ```cpp
   Serial.print("Temperature: ");
   Serial.print(temperature, 1);
   Serial.println(" °C");
   ```

4. **Error indicators**:
   ```cpp
   Serial.print("ERROR ");
   Serial.print(errorCode);
   Serial.print(": ");
   Serial.println(getErrorMessage(errorCode));
   ```

5. **State transitions**:
   ```cpp
   Serial.print("Sensor state: ");
   Serial.print(oldState);
   Serial.print(" -> ");
   Serial.println(newState);
   ```

### Common Issues and Solutions

1. **Readings fluctuate wildly**
    - Solution: Implement filtering (moving average, median filter)
    - Example:
      ```cpp
      // Simple moving average
      value = (value * 0.9) + (newReading * 0.1);
      ```

2. **Failed initialization**
    - Solution: Add retry mechanism
    - Example:
      ```cpp
      bool init() override {
          for (int attempt = 1; attempt <= 3; attempt++) {
              if (initHardware()) {
                  return true;
              }
              delay(100);
          }
          return false;
      }
      ```

3. **Communication errors**
    - Solution: Add timeouts and error checking
    - Example:
      ```cpp
      bool readFromI2C() {
          unsigned long startTime = millis();
          while (millis() - startTime < 100) { // 100ms timeout
              if (Wire.requestFrom(_address, 1)) {
                  return true;
              }
              delay(10);
          }
          return false;
      }
      ```

4. **Calibration not applied**
    - Solution: Verify calibration values are loaded correctly
    - Example:
      ```cpp
      void checkCalibration() {
          if (!_calibrationLoaded) {
              Serial.println("Warning: No calibration data loaded");
          }
      }
      ```

5. **Memory corruption**
    - Solution: Check allocations and avoid buffer overflows
    - Example:
      ```cpp
      void safeStringCopy(char* dest, const char* src, size_t maxLen) {
          strncpy(dest, src, maxLen - 1);
          dest[maxLen - 1] = '\0';
      }
      ```

## 12. Power Management

### Low-Power Operation

Design sensors for low power operation:

```cpp
class LowPowerSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    uint8_t _powerPin;
    bool _powerSaveMode;
    unsigned long _powerOnTime;
    unsigned long _warmupTime;
    
public:
    LowPowerSensorV2(uint8_t sensorPin, uint8_t powerPin) 
        : _pin(sensorPin),
          _powerPin(powerPin),
          _powerSaveMode(true),
          _powerOnTime(0),
          _warmupTime(100) {  // 100ms warmup time
        
        addValueInfo("value", "Sensor Value", "", 2, TYPE_FLOAT);
        addValueInfo("powerState", "Power State", "", 0, TYPE_INT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        pinMode(_powerPin, OUTPUT);
        
        // Start with sensor powered off
        digitalWrite(_powerPin, LOW);
        
        updateValue("value", 0.0f);
        updateValue("powerState", 0);
        
        return true;
    }
    
    bool update() override {
        if (_powerSaveMode) {
            // Power saving mode - power up sensor only during reading
            
            // Power on sensor
            digitalWrite(_powerPin, HIGH);
            updateValue("powerState", 1);
            
            // Record power on time
            _powerOnTime = millis();
            
            // Wait for warmup
            delay(_warmupTime);
            
            // Read sensor
            float value = analogRead(_pin) * (5.0 / 1023.0);
            updateValue("value", value);
            
            // Power down sensor
            digitalWrite(_powerPin, LOW);
            updateValue("powerState", 0);
        } else {
            // Normal mode - sensor always on
            float value = analogRead(_pin) * (5.0 / 1023.0);
            updateValue("value", value);
        }
        
        return true;
    }
    
    void setPowerSaveMode(bool enabled) {
        _powerSaveMode = enabled;
        
        // Update power state immediately
        if (_powerSaveMode) {
            digitalWrite(_powerPin, LOW);
            updateValue("powerState", 0);
        } else {
            digitalWrite(_powerPin, HIGH);
            updateValue("powerState", 1);
        }
    }
    
    void setWarmupTime(unsigned long timeMs) {
        _warmupTime = timeMs;
    }
};
```

### Sleep Modes

Integrate with device sleep modes:

```cpp
class SleepAwareSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    bool _sleepPrepared;
    
public:
    SleepAwareSensorV2(uint8_t pin)
        : _pin(pin), _sleepPrepared(false) {
        
        addValueInfo("value", "Sensor Value", "", 2, TYPE_FLOAT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        updateValue("value", 0.0f);
        return true;
    }
    
    bool update() override {
        float value = analogRead(_pin) * (5.0 / 1023.0);
        updateValue("value", value);
        return true;
    }
    
    // Called before system sleep
    void prepareForSleep() {
        // Perform any actions needed before sleep
        // e.g., save state, put sensor in low power mode
        
        _sleepPrepared = true;
    }
    
    // Called after system wakes
    void resumeFromSleep() {
        if (_sleepPrepared) {
            // Restore sensor to normal operation
            // e.g., reinitialize, restore state
            
            _sleepPrepared = false;
        }
    }
};
```

### Power-Aware Sampling

Adjust sampling based on power state:

```cpp
class PowerAwareSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    bool _batteryPowered;
    unsigned long _normalInterval;
    unsigned long _batterySavingInterval;
    float _batteryVoltageThreshold;
    unsigned long _lastSampleTime;
    
public:
    PowerAwareSensorV2(uint8_t pin)
        : _pin(pin),
          _batteryPowered(false),
          _normalInterval(1000),     // 1 second when on power
          _batterySavingInterval(10000), // 10 seconds on battery
          _batteryVoltageThreshold(3.3),
          _lastSampleTime(0) {
        
        addValueInfo("value", "Sensor Value", "", 2, TYPE_FLOAT);
        addValueInfo("batteryVoltage", "Battery Voltage", "V", 2, TYPE_FLOAT);
        addValueInfo("sampleInterval", "Sample Interval", "ms", 0, TYPE_INT);
    }
    
    bool init() override {
        pinMode(_pin, INPUT);
        checkPowerSource();
        updateValue("value", 0.0f);
        return true;
    }
    
    bool update() override {
        unsigned long now = millis();
        unsigned long interval = getCurrentInterval();
        
        if (now - _lastSampleTime >= interval) {
            // Time to take a sample
            
            // Check power source periodically
            checkPowerSource();
            
            // Read sensor
            float value = analogRead(_pin) * (5.0 / 1023.0);
            updateValue("value", value);
            
            _lastSampleTime = now;
            return true;
        }
        
        return false;
    }
    
    void setBatteryPowered(bool batteryMode) {
        _batteryPowered = batteryMode;
        updateValue("sampleInterval", getCurrentInterval());
    }
    
    void setSamplingIntervals(unsigned long normal, unsigned long battery) {
        _normalInterval = normal;
        _batterySavingInterval = battery;
        updateValue("sampleInterval", getCurrentInterval());
    }
    
private:
    unsigned long getCurrentInterval() {
        return _batteryPowered ? _batterySavingInterval : _normalInterval;
    }
    
    void checkPowerSource() {
        // Example: read battery voltage from analog pin
        float batteryVoltage = (analogRead(A0) / 1023.0) * 5.0;
        updateValue("batteryVoltage", batteryVoltage);
        
        // Detect if running on battery
        bool onBattery = (batteryVoltage < _batteryVoltageThreshold);
        
        if (onBattery != _batteryPowered) {
            // Power source changed
            _batteryPowered = onBattery;
            updateValue("sampleInterval", getCurrentInterval());
        }
    }
};
```

## 13. Creating a Distributable Library

### Library Structure

Standard Arduino library structure:

```
YourSensorLibrary/
├── examples/
│   ├── BasicExample/
│   │   └── BasicExample.ino
│   └── AdvancedExample/
│       └── AdvancedExample.ino
├── src/
│   ├── YourSensorV2.h
│   └── YourSensorV2.cpp
├── keywords.txt
├── library.properties
└── README.md
```

### Documentation

Essential documentation files:

**README.md:**

```markdown
# YourSensorLibrary

A library for interfacing with YourSensor using the SensorModuleV2 framework.

## Features

- Feature 1
- Feature 2

## Installation

...

## Usage

```arduino
#include "YourSensorV2.h"

YourSensorV2 sensor(A0);
// ...
```

**library.properties:**

```
name=YourSensorLibrary
version=1.0.0
author=Your Name <your.email@example.com>
maintainer=Your Name <your.email@example.com>
sentence=Library for YourSensor using SensorModuleV2 framework.
paragraph=Provides a comprehensive interface for YourSensor with calibration support.
category=Sensors
url=https://github.com/yourusername/YourSensorLibrary
architectures=*
depends=SensorModuleV2
```

**keywords.txt:**

```
YourSensorV2	KEYWORD1
init	KEYWORD2
update	KEYWORD2
setParameter	KEYWORD2
```

### Examples

Include basic and advanced examples:

**BasicExample.ino:**

```cpp
/*
 * Basic example for YourSensorLibrary
 */
#include "SensorModuleV2.h"
#include "YourSensorV2.h"

YourSensorV2 sensor(A0);
SensorModuleV2 sensorModule;

void setup() {
  Serial.begin(9600);
  
  sensorModule.addSensor("mySensor", &sensor);
  sensorModule.init();
}

void loop() {
  sensorModule.update();
  
  // Read and display sensor value
  float value = sensor.getFloatValue("value");
  
  Serial.print("Sensor value: ");
  Serial.println(value);
  
  delay(1000);
}
```

**AdvancedExample.ino:**

```cpp
/*
 * Advanced example for YourSensorLibrary with calibration
 */
#include "SensorModuleV2.h"
#include "SensorCalibrationModuleV2.h"
#include "YourSensorV2.h"

YourSensorV2 sensor(A0);
SensorCalibrationModuleV2 sensorModule;

void setup() {
  Serial.begin(9600);
  
  // Configure sensor
  sensor.setParameter(42);
  
  // Add sensor to module
  sensorModule.addSensor("mySensor", &sensor);
  
  // Initialize
  sensorModule.init();
  
  // Auto-discover values for calibration
  sensorModule.discoverSensorValues();
  
  // Optional: Load calibration from EEPROM
  sensorModule.loadAllCalibrations();
  
  // Start calibration mode
  Serial.println("Enter 'c' to start calibration mode");
}

void loop() {
  sensorModule.update();
  
  // Check for calibration command
  if (Serial.available() && Serial.read() == 'c') {
    sensorModule.startCalibrationMode(&Serial);
  }
  
  // Read and display values
  if (!sensorModule.isInCalibrationMode()) {
    float rawValue = sensorModule.getRawValue("mySensor", "value");
    float calibratedValue = sensorModule.getCalibratedValue("mySensor", "value");
    
    Serial.print("Raw: ");
    Serial.print(rawValue);
    Serial.print(" | Calibrated: ");
    Serial.println(calibratedValue);
  }
  
  delay(1000);
}
```

### Versioning

Follow semantic versioning for your library:

- **MAJOR** version for incompatible API changes
- **MINOR** version for added functionality in a backwards compatible manner
- **PATCH** version for backwards compatible bug fixes

Update version number in:

- library.properties
- README.md
- Any version constants in the code

### GitHub Repository Setup

Set up your repository with:

1. **Clear README**: With installation and usage instructions
2. **License**: Typically MIT or Apache 2.0 for open source libraries
3. **Issues Template**: To help users report problems effectively
4. **.gitignore**: For Arduino and your development environment
5. **GitHub Actions**: For automated testing

### Arduino Library Manager Compatibility

To make your library compatible with Arduino Library Manager:

1. Use the standard folder structure described above
2. Create proper library.properties file
3. Tag releases with version numbers (v1.0.0)
4. Submit to Arduino Library Manager

## 14. Complete Implementation Examples

### Temperature/Humidity Sensor

DHT22 Sensor Implementation:

```cpp
// DHT22SensorV2.h
#ifndef DHT22_SENSOR_V2_H
#define DHT22_SENSOR_V2_H

#include "Arduino.h"
#include "SensorModuleV2.h"
#include "DHT.h"

class DHT22SensorV2 : public BaseSensV2 {
private:
    DHT _dht;
    uint8_t _pin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _enableHeatIndex;
    
public:
    DHT22SensorV2(uint8_t pin, bool enableHeatIndex = false);
    virtual ~DHT22SensorV2();
    
    bool init() override;
    bool update() override;
    
    void setUpdateInterval(uint32_t interval);
    void enableHeatIndex(bool enable);
};

#endif

// DHT22SensorV2.cpp
#include "DHT22SensorV2.h"

DHT22SensorV2::DHT22SensorV2(uint8_t pin, bool enableHeatIndex)
    : _dht(pin, DHT22),
      _pin(pin),
      _updateTimer(0),
      _updateInterval(2000),
      _enableHeatIndex(enableHeatIndex) {
    
    // Register standard values
    addValueInfo("temperature", "Temperature", "°C", 1, TYPE_FLOAT);
    addValueInfo("humidity", "Humidity", "%", 0, TYPE_FLOAT);
    
    // Register heat index if enabled
    if (_enableHeatIndex) {
        addValueInfo("heatIndex", "Heat Index", "°C", 1, TYPE_FLOAT);
    }
    
    // Status and error tracking
    addValueInfo("status", "Status", "", 0, TYPE_INT);
}

DHT22SensorV2::~DHT22SensorV2() {
    // No manual cleanup needed
}

bool DHT22SensorV2::init() {
    _dht.begin();
    
    // Initialize values
    updateValue("temperature", 0.0f);
    updateValue("humidity", 0.0f);
    if (_enableHeatIndex) {
        updateValue("heatIndex", 0.0f);
    }
    updateValue("status", 0); // 0 = initializing
    
    return true;
}

bool DHT22SensorV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        // Read humidity
        float h = _dht.readHumidity();
        // Read temperature as Celsius
        float t = _dht.readTemperature();
        
        // Check if any reads failed
        if (isnan(h) || isnan(t)) {
            // Failed reading
            updateValue("status", -1); // -1 = read error
        } else {
            // Successful reading
            updateValue("temperature", t);
            updateValue("humidity", h);
            updateValue("status", 1); // 1 = OK
            
            // Calculate heat index if enabled
            if (_enableHeatIndex) {
                float hi = _dht.computeHeatIndex(t, h, false);
                updateValue("heatIndex", hi);
            }
        }
        
        _updateTimer = millis();
        return true;
    }
    return false;
}

void DHT22SensorV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void DHT22SensorV2::enableHeatIndex(bool enable) {
    if (enable && !_enableHeatIndex) {
        // Add heat index value if it wasn't enabled before
        addValueInfo("heatIndex", "Heat Index", "°C", 1, TYPE_FLOAT);
        updateValue("heatIndex", 0.0f);
    }
    _enableHeatIndex = enable;
}
```

### Soil Moisture Sensor

```cpp
// SoilMoistureSensorV2.h
#ifndef SOIL_MOISTURE_SENSOR_V2_H
#define SOIL_MOISTURE_SENSOR_V2_H

#include "Arduino.h"
#include "SensorModuleV2.h"

class SoilMoistureSensorV2 : public BaseSensV2 {
private:
    uint8_t _sensorPin;
    uint8_t _powerPin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    
    int _dryValue;
    int _wetValue;
    bool _usePowerManagement;
    
public:
    SoilMoistureSensorV2(uint8_t sensorPin, uint8_t powerPin = 0xFF);
    virtual ~SoilMoistureSensorV2();
    
    bool init() override;
    bool update() override;
    
    void setUpdateInterval(uint32_t interval);
    void setCalibrationValues(int dryValue, int wetValue);
    void enablePowerManagement(bool enable);
};

#endif

// SoilMoistureSensorV2.cpp
#include "SoilMoistureSensorV2.h"

SoilMoistureSensorV2::SoilMoistureSensorV2(uint8_t sensorPin, uint8_t powerPin)
    : _sensorPin(sensorPin),
      _powerPin(powerPin),
      _updateTimer(0),
      _updateInterval(5000),
      _dryValue(900),
      _wetValue(300),
      _usePowerManagement(powerPin != 0xFF) {
    
    addValueInfo("raw", "Raw Value", "", 0, TYPE_INT);
    addValueInfo("percent", "Moisture", "%", 0, TYPE_INT);
    addValueInfo("level", "Moisture Level", "", 0, TYPE_INT); // 0=dry, 1=medium, 2=wet
}

SoilMoistureSensorV2::~SoilMoistureSensorV2() {
    // No manual cleanup needed
}

bool SoilMoistureSensorV2::init() {
    pinMode(_sensorPin, INPUT);
    
    if (_usePowerManagement) {
        pinMode(_powerPin, OUTPUT);
        digitalWrite(_powerPin, LOW); // Start with power off
    }
    
    updateValue("raw", 0);
    updateValue("percent", 0);
    updateValue("level", 0);
    
    return true;
}

bool SoilMoistureSensorV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        int rawValue;
        
        if (_usePowerManagement) {
            // Power on sensor
            digitalWrite(_powerPin, HIGH);
            delay(100); // Wait for stabilization
            
            // Read value
            rawValue = analogRead(_sensorPin);
            
            // Power off sensor
            digitalWrite(_powerPin, LOW);
        } else {
            // Read value (power always on)
            rawValue = analogRead(_sensorPin);
        }
        
        // Calculate moisture percentage
        int range = _dryValue - _wetValue;
        int adjustedValue = constrain(rawValue, _wetValue, _dryValue);
        int moisturePercent = map(adjustedValue, _dryValue, _wetValue, 0, 100);
        
        // Determine moisture level
        int moistureLevel;
        if (moisturePercent < 30) {
            moistureLevel = 0; // Dry
        } else if (moisturePercent < 70) {
            moistureLevel = 1; // Medium
        } else {
            moistureLevel = 2; // Wet
        }
        
        // Update values
        updateValue("raw", rawValue);
        updateValue("percent", moisturePercent);
        updateValue("level", moistureLevel);
        
        _updateTimer = millis();
        return true;
    }
    return false;
}

void SoilMoistureSensorV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void SoilMoistureSensorV2::setCalibrationValues(int dryValue, int wetValue) {
    _dryValue = dryValue;
    _wetValue = wetValue;
}

void SoilMoistureSensorV2::enablePowerManagement(bool enable) {
    _usePowerManagement = enable && (_powerPin != 0xFF);
}
```

### Accelerometer

```cpp
// ADXL345AccelerometerV2.h
#ifndef ADXL345_ACCELEROMETER_V2_H
#define ADXL345_ACCELEROMETER_V2_H

#include "Arduino.h"
#include "SensorModuleV2.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

class ADXL345AccelerometerV2 : public BaseSensV2 {
private:
    Adafruit_ADXL345_Unified _accel;
    uint8_t _address;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    bool _detectTaps;
    bool _detectOrientation;
    
public:
    ADXL345AccelerometerV2(uint8_t address = 0x53, bool detectTaps = true, bool detectOrientation = true);
    virtual ~ADXL345AccelerometerV2();
    
    bool init() override;
    bool update() override;
    
    void setUpdateInterval(uint32_t interval);
    void configureAccelerometer(uint8_t range);
    void enableTapDetection(bool enable);
    void enableOrientationDetection(bool enable);
};

#endif

// ADXL345AccelerometerV2.cpp
#include "ADXL345AccelerometerV2.h"

ADXL345AccelerometerV2::ADXL345AccelerometerV2(uint8_t address, bool detectTaps, bool detectOrientation)
    : _accel(12345), // ID is arbitrary
      _address(address),
      _updateTimer(0),
      _updateInterval(100),
      _detectTaps(detectTaps),
      _detectOrientation(detectOrientation) {
    
    // Basic acceleration values
    addValueInfo("accelX", "X Acceleration", "g", 2, TYPE_FLOAT);
    addValueInfo("accelY", "Y Acceleration", "g", 2, TYPE_FLOAT);
    addValueInfo("accelZ", "Z Acceleration", "g", 2, TYPE_FLOAT);
    
    // Calculated magnitude
    addValueInfo("magnitude", "Magnitude", "g", 2, TYPE_FLOAT);
    
    // Optional tap detection
    if (_detectTaps) {
        addValueInfo("tapDetected", "Tap Detected", "", 0, TYPE_INT);
        addValueInfo("doubleTap", "Double Tap", "", 0, TYPE_INT);
    }
    
    // Optional orientation detection
    if (_detectOrientation) {
        addValueInfo("orientation", "Orientation", "", 0, TYPE_INT);
    }
}

ADXL345AccelerometerV2::~ADXL345AccelerometerV2() {
    // No manual cleanup needed
}

bool ADXL345AccelerometerV2::init() {
    // Initialize I2C and accelerometer
    Wire.begin();
    if (!_accel.begin(_address)) {
        return false;
    }
    
    // Set range (2, 4, 8, 16g)
    _accel.setRange(ADXL345_RANGE_2_G);
    
    // Configure tap detection if enabled
    if (_detectTaps) {
        _accel.setTapDetectionThreshold(10); // 62.5mg per increment
        _accel.setTapDuration(15);          // 625us per increment
        _accel.setDoubleTapLatency(80);     // 1.25ms per increment
        _accel.setDoubleTapWindow(200);     // 1.25ms per increment
        
        // Enable single/double tap detection on all axes
        _accel.enableTapDetection(true, true, true);
    }
    
    // Initialize values
    updateValue("accelX", 0.0f);
    updateValue("accelY", 0.0f);
    updateValue("accelZ", 0.0f);
    updateValue("magnitude", 0.0f);
    
    if (_detectTaps) {
        updateValue("tapDetected", 0);
        updateValue("doubleTap", 0);
    }
    
    if (_detectOrientation) {
        updateValue("orientation", 0);
    }
    
    return true;
}

bool ADXL345AccelerometerV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        // Read acceleration
        sensors_event_t event;
        _accel.getEvent(&event);
        
        // Update basic values
        updateValue("accelX", event.acceleration.x);
        updateValue("accelY", event.acceleration.y);
        updateValue("accelZ", event.acceleration.z);
        
        // Calculate magnitude
        float mag = sqrt(
            event.acceleration.x * event.acceleration.x +
            event.acceleration.y * event.acceleration.y +
            event.acceleration.z * event.acceleration.z
        );
        updateValue("magnitude", mag);
        
        // Handle tap detection
        if (_detectTaps) {
            uint8_t tapSource = 0;
            bool tapDetected = _accel.getTapSourceRegister(&tapSource);
            
            if (tapDetected) {
                // Determine if it's a single or double tap
                bool isDoubleTap = tapSource & 0x20;
                
                updateValue("tapDetected", 1);
                updateValue("doubleTap", isDoubleTap ? 1 : 0);
            } else {
                updateValue("tapDetected", 0);
                updateValue("doubleTap", 0);
            }
        }
        
        // Handle orientation detection
        if (_detectOrientation) {
            // Determine orientation based on axis with largest absolute value
            float absX = abs(event.acceleration.x);
            float absY = abs(event.acceleration.y);
            float absZ = abs(event.acceleration.z);
            
            int orientation;
            
            if (absZ > absX && absZ > absY) {
                // Z axis dominates
                orientation = (event.acceleration.z > 0) ? 0 : 1; // 0=face up, 1=face down
            } else if (absX > absY) {
                // X axis dominates
                orientation = (event.acceleration.x > 0) ? 2 : 3; // 2=right side, 3=left side
            } else {
                // Y axis dominates
                orientation = (event.acceleration.y > 0) ? 4 : 5; // 4=top side, 5=bottom side
            }
            
            updateValue("orientation", orientation);
        }
        
        _updateTimer = millis();
        return true;
    }
    return false;
}

void ADXL345AccelerometerV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void ADXL345AccelerometerV2::configureAccelerometer(uint8_t range) {
    switch (range) {
        case 2:
            _accel.setRange(ADXL345_RANGE_2_G);
            break;
        case 4:
            _accel.setRange(ADXL345_RANGE_4_G);
            break;
        case 8:
            _accel.setRange(ADXL345_RANGE_8_G);
            break;
        case 16:
            _accel.setRange(ADXL345_RANGE_16_G);
            break;
        default:
            _accel.setRange(ADXL345_RANGE_2_G);
            break;
    }
}

void ADXL345AccelerometerV2::enableTapDetection(bool enable) {
    _detectTaps = enable;
    
    if (enable && !getValueInfoByKey("tapDetected")) {
        // Add tap detection values if not already present
        addValueInfo("tapDetected", "Tap Detected", "", 0, TYPE_INT);
        addValueInfo("doubleTap", "Double Tap", "", 0, TYPE_INT);
        
        updateValue("tapDetected", 0);
        updateValue("doubleTap", 0);
        
        // Configure tap detection
        _accel.setTapDetectionThreshold(10);
        _accel.setTapDuration(15);
        _accel.setDoubleTapLatency(80);
        _accel.setDoubleTapWindow(200);
        
        // Enable single/double tap detection on all axes
        _accel.enableTapDetection(true, true, true);
    }
}

void ADXL345AccelerometerV2::enableOrientationDetection(bool enable) {
    _detectOrientation = enable;
    
    if (enable && !getValueInfoByKey("orientation")) {
        // Add orientation value if not already present
        addValueInfo("orientation", "Orientation", "", 0, TYPE_INT);
        updateValue("orientation", 0);
    }
}
```

### Distance Sensor

```cpp
// UltrasonicSensorV2.h
#ifndef ULTRASONIC_SENSOR_V2_H
#define ULTRASONIC_SENSOR_V2_H

#include "Arduino.h"
#include "SensorModuleV2.h"

class UltrasonicSensorV2 : public BaseSensV2 {
private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    uint8_t _numSamples;
    float _speedOfSound;
    uint16_t _maxDistance;
    float _minValidDistance;
    
    // Filtering
    bool _useMedianFilter;
    static const uint8_t MAX_SAMPLES = 5;
    float _samples[MAX_SAMPLES];
    
public:
    UltrasonicSensorV2(uint8_t trigPin, uint8_t echoPin);
    virtual ~UltrasonicSensorV2();
    
    bool init() override;
    bool update() override;
    
    void setUpdateInterval(uint32_t interval);
    void setSpeedOfSound(float speedMps);
    void setMaxDistance(uint16_t maxCm);
    void setMinValidDistance(float minCm);
    void setNumberOfSamples(uint8_t samples);
    void enableMedianFilter(bool enable);
    
private:
    float measureDistance();
    float medianFilter(float newReading);
};

#endif

// UltrasonicSensorV2.cpp
#include "UltrasonicSensorV2.h"

UltrasonicSensorV2::UltrasonicSensorV2(uint8_t trigPin, uint8_t echoPin)
    : _trigPin(trigPin),
      _echoPin(echoPin),
      _updateTimer(0),
      _updateInterval(100),
      _numSamples(3),
      _speedOfSound(343.0),      // m/s at 20°C
      _maxDistance(400),         // 4m max
      _minValidDistance(2.0),    // 2cm min
      _useMedianFilter(true) {
    
    // Initialize sample array
    for (uint8_t i = 0; i < MAX_SAMPLES; i++) {
        _samples[i] = 0;
    }
    
    // Register standard values
    addValueInfo("distance", "Distance", "cm", 1, TYPE_FLOAT);
    addValueInfo("raw", "Raw Distance", "cm", 1, TYPE_FLOAT);
    addValueInfo("status", "Status", "", 0, TYPE_INT);
}

UltrasonicSensorV2::~UltrasonicSensorV2() {
    // No manual cleanup needed
}

bool UltrasonicSensorV2::init() {
    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);
    
    // Initialize pins to known state
    digitalWrite(_trigPin, LOW);
    
    // Initialize values
    updateValue("distance", 0.0f);
    updateValue("raw", 0.0f);
    updateValue("status", 0);
    
    return true;
}

bool UltrasonicSensorV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        float totalDistance = 0;
        int validReadings = 0;
        
        // Take multiple samples
        for (uint8_t i = 0; i < _numSamples; i++) {
            float rawDistance = measureDistance();
            
            // Check if reading is valid
            if (rawDistance > _minValidDistance && rawDistance < _maxDistance) {
                if (_useMedianFilter) {
                    // Apply median filter
                    float filteredDistance = medianFilter(rawDistance);
                    totalDistance += filteredDistance;
                } else {
                    totalDistance += rawDistance;
                }
                validReadings++;
            }
            
            // Small delay between readings
            delay(10);
        }
        
        if (validReadings > 0) {
            // Calculate average of valid readings
            float avgDistance = totalDistance / validReadings;
            
            // Update values
            updateValue("distance", avgDistance);
            updateValue("raw", measureDistance());
            updateValue("status", 1); // 1 = valid reading
        } else {
            // No valid readings
            updateValue("status", -1); // -1 = no valid readings
        }
        
        _updateTimer = millis();
        return true;
    }
    return false;
}

float UltrasonicSensorV2::measureDistance() {
    // Clear the trigger pin
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    
    // Set the trigger pin high for 10 microseconds
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);
    
    // Read the echo pin - pulse length determines distance
    unsigned long duration = pulseIn(_echoPin, HIGH, 30000); // 30ms timeout
    
    // Calculate distance: time * speed of sound / 2 (round trip)
    // Convert to cm: * 100
    float distance = 0;
    
    if (duration > 0) {
        distance = (duration * 0.000001 * _speedOfSound * 100) / 2.0;
    }
    
    return distance;
}

float UltrasonicSensorV2::medianFilter(float newReading) {
    // Shift the array
    for (uint8_t i = MAX_SAMPLES - 1; i > 0; i--) {
        _samples[i] = _samples[i - 1];
    }
    _samples[0] = newReading;
    
    // Create a copy for sorting
    float sortedValues[MAX_SAMPLES];
    for (uint8_t i = 0; i < MAX_SAMPLES; i++) {
        sortedValues[i] = _samples[i];
    }
    
    // Simple bubble sort
    for (uint8_t i = 0; i < MAX_SAMPLES - 1; i++) {
        for (uint8_t j = 0; j < MAX_SAMPLES - i - 1; j++) {
            if (sortedValues[j] > sortedValues[j + 1]) {
                float temp = sortedValues[j];
                sortedValues[j] = sortedValues[j + 1];
                sortedValues[j + 1] = temp;
            }
        }
    }
    
    // Return the middle value
    return sortedValues[MAX_SAMPLES / 2];
}

void UltrasonicSensorV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void UltrasonicSensorV2::setSpeedOfSound(float speedMps) {
    _speedOfSound = speedMps;
}

void UltrasonicSensorV2::setMaxDistance(uint16_t maxCm) {
    _maxDistance = maxCm;
}

void UltrasonicSensorV2::setMinValidDistance(float minCm) {
    _minValidDistance = minCm;
}

void UltrasonicSensorV2::setNumberOfSamples(uint8_t samples) {
    _numSamples = constrain(samples, 1, 10);
}

void UltrasonicSensorV2::enableMedianFilter(bool enable) {
    _useMedianFilter = enable;
}
```

### Light Sensor

```cpp
// LightSensorV2.h
#ifndef LIGHT_SENSOR_V2_H
#define LIGHT_SENSOR_V2_H

#include "Arduino.h"
#include "SensorModuleV2.h"

class LightSensorV2 : public BaseSensV2 {
private:
    uint8_t _pin;
    uint32_t _updateTimer;
    uint32_t _updateInterval;
    float _referenceVoltage;
    int _adcResolution;
    
    // Parameters for light level calculation
    float _darkThreshold;
    float _dimThreshold;
    float _normalThreshold;
    float _brightThreshold;
    
    // Low-pass filter
    bool _useFilter;
    float _filterAlpha;
    float _filteredValue;
    
public:
    LightSensorV2(uint8_t pin, float refVoltage = 5.0, int adcRes = 1023);
    virtual ~LightSensorV2();
    
    bool init() override;
    bool update() override;
    
    void setUpdateInterval(uint32_t interval);
    void setThresholds(float dark, float dim, float normal, float bright);
    void setLowPassFilter(bool enable, float alpha = 0.1);
};

#endif

// LightSensorV2.cpp
#include "LightSensorV2.h"

LightSensorV2::LightSensorV2(uint8_t pin, float refVoltage, int adcRes)
    : _pin(pin),
      _updateTimer(0),
      _updateInterval(500),
      _referenceVoltage(refVoltage),
      _adcResolution(adcRes),
      _darkThreshold(0.1),
      _dimThreshold(0.3),
      _normalThreshold(0.6),
      _brightThreshold(0.8),
      _useFilter(true),
      _filterAlpha(0.1),
      _filteredValue(0) {
    
    // Register values
    addValueInfo("raw", "Raw Value", "", 0, TYPE_INT);
    addValueInfo("voltage", "Voltage", "V", 2, TYPE_FLOAT);
    addValueInfo("level", "Light Level", "", 0, TYPE_INT);  // 0=dark, 1=dim, 2=normal, 3=bright, 4=very bright
    addValueInfo("percent", "Light Percent", "%", 0, TYPE_INT);
}

LightSensorV2::~LightSensorV2() {
    // No manual cleanup needed
}

bool LightSensorV2::init() {
    pinMode(_pin, INPUT);
    
    // Initialize values
    updateValue("raw", 0);
    updateValue("voltage", 0.0f);
    updateValue("level", 0);
    updateValue("percent", 0);
    
    return true;
}

bool LightSensorV2::update() {
    if (millis() - _updateTimer >= _updateInterval) {
        // Read raw analog value
        int rawValue = analogRead(_pin);
        
        // Convert to voltage
        float voltage = (float)rawValue * _referenceVoltage / _adcResolution;
        
        // Apply low-pass filter if enabled
        if (_useFilter) {
            if (_filteredValue == 0) {
                // First reading
                _filteredValue = voltage;
            } else {
                // Apply filter
                _filteredValue = _filteredValue * (1 - _filterAlpha) + voltage * _filterAlpha;
            }
            
            voltage = _filteredValue;
        }
        
        // Calculate light percentage (invert since most light sensors decrease resistance with light)
        float normalizedVoltage = voltage / _referenceVoltage;
        int lightPercent = (int)(normalizedVoltage * 100);
        
        // Determine light level based on thresholds
        int lightLevel;
        if (normalizedVoltage <= _darkThreshold) {
            lightLevel = 0; // Dark
        } else if (normalizedVoltage <= _dimThreshold) {
            lightLevel = 1; // Dim
        } else if (normalizedVoltage <= _normalThreshold) {
            lightLevel = 2; // Normal
        } else if (normalizedVoltage <= _brightThreshold) {
            lightLevel = 3; // Bright
        } else {
            lightLevel = 4; // Very bright
        }
        
        // Update values
        updateValue("raw", rawValue);
        updateValue("voltage", voltage);
        updateValue("level", lightLevel);
        updateValue("percent", lightPercent);
        
        _updateTimer = millis();
        return true;
    }
    return false;
}

void LightSensorV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void LightSensorV2::setThresholds(float dark, float dim, float normal, float bright) {
    _darkThreshold = dark;
    _dimThreshold = dim;
    _normalThreshold = normal;
    _brightThreshold = bright;
}

void LightSensorV2::setLowPassFilter(bool enable, float alpha) {
    _useFilter = enable;
    _filterAlpha = constrain(alpha, 0.01, 1.0);
}
```

## 15. Testing and Validation

### Unit Testing

For Arduino platforms, basic unit testing:

```cpp
void testSensor() {
    Serial.println("Starting sensor tests...");
    
    YourSensorV2 sensor(A0);
    
    // Test initialization
    if (sensor.init()) {
        Serial.println("PASS: Sensor initialized");
    } else {
        Serial.println("FAIL: Sensor initialization failed");
    }
    
    // Test reading
    sensor.update();
    float value = sensor.getFloatValue("value");
    Serial.print("Value after update: ");
    Serial.println(value);
    
    // Test value registration
    const SensorValueInfo* info = sensor.getValueInfoByKey("value");
    if (info != nullptr) {
        Serial.println("PASS: Value info retrieved");
    } else {
        Serial.println("FAIL: Could not retrieve value info");
    }
    
    // Add more tests as needed
    
    Serial.println("Testing complete");
}
```

### Integration Testing

Test sensors with the calibration framework:

```cpp
void testCalibrationSystem() {
    Serial.println("Starting calibration system test...");
    
    YourSensorV2 sensor(A0);
    SensorCalibrationModuleV2 module;
    
    // Add sensor to module
    module.addSensor("testSensor", &sensor);
    
    // Initialize
    if (module.init()) {
        Serial.println("PASS: Module initialized");
    } else {
        Serial.println("FAIL: Module initialization failed");
    }
    
    // Discover values
    module.discoverSensorValues();
    
    // Test manual calibration
    bool calibSuccess = module.calibrateOnePoint("testSensor", "value", 50.0);
    
    if (calibSuccess) {
        Serial.println("PASS: Calibration successful");
    } else {
        Serial.println("FAIL: Calibration failed");
    }
    
    // Test reading calibrated values
    module.update();
    float raw = module.getRawValue("testSensor", "value");
    float calibrated = module.getCalibratedValue("testSensor", "value");
    
    Serial.print("Raw: ");
    Serial.print(raw);
    Serial.print(" | Calibrated: ");
    Serial.println(calibrated);
    
    Serial.println("Testing complete");
}
```

### Validation Strategies

1. **Known Reference Testing**: Test with known reference values
   ```cpp
   void validateAgainstReference() {
       // Test with known reference signals/values
       // E.g. fixed voltage for analog sensors
       // E.g. known distance for ultrasonic sensors
   }
   ```

2. **Cross-sensor Validation**: Compare multiple sensors
   ```cpp
   void validateWithMultipleSensors() {
       // Compare readings from multiple sensors measuring same value
       // E.g. two temperature sensors at same location
   }
   ```

3. **Range Testing**: Test across full range
   ```cpp
   void validateAcrossRange() {
       // Test with values across the entire expected range
       // Document accuracy at different points in the range
   }
   ```

4. **Stability Testing**: Test reading stability
   ```cpp
   void validateStability() {
       // Take multiple readings under constant conditions
       // Calculate standard deviation
       // Ensure acceptable stability
   }
   ```

### Performance Testing

1. **Update Timing Test**:
   ```cpp
   void measureUpdateTime() {
       YourSensorV2 sensor(A0);
       sensor.init();
       
       unsigned long start = micros();
       sensor.update();
       unsigned long elapsed = micros() - start;
       
       Serial.print("Update time: ");
       Serial.print(elapsed);
       Serial.println(" microseconds");
   }
   ```

2. **Memory Usage Test**:
   ```cpp
   void checkMemoryUsage() {
       Serial.print("Free memory before sensor: ");
       Serial.println(freeMemory());
       
       YourSensorV2* sensor = new YourSensorV2(A0);
       sensor->init();
       
       Serial.print("Free memory with sensor: ");
       Serial.println(freeMemory());
       
       delete sensor;
       
       Serial.print("Free memory after cleanup: ");
       Serial.println(freeMemory());
   }
   ```

3. **Power Consumption Test**:
   ```cpp
   void measurePowerUsage() {
       // Measure baseline current
       disableSensor();
       float baselineCurrent = readCurrentSensor();
       
       // Measure with sensor active
       enableSensor();
       float activeCurrent = readCurrentSensor();
       
       Serial.print("Sensor current draw: ");
       Serial.print(activeCurrent - baselineCurrent);
       Serial.println(" mA");
   }
   ```

## 16. Performance Optimization

### CPU Efficiency

1. **Reduce unnecessary calculations**:
   ```cpp
   // Instead of calculating in every update
   float result = complexCalculation(rawValue);

   // Only calculate when needed
   if (needsCalculation) {
       result = complexCalculation(rawValue);
       needsCalculation = false;
   }
   ```

2. **Use fixed-point arithmetic for speed**:
   ```cpp
   // Instead of floating point
   float result = rawValue * 0.01;

   // Use fixed-point (e.g., 100x scale)
   int32_t scaledResult = (rawValue * 100) / 10000;
   ```

3. **Cache computed values**:
   ```cpp
   // Before
   float getComputedValue() {
       return rawValue * calibrationFactor;
   }

   // After
   float getComputedValue() {
       if (isDirty) {
           cachedValue = rawValue * calibrationFactor;
           isDirty = false;
       }
       return cachedValue;
   }
   ```

4. **Optimize conditionals for common case**:
   ```cpp
   // Before
   if (rareCondition) {
       // Rare path
   } else {
       // Common path
   }

   // After
   if (!rareCondition) {
       // Common path
       return;
   }
   // Rare path
   ```

### Memory Usage

1. **Minimize string duplication**:
   ```cpp
   // Define static strings
   static const char KEY_TEMP[] = "temp";
   static const char LABEL_TEMP[] = "Temperature";
   static const char UNIT_TEMP[] = "°C";
   
   addValueInfo(KEY_TEMP, LABEL_TEMP, UNIT_TEMP, 1, TYPE_FLOAT);
   ```

2. **Use smaller integer types when possible**:
   ```cpp
   // Instead of
   int status;       // 4 bytes on most platforms
   
   // Use
   uint8_t status;   // 1 byte
   ```

3. **Reuse buffers**:
   ```cpp
   // Define a shared buffer
   char buffer[64];
   
   // Reuse for different operations
   snprintf(buffer, sizeof(buffer), "Value: %d", value1);
   // ...
   snprintf(buffer, sizeof(buffer), "Status: %s", statusText);
   ```

4. **Optimize JSON usage**:
   ```cpp
   // Use static memory allocation for small documents
   StaticJsonDocument<200> doc;
   
   // Only use dynamic allocation for documents of unknown size
   DynamicJsonDocument doc(1024);
   ```

### Bandwidth Optimization

For sensors that transmit data:

1. **Delta encoding**:
   ```cpp
   // Only send changes rather than full values
   int16_t delta = currentValue - lastSentValue;
   if (abs(delta) > threshold) {
       sendValue(delta);
       lastSentValue = currentValue;
   }
   ```

2. **Batch updates**:
   ```cpp
   // Instead of sending one value at a time
   sendValue("temp", temperature);
   sendValue("humidity", humidity);
   
   // Batch values
   startBatch();
   addToBatch("temp", temperature);
   addToBatch("humidity", humidity);
   sendBatch();
   ```

3. **Compression techniques**:
   ```cpp
   // Pack multiple values into fewer bytes
   // E.g., 16-bit temp and 8-bit humidity into 3 bytes
   uint8_t packet[3];
   packet[0] = (temperature >> 8) & 0xFF;
   packet[1] = temperature & 0xFF;
   packet[2] = humidity & 0xFF;
   ```

## 17. Troubleshooting Guide

### Common Problems

1. **Sensor readings are zero or invalid**
    - **Causes**:
        - Incorrect wiring
        - Initialization failure
        - Power issues
    - **Solutions**:
        - Double-check wiring
        - Verify power supply voltage
        - Add debug output in init()
        - Check return values from hardware operations

2. **Calibration not working**
    - **Causes**:
        - Wrong reference values
        - Incorrect calibration method
        - EEPROM issues
    - **Solutions**:
        - Verify calibration points are reasonable
        - Check if correct calibration method is used
        - Test EEPROM read/write separately
        - Print calibration parameters

3. **Memory issues (crashes, unstable behavior)**
    - **Causes**:
        - Memory leaks
        - Stack overflow
        - Heap fragmentation
    - **Solutions**:
        - Check all allocations have matching deallocations
        - Monitor free memory during operation
        - Reduce deep call stacks
        - Use static allocations where possible

4. **Values not updating**
    - **Causes**:
        - Incorrect update interval
        - Missing update() calls
        - Condition for update never met
    - **Solutions**:
        - Verify update timing
        - Add debug to update method
        - Check if update() returns true/false
        - Ensure main loop calls update regularly

5. **Sensor values fluctuate excessively**
    - **Causes**:
        - Electrical noise
        - Insufficient filtering
        - Hardware issues
    - **Solutions**:
        - Add filtering (moving average, median)
        - Improve power supply stability
        - Add bypass capacitors to sensor
        - Increase ADC sampling (oversampling)

### Debugging Process

Systematic approach to debugging:

1. **Isolate the problem**:
    - Test sensor directly without framework
    - Test with simplified code
    - Verify hardware with known-good code

2. **Add targeted debugging**:
   ```cpp
   void debugSensor() {
       Serial.println("--- DEBUG INFO ---");
       Serial.print("Raw reading: ");
       Serial.println(rawValue);
       Serial.print("Calculated value: ");
       Serial.println(calculatedValue);
       Serial.print("Calibration factor: ");
       Serial.println(calibrationFactor);
       Serial.println("-----------------");
   }
   ```

3. **Use binary search for bugs**:
    - Comment out half of functionality
    - If problem disappears, issue is in commented part
    - If problem remains, issue is in active part
    - Repeat, narrowing down the problematic section

4. **Check timing issues**:
   ```cpp
   unsigned long startTime = millis();
   // Code to test
   unsigned long elapsed = millis() - startTime;
   Serial.print("Execution time: ");
   Serial.println(elapsed);
   ```

5. **Test boundary conditions**:
    - Test with minimum values
    - Test with maximum values
    - Test with invalid values

### Error Codes

Standardized error codes for troubleshooting:

```cpp
// In your sensor class
#define ERROR_NONE 0
#define ERROR_INIT_FAILED -1
#define ERROR_COMMUNICATION -2
#define ERROR_TIMEOUT -3
#define ERROR_INVALID_READING -4
#define ERROR_CALIBRATION -5

// Usage
updateValue("errorCode", ERROR_COMMUNICATION);
```

Error logging function:

```cpp
void logError(int code, const char* message) {
    Serial.print("ERROR ");
    Serial.print(code);
    Serial.print(": ");
    Serial.println(message);
    
    // Store last error
    lastErrorCode = code;
    lastErrorTime = millis();
}
```

## 18. Appendix

### Glossary

- **ADC (Analog-to-Digital Converter)**: Converts analog signals to digital values
- **Calibration**: Process of adjusting sensor readings to match known reference values
- **EEPROM**: Non-volatile memory used to store calibration data
- **I2C**: Serial communication protocol used by many sensors
- **Interpolation**: Estimating values between known calibration points
- **JSON**: JavaScript Object Notation, used for structured data
- **Metadata**: Additional information that describes a value (unit, precision, etc.)
- **SPI**: Serial Peripheral Interface, a communication protocol
- **UART**: Universal Asynchronous Receiver/Transmitter, used for serial communication

### Reference Documentation

- [Arduino Reference](https://www.arduino.cc/reference/en/)
- [ArduinoJson Documentation](https://arduinojson.org/v6/doc/)
- [I2C Bus Specification](https://www.i2c-bus.org/)
- [SPI Protocol](https://www.arduino.cc/en/reference/SPI)

### External Resources

- [Arduino Sensor Tutorials](https://www.arduino.cc/en/Tutorial/BuiltInExamples#sensors)
- [Adafruit Learning System](https://learn.adafruit.com/)
- [Sparkfun Tutorials](https://learn.sparkfun.com/)
- [Random Nerd Tutorials](https://randomnerdtutorials.com/)