#ifndef SERIAL_DEBUGGER_H
#define SERIAL_DEBUGGER_H

#include "Arduino.h"

enum LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    NONE = 4
};

struct DebugFormat {
    String separatorStart;
    String separatorBetween;
    String nameValueSeparator;
    String timeFormat;
    String prefixFormat;
};

class SerialDebugger {
private:
    unsigned long _startTime;
    bool _enabled;
    unsigned int _baudRate;
    String _prefix;
    bool _showTimestamp;
    bool _showMillis;
    bool _showFreeMemory;
    LogLevel _logLevel;
    DebugFormat _format;

    unsigned long _lastDebugTime;
    unsigned int _minDebugInterval;

    char *_buffer;
    size_t _bufferSize;
    size_t _bufferPos;
    bool _bufferingEnabled;

    void printHeader(const String &header, LogLevel level);
    void printFooter();
    void flushBuffer();
    void bufferWrite(const char *data);
    void bufferWrite(const String &data);

    bool shouldPrint(LogLevel level);
    bool throttleCheck();

public:
    explicit SerialDebugger(unsigned int baudRate = 115200, size_t bufferSize = 256);
    ~SerialDebugger();

    void begin();
    void enable();
    void disable();
    bool isEnabled() const;

    void setPrefix(const String &prefix);
    void showTimestamp(bool show);
    void showMillis(bool show);
    void showFreeMemory(bool show);

    void setLogLevel(LogLevel level);
    LogLevel getLogLevel();

    void setFormat(const DebugFormat &format);
    static DebugFormat getDefaultFormat();
    void resetFormat();

    void enableBuffering(bool enable);
    void setBufferSize(size_t size);
    void setMinDebugInterval(unsigned int millisInterval);

    template<typename T>
    void printValue(const String &name, T value) {
        if (!_enabled || !throttleCheck()) return;

        String output = _format.separatorBetween + name + _format.nameValueSeparator + String(value);

        if (_bufferingEnabled) {
            bufferWrite(output);
        } else {
            Serial.print(output);
        }
    }

    template<typename T>
    void print(const String &name, T value, LogLevel level = DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name, value);
        printFooter();
    }

    template<typename T1, typename T2>
    void print(const String &name1, T1 value1, const String &name2, T2 value2, LogLevel level = DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name1, value1);
        printValue(name2, value2);
        printFooter();
    }

    template<typename T1, typename T2, typename T3>
    void print(const String &name1, T1 value1, const String &name2, T2 value2, const String &name3, T3 value3, LogLevel level = DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name1, value1);
        printValue(name2, value2);
        printValue(name3, value3);
        printFooter();
    }

    template<typename T1, typename T2, typename T3, typename T4>
    void print(const String &name1, T1 value1, const String &name2, T2 value2, const String &name3, T3 value3, const String &name4, T4 value4, LogLevel level = DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name1, value1);
        printValue(name2, value2);
        printValue(name3, value3);
        printValue(name4, value4);
        printFooter();
    }

    template<typename T>
    void printArray(const String &name, T *array, size_t size, LogLevel level = DEBUG) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        Serial.print(_format.separatorBetween + name + "[" + String(size) + "]: {");

        for (size_t i = 0; i < size; i++) {
            Serial.print(array[i]);
            if (i < size - 1) {
                Serial.print(", ");
            }
        }

        Serial.print("}");
        printFooter();
    }

    template<typename T>
    void printMatrix(const String &name, T **matrix, size_t rows, size_t cols, LogLevel level = DEBUG) {
        if (!shouldPrint(level)) return;

        section(name + " Matrix[" + String(rows) + "][" + String(cols) + "]", level);

        for (size_t i = 0; i < rows; i++) {
            if (_bufferingEnabled) {
                bufferWrite("  [");
            } else {
                Serial.print("  [");
            }

            for (size_t j = 0; j < cols; j++) {
                if (_bufferingEnabled) {
                    bufferWrite(String(matrix[i][j]));
                } else {
                    Serial.print(matrix[i][j]);
                }

                if (j < cols - 1) {
                    if (_bufferingEnabled) {
                        bufferWrite(", ");
                    } else {
                        Serial.print(", ");
                    }
                }
            }

            if (_bufferingEnabled) {
                bufferWrite("]\n");
            } else {
                Serial.println("]");
            }
        }

        if (_bufferingEnabled) {
            flushBuffer();
        }
    }

    void println();

    void section(const String &title, LogLevel level = DEBUG);
    void startSection(const String &title, LogLevel level = DEBUG);
    void endSection(LogLevel level = DEBUG);

    void separator(LogLevel level = DEBUG);
    void newLine(LogLevel level = DEBUG);

    void debug(const String &message);
    void info(const String &message);
    void warning(const String &message);
    void error(const String &message);
    void success(const String &message);

    void hexDump(const void *data, size_t size, LogLevel level = DEBUG);

    void memoryInfo(LogLevel level = INFO);
    void systemInfo(LogLevel level = INFO);

    void startTimer();
    void logExecutionTime(const String &operation, LogLevel level = DEBUG);

    void clear();
};

#endif