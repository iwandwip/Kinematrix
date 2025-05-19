/*
 *  sensor-header.cpp
 *
 *  sensor header c
 *  Created on: 2023. 4. 3
 */

extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

#include "sensor-header.h"

#if !defined(ESP32)

int freeMemory() {
    int free_memory;

    if ((int) __brkval == 0)
        free_memory = ((int) &free_memory) - ((int) &__bss_end);
    else
        free_memory = ((int) &free_memory) - ((int) __brkval);

    return free_memory;
}

#endif

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}