/*
 *  KinematrixModuleNoDef.h
 *
 *  KinematrixModuleNoDef lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifdef ENABLE_MODULE_NODEF_I2C_EXPANDER
#include "../lib/modules/communication/wired/i2c/i2c-expander.h"
#endif

#ifdef ENABLE_MODULE_NODEF_I2C_SCANNER
#include "../lib/modules/communication/wired/i2c/i2c-scanner.h"
#endif

#ifdef ENABLE_MODULE_NODEF_IO_EXPANDER
#include "../lib/modules/communication/wired/i2c/io-expander.h"
#endif

#ifdef ENABLE_MODULE_NODEF_MODBUS
#include "../lib/modules/communication/wired/modbus/modbus.h"
#include "../lib/modules/communication/wired/modbus/modbusDevice.h"
#include "../lib/modules/communication/wired/modbus/modbusRegBank.h"
#include "../lib/modules/communication/wired/modbus/modbusSlave.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_ENHANCED
#include "../lib/modules/communication/wired/serial/enhanced-serial.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_HARD
#include "../lib/modules/communication/wired/serial/hard-serial.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_NEMA
#include "../lib/modules/communication/wired/serial/nema-serial.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_SOFT
#include "../lib/modules/communication/wired/serial/soft-serial.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_SWAP
#include "../lib/modules/communication/wired/serial/swap-serial.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_SPI
#include "../lib/modules/communication/wired/spi/spi-serial.h"
#endif

#ifdef ENABLE_MODULE_NODEF_DOT_MATRIX
#include "../lib/modules/communication/wired/spi/dot-matrix.h"
#endif

#ifdef ENABLE_MODULE_NODEF_LORA_COM
#include "../lib/modules/communication/wireless/lora/lora-base.h"
#include "../lib/modules/communication/wireless/lora/lora-com.h"
#endif

#ifdef ENABLE_MODULE_NODEF_LORA_COM_V2
#include "../lib/modules/communication/wireless/lora/lora-base.h"
#include "../lib/modules/communication/wireless/lora/lora-comv2.h"
#endif

#ifdef ENABLE_MODULE_NODEF_LORA_EBYTE_E220
#include "../lib/modules/communication/wireless/lora/lora-ebyte-e220.h"
#endif

#ifdef ENABLE_MODULE_NODEF_ESP_NOW
#include "../lib/modules/communication/wireless/now/esp-now.h"
#endif

#ifdef ENABLE_MODULE_NODEF_DECISION_TREE
#include "../lib/modules/control/DecisionTree.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FUZZY_MAMDANI
#include "../lib/modules/control/FuzzyMamdani.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FUZZY_SUGENO
#include "../lib/modules/control/FuzzySugeno.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FUZZY_TSUKAMOTO
#include "../lib/modules/control/FuzzyTsukamoto.h"
#endif

#ifdef ENABLE_MODULE_NODEF_KNN
#include "../lib/modules/control/KNN.h"
#endif

#ifdef ENABLE_MODULE_NODEF_PID
#include "../lib/modules/control/PID.h"
#endif

#ifdef ENABLE_MODULE_NODEF_PID_CONTROLLER
#include "../lib/modules/control/PIDController.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_DEBUGGER
#include "../lib/modules/control/SerialDebugger.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERIAL_DEBUGGER_LITE
#include "../lib/modules/control/SerialDebuggerLite.h"
#endif

#ifdef ENABLE_MODULE_NODEF_DATETIME_NTP
#include "../lib/modules/wifi/datetime-ntp.h"
#endif

#ifdef ENABLE_MODULE_NODEF_DATETIME_NTP_V2
#include "../lib/modules/wifi/datetime-ntpv2.h"
#endif

#ifdef ENABLE_MODULE_NODEF_GOOGLE_SHEETS
#include "../lib/modules/wifi/google-sheets.h"
#endif

#ifdef ENABLE_MODULE_NODEF_MQTT_MANAGER
#include "../lib/modules/wifi/mqtt-manager.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_HANDLER
#include "../lib/modules/wifi/firebase/firebasev1/firebase-handler.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_FIRESTORE_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-firestorev2.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_MESSAGING_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-messagingv2.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_RTDB_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-rtdbv2.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_STORAGE_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-storagev2.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_APPLICATION_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-applicationv3.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_FIRESTORE_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-firestorev3.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_MESSAGING_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-messagingv3.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_RTDB_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-rtdbv3.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FIREBASE_STORAGE_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-storagev3.h"
#endif

#ifdef ENABLE_MODULE_NODEF_TELEGRAM_BOT
#include "../lib/modules/wifi/telegram-bot.h"
#endif

#ifdef ENABLE_MODULE_NODEF_WHATSAPP_BOT
#include "../lib/modules/wifi/whatsapp-bot.h"
#endif

#ifdef ENABLE_MODULE_NODEF_WIFI_HANDLER
#include "../lib/modules/wifi/wifi-handler.h"
#endif

#ifdef ENABLE_MODULE_NODEF_WIFI_HANDLER_V2
#include "../lib/modules/wifi/wifi-handlerv2.h"
#endif

#ifdef ENABLE_MODULE_NODEF_LCD_MENU
#include "../lib/modules/display/LiquidCrystal_I2C.h"
#include "../lib/modules/display/lcd-menu.h"
#endif

#ifdef ENABLE_MODULE_NODEF_OLED_MENU
#include "../lib/modules/display/oled-menu.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SH1106_MENU
#include "../lib/modules/display/sh1106-menu.h"
#endif

#ifdef ENABLE_MODULE_NODEF_MOTOR_DRIVER
#include "../lib/modules/driver/motor/motor-driver.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SERVO_HCPA9685
#include "../lib/modules/driver/servo/HCPCA9685.h"
#endif

#ifdef ENABLE_MODULE_NODEF_EEPROM_LIB
#include "../lib/modules/file/EEPROMLib.h"
#endif

#ifdef ENABLE_MODULE_NODEF_EEPROM_LIB_ESP8266
#include "../lib/modules/file/EEPROMLibESP8266.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SD_ARDUINO
#include "../lib/modules/file/SDArduino.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SD_CARD_MODULE_ESP32
#include "../lib/modules/file/SDCardModuleESP32.h"
#endif

#ifdef ENABLE_MODULE_NODEF_DIGITAL_INPUT
#include "../lib/modules/io/input-module.h"
#endif

#ifdef ENABLE_MODULE_NODEF_DIGITAL_OUTPUT
#include "../lib/modules/io/output-module.h"
#endif

#ifdef ENABLE_MODULE_NODEF_PCF8574_INPUT_MODULE
#include "../lib/modules/io/pcf8574-input-module.h"
#endif

#ifdef ENABLE_MODULE_NODEF_PCF8574_MODULE
#include "../lib/modules/io/pcf8574-module.h"
#endif

#ifdef ENABLE_MODULE_NODEF_PCF8574_OUTPUT_MODULE
#include "../lib/modules/io/pcf8574-output-module.h"
#endif

#ifdef ENABLE_MODULE_NODEF_ROTARY_ENCODER
#include "../lib/modules/io/rotary-module.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SEVEN_SEGMENT
#include "../lib/modules/io/seven-segment.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SEVEN_SEGMENT_PCF
#include "../lib/modules/io/seven-segment-pcf.h"
#endif

#ifdef ENABLE_MODULE_NODEF_SEVEN_SEGMENT_74HC595
#include "../lib/modules/io/seven-segment-74hc595.h"
#endif

#ifdef ENABLE_MODULE_NODEF_FREE_RTOS_HANDLER
#include "../lib/modules/task/FreeRTOSHandler.h"
#endif

#ifdef ENABLE_MODULE_NODEF_TASK_HANDLER
#include "../lib/modules/task/Task.h"
#endif

#ifdef ENABLE_MODULE_NODEF_TIMER_DURATION
#include "../lib/modules/time/timer-duration.h"
#endif

#ifdef ENABLE_MODULE_NODEF_TIMER_MANAGER
#include "../lib/modules/time/timer-manager.h"
#endif

#ifdef ENABLE_MODULE_NODEF_TIMER_TASK
#include "../lib/modules/time/timer-task.h"
#endif