/*
 *  sensor-header.h
 *
 *  sensor header
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SENSOR_HEADER_H
#define SENSOR_HEADER_H

#include "Arduino.h"

#if !defined(ESP32)
#ifdef __cplusplus
extern "C" {
#endif

int freeMemory();

#ifdef  __cplusplus
}
#endif
#endif

#endif  // SENSOR_HEADER_H