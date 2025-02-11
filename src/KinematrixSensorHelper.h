/*
 *  KinematrixSensorHelper.h
 *
 *  KinematrixSensorHelper lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifdef ENABLE_SENSOR_HELPER_MODULE
#include "../lib/sensors/base/sensor-header.h"
#include "../lib/sensors/base/sensor-header.cpp"
#include "../lib/sensors/base/sensor-module.h"
#include "../lib/sensors/base/sensor-module.cpp"
#include "../lib/sensors/base/sensor-module-def.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MODULE_UTILITY
#include "../lib/sensors/base/sensor-debug.h"
#include "../lib/sensors/base/sensor-debug.cpp"
#include "../lib/sensors/base/sensor-calibration.h"
#include "../lib/sensors/base/sensor-calibration.cpp"
#include "../lib/sensors/addons/sensor-filter.h"
#include "../lib/sensors/addons/sensor-filter.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ABSTRACT
#include "../lib/sensors/abstract-sens.h"
#include "../lib/sensors/abstract-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_AHT
#include "../lib/sensors/aht-sens.h"
#include "../lib/sensors/aht-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ANALOG
#include "../lib/sensors/analog-sens.h"
#include "../lib/sensors/analog-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_BME280
#include "../lib/sensors/bme280-sens.h"
#include "../lib/sensors/bme280-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DHT
#include "../lib/sensors/dht-sens.h"
#include "../lib/sensors/dht-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DIGITAL
#include "../lib/sensors/digital-sens.h"
#include "../lib/sensors/digital-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DS18B20
#include "../lib/sensors/ds18b20-sens.h"
#include "../lib/sensors/ds18b20-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DUST
#include "../lib/sensors/dust-sens.h"
#include "../lib/sensors/dust-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_FLOWMETER
#include "../lib/sensors/flowmeter-sens.h"
#include "../lib/sensors/flowmeter-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_GPS
#include "../lib/sensors/gps-sens.h"
#include "../lib/sensors/gps-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_GM67
#include "../lib/sensors/gm67-sens.h"
#include "../lib/sensors/gm67-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_HX711
#include "../lib/sensors/hx711-sens.h"
#include "../lib/sensors/hx711-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_INA219
#include "../lib/sensors/ina219-sens.h"
#include "../lib/sensors/ina219-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_KEYPADI2C
#include "../lib/sensors/lib/Keypad/Key.h"
#include "../lib/sensors/lib/Keypad/Key.cpp"
#include "../lib/sensors/lib/Keypad/Keypad.h"
#include "../lib/sensors/lib/Keypad/Keypad.cpp"
#include "../lib/sensors/lib/Keypad_I2c/Keypad_I2C.h"
#include "../lib/sensors/lib/Keypad_I2c/Keypad_I2C.cpp"
#include "../lib/sensors/keypad-i2c-sens.h"
#include "../lib/sensors/keypad-i2c-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX3010X
#include "../lib/sensors/max3010x-sens.h"
#include "../lib/sensors/max3010x-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX3010XV2
#include "../lib/sensors/max3010xv2-sens.h"
#include "../lib/sensors/max3010xv2-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX6675
#include "../lib/sensors/max6675-sens.h"
#include "../lib/sensors/max6675-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX30100LIB
#include "../lib/sensors/max30100lib-sens.h"
#include "../lib/sensors/max30100lib-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX31865
#include "../lib/sensors/max31865-sens.h"
#include "../lib/sensors/max31865-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MLX90614
#include "../lib/sensors/mlx90614-sens.h"
#include "../lib/sensors/mlx90614-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MQ
#include "../lib/sensors/mq-sens.h"
#include "../lib/sensors/mq-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_PH
#include "../lib/sensors/ph-sens.h"
#include "../lib/sensors/ph-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_PZEM004T
#include "../lib/sensors/lib/PZEM004t/PZEM004Tc30_custom.h"
#include "../lib/sensors/lib/PZEM004t/PZEM004Tc30_custom.cpp"
#include "../lib/sensors/pzem004t-sens.h"
#include "../lib/sensors/pzem004t-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RFID
#include "../lib/sensors/rfid-sens.h"
#include "../lib/sensors/rfid-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RPM
#include "../lib/sensors/rpm-sens.h"
#include "../lib/sensors/rpm-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RTC
#include "../lib/sensors/rtc-sens.h"
#include "../lib/sensors/rtc-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_SCD30
#include "../lib/sensors/scd30-sens.h"
#include "../lib/sensors/scd30-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_SOIL
#include "../lib/sensors/soil-sens.h"
#include "../lib/sensors/soil-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_SOILPH
#include "../lib/sensors/soilph-sens.h"
#include "../lib/sensors/soilph-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_TCS3200
#include "../lib/sensors/tcs3200-sens.h"
#include "../lib/sensors/tcs3200-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_TDS
#include "../lib/sensors/tds-sens.h"
#include "../lib/sensors/tds-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_TURBIDITY
#include "../lib/sensors/turbidity-sens.h"
#include "../lib/sensors/turbidity-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ULTRASONIC
#include "../lib/sensors/ultrasonic-sens.h"
#include "../lib/sensors/ultrasonic-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_VOLTAGE
#include "../lib/sensors/voltage-sens.h"
#include "../lib/sensors/voltage-sens.cpp"
#endif