# CLAUDE.md - Sensor Framework

## Directory Overview
Dual sensor framework supporting both legacy (V1) and modern (V2) sensor implementations with comprehensive features.

## Architecture
- **`SensorModuleV1/`** - Legacy sensor system with 40+ sensors
- **`SensorModuleV2/`** - Modern type-safe sensor framework

## Sensor Framework V1 (Legacy)

### Features
- **Base Class**: `BaseSens` virtual base class
- **Data Storage**: JSON document-based sensor data
- **Value Access**: Direct float/int/string getters
- **Calibration**: Integrated calibration system with EEPROM persistence
- **Filtering**: Built-in filtering algorithms
- **Debugging**: Comprehensive sensor debugging tools

### Supported Sensors (40+)
- **Environmental**: DHT, BME280/680, AHT20, SCD30 (CO2)
- **Temperature**: DS18B20, MAX31865 (RTD), MAX6675, MLX90614 (IR)
- **Analog**: General analog sensors, voltage monitoring, pH sensors
- **Digital**: Digital input/output sensors
- **Flow**: Flow meters (multiple versions), water level
- **Weight**: HX711 load cell sensors
- **Power**: INA219 current/voltage monitoring, PZEM004T
- **Environmental**: Dust/air quality, turbidity sensors
- **Identification**: RFID (MFRC522), GM67 barcode scanner
- **Navigation**: GPS with TinyGPS++, RPM sensors
- **Color**: TCS3200 color sensor
- **Biometric**: MAX30100/MAX3010x pulse oximeter
- **Soil**: Soil moisture and pH sensors
- **Input**: I2C keypad interface
- **Time**: RTC (Real-time clock) modules

### V1 Structure
```
SensorModuleV1/
├── base/                    # Core sensor framework
├── calibration/            # Calibration system
├── addons/                 # Sensor debugging and filtering
├── lib/                    # External sensor libraries
└── [sensor-name]-sens.h/cpp # Individual sensor implementations
```

## Sensor Framework V2 (Modern)

### Features
- **Type-Safe Access**: Template-based `BaseSensV2` with typed value access
- **Real-Time Filtering**: Multiple algorithms (Moving Average, Kalman, Median, Exponential)
- **Alert System**: Comprehensive threshold monitoring with callbacks
- **Interactive Calibration**: User-guided calibration with EEPROM persistence
- **Modular Design**: Clean separation of sensor implementations

### V2 Structure
```
SensorModuleV2/
├── SensorModule/           # Core V2 framework
│   ├── SensorModuleV2.h/cpp
│   └── SensorCalibrationModuleV2.h/cpp
└── SensorList/            # Individual sensor implementations
    ├── AnalogSensor/
    ├── DigitalSensor/
    └── [specific sensors]/
```

### V2 Advantages
- **Memory Efficient**: Optimized memory usage
- **Type Safety**: Compile-time type checking
- **Advanced Filtering**: Real-time signal processing
- **Alert Callbacks**: Event-driven sensor monitoring
- **Modern C++**: Leverages modern C++ features

## Sensor Categories

### Environmental Monitoring
- Temperature, humidity, pressure sensors
- Air quality and gas detection
- Light and UV sensors

### Industrial Sensors
- Flow measurement and control
- Weight and load monitoring
- Power consumption tracking

### Navigation & Positioning
- GPS location tracking
- Compass and orientation
- Speed and RPM measurement

### Biometric & Health
- Heart rate and SpO2 monitoring
- Temperature monitoring

### Identification Systems
- RFID card reading
- Barcode scanning
- Color detection

## Usage Examples

### V1 Sensor Usage
```cpp
#define ENABLE_SENSOR_MODULE_V1
#define ENABLE_SENSOR_DHT_V1
#include "Kinematrix.h"

DHTSens dht(2, DHT22);
dht.init();
float temp = dht.getTemperature();
float humidity = dht.getHumidity();
```

### V2 Sensor Usage
```cpp
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"

AnalogSensorV2 analog(A0);
analog.enableFilter(MOVING_AVERAGE, 10);
analog.setAlertThreshold(500, HIGH_THRESHOLD);
float value = analog.getValue();
```

## Migration Path
- V1 sensors remain fully supported
- V2 provides enhanced features for new projects
- Gradual migration possible with both frameworks coexisting

## Platform Support
- **ESP32**: Full V1 and V2 feature set
- **ESP8266**: Complete support with memory optimization
- **AVR**: Essential sensors with resource optimization

## Integration
Sensor frameworks are integrated through:
- `KinematrixSensor.h` - Main sensor inclusion
- `KinematrixSensorHelper.h` - Helper functions
- `KinematrixSensorNoDef.h` - Alternative inclusion

## Documentation
- `SensorModuleV2.md` - Detailed V2 framework documentation
- Individual sensor examples in `example/sensors/`