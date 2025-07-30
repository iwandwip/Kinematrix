#ifndef SD_CARD_MODULE_ESP32_H
#define SD_CARD_MODULE_ESP32_H

#if defined(ESP32)

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <vector>

enum class SDError {
    None = 0,
    NotInitialized,
    FileNotFound,
    WriteError,
    ReadError,
    OutOfMemory,
    InvalidPath,
    PermissionDenied,
    DiskFull,
    Timeout,
    ChecksumMismatch
};

struct FileHandle {
    fs::File file;
    String path;
    bool inUse;
    unsigned long lastAccess;
};

struct DirectoryCache {
    String path;
    std::vector<String> files;
    std::vector<String> dirs;
    unsigned long cacheTime;
    bool valid;
};

struct PerformanceStats {
    unsigned long totalReads;
    unsigned long totalWrites;
    unsigned long totalBytesRead;
    unsigned long totalBytesWritten;
    unsigned long totalErrors;
    unsigned long averageReadTime;
    unsigned long averageWriteTime;
};

class SDCardModuleESP32 {
private:
    uint8_t _csPin;
    bool _initialized;
    size_t _bufferSize;
    uint8_t* _primaryBuffer;
    uint8_t* _secondaryBuffer;
    bool _useDoubleBuffer;
    SDError _lastError;
    String _lastErrorMessage;
    
    static const size_t _maxFileHandles = 5;
    FileHandle _fileHandles[_maxFileHandles];
    
    static const size_t _maxCacheEntries = 3;
    DirectoryCache _dirCache[_maxCacheEntries];
    
    PerformanceStats _stats;
    static const unsigned long _cacheTimeout = 30000;
    static const unsigned long _handleTimeout = 60000;
    
    void setError(SDError error, const String& message = "");
    void updateStats(bool isRead, size_t bytes, unsigned long duration);
    FileHandle* getAvailableHandle();
    FileHandle* findHandle(const String& path);
    void closeExpiredHandles();
    DirectoryCache* getCachedDir(const String& path);
    DirectoryCache* getAvailableCacheSlot();
    void cacheDirectory(const String& path, const std::vector<String>& files, const std::vector<String>& dirs);
    uint32_t calculateCRC32(const uint8_t* data, size_t length);
    size_t getOptimalBufferSize(size_t fileSize);

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
    
    SDError getLastError() const;
    String getLastErrorMessage() const;
    void clearError();
    
    bool setBufferSize(size_t size);
    void enableDoubleBuffering(bool enable);
    
    bool readFileStream(const char* path, size_t offset, size_t length, uint8_t* buffer);
    bool writeFileStream(const char* path, const uint8_t* data, size_t length, bool append = false);
    
    bool copyFile(const char* source, const char* destination);
    bool moveFile(const char* source, const char* destination);
    bool batchDelete(const std::vector<String>& paths);
    bool batchCopy(const std::vector<String>& sources, const std::vector<String>& destinations);
    
    uint32_t getFileCRC32(const char* path);
    bool verifyFileIntegrity(const char* path, uint32_t expectedCRC);
    bool writeFileWithChecksum(const char* path, const char* data);
    bool readFileWithChecksum(const char* path, String& data);
    
    void printPerformanceStats();
    void resetPerformanceStats();
    PerformanceStats getPerformanceStats() const;
    
    void clearDirectoryCache();
    bool refreshDirectoryCache(const char* path);
    
    size_t getAvailableMemory();
    void optimizeMemoryUsage();
};

#endif
#endif