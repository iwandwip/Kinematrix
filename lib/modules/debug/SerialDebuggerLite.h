#ifndef SERIAL_DEBUGGER_LITE_H
#define SERIAL_DEBUGGER_LITE_H

#include <Arduino.h>

#define DEBUG_LEVEL_NONE 0
#define DEBUG_LEVEL_ERROR 1
#define DEBUG_LEVEL_WARNING 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_DEBUG 4

class SerialDebuggerLite {
private:
    uint8_t _enabled;
    uint8_t _logLevel;
    uint32_t _startTime;
    uint32_t _lastDebugTime;
    uint16_t _minDebugInterval;
    char _prefix[10];
    uint8_t _showMillis;

    void printHeader(const char *header, uint8_t level);
    void printFooter();

    uint8_t shouldPrint(uint8_t level);
    uint8_t throttleCheck();

public:
    SerialDebuggerLite();

    void begin(uint32_t baudRate);
    void enable();
    void disable();
    uint8_t isEnabled();

    void setPrefix(const char *prefix);
    void showMillis(uint8_t show);

    void setLogLevel(uint8_t level);
    void setMinDebugInterval(uint16_t millisInterval);

    template<typename T>
    void printValue(const char *name, T value) {
        if (!_enabled) return;

        Serial.print(F(" | "));
        Serial.print(name);
        Serial.print(F(": "));
        Serial.print(value);
    }

    template<typename T>
    void print(const char *name, T value, uint8_t level = DEBUG_LEVEL_DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name, value);
        printFooter();
    }

    template<typename T1, typename T2>
    void print(const char *name1, T1 value1, const char *name2, T2 value2, uint8_t level = DEBUG_LEVEL_DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name1, value1);
        printValue(name2, value2);
        printFooter();
    }

    template<typename T>
    void printArray(const char *name, T *array, uint8_t size, uint8_t level = DEBUG_LEVEL_DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        Serial.print(F(" | "));
        Serial.print(name);
        Serial.print(F("["));
        Serial.print(size);
        Serial.print(F("]: {"));

        for (uint8_t i = 0; i < size; i++) {
            Serial.print(array[i]);
            if (i < size - 1) {
                Serial.print(F(", "));
            }
        }

        Serial.print(F("}"));
        printFooter();
    }

    void println();

    void section(const char *title, uint8_t level = DEBUG_LEVEL_DEBUG);
    void separator(uint8_t level = DEBUG_LEVEL_DEBUG);

    void debug(const char *message);
    void info(const char *message);
    void warning(const char *message);
    void error(const char *message);

    void hexDump(const void *data, uint8_t size, uint8_t level = DEBUG_LEVEL_DEBUG);

    void startTimer();
    void logExecutionTime(const char *operation, uint8_t level = DEBUG_LEVEL_DEBUG);
};

#endif