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
    bool begin(uint8_t _cs_pin, void (*beginCallback)() = nullptr);
    bool exists(const char *_file_name);
    void createFiles(const char *_file_name, bool debug = false);
    bool deleteFiles(const char *_file_name, bool debug = false);
    void printDir(File dir, int numTabs);
    void listDir(int numTabs = 0, bool debug = false);
    void writeFiles(const char *_file_name, const char *_text, bool debug = false);
    void appendFiles(const char *_file_name, const char *_text, bool debug = false);
    void emptyFiles(const char *_file_name, bool debug = false);
    void readFiles(const char *_file_name, bool debug = false, void (*readCallback)(int next) = nullptr
    );
    String readFilesString(const char *_file_name, bool debug = false);
};

#endif


#endif // SD_ARDUINO_H