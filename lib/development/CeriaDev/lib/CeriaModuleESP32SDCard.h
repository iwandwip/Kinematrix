#ifndef CERIA_MODULE_ESP32_SDCARD_H
#define CERIA_MODULE_ESP32_SDCARD_H

#pragma message("[COMPILED]: CeriaModuleESP32SDCard.h")

#include "Arduino.h"

#ifdef ESP32
#include "FS.h"
#include "SD_MMC.h"
#include "SD.h"
#include "SPI.h"

namespace CeriaDevOP {

    enum SDCardInterface : uint8_t {
        SDCARD_SPI = 0,
        SDCARD_SDMMC_1BIT = 1,
        SDCARD_SDMMC_4BIT = 2
    };

    enum SDCardType : uint8_t {
        SDCARD_TYPE_UNKNOWN = 0,
        SDCARD_TYPE_MMC = 1,
        SDCARD_TYPE_SD = 2,
        SDCARD_TYPE_SDHC = 3
    };

    enum SDCardError : uint8_t {
        SDCARD_ERR_NONE = 0,
        SDCARD_ERR_MOUNT_FAILED,
        SDCARD_ERR_NO_CARD,
        SDCARD_ERR_CARD_UNKNOWN,
        SDCARD_ERR_FILE_NOT_FOUND,
        SDCARD_ERR_DIR_NOT_FOUND,
        SDCARD_ERR_WRITE_FAILED,
        SDCARD_ERR_READ_FAILED,
        SDCARD_ERR_DELETE_FAILED,
        SDCARD_ERR_CREATE_FAILED,
        SDCARD_ERR_RENAME_FAILED,
        SDCARD_ERR_COPY_FAILED,
        SDCARD_ERR_INSUFFICIENT_SPACE,
        SDCARD_ERR_INVALID_PATH,
        SDCARD_ERR_NOT_INITIALIZED
    };

    enum SDCardFileMode : uint8_t {
        SDCARD_FILE_READ = 0,
        SDCARD_FILE_WRITE = 1,
        SDCARD_FILE_APPEND = 2
    };

    struct SDCardInfo {
        SDCardType cardType;
        uint64_t cardSizeMB;
        uint64_t totalSpaceMB;
        uint64_t usedSpaceMB;
        uint64_t freeSpaceMB;
        uint32_t sectorSize;
        uint32_t numSectors;
        String cardTypeString;
        String fileSystemType;
        bool isValid;
    };

    struct SDCardStats {
        uint32_t filesCreated;
        uint32_t filesDeleted;
        uint32_t filesRead;
        uint32_t filesWritten;
        uint32_t directoriesCreated;
        uint32_t directoriesDeleted;
        uint64_t bytesRead;
        uint64_t bytesWritten;
        uint32_t errorCount;
        uint32_t lastErrorTime;
        SDCardError lastError;
        uint32_t operationsTotal;
    };

    struct SDCardPinConfig {
        int8_t sck;
        int8_t miso;
        int8_t mosi;
        int8_t cs;
        int8_t clk;
        int8_t cmd;
        int8_t d0;
        int8_t d1;
        int8_t d2;
        int8_t d3;
    };

    typedef void (*SDCardErrorCallback)(SDCardError error, const String &operation);
    typedef void (*SDCardFileCallback)(const String &filename, uint32_t size);

    class CeriaModuleESP32SDCard {
    private:
        SDCardInterface _interface;
        fs::FS *_fs;
        SDCardInfo _cardInfo;
        SDCardStats _stats;
        SDCardPinConfig _pinConfig;

        bool _initialized;
        bool _hasNewData;
        uint32_t _lastUpdateTime;
        uint32_t _updateInterval;
        uint32_t _timeout;

        uint32_t _spiFrequency;
        uint32_t _sdmmcFrequency;
        bool _formatIfMountFailed;
        uint8_t _maxOpenFiles;

        SDCardErrorCallback _onError;
        SDCardFileCallback _onFileOperation;

        String _mountPoint;
        String _lastOperation;
        String _workingDirectory;

        bool initializeSPI();
        bool initializeSDMMC(bool mode1bit);
        void updateCardInfo();
        void updateStats();
        void triggerError(SDCardError error, const String &operation);
        void triggerFileOperation(const String &filename, uint32_t size);

        String getCardTypeString(SDCardType type);
        bool validatePath(const String &path);
        String normalizePath(const String &path);
        String getDirectoryFromPath(const String &path);
        String getFilenameFromPath(const String &path);

        void incrementStat(const String &operation, uint32_t bytes = 0);
        bool checkInitialized();

        static const char *_defaultMountPoint;
        static const int8_t _defaultSPIPins[4];
        static const int8_t _defaultSDMMCPins[6];

    public:
        CeriaModuleESP32SDCard();
        ~CeriaModuleESP32SDCard();

        bool begin(SDCardInterface interface = SDCARD_SDMMC_1BIT, const String &mountPoint = "/sdcard");
        bool beginSPI(int8_t sck = 18, int8_t miso = 19, int8_t mosi = 23, int8_t cs = 5, uint32_t frequency = 4000000);
        bool beginSDMMC(bool mode1bit = true, int8_t clk = -1, int8_t cmd = -1, int8_t d0 = -1, int8_t d1 = -1, int8_t d2 = -1, int8_t d3 = -1);
        bool beginSDMMC1Bit(int8_t clk = 14, int8_t cmd = 15, int8_t d0 = 2);
        bool beginSDMMC4Bit(int8_t clk = 14, int8_t cmd = 15, int8_t d0 = 2, int8_t d1 = 4, int8_t d2 = 12, int8_t d3 = 13);

        void end();
        void update();

        bool isInitialized();
        bool hasNewData();
        bool isConnected(uint32_t timeoutMs = 5000);

        SDCardInfo getCardInfo();
        SDCardStats getStats();
        SDCardInterface getInterface();
        String getMountPoint();

        uint64_t getCardSize();
        uint64_t getTotalBytes();
        uint64_t getUsedBytes();
        uint64_t getFreeBytes();
        uint32_t getSectorSize();
        uint32_t getNumSectors();
        SDCardType getCardType();
        String getCardTypeString();

        void setUpdateInterval(uint32_t intervalMs);
        void setTimeout(uint32_t timeoutMs);
        void setFormatIfMountFailed(bool format);
        void setMaxOpenFiles(uint8_t maxFiles);

        void setErrorCallback(SDCardErrorCallback callback);
        void setFileOperationCallback(SDCardFileCallback callback);

        bool exists(const String &path);
        bool isFile(const String &path);
        bool isDirectory(const String &path);
        uint32_t getFileSize(const String &path);
        time_t getLastModified(const String &path);

        File open(const String &path, SDCardFileMode mode = SDCARD_FILE_READ);
        bool createFile(const String &path);
        bool deleteFile(const String &path);
        bool renameFile(const String &oldPath, const String &newPath);
        bool copyFile(const String &sourcePath, const String &destPath);
        bool moveFile(const String &oldPath, const String &newPath);

        bool createDirectory(const String &path);
        bool deleteDirectory(const String &path);
        bool renameDirectory(const String &oldPath, const String &newPath);

        bool writeFile(const String &path, const String &content);
        bool writeFile(const String &path, const uint8_t *data, uint32_t length);
        bool appendFile(const String &path, const String &content);
        bool appendFile(const String &path, const uint8_t *data, uint32_t length);
        String readFile(const String &path);
        bool readFile(const String &path, uint8_t *buffer, uint32_t maxLength, uint32_t &bytesRead);

        bool writeCSV(const String &path, const String &data, bool append = true);
        bool writeJSON(const String &path, const String &jsonData, bool pretty = false);
        bool writeLog(const String &path, const String &message, bool timestamp = true);

        String listFiles(const String &directory = "/");
        String listDirectories(const String &directory = "/");
        String listAll(const String &directory = "/", bool recursive = false);
        uint32_t countFiles(const String &directory = "/", bool recursive = false);
        uint32_t countDirectories(const String &directory = "/", bool recursive = false);

        bool setWorkingDirectory(const String &path);
        String getWorkingDirectory();
        String getCurrentPath(const String &relativePath);

        bool format();
        bool test();
        bool benchmark();
        float getReadSpeed();
        float getWriteSpeed();

        String getSDCardString();
        String getInfoString();
        String getStatsString();
        String getStatusString();
        String getErrorString(SDCardError error);

        void resetStats();
        void printInfo();
        void printStats();
        void printDirectory(const String &path = "/", bool recursive = false);

        bool waitForCard(uint32_t timeoutMs = 10000);
        bool waitForFile(const String &path, uint32_t timeoutMs = 5000);

        void enableAutoUpdate(bool enable = true);
        void enableVerbose(bool enable = true);

        bool backupFile(const String &sourcePath, const String &backupDir = "/backup");
        bool restoreFile(const String &backupPath, const String &destPath);
        bool archiveDirectory(const String &sourceDir, const String &archivePath);

        String generateUniqueFilename(const String &basePath, const String &extension = ".txt");
        bool ensureDirectory(const String &path);
        bool cleanupOldFiles(const String &directory, uint32_t maxAgeMs, uint32_t maxFiles = 0);

        SDCardError getLastError();
        String getLastOperation();
        void clearLastError();

        static String formatBytes(uint64_t bytes);
        static String formatSpeed(float bytesPerSecond);
        static bool isValidFilename(const String &filename);
        static String sanitizeFilename(const String &filename);
    };

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(ESP32S3)
#define CERIA_SDCARD_DEFAULT_SDMMC_CLK    14
#define CERIA_SDCARD_DEFAULT_SDMMC_CMD    15
#define CERIA_SDCARD_DEFAULT_SDMMC_D0     2
#define CERIA_SDCARD_DEFAULT_SDMMC_D1     4
#define CERIA_SDCARD_DEFAULT_SDMMC_D2     12
#define CERIA_SDCARD_DEFAULT_SDMMC_D3     13
#elif defined(ESP32)
#define CERIA_SDCARD_DEFAULT_SDMMC_CLK    14
#define CERIA_SDCARD_DEFAULT_SDMMC_CMD    15
#define CERIA_SDCARD_DEFAULT_SDMMC_D0     2
#define CERIA_SDCARD_DEFAULT_SDMMC_D1     4
#define CERIA_SDCARD_DEFAULT_SDMMC_D2     12
#define CERIA_SDCARD_DEFAULT_SDMMC_D3     13
#endif

#define CERIA_SDCARD_DEFAULT_SPI_SCK      18
#define CERIA_SDCARD_DEFAULT_SPI_MISO     19
#define CERIA_SDCARD_DEFAULT_SPI_MOSI     23
#define CERIA_SDCARD_DEFAULT_SPI_CS       5

#define CERIA_SDCARD_DEFAULT_FREQUENCY    4000000
#define CERIA_SDCARD_DEFAULT_TIMEOUT      5000
#define CERIA_SDCARD_DEFAULT_UPDATE_INTERVAL  1000
#define CERIA_SDCARD_MAX_PATH_LENGTH      255
#define CERIA_SDCARD_MAX_FILENAME_LENGTH  127

}

#endif

#endif