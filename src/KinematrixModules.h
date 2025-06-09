/*
 *  KinematrixModules.h
 *
 *  KinematrixModules lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#include "Arduino.h"


#ifdef ENABLE_MODULE_I2C_EXPANDER
#include "../lib/modules/communication/wired/i2c/i2c-expander.h"
#include "../lib/modules/communication/wired/i2c/i2c-expander.cpp"
#endif

#ifdef ENABLE_MODULE_I2C_SCANNER
#include "../lib/modules/communication/wired/i2c/i2c-scanner.h"
#include "../lib/modules/communication/wired/i2c/i2c-scanner.cpp"
#endif

#ifdef ENABLE_MODULE_IO_EXPANDER
#include "../lib/modules/communication/wired/i2c/io-expander.h"
#include "../lib/modules/communication/wired/i2c/io-expander.cpp"
#endif

#ifdef ENABLE_MODULE_MODBUS
#include "../lib/modules/communication/wired/modbus/modbus.h"
#include "../lib/modules/communication/wired/modbus/modbusDevice.h"
#include "../lib/modules/communication/wired/modbus/modbusRegBank.h"
#include "../lib/modules/communication/wired/modbus/modbusSlave.h"
#include "../lib/modules/communication/wired/modbus/modbusDevice.cpp"
#include "../lib/modules/communication/wired/modbus/modbusRegBank.cpp"
#include "../lib/modules/communication/wired/modbus/modbusSlave.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_ENHANCED
#include "../lib/modules/communication/wired/serial/enhanced-serial.h"
#include "../lib/modules/communication/wired/serial/enhanced-serial.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_HARD
#include "../lib/modules/communication/wired/serial/hard-serial.h"
#include "../lib/modules/communication/wired/serial/hard-serial.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_NEMA
#include "../lib/modules/communication/wired/serial/nema-serial.h"
#include "../lib/modules/communication/wired/serial/nema-serial.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_SOFT
#include "../lib/modules/communication/wired/serial/soft-serial.h"
#include "../lib/modules/communication/wired/serial/soft-serial.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_SWAP
#include "../lib/modules/communication/wired/serial/swap-serial.h"
#include "../lib/modules/communication/wired/serial/swap-serial.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_SPI
#include "../lib/modules/communication/wired/spi/spi-serial.h"
#include "../lib/modules/communication/wired/spi/spi-serial.cpp"
#endif

#ifdef ENABLE_MODULE_DOT_MATRIX
#include "../lib/modules/communication/wired/spi/dot-matrix.h"
#include "../lib/modules/communication/wired/spi/dot-matrix.cpp"
#endif

#ifdef ENABLE_MODULE_LORA_COM
#include "../lib/modules/communication/wireless/lora/lora-base.h"
#include "../lib/modules/communication/wireless/lora/lora-base.cpp"
#include "../lib/modules/communication/wireless/lora/lora-com.h"
#include "../lib/modules/communication/wireless/lora/lora-com.cpp"
#endif

#ifdef ENABLE_MODULE_LORA_COM_V2
#include "../lib/modules/communication/wireless/lora/lora-base.h"
#include "../lib/modules/communication/wireless/lora/lora-base.cpp"
#include "../lib/modules/communication/wireless/lora/lora-comv2.h"
#include "../lib/modules/communication/wireless/lora/lora-comv2.cpp"
#endif

#ifdef ENABLE_MODULE_LORA_EBYTE_E220
#include "../lib/modules/communication/wireless/lora/lora-ebyte-e220.h"
#include "../lib/modules/communication/wireless/lora/lora-ebyte-e220.cpp"
#endif


#ifdef ENABLE_MODULE_ESP_NOW
#include "../lib/modules/communication/wireless/now/esp-now.h"
#include "../lib/modules/communication/wireless/now/esp-now.cpp"
#endif

#ifdef ENABLE_MODULE_DECISION_TREE
#include "../lib/modules/control/DecisionTree.h"
#include "../lib/modules/control/DecisionTree.cpp"
#endif

#ifdef ENABLE_MODULE_FUZZY_MAMDANI
#include "../lib/modules/control/FuzzyMamdani.h"
#include "../lib/modules/control/FuzzyMamdani.cpp"
#endif

#ifdef ENABLE_MODULE_FUZZY_SUGENO
#include "../lib/modules/control/FuzzySugeno.h"
#include "../lib/modules/control/FuzzySugeno.cpp"
#endif

#ifdef ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "../lib/modules/control/FuzzyTsukamoto.h"
#include "../lib/modules/control/FuzzyTsukamoto.cpp"
#endif

#ifdef ENABLE_MODULE_KNN
#include "../lib/modules/control/KNN.h"
#include "../lib/modules/control/KNN.cpp"
#endif

#ifdef ENABLE_MODULE_PID
#include "../lib/modules/control/PID.h"
#include "../lib/modules/control/PID.cpp"
#endif

#ifdef ENABLE_MODULE_PID_CONTROLLER
#include "../lib/modules/control/PIDController.h"
#include "../lib/modules/control/PIDController.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_DEBUGGER
#include "../lib/modules/debug/SerialDebugger.h"
#include "../lib/modules/debug/SerialDebugger.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_DEBUGGER_LITE
#include "../lib/modules/debug/SerialDebuggerLite.h"
#include "../lib/modules/debug/SerialDebuggerLite.cpp"
#endif

#ifdef ENABLE_MODULE_SERIAL_DEBUGGER_V2
#include "../lib/modules/debug/SerialDebuggerV2.h"
#include "../lib/modules/debug/SerialDebuggerV2.cpp"
#endif

#ifdef ENABLE_MODULE_LCD_MENU
#include "../lib/modules/display/LiquidCrystal_I2C.h"
#include "../lib/modules/display/LiquidCrystal_I2C.cpp"
#include "../lib/modules/display/lcd-menu.h"
#include "../lib/modules/display/lcd-menu.cpp"
#endif

#ifdef ENABLE_MODULE_OLED_MENU
#include "../lib/modules/display/oled-menu.h"
#include "../lib/modules/display/oled-menu.cpp"
#endif

#ifdef ENABLE_MODULE_SH1106_MENU
#include "../lib/modules/display/sh1106-menu.h"
#include "../lib/modules/display/sh1106-menu.cpp"
#include "../lib/modules/display/sh1106-render1.cpp"
#include "../lib/modules/display/sh1106-render2.cpp"
#include "../lib/modules/display/sh1106-render3.cpp"
#include "../lib/modules/display/sh1106-render4.cpp"
#endif

#ifdef ENABLE_MODULE_MOTOR_DRIVER
#include "../lib/modules/driver/motor/motor-driver.h"
#include "../lib/modules/driver/motor/motor-driver.cpp"
#endif

#ifdef ENABLE_MODULE_SERVO_HCPA9685
#include "../lib/modules/driver/servo/HCPCA9685.h"
#include "../lib/modules/driver/servo/HCPCA9685.cpp"
#endif

#ifdef ENABLE_MODULE_EEPROM_LIB
#include "../lib/modules/file/EEPROMLib.h"
#include "../lib/modules/file/EEPROMLib.cpp"
#endif

#ifdef ENABLE_MODULE_EEPROM_LIB_ESP8266
#include "../lib/modules/file/EEPROMLibESP8266.h"
#include "../lib/modules/file/EEPROMLibESP8266.cpp"
#endif

#ifdef ENABLE_MODULE_SD_ARDUINO
#include "../lib/modules/file/SDArduino.h"
#include "../lib/modules/file/SDArduino.cpp"
#endif

#ifdef ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "../lib/modules/file/SDCardModuleESP32.h"
#include "../lib/modules/file/SDCardModuleESP32.cpp"
#endif

#ifdef ENABLE_MODULE_DIGITAL_INPUT
#include "../lib/modules/io/input-module.h"
#include "../lib/modules/io/input-module.cpp"
#endif

#ifdef ENABLE_MODULE_DIGITAL_OUTPUT
#include "../lib/modules/io/output-module.h"
#include "../lib/modules/io/output-module.cpp"
#endif

#ifdef ENABLE_MODULE_PCF8574_INPUT_MODULE
#include "../lib/modules/io/pcf8574-input-module.h"
#include "../lib/modules/io/pcf8574-input-module.cpp"
#endif

#ifdef ENABLE_MODULE_PCF8574_MODULE
#include "../lib/modules/io/pcf8574-module.h"
#include "../lib/modules/io/pcf8574-module.cpp"
#endif

#ifdef ENABLE_MODULE_PCF8574_OUTPUT_MODULE
#include "../lib/modules/io/pcf8574-output-module.h"
#include "../lib/modules/io/pcf8574-output-module.cpp"
#endif

#ifdef ENABLE_MODULE_ROTARY_ENCODER
#include "../lib/modules/io/rotary-module.h"
#include "../lib/modules/io/rotary-module.cpp"
#endif

#ifdef ENABLE_MODULE_SEVEN_SEGMENT
#include "../lib/modules/io/seven-segment.h"
#include "../lib/modules/io/seven-segment.cpp"
#endif

#ifdef ENABLE_MODULE_SEVEN_SEGMENT_PCF
#include "../lib/modules/io/seven-segment-pcf.h"
#include "../lib/modules/io/seven-segment-pcf.cpp"
#endif

#ifdef ENABLE_MODULE_SEVEN_SEGMENT_74HC595
#include "../lib/modules/io/seven-segment-74hc595.h"
#include "../lib/modules/io/seven-segment-74hc595.cpp"
#endif

#ifdef ENABLE_MODULE_FREE_RTOS_HANDLER
#include "../lib/modules/task/FreeRTOSHandler.h"
#include "../lib/modules/task/FreeRTOSHandler.cpp"
#endif

#ifdef ENABLE_MODULE_TASK_HANDLER
#include "../lib/modules/task/Task.h"
#include "../lib/modules/task/Task.cpp"
#endif

#ifdef ENABLE_MODULE_TIMER_DURATION
#include "../lib/modules/time/timer-duration.h"
#include "../lib/modules/time/timer-duration.cpp"
#endif

#ifdef ENABLE_MODULE_TIMER_MANAGER
#include "../lib/modules/time/timer-manager.h"
#include "../lib/modules/time/timer-manager.cpp"
#endif

#ifdef ENABLE_MODULE_TIMER_TASK
#include "../lib/modules/time/timer-task.h"
#include "../lib/modules/time/timer-task.cpp"
#endif

#ifdef ENABLE_MODULE_EASY_LOGIC
#include "../lib/modules/utils/EasyLogic.h"
#include "../lib/modules/utils/EasyLogic.cpp"
#endif

#ifdef ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "../lib/modules/utils/LogicConditionManager.h"
#include "../lib/modules/utils/LogicConditionManager.cpp"
#endif

#ifdef ENABLE_MODULE_STATE_ACTION_MANAGER
#include "../lib/modules/utils/StateActionManager.h"
#include "../lib/modules/utils/StateActionManager.cpp"
#endif

#ifdef ENABLE_MODULE_VARIABLE_WATCHER
#include "../lib/modules/utils/VariableWatcher.h"
#include "../lib/modules/utils/VariableWatcher.cpp"
#endif

#ifdef ENABLE_MODULE_DATETIME_NTP
#include "../lib/modules/wifi/datetime-ntp.h"
#include "../lib/modules/wifi/datetime-ntp.cpp"
#endif

#ifdef ENABLE_MODULE_DATETIME_NTP_V2
#include "../lib/modules/wifi/datetime-ntpv2.h"
#include "../lib/modules/wifi/datetime-ntpv2.cpp"
#endif

#ifdef ENABLE_MODULE_GOOGLE_SHEETS
#include "../lib/modules/wifi/google-sheets.h"
#include "../lib/modules/wifi/google-sheets.cpp"
#endif

#ifdef ENABLE_MODULE_MQTT_MANAGER
#include "../lib/modules/wifi/mqtt-manager.h"
#include "../lib/modules/wifi/mqtt-manager.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_HANDLER
#include "../lib/modules/wifi/firebase/firebasev1/firebase-handler.h"
#include "../lib/modules/wifi/firebase/firebasev1/firebase-handler.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_FIRESTORE_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-firestorev2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-firestorev2.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_MESSAGING_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-messagingv2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-messagingv2.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_RTDB_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-rtdbv2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-rtdbv2.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_STORAGE_V2
#include "../lib/modules/wifi/firebase/firebasev2/firebase-storagev2.h"
#include "../lib/modules/wifi/firebase/firebasev2/firebase-storagev2.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_APPLICATION_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-applicationv3.h"
#include "../lib/modules/wifi/firebase/firebasev3/firebase-applicationv3.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_FIRESTORE_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-firestorev3.h"
#include "../lib/modules/wifi/firebase/firebasev3/firebase-firestorev3.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_MESSAGING_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-messagingv3.h"
#include "../lib/modules/wifi/firebase/firebasev3/firebase-messagingv3.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_RTDB_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-rtdbv3.h"
#include "../lib/modules/wifi/firebase/firebasev3/firebase-rtdbv3.cpp"
#endif

#ifdef ENABLE_MODULE_FIREBASE_STORAGE_V3
#include "../lib/modules/wifi/firebase/firebasev3/firebase-storagev3.h"
#include "../lib/modules/wifi/firebase/firebasev3/firebase-storagev3.cpp"
#endif

#ifdef ENABLE_MODULE_TELEGRAM_BOT
#include "../lib/modules/wifi/telegram-bot.h"
#include "../lib/modules/wifi/telegram-bot.cpp"
#endif

#ifdef ENABLE_MODULE_WHATSAPP_BOT
#include "../lib/modules/wifi/whatsapp-bot.h"
#include "../lib/modules/wifi/whatsapp-bot.cpp"
#endif

#ifdef ENABLE_MODULE_WIFI_HANDLER
#include "../lib/modules/wifi/wifi-handler.h"
#include "../lib/modules/wifi/wifi-handler.cpp"
#endif

#ifdef ENABLE_MODULE_WIFI_HANDLER_2
#include "../lib/modules/wifi/wifi-handlerv2.h"
#include "../lib/modules/wifi/wifi-handlerv2.cpp"
#endif

#ifdef ENABLE_MODULE_WIFI_MODULE
#include "../lib/modules/wifi/WiFiModule.h"
#include "../lib/modules/wifi/WiFiModule.cpp"
#endif