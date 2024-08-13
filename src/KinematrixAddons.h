/*
 *  KinematrixAddons.h
 *
 *  KinematrixAddons lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#include "Arduino.h"

#ifdef ENABLE_ADDONS_AUTOLIGHT
#include "../lib/addons/AutoLight.h"
#endif

#ifdef ENABLE_ADDONS_AUTOLIGHT_V2
#include "../lib/addons/AutoLightv2.h"
#endif