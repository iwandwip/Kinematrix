#define ENABLE_DEVELOPMENT_CERIA_DEV_OP
#define ENABLE_MODULE_SERIAL_ENHANCED
#include "Kinematrix.h"

using namespace CeriaDevOP;

EnhancedSerial usbSerial;

CeriaModuleESP32SDCard sdCard;

CeriaModuleIO buttonPCF(0x20);
CeriaSensorDS3231 rtcSensor;

CeriaSensorINA219 powerSensor(0x40);
CeriaSensorINA219 batterySensor(0x41);
CeriaSensorGPS gpsSensor;

CeriaSerialI2C i2cMaster;
const uint8_t ARDUINO_MEGA_SLAVE_ADDR = 0x08;

struct INA219SensorData {
  float voltage;
  float current;
  float power;
};

INA219SensorData power;
INA219SensorData battery;


bool debugEnable = false;