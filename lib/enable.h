/*
 *  index
 *
 *  all index lib
 *  Created on: 2023. 4. 3
 */

#include "Arduino.h"

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