# CeriaSensorINA219

## Overview
CeriaSensorINA219 adalah wrapper sederhana dan powerful untuk Adafruit_INA219 library yang menyediakan interface yang clean untuk high-side DC current sensing. Dirancang untuk monitoring power consumption dengan precision tinggi tanpa kompleksitas yang tidak perlu.

## Features
- **Simple API**: Methods yang intuitif untuk voltage, current, dan power monitoring
- **Multiple Calibration Modes**: 3 preset modes untuk different voltage/current ranges
- **I2C Auto-Detection**: Built-in connection status checking
- **Power Management**: Built-in power save mode support
- **Data Validation**: Automatic error detection dan success checking
- **String Formatting**: Human-readable output format
- **Blocking Wait**: Optional wait-for-reading functionality

## Specifications
- **Bus Voltage Range**: 0-16V atau 0-32V (tergantung calibration mode)
- **Current Range**: 0-400mA hingga 0-3.2A (tergantung calibration mode)
- **Resolution**: 40uA hingga 100uA per bit (tergantung calibration mode)
- **Interface**: I2C (address 0x40-0x4F, configurable via A0/A1 pins)
- **Shunt Resistor**: 0.1Ω (built-in pada breakout board)

## Calibration Modes

### INA219_CAL_32V_2A (Default)
- **Voltage Range**: 0-32V
- **Current Range**: 0-3.2A (max before overflow)
- **Resolution**: 100uA per bit
- **Power Resolution**: 2mW per bit
- **Use Case**: General purpose, high current applications

### INA219_CAL_32V_1A
- **Voltage Range**: 0-32V
- **Current Range**: 0-1.3A (max before overflow)
- **Resolution**: 40uA per bit
- **Power Resolution**: 0.8mW per bit
- **Use Case**: Medium current with better resolution

### INA219_CAL_16V_400MA
- **Voltage Range**: 0-16V
- **Current Range**: 0-400mA (max)
- **Resolution**: 50uA per bit
- **Power Resolution**: 1mW per bit
- **Use Case**: Low voltage/current applications, highest precision

## Basic Usage

### Initialization
```cpp
#include "CeriaSensorINA219.h"
using namespace CeriaDevOP;

CeriaSensorINA219 power;  // Default I2C address 0x40

void setup() {
    Serial.begin(115200);
    power.begin(&Wire, INA219_CAL_32V_2A);  // 32V/2A mode
}
```

### Reading Power Data
```cpp
void loop() {
    power.update();
    
    if (power.hasNewData()) {
        if (power.isConnected()) {
            Serial.println("=== Power Monitor ===");
            Serial.println("Voltage: " + String(power.getVoltage(), 3) + " V");
            Serial.println("Current: " + String(power.getCurrent(), 2) + " mA");
            Serial.println("Power: " + String(power.getPower(), 2) + " mW");
            Serial.println("Summary: " + power.getPowerString());
        } else {
            Serial.println("INA219 not connected");
        }
    }
    
    delay(1000);
}
```

## API Reference

### Initialization Methods
```cpp
CeriaSensorINA219(uint8_t addr = 0x40);              // Constructor with I2C address
void begin(TwoWire *wire = &Wire,                    // Initialize sensor
          INA219CalibrationMode mode = INA219_CAL_32V_2A);
```

### Core Methods
```cpp
void update();                                       // Read sensor data
bool hasNewData();                                   // Check for new data since last call
bool isConnected(uint32_t timeoutMs = 5000);        // Check sensor connection
```

### Data Reading Methods
```cpp
float getVoltage();                                  // Get bus voltage in V
float getCurrent();                                  // Get current in mA
float getPower();                                    // Get power in mW
```

### Configuration Methods
```cpp
void setCalibrationMode(INA219CalibrationMode mode); // Change calibration mode
void setPowerSave(bool enabled);                     // Enable/disable power save mode
```

### Output Methods
```cpp
String getPowerString();                             // Get formatted string "12.5V, 250mA, 3.1W"
```

### Utility Methods
```cpp
bool waitForReading(uint32_t timeoutMs = 5000);      // Wait for valid reading with timeout
```

## Usage Examples

### Basic Power Monitor
```cpp
void setup() {
    Serial.begin(115200);
    power.begin(&Wire, INA219_CAL_32V_2A);
    
    if (power.waitForReading(5000)) {
        Serial.println("INA219 initialized successfully");
    } else {
        Serial.println("Failed to initialize INA219");
    }
}

void loop() {
    power.update();
    
    if (power.hasNewData()) {
        Serial.println(power.getPowerString());
    }
    
    delay(1000);
}
```

### Battery Monitor
```cpp
void setup() {
    Serial.begin(115200);
    // Use high precision mode for battery monitoring
    power.begin(&Wire, INA219_CAL_16V_400MA);
}

void loop() {
    power.update();
    
    if (power.hasNewData()) {
        float voltage = power.getVoltage();
        float current = power.getCurrent();
        
        Serial.println("Battery Status:");
        Serial.println("  Voltage: " + String(voltage, 3) + "V");
        Serial.println("  Current: " + String(current, 2) + "mA");
        
        // Battery level estimation (for Li-Ion)
        if (voltage > 4.0) {
            Serial.println("  Level: High");
        } else if (voltage > 3.7) {
            Serial.println("  Level: Medium");
        } else if (voltage > 3.3) {
            Serial.println("  Level: Low");
        } else {
            Serial.println("  Level: Critical");
        }
        
        // Current direction
        if (current > 0) {
            Serial.println("  Status: Discharging");
        } else {
            Serial.println("  Status: Charging");
        }
    }
    
    delay(2000);
}
```

### Power Supply Analysis
```cpp
void setup() {
    Serial.begin(115200);
    power.begin(&Wire, INA219_CAL_32V_1A);  // Good balance of range and precision
}

void loop() {
    power.update();
    
    if (power.hasNewData()) {
        float voltage = power.getVoltage();
        float current = power.getCurrent();
        float powerW = power.getPower() / 1000.0;  // Convert mW to W
        
        Serial.println("=== Power Supply Analysis ===");
        Serial.println("Input: " + String(voltage, 2) + "V");
        Serial.println("Load: " + String(current, 1) + "mA");
        Serial.println("Power: " + String(powerW, 3) + "W");
        
        // Calculate efficiency (assuming 5V input)
        float inputPower = 5.0 * (current / 1000.0);  // Convert mA to A
        if (inputPower > 0) {
            float efficiency = (powerW / inputPower) * 100.0;
            Serial.println("Efficiency: " + String(efficiency, 1) + "%");
        }
        
        // Load regulation check
        if (voltage < 4.8) {
            Serial.println("WARNING: Voltage drop detected");
        }
    }
    
    delay(1000);
}
```

### Multiple INA219 Sensors
```cpp
CeriaSensorINA219 power1(0x40);  // A0=GND, A1=GND
CeriaSensorINA219 power2(0x41);  // A0=VCC, A1=GND
CeriaSensorINA219 power3(0x44);  // A0=GND, A1=VCC

void setup() {
    Serial.begin(115200);
    
    power1.begin(&Wire, INA219_CAL_32V_2A);
    power2.begin(&Wire, INA219_CAL_32V_2A);
    power3.begin(&Wire, INA219_CAL_16V_400MA);
}

void loop() {
    power1.update();
    power2.update();
    power3.update();
    
    if (power1.hasNewData() || power2.hasNewData() || power3.hasNewData()) {
        Serial.println("=== Multi-Channel Power Monitor ===");
        Serial.println("Channel 1 (0x40): " + power1.getPowerString());
        Serial.println("Channel 2 (0x41): " + power2.getPowerString());
        Serial.println("Channel 3 (0x44): " + power3.getPowerString());
        
        float totalPower = power1.getPower() + power2.getPower() + power3.getPower();
        Serial.println("Total Power: " + String(totalPower, 1) + "mW");
    }
    
    delay(1000);
}
```

## Hardware Setup

### Basic Connections
```
INA219        →    ESP32/Arduino
VCC           →    3.3V or 5V
GND           →    GND
SDA           →    SDA (I2C Data)
SCL           →    SCL (I2C Clock)
```

### Load Connection
```
Power Source (+) → VIN+ (INA219) → Load (+)
Power Source (-) → VIN- (INA219) → Load (-)
```

### I2C Address Configuration
```
A1    A0    Address
GND   GND   0x40 (default)
GND   VCC   0x41
VCC   GND   0x44
VCC   VCC   0x45
```

## Performance Notes

### Update Frequency
- Call `update()` setiap kali ingin membaca data
- INA219 conversion time: ~532μs hingga ~69ms (tergantung averaging setting)
- Recommended update interval: 100ms hingga 1s untuk most applications

### Accuracy Considerations
- ±0.5% accuracy pada bus voltage measurement
- ±1% accuracy pada current measurement  
- Temperature coefficient: ±50ppm/°C
- Avoid long wire runs untuk precision measurements

### Power Consumption
- INA219 consumption: ~1mA typical
- Power save mode: ~10μA
- Use power save mode untuk battery applications when not measuring

## Troubleshooting

### No Communication
1. Check I2C connections (SDA, SCL)
2. Verify I2C address with I2C scanner
3. Check pull-up resistors (4.7kΩ typical)
4. Verify power supply (3.3V or 5V)

### Incorrect Readings
1. Check load connections (VIN+, VIN-)
2. Verify shunt resistor value (should be 0.1Ω)
3. Consider different calibration mode untuk current range
4. Check for ground loops atau noise

### Overflow/Clipping
1. Current readings capped at calibration limit
2. Switch to higher current calibration mode
3. Check actual current vs expected range
4. Consider multiple INA219 untuk high current applications

## Integration with CeriaDev

### Enable in Project
```cpp
// In your enable.h or main file
#define ENABLE_CERIA_SENSOR_INA219
#include "CeriaSensorINA219.h"
```

### Platform Compatibility
- **ESP32**: Full support, hardware I2C
- **ESP8266**: Full support, hardware I2C
- **Arduino Uno**: Full support, hardware I2C
- **Arduino Mega**: Full support, multiple I2C buses available

## Adafruit_INA219 Base Class Reference

CeriaSensorINA219 inherits dari Adafruit_INA219, sehingga semua methods original tetap tersedia. Berikut dokumentasi lengkap dari base class:

### Core Adafruit_INA219 Methods

#### Initialization Methods
```cpp
Adafruit_INA219(uint8_t addr = INA219_ADDRESS);      // Constructor
bool begin(TwoWire *theWire = &Wire);                // Initialize I2C communication
```

#### Data Reading Methods
```cpp
float getBusVoltage_V();                             // Get bus voltage in volts
float getShuntVoltage_mV();                          // Get shunt voltage in millivolts
float getCurrent_mA();                               // Get current in milliamps
float getPower_mW();                                 // Get power in milliwatts
```

#### Raw Data Methods
```cpp
int16_t getBusVoltage_raw();                         // Get raw bus voltage register value
int16_t getShuntVoltage_raw();                       // Get raw shunt voltage register value
int16_t getCurrent_raw();                            // Get raw current register value
int16_t getPower_raw();                              // Get raw power register value
```

#### Calibration Methods
```cpp
void setCalibration_32V_2A();                        // Set for 32V, 2A range (100uA resolution)
void setCalibration_32V_1A();                        // Set for 32V, 1A range (40uA resolution)
void setCalibration_16V_400mA();                     // Set for 16V, 400mA range (50uA resolution)
```

#### Power Management
```cpp
void powerSave(bool on);                             // Enable/disable power save mode
bool success();                                      // Check if last operation was successful
```

### INA219 Register Constants

#### I2C Addresses
```cpp
#define INA219_ADDRESS (0x40)                        // Default I2C address (A0=GND, A1=GND)
// Address calculation: 0x40 | (A0 ? 0x01 : 0x00) | (A1 ? 0x04 : 0x00)
// 0x40: A0=GND, A1=GND    0x41: A0=VCC, A1=GND
// 0x44: A0=GND, A1=VCC    0x45: A0=VCC, A1=VCC
```

#### Configuration Register Values
```cpp
// Bus Voltage Range
INA219_CONFIG_BVOLTAGERANGE_16V                      // 0-16V range
INA219_CONFIG_BVOLTAGERANGE_32V                      // 0-32V range

// PGA Gain Settings
INA219_CONFIG_GAIN_1_40MV                            // ±40mV range
INA219_CONFIG_GAIN_2_80MV                            // ±80mV range
INA219_CONFIG_GAIN_4_160MV                           // ±160mV range
INA219_CONFIG_GAIN_8_320MV                           // ±320mV range

// ADC Resolution/Averaging
INA219_CONFIG_BADCRES_9BIT                           // 9-bit resolution
INA219_CONFIG_BADCRES_10BIT                          // 10-bit resolution
INA219_CONFIG_BADCRES_11BIT                          // 11-bit resolution
INA219_CONFIG_BADCRES_12BIT                          // 12-bit resolution
INA219_CONFIG_BADCRES_12BIT_2S_1060US                // 12-bit, 2 samples averaged
INA219_CONFIG_BADCRES_12BIT_4S_2130US                // 12-bit, 4 samples averaged
// ... up to 128 samples averaged

// Operating Modes
INA219_CONFIG_MODE_POWERDOWN                         // Power down
INA219_CONFIG_MODE_SVOLT_TRIGGERED                   // Shunt voltage triggered
INA219_CONFIG_MODE_BVOLT_TRIGGERED                   // Bus voltage triggered
INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED               // Shunt and bus triggered
INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS              // Continuous conversion
```

### Advanced Usage Examples

#### Raw Data Analysis
```cpp
void analyzeRawData() {
    power.update();
    
    if (power.isConnected()) {
        // Get both processed and raw values
        float busVoltage = power.getBusVoltage_V();
        float shuntVoltage = power.getShuntVoltage_mV();
        float current = power.getCurrent_mA();
        float powerMW = power.getPower_mW();
        
        int16_t rawBus = power.getBusVoltage_raw();
        int16_t rawShunt = power.getShuntVoltage_raw();
        int16_t rawCurrent = power.getCurrent_raw();
        int16_t rawPower = power.getPower_raw();
        
        Serial.println("=== Raw vs Processed Data ===");
        Serial.println("Bus Voltage: " + String(busVoltage, 3) + "V (raw: " + String(rawBus) + ")");
        Serial.println("Shunt Voltage: " + String(shuntVoltage, 3) + "mV (raw: " + String(rawShunt) + ")");
        Serial.println("Current: " + String(current, 2) + "mA (raw: " + String(rawCurrent) + ")");
        Serial.println("Power: " + String(powerMW, 2) + "mW (raw: " + String(rawPower) + ")");
        
        // Calculate values manually for verification
        float busLSB = 0.004;  // 4mV per bit
        float shuntLSB = 0.01; // 0.01mV per bit
        
        float calculatedBus = rawBus * busLSB;
        float calculatedShunt = rawShunt * shuntLSB;
        
        Serial.println("=== Manual Calculations ===");
        Serial.println("Calculated Bus: " + String(calculatedBus, 3) + "V");
        Serial.println("Calculated Shunt: " + String(calculatedShunt, 3) + "mV");
    }
}
```

#### Precision Measurement with Error Checking
```cpp
void precisionMeasurement() {
    // Take multiple readings for averaging
    const int samples = 10;
    float voltageSum = 0, currentSum = 0, powerSum = 0;
    int validSamples = 0;
    
    for (int i = 0; i < samples; i++) {
        power.update();
        
        if (power.success()) {  // Check if reading was successful
            voltageSum += power.getBusVoltage_V();
            currentSum += power.getCurrent_mA();
            powerSum += power.getPower_mW();
            validSamples++;
        }
        
        delay(10);  // Small delay between readings
    }
    
    if (validSamples > 0) {
        float avgVoltage = voltageSum / validSamples;
        float avgCurrent = currentSum / validSamples;
        float avgPower = powerSum / validSamples;
        
        Serial.println("=== Precision Measurement ===");
        Serial.println("Samples: " + String(validSamples) + "/" + String(samples));
        Serial.println("Avg Voltage: " + String(avgVoltage, 4) + "V");
        Serial.println("Avg Current: " + String(avgCurrent, 3) + "mA");
        Serial.println("Avg Power: " + String(avgPower, 3) + "mW");
        
        // Calculate standard deviation for current
        float currentVariance = 0;
        for (int i = 0; i < samples; i++) {
            // Re-read for variance calculation (simplified example)
            float reading = power.getCurrent_mA();
            if (power.success()) {
                currentVariance += sq(reading - avgCurrent);
            }
        }
        
        float currentStdDev = sqrt(currentVariance / validSamples);
        Serial.println("Current StdDev: " + String(currentStdDev, 3) + "mA");
    }
}
```

#### Power Monitoring with Data Logging
```cpp
struct PowerReading {
    uint32_t timestamp;
    float voltage;
    float current;
    float power;
    bool valid;
};

PowerReading readings[100];  // Circular buffer
int readingIndex = 0;

void logPowerData() {
    power.update();
    
    // Store reading in circular buffer
    readings[readingIndex].timestamp = millis();
    readings[readingIndex].voltage = power.getBusVoltage_V();
    readings[readingIndex].current = power.getCurrent_mA();
    readings[readingIndex].power = power.getPower_mW();
    readings[readingIndex].valid = power.success();
    
    readingIndex = (readingIndex + 1) % 100;
}

void analyzeStoredData() {
    Serial.println("=== Power Analysis (Last 100 readings) ===");
    
    float minVoltage = 999, maxVoltage = 0;
    float minCurrent = 999, maxCurrent = 0;
    float totalEnergy = 0;  // mW*s
    int validCount = 0;
    
    uint32_t firstTime = 0, lastTime = 0;
    
    for (int i = 0; i < 100; i++) {
        if (readings[i].valid) {
            if (validCount == 0) firstTime = readings[i].timestamp;
            lastTime = readings[i].timestamp;
            
            minVoltage = min(minVoltage, readings[i].voltage);
            maxVoltage = max(maxVoltage, readings[i].voltage);
            minCurrent = min(minCurrent, readings[i].current);
            maxCurrent = max(maxCurrent, readings[i].current);
            
            totalEnergy += readings[i].power;  // Simplified energy calculation
            validCount++;
        }
    }
    
    if (validCount > 0) {
        uint32_t duration = (lastTime - firstTime) / 1000;  // seconds
        float energyWh = totalEnergy / 3600.0 / 1000.0;     // Convert to Wh
        
        Serial.println("Valid readings: " + String(validCount) + "/100");
        Serial.println("Duration: " + String(duration) + " seconds");
        Serial.println("Voltage range: " + String(minVoltage, 2) + "V - " + String(maxVoltage, 2) + "V");
        Serial.println("Current range: " + String(minCurrent, 1) + "mA - " + String(maxCurrent, 1) + "mA");
        Serial.println("Energy consumed: " + String(energyWh, 4) + " Wh");
    }
}
```

#### Multiple Sensor Management
```cpp
struct PowerChannel {
    CeriaSensorINA219* sensor;
    String name;
    float voltageOffset;  // For calibration
    float currentOffset;  // For calibration
};

PowerChannel channels[] = {
    {new CeriaSensorINA219(0x40), "Main Power", 0.0, 0.0},
    {new CeriaSensorINA219(0x41), "Motor Driver", 0.0, 0.0},
    {new CeriaSensorINA219(0x44), "Sensor Power", 0.0, 0.0}
};

const int numChannels = sizeof(channels) / sizeof(channels[0]);

void initializeMultiChannels() {
    Serial.begin(115200);
    
    for (int i = 0; i < numChannels; i++) {
        channels[i].sensor->begin(&Wire, INA219_CAL_32V_2A);
        
        if (channels[i].sensor->waitForReading(2000)) {
            Serial.println(channels[i].name + " initialized successfully");
        } else {
            Serial.println(channels[i].name + " initialization failed");
        }
    }
}

void updateAllChannels() {
    float totalPower = 0;
    
    Serial.println("=== Multi-Channel Power Monitor ===");
    
    for (int i = 0; i < numChannels; i++) {
        channels[i].sensor->update();
        
        if (channels[i].sensor->hasNewData() && channels[i].sensor->isConnected()) {
            float voltage = channels[i].sensor->getVoltage() + channels[i].voltageOffset;
            float current = channels[i].sensor->getCurrent() + channels[i].currentOffset;
            float power = channels[i].sensor->getPower();
            
            Serial.println(channels[i].name + ": " + 
                          String(voltage, 2) + "V, " +
                          String(current, 1) + "mA, " +
                          String(power, 1) + "mW");
            
            totalPower += power;
        } else {
            Serial.println(channels[i].name + ": OFFLINE");
        }
    }
    
    Serial.println("Total System Power: " + String(totalPower, 1) + "mW");
    Serial.println("Total System Current: " + String(totalPower / 12.0, 1) + "mA @ 12V");
}
```

#### Power Save Mode Management
```cpp
class PowerManager {
private:
    CeriaSensorINA219* ina219;
    bool powerSaveEnabled;
    uint32_t lastActiveTime;
    uint32_t powerSaveDelay;
    
public:
    PowerManager(CeriaSensorINA219* sensor, uint32_t saveDelayMs = 30000) {
        ina219 = sensor;
        powerSaveEnabled = false;
        lastActiveTime = millis();
        powerSaveDelay = saveDelayMs;
    }
    
    void update() {
        // Check if we should enter power save mode
        if (!powerSaveEnabled && (millis() - lastActiveTime > powerSaveDelay)) {
            Serial.println("Entering power save mode");
            ina219->setPowerSave(true);
            powerSaveEnabled = true;
        }
        
        // Always try to update (will wake up if needed)
        ina219->update();
        
        if (ina219->hasNewData()) {
            if (powerSaveEnabled) {
                Serial.println("Exiting power save mode");
                ina219->setPowerSave(false);
                powerSaveEnabled = false;
            }
            lastActiveTime = millis();
        }
    }
    
    void forceWakeup() {
        if (powerSaveEnabled) {
            Serial.println("Force wakeup from power save");
            ina219->setPowerSave(false);
            powerSaveEnabled = false;
            lastActiveTime = millis();
        }
    }
    
    bool isInPowerSave() { return powerSaveEnabled; }
};

PowerManager powerMgr(&power, 30000);  // 30 second timeout

void loop() {
    powerMgr.update();
    
    if (power.hasNewData()) {
        Serial.println(power.getPowerString());
    }
    
    // Force wakeup on button press or other event
    if (digitalRead(BUTTON_PIN) == LOW) {
        powerMgr.forceWakeup();
    }
    
    delay(1000);
}
```

### Register-Level Access (Advanced)

```cpp
// Access to underlying I2C device for custom operations
void customRegisterAccess() {
    // Note: This requires understanding of INA219 register map
    // Register addresses from Adafruit_INA219.h:
    // INA219_REG_CONFIG (0x00)
    // INA219_REG_SHUNTVOLTAGE (0x01)
    // INA219_REG_BUSVOLTAGE (0x02)
    // INA219_REG_POWER (0x03)
    // INA219_REG_CURRENT (0x04)
    // INA219_REG_CALIBRATION (0x05)
    
    // Example: Read configuration register
    // (This would require access to private i2c_dev member)
    // Advanced users can implement custom register access
    // by studying the Adafruit_INA219 source code
    
    Serial.println("Custom register access requires");
    Serial.println("direct modification of Adafruit_INA219 library");
    Serial.println("or inheritance with protected access");
}
```

CeriaSensorINA219 provides clean, simple access to precision power monitoring while maintaining all the flexibility of the underlying Adafruit_INA219 library. You can mix and match wrapper methods dengan original Adafruit_INA219 methods sesuai kebutuhan aplikasi Anda untuk maximum flexibility dan performance.