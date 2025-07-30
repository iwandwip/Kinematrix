/*
 *  KinematrixSensorHelper.h
 *
 *  KinematrixSensorHelper lib
 *  Created on: 2023. 4. 3
 */

#pragma once

// SensorModuleV1
#ifdef ENABLE_SENSOR_HELPER_MODULE
#include "../lib/sensors/SensorModuleV1/base/sensor-header.h"
#include "../lib/sensors/SensorModuleV1/base/sensor-header.cpp"
#include "../lib/sensors/SensorModuleV1/base/sensor-module.h"
#include "../lib/sensors/SensorModuleV1/base/sensor-module.cpp"
#include "../lib/sensors/SensorModuleV1/base/sensor-module-def.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MODULE_UTILITY
#include "../lib/sensors/SensorModuleV1/base/sensor-debug.h"
#include "../lib/sensors/SensorModuleV1/base/sensor-debug.cpp"
#include "../lib/sensors/SensorModuleV1/base/sensor-calibration.h"
#include "../lib/sensors/SensorModuleV1/base/sensor-calibration.cpp"
#include "../lib/sensors/SensorModuleV1/addons/sensor-filter.h"
#include "../lib/sensors/SensorModuleV1/addons/sensor-filter.cpp"
#endif

#ifdef ENABLE_HELPER_ANALOG_SENSOR_CALIBRATOR
#include "../lib/sensors/SensorModuleV1/calibration/AnalogSensorCalibrator.h"
#include "../lib/sensors/SensorModuleV1/calibration/AnalogSensorCalibrator.cpp"
#endif

#ifdef ENABLE_HELPER_INTERACTIVE_SERIAL_ANALOG_SENSOR_CALIBRATOR
#include "../lib/sensors/SensorModuleV1/calibration/InteractiveSerialAnalogSensorCalibrator.h"
#include "../lib/sensors/SensorModuleV1/calibration/InteractiveSerialAnalogSensorCalibrator.cpp"
#endif

#ifdef ENABLE_HELPER_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR
#include "../lib/sensors/SensorModuleV1/calibration/InteractiveSerialGeneralSensorCalibrator.h"
#include "../lib/sensors/SensorModuleV1/calibration/InteractiveSerialGeneralSensorCalibrator.cpp"
#endif

#ifdef ENABLE_HELPER_MULTI_SENSOR_CALIBRATION_MANAGER
#include "../lib/sensors/SensorModuleV1/calibration/MultiSensorCalibrationManager.h"
#include "../lib/sensors/SensorModuleV1/calibration/MultiSensorCalibrationManager.cpp"
#endif

#ifdef ENABLE_HELPER_SENSOR_CALIBRATION_MODULE
#include "../lib/sensors/SensorModuleV1/calibration/SensorCalibrationModule.h"
#include "../lib/sensors/SensorModuleV1/calibration/SensorCalibrationModule.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ABSTRACT
#include "../lib/sensors/SensorModuleV1/abstract-sens.h"
#include "../lib/sensors/SensorModuleV1/abstract-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_AHT
#include "../lib/sensors/SensorModuleV1/aht-sens.h"
#include "../lib/sensors/SensorModuleV1/aht-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ANALOG
#include "../lib/sensors/SensorModuleV1/analog-sens.h"
#include "../lib/sensors/SensorModuleV1/analog-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_BME280
#include "../lib/sensors/SensorModuleV1/bme280-sens.h"
#include "../lib/sensors/SensorModuleV1/bme280-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DHT
#include "../lib/sensors/SensorModuleV1/dht-sens.h"
#include "../lib/sensors/SensorModuleV1/dht-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DIGITAL
#include "../lib/sensors/SensorModuleV1/digital-sens.h"
#include "../lib/sensors/SensorModuleV1/digital-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DS18B20
#include "../lib/sensors/SensorModuleV1/ds18b20-sens.h"
#include "../lib/sensors/SensorModuleV1/ds18b20-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DUST
#include "../lib/sensors/SensorModuleV1/dust-sens.h"
#include "../lib/sensors/SensorModuleV1/dust-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_FLOWMETER
#include "../lib/sensors/SensorModuleV1/flowmeter-sens.h"
#include "../lib/sensors/SensorModuleV1/flowmeter-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_FLOWMETERV2
#include "../lib/sensors/SensorModuleV1/flowmeterv2-sens.h"
#include "../lib/sensors/SensorModuleV1/flowmeterv2-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_FLOWMETERV3
#include "../lib/sensors/SensorModuleV1/flowmeterv3-sens.h"
#include "../lib/sensors/SensorModuleV1/flowmeterv3-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_GPS
#include "../lib/sensors/SensorModuleV1/gps-sens.h"
#include "../lib/sensors/SensorModuleV1/gps-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_GM67
#include "../lib/sensors/SensorModuleV1/gm67-sens.h"
#include "../lib/sensors/SensorModuleV1/gm67-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_HX711
#include "../lib/sensors/SensorModuleV1/hx711-sens.h"
#include "../lib/sensors/SensorModuleV1/hx711-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_INA219
#include "../lib/sensors/SensorModuleV1/ina219-sens.h"
#include "../lib/sensors/SensorModuleV1/ina219-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_KEYPADI2C
#include "../lib/sensors/SensorModuleV1/lib/Keypad/Key.h"
#include "../lib/sensors/SensorModuleV1/lib/Keypad/Key.cpp"
#include "../lib/sensors/SensorModuleV1/lib/Keypad/Keypad.h"
#include "../lib/sensors/SensorModuleV1/lib/Keypad/Keypad.cpp"
#include "../lib/sensors/SensorModuleV1/lib/Keypad_I2c/Keypad_I2C.h"
#include "../lib/sensors/SensorModuleV1/lib/Keypad_I2c/Keypad_I2C.cpp"
#include "../lib/sensors/SensorModuleV1/keypad-i2c-sens.h"
#include "../lib/sensors/SensorModuleV1/keypad-i2c-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX3010X
#include "../lib/sensors/SensorModuleV1/max3010x-sens.h"
#include "../lib/sensors/SensorModuleV1/max3010x-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX3010XV2
#include "../lib/sensors/SensorModuleV1/max3010xv2-sens.h"
#include "../lib/sensors/SensorModuleV1/max3010xv2-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX6675
#include "../lib/sensors/SensorModuleV1/max6675-sens.h"
#include "../lib/sensors/SensorModuleV1/max6675-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX30100LIB
#include "../lib/sensors/SensorModuleV1/max30100lib-sens.h"
#include "../lib/sensors/SensorModuleV1/max30100lib-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MAX31865
#include "../lib/sensors/SensorModuleV1/max31865-sens.h"
#include "../lib/sensors/SensorModuleV1/max31865-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MLX90614
#include "../lib/sensors/SensorModuleV1/mlx90614-sens.h"
#include "../lib/sensors/SensorModuleV1/mlx90614-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MQ
#include "../lib/sensors/SensorModuleV1/mq-sens.h"
#include "../lib/sensors/SensorModuleV1/mq-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_PH
#include "../lib/sensors/SensorModuleV1/ph-sens.h"
#include "../lib/sensors/SensorModuleV1/ph-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_PZEM004T
#include "../lib/sensors/SensorModuleV1/lib/PZEM004t/PZEM004Tc30_custom.h"
#include "../lib/sensors/SensorModuleV1/lib/PZEM004t/PZEM004Tc30_custom.cpp"
#include "../lib/sensors/SensorModuleV1/pzem004t-sens.h"
#include "../lib/sensors/SensorModuleV1/pzem004t-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RFID
#include "../lib/sensors/SensorModuleV1/rfid-sens.h"
#include "../lib/sensors/SensorModuleV1/rfid-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RPM
#include "../lib/sensors/SensorModuleV1/rpm-sens.h"
#include "../lib/sensors/SensorModuleV1/rpm-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RTC
#include "../lib/sensors/SensorModuleV1/rtc-sens.h"
#include "../lib/sensors/SensorModuleV1/rtc-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_SCD30
#include "../lib/sensors/SensorModuleV1/scd30-sens.h"
#include "../lib/sensors/SensorModuleV1/scd30-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_SOIL
#include "../lib/sensors/SensorModuleV1/soil-sens.h"
#include "../lib/sensors/SensorModuleV1/soil-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_SOILPH
#include "../lib/sensors/SensorModuleV1/soilph-sens.h"
#include "../lib/sensors/SensorModuleV1/soilph-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_TCS3200
#include "../lib/sensors/SensorModuleV1/tcs3200-sens.h"
#include "../lib/sensors/SensorModuleV1/tcs3200-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_TDS
#include "../lib/sensors/SensorModuleV1/tds-sens.h"
#include "../lib/sensors/SensorModuleV1/tds-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_TURBIDITY
#include "../lib/sensors/SensorModuleV1/turbidity-sens.h"
#include "../lib/sensors/SensorModuleV1/turbidity-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ULTRASONIC
#include "../lib/sensors/SensorModuleV1/ultrasonic-sens.h"
#include "../lib/sensors/SensorModuleV1/ultrasonic-sens.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_VOLTAGE
#include "../lib/sensors/SensorModuleV1/voltage-sens.h"
#include "../lib/sensors/SensorModuleV1/voltage-sens.cpp"
#endif

// SensorModuleV2
#ifdef ENABLE_HELPER_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/SensorDataInterface.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/SensorDataInterface.cpp"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/CalibrationEngine.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/CalibrationEngine.cpp"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/CalibrationStorage.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/CalibrationStorage.cpp"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/SerialInputHandler.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/SerialInputHandler.cpp"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/CalibrationUI.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/CalibrationUI.cpp"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/InteractiveCalibrator.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/InteractiveCalibrator.cpp"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/InteractiveSerialGeneralSensorCalibratorV2.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Tools/InteractiveSerialGeneralSensorCalibratorV2.cpp"
#endif

#ifdef ENABLE_HELPER_SENSOR_ALERT_SYSTEM_V2
#include "../lib/sensors/SensorModuleV2/SensorModule/Systems/SensorAlertSystemV2.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Systems/SensorAlertSystemV2.cpp"
#endif

#ifdef ENABLE_HELPER_SENSOR_CALIBRATION_MODULE_V2
#include "../lib/sensors/SensorModuleV2/SensorModule/SensorCalibrationModuleV2.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/SensorCalibrationModuleV2.cpp"
#endif

#ifdef ENABLE_HELPER_SENSOR_FILTER_V2
#include "../lib/sensors/SensorModuleV2/SensorModule/Systems/SensorFilterV2.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Systems/SensorFilterV2.cpp"
#endif

#ifdef ENABLE_HELPER_SENSOR_MODULE_V2
#include "../lib/sensors/SensorModuleV2/SensorModule/SensorModuleV2.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/SensorModuleV2.cpp"
#endif

#ifdef ENABLE_HELPER_SENSOR_UTILITY_V2
#include "../lib/sensors/SensorModuleV2/SensorModule/Utils/SensorUtilityV2.h"
#include "../lib/sensors/SensorModuleV2/SensorModule/Utils/SensorUtilityV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ABSTRACT_V2
#include "../lib/sensors/SensorModuleV2/SensorList/AbstractSensV2/AbstractSensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/AbstractSensV2/AbstractSensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_ANALOG_V2
#include "../lib/sensors/SensorModuleV2/SensorList/AnalogSensV2/AnalogSensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/AnalogSensV2/AnalogSensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_BME680_V2
#include "../lib/sensors/SensorModuleV2/SensorList/BME680SensV2/BME680SensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/BME680SensV2/BME680SensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_DHT_V2
#include "../lib/sensors/SensorModuleV2/SensorList/DHTSensV2/DHTSensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/DHTSensV2/DHTSensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_INA219_V2
#include "../lib/sensors/SensorModuleV2/SensorList/INA219SensV2/INA219SensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/INA219SensV2/INA219SensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MLX90614_V2
#include "../lib/sensors/SensorModuleV2/SensorList/MLX90614SensV2/MLX90614SensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/MLX90614SensV2/MLX90614SensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RTC_V2
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/BaseRTCSensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/BaseRTCSensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RTC_DS1307_V2
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCDS1307SensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCDS1307SensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RTC_DS3231_V2
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCDS3231SensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCDS3231SensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RTC_PCF8523_V2
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCPCF8523SensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCPCF8523SensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_RTC_PCF8563_V2
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCPCF8563SensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/RTCSensV2/RTCPCF8563SensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_GP2Y_DUST_V2
#include "../lib/sensors/SensorModuleV2/SensorList/GP2YDustSensV2/GP2YDustSensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/GP2YDustSensV2/GP2YDustSensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MQ_V2
#include "../lib/sensors/SensorModuleV2/SensorList/MQSensV2/MQSensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/MQSensV2/MQSensV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_CUSTOM_TEMPLATE_V2
#include "../lib/sensors/SensorModuleV2/SensorList/CustomSensorTemplateV2/CustomSensorTemplateV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/CustomSensorTemplateV2/CustomSensorTemplateV2.cpp"
#endif

#ifdef ENABLE_SENSOR_HELPER_MHRTC_V2
#include "../lib/sensors/SensorModuleV2/SensorList/MHRTCSensV2/MHRTCSensV2.h"
#include "../lib/sensors/SensorModuleV2/SensorList/MHRTCSensV2/MHRTCSensV2.cpp"
#endif
