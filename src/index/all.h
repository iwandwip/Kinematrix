/*
 *  all.h
 *
 *  all index lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef ALL_LIB_H
#define ALL_LIB_H

//#include "KinematrixConfig.h"

// modules
#ifdef USE_I2C_EXPANDER
#include "modules/communication/wired/i2c/i2c-expander.h"
#endif

#ifdef USE_I2C_SCANNER
#include "modules/communication/wired/i2c/i2c-scanner.h"
#endif

#ifdef USE_IO_EXPANDER
#include "modules/communication/wired/i2c/io-expander.h"
#endif

#ifdef USE_HARD_SERIAL
#include "modules/communication/wired/serial/hard-serial.h"
#endif

#ifdef USE_SOFT_SERIAL
#include "modules/communication/wired/serial/soft-serial.h"
#endif

#ifdef USE_SWAP_SERIAL
#include "modules/communication/wired/serial/swap-serial.h"
#endif

#ifdef USE_SPI_SERIAL
#include "modules/communication/wired/spi/spi-serial.h"
#endif

#ifdef USE_DOT_MATRIX
#include "modules/communication/wired/spi/dot-matrix.h"
#endif

#ifdef USE_LORA_COM
#include "modules/communication/wireless/lora/lora-com.h"
#endif

#ifdef USE_ESP_NOW
#include "modules/communication/wireless/now/esp-now.h"
#endif

#ifdef USE_HTTP_API_CLIENT
#include "modules/communication/api/http/http-api-client.h"
#include "modules/communication/api/http/response.h"
#endif

#ifdef USE_PID
#include "modules/control/PID.h"
#endif

#ifdef USE_KNN
#include "modules/control/KNN.h"
#endif

#ifdef USE_DATETIME_NTP
#include "modules/wifi/datetime-ntp.h"
#endif

#ifdef USE_FIREBASE_HANDLER
#include "modules/wifi/firebase-handler.h"
#endif

#ifdef USE_TELEGRAM_BOT
#include "modules/wifi/telegram-bot.h"
#endif

#ifdef USE_WHATSAPP_BOT
#include "modules/wifi/whatsapp-bot.h"
#endif

#ifdef USE_WIFI_HANDLER
#include "modules/wifi/wifi-handler.h"
#endif

#ifdef USE_LCD_MENU
#include "modules/display/lcd-menu.h"
#endif

#ifdef USE_MOTOR_DRIVER
#include "modules/driver/motor/motor-driver.h"
#endif

#ifdef USE_SERVO_DRIVER
#include "modules/driver/servo/HCPCA9685.h"
#endif

#ifdef USE_EEPROM_LIB
#include "modules/file/EEPROMLib.h"
#endif

#ifdef USE_SD_ARDUINO
#include "modules/file/SDArduino.h"
#endif

#include "modules/io/input-module.h"
#include "modules/io/output-module.h"


#ifdef USE_ROTARY_MODULE
#include "modules/io/rotary-module.h"
#endif

#ifdef USE_SEVEN_SEGMENT
#include "modules/io/seven-segment.h"
#endif

#include "modules/task/Task.h"
#include "modules/time/timer-duration.h"
#include "modules/time/timer-task.h"

#include "sensors/abstract-sens.h"

#ifdef USE_AHT_SENS
#include "sensors/aht-sens.h"
#endif

#ifdef USE_BME280_SENS
#include "sensors/bme280-sens.h"
#endif

#ifdef USE_DHT_SENS
#include "sensors/dht-sens.h"
#endif

#include "sensors/digital-sens.h"


#ifdef USE_DS18B20_SENS
#include "sensors/ds18b20-sens.h"
#endif

#ifdef USE_DUST_SENS
#include "sensors/dust-sens.h"
#endif

#ifdef USE_FLOWMETER_SENS
#include "sensors/flowmeter-sens.h"
#endif

#ifdef USE_GPS_SENS
#include "sensors/gps-sens.h"
#endif

#ifdef USE_GM67_SENS
#include "sensors/gm67-sens.h"
#endif

#ifdef USE_HX711_SENS
#include "sensors/hx711-sens.h"
#endif

#ifdef USE_INA219_SENS
#include "sensors/ina219-sens.h"
#endif

#ifdef USE_KEYPAD_I2C_SENS
#include "sensors/keypad-i2c-sens.h"
#endif

#ifdef USE_MAX3010X_SENS
#include "sensors/max3010x-sens.h"
#endif

#ifdef USE_MAX6675_SENS
#include "sensors/max6675-sens.h"
#endif

#ifdef USE_MAX31865_SENS
#include "sensors/max31865-sens.h"
#endif

#ifdef USE_RFID_SENS
#include "sensors/rfid-sens.h"
#endif

#ifdef USE_RPM_SENS
#include "sensors/rpm-sens.h"
#endif

#ifdef USE_RTC_SENS
#include "sensors/rtc-sens.h"
#endif

#ifdef USE_SOIL_SENS
#include "sensors/soil-sens.h"
#endif

#ifdef USE_SOILPH_SENS
#include "sensors/soilph-sens.h"
#endif

#ifdef USE_TCS3200_SENS
#include "sensors/tcs3200-sens.h"
#endif

#ifdef USE_TDS_SENS
#include "sensors/tds-sens.h"
#endif

#ifdef USE_TURBIDITY_SENS
#include "sensors/turbidity-sens.h"
#endif

#ifdef USE_ULTRASONIC_SENS
#include "sensors/ultrasonic-sens.h"
#endif

#ifdef USE_VOLTAGE_SENS
#include "sensors/voltage-sens.h"
#endif

#endif // ALL_LIB_H
