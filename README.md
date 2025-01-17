# **Kinematrix Beta**

**Simplify Your Arduino Projects**  
Kinematrix is a modular and comprehensive library for Arduino that streamlines project development by abstracting complex modules and sensor integrations. Whether you’re a beginner or a professional, Kinematrix helps you build faster, smarter, and more efficient projects.

---

## **Features**
- Modular support for a wide range of **inputs and outputs**.
- Pre-built utilities for **sensors**, **communication modules**, and **data handling**.
- Beginner-friendly API with scalability for advanced use cases.

---

## **Getting Started**

### **1. Installation**
To use Kinematrix, include the required modules and sensors for your project:

```cpp
#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_ULTRASONIC
#define ENABLE_SENSOR_SOIL
#include "Kinematrix.h"
```

### **2. Basic Usage**

#### **Example 1: Sensor Integration**
This example demonstrates how to use ultrasonic and soil moisture sensors:
```cpp
#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_ULTRASONIC
#define ENABLE_SENSOR_SOIL

SensorModule sensor;

void setup() {
    Serial.begin(115200);
    sensor.addModule("sonar", new UltrasonicSens(13, 12));
    sensor.addModule("moisture", new SoilMoistureSens(A0));
    sensor.init();
}

void loop() {
    sensor.update([]() -> void {
        sensor.debug(300, true);
    });
}
```

#### **Example 2: Digital Input and Output**
This example demonstrates controlling an LED with a button:
```cpp
#define ENABLE_MODULE_DIGITAL_INPUT
#define ENABLE_MODULE_DIGITAL_OUTPUT

DigitalIn button(2);
DigitalOut led(3);

void setup() {
    Serial.begin(115200);
}

void loop() {
    if (button.isPressed()) {
        led.on();
    } else {
        led.off();
    }
    button.update();
}
```

---

## **Available Modules**

### **Core Modules**
- **I/O Handling**: Digital and analog input/output, I2C expanders, SPI communication.
- **Communication**: LoRa, ESP-NOW, Telegram Bot, WhatsApp Bot.
- **Timers**: Task scheduling, duration-based timers.
- **Display**: Dot matrix, seven-segment displays (including 74HC595).

### **Sensors**
- Ultrasonic, soil moisture, DHT, BME280, INA219, MAX3010X, HX711, and many more.

### **Add-Ons**
- Auto-light systems, PID controllers, K-Nearest Neighbors (KNN), and more.

For the full list of modules and sensors, refer to the **Package List** below.

---

## **Package List**
Include only the necessary packages for your project to keep your code optimized:
```cpp
#define ENABLE_MODULE_SERIAL_HARD
#define ENABLE_MODULE_LCD_MENU
#define ENABLE_SENSOR_DHT
#define ENABLE_SENSOR_ULTRASONIC
#include "Kinematrix.h"
```
Available Packages
```cpp
// modules
#define ENABLE_MODULE_I2C_EXPANDER
#define ENABLE_MODULE_I2C_SCANNER
#define ENABLE_MODULE_IO_EXPANDER

#define ENABLE_MODULE_SERIAL_HARD
#define ENABLE_MODULE_SERIAL_SOFT
#define ENABLE_MODULE_SERIAL_SWAP

#define ENABLE_MODULE_SERIAL_SPI
#define ENABLE_MODULE_DOT_MATRIX
#define ENABLE_MODULE_LORA_COM
#define ENABLE_MODULE_ESP_NOW

#define ENABLE_MODULE_PID
#define ENABLE_MODULE_KNN

#define ENABLE_MODULE_DATETIME_NTP
#define ENABLE_MODULE_FIREBASE_HANDLER
#define ENABLE_MODULE_TELEGRAM_BOT
#define ENABLE_MODULE_WHATSAPP_BOT
#define ENABLE_MODULE_WIFI_HANDLER

#define ENABLE_MODULE_LCD_MENU

#define ENABLE_MODULE_MOTOR_DRIVER
#define ENABLE_MODULE_SERVO_HCPA9685

#define ENABLE_MODULE_EEPROM_LIB
#define ENABLE_MODULE_SD_ARDUINO

#define ENABLE_MODULE_DIGITAL_INPUT
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_ROTARY_ENCODER
#define ENABLE_MODULE_SEVEN_SEGMENT
#define ENABLE_MODULE_SEVEN_SEGMENT_PCF
#define ENABLE_MODULE_SEVEN_SEGMENT_74HC595

#define ENABLE_MODULE_TASK_HANDLER
#define ENABLE_MODULE_TIMER_DURATION
#define ENABLE_MODULE_TIMER_TASK

// sensor
#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY

#define ENABLE_SENSOR_ABSTRACT
#define ENABLE_SENSOR_AHT
#define ENABLE_SENSOR_ANALOG
#define ENABLE_SENSOR_BME280
#define ENABLE_SENSOR_DHT
#define ENABLE_SENSOR_DIGITAL
#define ENABLE_SENSOR_DS18B20
#define ENABLE_SENSOR_DUST
#define ENABLE_SENSOR_FLOWMETER
#define ENABLE_SENSOR_GPS
#define ENABLE_SENSOR_GM67
#define ENABLE_SENSOR_HX711
#define ENABLE_SENSOR_INA219
#define ENABLE_SENSOR_KEYPADI2C
#define ENABLE_SENSOR_MAX3010X
#define ENABLE_SENSOR_MAX3010XV2
#define ENABLE_SENSOR_MAX6675
#define ENABLE_SENSOR_MAX30100LIB
#define ENABLE_SENSOR_MAX31865
#define ENABLE_SENSOR_MLX90614
#define ENABLE_SENSOR_PH
#define ENABLE_SENSOR_PZEM004T
#define ENABLE_SENSOR_RFID
#define ENABLE_SENSOR_RPM
#define ENABLE_SENSOR_RTC
#define ENABLE_SENSOR_SCD30
#define ENABLE_SENSOR_SOIL
#define ENABLE_SENSOR_SOILPH
#define ENABLE_SENSOR_TCS3200
#define ENABLE_SENSOR_TDS
#define ENABLE_SENSOR_TURBIDITY
#define ENABLE_SENSOR_ULTRASONIC
#define ENABLE_SENSOR_VOLTAGE

// addons
#define ENABLE_ADDONS_AUTOLIGHT
#define ENABLE_ADDONS_AUTOLIGHT_V2
```

---

## **Contribution**
We welcome contributions to enhance Kinematrix! If you'd like to contribute:
1. Fork the repository.
2. Make your changes.
3. Submit a pull request with detailed explanations.

---

## **Feedback**
If you have suggestions or issues, feel free to reach out via email: [mcclawsher369@gmail.com](mailto:mcclawsher369@gmail.com).

---

## **Authors**
- **[@iwandwip](https://github.com/iwandwip)**

---

## **License**
This library is distributed under the [MIT License](https://choosealicense.com/licenses/mit/).

---

### **Improvements Made:**
1. **Clearer Introduction**:
    - Simplified the description for a stronger first impression.
2. **Structured Features and Examples**:
    - Organized into clear sections (Installation, Usage, Modules).
3. **Enhanced Visual Appeal**:
    - Used markdown headers and consistent formatting for better readability.
4. **Call to Action**:
    - Encouraged contributions and feedback with clear instructions.