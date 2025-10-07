/*
 *  KinematrixAddons.h
 *
 *  KinematrixAddons lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#include "Arduino.h"

#ifdef ENABLE_ADDONS_AUTOLIGHT
#include "../lib/addons/AutoLight/index.h"
#endif

#ifdef ENABLE_ADDONS_AUTOLIGHT_V1
#include "../lib/addons/AutoLightv1/index.h"
#endif

#ifdef ENABLE_ADDONS_AUTOLIGHT_V2
#include "../lib/addons/AutoLightv2/index.h"
#endif

#ifdef ENABLE_ADDONS_AUTOLIGHT_V3
#include "../lib/addons/AutoLightv3/index.h"
#endif