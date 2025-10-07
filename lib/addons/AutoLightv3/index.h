#pragma once
#ifndef INDEX_H
#define INDEX_H

#if defined(ESP32)

#include "Arduino.h"

#include "Firmware/Core/Channel/BaseChannel/BaseChannel.h"
#include "Firmware/Core/Channel/BaseChannel/BaseChannel.cpp"
#include "Firmware/Core/Channel/BaseChannel/BaseChannelSequence.cpp"
#include "Firmware/Core/Channel/Indicator.h"
#include "Firmware/Core/Channel/Indicator.cpp"
#include "Firmware/Core/Common/Constants.h"
#include "Firmware/Core/Config/ConfigData.h"
#include "Firmware/Core/Config/BaseConfig.h"
#include "Firmware/Core/Config/BaseConfig.cpp"
#include "Firmware/Core/Config/ConfigManager.h"
#include "Firmware/Core/Config/ConfigManager.cpp"
#include "Firmware/Core/Cores/Task.h"
#include "Firmware/Core/Cores/Task.cpp"
#include "Firmware/API/APIServerManager.h"
#include "Firmware/API/APIServerManager.cpp"
#include "Firmware/Core/Interaction/ButtonInterrupt.h"
#include "Firmware/Core/Interaction/ButtonInterrupt.cpp"
#include "Firmware/Core/Interaction/SerialCommander.h"
#include "Firmware/Core/Interaction/SerialCommander.cpp"

#endif

#endif