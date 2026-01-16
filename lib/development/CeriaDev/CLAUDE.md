# CLAUDE.md - CeriaDev Development

## Directory Overview
CeriaDev adalah workspace pengembangan untuk komponen dan protokol Ceria, sistem komunikasi inter-mikrokontroler yang canggih dalam ekosistem Kinematrix Beta.

## Struktur Directory
```
CeriaDev/
├── CeriaDevOP.h              # Master header untuk semua komponen CeriaDev
├── lib/                      # Library implementations
│   ├── CeriaSerial.h/.cpp           # Protokol komunikasi serial enterprise-grade
│   ├── CeriaDisplayDWIN.h/.cpp      # DWIN HMI display controller dengan touch support
│   ├── CeriaSensorGPS.h/.cpp        # TinyGPSPlus wrapper dengan API sederhana
│   ├── CeriaSensorINA219.h/.cpp     # Adafruit INA219 power monitoring wrapper
│   ├── CeriaModulePCF8574.h/.cpp    # PCF8574 I2C expander dengan encoder support
│   ├── CeriaModuleIO.h/.cpp         # Advanced I/O module dengan button/LED management
│   ├── CeriaSensorAnalog.h/.cpp     # Enhanced analog sensor dengan calibration
│   ├── CeriaSensorDS3231.h/.cpp     # DS3231 RTC wrapper dengan alarm support
│   ├── CeriaModuleESP32NTP.h/.cpp   # ESP32 NTP time sync dengan timezone support
│   └── [ComponentName].md           # Individual component documentation
├── docs/                     # Comprehensive documentation
│   ├── WRAPPER_CREATION_GUIDE.md   # Complete wrapper development guide
│   ├── DWIN_HMI_COMPLETE_KNOWLEDGE.md # Advanced DWIN HMI documentation
│   └── UPGRADE_PLAN.md              # Development roadmap
├── Node*/                    # Example implementations
│   ├── NodeArduinoMega/      # Arduino Mega example node
│   ├── NodeESP32/            # ESP32 example node
│   └── NodeGatewayESP32/     # ESP32 gateway node
├── test_compilation.ino      # Compilation test file
└── main.ino                  # Main entry point
```

## Komponen CeriaDev Lengkap

### Master Header Integration
**File**: `CeriaDevOP.h`  
**Function**: Central header yang mengintegrasikan semua komponen CeriaDev

```cpp
#include "CeriaDevOP.h"  // Single include untuk semua komponen
using namespace CeriaDevOP;
```

Master header ini menyediakan akses ke **10 komponen utama**:
1. **CeriaSerial** - Enterprise-grade serial communication protocol
2. **CeriaDisplayDWIN** - DWIN HMI display controller dengan touch interface
3. **CeriaSensorGPS** - TinyGPSPlus wrapper untuk GPS functionality
4. **CeriaSensorINA219** - Adafruit INA219 power monitoring wrapper
5. **CeriaModulePCF8574** - PCF8574 I2C GPIO expander dengan encoder support
6. **CeriaModuleIO** - Advanced I/O management untuk buttons dan LEDs
7. **CeriaSensorAnalog** - Enhanced analog sensor dengan calibration system
8. **CeriaSensorDS3231** - DS3231 RTC wrapper dengan alarm dan timezone
9. **CeriaModuleESP32NTP** - ESP32 NTP time synchronization dengan multi-server
10. **CeriaModuleESP32SDCard** - Comprehensive SD card management dengan SPI/SDMMC support

## Komponen Utama

### 1. CeriaSerial Protocol
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaSerial.h`, `CeriaSerial.cpp`

Protokol komunikasi serial canggih dengan fitur enterprise-grade untuk komunikasi antar mikrokontroler.

#### Fitur Utama
- **Frame Protocol**: STX/ETX framing dengan byte stuffing untuk integritas data
- **Error Detection**: CRC16 checksum untuk validasi data
- **Dual Communication Modes**: 
  - Full-duplex (komunikasi simultan dua arah)
  - Half-duplex (komunikasi bergantian dengan deteksi kolisi)
- **Platform Optimization**: Buffer adaptif berdasarkan platform
  - ESP32: 256 bytes
  - ESP8266: 128 bytes
  - Arduino AVR: 64 bytes
- **Type-Safe Data**: Template-based sending dengan auto-detection tipe data
- **Collision Detection**: Mekanisme retry cerdas untuk mode half-duplex
- **Health Monitoring**: Heartbeat otomatis dan status koneksi
- **Statistics**: Tracking frame sent/received, CRC errors, dan collisions

#### Arsitektur Frame Protocol
```
Frame Structure:
[STX][Length][Flags][Payload][CRC16][ETX]

Flags Format:
Bit 7-5: Command (DATA=0, ACK=1, PING=2, HEARTBEAT=3)
Bit 4-2: Data Type (STRING=0, INT=1, FLOAT=2, JSON=3, BINARY=4)
Bit 1-0: Sequence Number (0-3)
```

#### Data Types Support
- **CERIA_SERIAL_TYPE_STRING**: String biasa
- **CERIA_SERIAL_TYPE_INT**: Integer (semua ukuran)
- **CERIA_SERIAL_TYPE_FLOAT**: Float/double dengan 6 digit presisi
- **CERIA_SERIAL_TYPE_JSON**: JSON auto-detection berdasarkan {} atau []
- **CERIA_SERIAL_TYPE_BINARY**: Data binary (reserved)

#### Error Handling System
```cpp
enum CeriaSerialError {
    CERIA_SERIAL_ERR_NONE = 0,
    CERIA_SERIAL_ERR_TIMEOUT,
    CERIA_SERIAL_ERR_CRC_FAILED,
    CERIA_SERIAL_ERR_BUFFER_FULL,
    CERIA_SERIAL_ERR_FRAME_TOO_LARGE,
    CERIA_SERIAL_ERR_NO_CONNECTION,
    CERIA_SERIAL_ERR_COLLISION
};
```

#### API Usage Pattern
```cpp
#include "CeriaSerial.h"
using namespace CeriaDevOP;

CeriaSerial comm;

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    
    // Initialize CeriaSerial
    comm.begin(Serial1, 115200, FRAME_AUTO);
    comm.setMode(FULL_DUPLEX);
    comm.setRetries(3);
    comm.setTimeout(2000);
    
    // Setup callbacks
    comm.onReceive([](const String& key, const String& value) {
        Serial.println("Received: " + key + " = " + value);
    });
    
    comm.onError([](CeriaSerialError error) {
        Serial.println("Error: " + String(error));
    });
}

void loop() {
    comm.tick();  // Process incoming data
    
    // Send different data types
    comm.send("temperature", 25.6f);
    comm.send("humidity", 60);
    comm.send("status", "online");
    comm.send("config", "{\"mode\":1,\"interval\":1000}");
    
    delay(1000);
}
```

#### Template Function Support
Class mendukung template function untuk type-safe sending:
```cpp
template<typename T>
bool send(const String& key, T value, bool reliable = false);
```

Dengan overload untuk semua tipe data standar:
- int, long, uint8_t, uint16_t, uint32_t
- float, double
- const char*, String
- Auto-detection untuk JSON strings

#### Memory Management
- **Static Buffers**: Untuk frame ≤128B menggunakan static array
- **Dynamic Allocation**: Untuk frame >128B menggunakan heap allocation
- **Auto Cleanup**: Destructor otomatis membersihkan memory
- **Buffer Overflow Protection**: Bounds checking untuk mencegah corruption

#### Communication Features
- **Sequence Numbering**: 2-bit sequence number untuk tracking frame order
- **Heartbeat System**: Ping/pong otomatis untuk monitoring koneksi
- **Retry Mechanism**: Configurable retry count untuk reliable transmission
- **Collision Avoidance**: Exponential backoff untuk half-duplex mode
- **Channel Clear Detection**: Monitoring aktivitas channel untuk half-duplex

#### Platform-Specific Optimizations
```cpp
#if defined(ESP32)
    #define CERIA_SERIAL_DEFAULT_FRAME    FRAME_256B
    #define CERIA_SERIAL_DEFAULT_TIMEOUT  2000
#elif defined(ESP8266)
    #define CERIA_SERIAL_DEFAULT_FRAME    FRAME_128B
    #define CERIA_SERIAL_DEFAULT_TIMEOUT  3000
#else
    #define CERIA_SERIAL_DEFAULT_FRAME    FRAME_64B
    #define CERIA_SERIAL_DEFAULT_TIMEOUT  5000
#endif
```

#### Statistics dan Monitoring
```cpp
uint32_t sent, received, errors;
comm.getStats(sent, received, errors);

// Monitoring connection health
bool connected = comm.isConnected();
comm.ping();  // Manual ping
```

### 2. CeriaSensorGPS
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaSensorGPS.h`, `CeriaSensorGPS.cpp`

Wrapper untuk TinyGPSPlus library yang menyediakan API sederhana untuk GPS sensor dengan fitur lengkap.

#### Fitur Utama
- **Inheritance dari TinyGPSPlus**: Akses penuh ke semua fitur TinyGPSPlus
- **Simplified API**: Getter methods sederhana untuk data GPS umum
- **Connection Monitoring**: Deteksi koneksi GPS dan status fix
- **Data Validation**: Built-in validation untuk location, time, dan date
- **Distance calculation**: Method untuk menghitung jarak ke koordinat tertentu
- **Wait for Fix**: Blocking method untuk menunggu GPS fix dengan timeout

#### API Usage Pattern
```cpp
#include "CeriaSensorGPS.h"
using namespace CeriaDevOP;

CeriaSensorGPS gps;

void setup() {
    Serial.begin(115200);
    gps.begin(Serial1, 9600);  // GPS module pada Serial1
}

void loop() {
    gps.update();
    
    if (gps.hasNewData() && gps.hasFix()) {
        Serial.println("Location: " + gps.getLocation());
        Serial.println("Satellites: " + String(gps.getSatellites()));
        Serial.println("Speed: " + String(gps.getSpeed()) + " km/h");
        Serial.println("Altitude: " + String(gps.getAltitude()) + " m");
        Serial.println("Date: " + gps.getDate());
        Serial.println("Time: " + gps.getTime());
        
        // Calculate distance to home
        double distanceToHome = gps.distanceTo(-6.200000, 106.816666);
        Serial.println("Distance to home: " + String(distanceToHome/1000.0, 2) + " km");
    }
    
    delay(1000);
}
```

#### Key Methods
```cpp
void begin(Stream &serial, unsigned long baud = 9600);
void update();
bool hasNewData();
bool isConnected(uint32_t timeoutMs = 5000);
bool hasFix();

double getLatitude();
double getLongitude();
bool isLocationValid();
uint8_t getSatellites();
double getSpeed();        // km/h
double getAltitude();     // meters
String getDate();         // DD/MM/YYYY
String getTime();         // HH:MM:SS
String getLocation();     // "lat,lng"
double distanceTo(double lat, double lng);
bool waitForFix(uint32_t timeoutMs = 30000);
```

### 3. CeriaSensorINA219
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaSensorINA219.h`, `CeriaSensorINA219.cpp`

Wrapper untuk Adafruit INA219 library yang menyediakan power monitoring dengan API yang disederhanakan.

#### Fitur Utama
- **Inheritance dari Adafruit_INA219**: Akses penuh ke semua fitur INA219
- **Multiple Calibration Modes**: 32V/2A, 32V/1A, 16V/400mA
- **Power Save Mode**: Dukungan untuk mode hemat daya
- **Connection Monitoring**: Deteksi status koneksi sensor
- **String Output**: Format output yang mudah dibaca

#### API Usage Pattern
```cpp
#include "CeriaSensorINA219.h"
using namespace CeriaDevOP;

CeriaSensorINA219 power(0x40);  // I2C address 0x40

void setup() {
    Serial.begin(115200);
    Wire.begin();
    power.begin(&Wire, INA219_CAL_32V_2A);
}

void loop() {
    power.update();
    
    if (power.hasNewData()) {
        Serial.println("Voltage: " + String(power.getVoltage(), 2) + "V");
        Serial.println("Current: " + String(power.getCurrent(), 1) + "mA");
        Serial.println("Power: " + String(power.getPower(), 1) + "mW");
        Serial.println("Summary: " + power.getPowerString());
    }
    
    delay(1000);
}
```

#### Key Methods
```cpp
CeriaSensorINA219(uint8_t addr = 0x40);
void begin(TwoWire *wire = &Wire, INA219CalibrationMode mode = INA219_CAL_32V_2A);
void update();
bool hasNewData();
bool isConnected(uint32_t timeoutMs = 5000);

float getVoltage();       // Volts
float getCurrent();       // mA
float getPower();         // mW
void setCalibrationMode(INA219CalibrationMode mode);
void setPowerSave(bool enabled);
String getPowerString();  // "12.5V, 250mA, 3.1W"
bool waitForReading(uint32_t timeoutMs = 5000);
```

#### Calibration Modes
```cpp
enum INA219CalibrationMode {
    INA219_CAL_32V_2A = 0,    // 32V, ±2A range
    INA219_CAL_32V_1A = 1,    // 32V, ±1A range  
    INA219_CAL_16V_400MA = 2  // 16V, ±400mA range
};
```

### 4. CeriaDisplayDWIN
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaDisplayDWIN.h`, `CeriaDisplayDWIN.cpp`

Advanced DWIN HMI display controller dengan enterprise-grade features untuk human-machine interface applications.

#### Fitur Utama
- **DWIN Protocol Implementation**: Native support untuk DWIN display communication protocol
- **Touch Interface**: Multi-touch event handling dengan press/release/long press/drag detection
- **Variable Pointer Management**: Up to 32 registered VP addresses untuk real-time data exchange
- **Page Management**: Automatic page change detection dan callback system
- **Display Control**: Brightness, volume, dan system register management
- **Frame Validation**: CRC16 checksum untuk data integrity
- **Auto Upload**: Configurable automatic variable upload dari display
- **Type-Safe Operations**: Support untuk uint8/16/32, float, dan string data types

#### API Usage Pattern
```cpp
#include "CeriaDisplayDWIN.h"
using namespace CeriaDevOP;

CeriaDisplayDWIN display;

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);  // DWIN connection
    
    display.begin(Serial2, 115200);
    
    // Setup callbacks
    display.setTouchCallback([](uint16_t vp, uint16_t value, DWINTouchEvent event) {
        Serial.println("Touch at VP: " + String(vp, HEX) + " Value: " + String(value));
    });
    
    display.setPageCallback([](uint8_t newPage, uint8_t oldPage) {
        Serial.println("Page changed from " + String(oldPage) + " to " + String(newPage));
    });
    
    // Register variables untuk monitoring
    display.registerVP(0x1000, DWIN_TYPE_UINT16, true);  // Auto-upload enabled
    display.registerVP(0x1001, DWIN_TYPE_STRING, false);
}

void loop() {
    display.update();
    
    // Write data to display
    display.writeVP(0x1000, 12345);
    display.setText(0x1001, "Hello DWIN!");
    display.setProgress(0x1002, 75);
    
    // Read touch events
    if (display.hasNewTouch()) {
        DWINTouchData touch = display.getLastTouch();
        Serial.println("Touch VP: " + String(touch.vp, HEX));
    }
    
    delay(100);
}
```

#### Key Methods
```cpp
// Initialization
bool begin(Stream &serial, uint32_t baud = 115200);
void update();

// Display control
bool setPage(uint8_t page);
bool setBrightness(uint8_t level);
bool setVolume(uint8_t level);

// Variable pointer operations
bool writeVP(uint16_t vp, [various types]);
bool readVP(uint16_t vp, DWINDataType type);
bool registerVP(uint16_t vp, DWINDataType type, bool autoUpload = false);

// Touch handling
void setTouchCallback(DWINTouchCallback callback);
DWINTouchData getLastTouch();

// Convenience methods
bool setText(uint16_t vp, const String &text);
bool setNumber(uint16_t vp, int32_t number);
bool setProgress(uint16_t vp, uint8_t percentage);
bool setIcon(uint16_t vp, uint16_t iconId);
```

### 5. CeriaModulePCF8574
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaModulePCF8574.h`, `CeriaModulePCF8574.cpp`

Enhanced PCF8574 I2C GPIO expander wrapper dengan advanced features untuk digital I/O expansion.

#### Fitur Utama
- **Inheritance dari PCF8574**: Akses penuh ke semua fitur base library
- **Rotary Encoder Support**: Built-in encoder reading dengan multiple algorithms
- **Interrupt Support**: Hardware interrupt handling untuk responsive I/O
- **Pin Mode Management**: Individual pin configuration dengan pullup support
- **Batch Operations**: Read/write semua pins dalam satu operasi
- **Connection Monitoring**: Real-time connection status checking
- **Platform Compatibility**: Support untuk ESP32, ESP8266, dan Arduino platforms

#### API Usage Pattern
```cpp
#include "CeriaModulePCF8574.h"
using namespace CeriaDevOP;

CeriaModulePCF8574 expander(0x38);  // I2C address 0x38

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    expander.begin();
    
    // Setup pins
    expander.setPinMode(0, PCF8574_PIN_OUTPUT);    // LED
    expander.setPinMode(1, PCF8574_PIN_OUTPUT);    // LED
    expander.setPinMode(2, PCF8574_PIN_INPUT_PULLUP);  // Button
    
    // Setup rotary encoder pada pins 4&5
    expander.setupEncoder(4, 5);
}

void loop() {
    expander.update();
    
    if (expander.hasNewData()) {
        // Read button
        if (expander.readPin(2) == LOW) {
            Serial.println("Button pressed!");
        }
        
        // Control LEDs
        expander.writePin(0, HIGH);
        expander.writePin(1, LOW);
        
        // Read encoder
        volatile long encoderValue = 0;
        if (expander.readEncoder(4, 5, &encoderValue)) {
            Serial.println("Encoder: " + String(encoderValue));
        }
    }
    
    delay(50);
}
```

### 6. CeriaModuleIO
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaModuleIO.h`, `CeriaModuleIO.cpp`

Advanced I/O management system yang menggabungkan PCF8574 expansion dengan sophisticated button/LED control.

#### Fitur Utama
- **Inheritance dari CeriaModulePCF8574**: Semua fitur GPIO expander
- **Button Management**: Debounced button handling dengan event detection
- **LED Control**: Advanced LED control dengan blinking patterns
- **Digital I/O Classes**: Dedicated CeriaDigitalInput dan CeriaDigitalOutput classes
- **Count Functionality**: Button press counting dengan configurable modes
- **Async Operations**: Non-blocking toggle dan blink operations
- **Batch Updates**: Efficient batch updating semua I/O components

#### API Usage Pattern
```cpp
#include "CeriaModuleIO.h"
using namespace CeriaDevOP;

CeriaModuleIO io(0x38);

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    io.begin();
    
    // Setup buttons dan LEDs
    io.setupButton(0, 50);    // Button pada pin 0, debounce 50ms
    io.setupButton(1, 30);    // Button pada pin 1, debounce 30ms
    io.setupLED(4, false);    // LED pada pin 4, initial OFF
    io.setupLED(5, true);     // LED pada pin 5, initial ON
    
    // Enable automatic updates
    io.enableAutoUpdate(true);
    io.setBatchUpdateInterval(20);  // 20ms update interval
}

void loop() {
    io.updateAll();  // Update semua components
    
    // Button handling
    if (io.buttonPressed(0)) {
        Serial.println("Button 0 pressed!");
        io.ledToggle(4);  // Toggle LED
    }
    
    if (io.buttonLongPressed(1, 1000)) {
        Serial.println("Button 1 long pressed!");
        io.ledBlinkPattern(5, 200, 5);  // Blink 5 times, 200ms each
    }
    
    // LED operations
    io.ledBlink(4, 500);  // Blink every 500ms
    
    delay(10);
}
```

### 7. CeriaSensorAnalog
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaSensorAnalog.h`, `CeriaSensorAnalog.cpp`

Enhanced analog sensor wrapper dengan advanced calibration dan statistics tracking.

#### Fitur Utama
- **Multi-Platform Support**: Auto-detection untuk Arduino UNO, Mega, Due, ESP32, ESP8266
- **Flexible Resolution**: 8-bit hingga 16-bit ADC resolution support
- **Calibration System**: Zero-point dan span calibration dengan offset/multiplier
- **Multiple Sampling**: Configurable sampling untuk noise reduction
- **Statistics Tracking**: Running average, min/max values, total readings
- **Reference Voltage**: Configurable reference voltage untuk accurate measurements
- **Range Mapping**: Custom range mapping untuk sensor scaling

#### API Usage Pattern
```cpp
#include "CeriaSensorAnalog.h"
using namespace CeriaDevOP;

CeriaSensorAnalog sensor(A0);

void setup() {
    Serial.begin(115200);
    
    sensor.begin(3.3);  // 3.3V reference
    sensor.setResolution(ANALOG_12BIT);
    sensor.setSampling(10);  // 10 samples average
    sensor.setRange(0, 4095);  // 12-bit range
    sensor.setUpdateInterval(100);  // Update setiap 100ms
    
    // Enable calibration
    sensor.enableCalibration(0.05, 0.98);  // Offset dan multiplier
}

void loop() {
    sensor.update();
    
    if (sensor.hasNewData()) {
        Serial.println("Raw: " + String(sensor.getRaw()));
        Serial.println("Voltage: " + String(sensor.getVoltage(), 3) + "V");
        Serial.println("Percentage: " + String(sensor.getPercentage(), 1) + "%");
        Serial.println("Statistics: " + sensor.getStatisticsString());
    }
    
    delay(50);
}
```

### 8. CeriaSensorDS3231
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaSensorDS3231.h`, `CeriaSensorDS3231.cpp`

Comprehensive DS3231 RTC wrapper dengan advanced time management dan alarm functionality.

#### Fitur Utama
- **Inheritance dari RTC_DS3231**: Akses penuh ke Adafruit RTClib features
- **Comprehensive Time Data**: Structured time information dengan multiple formats
- **Alarm Management**: Dual alarm support dengan flexible scheduling
- **Temperature Reading**: Built-in temperature sensor access
- **Power Loss Detection**: Automatic detection untuk power interruption
- **Multiple String Formats**: Date/time strings dalam berbagai format
- **Timezone Aware**: Support untuk timezone calculations
- **SQW Output**: Square wave output configuration

#### API Usage Pattern
```cpp
#include "CeriaSensorDS3231.h"
using namespace CeriaDevOP;

CeriaSensorDS3231 rtc;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    rtc.begin(&Wire);
    
    if (rtc.isPowerLost()) {
        Serial.println("RTC lost power, setting time...");
        rtc.setToCompileTime();  // Set ke compile time
    }
    
    // Set alarm (wake at 07:30:00)
    rtc.setAlarm1(7, 30, 0, DS3231_MATCH_H_M_S);
    
    rtc.setUpdateInterval(1000);  // Update setiap detik
}

void loop() {
    rtc.update();
    
    if (rtc.hasNewData()) {
        DS3231TimeData time = rtc.getTime();
        
        Serial.println("Date: " + time.dateString);
        Serial.println("Time: " + time.timeString);
        Serial.println("Temperature: " + String(rtc.getTemperature(), 2) + "°C");
        
        if (rtc.isAlarmTriggered()) {
            Serial.println("Alarm triggered!");
            rtc.clearAlarm();
        }
    }
    
    delay(100);
}
```

### 9. CeriaModuleESP32NTP
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaModuleESP32NTP.h`, `CeriaModuleESP32NTP.cpp`

Sophisticated ESP32 NTP time synchronization module dengan multi-server support dan timezone management.

#### Fitur Utama
- **ESP32 Core Compatibility**: Support untuk Arduino Core v2.0+ dan v3.0+
- **Multi-Server NTP**: Primary/secondary/tertiary NTP server configuration
- **Timezone Support**: 24+ common timezone presets dengan DST handling
- **Sync Modes**: On-demand, periodic, dan aggressive sync strategies
- **Connection Monitoring**: WiFi status monitoring dan automatic reconnection
- **Statistics Tracking**: Sync success/failure counts dan timing
- **Callback System**: Event callbacks untuk sync completion dan time updates
- **Flexible Configuration**: Comprehensive configuration structure

#### API Usage Pattern
```cpp
#include "CeriaModuleESP32NTP.h"
using namespace CeriaDevOP;

CeriaModuleESP32NTP ntp;

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    
    // Wait for WiFi connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    // Initialize NTP
    ntp.begin("WIB-7", "pool.ntp.org");  // Indonesia timezone
    ntp.setSyncInterval(3600000);  // Sync setiap jam
    ntp.enableAutoSync(true);
    
    // Setup callbacks
    ntp.onSyncComplete([](bool success) {
        Serial.println(success ? "NTP Sync OK" : "NTP Sync Failed");
    });
    
    ntp.onTimeUpdate([](const NTPTimeInfo &timeInfo) {
        Serial.println("Time updated: " + timeInfo.dateTimeString);
    });
}

void loop() {
    ntp.update();
    
    if (ntp.isTimeValid()) {
        NTPTimeInfo time = ntp.getTime();
        
        Serial.println("Current time: " + time.dateTimeString);
        Serial.println("Timezone: " + time.timezoneString);
        Serial.println("Uptime: " + String(ntp.getUptime()) + "s");
        
        NTPStatus status = ntp.getStatus();
        Serial.println("Sync count: " + String(status.syncCount));
    }
    
    delay(5000);
}

### 10. CeriaModuleESP32SDCard
**Namespace**: `CeriaDevOP`  
**Files**: `CeriaModuleESP32SDCard.h`, `CeriaModuleESP32SDCard.cpp`

Comprehensive ESP32 SD card management module dengan unified API untuk SPI dan SDMMC interfaces.

#### Fitur Utama
- **Multi-Interface Support**: SPI dan SDMMC (1-bit/4-bit) dengan auto-selection
- **Advanced File Operations**: Create, read, write, delete, rename, copy, move dengan error handling
- **Directory Management**: Recursive directory operations dan working directory support
- **Specialized Formats**: CSV, JSON, dan log file writing dengan timestamp
- **Performance Monitoring**: Built-in benchmarking dan speed testing
- **Data Management**: Backup/restore, archiving, cleanup tools
- **Statistics Tracking**: Comprehensive operation statistics dan performance metrics
- **Path Validation**: Complete path dan filename validation dengan sanitization

#### Interface Comparison
- **SPI Interface**: ~4MB/s, flexible pin assignment, compatible dengan semua ESP32
- **SDMMC 1-bit**: ~12MB/s, minimal pins (CLK, CMD, D0), faster than SPI
- **SDMMC 4-bit**: ~25MB/s, maximum performance, requires 6 pins

#### API Usage Pattern
```cpp
#include "CeriaModuleESP32SDCard.h"
using namespace CeriaDevOP;

CeriaModuleESP32SDCard sdcard;

void setup() {
    Serial.begin(115200);
    
    // Initialize dengan SDMMC 4-bit untuk performance maksimal
    if (!sdcard.beginSDMMC4Bit()) {
        Serial.println("SDMMC failed, trying SPI...");
        sdcard.beginSPI(18, 19, 23, 5, 4000000);
    }
    
    // Setup error handling
    sdcard.setErrorCallback([](SDCardError error, const String &operation) {
        Serial.println("SD Error: " + sdcard.getErrorString(error) + " in " + operation);
    });
    
    // Create directory structure
    sdcard.ensureDirectory("/data/sensors");
    sdcard.ensureDirectory("/logs");
    sdcard.setWorkingDirectory("/data");
    
    Serial.println("SD Card ready: " + sdcard.getStatusString());
}

void loop() {
    sdcard.update();
    
    // Log sensor data
    float temp = 25.6, humidity = 60.2;
    String csvData = String(millis()) + "," + String(temp) + "," + String(humidity);
    
    if (sdcard.writeCSV("sensors/environmental.csv", csvData, true)) {
        Serial.println("Data logged: " + csvData);
    }
    
    // System logging dengan timestamp
    sdcard.writeLog("/logs/system.log", "Temperature: " + String(temp) + "°C", true);
    
    if (sdcard.hasNewData()) {
        Serial.println("Card info: " + sdcard.getInfoString());
        Serial.println("Stats: " + sdcard.getStatsString());
    }
    
    delay(10000);
}
```

#### Key Methods
```cpp
// Multi-interface initialization
bool begin(SDCardInterface interface = SDCARD_SDMMC_1BIT, const String &mountPoint = "/sdcard");
bool beginSPI(int8_t sck = 18, int8_t miso = 19, int8_t mosi = 23, int8_t cs = 5, uint32_t frequency = 4000000);
bool beginSDMMC1Bit(int8_t clk = 14, int8_t cmd = 15, int8_t d0 = 2);
bool beginSDMMC4Bit(int8_t clk = 14, int8_t cmd = 15, int8_t d0 = 2, int8_t d1 = 4, int8_t d2 = 12, int8_t d3 = 13);

// File operations
bool writeFile(const String &path, const String &content);
String readFile(const String &path);
bool deleteFile(const String &path);
bool copyFile(const String &sourcePath, const String &destPath);

// Specialized formats
bool writeCSV(const String &path, const String &data, bool append = true);
bool writeJSON(const String &path, const String &jsonData, bool pretty = false);
bool writeLog(const String &path, const String &message, bool timestamp = true);

// Directory management
bool createDirectory(const String &path);
String listAll(const String &directory = "/", bool recursive = false);
uint32_t countFiles(const String &directory = "/", bool recursive = false);

// Advanced features
bool backupFile(const String &sourcePath, const String &backupDir = "/backup");
String generateUniqueFilename(const String &basePath, const String &extension = ".txt");
bool cleanupOldFiles(const String &directory, uint32_t maxAgeMs, uint32_t maxFiles = 0);

// Performance & monitoring
bool test();
bool benchmark();
SDCardInfo getCardInfo();
SDCardStats getStats();
String formatBytes(uint64_t bytes);
```

#### Hardware Connections
```cpp
// SPI Interface (Default)
SCK  -> GPIO 18
MISO -> GPIO 19
MOSI -> GPIO 23
CS   -> GPIO 5

// SDMMC Interface (ESP32 Fixed)
CLK -> GPIO 14
CMD -> GPIO 15
D0  -> GPIO 2
D1  -> GPIO 4  (4-bit mode)
D2  -> GPIO 12 (4-bit mode)
D3  -> GPIO 13 (4-bit mode)
```

**Important**: SDMMC pins memerlukan external 10kΩ pull-up resistors pada data lines.

#### Error Handling System
```cpp
enum SDCardError {
    SDCARD_ERR_NONE = 0,
    SDCARD_ERR_MOUNT_FAILED,
    SDCARD_ERR_NO_CARD,
    SDCARD_ERR_FILE_NOT_FOUND,
    SDCARD_ERR_WRITE_FAILED,
    SDCARD_ERR_READ_FAILED,
    SDCARD_ERR_INSUFFICIENT_SPACE,
    // ... 15 comprehensive error types
};

// Error callback setup
sdcard.setErrorCallback([](SDCardError error, const String &operation) {
    Serial.println("SD Error: " + sdcard.getErrorString(error));
    // Implement recovery strategies
});
```

### 11. Wrapper Creation Framework
**File**: `WRAPPER_CREATION_GUIDE.md`

Panduan komprehensif untuk membuat sensor wrapper baru menggunakan pola CeriaDev.

#### Design Philosophy
- **Inheritance over Composition**: Extend existing libraries
- **Simple API Surface**: Essential methods only
- **Clean Data Access**: Direct getter methods
- **String Output Formats**: Human-readable outputs
- **Smart Defaults**: Sensible configuration out-of-the-box
- **Complete Documentation**: Include base class reference

#### Wrapper Pattern Template
Setiap wrapper mengikuti pola standar:
- Constructor dengan parameter default
- `begin()` untuk inisialisasi
- `update()` untuk membaca data
- `hasNewData()` untuk deteksi data baru
- `isConnected()` untuk status koneksi
- Getter methods untuk data utama
- `waitForReading()` untuk blocking read
- String output method untuk format readable

## Development Guidelines

### Coding Standards
- Gunakan namespace `CeriaDevOP` untuk semua komponen
- Implementasi error handling yang komprehensif
- Memory management yang efisien dengan static/dynamic allocation
- Platform-specific optimization untuk performa optimal
- Type-safe template functions untuk API yang bersih

### Testing Pattern
- Test pada minimal 2 platform berbeda (ESP32 dan Arduino)
- Validate frame integrity dengan CRC checking
- Test collision handling untuk half-duplex mode
- Monitor memory usage untuk mencegah leaks
- Performance testing untuk throughput maksimal

### Integration dengan Kinematrix
Komponen CeriaDev dapat diintegrasikan dengan framework Kinematrix melalui:
1. Module enable system di `lib/enable.h`
2. Include orchestration di main headers
3. Example integration di project examples

## Future Development

### Planned Features
- **Encryption Layer**: AES encryption untuk secure communication
- **Network Stack**: TCP/IP implementation over CeriaSerial
- **Mesh Networking**: Multi-hop communication protocol
- **OTA Updates**: Over-the-air firmware update capability
- **Advanced Diagnostics**: Real-time protocol analysis tools

### Architecture Expansion
- Multiple communication channels
- Priority queue system
- Flow control mechanisms
- Advanced error recovery
- Protocol versioning support

## Files Detail

### CeriaSerial.h (153 lines)
Header file yang mendefinisikan:
- Enums untuk frame size, duplex mode, error types, data types
- Constants untuk protocol framing (STX, ETX, ESC, XOR)
- CeriaSerial class dengan private/public methods
- Platform-specific default configurations
- Template function declarations
- Callback type definitions

### CeriaSerial.cpp (386 lines)
Implementation file yang berisi:
- Constructor/destructor dengan proper initialization
- Buffer management (static vs dynamic allocation)
- Frame building dan parsing logic
- CRC16 calculation (polynomial 0xA001)
- Byte stuffing/unstuffing untuk frame integrity
- Communication mode handling (full/half duplex)
- Error handling dan statistics tracking
- Type detection dan conversion functions

### CeriaSerial.md (130 lines)
Dokumentasi existing yang sudah ada, berisi overview dan usage examples untuk CeriaSerial protocol.

### CeriaSensorGPS.h (44 lines)
Header file GPS sensor wrapper:
- Class definition inheriting dari TinyGPSPlus
- Public API methods untuk GPS data access
- Private state management variables
- GPS-specific enums dan constants

### CeriaSensorGPS.cpp (125 lines)
Implementation GPS sensor wrapper:
- Constructor/destructor initialization
- Serial port handling untuk berbagai platform
- GPS data parsing dan validation
- String formatting untuk date/time/location
- Distance calculation methods
- Connection monitoring dan timeout handling

### CeriaSensorINA219.h (46 lines)
Header file power sensor wrapper:
- Class definition inheriting dari Adafruit_INA219
- Calibration modes enum definition
- Public API untuk power monitoring
- Private state tracking variables

### CeriaSensorINA219.cpp (89 lines)
Implementation power sensor wrapper:
- Constructor dengan I2C address handling
- Calibration mode switching logic
- Power/voltage/current reading methods
- String formatting untuk power data
- Connection status monitoring
- Power save mode handling

### WRAPPER_CREATION_GUIDE.md (734 lines)
Panduan komprehensif untuk membuat sensor wrapper:
- Design philosophy dan principles
- Template files untuk header dan implementation
- Documentation requirements dan standards
- API design best practices
- Integration patterns dengan CeriaDev
- Testing guidelines dan platform compatibility
- Real-world implementation examples

## Usage Examples

### Basic Communication
```cpp
CeriaSerial link;
link.begin(Serial1, 115200);
link.send("sensor", 25.6f);
```

### Advanced Configuration
```cpp
link.setMode(HALF_DUPLEX);
link.setRetries(5);
link.setTimeout(3000);
```

### Event Handling
```cpp
link.onReceive([](const String& key, const String& value) {
    handleReceivedData(key, value);
});

link.onError([](CeriaSerialError error) {
    handleCommunicationError(error);
});
```

## Technical Specifications

### Communication Performance
- **CeriaSerial**: Up to 115200 baud dengan CRC16 error detection, 64B-512B adaptive buffers
- **CeriaDisplayDWIN**: DWIN protocol implementation dengan 64-byte frame buffers, CRC16 validation
- **I2C Components**: Standard I2C communication untuk PCF8574, DS3231, INA219 sensors
- **NTP Sync**: ESP32 multi-server NTP dengan automatic retry dan timezone support

### Memory Optimization
- **Platform Adaptive**: Buffer sizes optimized per platform (ESP32: 256B, ESP8266: 128B, AVR: 64B)
- **Dynamic Allocation**: Smart memory management untuk large frames
- **Statistics Tracking**: Minimal overhead untuk performance monitoring
- **Inheritance Pattern**: Zero-overhead wrapper implementation

### Real-Time Capabilities
- **Interrupt Support**: Hardware interrupts untuk responsive I/O (PCF8574)
- **Non-blocking Operations**: Async LED control, encoder reading, display updates
- **Callback System**: Event-driven architecture untuk real-time responses
- **Batch Updates**: Efficient batch processing untuk multiple I/O operations

### Platform Support
- **ESP32**: Full feature set termasuk WiFi, NTP, advanced displays
- **ESP8266**: Optimized feature set dengan memory constraints
- **Arduino AVR**: Core functionality untuk Uno/Mega dengan minimal resources
- **Compatibility**: Automatic platform detection dan feature scaling

## Summary

Direktori CeriaDev menyediakan foundation solid untuk advanced embedded communication systems dengan fokus pada reliability, performance, dan ease of use. Komponen utama mencakup:

### Core Components (10 Total Components)
1. **CeriaSerial Protocol** - Enterprise-grade serial communication dengan CRC16, collision detection, dan platform optimization
2. **CeriaDisplayDWIN** - Advanced DWIN HMI display controller dengan touch interface dan variable pointer management
3. **CeriaSensorGPS** - TinyGPSPlus wrapper dengan API sederhana untuk GPS functionality
4. **CeriaSensorINA219** - Adafruit INA219 wrapper untuk power monitoring dengan multiple calibration modes
5. **CeriaModulePCF8574** - Enhanced PCF8574 I2C GPIO expander dengan rotary encoder support
6. **CeriaModuleIO** - Advanced I/O management dengan button/LED control dan digital I/O classes
7. **CeriaSensorAnalog** - Enhanced analog sensor dengan calibration system dan statistics tracking
8. **CeriaSensorDS3231** - Comprehensive DS3231 RTC wrapper dengan alarm dan temperature monitoring
9. **CeriaModuleESP32NTP** - Sophisticated ESP32 NTP time sync dengan multi-server dan timezone support
10. **CeriaModuleESP32SDCard** - Comprehensive SD card management dengan unified SPI/SDMMC interface support

### Architecture Benefits
- **Modular Design**: Setiap komponen dapat digunakan secara terpisah atau kombinasi
- **Inheritance Pattern**: Wrapper mewarisi semua fitur base library sambil menyediakan API sederhana
- **Platform Optimized**: Adaptive buffer management dan configuration berdasarkan target platform
- **Production Ready**: Comprehensive error handling, statistics tracking, dan monitoring
- **Developer Friendly**: Consistent API patterns, extensive documentation, dan real-world examples

### Integration Ready
Semua komponen CeriaDev siap diintegrasikan dengan framework Kinematrix Beta melalui module enable system dan orchestration headers.

## Development Guidelines

### Coding Standards CeriaDev
- **Namespace Usage**: Semua komponen menggunakan `namespace CeriaDevOP`
- **Inheritance Pattern**: Extend existing libraries, tidak wrap dalam composition
- **Consistent API**: Semua komponen mengikuti pattern: `begin()`, `update()`, `hasNewData()`, `isConnected()`
- **Error Handling**: Comprehensive error enums dan callback systems
- **Memory Management**: Platform-adaptive buffer allocation dengan auto-cleanup
- **String Outputs**: Human-readable format strings untuk debugging dan monitoring

### Component Integration Pattern
```cpp
// Single header inclusion untuk semua components
#include "CeriaDevOP.h"
using namespace CeriaDevOP;

// Initialize multiple components
CeriaSerial comm;
CeriaDisplayDWIN display;
CeriaSensorGPS gps;
CeriaModuleIO io(0x38);

void setup() {
    // Standard initialization pattern
    comm.begin(Serial1, 115200);
    display.begin(Serial2, 115200);
    gps.begin(Serial3, 9600);
    io.begin();
    
    // Setup unified callbacks
    setupCallbacks();
}

void loop() {
    // Unified update cycle
    comm.tick();
    display.update();
    gps.update();
    io.updateAll();
    
    // Process data from all components
    processAllData();
}
```

### Example Node Implementations
CeriaDev menyediakan 3 example nodes untuk different use cases:

#### 1. NodeArduinoMega
**Target**: Arduino Mega 2560  
**Components**: CeriaSerial, CeriaModuleIO, CeriaSensorAnalog  
**Use Case**: Basic sensor node dengan I/O expansion

#### 2. NodeESP32
**Target**: ESP32 DevKit  
**Components**: CeriaSerial, CeriaDisplayDWIN, CeriaSensorGPS, CeriaModuleESP32NTP  
**Use Case**: Advanced sensor node dengan display dan GPS tracking

#### 3. NodeGatewayESP32
**Target**: ESP32 (Gateway)  
**Components**: Multiple CeriaSerial instances, WiFi integration  
**Use Case**: Central gateway untuk multi-node communication

### Testing Framework
```cpp
// Comprehensive component testing
void testAllComponents() {
    Serial.println("Testing CeriaDev Components...");
    
    // Test communication
    if (comm.isConnected()) {
        Serial.println("✓ CeriaSerial: Connected");
        comm.send("test", "ok");
    }
    
    // Test display
    if (display.isConnected()) {
        Serial.println("✓ CeriaDisplayDWIN: Connected");
        display.setText(0x1000, "Test OK");
    }
    
    // Test sensors
    if (gps.isConnected() && gps.hasFix()) {
        Serial.println("✓ CeriaSensorGPS: Fixed");
    }
    
    // Test I/O
    if (io.isConnected()) {
        Serial.println("✓ CeriaModuleIO: Online");
        io.ledBlink(0, 200);  // Blink test
    }
}
```

### Advanced Integration Features

#### Multi-Component Data Fusion
```cpp
void processAllData() {
    // Fuse data dari multiple components
    if (gps.hasNewData() && display.isConnected()) {
        // Update display dengan GPS data
        display.setText(0x2000, gps.getLocation());
        display.setFloat(0x2001, gps.getSpeed(), 1);
    }
    
    if (io.hasNewData() && comm.isConnected()) {
        // Send I/O status via serial communication
        comm.send("button_state", String(io.buttonPressed(0)));
        comm.send("led_status", io.getOutputStatusString());
    }
}
```

#### Centralized Error Handling
```cpp
void setupCallbacks() {
    // Unified error handling
    comm.onError([](CeriaSerialError error) {
        handleComponentError("CeriaSerial", String(error));
    });
    
    display.setErrorCallback([](DWINError error) {
        handleComponentError("CeriaDisplayDWIN", String(error));
    });
    
    // Unified data callbacks
    comm.onReceive([](const String& key, const String& value) {
        processIncomingCommand(key, value);
    });
}

void handleComponentError(const String& component, const String& error) {
    Serial.println("ERROR [" + component + "]: " + error);
    // Implement recovery strategies
}
```

### Future Development Roadmap
**File**: `docs/UPGRADE_PLAN.md`

#### Planned Enhancements
1. **CAN Bus Module** - CeriaSensorCAN untuk automotive applications
2. **LoRa Communication** - CeriaModuleLoRa untuk long-range communication
3. **SD Card Logger** - CeriaModuleSDCard untuk data logging
4. **Advanced Sensors** - More sensor wrappers (BME680, SCD30, etc.)
5. **Mesh Networking** - CeriaNetworkMesh untuk multi-node networks

#### Architecture Improvements
- **Event System**: Centralized event dispatch system
- **Configuration Manager**: JSON-based configuration management
- **Over-the-Air Updates**: OTA support untuk ESP32 nodes
- **Data Pipeline**: Real-time data processing pipeline
- **Security Layer**: Encryption dan authentication untuk communication

### Documentation Standards
Setiap komponen CeriaDev harus menyediakan:
1. **Header Documentation**: Comprehensive API documentation dalam header file
2. **Implementation Guide**: Step-by-step usage examples
3. **Integration Examples**: Real-world integration scenarios
4. **Performance Benchmarks**: Memory usage dan timing characteristics
5. **Troubleshooting Guide**: Common issues dan solutions

CeriaDev framework menyediakan complete ecosystem untuk professional embedded development dengan focus pada modularity, reliability, dan ease of integration.