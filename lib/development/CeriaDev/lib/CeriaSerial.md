# CeriaSerial Protocol Documentation

## Overview
CeriaSerial adalah protokol komunikasi serial enterprise-grade untuk komunikasi antar mikrokontroler dengan fitur advanced seperti frame validation, error recovery, dan collision detection.

## Key Features
- **Robust Frame Protocol**: STX/ETX framing dengan byte stuffing untuk integritas data
- **CRC16 Protection**: Error detection dan validation menggunakan CRC16 checksum
- **Dual Communication Modes**: Full-duplex dan half-duplex dengan collision detection
- **Platform Optimization**: Adaptive buffering berdasarkan platform (ESP32: 256B, ESP8266: 128B, AVR: 64B)
- **Type-Safe Data**: Template-based sending dengan auto-detection tipe data
- **Health Monitoring**: Automatic heartbeat dan connection status monitoring
- **Statistics Tracking**: Built-in counters untuk frames sent/received/errors

## Quick Start Guide

### 1. Basic Setup
```cpp
#include "CeriaSerial.h"
using namespace CeriaDevOP;

CeriaSerial comm;

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);  // Hardware serial untuk komunikasi
    
    // Initialize CeriaSerial
    if (comm.begin(Serial1, 115200)) {
        Serial.println("CeriaSerial initialized successfully");
    } else {
        Serial.println("Failed to initialize CeriaSerial");
    }
}

void loop() {
    comm.tick();  // Process incoming data
    
    // Send data every second
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 1000) {
        comm.send("uptime", millis());
        lastSend = millis();
    }
}
```

### 2. Advanced Configuration
```cpp
void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    
    // Initialize dengan konfigurasi khusus
    comm.begin(Serial1, 115200, FRAME_256B);  // Frame size 256 bytes
    comm.setMode(HALF_DUPLEX);                 // Half-duplex mode
    comm.setRetries(5);                        // 5 retry attempts
    comm.setTimeout(3000);                     // 3 second timeout
    
    // Setup callbacks
    comm.onReceive(handleReceivedData);
    comm.onError(handleCommError);
}

void handleReceivedData(const String& key, const String& value) {
    Serial.println("Received: " + key + " = " + value);
    
    // Response berdasarkan key
    if (key == "get_status") {
        comm.send("status", "online");
    } else if (key == "get_temp") {
        float temperature = 25.6;  // Baca sensor
        comm.send("temperature", temperature);
    }
}

void handleCommError(CeriaSerialError error) {
    Serial.println("Communication Error: " + String(error));
    
    switch (error) {
        case CERIA_SERIAL_ERR_TIMEOUT:
            Serial.println("Communication timeout");
            break;
        case CERIA_SERIAL_ERR_CRC_FAILED:
            Serial.println("Frame corruption detected");
            break;
        case CERIA_SERIAL_ERR_COLLISION:
            Serial.println("Collision detected (half-duplex)");
            break;
        default:
            Serial.println("Unknown error");
            break;
    }
}
```

## Data Types dan Sending Methods

### 1. Numeric Data
```cpp
// Integer types
comm.send("counter", 42);
comm.send("pin_state", true);
comm.send("sensor_id", 255);

// Float types dengan 6 digit precision
comm.send("temperature", 25.67f);
comm.send("voltage", 3.3);
comm.send("humidity", 65.2);
```

### 2. String Data
```cpp
// String literals
comm.send("status", "online");
comm.send("device_name", "Sensor_Node_01");

// String objects
String message = "System OK";
comm.send("message", message);
```

### 3. JSON Data (Auto-Detection)
```cpp
// JSON objects (auto-detected)
comm.send("config", "{\"mode\":1,\"interval\":1000}");
comm.send("sensor_data", "{\"temp\":25.6,\"hum\":60.2}");

// JSON arrays
comm.send("readings", "[25.6, 60.2, 1013.25]");
```

### 4. Reliable Transmission
```cpp
// Send dengan retry mechanism
bool success = comm.send("critical_data", 42, true);  // reliable = true
if (!success) {
    Serial.println("Failed to send critical data");
}
```

## Communication Modes

### Full-Duplex Mode (Default)
Komunikasi simultan dua arah tanpa collision detection.
```cpp
comm.setMode(FULL_DUPLEX);

void loop() {
    comm.tick();
    
    // Bisa send kapan saja
    if (sensorChanged()) {
        comm.send("sensor", readSensor());
    }
}
```

### Half-Duplex Mode
Komunikasi bergantian dengan collision detection dan retry mechanism.
```cpp
comm.setMode(HALF_DUPLEX);

void loop() {
    comm.tick();
    
    // Channel akan dicek sebelum sending
    if (needToSend()) {
        bool sent = comm.send("data", value);
        if (!sent) {
            // Collision detected, akan retry otomatis
            Serial.println("Collision, retrying...");
        }
    }
}
```

## Error Handling dan Monitoring

### Connection Monitoring
```cpp
void loop() {
    comm.tick();
    
    // Check connection status
    if (!comm.isConnected()) {
        Serial.println("Connection lost!");
        // Implement reconnection logic
    }
    
    // Manual ping
    static unsigned long lastPing = 0;
    if (millis() - lastPing > 5000) {
        comm.ping();
        lastPing = millis();
    }
}
```

### Statistics Tracking
```cpp
void printStats() {
    uint32_t sent, received, errors;
    comm.getStats(sent, received, errors);
    
    Serial.println("=== CeriaSerial Statistics ===");
    Serial.println("Frames sent: " + String(sent));
    Serial.println("Frames received: " + String(received));
    Serial.println("Total errors: " + String(errors));
    
    float successRate = (sent > 0) ? ((float)(sent - errors) / sent * 100) : 100;
    Serial.println("Success rate: " + String(successRate, 1) + "%");
}
```

### Error Recovery
```cpp
void handleCommError(CeriaSerialError error) {
    switch (error) {
        case CERIA_SERIAL_ERR_TIMEOUT:
            // Increase timeout or check physical connection
            comm.setTimeout(comm.getTimeout() + 1000);
            break;
            
        case CERIA_SERIAL_ERR_CRC_FAILED:
            // Frame corruption, possibly EMI interference
            // Implement error counter and reset if too many
            break;
            
        case CERIA_SERIAL_ERR_BUFFER_FULL:
            // Reduce sending frequency or increase buffer size
            comm.reset();  // Clear buffers
            break;
            
        case CERIA_SERIAL_ERR_COLLISION:
            // Normal dalam half-duplex mode
            // System akan retry otomatis
            break;
    }
}
```

## Advanced Usage Patterns

### 1. Request-Response Pattern
```cpp
// Master node
void requestSensorData() {
    comm.send("get_temperature", "");
    comm.send("get_humidity", "");
}

void handleReceivedData(const String& key, const String& value) {
    if (key == "temperature") {
        float temp = value.toFloat();
        Serial.println("Temperature: " + String(temp) + "°C");
    } else if (key == "humidity") {
        float hum = value.toFloat();
        Serial.println("Humidity: " + String(hum) + "%");
    }
}

// Slave node  
void handleReceivedData(const String& key, const String& value) {
    if (key == "get_temperature") {
        float temp = readTemperature();
        comm.send("temperature", temp);
    } else if (key == "get_humidity") {
        float hum = readHumidity();
        comm.send("humidity", hum);
    }
}
```

### 2. Command Processing
```cpp
void handleReceivedData(const String& key, const String& value) {
    if (key == "cmd") {
        processCommand(value);
    } else if (key == "set_led") {
        digitalWrite(LED_PIN, value.toInt());
        comm.send("led_status", value);  // Confirm
    } else if (key == "get_pin") {
        int pinNum = value.toInt();
        int pinValue = digitalRead(pinNum);
        comm.send("pin_" + String(pinNum), pinValue);
    }
}

void processCommand(const String& cmd) {
    if (cmd == "restart") {
        comm.send("status", "restarting");
        delay(100);
        ESP.restart();
    } else if (cmd == "get_info") {
        comm.send("device_id", "ESP32_Node_01");
        comm.send("firmware", "v1.0.0");
        comm.send("uptime", millis());
    }
}
```

### 3. Multi-Node Network
```cpp
// Gateway node
struct NodeInfo {
    String nodeId;
    unsigned long lastSeen;
    bool isOnline;
};

NodeInfo nodes[10];
int nodeCount = 0;

void handleReceivedData(const String& key, const String& value) {
    // Parse node ID dari key format "nodeX_data"
    int underscoreIndex = key.indexOf('_');
    if (underscoreIndex > 0) {
        String nodeId = key.substring(0, underscoreIndex);
        String dataType = key.substring(underscoreIndex + 1);
        
        updateNodeStatus(nodeId);
        processNodeData(nodeId, dataType, value);
    }
}

void updateNodeStatus(const String& nodeId) {
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].nodeId == nodeId) {
            nodes[i].lastSeen = millis();
            nodes[i].isOnline = true;
            return;
        }
    }
    
    // New node
    if (nodeCount < 10) {
        nodes[nodeCount].nodeId = nodeId;
        nodes[nodeCount].lastSeen = millis();
        nodes[nodeCount].isOnline = true;
        nodeCount++;
    }
}

// Sensor node
void loop() {
    comm.tick();
    
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 5000) {
        // Send dengan node ID prefix
        comm.send("node1_temperature", readTemperature());
        comm.send("node1_humidity", readHumidity());
        comm.send("node1_status", "online");
        
        lastSend = millis();
    }
}
```

## Platform-Specific Considerations

### ESP32 (Full Features)
```cpp
// Optimal configuration untuk ESP32
comm.begin(Serial1, 115200, FRAME_256B);
comm.setTimeout(2000);
comm.setMode(FULL_DUPLEX);  // Bisa handle simultaneous communication
```

### ESP8266 (Memory Optimized)
```cpp
// Optimized untuk ESP8266
comm.begin(Serial, 115200, FRAME_128B);
comm.setTimeout(3000);      // Longer timeout untuk processing
comm.setRetries(3);         // Moderate retry count
```

### Arduino AVR (Resource Constrained)
```cpp
// Minimal configuration untuk Arduino Uno/Mega
comm.begin(Serial1, 9600, FRAME_64B);   // Lower baud rate
comm.setTimeout(5000);                   // Longer timeout
comm.setMode(HALF_DUPLEX);              // Simpler collision handling
comm.setRetries(2);                     // Minimal retries
```

## Troubleshooting Guide

### 1. Connection Issues
**Problem**: `isConnected()` returns false
**Solutions**:
- Check physical wiring (TX/RX crossed)
- Verify baud rate matching
- Check power supply stability
- Monitor serial activity with oscilloscope

### 2. Frame Corruption
**Problem**: Frequent CRC errors
**Solutions**:
- Check for EMI interference
- Verify ground connection
- Reduce baud rate
- Add ferrite cores on cables
- Check cable quality and length

### 3. Performance Issues
**Problem**: Slow communication or timeouts
**Solutions**:
- Increase buffer size if available memory allows
- Optimize tick() calling frequency
- Reduce data sending frequency
- Use reliable=false untuk non-critical data

### 4. Memory Issues
**Problem**: Buffer allocation failures
**Solutions**:
- Use smaller frame sizes (FRAME_64B atau FRAME_128B)
- Check available heap memory
- Implement periodic reset() calls
- Monitor memory fragmentation

## Best Practices

### 1. Initialization
```cpp
void setup() {
    // Always check initialization result
    if (!comm.begin(Serial1, 115200)) {
        Serial.println("FATAL: CeriaSerial initialization failed");
        while(1) delay(1000);  // Stop execution
    }
    
    // Setup callbacks before sending data
    comm.onReceive(handleData);
    comm.onError(handleError);
}
```

### 2. Main Loop
```cpp
void loop() {
    // Always call tick() first untuk process incoming data
    comm.tick();
    
    // Implement rate limiting untuk sending
    static unsigned long lastSend = 0;
    if (millis() - lastSend > SEND_INTERVAL) {
        sendData();
        lastSend = millis();
    }
    
    // Monitor connection status
    if (!comm.isConnected()) {
        handleDisconnection();
    }
}
```

### 3. Data Management
```cpp
// Use meaningful keys
comm.send("sensor_temperature", temp);  // Good
comm.send("t", temp);                   // Bad

// Keep JSON compact
String config = "{\"m\":1,\"i\":1000}";  // Good
String config = "{ \"mode\" : 1 , \"interval\" : 1000 }";  // Wasteful

// Use appropriate data types
comm.send("counter", 42);        // int
comm.send("voltage", 3.3f);      // float  
comm.send("status", "online");   // string
```

### 4. Error Handling
```cpp
void handleCommError(CeriaSerialError error) {
    // Log all errors untuk debugging
    logError("CeriaSerial", error, millis());
    
    // Implement exponential backoff untuk retries
    static int errorCount = 0;
    static unsigned long lastError = 0;
    
    if (millis() - lastError < 1000) {
        errorCount++;
    } else {
        errorCount = 0;
    }
    
    if (errorCount > 5) {
        // Too many errors, implement recovery
        comm.reset();
        delay(1000 * errorCount);  // Exponential backoff
    }
    
    lastError = millis();
}
```

## Technical Specifications

### Frame Protocol Structure
```
Frame Format: [STX][Length][Flags][Payload][CRC16][ETX]
STX: 0x7E (Start of frame)
ETX: 0x7F (End of frame)
ESC: 0x7D (Escape character)
XOR: 0x20 (XOR mask untuk escaped bytes)

Flags Format (8 bits):
Bit 7-5: Command (3 bits)
  000 = DATA
  001 = ACK  
  010 = PING
  011 = HEARTBEAT
Bit 4-2: Data Type (3 bits)  
  000 = STRING
  001 = INT
  010 = FLOAT
  011 = JSON
  100 = BINARY
Bit 1-0: Sequence Number (2 bits, 0-3)
```

### CRC16 Implementation
Menggunakan polynomial 0xA001 (reversed 0x8005) dengan initial value 0xFFFF:
```cpp
uint16_t calculateCRC16(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}
```

### Memory Management Details
```cpp
// Buffer allocation strategy
if (frameSize <= 128) {
    // Use static buffers untuk efficiency
    static uint8_t staticTxBuffer[128];
    static uint8_t staticRxBuffer[128];
} else {
    // Dynamic allocation untuk larger frames
    _txBuffer = new uint8_t[bufferSize];
    _rxBuffer = new uint8_t[bufferSize];
}
```

### Platform Optimization Settings
```cpp
#if defined(ESP32)
    #define FRAME_SIZE        FRAME_256B
    #define TIMEOUT_MS        2000
    #define MAX_RETRIES       5
#elif defined(ESP8266)  
    #define FRAME_SIZE        FRAME_128B
    #define TIMEOUT_MS        3000
    #define MAX_RETRIES       3
#else  // Arduino AVR
    #define FRAME_SIZE        FRAME_64B
    #define TIMEOUT_MS        5000
    #define MAX_RETRIES       2
#endif
```

## API Reference

### Constructor & Initialization
```cpp
CeriaSerial();                    // Default constructor
~CeriaSerial();                   // Destructor dengan cleanup

bool begin(Stream &serial, 
          uint32_t baud = 115200, 
          FrameSize size = FRAME_AUTO);
```

### Configuration Methods
```cpp
void setMode(DuplexMode mode);          // FULL_DUPLEX atau HALF_DUPLEX
void setRetries(uint8_t count);         // Retry count untuk reliable sending
void setTimeout(uint32_t ms);           // Timeout untuk connection monitoring
```

### Callback Setup
```cpp
void onReceive(CeriaSerialReceiveCallback callback);
void onError(CeriaSerialErrorCallback callback);

// Callback function prototypes
typedef void (*CeriaSerialReceiveCallback)(const String &key, const String &value);
typedef void (*CeriaSerialErrorCallback)(CeriaSerialError error);
```

### Data Transmission
```cpp
// Template method (auto-type detection)
template<typename T>
bool send(const String &key, T value, bool reliable = false);

// Specific type methods
bool send(const String &key, int value, bool reliable = false);
bool send(const String &key, float value, bool reliable = false);
bool send(const String &key, const String &value, bool reliable = false);
```

### Status & Monitoring
```cpp
void tick();                            // Process incoming data (call dalam loop)
bool isConnected();                     // Check connection status
void ping();                            // Manual ping untuk connection test
void getStats(uint32_t &sent, uint32_t &received, uint32_t &errors);
void reset();                           // Reset statistics dan buffers
```

### Error Codes Reference
```cpp
enum CeriaSerialError {
    CERIA_SERIAL_ERR_NONE = 0,          // No error
    CERIA_SERIAL_ERR_TIMEOUT,           // Communication timeout
    CERIA_SERIAL_ERR_CRC_FAILED,        // Frame corruption detected
    CERIA_SERIAL_ERR_BUFFER_FULL,       // Buffer overflow
    CERIA_SERIAL_ERR_FRAME_TOO_LARGE,   // Frame exceeds buffer size
    CERIA_SERIAL_ERR_NO_CONNECTION,     // No connection established
    CERIA_SERIAL_ERR_COLLISION          // Collision dalam half-duplex mode
};
```

## Development Guidelines

### Namespace Convention
```cpp
namespace CeriaDevOP {
    // All Ceria components menggunakan namespace ini
}
```

### Integration dengan Kinematrix
CeriaSerial dapat diintegrasikan dengan Kinematrix framework:
1. Add enable definition di `lib/enable.h`
2. Include di orchestration headers
3. Create examples di project examples directory

### Testing Recommendations
```cpp
void testCeriaSerial() {
    CeriaSerial comm;
    
    // Test initialization
    assert(comm.begin(Serial1, 115200));
    
    // Test basic sending
    assert(comm.send("test", 123));
    
    // Test connection
    assert(comm.isConnected());
    
    // Test statistics
    uint32_t sent, received, errors;
    comm.getStats(sent, received, errors);
    assert(sent > 0);
    
    Serial.println("All tests passed!");
}
```

### Debugging Tools
```cpp
void enableDebugMode() {
    comm.onReceive([](const String& key, const String& value) {
        Serial.println("[RX] " + key + " = " + value);
    });
    
    comm.onError([](CeriaSerialError error) {
        Serial.println("[ERR] Code: " + String(error));
    });
}

void printDetailedStats() {
    uint32_t sent, received, errors;
    comm.getStats(sent, received, errors);
    
    Serial.println("=== CeriaSerial Debug Info ===");
    Serial.println("Connected: " + String(comm.isConnected() ? "YES" : "NO"));
    Serial.println("Frames sent: " + String(sent));
    Serial.println("Frames received: " + String(received));  
    Serial.println("Total errors: " + String(errors));
    Serial.println("Success rate: " + String((sent > 0) ? (100.0 * (sent - errors) / sent) : 100.0) + "%");
    Serial.println("Free heap: " + String(ESP.getFreeHeap()) + " bytes");
}
```

CeriaSerial protocol menyediakan foundation solid untuk reliable inter-microcontroller communication dengan enterprise-grade features dan comprehensive error handling.