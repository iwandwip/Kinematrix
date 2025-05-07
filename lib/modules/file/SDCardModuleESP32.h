#ifndef SD_CARD_MODULE_ESP32_H
#define SD_CARD_MODULE_ESP32_H

#if defined(ESP32)

#include "FS.h"
#include "SD.h"
#include "SPI.h"

class SDCardModuleESP32 {
private:
    uint8_t _csPin;
    bool _initialized;
    static const size_t _bufferSize = 512;
    uint8_t _buffer[_bufferSize];

public:
    explicit SDCardModuleESP32(uint8_t csPin = 5);
    ~SDCardModuleESP32();

    bool begin();
    bool isInitialized() const;
    void end();

    uint8_t cardType() const;
    String cardTypeString() const;
    uint64_t cardSize() const;
    uint64_t totalBytes() const;
    uint64_t usedBytes() const;
    float usedPercentage() const;

    bool listDir(const char *dirname, uint8_t levels = 0);
    bool createDir(const char *path) const;
    bool removeDir(const char *path) const;

    bool readFile(const char *path) const;
    String readFileString(const char *path) const;
    bool writeFile(const char *path, const char *message) const;
    bool appendFile(const char *path, const char *message) const;
    bool renameFile(const char *path1, const char *path2) const;
    bool deleteFile(const char *path) const;

    bool exists(const char *path) const;
    bool isDirectory(const char *path) const;
    size_t fileSize(const char *path) const;

    bool testFileIO(const char *path);

    bool setCustomSPI(uint8_t sck, uint8_t miso, uint8_t mosi);
    bool formatFAT32();
};

#endif
#endif