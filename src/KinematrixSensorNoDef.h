/*
 *  KinematrixSensorNoDef.h
 *
 *  KinematrixSensorNoDef lib
 *  Created on: 2023. 4. 3
 */

#pragma once

// sensor-v1
#ifdef ENABLE_SENSOR_NODEF_MODULE
#include "../lib/sensors/sensor-v1/base/sensor-header.h"
#include "../lib/sensors/sensor-v1/base/sensor-module.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MODULE_UTILITY
#include "../lib/sensors/sensor-v1/base/sensor-debug.h"
#include "../lib/sensors/sensor-v1/base/sensor-calibration.h"
#include "../lib/sensors/sensor-v1/addons/sensor-filter.h"
#endif

#ifdef ENABLE_NODEF_ANALOG_SENSOR_CALIBRATOR
#include "../lib/sensors/sensor-v1/calibration/AnalogSensorCalibrator.h"
#endif

#ifdef ENABLE_NODEF_INTERACTIVE_SERIAL_ANALOG_SENSOR_CALIBRATOR
#include "../lib/sensors/sensor-v1/calibration/InteractiveSerialAnalogSensorCalibrator.h"
#endif

#ifdef ENABLE_NODEF_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR
#include "../lib/sensors/sensor-v1/calibration/InteractiveSerialGeneralSensorCalibrator.h"
#endif

#ifdef ENABLE_NODEF_MULTI_SENSOR_CALIBRATION_MANAGER
#include "../lib/sensors/sensor-v1/calibration/MultiSensorCalibrationManager.h"
#endif

#ifdef ENABLE_NODEF_SENSOR_CALIBRATION_MODULE
#include "../lib/sensors/sensor-v1/calibration/SensorCalibrationModule.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ABSTRACT
#include "../lib/sensors/sensor-v1/abstract-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_AHT
#include "../lib/sensors/sensor-v1/aht-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ANALOG
#include "../lib/sensors/sensor-v1/analog-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_BME280
#include "../lib/sensors/sensor-v1/bme280-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DHT
#include "../lib/sensors/sensor-v1/dht-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DIGITAL
#include "../lib/sensors/sensor-v1/digital-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DS18B20
#include "../lib/sensors/sensor-v1/ds18b20-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DUST
#include "../lib/sensors/sensor-v1/dust-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_FLOWMETER
#include "../lib/sensors/sensor-v1/flowmeter-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_FLOWMETERV2
#include "../lib/sensors/sensor-v1/flowmeterv2-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_FLOWMETERV3
#include "../lib/sensors/sensor-v1/flowmeterv3-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_GPS
#include "../lib/sensors/sensor-v1/gps-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_GM67
#include "../lib/sensors/sensor-v1/gm67-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_HX711
#include "../lib/sensors/sensor-v1/hx711-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_INA219
#include "../lib/sensors/sensor-v1/ina219-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_KEYPADI2C
#include "../lib/sensors/sensor-v1/lib/Keypad/Key.h"
#include "../lib/sensors/sensor-v1/lib/Keypad/Keypad.h"
#include "../lib/sensors/sensor-v1/lib/Keypad_I2c/Keypad_I2C.h"
#include "../lib/sensors/sensor-v1/keypad-i2c-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX3010X
#include "../lib/sensors/sensor-v1/max3010x-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX3010XV2
#include "../lib/sensors/sensor-v1/max3010xv2-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX6675
#include "../lib/sensors/sensor-v1/max6675-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX30100LIB
#include "../lib/sensors/sensor-v1/max30100lib-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MAX31865
#include "../lib/sensors/sensor-v1/max31865-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MLX90614
#include "../lib/sensors/sensor-v1/mlx90614-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MQ
#include "../lib/sensors/sensor-v1/mq-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_PH
#include "../lib/sensors/sensor-v1/ph-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_PZEM004T
#include "../lib/sensors/sensor-v1/lib/PZEM004t/PZEM004Tc30_custom.h"
#include "../lib/sensors/sensor-v1/pzem004t-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_RFID
#include "../lib/sensors/sensor-v1/rfid-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_RPM
#include "../lib/sensors/sensor-v1/rpm-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_RTC
#include "../lib/sensors/sensor-v1/rtc-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_SCD30
#include "../lib/sensors/sensor-v1/scd30-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_SOIL
#include "../lib/sensors/sensor-v1/soil-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_SOILPH
#include "../lib/sensors/sensor-v1/soilph-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_TCS3200
#include "../lib/sensors/sensor-v1/tcs3200-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_TDS
#include "../lib/sensors/sensor-v1/tds-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_TURBIDITY
#include "../lib/sensors/sensor-v1/turbidity-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ULTRASONIC
#include "../lib/sensors/sensor-v1/ultrasonic-sens.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_VOLTAGE
#include "../lib/sensors/sensor-v1/voltage-sens.h"
#endif

// sensor-v2
#ifdef ENABLE_NODEF_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#include "../lib/sensors/sensor-v2/InteractiveSerialGeneralSensorCalibratorV2.h"
#endif

#ifdef ENABLE_NODEF_SENSOR_ALERT_SYSTEM_V2
#include "../lib/sensors/sensor-v2/SensorAlertSystemV2.h"
#endif

#ifdef ENABLE_NODEF_SENSOR_CALIBRATION_MODULE_V2
#include "../lib/sensors/sensor-v2/SensorCalibrationModuleV2.h"
#endif

#ifdef ENABLE_NODEF_SENSOR_FILTER_V2
#include "../lib/sensors/sensor-v2/SensorFilterV2.h"
#endif

#ifdef ENABLE_NODEF_SENSOR_MODULE_V2
#include "../lib/sensors/sensor-v2/SensorModuleV2.h"
#endif

#ifdef ENABLE_NODEF_SENSOR_UTILITY_V2
#include "../lib/sensors/sensor-v2/SensorUtilityV2.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ABSTRACT_V2
#include "../lib/sensors/sensor-v2/SensorList/AbstractSensV2.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_ANALOG_V2
#include "../lib/sensors/sensor-v2/SensorList/AnalogSensV2.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_BME680_v2
#include "../lib/sensors/sensor-v2/SensorList/BME680SensV2.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_DHT_V2
#include "../lib/sensors/sensor-v2/SensorList/DHTSensV2.h"
#endif

#ifdef ENABLE_NODEF_INA219_V2
#include "../lib/sensors/sensor-v2/SensorList/INA219SensV2.h"
#endif

#ifdef ENABLE_SENSOR_NODEF_MLX90614_V2
#include "../lib/sensors/sensor-v2/SensorList/MLX90614SensV2.h"
#endif