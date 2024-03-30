/*
 *  SDArduino.h
 *
 *  SDArduino lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef SD_ARDUINO_H
#define SD_ARDUINO_H

#if defined(ESP32)
#else

#include "Arduino.h"
#include "SD.h"

class SDArduino {
private:
    File file;
public:
    SDArduino();
    virtual ~SDArduino();
    bool begin(uint8_t _cs_pin);
    bool exists(const char *_file_name);
    void createFiles(const char *_file_name);
    bool deleteFiles(const char *_file_name);
    void printDir(File dir, int numTabs);
    void listDir(int numTabs = 0);
    void writeFiles(const char *_file_name, const char *_text);
    void appendFiles(const char *_file_name, const char *_text);
    void readFiles(const char *_file_name);
};

#endif


#endif // SD_ARDUINO_H