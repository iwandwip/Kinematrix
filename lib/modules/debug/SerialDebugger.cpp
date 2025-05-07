#include "SerialDebugger.h"

SerialDebugger::SerialDebugger(unsigned int baudRate, size_t bufferSize) {
    _baudRate = baudRate;
    _enabled = true;
    _showTimestamp = false;
    _showMillis = true;
    _showFreeMemory = false;
    _prefix = "";
    _startTime = 0;
    _logLevel = DEBUG;
    _lastDebugTime = 0;
    _minDebugInterval = 0;

    _bufferSize = bufferSize;
    _buffer = new char[_bufferSize];
    _bufferPos = 0;
    _bufferingEnabled = false;

    resetFormat();
}

SerialDebugger::~SerialDebugger() {
    delete[] _buffer;
}

void SerialDebugger::begin() {
    Serial.begin(_baudRate);
    while (!Serial) {
        delay(10);
    }
    _startTime = millis();
}

void SerialDebugger::enable() {
    _enabled = true;
}

void SerialDebugger::disable() {
    _enabled = false;
}

bool SerialDebugger::isEnabled() const {
    return _enabled;
}

void SerialDebugger::setPrefix(const String &prefix) {
    _prefix = prefix;
}

void SerialDebugger::showTimestamp(bool show) {
    _showTimestamp = show;
}

void SerialDebugger::showMillis(bool show) {
    _showMillis = show;
}

void SerialDebugger::showFreeMemory(bool show) {
    _showFreeMemory = show;
}

void SerialDebugger::setLogLevel(LogLevel level) {
    _logLevel = level;
}

LogLevel SerialDebugger::getLogLevel() {
    return _logLevel;
}

void SerialDebugger::setFormat(const DebugFormat &format) {
    _format = format;
}

DebugFormat SerialDebugger::getDefaultFormat() {
    DebugFormat format;
    format.separatorStart = "";
    format.separatorBetween = " | ";
    format.nameValueSeparator = ": ";
    format.timeFormat = "[%02d:%02d:%02d]";
    format.prefixFormat = "%s ";
    return format;
}

void SerialDebugger::resetFormat() {
    _format = getDefaultFormat();
}

void SerialDebugger::enableBuffering(bool enable) {
    if (enable && !_bufferingEnabled) {
        _bufferPos = 0;
    } else if (!enable && _bufferingEnabled) {
        flushBuffer();
    }
    _bufferingEnabled = enable;
}

void SerialDebugger::setBufferSize(size_t size) {
    if (_bufferingEnabled) {
        flushBuffer();
    }

    delete[] _buffer;
    _bufferSize = size;
    _buffer = new char[_bufferSize];
    _bufferPos = 0;
}

void SerialDebugger::setMinDebugInterval(unsigned int millisInterval) {
    _minDebugInterval = millisInterval;
}

void SerialDebugger::printHeader(const String &header, LogLevel level) {
    if (!_enabled || !throttleCheck()) return;

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

    if (level != DEBUG && level != NONE) {
        switch (level) {
            case INFO:
                output += "[INFO] ";
                break;
            case WARNING:
                output += "[WARNING] ";
                break;
            case ERROR:
                output += "[ERROR] ";
                break;
            default:
                break;
        }
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

void SerialDebugger::printFooter() {
    if (!_enabled) return;

    if (_bufferingEnabled) {
        bufferWrite("\n");
        flushBuffer();
    } else {
        Serial.println();
    }
}

void SerialDebugger::flushBuffer() {
    if (_bufferPos > 0) {
        _buffer[_bufferPos] = '\0';
        Serial.print(_buffer);
        _bufferPos = 0;
    }
}

void SerialDebugger::bufferWrite(const char *data) {
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

void SerialDebugger::bufferWrite(const String &data) {
    bufferWrite(data.c_str());
}

bool SerialDebugger::shouldPrint(LogLevel level) {
    return _enabled && level >= _logLevel && throttleCheck();
}

bool SerialDebugger::throttleCheck() {
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

void SerialDebugger::println() {
    if (!_enabled || !throttleCheck()) return;

    if (_bufferingEnabled) {
        bufferWrite("\n");
        flushBuffer();
    } else {
        Serial.println();
    }
}

void SerialDebugger::section(const String &title, LogLevel level) {
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

void SerialDebugger::startSection(const String &title, LogLevel level) {
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

void SerialDebugger::endSection(LogLevel level) {
    if (!shouldPrint(level)) return;

    String output = "===== END =====";

    if (_bufferingEnabled) {
        bufferWrite(output + "\n");
        flushBuffer();
    } else {
        Serial.println(output);
    }
}

void SerialDebugger::separator(LogLevel level) {
    if (!shouldPrint(level)) return;

    String output = "---------------------------";

    if (_bufferingEnabled) {
        bufferWrite(output + "\n");
        flushBuffer();
    } else {
        Serial.println(output);
    }
}

void SerialDebugger::newLine(LogLevel level) {
    if (!shouldPrint(level)) return;

    if (_bufferingEnabled) {
        bufferWrite("\n");
        flushBuffer();
    } else {
        Serial.println();
    }
}

void SerialDebugger::debug(const String &message) {
    if (!shouldPrint(DEBUG)) return;

    printHeader("[DEBUG]", DEBUG);

    if (_bufferingEnabled) {
        bufferWrite(message);
    } else {
        Serial.print(message);
    }

    printFooter();
}

void SerialDebugger::info(const String &message) {
    if (!shouldPrint(INFO)) return;

    printHeader("[INFO]", INFO);

    if (_bufferingEnabled) {
        bufferWrite(message);
    } else {
        Serial.print(message);
    }

    printFooter();
}

void SerialDebugger::warning(const String &message) {
    if (!shouldPrint(WARNING)) return;

    printHeader("[WARNING]", WARNING);

    if (_bufferingEnabled) {
        bufferWrite(message);
    } else {
        Serial.print(message);
    }

    printFooter();
}

void SerialDebugger::error(const String &message) {
    if (!shouldPrint(ERROR)) return;

    printHeader("[ERROR]", ERROR);

    if (_bufferingEnabled) {
        bufferWrite(message);
    } else {
        Serial.print(message);
    }

    printFooter();
}

void SerialDebugger::success(const String &message) {
    if (!shouldPrint(INFO)) return;

    printHeader("[SUCCESS]", INFO);

    if (_bufferingEnabled) {
        bufferWrite(message);
    } else {
        Serial.print(message);
    }

    printFooter();
}

void SerialDebugger::hexDump(const void *data, size_t size, LogLevel level) {
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

void SerialDebugger::memoryInfo(LogLevel level) {
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

void SerialDebugger::systemInfo(LogLevel level) {
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

void SerialDebugger::startTimer() {
    _startTime = millis();
}

void SerialDebugger::logExecutionTime(const String &operation, LogLevel level) {
    if (!shouldPrint(level)) return;

    unsigned long duration = millis() - _startTime;

    print(operation + " Time", String(duration) + "ms", level);

    _startTime = millis();
}

void SerialDebugger::clear() {
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