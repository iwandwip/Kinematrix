/*
 *  Index.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#pragma once
#ifndef INDEX_H
#define INDEX_H

#if defined(ESP32)

#include "Arduino.h"

#include "Channel/BaseChannel.h"
#include "Channel/BaseChannel.cpp"
#include "Channel/BaseChannelSequence.cpp"
#include "Channel/Indicator.h"
#include "Channel/Indicator.cpp"
#include "Common/Constants.h"
#include "Config/ConfigData.h"
#include "Config/BaseConfig.h"
#include "Config/BaseConfig.cpp"
#include "Cores/Task.h"
#include "Cores/Task.cpp"
#include "Web/WebManager.h"
#include "Web/WebManager.cpp"
#include "ButtonInterrupt.h"
#include "ButtonInterrupt.cpp"

#endif

#endif // INDEX_H
