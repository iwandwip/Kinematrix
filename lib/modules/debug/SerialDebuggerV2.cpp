#include "SerialDebuggerV2.h"

SerialDebuggerV2::SerialDebuggerV2(unsigned int baudRate, size_t bufferSize) {
    _baudRate = baudRate;
    _enabled = true;
    _showTimestamp = false;
    _showMillis = true;
    _showFreeMemory = false;
    _prefix = "";
    _startTime = 0;
    _minLogLevel = DEBUG;
    _enabledLogLevels = 0x0000FFFF;
    _lastDebugTime = 0;
    _minDebugInterval = 0;
    _nextCustomLevelIndex = CUSTOM_START;

    _bufferSize = bufferSize;
    _buffer = new char[_bufferSize];
    _bufferPos = 0;
    _bufferingEnabled = false;

    _levelNamesCapacity = 20;
    _levelNames = new LogLevelName[_levelNamesCapacity];
    _levelNamesCount = 0;

    registerLogLevelName(DEBUG_LEVEL, "DEBUG");
    registerLogLevelName(INFO_LEVEL, "INFO");
    registerLogLevelName(WARNING_LEVEL, "WARNING");
    registerLogLevelName(ERROR_LEVEL, "ERROR");

    registerLogLevelName(SENSOR, "SENSOR");
    registerLogLevelName(NETWORK, "NETWORK");
    registerLogLevelName(MEMORY, "MEMORY");
    registerLogLevelName(TIMING, "TIMING");
    registerLogLevelName(IO, "IO");

    resetFormat();
}

SerialDebuggerV2::~SerialDebuggerV2() {
    delete[] _buffer;
    delete[] _levelNames;
}

void SerialDebuggerV2::begin() {
    Serial.begin(_baudRate);
    while (!Serial) {
        delay(10);
    }
    _startTime = millis();
}

void SerialDebuggerV2::enable() {
    _enabled = true;
}

void SerialDebuggerV2::disable() {
    _enabled = false;
}

bool SerialDebuggerV2::isEnabled() const {
    return _enabled;
}

void SerialDebuggerV2::setPrefix(const String &prefix) {
    _prefix = prefix;
}

void SerialDebuggerV2::showTimestamp(bool show) {
    _showTimestamp = show;
}

void SerialDebuggerV2::showMillis(bool show) {
    _showMillis = show;
}

void SerialDebuggerV2::showFreeMemory(bool show) {
    _showFreeMemory = show;
}

CustomLogLevel SerialDebuggerV2::createLogLevel(const char *name) {
    CustomLogLevel newLevel = (1UL << _nextCustomLevelIndex);
    _nextCustomLevelIndex++;

    if (name != nullptr) {
        registerLogLevelName(newLevel, name);
    }

    return newLevel;
}

void SerialDebuggerV2::useUserDefinedLogLevels() {
    _nextCustomLevelIndex = CUSTOM_START + 10;
}

void SerialDebuggerV2::setLogLevel(LogLevel level) {
    _minLogLevel = level;

    _enabledLogLevels &= ~0x0000FFFF;
    for (int i = level; i <= NONE; i++) {
        _enabledLogLevels |= (1UL << i);
    }
}

LogLevel SerialDebuggerV2::getLogLevel() {
    return _minLogLevel;
}

void SerialDebuggerV2::enableLogLevel(CustomLogLevel level) {
    _enabledLogLevels |= level;
}

void SerialDebuggerV2::disableLogLevel(CustomLogLevel level) {
    _enabledLogLevels &= ~level;
}

bool SerialDebuggerV2::isLogLevelEnabled(CustomLogLevel level) {
    return (_enabledLogLevels & level) != 0;
}

void SerialDebuggerV2::setEnabledLogLevels(CustomLogLevel levelsMask) {
    _enabledLogLevels = levelsMask;
}

CustomLogLevel SerialDebuggerV2::getEnabledLogLevels() {
    return _enabledLogLevels;
}

void SerialDebuggerV2::registerLogLevelName(CustomLogLevel level, const char *name) {
    if (_levelNamesCount >= _levelNamesCapacity) {
        _levelNamesCapacity *= 2;
        LogLevelName *newArray = new LogLevelName[_levelNamesCapacity];

        for (uint8_t i = 0; i < _levelNamesCount; i++) {
            newArray[i] = _levelNames[i];
        }

        delete[] _levelNames;
        _levelNames = newArray;
    }

    _levelNames[_levelNamesCount].level = level;
    _levelNames[_levelNamesCount].name = name;
    _levelNamesCount++;
}

void SerialDebuggerV2::setFormat(const DebugFormat &format) {
    _format = format;
}

DebugFormat SerialDebuggerV2::getDefaultFormat() {
    DebugFormat format;
    format.separatorStart = "";
    format.separatorBetween = " | ";
    format.nameValueSeparator = ": ";
    format.timeFormat = "[%02d:%02d:%02d]";
    format.prefixFormat = "%s ";
    return format;
}

void SerialDebuggerV2::resetFormat() {
    _format = getDefaultFormat();
}

void SerialDebuggerV2::enableBuffering(bool enable) {
    if (enable && !_bufferingEnabled) {
        _bufferPos = 0;
    } else if (!enable && _bufferingEnabled) {
        flushBuffer();
    }
    _bufferingEnabled = enable;
}

void SerialDebuggerV2::setBufferSize(size_t size) {
    if (_bufferingEnabled) {
        flushBuffer();
    }

    delete[] _buffer;
    _bufferSize = size;
    _buffer = new char[_bufferSize];
    _bufferPos = 0;
}

void SerialDebuggerV2::setMinDebugInterval(unsigned int millisInterval) {
    _minDebugInterval = millisInterval;
}

void SerialDebuggerV2::endPrint(CustomLogLevel level, bool addNewLine) {
    if (!shouldPrint(level)) return;
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

void SerialDebuggerV2::println() {
    if (!_enabled || !throttleCheck()) return;

    if (_bufferingEnabled) {
        bufferWrite("\n");
        flushBuffer();
    } else {
        Serial.println();
    }
}

void SerialDebuggerV2::section(const String &title, CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    println();

    String output = "===== " + title + " =====";

    if (_bufferingEnabled) {
        bufferWrite(output + "\n");
        flushBuffer();
    } else {
        Serial.println(output);
    }
}

void SerialDebuggerV2::startSection(const String &title, CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    println();

    String output = "===== START: " + title + " =====";

    if (_bufferingEnabled) {
        bufferWrite(output + "\n");
        flushBuffer();
    } else {
        Serial.println(output);
    }
}

void SerialDebuggerV2::endSection(CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    String output = "===== END =====";

    if (_bufferingEnabled) {
        bufferWrite(output + "\n");
        flushBuffer();
    } else {
        Serial.println(output);
    }
}

void SerialDebuggerV2::separator(CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    String output = "---------------------------";

    if (_bufferingEnabled) {
        bufferWrite(output + "\n");
        flushBuffer();
    } else {
        Serial.println(output);
    }
}

void SerialDebuggerV2::newLine(CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    if (_bufferingEnabled) {
        bufferWrite("\n");
        flushBuffer();
    } else {
        Serial.println();
    }
}

void SerialDebuggerV2::debug(const String &message, bool addNewLine) {
    log(message, DEBUG_LEVEL, addNewLine);
}

void SerialDebuggerV2::info(const String &message, bool addNewLine) {
    log(message, INFO_LEVEL, addNewLine);
}

void SerialDebuggerV2::warning(const String &message, bool addNewLine) {
    log(message, WARNING_LEVEL, addNewLine);
}

void SerialDebuggerV2::error(const String &message, bool addNewLine) {
    log(message, ERROR_LEVEL, addNewLine);
}

void SerialDebuggerV2::success(const String &message, bool addNewLine) {
    log(message, INFO_LEVEL, addNewLine);
}

void SerialDebuggerV2::log(const String &message, CustomLogLevel level, bool addNewLine) {
    if (!shouldPrint(level)) return;

    printHeader("", level);

    if (_bufferingEnabled) {
        bufferWrite(message);
    } else {
        Serial.print(message);
    }

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

void SerialDebuggerV2::hexDump(const void *data, size_t size, CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    const auto *bytes = (const unsigned char *) data;
    char buffer[80];
    char ascii[17];

    printHeader("[HEX DUMP]", level);

    if (_bufferingEnabled) {
        bufferWrite("\n");
    } else {
        Serial.println();
    }

    for (size_t i = 0; i < size; i += 16) {
        sprintf(buffer, "%04X: ", i);

        if (_bufferingEnabled) {
            bufferWrite(buffer);
        } else {
            Serial.print(buffer);
        }

        for (size_t j = 0; j < 16; j++) {
            if (i + j < size) {
                sprintf(buffer, "%02X ", bytes[i + j]);

                if (_bufferingEnabled) {
                    bufferWrite(buffer);
                } else {
                    Serial.print(buffer);
                }

                if (bytes[i + j] >= 32 && bytes[i + j] <= 126) {
                    ascii[j] = bytes[i + j];
                } else {
                    ascii[j] = '.';
                }
            } else {
                if (_bufferingEnabled) {
                    bufferWrite("   ");
                } else {
                    Serial.print("   ");
                }
                ascii[j] = ' ';
            }
        }

        ascii[16] = '\0';

        if (_bufferingEnabled) {
            bufferWrite(" | ");
            bufferWrite(ascii);
            bufferWrite("\n");
        } else {
            Serial.print(" | ");
            Serial.println(ascii);
        }
    }

    if (_bufferingEnabled) {
        flushBuffer();
    }
}

void SerialDebuggerV2::memoryInfo(CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    section("MEMORY INFO", level);

#if defined(ARDUINO_ARCH_ESP8266)
    print("Free Heap", ESP.getFreeHeap(), "Total Heap", ESP.getHeapSize(), level);
    print("Free Sketch Space", ESP.getFreeSketchSpace(), "Sketch Size", ESP.getSketchSize(), level);
    print("Free Cont Stack", ESP.getFreeContStack(), "Heap Fragmentation", ESP.getHeapFragmentation(), level);
#elif defined(ARDUINO_ARCH_ESP32)
    print("Free Heap", ESP.getFreeHeap(), "Total Heap", ESP.getHeapSize(), level);
    print("Free Sketch Space", ESP.getFreeSketchSpace(), "Sketch Size", ESP.getSketchSize(), level);
    print("Min Free Heap", ESP.getMinFreeHeap(), "Max Alloc Heap", ESP.getMaxAllocHeap(), level);
#else
    if (_bufferingEnabled) {
        bufferWrite("Memory info not available for this board\n");
        flushBuffer();
    } else {
        Serial.println("Memory info not available for this board");
    }
#endif
}

void SerialDebuggerV2::systemInfo(CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    section("SYSTEM INFO", level);

#if defined(ARDUINO_ARCH_ESP8266)
    print("Chip ID", ESP.getChipId(), "Core Version", ESP.getCoreVersion(), level);
    print("CPU Freq MHz", ESP.getCpuFreqMHz(), "SDK Version", ESP.getSdkVersion(), level);
    print("Flash Chip ID", ESP.getFlashChipId(), "Flash Size", ESP.getFlashChipSize(), level);
    print("Flash Real Size", ESP.getFlashChipRealSize(), "Flash Speed", ESP.getFlashChipSpeed(), level);
    print("Cycle Count", ESP.getCycleCount(), "VCC", ESP.getVcc(), level);
#elif defined(ARDUINO_ARCH_ESP32)
    print("Chip Model", ESP.getChipModel(), "Chip Revision", ESP.getChipRevision(), level);
    print("Chip Cores", ESP.getChipCores(), "CPU Freq MHz", ESP.getCpuFreqMHz(), level);
    print("SDK Version", ESP.getSdkVersion(), "Flash Size", ESP.getFlashChipSize(), level);
    print("Flash Speed", ESP.getFlashChipSpeed(), "Cycle Count", ESP.getCycleCount(), level);
#else
    if (_bufferingEnabled) {
        bufferWrite("System info not available for this board\n");
        flushBuffer();
    } else {
        Serial.println("System info not available for this board");
    }
#endif
}

void SerialDebuggerV2::startTimer() {
    _startTime = millis();
}

void SerialDebuggerV2::logExecutionTime(const String &operation, CustomLogLevel level) {
    if (!shouldPrint(level)) return;

    unsigned long duration = millis() - _startTime;

    print(operation + " Time", String(duration) + "ms", level);

    _startTime = millis();
}

void SerialDebuggerV2::clear() {
    if (!_enabled) return;

    if (_bufferingEnabled) {
        bufferWrite("\033[2J\033[H");
        flushBuffer();
    } else {
        Serial.write(27);
        Serial.print("[2J");
        Serial.write(27);
        Serial.print("[H");
    }
}

const char *SerialDebuggerV2::getLogLevelName(CustomLogLevel level) {
    for (uint8_t i = 0; i < _levelNamesCount; i++) {
        if (_levelNames[i].level == level) {
            return _levelNames[i].name;
        }
    }

    static char unknownBuffer[20];
    for (int i = 0; i < 32; i++) {
        if (level & (1UL << i)) {
            sprintf(unknownBuffer, "CUSTOM-%d", i);
            return unknownBuffer;
        }
    }

    return "UNKNOWN";
}

void SerialDebuggerV2::printHeader(const String &header, CustomLogLevel level) {
    if (!_enabled || !throttleCheck() || !shouldPrint(level)) return;

    String output = _format.separatorStart;

    if (_prefix != "") {
        output += _prefix + " ";
    }

    if (_showMillis) {
        output += "[" + String(millis()) + "ms] ";
    }

    if (_showTimestamp) {
        unsigned long runtime = millis() - _startTime;
        unsigned long seconds = runtime / 1000;
        unsigned long minutes = seconds / 60;
        unsigned long hours = minutes / 60;

        char timeBuffer[20];
        sprintf(timeBuffer, _format.timeFormat.c_str(), hours, minutes % 60, seconds % 60);
        output += String(timeBuffer) + " ";
    }

    if (_showFreeMemory) {
#if defined(ARDUINO_ARCH_ESP8266)
        output += "[Mem:" + String(ESP.getFreeHeap()) + "B] ";
#elif defined(ARDUINO_ARCH_ESP32)
        output += "[Mem:" + String(ESP.getFreeHeap()) + "B] ";
#endif
    }

    if (level != DEBUG_LEVEL) {
        output += "[" + String(getLogLevelName(level)) + "] ";
    }

    if (header != "") {
        output += header + " ";
    }

    if (_bufferingEnabled) {
        bufferWrite(output);
    } else {
        Serial.print(output);
    }
}

void SerialDebuggerV2::printFooter() {
    if (!_enabled) return;

    if (_bufferingEnabled) {
        flushBuffer();
    }
}

void SerialDebuggerV2::flushBuffer() {
    if (_bufferPos > 0) {
        _buffer[_bufferPos] = '\0';
        Serial.print(_buffer);
        _bufferPos = 0;
    }
}

void SerialDebuggerV2::bufferWrite(const char *data) {
    size_t len = strlen(data);

    if (_bufferPos + len >= _bufferSize - 1) {
        flushBuffer();
    }

    if (len >= _bufferSize - 1) {
        Serial.print(data);
        return;
    }

    memcpy(_buffer + _bufferPos, data, len);
    _bufferPos += len;
}

void SerialDebuggerV2::bufferWrite(const String &data) {
    bufferWrite(data.c_str());
}

bool SerialDebuggerV2::shouldPrint(CustomLogLevel level) {
    return _enabled && (_enabledLogLevels & level) && throttleCheck();
}

bool SerialDebuggerV2::throttleCheck() {
    if (_minDebugInterval == 0) {
        return true;
    }

    unsigned long currentTime = millis();
    if (currentTime - _lastDebugTime >= _minDebugInterval) {
        _lastDebugTime = currentTime;
        return true;
    }

    return false;
}