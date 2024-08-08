/*
 *  sensor-debug.h
 *
 *  sensor debug header
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SENSOR_DEBUG_H
#define SENSOR_DEBUG_H

#include "Arduino.h"

#pragma message("[COMPILED]: sensor-debug.h")

class Debug {
private:
public:
    template<typename T>
    Debug &print(T str) {
        Serial.print(str);
        return *this;
    }

    template<typename T>
    Debug &println(T str) {
        Serial.println(str);
        return *this;
    }

    Debug &println() {
        Serial.println();
        return *this;
    }

    static void printf(const char *format, ...) {
        va_list args;
        va_start(args, format);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        Serial.print(buffer);
        va_end(args);
    }
};

#endif  // SENSOR_DEBUG_H