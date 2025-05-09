/*
 *  all.h
 *
 *  all index lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef ALL_LIB_H
#define ALL_LIB_H

// modules
#include "modules/communication/wired/i2c/i2c-expander.h"
#include "modules/communication/wired/i2c/i2c-scanner.h"
#include "modules/communication/wired/i2c/io-expander.h"

#include "modules/communication/wired/modbus/modbus.h"

#include "modules/communication/wired/serial/enhanced-serial.h"
#include "modules/communication/wired/serial/hard-serial.h"
#include "modules/communication/wired/serial/nema-serial.h"
#include "modules/communication/wired/serial/soft-serial.h"
#include "modules/communication/wired/serial/swap-serial.h"

#include "modules/communication/wired/spi/spi-serial.h"
#include "modules/communication/wired/spi/dot-matrix.h"
#include "modules/communication/wireless/lora/lora-com.h"
#include "modules/communication/wireless/lora/lora-comv2.h"
#include "modules/communication/wireless/now/esp-now.h"

#include "modules/control/DecisionTree.h"
#include "modules/control/FuzzyMamdani.h"
#include "modules/control/FuzzySugeno.h"
#include "modules/control/FuzzyTsukamoto.h"
#include "modules/control/KNN.h"
#include "modules/control/PID.h"
#include "modules/control/PIDController.h"

#include "modules/control/SerialDebugger.h"
#include "modules/control/SerialDebuggerLite.h"

#include "modules/wifi/datetime-ntp.h"
#include "modules/wifi/datetime-ntpv2.h"
#include "modules/wifi/google-sheets.h"
#include "modules/wifi/mqtt-manager.h"
#include "modules/wifi/firebase/firebasev1/firebase-handler.h"
#include "modules/wifi/firebase/firebasev2/firebase-firestorev2.h"
#include "modules/wifi/firebase/firebasev2/firebase-messagingv2.h"
#include "modules/wifi/firebase/firebasev2/firebase-rtdbv2.h"
#include "modules/wifi/firebase/firebasev2/firebase-storagev2.h"
#include "modules/wifi/firebase/firebasev3/firebase-applicationv3.h"
#include "modules/wifi/firebase/firebasev3/firebase-firestorev3.h"
#include "modules/wifi/firebase/firebasev3/firebase-messagingv3.h"
#include "modules/wifi/firebase/firebasev3/firebase-rtdbv3.h"
#include "modules/wifi/firebase/firebasev3/firebase-storagev3.h"
#include "modules/wifi/telegram-bot.h"
#include "modules/wifi/whatsapp-bot.h"
#include "modules/wifi/wifi-handler.h"
#include "modules/wifi/wifi-handlerv2.h"

#include "modules/display/lcd-menu.h"
#include "modules/display/oled-menu.h"
#include "modules/display/sh1106-menu.h"

#include "modules/driver/motor/motor-driver.h"
#include "modules/driver/servo/HCPCA9685.h"

#include "modules/file/EEPROMLib.h"
#include "modules/file/EEPROMLibESP8266.h"
#include "modules/file/SDArduino.h"
#include "modules/file/SDCardESP32.h"

#include "modules/io/input-module.h"
#include "modules/io/output-module.h"
#include "modules/io/pcf8574-input-module.h"
#include "modules/io/pcf8574-module.h"
#include "modules/io/pcf8574-output-module.h"
#include "modules/io/rotary-module.h"
#include "modules/io/seven-segment.h"
#include "modules/io/seven-segment-pcf.h"
#include "modules/io/seven-segment-74hc595.h"

#include "modules/task/FreeRTOSHandler.h"
#include "modules/task/Task.h"
#include "modules/time/timer-duration.h"
#include "modules/time/timer-manager.h"
#include "modules/time/timer-task.h"

// sensor
#include "sensors/abstract-sens.h"
#include "sensors/aht-sens.h"
#include "sensors/analog-sens.h"
#include "sensors/bme280-sens.h"
#include "sensors/dht-sens.h"
#include "sensors/digital-sens.h"
#include "sensors/ds18b20-sens.h"
#include "sensors/dust-sens.h"
#include "sensors/flowmeter-sens.h"
#include "sensors/flowmeterv2-sens.h"
#include "sensors/flowmeterv3-sens.h"
#include "sensors/gps-sens.h"
#include "sensors/gm67-sens.h"
#include "sensors/hx711-sens.h"
#include "sensors/ina219-sens.h"
#include "sensors/keypad-i2c-sens.h"
#include "sensors/max3010x-sens.h"
#include "sensors/max3010xv2-sens.h"
#include "sensors/max6675-sens.h"
#include "sensors/max30100lib-sens.h"
#include "sensors/max31865-sens.h"
#include "sensors/mlx90614-sens.h"
#include "sensors/mq-sens.h"
#include "sensors/ph-sens.h"
#include "sensors/pzem004t-sens.h"
#include "sensors/rfid-sens.h"
#include "sensors/rpm-sens.h"
#include "sensors/rtc-sens.h"
#include "sensors/scd30-sens.h"
#include "sensors/soil-sens.h"
#include "sensors/soilph-sens.h"
#include "sensors/tcs3200-sens.h"
#include "sensors/tds-sens.h"
#include "sensors/turbidity-sens.h"
#include "sensors/ultrasonic-sens.h"
#include "sensors/voltage-sens.h"

#endif
