#ifndef SERIAL_DEBUGGER_V2_H
#define SERIAL_DEBUGGER_V2_H

#include "Arduino.h"

enum LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    NONE = 4,
    CUSTOM_START = 16
};

typedef uint32_t CustomLogLevel;

const CustomLogLevel DEBUG_LEVEL = (1UL << DEBUG);
const CustomLogLevel INFO_LEVEL = (1UL << INFO);
const CustomLogLevel WARNING_LEVEL = (1UL << WARNING);
const CustomLogLevel ERROR_LEVEL = (1UL << ERROR);
const CustomLogLevel NONE_LEVEL = (1UL << NONE);

inline CustomLogLevel logLevelToCustom(LogLevel level) {
    return (1UL << level);
}

struct DebugFormat {
    String separatorStart;
    String separatorBetween;
    String nameValueSeparator;
    String timeFormat;
    String prefixFormat;
};

struct LogLevelName {
    CustomLogLevel level;
    const char *name;
};

class SerialDebuggerV2 {
private:
    unsigned long _startTime;
    bool _enabled;
    unsigned int _baudRate;
    String _prefix;
    bool _showTimestamp;
    bool _showMillis;
    bool _showFreeMemory;
    LogLevel _minLogLevel;
    CustomLogLevel _enabledLogLevels;
    DebugFormat _format;
    uint8_t _nextCustomLevelIndex;

    LogLevelName *_levelNames;
    uint8_t _levelNamesCount;
    uint8_t _levelNamesCapacity;

    unsigned long _lastDebugTime;
    unsigned int _minDebugInterval;

    char *_buffer;
    size_t _bufferSize;
    size_t _bufferPos;
    bool _bufferingEnabled;

    const char *getLogLevelName(CustomLogLevel level);
    void printHeader(const String &header, CustomLogLevel level);
    void printFooter();
    void flushBuffer();
    void bufferWrite(const char *data);
    void bufferWrite(const String &data);

    bool shouldPrint(CustomLogLevel level);
    bool throttleCheck();

public:
    explicit SerialDebuggerV2(unsigned int baudRate = 115200, size_t bufferSize = 256);
    ~SerialDebuggerV2();

    void begin();
    void enable();
    void disable();
    bool isEnabled() const;

    void setPrefix(const String &prefix);
    void showTimestamp(bool show);
    void showMillis(bool show);
    void showFreeMemory(bool show);

    CustomLogLevel createLogLevel(const char *name = nullptr);
    void useUserDefinedLogLevels();

    void setLogLevel(LogLevel level);
    LogLevel getLogLevel();

    void enableLogLevel(CustomLogLevel level);
    void disableLogLevel(CustomLogLevel level);
    bool isLogLevelEnabled(CustomLogLevel level);
    void setEnabledLogLevels(CustomLogLevel levelsMask);
    CustomLogLevel getEnabledLogLevels();

    void registerLogLevelName(CustomLogLevel level, const char *name);

    void setFormat(const DebugFormat &format);
    static DebugFormat getDefaultFormat();
    void resetFormat();

    void enableBuffering(bool enable);
    void setBufferSize(size_t size);
    void setMinDebugInterval(unsigned int millisInterval);

    template<typename T>
    void printValue(const String &name, T value, CustomLogLevel level = DEBUG_LEVEL) {
        if (!_enabled || !throttleCheck() || !shouldPrint(level)) return;

        String output = _format.separatorBetween + name + _format.nameValueSeparator + String(value);

        if (_bufferingEnabled) {
            bufferWrite(output);
        } else {
            Serial.print(output);
        }
    }

    template<typename T>
    void print(const String &name, T value, CustomLogLevel level = DEBUG_LEVEL, bool addNewLine = false) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name, value, level);
        printFooter();

        if (addNewLine) {
            if (_bufferingEnabled) {
                bufferWrite("\n");
                flushBuffer();
            } else {
                Serial.println();
            }
        }
    }

    template<typename T1, typename T2>
    void print(const String &name1, T1 value1, const String &name2, T2 value2, CustomLogLevel level = DEBUG_LEVEL, bool addNewLine = false) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name1, value1, level);
        printValue(name2, value2, level);
        printFooter();

        if (addNewLine) {
            if (_bufferingEnabled) {
                bufferWrite("\n");
                flushBuffer();
            } else {
                Serial.println();
            }
        }
    }

    template<typename T1, typename T2, typename T3>
    void print(const String &name1, T1 value1, const String &name2, T2 value2, const String &name3, T3 value3, CustomLogLevel level = DEBUG_LEVEL, bool addNewLine = false) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name1, value1, level);
        printValue(name2, value2, level);
        printValue(name3, value3, level);
        printFooter();

        if (addNewLine) {
            if (_bufferingEnabled) {
                bufferWrite("\n");
                flushBuffer();
            } else {
                Serial.println();
            }
        }
    }

    template<typename T1, typename T2, typename T3, typename T4>
    void print(const String &name1, T1 value1, const String &name2, T2 value2, const String &name3, T3 value3, const String &name4, T4 value4, CustomLogLevel level = DEBUG_LEVEL, bool addNewLine = false) {
        if (!shouldPrint(level)) return;

        printHeader("", level);
        printValue(name1, value1, level);
        printValue(name2, value2, level);
        printValue(name3, value3, level);
        printValue(name4, value4, level);
        printFooter();

        if (addNewLine) {
            if (_bufferingEnabled) {
                bufferWrite("\n");
                flushBuffer();
            } else {
                Serial.println();
            }
        }
    }

    void startPrint(CustomLogLevel level = DEBUG_LEVEL) {
        if (!shouldPrint(level)) return;
        printHeader("", level);
    }

    template<typename T>
    void continuePrint(const String &name, T value, CustomLogLevel level = DEBUG_LEVEL) {
        if (!shouldPrint(level)) return;
        printValue(name, value, level);
    }

    template<typename T>
    void continuePrintArray(const String &name, T *array, size_t size, CustomLogLevel level = DEBUG_LEVEL) {
        if (!shouldPrint(level)) return;

        String output = _format.separatorBetween + name + "[" + String(size) + "]: {";

        if (_bufferingEnabled) {
            bufferWrite(output);
        } else {
            Serial.print(output);
        }

        for (size_t i = 0; i < size; i++) {
            if (_bufferingEnabled) {
                bufferWrite(String(array[i]));
            } else {
                Serial.print(array[i]);
            }

            if (i < size - 1) {
                if (_bufferingEnabled) {
                    bufferWrite(", ");
                } else {
                    Serial.print(", ");
                }
            }
        }

        if (_bufferingEnabled) {
            bufferWrite("}");
        } else {
            Serial.print("}");
        }
    }

    template<typename T>
    void continuePrintMatrix(const String &name, T **matrix, size_t rows, size_t cols, CustomLogLevel level = DEBUG_LEVEL) {
        if (!shouldPrint(level)) return;

        String output = _format.separatorBetween + name + "[" + String(rows) + "][" + String(cols) + "]: {";

        if (_bufferingEnabled) {
            bufferWrite(output);
        } else {
            Serial.print(output);
        }

        for (size_t i = 0; i < rows; i++) {
            if (_bufferingEnabled) {
                bufferWrite("{");
            } else {
                Serial.print("{");
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

            if (i < rows - 1) {
                if (_bufferingEnabled) {
                    bufferWrite("}, ");
                } else {
                    Serial.print("}, ");
                }
            } else {
                if (_bufferingEnabled) {
                    bufferWrite("}");
                } else {
                    Serial.print("}");
                }
            }
        }

        if (_bufferingEnabled) {
            bufferWrite("}");
        } else {
            Serial.print("}");
        }
    }

    void endPrint(CustomLogLevel level = DEBUG_LEVEL, bool addNewLine = false);

    template<typename T>
    void printArray(const String &name, T *array, size_t size, CustomLogLevel level = DEBUG_LEVEL, bool addNewLine = false) {
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

        if (addNewLine) {
            if (_bufferingEnabled) {
                bufferWrite("\n");
                flushBuffer();
            } else {
                Serial.println();
            }
        }
    }

    template<typename T>
    void printMatrix(const String &name, T **matrix, size_t rows, size_t cols, CustomLogLevel level = DEBUG_LEVEL) {
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

    void section(const String &title, CustomLogLevel level = DEBUG_LEVEL);
    void startSection(const String &title, CustomLogLevel level = DEBUG_LEVEL);
    void endSection(CustomLogLevel level = DEBUG_LEVEL);

    void separator(CustomLogLevel level = DEBUG_LEVEL);
    void newLine(CustomLogLevel level = DEBUG_LEVEL);

    void debug(const String &message, bool addNewLine = false);
    void info(const String &message, bool addNewLine = false);
    void warning(const String &message, bool addNewLine = false);
    void error(const String &message, bool addNewLine = false);
    void success(const String &message, bool addNewLine = false);

    void log(const String &message, CustomLogLevel level, bool addNewLine = false);

    void hexDump(const void *data, size_t size, CustomLogLevel level = DEBUG_LEVEL);

    void memoryInfo(CustomLogLevel level = INFO_LEVEL);
    void systemInfo(CustomLogLevel level = INFO_LEVEL);

    void startTimer();
    void logExecutionTime(const String &operation, CustomLogLevel level = DEBUG_LEVEL);

    void clear();

    static CustomLogLevel SENSOR;
    static CustomLogLevel NETWORK;
    static CustomLogLevel MEMORY;
    static CustomLogLevel TIMING;
    static CustomLogLevel IO;
};

CustomLogLevel SerialDebuggerV2::SENSOR = (1UL << (CUSTOM_START));
CustomLogLevel SerialDebuggerV2::NETWORK = (1UL << (CUSTOM_START + 1));
CustomLogLevel SerialDebuggerV2::MEMORY = (1UL << (CUSTOM_START + 2));
CustomLogLevel SerialDebuggerV2::TIMING = (1UL << (CUSTOM_START + 3));
CustomLogLevel SerialDebuggerV2::IO = (1UL << (CUSTOM_START + 4));

#endif