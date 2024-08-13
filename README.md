
# Kinematrix Beta

**Arduino Library for Educational Purposes**.
Utilize this library to fulfill your project needs. Within it, there are modules for inputs such as button sensors, and the like, as well as modules for outputs such as LEDs, buzzers, and motor drivers.


## Basic Usage
#### Example 1
```cpp
#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_ULTRASONIC
#define ENABLE_SENSOR_SOIL

#include "Kinematrix.h"

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
#### Example 2
```cpp
#define ENABLE_MODULE_DIGITAL_INPUT
#define ENABLE_MODULE_DIGITAL_OUTPUT

#include "Kinematrix.h"

DigitalIn button(2);
DigitalOut led(3);

void setup() {
    Serial.begin(115200);
}

void loop() {
    if (button.isPressed()) {
        led.on();
    }
    button.update();
}

```

## Package List
First, include package according to your project's needs
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
#define ENABLE_SENSOR_RFID
#define ENABLE_SENSOR_RPM
#define ENABLE_SENSOR_RTC
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
And now you can add the header
```cpp
#include "Kinematrix.h"
```

## Authors

- [@iwandwip](https://github.com/iwandwip)


## Feedback

If you have any feedback, please reach out to me at mcclawsher369@gmail.com


## License

[MIT](https://choosealicense.com/licenses/mit/)

