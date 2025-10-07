# CeriaSerialI2C Enhanced Documentation

## Overview
CeriaSerialI2C Enhanced adalah implementasi powerful namun tetap simple untuk komunikasi I2C antar mikrokontroler dengan arsitektur master-slave. Versi enhanced ini sekarang menggunakan frame protocol untuk data integrity, robust error handling dengan bus recovery, dan proper data storage untuk slave mode.

## Key Features Enhanced
- **Simple API**: Tetap minimal tapi powerful dengan frame protocol internal
- **Frame Protocol**: XOR checksum untuk data integrity dan type identification
- **Robust Error Handling**: Detailed error codes dengan automatic bus recovery
- **Enhanced Data Storage**: Slave mode sekarang properly menyimpan received data
- **Type-Safe Communication**: Support untuk uint16, float, String dengan validation
- **Statistics Tracking**: Enhanced monitoring dengan error classification
- **Platform Compliant**: Sesuai standar I2C communication best practices
- **Production Ready**: Timeout handling, frame validation, dan bus recovery

## Quick Start Guide

### 1. Master Configuration
```cpp
#include "CeriaSerialI2C.h"
using namespace CeriaDevOP;

CeriaSerialI2C i2cMaster;

void setup() {
    Serial.begin(115200);
    
    // Initialize as master
    if (!i2cMaster.begin(I2C_MASTER)) {
        Serial.println("Failed to initialize I2C Master");
        return;
    }
    
    // Optional configuration
    i2cMaster.setTimeout(2000);    // 2 second timeout
    i2cMaster.setRetries(3);       // 3 retry attempts
    
    // Setup error callback
    i2cMaster.onError([](CeriaI2CError error) {
        Serial.println("I2C Error: " + String(error));
    });
}

void loop() {
    i2cMaster.update();
    
    // Send different data types
    i2cMaster.send(0x08, 255);              // uint8
    i2cMaster.send(0x09, 1234);             // uint16
    i2cMaster.send(0x0A, 3.14f);            // float
    i2cMaster.send(0x0B, "Hello");          // string
    
    // Request data from slaves
    uint16_t sensorValue;
    if (i2cMaster.request(0x08, sensorValue)) {
        Serial.println("Sensor: " + String(sensorValue));
    }
    
    String status;
    if (i2cMaster.request(0x09, status)) {
        Serial.println("Status: " + status);
    }
    
    delay(1000);
}
```

### 2. Slave Configuration
```cpp
#include "CeriaSerialI2C.h"
using namespace CeriaDevOP;

CeriaSerialI2C i2cSlave;
const uint8_t MY_ADDRESS = 0x08;

uint16_t sensorValue = 0;
String deviceStatus = "online";

void setup() {
    Serial.begin(115200);
    
    // Initialize as slave
    if (!i2cSlave.begin(I2C_SLAVE, MY_ADDRESS)) {
        Serial.println("Failed to initialize I2C Slave");
        return;
    }
    
    // Handle incoming data from master
    i2cSlave.onReceive([](uint8_t* data, uint8_t length) {
        Serial.println("Received " + String(length) + " bytes");
        
        // Process received command
        if (length >= 1) {
            uint8_t command = data[0];
            processCommand(command);
        }
    });
    
    // Handle data requests from master
    i2cSlave.onRequest([](uint8_t* data, uint8_t* length) {
        // Respond with current sensor value
        *(uint16_t*)data = sensorValue;
        *length = 2;
    });
}

void loop() {
    i2cSlave.update();
    
    // Update sensor reading
    sensorValue = analogRead(A0);
    
    delay(100);
}
```

## Data Types Support

### Numeric Data Types
```cpp
// Unsigned integers
i2cMaster.send(slaveAddr, (uint8_t)255);     // 1 byte
i2cMaster.send(slaveAddr, (uint16_t)65535);  // 2 bytes
i2cMaster.send(slaveAddr, (uint32_t)123456); // 4 bytes

// Signed integers
i2cMaster.send(slaveAddr, (int8_t)-128);     // 1 byte
i2cMaster.send(slaveAddr, (int16_t)-32768);  // 2 bytes
i2cMaster.send(slaveAddr, (int32_t)-123456); // 4 bytes

// Floating point
i2cMaster.send(slaveAddr, 3.14159f);         // 4 bytes
```

### String Data
```cpp
// String objects
String message = "Hello Slave";
i2cMaster.send(slaveAddr, message);

// C-style strings
i2cMaster.send(slaveAddr, "Status OK");

// Receiving strings
String response;
if (i2cMaster.request(slaveAddr, response)) {
    Serial.println("Response: " + response);
}
```

### Binary Data
```cpp
// Send binary data
uint8_t binaryData[] = {0x01, 0x02, 0x03, 0x04};
i2cMaster.sendBinary(slaveAddr, binaryData, 4);

// Receive binary data
uint8_t buffer[32];
uint8_t length = sizeof(buffer);
if (i2cMaster.requestBinary(slaveAddr, buffer, length)) {
    Serial.println("Received " + String(length) + " bytes");
}
```

## Advanced Usage Patterns

### 1. Multi-Slave Communication
```cpp
const uint8_t TEMP_SENSOR = 0x08;
const uint8_t LED_CONTROLLER = 0x09;
const uint8_t MOTOR_DRIVER = 0x0A;

void communicateWithSlaves() {
    // Get temperature
    float temperature;
    if (i2cMaster.request(TEMP_SENSOR, temperature)) {
        Serial.println("Temperature: " + String(temperature, 2) + "°C");
        
        // Control LED based on temperature
        uint8_t ledBrightness = map(temperature, 0, 50, 0, 255);
        i2cMaster.send(LED_CONTROLLER, ledBrightness);
        
        // Control motor based on temperature
        if (temperature > 30) {
            i2cMaster.send(MOTOR_DRIVER, "start_fan");
        }
    }
}
```

### 2. Command-Response Pattern
```cpp
// Master sending commands
void sendCommand(uint8_t slaveAddr, const String& command) {
    if (i2cMaster.send(slaveAddr, command)) {
        Serial.println("Command sent: " + command);
        
        // Wait for acknowledgment
        String response;
        if (i2cMaster.request(slaveAddr, response)) {
            Serial.println("Response: " + response);
        }
    }
}

// Usage
sendCommand(0x08, "read_sensor");
sendCommand(0x09, "toggle_led");
sendCommand(0x0A, "get_status");
```

### 3. Sensor Node Pattern
```cpp
// Sensor slave implementation
void setupSensorSlave() {
    i2cSlave.onReceive([](uint8_t* data, uint8_t length) {
        // Parse command
        String command = String((char*)data, length);
        
        if (command == "read_temp") {
            currentSensorType = TEMPERATURE;
        } else if (command == "read_hum") {
            currentSensorType = HUMIDITY;
        } else if (command == "read_all") {
            currentSensorType = ALL_SENSORS;
        }
    });
    
    i2cSlave.onRequest([](uint8_t* data, uint8_t* length) {
        switch (currentSensorType) {
            case TEMPERATURE:
                *(float*)data = readTemperature();
                *length = 4;
                break;
                
            case HUMIDITY:
                *(float*)data = readHumidity();
                *length = 4;
                break;
                
            case ALL_SENSORS: {
                struct SensorData {
                    float temperature;
                    float humidity;
                    uint16_t light;
                } sensorData;
                
                sensorData.temperature = readTemperature();
                sensorData.humidity = readHumidity();
                sensorData.light = readLightSensor();
                
                memcpy(data, &sensorData, sizeof(sensorData));
                *length = sizeof(sensorData);
                break;
            }
        }
    });
}
```

## Error Handling dan Debugging

### Error Types
```cpp
enum CeriaI2CError {
    CERIA_I2C_ERR_NONE = 0,          // No error
    CERIA_I2C_ERR_TIMEOUT,           // Communication timeout
    CERIA_I2C_ERR_NACK_ADDR,         // No ACK on address
    CERIA_I2C_ERR_NACK_DATA,         // No ACK on data
    CERIA_I2C_ERR_OTHER,             // Other I2C error
    CERIA_I2C_ERR_BUFFER_FULL,       // Buffer overflow
    CERIA_I2C_ERR_NO_DATA            // No data received
};
```

### Comprehensive Error Handling
```cpp
void setupErrorHandling() {
    i2cMaster.onError([](CeriaI2CError error) {
        switch (error) {
            case CERIA_I2C_ERR_TIMEOUT:
                Serial.println("Communication timeout - check connections");
                break;
                
            case CERIA_I2C_ERR_NACK_ADDR:
                Serial.println("No ACK on address - slave not responding");
                break;
                
            case CERIA_I2C_ERR_NACK_DATA:
                Serial.println("No ACK on data - transmission failed");
                break;
                
            case CERIA_I2C_ERR_BUFFER_FULL:
                Serial.println("Buffer overflow - data too large");
                break;
                
            case CERIA_I2C_ERR_NO_DATA:
                Serial.println("No data received from slave");
                break;
                
            default:
                Serial.println("Unknown I2C error: " + String(error));
                break;
        }
    });
}
```

### Connection Monitoring
```cpp
void monitorSlaves() {
    const uint8_t slaves[] = {0x08, 0x09, 0x0A, 0x0B};
    const uint8_t numSlaves = sizeof(slaves) / sizeof(slaves[0]);
    
    Serial.println("=== I2C Bus Scan ===");
    for (uint8_t i = 0; i < numSlaves; i++) {
        uint8_t addr = slaves[i];
        
        if (i2cMaster.isConnected(addr)) {
            Serial.println("✓ Slave 0x" + String(addr, HEX) + " online");
        } else {
            Serial.println("✗ Slave 0x" + String(addr, HEX) + " offline");
        }
    }
}
```

### Statistics Monitoring
```cpp
void printCommunicationStats() {
    uint32_t sent, received, errors;
    i2cMaster.getStats(sent, received, errors);
    
    Serial.println("=== Communication Statistics ===");
    Serial.println("Frames sent: " + String(sent));
    Serial.println("Frames received: " + String(received));
    Serial.println("Total errors: " + String(errors));
    
    if (sent > 0) {
        float successRate = (float)(sent - errors) / sent * 100;
        Serial.println("Success rate: " + String(successRate, 1) + "%");
    }
    
    if (errors > 0) {
        float errorRate = (float)errors / sent * 100;
        Serial.println("Error rate: " + String(errorRate, 1) + "%");
    }
}
```

## Hardware Considerations

### I2C Connection Requirements
```
Master ESP32        Slave Arduino
GPIO 21 (SDA) ----> A4 (SDA)
GPIO 22 (SCL) ----> A5 (SCL)
GND ------------> GND
```

### Pull-up Resistors
- **Standard I2C**: 4.7kΩ pull-up resistors pada SDA dan SCL
- **Fast Mode**: 2.2kΩ untuk higher speed (400kHz)
- **High-Speed**: 1kΩ untuk very high speed applications

### Multiple Slaves Wiring
```
Master (ESP32)
├── SDA ─┬── Slave1 (0x08)
│        ├── Slave2 (0x09)
│        └── Slave3 (0x0A)
│
├── SCL ─┬── Slave1
│        ├── Slave2
│        └── Slave3
│
└── GND ─┴── All Slaves
```

### Address Assignment
```cpp
// Standard I2C addresses (7-bit)
const uint8_t SENSOR_NODE_1    = 0x08;  // 0001000
const uint8_t SENSOR_NODE_2    = 0x09;  // 0001001
const uint8_t LED_CONTROLLER   = 0x10;  // 0010000
const uint8_t MOTOR_DRIVER     = 0x11;  // 0010001
const uint8_t DISPLAY_MODULE   = 0x3C;  // 0111100 (common OLED)
const uint8_t RTC_MODULE       = 0x68;  // 1101000 (DS3231)

// Avoid reserved addresses:
// 0x00-0x07: Reserved
// 0x78-0x7F: Reserved
```

## Performance Optimization

### Buffer Management
```cpp
// Optimal packet sizes untuk different data types
uint8_t data:     1 + 2 = 3 bytes total
uint16_t data:    2 + 2 = 4 bytes total
uint32_t data:    4 + 2 = 6 bytes total
float data:       4 + 2 = 6 bytes total
String data:      n + 2 = n+2 bytes total (max 30 chars)
Binary data:      n + 2 = n+2 bytes total (max 30 bytes)
```

### Communication Frequency
```cpp
void optimizedCommunication() {
    static unsigned long lastCommunication = 0;
    const unsigned long COMM_INTERVAL = 100;  // 100ms = 10Hz
    
    if (millis() - lastCommunication >= COMM_INTERVAL) {
        // Batch multiple operations
        uint16_t sensor1 = 0, sensor2 = 0;
        String status = "";
        
        if (i2cMaster.request(0x08, sensor1) &&
            i2cMaster.request(0x09, sensor2) &&
            i2cMaster.request(0x0A, status)) {
            
            // Process all data together
            processSensorData(sensor1, sensor2, status);
        }
        
        lastCommunication = millis();
    }
}
```

## Integration dengan CeriaDev

### CeriaDevOP.h Integration
```cpp
// Add ke CeriaDevOP.h
#include "lib/CeriaSerialI2C.h"
#include "lib/CeriaSerialI2C.cpp"
```

### Combined Communication Strategy
```cpp
// Hybrid approach: I2C untuk local, Serial untuk long-distance
CeriaSerial longRangeComm;      // Untuk node-to-node communication
CeriaSerialI2C localI2C;        // Untuk sensor/actuator communication

void setupHybridCommunication() {
    // Serial untuk inter-node
    longRangeComm.begin(Serial1, 115200);
    
    // I2C untuk local devices
    localI2C.begin(I2C_MASTER);
    
    // Forward sensor data dari I2C ke Serial
    static unsigned long lastForward = 0;
    if (millis() - lastForward > 1000) {
        float temperature;
        if (localI2C.request(0x08, temperature)) {
            longRangeComm.send("node1_temp", temperature);
        }
        lastForward = millis();
    }
}
```

## API Reference

### Constructor & Initialization
```cpp
CeriaSerialI2C();
~CeriaSerialI2C();
bool begin(I2CMode mode, uint8_t slaveAddress = 0, TwoWire* wire = &Wire);
```

### Configuration Methods
```cpp
void setTimeout(uint32_t ms);               // Set timeout (default: 1000ms)
void setRetries(uint8_t count);            // Set retry count (default: 3)
void onReceive(CeriaI2CReceiveCallback);   // Set receive callback (slave)
void onRequest(CeriaI2CRequestCallback);   // Set request callback (slave)
void onError(CeriaI2CErrorCallback);       // Set error callback
```

### Master Methods
```cpp
// Template methods (auto-type detection)
template<typename T> bool send(uint8_t slaveAddr, T value);
template<typename T> bool request(uint8_t slaveAddr, T& value);

// Specific type methods
bool sendTyped(uint8_t slaveAddr, uint8_t/uint16_t/uint32_t value);
bool sendTyped(uint8_t slaveAddr, int8_t/int16_t/int32_t value);
bool sendTyped(uint8_t slaveAddr, float value);
bool sendTyped(uint8_t slaveAddr, const String& value);
bool sendBinary(uint8_t slaveAddr, uint8_t* data, uint8_t length);

bool requestTyped(uint8_t slaveAddr, uint8_t/uint16_t/uint32_t& value);
bool requestTyped(uint8_t slaveAddr, int8_t/int16_t/int32_t& value);
bool requestTyped(uint8_t slaveAddr, float& value);
bool requestTyped(uint8_t slaveAddr, String& value);
bool requestBinary(uint8_t slaveAddr, uint8_t* data, uint8_t& length);
```

### Slave Response Methods
```cpp
void respondWith(uint8_t* data, uint8_t length);
void respondWith(uint8_t/uint16_t/uint32_t value);
void respondWith(int8_t/int16_t/int32_t value);
void respondWith(float value);
void respondWith(const String& value);
```

### Status & Monitoring
```cpp
void update();                             // Process I2C events (call in loop)
bool isConnected(uint8_t slaveAddr);      // Check slave connection
bool hasNewData();                        // Check for new data (slave)
void getStats(uint32_t& sent, uint32_t& received, uint32_t& errors);
void reset();                             // Reset statistics
```

## Best Practices

### 1. Initialization Pattern
```cpp
void setup() {
    Serial.begin(115200);
    
    // Always check initialization
    if (!i2c.begin(mode, address)) {
        Serial.println("FATAL: I2C initialization failed");
        while(1) delay(1000);
    }
    
    // Setup callbacks before communication
    i2c.onReceive(handleReceive);
    i2c.onRequest(handleRequest);
    i2c.onError(handleError);
}
```

### 2. Main Loop Pattern
```cpp
void loop() {
    // Always call update first
    i2c.update();
    
    // Rate limiting untuk transmission
    static unsigned long lastAction = 0;
    if (millis() - lastAction > INTERVAL) {
        performCommunication();
        lastAction = millis();
    }
    
    // Monitor connection health
    if (!i2c.isConnected(slaveAddr)) {
        handleConnectionLoss();
    }
}
```

### 3. Error Recovery Pattern
```cpp
void robustCommunication(uint8_t addr, float data) {
    static uint8_t errorCount = 0;
    static unsigned long lastError = 0;
    
    if (!i2c.send(addr, data)) {
        errorCount++;
        lastError = millis();
        
        if (errorCount > 5) {
            // Reset communication
            i2c.reset();
            delay(1000);
            errorCount = 0;
        }
    } else {
        errorCount = 0;  // Reset pada successful transmission
    }
}
```

## Troubleshooting Guide

### 1. Communication Failures
**Problem**: `send()` returns false, NACK errors
**Solutions**:
- Verify I2C wiring (SDA/SCL crossed?)
- Check pull-up resistors (4.7kΩ recommended)
- Verify slave address (use bus scanner)
- Check power supply stability

### 2. Data Corruption
**Problem**: Received data tidak sesuai yang dikirim
**Solutions**:
- Check for EMI interference
- Verify ground connections
- Reduce I2C clock speed
- Add ferrite cores pada cables

### 3. Performance Issues
**Problem**: Slow communication atau timeouts
**Solutions**:
- Optimize communication frequency (don't spam bus)
- Use appropriate timeout values
- Batch multiple operations together
- Monitor bus utilization

CeriaSerialI2C menyediakan robust foundation untuk inter-microcontroller I2C communication dengan simple API dan comprehensive error handling, perfect untuk integration dalam CeriaDev ecosystem.