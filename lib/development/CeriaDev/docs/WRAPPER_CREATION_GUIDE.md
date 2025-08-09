# CeriaDev Wrapper Creation Guide

## Overview

This guide provides comprehensive instructions for creating user-friendly sensor and module wrappers within the CeriaDev ecosystem. These wrappers follow a standardized pattern that makes complex libraries accessible through simple, powerful APIs.

## Design Philosophy

### Core Principles
1. **Inheritance over Composition** - Extend existing libraries rather than wrapping them
2. **Simple API Surface** - Essential methods only, avoid over-engineering
3. **Clean Data Access** - Direct getter methods instead of complex structs
4. **String Output Formats** - Human-readable outputs, avoid JSON complexity
5. **Smart Defaults** - Sensible configuration out-of-the-box
6. **Complete Documentation** - Include base class reference in wrapper docs

### Naming Convention
- **Header**: `Ceria[SensorType][LibraryName].h`
- **Implementation**: `Ceria[SensorType][LibraryName].cpp`
- **Class**: `Ceria[SensorType][LibraryName]`
- **Namespace**: `CeriaDevOP`

**Examples:**
- `CeriaSensorGPS` (GPS sensors)
- `CeriaSensorPower` (Power monitoring)
- `CeriaModuleWiFi` (WiFi modules)
- `CeriaDisplayOLED` (Display modules)

## File Structure Template

### Simple Header File Template (`CeriaSensor[Name].h`)

```cpp
#ifndef CERIA_SENSOR_[NAME]_H
#define CERIA_SENSOR_[NAME]_H

#include "Arduino.h"
#include "[OriginalLibrary].h"  // Include the base library

namespace CeriaDevOP {

class CeriaSensor[Name] : public [OriginalLibraryClass] {
public:
    // Constructor
    CeriaSensor[Name]();
    
    // Initialization
    void begin([initialization_parameters]);
    
    // Core functionality
    void update();
    bool hasNewData();
    bool isConnected(uint32_t timeoutMs = 5000);
    
    // Simple data access methods
    [data_type] get[MainValue]();
    [data_type] get[SecondaryValue]();
    bool is[MainValue]Valid();
    
    // Configuration methods (if needed)
    void set[ConfigOption]([parameters]);
    
    // String output
    String get[Name]String();
    
    // Utility methods
    bool waitForReading(uint32_t timeoutMs = 5000);

private:
    // Minimal internal state
    bool _hasNewData;
    uint32_t _lastUpdateTime;
    [essential_private_members];
};

}

#endif
```

### Simple Implementation File Template (`CeriaSensor[Name].cpp`)

```cpp
#include "CeriaSensor[Name].h"

namespace CeriaDevOP {

CeriaSensor[Name]::CeriaSensor[Name]() : [OriginalLibraryClass]() {
    _hasNewData = false;
    _lastUpdateTime = 0;
    // Initialize other essential private members
}

void CeriaSensor[Name]::begin([initialization_parameters]) {
    // Initialize the base library
    [OriginalLibraryClass]::begin([parameters]);
    _lastUpdateTime = millis();
}

void CeriaSensor[Name]::update() {
    // Read from the sensor using base library methods
    // Check if reading was successful (use base library success methods)
    if ([success_check]) {
        _hasNewData = true;
        _lastUpdateTime = millis();
    }
}

bool CeriaSensor[Name]::hasNewData() {
    bool result = _hasNewData;
    _hasNewData = false;
    return result;
}

bool CeriaSensor[Name]::isConnected(uint32_t timeoutMs) {
    return (millis() - _lastUpdateTime) < timeoutMs && [additional_check];
}

[data_type] CeriaSensor[Name]::get[MainValue]() {
    return [base_library_method]();
}

[data_type] CeriaSensor[Name]::get[SecondaryValue]() {
    return [base_library_method]();
}

bool CeriaSensor[Name]::is[MainValue]Valid() {
    return [base_library_validity_check]();
}

void CeriaSensor[Name]::set[ConfigOption]([parameters]) {
    [base_library_config_method]([parameters]);
}

String CeriaSensor[Name]::get[Name]String() {
    if (![validity_check]) return "[Name] Error";
    
    return String(get[MainValue]()) + "[units], " + 
           String(get[SecondaryValue]()) + "[units]";
}

bool CeriaSensor[Name]::waitForReading(uint32_t timeoutMs) {
    uint32_t startTime = millis();
    while (millis() - startTime < timeoutMs) {
        update();
        if (hasNewData() && is[MainValue]Valid()) {
            return true;
        }
        delay(10);
    }
    return false;
}

}
```

## Documentation Requirements

### Wrapper Documentation Template (`CeriaSensor[Name].md`)

Create comprehensive documentation for each wrapper following this structure:

#### 1. Basic Wrapper Documentation
```markdown
# CeriaSensor[Name]

## Overview
Brief description of the wrapper and its purpose.

## Features
- List of key wrapper features
- Simple API highlights
- Hardware support notes

## Basic Usage
```cpp
// Simple initialization and usage example
CeriaSensor[Name] sensor;
sensor.begin();
sensor.update();
if (sensor.hasNewData()) {
    Serial.println(sensor.get[Name]String());
}
```

## API Reference
Complete list of wrapper methods with signatures and descriptions.
```

#### 2. **REQUIRED: Base Class Documentation Section**

**IMPORTANT:** Every wrapper documentation MUST include a complete base class reference section:

```markdown
## [OriginalLibraryName] Base Class Reference

[WrapperClass] inherits dari [OriginalLibraryClass], sehingga semua methods original tetap tersedia. Berikut dokumentasi lengkap dari base class:

### Core [OriginalLibraryName] Methods

#### Initialization Methods
```cpp
// List all initialization methods from base class
[OriginalLibraryClass]([constructor_params]);
bool begin([init_params]);
```

#### Data Reading Methods
```cpp
// List all data reading methods
[return_type] get[DataType]();
[return_type] get[DataType]_[units]();
```

#### Configuration Methods
```cpp
// List all configuration methods
void set[ConfigOption]([params]);
void [configMethod]([params]);
```

#### Advanced/Raw Methods
```cpp
// List low-level or advanced methods
[return_type] get[DataType]_raw();
[return_type] [advancedMethod]();
```

### Advanced Usage Examples

#### Example 1: Using Base Class Methods
```cpp
void advancedExample() {
    sensor.update();
    
    // Mix wrapper and base class methods
    String simple = sensor.get[Name]String();  // Wrapper method
    [data_type] raw = sensor.[baseMethod]();   // Base class method
    
    Serial.println("Simple: " + simple);
    Serial.println("Raw: " + String(raw));
}
```

#### Example 2: Accessing All Features
```cpp
// Show how to access advanced base class features
// that are not exposed in the wrapper
```

#### Example 3: Performance Optimization
```cpp
// Show direct base class usage for performance-critical code
```

### Base Class Constants and Enums
```cpp
// Document important constants, enums, and defines
// from the original library
#define [CONSTANT_NAME] [value]
enum [EnumName] {
    [ENUM_VALUE_1],
    [ENUM_VALUE_2]
};
```

[WrapperClass] provides clean, simple access to [functionality] while maintaining all the power and flexibility of the underlying [OriginalLibraryName] library. You can mix and match wrapper methods dengan original [OriginalLibraryName] methods sesuai kebutuhan aplikasi Anda.
```

### Documentation Research Process

Before writing wrapper documentation, thoroughly analyze the base library:

#### Step 1: Header File Analysis
```bash
# Read the main header file
find [library_path] -name "*.h" -exec cat {} \;

# Look for:
# - Class definitions and public methods
# - Constants and enums
# - Configuration options
# - Data structures
```

#### Step 2: Source File Analysis
```bash
# Read implementation files for method details
find [library_path] -name "*.cpp" -exec cat {} \;

# Look for:
# - Method implementations
# - Default values
# - Internal logic and limitations
# - Error handling patterns
```

#### Step 3: Example Analysis
```bash
# Study existing examples
find [library_path]/examples -name "*.ino" -exec cat {} \;

# Extract:
# - Common usage patterns
# - Advanced features
# - Performance considerations
# - Best practices
```

## Simplified Implementation Examples

### Example 1: Simple Power Monitoring (INA219)

```cpp
// CeriaSensorPower.h
#ifndef CERIA_SENSOR_POWER_H
#define CERIA_SENSOR_POWER_H

#include "Adafruit_INA219.h"

namespace CeriaDevOP {

class CeriaSensorPower : public Adafruit_INA219 {
public:
    CeriaSensorPower(uint8_t addr = 0x40);
    
    void begin(TwoWire *wire = &Wire);
    void update();
    
    bool hasNewData();
    bool isConnected(uint32_t timeoutMs = 5000);
    
    float getVoltage();        // V
    float getCurrent();        // mA
    float getPower();          // mW
    
    String getPowerString();   // "12.5V, 250mA, 3.1W"
    
    bool waitForReading(uint32_t timeoutMs = 5000);

private:
    bool _hasNewData;
    uint32_t _lastUpdateTime;
    uint8_t _i2cAddress;
};

}

#endif
```

### Example 2: Simple Environmental Sensor (BME280)

```cpp
// CeriaSensorEnvironment.h
#ifndef CERIA_SENSOR_ENVIRONMENT_H
#define CERIA_SENSOR_ENVIRONMENT_H

#include "Adafruit_BME280.h"

namespace CeriaDevOP {

class CeriaSensorEnvironment : public Adafruit_BME280 {
public:
    CeriaSensorEnvironment();
    
    void begin(uint8_t addr = 0x76);
    void update();
    
    bool hasNewData();
    bool isConnected(uint32_t timeoutMs = 5000);
    
    float getTemperature();    // °C
    float getHumidity();       // %RH
    float getPressure();       // hPa
    float getAltitude();       // m
    
    String getEnvironmentString(); // "25.6°C, 65%RH, 1013hPa"
    
    bool waitForReading(uint32_t timeoutMs = 5000);

private:
    bool _hasNewData;
    uint32_t _lastUpdateTime;
};

}

#endif
```

## Best Practices

### 1. Simple API Design (RECOMMENDED)
```cpp
// ✅ Good: Direct getter methods
float getTemperature();
float getHumidity();
bool isTemperatureValid();
String getEnvironmentString();

// ❌ Avoid: Complex structures for simple data
struct EnvironmentData {
    float temperature;
    float humidity;
    bool valid;
    uint32_t age;
};
EnvironmentData getData();  // Too complex for simple wrappers
```

### 2. Inheritance Best Practices
```cpp
// ✅ Good: Clean inheritance with base class access
class CeriaSensorGPS : public TinyGPSPlus {
public:
    // Simple wrapper methods
    double getLatitude() { return location.lat(); }
    String getLocation() { /* format string */ }
    
    // Base class methods still available:
    // gps.location.isValid()
    // gps.satellites.value()
    // gps.charsProcessed()
};

// ❌ Avoid: Hiding useful base class functionality
class BadWrapper {
private:
    TinyGPSPlus gps;  // Composition hides base methods
};
```

### 3. Error Handling Patterns
```cpp
// ✅ Good: Use base library success methods
void update() {
    if (success()) {  // Use base class success checking
        _hasNewData = true;
        _lastUpdateTime = millis();
    }
}

// ✅ Good: Graceful degradation in string output
String getPowerString() {
    if (!success()) return "INA219 Error";
    return String(getVoltage(), 2) + "V, " + String(getCurrent(), 1) + "mA";
}
```

### 4. Avoid Over-Engineering
```cpp
// ❌ Avoid: Complex callback systems
void setUpdateCallback(void (*callback)(SensorData));
void setThresholdCallback(void (*callback)(SensorData), float threshold);
void setLocationChangeCallback(void (*callback)(GPSLocation), double minDistance);

// ❌ Avoid: JSON output for simple wrappers
String toJSON();  // Skip this - use simple string output instead

// ❌ Avoid: Statistics and advanced features in simple wrappers
void resetStatistics();
float getAverageReading();
uint32_t getSampleCount();

// ✅ Good: Keep it simple
String getSensorString();  // Simple, human-readable output
bool waitForReading(uint32_t timeoutMs);  // Essential utility only
```

## Integration with CeriaDev

### 1. Adding to Enable System
Add definitions to `/lib/enable.h`:
```cpp
// Sensor Enables
#define ENABLE_CERIA_SENSOR_GPS
#define ENABLE_CERIA_SENSOR_POWER
#define ENABLE_CERIA_SENSOR_ENVIRONMENT
```

### 2. Module Orchestration
Add to appropriate orchestration files:
```cpp
// In KinematrixModules.h
#ifdef ENABLE_CERIA_SENSOR_GPS
#include "../lib/development/CeriaDev/lib/CeriaSensorGPS.h"
#include "../lib/development/CeriaDev/lib/CeriaSensorGPS.cpp"
#endif
```

### 3. Example Creation
Create examples in `/example/CeriaDev/`:
```
example/
└── CeriaDev/
    ├── EXAMPLE-CeriaSensorGPS/
    │   ├── basic-usage/
    │   ├── with-callbacks/
    │   └── advanced-features/
    └── EXAMPLE-CeriaSensorPower/
        ├── power-monitoring/
        └── overload-protection/
```

## Testing Guidelines

### 1. Basic Functionality Test
```cpp
void testBasicFunctionality() {
    CeriaSensor[Name] sensor;
    sensor.begin();
    
    // Test initialization
    assert(sensor.isConnected());
    
    // Test data reading
    sensor.update();
    [Name]Data data = sensor.getData();
    assert(data.valid);
    
    // Test output formats
    String json = sensor.toJSON();
    assert(json.length() > 0);
}
```

### 2. Callback Test
```cpp
bool callbackTriggered = false;

void testCallback() {
    CeriaSensor[Name] sensor;
    sensor.setUpdateCallback([](auto data) {
        callbackTriggered = true;
    });
    
    sensor.begin();
    sensor.update();
    
    assert(callbackTriggered);
}
```

### 3. Platform Compatibility
Test on multiple platforms:
- **ESP32**: Full feature set
- **ESP8266**: Memory-optimized features
- **Arduino Uno**: Basic functionality only

## Common Patterns

### 1. Multi-Sensor Support
```cpp
class CeriaSensorMulti : public BaseSensorLibrary {
private:
    static const uint8_t MAX_SENSORS = 4;
    SensorData _sensorData[MAX_SENSORS];
    
public:
    SensorData getData(uint8_t sensorIndex = 0);
    void updateAll();
    void setGlobalCallback(void (*callback)(uint8_t index, SensorData));
};
```

### 2. Calibration Support
```cpp
struct CalibrationData {
    float offset = 0.0;
    float scale = 1.0;
    bool enabled = false;
};

void calibrate(float referenceValue);
void setCalibration(const CalibrationData &cal);
CalibrationData getCalibration();
```

### 3. Data Logging
```cpp
void enableDataLogging(bool enable = true);
void setLogCallback(void (*callback)(String timestamp, SensorData));
String getLoggedData(uint32_t maxEntries = 100);
```

## Wrapper Documentation Checklist

### Pre-Documentation Research
- [ ] Read and analyze all header files (.h) in the base library
- [ ] Read and analyze all source files (.cpp) in the base library  
- [ ] Study all example files in the base library
- [ ] Identify key classes, methods, constants, and enums
- [ ] Test base library functionality to understand behavior
- [ ] Document any limitations or quirks discovered

### Required Documentation Sections
- [ ] **Overview** - Brief description and purpose
- [ ] **Features** - Key wrapper features list
- [ ] **Basic Usage** - Simple initialization and usage examples
- [ ] **API Reference** - Complete wrapper method documentation
- [ ] **Usage Examples** - Multiple real-world scenarios
- [ ] **Hardware Setup** - Wiring and configuration guides
- [ ] **Troubleshooting** - Common problems and solutions
- [ ] **Integration** - CeriaDev integration instructions

### **MANDATORY: Base Class Documentation**
- [ ] **Base Class Reference section** - Complete original library documentation
- [ ] **Core Methods** - All public methods from base class with signatures
- [ ] **Constants and Enums** - Important defines and enums from base library
- [ ] **Advanced Usage Examples** - Using base class methods directly
- [ ] **Mixed Usage Examples** - Combining wrapper and base class methods
- [ ] **Performance Examples** - Direct base class usage for optimization
- [ ] **Raw Data Access** - Low-level base class features
- [ ] **Configuration Options** - Advanced base class configuration

### Documentation Quality Standards
- [ ] All method signatures include parameter types and return types
- [ ] Code examples are tested and functional
- [ ] Examples progress from basic to advanced
- [ ] Base class methods are thoroughly documented
- [ ] Real-world usage scenarios are included
- [ ] Performance considerations are noted
- [ ] Memory usage implications are documented
- [ ] Platform compatibility is specified

### Example Documentation Structure Verification
```markdown
# CeriaSensor[Name]
## Overview
## Features  
## Basic Usage
## API Reference
## Usage Examples
## Hardware Setup
## Performance Notes
## Troubleshooting
## Integration with CeriaDev

## [OriginalLibraryName] Base Class Reference  ← MANDATORY SECTION
### Core [OriginalLibraryName] Methods
### Advanced Usage Examples
### Base Class Constants and Enums
### Performance Optimization
### Raw Data Access

[WrapperClass] provides clean, simple access to [functionality] while 
maintaining all the power and flexibility of the underlying 
[OriginalLibraryName] library.
```

## Common Documentation Mistakes to Avoid

### ❌ Incomplete Base Class Documentation
```markdown
## Advanced Usage
// Access original methods
sensor.originalMethod();
```

### ✅ Complete Base Class Documentation
```markdown
## TinyGPSPlus Base Class Reference

### Core TinyGPSPlus Objects
#### Location Object (`gps.location`)
```cpp
bool location.isValid();
double location.lat();
double location.lng();
// ... complete method list
```

### ❌ Missing Method Signatures
```markdown
// Get GPS data
getData()
```

### ✅ Complete Method Signatures  
```markdown
```cpp
double getLatitude();                               // Get latitude in decimal degrees
double getLongitude();                              // Get longitude in decimal degrees
bool isLocationValid();                             // Check if location data is valid
```

### ❌ No Real-World Examples
```markdown
gps.update();
Serial.println(gps.getLocation());
```

### ✅ Complete Real-World Examples
```markdown
#### GPS Tracker with Distance Monitoring
```cpp
const double HOME_LAT = -6.200000;
const double HOME_LNG = 106.816666;

void loop() {
    gps.update();
    if (gps.hasNewData() && gps.hasFix()) {
        double distance = gps.distanceTo(HOME_LAT, HOME_LNG);
        Serial.println("Distance from home: " + String(distance/1000.0, 2) + " km");
    }
}
```

## Documentation Maintenance

### Version Updates
- [ ] Update documentation when wrapper API changes
- [ ] Update base class documentation when library updates
- [ ] Test all examples after library updates
- [ ] Verify compatibility notes remain accurate

### Continuous Improvement  
- [ ] Add new examples based on user feedback
- [ ] Expand troubleshooting section based on issues
- [ ] Update performance notes with benchmarks
- [ ] Add platform-specific notes as needed

## Conclusion

Following this guide ensures consistent, powerful, and user-friendly sensor wrappers that integrate seamlessly with the CeriaDev ecosystem. The key principles are:

1. **Keep wrappers simple** - Essential methods only, avoid over-engineering
2. **Preserve base class access** - Inheritance allows users to access original library features
3. **Document everything** - Comprehensive wrapper AND base class documentation
4. **Provide real examples** - Show practical, working code for common scenarios
5. **Test thoroughly** - Verify all examples work and document limitations

Remember: **Inherit from existing libraries, keep APIs minimal, document completely, and provide comprehensive base class access.**