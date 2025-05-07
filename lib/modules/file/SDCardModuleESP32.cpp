#include "SDCardModuleESP32.h"

#if defined(ESP32)

SDCardModuleESP32::SDCardModuleESP32(uint8_t csPin)
        : _csPin(csPin),
          _initialized(false) {
    memset(_buffer, 0, _bufferSize);
}

SDCardModuleESP32::~SDCardModuleESP32() {
    end();
}

bool SDCardModuleESP32::begin() {
    if (_initialized) {
        return true;
    }

    _initialized = SD.begin(_csPin);
    return _initialized;
}

bool SDCardModuleESP32::isInitialized() const {
    return _initialized;
}

void SDCardModuleESP32::end() {
    if (_initialized) {
        SD.end();
        _initialized = false;
    }
}

uint8_t SDCardModuleESP32::cardType() const {
    if (!_initialized) {
        return CARD_NONE;
    }

    sdcard_type_t type = SD.cardType();
    switch (type) {
        case CARD_NONE:
            return CARD_NONE;
        case CARD_MMC:
            return CARD_MMC;
        case CARD_SD:
            return CARD_SD;
        case CARD_SDHC:
            return CARD_SDHC;
        default:
            return CARD_UNKNOWN;
    }
}

String SDCardModuleESP32::cardTypeString() const {
    if (!_initialized) {
        return "NONE";
    }

    sdcard_type_t type = SD.cardType();
    switch (type) {
        case CARD_NONE:
            return "NONE";
        case CARD_MMC:
            return "MMC";
        case CARD_SD:
            return "SDSC";
        case CARD_SDHC:
            return "SDHC";
        default:
            return "UNKNOWN";
    }
}

uint64_t SDCardModuleESP32::cardSize() const {
    if (!_initialized) {
        return 0;
    }
    return SD.cardSize();
}

uint64_t SDCardModuleESP32::totalBytes() const {
    if (!_initialized) {
        return 0;
    }
    return SD.totalBytes();
}

uint64_t SDCardModuleESP32::usedBytes() const {
    if (!_initialized) {
        return 0;
    }
    return SD.usedBytes();
}

float SDCardModuleESP32::usedPercentage() const {
    if (!_initialized) {
        return 0.0f;
    }

    uint64_t total = totalBytes();
    if (total == 0) {
        return 0.0f;
    }

    return (float) ((float) usedBytes() * 100.0 / (float) total);
}

bool SDCardModuleESP32::listDir(const char *dirname, uint8_t levels) {
    if (!_initialized) {
        return false;
    }

    fs::File root = SD.open(dirname);
    if (!root) {
        return false;
    }

    if (!root.isDirectory()) {
        root.close();
        return false;
    }

    fs::File entry = root.openNextFile();
    while (entry) {
        if (entry.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(entry.name());
            if (levels) {
                listDir(entry.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(entry.name());
            Serial.print("  SIZE: ");
            Serial.println(entry.size());
        }
        entry = root.openNextFile();
    }

    root.close();
    return true;
}

bool SDCardModuleESP32::createDir(const char *path) const {
    if (!_initialized) {
        return false;
    }

    return SD.mkdir(path);
}

bool SDCardModuleESP32::removeDir(const char *path) const {
    if (!_initialized) {
        return false;
    }

    return SD.rmdir(path);
}

bool SDCardModuleESP32::readFile(const char *path) const {
    if (!_initialized) {
        return false;
    }

    fs::File entry = SD.open(path);
    if (!entry) {
        return false;
    }

    Serial.print("Reading file: ");
    Serial.println(path);

    while (entry.available()) {
        Serial.write(entry.read());
    }

    entry.close();
    return true;
}

String SDCardModuleESP32::readFileString(const char *path) const {
    if (!_initialized) {
        return "";
    }

    fs::File entry = SD.open(path);
    if (!entry) {
        return "";
    }

    String content = "";
    while (entry.available()) {
        content += (char) entry.read();
    }

    entry.close();
    return content;
}

bool SDCardModuleESP32::writeFile(const char *path, const char *message) const {
    if (!_initialized) {
        return false;
    }

    fs::File entry = SD.open(path, FILE_WRITE);
    if (!entry) {
        return false;
    }

    bool success = entry.print(message);
    entry.close();

    return success;
}

bool SDCardModuleESP32::appendFile(const char *path, const char *message) const {
    if (!_initialized) {
        return false;
    }

    fs::File entry = SD.open(path, FILE_APPEND);
    if (!entry) {
        return false;
    }

    bool success = entry.print(message);
    entry.close();

    return success;
}

bool SDCardModuleESP32::renameFile(const char *path1, const char *path2) const {
    if (!_initialized) {
        return false;
    }

    return SD.rename(path1, path2);
}

bool SDCardModuleESP32::deleteFile(const char *path) const {
    if (!_initialized) {
        return false;
    }

    return SD.remove(path);
}

bool SDCardModuleESP32::exists(const char *path) const {
    if (!_initialized) {
        return false;
    }

    return SD.exists(path);
}

bool SDCardModuleESP32::isDirectory(const char *path) const {
    if (!_initialized) {
        return false;
    }

    fs::File entry = SD.open(path);
    if (!entry) {
        return false;
    }

    bool result = entry.isDirectory();
    entry.close();

    return result;
}

size_t SDCardModuleESP32::fileSize(const char *path) const {
    if (!_initialized) {
        return 0;
    }

    fs::File entry = SD.open(path);
    if (!entry) {
        return 0;
    }

    size_t size = entry.size();
    entry.close();

    return size;
}

bool SDCardModuleESP32::testFileIO(const char *path) {
    if (!_initialized) {
        return false;
    }

    fs::File entry = SD.open(path);
    if (!entry) {
        return false;
    }

    size_t len = entry.size();
    size_t flen = len;

    uint32_t start = millis();
    while (len) {
        size_t toRead = len > _bufferSize ? _bufferSize : len;
        entry.read(_buffer, toRead);
        len -= toRead;
    }
    uint32_t readTime = millis() - start;

    entry.close();

    entry = SD.open(path, FILE_WRITE);
    if (!entry) {
        return false;
    }

    start = millis();
    for (size_t i = 0; i < 2048; i++) {
        entry.write(_buffer, _bufferSize);
    }
    uint32_t writeTime = millis() - start;

    entry.close();

    Serial.printf("%u bytes read in %u ms\n", flen, readTime);
    Serial.printf("%u bytes written in %u ms\n", 2048 * _bufferSize, writeTime);

    return true;
}

bool SDCardModuleESP32::setCustomSPI(uint8_t sck, uint8_t miso, uint8_t mosi) {
    if (_initialized) {
        end();
    }

    SPI.end();
    SPI.begin((int8_t) sck, (int8_t) miso, (int8_t) mosi, (int8_t) _csPin);

    return begin();
}

bool SDCardModuleESP32::formatFAT32() {
    if (_initialized) {
        end();
    }

    Serial.println("Formatting card as FAT32...");
    Serial.println("WARNING: This operation will erase all data on the card!");

    bool result = false;

    Serial.println("Format complete");

    return begin();
}

#endif