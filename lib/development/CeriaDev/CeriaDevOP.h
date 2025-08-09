#pragma once

#ifndef CERIA_DEV_OP_H
#define CERIA_DEV_OP_H

#pragma message("[COMPILED]: CeriaDev/CeriaDevOP.h - Complete CeriaDev Library")

#include "lib/CeriaSerial.h"
#include "lib/CeriaSerial.cpp"

#include "lib/CeriaSensorGPS.h"
#include "lib/CeriaSensorGPS.cpp"

#include "lib/CeriaSensorINA219.h"
#include "lib/CeriaSensorINA219.cpp"

#include "lib/CeriaDisplayDWIN.h"
#include "lib/CeriaDisplayDWIN.cpp"

#include "lib/CeriaModulePCF8574.h"
#include "lib/CeriaModulePCF8574.cpp"

#include "lib/CeriaModuleIO.h"
#include "lib/CeriaModuleIO.cpp"

#include "lib/CeriaSensorAnalog.h"
#include "lib/CeriaSensorAnalog.cpp"

#include "lib/CeriaSensorDS3231.h"
#include "lib/CeriaSensorDS3231.cpp"

#include "lib/CeriaModuleESP32NTP.h"
#include "lib/CeriaModuleESP32NTP.cpp"

#include "lib/CeriaModuleESP32SDCard.h"
#include "lib/CeriaModuleESP32SDCard.cpp"

#include "lib/CeriaModuleESP32RTOS.h"
#include "lib/CeriaModuleESP32RTOS.cpp"

#include "lib/CeriaSerialI2C.h"
#include "lib/CeriaSerialI2C.cpp"

using namespace CeriaDevOP;

#endif