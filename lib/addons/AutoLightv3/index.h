#pragma once
#ifndef INDEX_H
#define INDEX_H

#if defined(ESP32)

#include "Arduino.h"

#include "Channel/BaseChannel.h"
#include "Channel/PatternHelper.h"
#include "Channel/ChainHelper.h"
#include "Channel/SimplePatterns.h"
#include "Channel/BaseChannel.cpp"
#include "Channel/BaseChannelSequence.cpp"
#include "Channel/PatternHelper.cpp"
#include "Channel/ChainHelper.cpp"
#include "Channel/SimplePatterns.cpp"
#include "Channel/PatternEngineTest.cpp"
#include "Channel/Indicator.h"
#include "Channel/Indicator.cpp"
#include "Common/Constants.h"
#include "Common/VirtualPCF.h"
#include "Common/VirtualPCF.cpp"
#include "Config/ConfigData.h"
#include "Config/BaseConfig.h"
#include "Config/BaseConfig.cpp"
#include "Config/ConfigManager.h"
#include "Config/ConfigManager.cpp"
#include "Cores/Task.h"
#include "Cores/Task.cpp"
#include "Web/WebManager.h"
#include "Web/WebManager.cpp"
#include "ButtonInterrupt.h"
#include "ButtonInterrupt.cpp"
#include "Visualization/LEDMatrixVisualizer.h"
#include "Visualization/LEDMatrixVisualizer.cpp"
#include "SerialCommander.h"
#include "SerialCommander.cpp"

#endif

#endif