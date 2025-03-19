/*
 *  KinematrixSensorNoDef.h
 *
 *  KinematrixSensorNoDef lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifdef ENABLE_SENSOR_NODEF_MODULE
#include "../lib/sensors/base/sensor-header.h"
#include "../lib/sensors/base/sensor-module.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MODULE_UTILITY
#include "../lib/sensors/base/sensor-debug.h"
#include "../lib/sensors/base/sensor-calibration.h"
#include "../lib/sensors/addons/sensor-filter.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ABSTRACT
#include "../lib/sensors/abstract-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_AHT
#include "../lib/sensors/aht-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ANALOG
#include "../lib/sensors/analog-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_BME280
#include "../lib/sensors/bme280-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DHT
#include "../lib/sensors/dht-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DIGITAL
#include "../lib/sensors/digital-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DS18B20
#include "../lib/sensors/ds18b20-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DUST
#include "../lib/sensors/dust-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_FLOWMETER
#include "../lib/sensors/flowmeter-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_FLOWMETERV2
#include "../lib/sensors/flowmeterv2-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_GPS
#include "../lib/sensors/gps-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_GM67
#include "../lib/sensors/gm67-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_HX711
#include "../lib/sensors/hx711-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_INA219
#include "../lib/sensors/ina219-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_KEYPADI2C
#include "../lib/sensors/lib/Keypad/Key.h"
#include "../lib/sensors/lib/Keypad/Keypad.h"
#include "../lib/sensors/lib/Keypad_I2c/Keypad_I2C.h"
#include "../lib/sensors/keypad-i2c-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX3010X
#include "../lib/sensors/max3010x-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX3010XV2
#include "../lib/sensors/max3010xv2-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX6675
#include "../lib/sensors/max6675-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX30100LIB
#include "../lib/sensors/max30100lib-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX31865
#include "../lib/sensors/max31865-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MLX90614
#include "../lib/sensors/mlx90614-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MQ
#include "../lib/sensors/mq-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_PH
#include "../lib/sensors/ph-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_PZEM004T
#include "../lib/sensors/lib/PZEM004t/PZEM004Tc30_custom.h"
#include "../lib/sensors/pzem004t-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_RFID
#include "../lib/sensors/rfid-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_RPM
#include "../lib/sensors/rpm-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_RTC
#include "../lib/sensors/rtc-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_SCD30
#include "../lib/sensors/scd30-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_SOIL
#include "../lib/sensors/soil-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_SOILPH
#include "../lib/sensors/soilph-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_TCS3200
#include "../lib/sensors/tcs3200-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_TDS
#include "../lib/sensors/tds-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_TURBIDITY
#include "../lib/sensors/turbidity-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ULTRASONIC
#include "../lib/sensors/ultrasonic-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_VOLTAGE
#include "../lib/sensors/voltage-sens.h"
#endif