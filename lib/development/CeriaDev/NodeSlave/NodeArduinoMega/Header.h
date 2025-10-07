#define ENABLE_DEVELOPMENT_CERIA_DEV_OP
#define ENABLE_MODULE_SERIAL_ENHANCED
#include "Kinematrix.h"

using namespace CeriaDevOP;

EnhancedSerial usbSerial;

CeriaSensorGPS gpsSensor;

CeriaSerialI2C i2cSlave;
const uint8_t MY_SLAVE_ADDRESS = 0x08;

struct GPSSensorData {
  double latitude;
  double longitude;
  double speed;
  double altitude;
  uint8_t satellites;
  String date;
  String time;
};

GPSSensorData gps;

bool debugEnable = false;