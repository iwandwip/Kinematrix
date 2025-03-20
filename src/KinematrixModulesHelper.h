/*
 *  KinematrixModuleHelper.h
 *
 *  KinematrixModuleHelper lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifdef ENABLE_MODULE_HELPER_I2C_EXPANDER
#include "../lib/modules/communication/wired/i2c/i2c-expander.h"
#include "../lib/modules/communication/wired/i2c/i2c-expander.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_I2C_SCANNER
#include "../lib/modules/communication/wired/i2c/i2c-scanner.h"
#include "../lib/modules/communication/wired/i2c/i2c-scanner.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_IO_EXPANDER
#include "../lib/modules/communication/wired/i2c/io-expander.h"
#include "../lib/modules/communication/wired/i2c/io-expander.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_MODBUS
#include "../lib/modules/communication/wired/modbus/modbus.h"
#include "../lib/modules/communication/wired/modbus/modbusDevice.h"
#include "../lib/modules/communication/wired/modbus/modbusRegBank.h"
#include "../lib/modules/communication/wired/modbus/modbusSlave.h"
#include "../lib/modules/communication/wired/modbus/modbusDevice.cpp"
#include "../lib/modules/communication/wired/modbus/modbusRegBank.cpp"
#include "../lib/modules/communication/wired/modbus/modbusSlave.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SERIAL_ENHANCED
#include "../lib/modules/communication/wired/serial/enhanced-serial.h"
#include "../lib/modules/communication/wired/serial/enhanced-serial.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SERIAL_HARD
#include "../lib/modules/communication/wired/serial/hard-serial.h"
#include "../lib/modules/communication/wired/serial/hard-serial.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SERIAL_SOFT
#include "../lib/modules/communication/wired/serial/soft-serial.h"
#include "../lib/modules/communication/wired/serial/soft-serial.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SERIAL_SWAP
#include "../lib/modules/communication/wired/serial/swap-serial.h"
#include "../lib/modules/communication/wired/serial/swap-serial.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SERIAL_SPI
#include "../lib/modules/communication/wired/spi/spi-serial.h"
#include "../lib/modules/communication/wired/spi/spi-serial.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_DOT_MATRIX
#include "../lib/modules/communication/wired/spi/dot-matrix.h"
#include "../lib/modules/communication/wired/spi/dot-matrix.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_LORA_COM
#include "../lib/modules/communication/wireless/lora/lora-base.h"
#include "../lib/modules/communication/wireless/lora/lora-base.cpp"
#include "../lib/modules/communication/wireless/lora/lora-com.h"
#include "../lib/modules/communication/wireless/lora/lora-com.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_LORA_COM_V2
#include "../lib/modules/communication/wireless/lora/lora-base.h"
#include "../lib/modules/communication/wireless/lora/lora-base.cpp"
#include "../lib/modules/communication/wireless/lora/lora-comv2.h"
#include "../lib/modules/communication/wireless/lora/lora-comv2.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_ESP_NOW
#include "../lib/modules/communication/wireless/now/esp-now.h"
#include "../lib/modules/communication/wireless/now/esp-now.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_PID
#include "../lib/modules/control/PID.h"
#include "../lib/modules/control/PID.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_KNN
#include "../lib/modules/control/KNN.h"
#include "../lib/modules/control/KNN.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_DATETIME_NTP
#include "../lib/modules/wifi/datetime-ntp.h"
#include "../lib/modules/wifi/datetime-ntp.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_DATETIME_NTP_V2
#include "../lib/modules/wifi/datetime-ntpv2.h"
#include "../lib/modules/wifi/datetime-ntpv2.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_GOOGLE_SHEETS
#include "../lib/modules/wifi/google-sheets.h"
#include "../lib/modules/wifi/google-sheets.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_FIREBASE_HANDLER
#include "../lib/modules/wifi/firebase/firebasev1/firebase-handler.h"
#include "../lib/modules/wifi/firebase/firebasev1/firebase-handler.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_FIREBASE_FIRESTORE_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-firestorev2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-firestorev2.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_FIREBASE_MESSAGING_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-messagingv2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-messagingv2.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_FIREBASE_RTDB_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-rtdbv2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-rtdbv2.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_FIREBASE_STORAGE_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-storagev2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-storagev2.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_TELEGRAM_BOT
#include "../lib/modules/wifi/telegram-bot.h"
#include "../lib/modules/wifi/telegram-bot.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_WHATSAPP_BOT
#include "../lib/modules/wifi/whatsapp-bot.h"
#include "../lib/modules/wifi/whatsapp-bot.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_WIFI_HANDLER
#include "../lib/modules/wifi/wifi-handler.h"
#include "../lib/modules/wifi/wifi-handler.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_WIFI_HANDLER_V2
#include "../lib/modules/wifi/wifi-handlerv2.h"
#include "../lib/modules/wifi/wifi-handlerv2.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_LCD_MENU
#include "../lib/modules/display/LiquidCrystal_I2C.h"
#include "../lib/modules/display/LiquidCrystal_I2C.cpp"
#include "../lib/modules/display/lcd-menu.h"
#include "../lib/modules/display/lcd-menu.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_OLED_MENU
#include "../lib/modules/display/oled-menu.h"
#include "../lib/modules/display/oled-menu.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SH1106_MENU
#include "../lib/modules/display/sh1106-menu.h"
#include "../lib/modules/display/sh1106-menu.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_MOTOR_DRIVER
#include "../lib/modules/driver/motor/motor-driver.h"
#include "../lib/modules/driver/motor/motor-driver.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SERVO_HCPA9685
#include "../lib/modules/driver/servo/HCPCA9685.h"
#include "../lib/modules/driver/servo/HCPCA9685.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_EEPROM_LIB
#include "../lib/modules/file/EEPROMLib.h"
#include "../lib/modules/file/EEPROMLib.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_EEPROM_LIB_ESP8266
#include "../lib/modules/file/EEPROMLibESP8266.h"
#include "../lib/modules/file/EEPROMLibESP8266.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SD_ARDUINO
#include "../lib/modules/file/SDArduino.h"
#include "../lib/modules/file/SDArduino.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_DIGITAL_INPUT
#include "../lib/modules/io/input-module.h"
#include "../lib/modules/io/input-module.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_DIGITAL_OUTPUT
#include "../lib/modules/io/output-module.h"
#include "../lib/modules/io/output-module.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_PCF8574_INPUT_MODULE
#include "../lib/modules/io/pcf8574-input-module.h"
#include "../lib/modules/io/pcf8574-input-module.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_PCF8574_MODULE
#include "../lib/modules/io/pcf8574-module.h"
#include "../lib/modules/io/pcf8574-module.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_PCF8574_OUTPUT_MODULE
#include "../lib/modules/io/pcf8574-output-module.h"
#include "../lib/modules/io/pcf8574-output-module.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_ROTARY_ENCODER
#include "../lib/modules/io/rotary-module.h"
#include "../lib/modules/io/rotary-module.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SEVEN_SEGMENT
#include "../lib/modules/io/seven-segment.h"
#include "../lib/modules/io/seven-segment.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SEVEN_SEGMENT_PCF
#include "../lib/modules/io/seven-segment-pcf.h"
#include "../lib/modules/io/seven-segment-pcf.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_SEVEN_SEGMENT_74HC595
#include "../lib/modules/io/seven-segment-74hc595.h"
#include "../lib/modules/io/seven-segment-74hc595.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_TASK_HANDLER
#include "../lib/modules/task/Task.h"
#include "../lib/modules/task/Task.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_TIMER_DURATION
#include "../lib/modules/time/timer-duration.h"
#include "../lib/modules/time/timer-duration.cpp"
#endif

#ifdef ENABLE_MODULE_HELPER_TIMER_TASK
#include "../lib/modules/time/timer-task.h"
#include "../lib/modules/time/timer-task.cpp"
#endif