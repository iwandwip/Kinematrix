/*
 *  index.h - AutoLightV4 Component Orchestration
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#ifndef AUTOLIGHT_V4_INDEX_H
#define AUTOLIGHT_V4_INDEX_H

#if defined(ESP32)

#include "Arduino.h"

#include "Config/ConfigData.h"
#include "Config/BaseConfig.h"
#include "Config/BaseConfig.cpp"

#include "Channel/BaseChannel.h"
#include "Channel/BaseChannel.cpp"
#include "Channel/BaseChannelSequence.cpp"

#endif

#endif // AUTOLIGHT_V4_INDEX_H