# CeriaSensorAnalog

## Overview
CeriaSensorAnalog provides a comprehensive wrapper for reading analog sensors with advanced features including calibration, statistics tracking, multi-sampling, and platform optimization.

## Features

### Core Functionality
- **Multi-platform Support**: Automatic resolution detection (ESP32: 12-bit, ESP8266/AVR: 10-bit)
- **Multi-sampling**: Configurable sample averaging for noise reduction
- **Real-time Statistics**: Min/max/average tracking with reading counts
- **Calibration System**: Two-point calibration with offset and multiplier
- **Range Mapping**: Configurable input ranges with percentage conversion
- **Update Intervals**: Configurable sampling rates to reduce processing load

### Advanced Features
- **Voltage Conversion**: Accurate voltage calculation with configurable reference
- **Scaling Functions**: Linear mapping to custom scales
- **Connection Monitoring**: Timeout-based health checking
- **Blocking Operations**: Wait for specific values or reading updates
- **Statistics Reset**: Runtime statistics management

## Usage Examples

### Basic Usage
```cpp
#include "CeriaSensorAnalog.h"
using namespace CeriaDevOP;

CeriaSensorAnalog sensor(A0);

void setup() {
    Serial.begin(115200);
    sensor.begin(3.3);  // 3.3V reference
    sensor.setRange(0, 4095);  // ESP32 12-bit range
}

void loop() {
    sensor.update();
    
    if (sensor.hasNewData()) {
        Serial.println("Raw: " + String(sensor.getRaw()));
        Serial.println("Voltage: " + String(sensor.getVoltage(), 3) + "V");
        Serial.println("Percentage: " + String(sensor.getPercentage(), 1) + "%");
        Serial.println("Status: " + sensor.getStatusString());
    }
    
    delay(100);
}
```

### Advanced Configuration
```cpp
CeriaSensorAnalog sensor(A0);

void setup() {
    Serial.begin(115200);
    sensor.begin(5.0);  // 5V reference
    
    // Configure sampling
    sensor.setSampling(10);  // Average 10 samples
    sensor.setUpdateInterval(50);  // Update every 50ms
    
    // Set custom range for sensor (e.g., pressure sensor 0.5V-4.5V)
    sensor.setRange(102, 922);  // Corresponding ADC values
    
    // Enable calibration
    sensor.enableCalibration(0.02, 0.98);  // Offset: 20mV, Scale: 98%
}

void loop() {
    sensor.update();
    
    if (sensor.hasNewData()) {
        // Get scaled value (e.g., 0-100 PSI)
        float pressure = sensor.getScaled(0.0, 100.0);
        Serial.println("Pressure: " + String(pressure, 1) + " PSI");
        
        // Print comprehensive status
        Serial.println(sensor.getStatusString());
        Serial.println(sensor.getStatisticsString());
    }
    
    delay(10);
}
```

### Calibration Example
```cpp
CeriaSensorAnalog sensor(A0);

void setup() {
    Serial.begin(115200);
    sensor.begin(3.3);
    
    // Zero calibration (sensor at 0 point)
    Serial.println("Place sensor at zero point, then press any key...");
    while (!Serial.available());
    sensor.calibrateZero();
    Serial.println("Zero calibrated");
    
    // Span calibration (sensor at known point)
    Serial.println("Place sensor at 10.0V point, then press any key...");
    while (!Serial.available());
    sensor.calibrateSpan(10.0);
    Serial.println("Span calibrated");
    
    Serial.println("Calibration complete!");
}

void loop() {
    sensor.update();
    
    if (sensor.hasNewData()) {
        Serial.println("Calibrated Value: " + String(sensor.getVoltage(), 3) + "V");
        Serial.println(sensor.getAnalogString());
    }
    
    delay(1000);
}
```

### Blocking Operations
```cpp
void loop() {
    // Wait for sensor to reach specific voltage
    if (sensor.waitForValue(2.5, 0.1, 5000)) {
        Serial.println("Sensor reached 2.5V ± 0.1V");
    } else {
        Serial.println("Timeout waiting for target value");
    }
    
    // Wait for any new reading
    if (sensor.waitForReading(1000)) {
        Serial.println("New reading: " + sensor.getAnalogString());
    }
    
    delay(1000);
}
```

## API Reference

### Constructor
- `CeriaSensorAnalog(uint8_t pin)` - Initialize with analog pin number

### Initialization
- `void begin(float referenceVoltage = 3.3)` - Initialize sensor with reference voltage

### Data Reading
- `void update()` - Read and process sensor data
- `bool hasNewData()` - Check for new data since last call
- `bool isConnected(uint32_t timeoutMs = 5000)` - Check sensor connectivity

### Data Access
- `uint16_t getRaw()` - Get raw ADC value
- `float getVoltage()` - Get voltage value (calibrated if enabled)
- `float getPercentage()` - Get percentage based on configured range
- `float getScaled(float minScale, float maxScale)` - Get value scaled to custom range

### Configuration
- `void setRange(uint16_t minValue, uint16_t maxValue)` - Set input range for percentage calculation
- `void setReferenceVoltage(float voltage)` - Set ADC reference voltage
- `void setResolution(AnalogResolution resolution)` - Set ADC resolution (ESP32 only)
- `void setSampling(uint8_t samples)` - Set number of samples to average (1-100)
- `void setUpdateInterval(uint32_t intervalMs)` - Set minimum time between updates

### Calibration
- `void enableCalibration(float offset = 0.0, float multiplier = 1.0)` - Enable two-point calibration
- `void disableCalibration()` - Disable calibration
- `void calibrateZero()` - Auto-calibrate zero point (current reading = 0V)
- `void calibrateSpan(float knownValue)` - Auto-calibrate span with known value

### Statistics
- `void resetStatistics()` - Reset all statistics counters
- `uint32_t getTotalReadings()` - Get total number of readings
- `float getAverageValue()` - Get average of all raw readings
- `uint16_t getMinReading()` - Get minimum raw reading
- `uint16_t getMaxReading()` - Get maximum raw reading

### String Output
- `String getAnalogString()` - Get formatted "raw (voltage, percentage)" string
- `String getStatusString()` - Get comprehensive status string
- `String getStatisticsString()` - Get statistics summary string

### Blocking Operations
- `bool waitForReading(uint32_t timeoutMs = 5000)` - Wait for new reading
- `bool waitForValue(float targetValue, float tolerance = 0.1, uint32_t timeoutMs = 10000)` - Wait for specific voltage

### Utility Functions
- `float mapFloat(float value, float fromMin, float fromMax, float toMin, float toMax)` - Float mapping function
- `uint16_t getResolutionMax()` - Get maximum value for current resolution

## Platform Support

### ESP32
- **Detection**: `ESP32` macro
- **Resolution**: 12-bit (0-4095) 
- **Reference**: 3.3V default (no analogReference() function)
- **Features**: Configurable resolution (8/10/12/16-bit)
- **Pins**: A0-A19 (GPIO 36, 39, 34, 35, 32, 33, 25-27, 14, 12-15, 2, 0, 4, 16-17, 9-10)

### ESP8266  
- **Detection**: `ESP8266` macro
- **Resolution**: 10-bit (0-1023)
- **Reference**: 3.3V default (no analogReference() function)
- **Pins**: A0 only (GPIO 17)

### Arduino Uno/Nano
- **Detection**: `ARDUINO_AVR_UNO`, `ARDUINO_AVR_NANO`, `__AVR_ATmega328P__`, `__AVR_ATmega168__`
- **Resolution**: 10-bit (0-1023)
- **Reference**: 5V default
- **Available References**: 
  - `ANALOG_REF_DEFAULT` (5V)
  - `ANALOG_REF_INTERNAL_1V1` (1.1V internal)
  - `ANALOG_REF_EXTERNAL` (External AREF pin)
- **Pins**: A0-A7

### Arduino Mega
- **Detection**: `ARDUINO_AVR_MEGA2560`, `__AVR_ATmega1280__`, `__AVR_ATmega2560__`  
- **Resolution**: 10-bit (0-1023)
- **Reference**: 5V default
- **Available References**:
  - `ANALOG_REF_DEFAULT` (5V)
  - `ANALOG_REF_INTERNAL_1V1` (1.1V internal)
  - `ANALOG_REF_INTERNAL_2V56` (2.56V internal)
  - `ANALOG_REF_EXTERNAL` (External AREF pin)
- **Pins**: A0-A15

### Arduino Due
- **Detection**: `ARDUINO_SAM_DUE`
- **Resolution**: 12-bit (0-4095)
- **Reference**: 3.3V default (only option)
- **Available References**: 
  - `ANALOG_REF_DEFAULT` / `ANALOG_REF_3V3` (3.3V)
- **Pins**: A0-A11

## Technical Specifications

### Performance
- **Update Rate**: Configurable (0ms = continuous, up to any interval)
- **Sampling**: 1-100 samples averaging
- **Precision**: Platform native (10-12 bit)
- **Calibration**: Two-point linear correction

### Memory Usage
- **RAM**: ~50 bytes per instance
- **Flash**: ~3KB compiled code

### Accuracy
- **Voltage**: ±0.1% with proper calibration
- **Linearity**: Platform ADC dependent
- **Noise**: Reduced by multi-sampling

## Examples Integration

This sensor integrates seamlessly with CeriaDev ecosystem:

```cpp
#include "CeriaDevOP.h"

CeriaSensorAnalog sensor(A0);
CeriaSerial comm;

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    
    sensor.begin(3.3);
    comm.begin(Serial1, 115200);
    
    sensor.setSampling(5);  // Reduce noise
    sensor.setUpdateInterval(100);  // 10Hz updates
}

void loop() {
    sensor.update();
    comm.tick();
    
    if (sensor.hasNewData()) {
        // Send data via CeriaSerial
        comm.send("analog_raw", sensor.getRaw());
        comm.send("analog_voltage", sensor.getVoltage());
        comm.send("analog_percentage", sensor.getPercentage());
        
        Serial.println(sensor.getStatusString());
    }
    
    delay(10);
}
```

## Summary

CeriaSensorAnalog provides enterprise-grade analog sensor reading with:
- Platform-optimized ADC handling
- Advanced calibration capabilities  
- Real-time statistics tracking
- Configurable sampling and filtering
- Comprehensive error handling
- Integration-ready design for CeriaDev ecosystem

Perfect for precision analog measurements in industrial, scientific, and IoT applications requiring reliable and accurate sensor data acquisition.