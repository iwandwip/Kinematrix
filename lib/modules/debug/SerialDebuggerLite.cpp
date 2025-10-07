#include "SerialDebuggerLite.h"

SerialDebuggerLite::SerialDebuggerLite() {
    _enabled = 1;
    _logLevel = DEBUG_LEVEL_DEBUG;
    _startTime = 0;
    _lastDebugTime = 0;
    _minDebugInterval = 0;
    _prefix[0] = '\0';
    _showMillis = 1;
}

void SerialDebuggerLite::begin(uint32_t baudRate) {
    Serial.begin(baudRate);
    while (!Serial) {
        delay(10);
    }
    _startTime = millis();
}

void SerialDebuggerLite::enable() {
    _enabled = 1;
}

void SerialDebuggerLite::disable() {
    _enabled = 0;
}

uint8_t SerialDebuggerLite::isEnabled() {
    return _enabled;
}

void SerialDebuggerLite::setPrefix(const char *prefix) {
    strncpy(_prefix, prefix, 9);
    _prefix[9] = '\0';
}

void SerialDebuggerLite::showMillis(uint8_t show) {
    _showMillis = show;
}

void SerialDebuggerLite::setLogLevel(uint8_t level) {
    _logLevel = level;
}

void SerialDebuggerLite::setMinDebugInterval(uint16_t millisInterval) {
    _minDebugInterval = millisInterval;
}

void SerialDebuggerLite::printHeader(const char *header, uint8_t level) {
    if (!_enabled || !throttleCheck()) return;

    if (_prefix[0] != '\0') {
        Serial.print(_prefix);
        Serial.print(F(" "));
    }

    if (_showMillis) {
        Serial.print(F("["));
        Serial.print(millis());
        Serial.print(F("ms] "));
    }

    if (level != DEBUG_LEVEL_DEBUG && level != DEBUG_LEVEL_NONE) {
        switch (level) {
            case DEBUG_LEVEL_INFO:
                Serial.print(F("[INFO] "));
                break;
            case DEBUG_LEVEL_WARNING:
                Serial.print(F("[WARN] "));
                break;
            case DEBUG_LEVEL_ERROR:
                Serial.print(F("[ERR] "));
                break;
            default:
                break;
        }
    }

    if (header[0] != '\0') {
        Serial.print(header);
        Serial.print(F(" "));
    }
}

void SerialDebuggerLite::printFooter() {
    if (!_enabled) return;

    Serial.println();
}

uint8_t SerialDebuggerLite::shouldPrint(uint8_t level) {
    return _enabled && level <= _logLevel && throttleCheck();
}

uint8_t SerialDebuggerLite::throttleCheck() {
    if (_minDebugInterval == 0) {
        return 1;
    }

    uint32_t currentTime = millis();
    if (currentTime - _lastDebugTime >= _minDebugInterval) {
        _lastDebugTime = currentTime;
        return 1;
    }

    return 0;
}

void SerialDebuggerLite::println() {
    if (!_enabled || !throttleCheck()) return;

    Serial.println();
}

void SerialDebuggerLite::section(const char *title, uint8_t level) {
    if (!shouldPrint(level)) return;

    println();
    Serial.print(F("===== "));
    Serial.print(title);
    Serial.println(F(" ====="));
}

void SerialDebuggerLite::separator(uint8_t level) {
    if (!shouldPrint(level)) return;

    Serial.println(F("---------------------------"));
}

void SerialDebuggerLite::debug(const char *message) {
    if (!shouldPrint(DEBUG_LEVEL_DEBUG)) return;

    printHeader("[DEBUG]", DEBUG_LEVEL_DEBUG);
    Serial.print(message);
    printFooter();
}

void SerialDebuggerLite::info(const char *message) {
    if (!shouldPrint(DEBUG_LEVEL_INFO)) return;

    printHeader("[INFO]", DEBUG_LEVEL_INFO);
    Serial.print(message);
    printFooter();
}

void SerialDebuggerLite::warning(const char *message) {
    if (!shouldPrint(DEBUG_LEVEL_WARNING)) return;

    printHeader("[WARN]", DEBUG_LEVEL_WARNING);
    Serial.print(message);
    printFooter();
}

void SerialDebuggerLite::error(const char *message) {
    if (!shouldPrint(DEBUG_LEVEL_ERROR)) return;

    printHeader("[ERR]", DEBUG_LEVEL_ERROR);
    Serial.print(message);
    printFooter();
}

void SerialDebuggerLite::hexDump(const void *data, uint8_t size, uint8_t level) {
    if (!shouldPrint(level)) return;

    const uint8_t *bytes = (const uint8_t *) data;
    char buffer[8];
    char ascii[9];

    printHeader("[HEX]", level);
    Serial.println();

    for (uint8_t i = 0; i < size; i += 8) {
        sprintf(buffer, "%04X: ", i);
        Serial.print(buffer);

        for (uint8_t j = 0; j < 8; j++) {
            if (i + j < size) {
                sprintf(buffer, "%02X ", bytes[i + j]);
                Serial.print(buffer);

                if (bytes[i + j] >= 32 && bytes[i + j] <= 126) {
                    ascii[j] = bytes[i + j];
                } else {
                    ascii[j] = '.';
                }
            } else {
                Serial.print(F("   "));
                ascii[j] = ' ';
            }
        }

        ascii[8] = '\0';
        Serial.print(F(" | "));
        Serial.println(ascii);
    }
}

void SerialDebuggerLite::startTimer() {
    _startTime = millis();
}

void SerialDebuggerLite::logExecutionTime(const char *operation, uint8_t level) {
    if (!shouldPrint(level)) return;

    uint32_t duration = millis() - _startTime;

    printHeader("", level);
    Serial.print(F(" | "));
    Serial.print(operation);
    Serial.print(F(" Time: "));
    Serial.print(duration);
    Serial.print(F("ms"));
    printFooter();

    _startTime = millis();
}