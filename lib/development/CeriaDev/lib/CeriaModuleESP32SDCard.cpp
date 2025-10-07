#include "CeriaModuleESP32SDCard.h"

#ifdef ESP32

namespace CeriaDevOP {

    const char *CeriaModuleESP32SDCard::_defaultMountPoint = "/sdcard";
    const int8_t CeriaModuleESP32SDCard::_defaultSPIPins[4] = {18, 19, 23, 5};
    const int8_t CeriaModuleESP32SDCard::_defaultSDMMCPins[6] = {14, 15, 2, 4, 12, 13};

    CeriaModuleESP32SDCard::CeriaModuleESP32SDCard() {
        _interface = SDCARD_SDMMC_1BIT;
        _fs = nullptr;

        _initialized = false;
        _hasNewData = false;
        _lastUpdateTime = 0;
        _updateInterval = CERIA_SDCARD_DEFAULT_UPDATE_INTERVAL;
        _timeout = CERIA_SDCARD_DEFAULT_TIMEOUT;

        _spiFrequency = CERIA_SDCARD_DEFAULT_FREQUENCY;
        _sdmmcFrequency = 20000000;
        _formatIfMountFailed = false;
        _maxOpenFiles = 5;

        _onError = nullptr;
        _onFileOperation = nullptr;

        _mountPoint = _defaultMountPoint;
        _workingDirectory = "/";

        memset(&_cardInfo, 0, sizeof(_cardInfo));
        memset(&_stats, 0, sizeof(_stats));
        memset(&_pinConfig, -1, sizeof(_pinConfig));
    }

    CeriaModuleESP32SDCard::~CeriaModuleESP32SDCard() {
        end();
    }

    bool CeriaModuleESP32SDCard::begin(SDCardInterface interface, const String &mountPoint) {
        _interface = interface;
        _mountPoint = mountPoint;

        switch (_interface) {
            case SDCARD_SPI:
                return beginSPI();
            case SDCARD_SDMMC_1BIT:
                return beginSDMMC(true);
            case SDCARD_SDMMC_4BIT:
                return beginSDMMC(false);
            default:
                triggerError(SDCARD_ERR_NOT_INITIALIZED, "Invalid interface");
                return false;
        }
    }

    bool CeriaModuleESP32SDCard::beginSPI(int8_t sck, int8_t miso, int8_t mosi, int8_t cs, uint32_t frequency) {
        _interface = SDCARD_SPI;
        _spiFrequency = frequency;

        _pinConfig.sck = sck;
        _pinConfig.miso = miso;
        _pinConfig.mosi = mosi;
        _pinConfig.cs = cs;

        return initializeSPI();
    }

    bool CeriaModuleESP32SDCard::beginSDMMC(bool mode1bit, int8_t clk, int8_t cmd, int8_t d0, int8_t d1, int8_t d2, int8_t d3) {
        _interface = mode1bit ? SDCARD_SDMMC_1BIT : SDCARD_SDMMC_4BIT;

        _pinConfig.clk = clk;
        _pinConfig.cmd = cmd;
        _pinConfig.d0 = d0;
        _pinConfig.d1 = d1;
        _pinConfig.d2 = d2;
        _pinConfig.d3 = d3;

        return initializeSDMMC(mode1bit);
    }

    bool CeriaModuleESP32SDCard::beginSDMMC1Bit(int8_t clk, int8_t cmd, int8_t d0) {
        return beginSDMMC(true, clk, cmd, d0, -1, -1, -1);
    }

    bool CeriaModuleESP32SDCard::beginSDMMC4Bit(int8_t clk, int8_t cmd, int8_t d0, int8_t d1, int8_t d2, int8_t d3) {
        return beginSDMMC(false, clk, cmd, d0, d1, d2, d3);
    }

    bool CeriaModuleESP32SDCard::initializeSPI() {
        SPIClass *spi = &SPI;

        if (_pinConfig.sck != -1 && _pinConfig.miso != -1 && _pinConfig.mosi != -1) {
            spi->begin(_pinConfig.sck, _pinConfig.miso, _pinConfig.mosi, _pinConfig.cs);
        }

        if (!SD.begin(_pinConfig.cs, *spi, _spiFrequency)) {
            triggerError(SDCARD_ERR_MOUNT_FAILED, "SPI mount failed");
            return false;
        }

        _fs = &SD;
        _initialized = true;
        _lastUpdateTime = millis();
        updateCardInfo();

        return true;
    }

    bool CeriaModuleESP32SDCard::initializeSDMMC(bool mode1bit) {
#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(ESP32S3)
        if (_pinConfig.clk != -1 && _pinConfig.cmd != -1 && _pinConfig.d0 != -1) {
            if (mode1bit) {
                SD_MMC.setPins(_pinConfig.clk, _pinConfig.cmd, _pinConfig.d0);
            } else {
                SD_MMC.setPins(_pinConfig.clk, _pinConfig.cmd, _pinConfig.d0, _pinConfig.d1, _pinConfig.d2, _pinConfig.d3);
            }
        }
#endif

        if (!SD_MMC.begin(_mountPoint.c_str(), mode1bit, _formatIfMountFailed, _sdmmcFrequency, _maxOpenFiles)) {
            triggerError(SDCARD_ERR_MOUNT_FAILED, "SDMMC mount failed");
            return false;
        }

        _fs = &SD_MMC;
        _initialized = true;
        _lastUpdateTime = millis();
        updateCardInfo();

        return true;
    }

    void CeriaModuleESP32SDCard::end() {
        if (!_initialized) return;

        if (_interface == SDCARD_SPI) {
            SD.end();
        } else {
            SD_MMC.end();
        }

        _initialized = false;
        _fs = nullptr;
    }

    void CeriaModuleESP32SDCard::update() {
        if (!_initialized || millis() - _lastUpdateTime < _updateInterval) {
            return;
        }

        updateCardInfo();
        updateStats();

        _hasNewData = true;
        _lastUpdateTime = millis();
    }

    void CeriaModuleESP32SDCard::updateCardInfo() {
        if (!_initialized || !_fs) return;

        if (_interface == SDCARD_SPI) {
            _cardInfo.cardType = (SDCardType) SD.cardType();
            _cardInfo.cardSizeMB = SD.cardSize() / (1024 * 1024);
            _cardInfo.totalSpaceMB = SD.totalBytes() / (1024 * 1024);
            _cardInfo.usedSpaceMB = SD.usedBytes() / (1024 * 1024);
            _cardInfo.sectorSize = SD.sectorSize();
            _cardInfo.numSectors = SD.numSectors();
        } else {
            _cardInfo.cardType = (SDCardType) SD_MMC.cardType();
            _cardInfo.cardSizeMB = SD_MMC.cardSize() / (1024 * 1024);
            _cardInfo.totalSpaceMB = SD_MMC.totalBytes() / (1024 * 1024);
            _cardInfo.usedSpaceMB = SD_MMC.usedBytes() / (1024 * 1024);
            _cardInfo.sectorSize = 512;
            _cardInfo.numSectors = _cardInfo.cardSizeMB * 1024 * 2;
        }

        _cardInfo.freeSpaceMB = _cardInfo.totalSpaceMB - _cardInfo.usedSpaceMB;
        _cardInfo.cardTypeString = getCardTypeString(_cardInfo.cardType);
        _cardInfo.fileSystemType = "FAT32";
        _cardInfo.isValid = (_cardInfo.cardType != SDCARD_TYPE_UNKNOWN);
    }

    void CeriaModuleESP32SDCard::updateStats() {

    }

    bool CeriaModuleESP32SDCard::isInitialized() {
        return _initialized;
    }

    bool CeriaModuleESP32SDCard::hasNewData() {
        bool result = _hasNewData;
        _hasNewData = false;
        return result;
    }

    bool CeriaModuleESP32SDCard::isConnected(uint32_t timeoutMs) {
        if (!_initialized) return false;
        return (millis() - _lastUpdateTime) < timeoutMs && _cardInfo.isValid;
    }

    SDCardInfo CeriaModuleESP32SDCard::getCardInfo() {
        return _cardInfo;
    }

    SDCardStats CeriaModuleESP32SDCard::getStats() {
        return _stats;
    }

    SDCardInterface CeriaModuleESP32SDCard::getInterface() {
        return _interface;
    }

    String CeriaModuleESP32SDCard::getMountPoint() {
        return _mountPoint;
    }

    uint64_t CeriaModuleESP32SDCard::getCardSize() {
        return _cardInfo.cardSizeMB * 1024 * 1024;
    }

    uint64_t CeriaModuleESP32SDCard::getTotalBytes() {
        return _cardInfo.totalSpaceMB * 1024 * 1024;
    }

    uint64_t CeriaModuleESP32SDCard::getUsedBytes() {
        return _cardInfo.usedSpaceMB * 1024 * 1024;
    }

    uint64_t CeriaModuleESP32SDCard::getFreeBytes() {
        return _cardInfo.freeSpaceMB * 1024 * 1024;
    }

    uint32_t CeriaModuleESP32SDCard::getSectorSize() {
        return _cardInfo.sectorSize;
    }

    uint32_t CeriaModuleESP32SDCard::getNumSectors() {
        return _cardInfo.numSectors;
    }

    SDCardType CeriaModuleESP32SDCard::getCardType() {
        return _cardInfo.cardType;
    }

    String CeriaModuleESP32SDCard::getCardTypeString() {
        return _cardInfo.cardTypeString;
    }

    String CeriaModuleESP32SDCard::getCardTypeString(SDCardType type) {
        switch (type) {
            case SDCARD_TYPE_MMC:
                return "MMC";
            case SDCARD_TYPE_SD:
                return "SD";
            case SDCARD_TYPE_SDHC:
                return "SDHC";
            default:
                return "Unknown";
        }
    }

    void CeriaModuleESP32SDCard::setUpdateInterval(uint32_t intervalMs) {
        _updateInterval = intervalMs;
    }

    void CeriaModuleESP32SDCard::setTimeout(uint32_t timeoutMs) {
        _timeout = timeoutMs;
    }

    void CeriaModuleESP32SDCard::setFormatIfMountFailed(bool format) {
        _formatIfMountFailed = format;
    }

    void CeriaModuleESP32SDCard::setMaxOpenFiles(uint8_t maxFiles) {
        _maxOpenFiles = maxFiles;
    }

    void CeriaModuleESP32SDCard::setErrorCallback(SDCardErrorCallback callback) {
        _onError = callback;
    }

    void CeriaModuleESP32SDCard::setFileOperationCallback(SDCardFileCallback callback) {
        _onFileOperation = callback;
    }

    bool CeriaModuleESP32SDCard::exists(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);
        return _fs->exists(fullPath);
    }

    bool CeriaModuleESP32SDCard::isFile(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);
        File file = _fs->open(fullPath);
        if (!file) return false;
        bool result = !file.isDirectory();
        file.close();
        return result;
    }

    bool CeriaModuleESP32SDCard::isDirectory(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);
        File file = _fs->open(fullPath);
        if (!file) return false;
        bool result = file.isDirectory();
        file.close();
        return result;
    }

    uint32_t CeriaModuleESP32SDCard::getFileSize(const String &path) {
        if (!checkInitialized()) return 0;
        String fullPath = normalizePath(path);
        File file = _fs->open(fullPath);
        if (!file) return 0;
        uint32_t size = file.size();
        file.close();
        return size;
    }

    time_t CeriaModuleESP32SDCard::getLastModified(const String &path) {
        if (!checkInitialized()) return 0;
        String fullPath = normalizePath(path);
        File file = _fs->open(fullPath);
        if (!file) return 0;
        time_t time = file.getLastWrite();
        file.close();
        return time;
    }

    File CeriaModuleESP32SDCard::open(const String &path, SDCardFileMode mode) {
        if (!checkInitialized()) return File();
        String fullPath = normalizePath(path);
        
        const char* fileMode;
        switch (mode) {
            case SDCARD_FILE_READ:
                fileMode = FILE_READ;
                break;
            case SDCARD_FILE_WRITE:
                fileMode = FILE_WRITE;
                break;
            case SDCARD_FILE_APPEND:
                fileMode = FILE_APPEND;
                break;
            default:
                fileMode = FILE_READ;
                break;
        }
        
        File file = _fs->open(fullPath, fileMode);
        if (file) {
            incrementStat("open");
        } else {
            triggerError(SDCARD_ERR_FILE_NOT_FOUND, "open: " + path);
        }
        return file;
    }

    bool CeriaModuleESP32SDCard::createFile(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        ensureDirectory(getDirectoryFromPath(fullPath));

        File file = _fs->open(fullPath, FILE_WRITE);
        if (file) {
            file.close();
            incrementStat("createFile");
            triggerFileOperation(path, 0);
            return true;
        }

        triggerError(SDCARD_ERR_CREATE_FAILED, "createFile: " + path);
        return false;
    }

    bool CeriaModuleESP32SDCard::deleteFile(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        if (_fs->remove(fullPath)) {
            incrementStat("deleteFile");
            triggerFileOperation(path, 0);
            return true;
        }

        triggerError(SDCARD_ERR_DELETE_FAILED, "deleteFile: " + path);
        return false;
    }

    bool CeriaModuleESP32SDCard::renameFile(const String &oldPath, const String &newPath) {
        if (!checkInitialized()) return false;
        String fullOldPath = normalizePath(oldPath);
        String fullNewPath = normalizePath(newPath);

        ensureDirectory(getDirectoryFromPath(fullNewPath));

        if (_fs->rename(fullOldPath, fullNewPath)) {
            incrementStat("renameFile");
            triggerFileOperation(newPath, getFileSize(newPath));
            return true;
        }

        triggerError(SDCARD_ERR_RENAME_FAILED, "renameFile: " + oldPath + " -> " + newPath);
        return false;
    }

    bool CeriaModuleESP32SDCard::copyFile(const String &sourcePath, const String &destPath) {
        if (!checkInitialized()) return false;

        String fullSourcePath = normalizePath(sourcePath);
        String fullDestPath = normalizePath(destPath);

        if (!exists(fullSourcePath)) {
            triggerError(SDCARD_ERR_FILE_NOT_FOUND, "copyFile source: " + sourcePath);
            return false;
        }

        ensureDirectory(getDirectoryFromPath(fullDestPath));

        File sourceFile = _fs->open(fullSourcePath, FILE_READ);
        if (!sourceFile) {
            triggerError(SDCARD_ERR_READ_FAILED, "copyFile source open: " + sourcePath);
            return false;
        }

        File destFile = _fs->open(fullDestPath, FILE_WRITE);
        if (!destFile) {
            sourceFile.close();
            triggerError(SDCARD_ERR_CREATE_FAILED, "copyFile dest create: " + destPath);
            return false;
        }

        uint8_t buffer[512];
        uint32_t totalBytes = 0;

        while (sourceFile.available()) {
            int bytesRead = sourceFile.readBytes((char *) buffer, sizeof(buffer));
            if (bytesRead > 0) {
                destFile.write(buffer, bytesRead);
                totalBytes += bytesRead;
            }
        }

        sourceFile.close();
        destFile.close();

        incrementStat("copyFile", totalBytes);
        triggerFileOperation(destPath, totalBytes);
        return true;
    }

    bool CeriaModuleESP32SDCard::moveFile(const String &oldPath, const String &newPath) {
        if (copyFile(oldPath, newPath)) {
            return deleteFile(oldPath);
        }
        return false;
    }

    bool CeriaModuleESP32SDCard::createDirectory(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        if (_fs->mkdir(fullPath)) {
            incrementStat("createDirectory");
            return true;
        }

        triggerError(SDCARD_ERR_CREATE_FAILED, "createDirectory: " + path);
        return false;
    }

    bool CeriaModuleESP32SDCard::deleteDirectory(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        if (_fs->rmdir(fullPath)) {
            incrementStat("deleteDirectory");
            return true;
        }

        triggerError(SDCARD_ERR_DELETE_FAILED, "deleteDirectory: " + path);
        return false;
    }

    bool CeriaModuleESP32SDCard::renameDirectory(const String &oldPath, const String &newPath) {
        if (!checkInitialized()) return false;
        String fullOldPath = normalizePath(oldPath);
        String fullNewPath = normalizePath(newPath);

        if (_fs->rename(fullOldPath, fullNewPath)) {
            incrementStat("renameDirectory");
            return true;
        }

        triggerError(SDCARD_ERR_RENAME_FAILED, "renameDirectory: " + oldPath + " -> " + newPath);
        return false;
    }

    bool CeriaModuleESP32SDCard::writeFile(const String &path, const String &content) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        ensureDirectory(getDirectoryFromPath(fullPath));

        File file = _fs->open(fullPath, FILE_WRITE);
        if (!file) {
            triggerError(SDCARD_ERR_CREATE_FAILED, "writeFile open: " + path);
            return false;
        }

        int bytesWritten = file.print(content);
        file.close();

        if (bytesWritten > 0) {
            incrementStat("writeFile", bytesWritten);
            triggerFileOperation(path, bytesWritten);
            return true;
        }

        triggerError(SDCARD_ERR_WRITE_FAILED, "writeFile: " + path);
        return false;
    }

    bool CeriaModuleESP32SDCard::writeFile(const String &path, const uint8_t *data, uint32_t length) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        ensureDirectory(getDirectoryFromPath(fullPath));

        File file = _fs->open(fullPath, FILE_WRITE);
        if (!file) {
            triggerError(SDCARD_ERR_CREATE_FAILED, "writeFile open: " + path);
            return false;
        }

        int bytesWritten = file.write(data, length);
        file.close();

        if (bytesWritten == (int) length) {
            incrementStat("writeFile", bytesWritten);
            triggerFileOperation(path, bytesWritten);
            return true;
        }

        triggerError(SDCARD_ERR_WRITE_FAILED, "writeFile: " + path);
        return false;
    }

    bool CeriaModuleESP32SDCard::appendFile(const String &path, const String &content) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        ensureDirectory(getDirectoryFromPath(fullPath));

        File file = _fs->open(fullPath, FILE_APPEND);
        if (!file) {
            triggerError(SDCARD_ERR_CREATE_FAILED, "appendFile open: " + path);
            return false;
        }

        int bytesWritten = file.print(content);
        file.close();

        if (bytesWritten > 0) {
            incrementStat("appendFile", bytesWritten);
            triggerFileOperation(path, bytesWritten);
            return true;
        }

        triggerError(SDCARD_ERR_WRITE_FAILED, "appendFile: " + path);
        return false;
    }

    bool CeriaModuleESP32SDCard::appendFile(const String &path, const uint8_t *data, uint32_t length) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        ensureDirectory(getDirectoryFromPath(fullPath));

        File file = _fs->open(fullPath, FILE_APPEND);
        if (!file) {
            triggerError(SDCARD_ERR_CREATE_FAILED, "appendFile open: " + path);
            return false;
        }

        int bytesWritten = file.write(data, length);
        file.close();

        if (bytesWritten == (int) length) {
            incrementStat("appendFile", bytesWritten);
            triggerFileOperation(path, bytesWritten);
            return true;
        }

        triggerError(SDCARD_ERR_WRITE_FAILED, "appendFile: " + path);
        return false;
    }

    String CeriaModuleESP32SDCard::readFile(const String &path) {
        if (!checkInitialized()) return "";
        String fullPath = normalizePath(path);

        File file = _fs->open(fullPath, FILE_READ);
        if (!file) {
            triggerError(SDCARD_ERR_FILE_NOT_FOUND, "readFile: " + path);
            return "";
        }

        String content = file.readString();
        uint32_t bytesRead = content.length();
        file.close();

        incrementStat("readFile", bytesRead);
        return content;
    }

    bool CeriaModuleESP32SDCard::readFile(const String &path, uint8_t *buffer, uint32_t maxLength, uint32_t &bytesRead) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        File file = _fs->open(fullPath, FILE_READ);
        if (!file) {
            triggerError(SDCARD_ERR_FILE_NOT_FOUND, "readFile: " + path);
            return false;
        }

        bytesRead = file.readBytes((char *) buffer, maxLength);
        file.close();

        incrementStat("readFile", bytesRead);
        return true;
    }

    bool CeriaModuleESP32SDCard::writeCSV(const String &path, const String &data, bool append) {
        String content = data;
        if (!content.endsWith("\n")) {
            content += "\n";
        }

        if (append) {
            return appendFile(path, content);
        } else {
            return writeFile(path, content);
        }
    }

    bool CeriaModuleESP32SDCard::writeJSON(const String &path, const String &jsonData, bool pretty) {
        String content = jsonData;
        if (pretty && !content.endsWith("\n")) {
            content += "\n";
        }
        return writeFile(path, content);
    }

    bool CeriaModuleESP32SDCard::writeLog(const String &path, const String &message, bool timestamp) {
        String logEntry = message;

        if (timestamp) {
            char timeStr[32];
            time_t now = time(nullptr);
            strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S] ", localtime(&now));
            logEntry = String(timeStr) + message;
        }

        if (!logEntry.endsWith("\n")) {
            logEntry += "\n";
        }

        return appendFile(path, logEntry);
    }

    String CeriaModuleESP32SDCard::listFiles(const String &directory) {
        if (!checkInitialized()) return "";
        String fullPath = normalizePath(directory);

        File dir = _fs->open(fullPath);
        if (!dir || !dir.isDirectory()) {
            triggerError(SDCARD_ERR_DIR_NOT_FOUND, "listFiles: " + directory);
            return "";
        }

        String result = "";
        File file = dir.openNextFile();

        while (file) {
            if (!file.isDirectory()) {
                result += file.name();
                result += " (" + String(file.size()) + " bytes)\n";
            }
            file = dir.openNextFile();
        }

        dir.close();
        return result;
    }

    String CeriaModuleESP32SDCard::listDirectories(const String &directory) {
        if (!checkInitialized()) return "";
        String fullPath = normalizePath(directory);

        File dir = _fs->open(fullPath);
        if (!dir || !dir.isDirectory()) {
            triggerError(SDCARD_ERR_DIR_NOT_FOUND, "listDirectories: " + directory);
            return "";
        }

        String result = "";
        File file = dir.openNextFile();

        while (file) {
            if (file.isDirectory()) {
                result += file.name();
                result += "/\n";
            }
            file = dir.openNextFile();
        }

        dir.close();
        return result;
    }

    String CeriaModuleESP32SDCard::listAll(const String &directory, bool recursive) {
        if (!checkInitialized()) return "";
        String fullPath = normalizePath(directory);

        File dir = _fs->open(fullPath);
        if (!dir || !dir.isDirectory()) {
            triggerError(SDCARD_ERR_DIR_NOT_FOUND, "listAll: " + directory);
            return "";
        }

        String result = "";
        File file = dir.openNextFile();

        while (file) {
            String fileName = String(file.name());

            if (file.isDirectory()) {
                result += fileName + "/\n";
                if (recursive) {
                    String subPath = directory;
                    if (!subPath.endsWith("/")) subPath += "/";
                    subPath += fileName;
                    result += listAll(subPath, true);
                }
            } else {
                result += fileName + " (" + String(file.size()) + " bytes)\n";
            }

            file = dir.openNextFile();
        }

        dir.close();
        return result;
    }

    uint32_t CeriaModuleESP32SDCard::countFiles(const String &directory, bool recursive) {
        if (!checkInitialized()) return 0;
        String fullPath = normalizePath(directory);

        File dir = _fs->open(fullPath);
        if (!dir || !dir.isDirectory()) return 0;

        uint32_t count = 0;
        File file = dir.openNextFile();

        while (file) {
            if (file.isDirectory()) {
                if (recursive) {
                    String subPath = directory;
                    if (!subPath.endsWith("/")) subPath += "/";
                    subPath += String(file.name());
                    count += countFiles(subPath, true);
                }
            } else {
                count++;
            }
            file = dir.openNextFile();
        }

        dir.close();
        return count;
    }

    uint32_t CeriaModuleESP32SDCard::countDirectories(const String &directory, bool recursive) {
        if (!checkInitialized()) return 0;
        String fullPath = normalizePath(directory);

        File dir = _fs->open(fullPath);
        if (!dir || !dir.isDirectory()) return 0;

        uint32_t count = 0;
        File file = dir.openNextFile();

        while (file) {
            if (file.isDirectory()) {
                count++;
                if (recursive) {
                    String subPath = directory;
                    if (!subPath.endsWith("/")) subPath += "/";
                    subPath += String(file.name());
                    count += countDirectories(subPath, true);
                }
            }
            file = dir.openNextFile();
        }

        dir.close();
        return count;
    }

    bool CeriaModuleESP32SDCard::setWorkingDirectory(const String &path) {
        if (!checkInitialized()) return false;
        String fullPath = normalizePath(path);

        if (isDirectory(fullPath)) {
            _workingDirectory = fullPath;
            if (!_workingDirectory.endsWith("/")) {
                _workingDirectory += "/";
            }
            return true;
        }

        return false;
    }

    String CeriaModuleESP32SDCard::getWorkingDirectory() {
        return _workingDirectory;
    }

    String CeriaModuleESP32SDCard::getCurrentPath(const String &relativePath) {
        if (relativePath.startsWith("/")) {
            return relativePath;
        }

        String result = _workingDirectory;
        if (!result.endsWith("/")) result += "/";
        result += relativePath;
        return normalizePath(result);
    }

    bool CeriaModuleESP32SDCard::format() {
        if (!_initialized) return false;

        end();

        bool success = false;
        if (_interface == SDCARD_SPI) {
            success = SD.begin(_pinConfig.cs);
        } else {
            success = SD_MMC.begin(_mountPoint.c_str(), _interface == SDCARD_SDMMC_1BIT, true);
        }

        if (success) {
            _initialized = true;
            if (_interface == SDCARD_SPI) {
                _fs = &SD;
            } else {
                _fs = &SD_MMC;
            }
            updateCardInfo();
        }

        return success;
    }

    bool CeriaModuleESP32SDCard::test() {
        if (!checkInitialized()) return false;

        String testFile = "/test_" + String(millis()) + ".tmp";
        String testData = "CeriaModuleESP32SDCard test data";

        if (!writeFile(testFile, testData)) return false;

        String readData = readFile(testFile);
        bool success = (readData == testData);

        deleteFile(testFile);
        return success;
    }

    bool CeriaModuleESP32SDCard::benchmark() {
        if (!checkInitialized()) return false;

        String benchFile = "/benchmark.tmp";
        const int testSize = 1024;
        uint8_t testData[testSize];

        for (int i = 0; i < testSize; i++) {
            testData[i] = i % 256;
        }

        uint32_t startTime = millis();
        bool writeSuccess = writeFile(benchFile, testData, testSize);
        uint32_t writeTime = millis() - startTime;

        if (!writeSuccess) return false;

        uint8_t readBuffer[testSize];
        uint32_t bytesRead;

        startTime = millis();
        bool readSuccess = readFile(benchFile, readBuffer, testSize, bytesRead);
        uint32_t readTime = millis() - startTime;

        deleteFile(benchFile);

        if (!readSuccess || bytesRead != testSize) return false;

        float writeSpeed = (float) testSize / writeTime * 1000.0;
        float readSpeed = (float) testSize / readTime * 1000.0;

        Serial.println("SD Card Benchmark Results:");
        Serial.println("Write Speed: " + formatSpeed(writeSpeed));
        Serial.println("Read Speed: " + formatSpeed(readSpeed));

        return true;
    }

    float CeriaModuleESP32SDCard::getReadSpeed() {
        return 0.0;
    }

    float CeriaModuleESP32SDCard::getWriteSpeed() {
        return 0.0;
    }

    String CeriaModuleESP32SDCard::getSDCardString() {
        if (!_cardInfo.isValid) return "SD Card: Not available";

        return "SD Card: " + _cardInfo.cardTypeString +
               " (" + String(_cardInfo.cardSizeMB) + "MB)" +
               " via " + (_interface == SDCARD_SPI ? "SPI" : "SDMMC");
    }

    String CeriaModuleESP32SDCard::getInfoString() {
        if (!_cardInfo.isValid) return "SD Card Info: Not available";

        return "Type: " + _cardInfo.cardTypeString +
               ", Size: " + String(_cardInfo.cardSizeMB) + "MB" +
               ", Used: " + String(_cardInfo.usedSpaceMB) + "MB" +
               ", Free: " + String(_cardInfo.freeSpaceMB) + "MB";
    }

    String CeriaModuleESP32SDCard::getStatsString() {
        return "Files: R" + String(_stats.filesRead) +
               "/W" + String(_stats.filesWritten) +
               "/C" + String(_stats.filesCreated) +
               "/D" + String(_stats.filesDeleted) +
               ", Bytes: R" + formatBytes(_stats.bytesRead) +
               "/W" + formatBytes(_stats.bytesWritten) +
               ", Errors: " + String(_stats.errorCount);
    }

    String CeriaModuleESP32SDCard::getStatusString() {
        String status = _initialized ? "Initialized" : "Not initialized";
        if (_initialized) {
            status += ", " + getInfoString();
        }
        return status;
    }

    String CeriaModuleESP32SDCard::getErrorString(SDCardError error) {
        switch (error) {
            case SDCARD_ERR_NONE:
                return "No error";
            case SDCARD_ERR_MOUNT_FAILED:
                return "Mount failed";
            case SDCARD_ERR_NO_CARD:
                return "No card detected";
            case SDCARD_ERR_CARD_UNKNOWN:
                return "Unknown card type";
            case SDCARD_ERR_FILE_NOT_FOUND:
                return "File not found";
            case SDCARD_ERR_DIR_NOT_FOUND:
                return "Directory not found";
            case SDCARD_ERR_WRITE_FAILED:
                return "Write failed";
            case SDCARD_ERR_READ_FAILED:
                return "Read failed";
            case SDCARD_ERR_DELETE_FAILED:
                return "Delete failed";
            case SDCARD_ERR_CREATE_FAILED:
                return "Create failed";
            case SDCARD_ERR_RENAME_FAILED:
                return "Rename failed";
            case SDCARD_ERR_COPY_FAILED:
                return "Copy failed";
            case SDCARD_ERR_INSUFFICIENT_SPACE:
                return "Insufficient space";
            case SDCARD_ERR_INVALID_PATH:
                return "Invalid path";
            case SDCARD_ERR_NOT_INITIALIZED:
                return "Not initialized";
            default:
                return "Unknown error";
        }
    }

    void CeriaModuleESP32SDCard::resetStats() {
        memset(&_stats, 0, sizeof(_stats));
    }

    void CeriaModuleESP32SDCard::printInfo() {
        Serial.println("=== SD Card Information ===");
        Serial.println(getStatusString());
        Serial.println("Interface: " + String(_interface == SDCARD_SPI ? "SPI" : "SDMMC"));
        Serial.println("Mount Point: " + _mountPoint);
        Serial.println("Working Directory: " + _workingDirectory);
    }

    void CeriaModuleESP32SDCard::printStats() {
        Serial.println("=== SD Card Statistics ===");
        Serial.println(getStatsString());
        Serial.println("Operations Total: " + String(_stats.operationsTotal));
        if (_stats.lastError != SDCARD_ERR_NONE) {
            Serial.println("Last Error: " + getErrorString(_stats.lastError));
        }
    }

    void CeriaModuleESP32SDCard::printDirectory(const String &path, bool recursive) {
        Serial.println("=== Directory Listing: " + path + " ===");
        Serial.print(listAll(path, recursive));
    }

    bool CeriaModuleESP32SDCard::waitForCard(uint32_t timeoutMs) {
        uint32_t startTime = millis();

        while (millis() - startTime < timeoutMs) {
            update();
            if (_cardInfo.isValid) {
                return true;
            }
            delay(100);
        }

        return false;
    }

    bool CeriaModuleESP32SDCard::waitForFile(const String &path, uint32_t timeoutMs) {
        uint32_t startTime = millis();

        while (millis() - startTime < timeoutMs) {
            if (exists(path)) {
                return true;
            }
            delay(100);
        }

        return false;
    }

    void CeriaModuleESP32SDCard::enableAutoUpdate(bool enable) {

    }

    void CeriaModuleESP32SDCard::enableVerbose(bool enable) {

    }

    bool CeriaModuleESP32SDCard::backupFile(const String &sourcePath, const String &backupDir) {
        String backupPath = backupDir;
        if (!backupPath.endsWith("/")) backupPath += "/";
        backupPath += getFilenameFromPath(sourcePath);

        ensureDirectory(backupDir);
        return copyFile(sourcePath, backupPath);
    }

    bool CeriaModuleESP32SDCard::restoreFile(const String &backupPath, const String &destPath) {
        return copyFile(backupPath, destPath);
    }

    bool CeriaModuleESP32SDCard::archiveDirectory(const String &sourceDir, const String &archivePath) {
        return false;
    }

    String CeriaModuleESP32SDCard::generateUniqueFilename(const String &basePath, const String &extension) {
        String path = basePath;
        String ext = extension;

        if (!ext.startsWith(".")) {
            ext = "." + ext;
        }

        int counter = 0;
        String uniquePath = path + ext;

        while (exists(uniquePath)) {
            counter++;
            uniquePath = path + "_" + String(counter) + ext;
        }

        return uniquePath;
    }

    bool CeriaModuleESP32SDCard::ensureDirectory(const String &path) {
        if (exists(path)) {
            return isDirectory(path);
        }

        String currentPath = "";
        int start = 0;
        int end = 0;

        while ((end = path.indexOf('/', start)) != -1) {
            currentPath = path.substring(0, end);
            if (currentPath.length() > 0 && !exists(currentPath)) {
                if (!createDirectory(currentPath)) {
                    return false;
                }
            }
            start = end + 1;
        }

        if (!exists(path)) {
            return createDirectory(path);
        }

        return true;
    }

    bool CeriaModuleESP32SDCard::cleanupOldFiles(const String &directory, uint32_t maxAgeMs, uint32_t maxFiles) {
        if (!checkInitialized()) return false;

        File dir = _fs->open(directory);
        if (!dir || !dir.isDirectory()) return false;

        uint32_t currentTime = millis();
        uint32_t deletedCount = 0;

        File file = dir.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                time_t fileTime = file.getLastWrite();
                uint32_t fileAge = currentTime - (fileTime * 1000);

                if (fileAge > maxAgeMs) {
                    String filePath = directory;
                    if (!filePath.endsWith("/")) filePath += "/";
                    filePath += String(file.name());

                    if (deleteFile(filePath)) {
                        deletedCount++;
                    }
                }
            }
            file = dir.openNextFile();
        }

        dir.close();
        return deletedCount > 0;
    }

    SDCardError CeriaModuleESP32SDCard::getLastError() {
        return _stats.lastError;
    }

    String CeriaModuleESP32SDCard::getLastOperation() {
        return _lastOperation;
    }

    void CeriaModuleESP32SDCard::clearLastError() {
        _stats.lastError = SDCARD_ERR_NONE;
        _lastOperation = "";
    }

    String CeriaModuleESP32SDCard::formatBytes(uint64_t bytes) {
        const char *units[] = {"B", "KB", "MB", "GB", "TB"};
        int unitIndex = 0;
        float value = bytes;

        while (value >= 1024 && unitIndex < 4) {
            value /= 1024;
            unitIndex++;
        }

        return String(value, 2) + " " + units[unitIndex];
    }

    String CeriaModuleESP32SDCard::formatSpeed(float bytesPerSecond) {
        return formatBytes((uint64_t) bytesPerSecond) + "/s";
    }

    bool CeriaModuleESP32SDCard::isValidFilename(const String &filename) {
        if (filename.length() == 0 || filename.length() > CERIA_SDCARD_MAX_FILENAME_LENGTH) {
            return false;
        }

        const char *invalidChars = "<>:\"|?*";
        for (int i = 0; i < filename.length(); i++) {
            char c = filename.charAt(i);
            if (c < 32 || strchr(invalidChars, c)) {
                return false;
            }
        }

        return true;
    }

    String CeriaModuleESP32SDCard::sanitizeFilename(const String &filename) {
        String result = filename;
        const char *invalidChars = "<>:\"|?*";

        for (int i = 0; i < result.length(); i++) {
            char c = result.charAt(i);
            if (c < 32 || strchr(invalidChars, c)) {
                result.setCharAt(i, '_');
            }
        }

        if (result.length() > CERIA_SDCARD_MAX_FILENAME_LENGTH) {
            result = result.substring(0, CERIA_SDCARD_MAX_FILENAME_LENGTH);
        }

        return result;
    }

    void CeriaModuleESP32SDCard::triggerError(SDCardError error, const String &operation) {
        _stats.lastError = error;
        _stats.lastErrorTime = millis();
        _stats.errorCount++;
        _lastOperation = operation;

        if (_onError) {
            _onError(error, operation);
        }
    }

    void CeriaModuleESP32SDCard::triggerFileOperation(const String &filename, uint32_t size) {
        if (_onFileOperation) {
            _onFileOperation(filename, size);
        }
    }

    bool CeriaModuleESP32SDCard::validatePath(const String &path) {
        if (path.length() == 0 || path.length() > CERIA_SDCARD_MAX_PATH_LENGTH) {
            return false;
        }

        if (!path.startsWith("/")) {
            return false;
        }

        return true;
    }

    String CeriaModuleESP32SDCard::normalizePath(const String &path) {
        if (path.startsWith("/")) {
            return path;
        }

        return _workingDirectory + path;
    }

    String CeriaModuleESP32SDCard::getDirectoryFromPath(const String &path) {
        int lastSlash = path.lastIndexOf('/');
        if (lastSlash == -1) {
            return "/";
        }

        return path.substring(0, lastSlash);
    }

    String CeriaModuleESP32SDCard::getFilenameFromPath(const String &path) {
        int lastSlash = path.lastIndexOf('/');
        if (lastSlash == -1) {
            return path;
        }

        return path.substring(lastSlash + 1);
    }

    void CeriaModuleESP32SDCard::incrementStat(const String &operation, uint32_t bytes) {
        _stats.operationsTotal++;

        if (operation.indexOf("read") != -1) {
            _stats.filesRead++;
            _stats.bytesRead += bytes;
        } else if (operation.indexOf("write") != -1) {
            _stats.filesWritten++;
            _stats.bytesWritten += bytes;
        } else if (operation.indexOf("create") != -1) {
            if (operation.indexOf("Directory") != -1) {
                _stats.directoriesCreated++;
            } else {
                _stats.filesCreated++;
            }
        } else if (operation.indexOf("delete") != -1) {
            if (operation.indexOf("Directory") != -1) {
                _stats.directoriesDeleted++;
            } else {
                _stats.filesDeleted++;
            }
        }
    }

    bool CeriaModuleESP32SDCard::checkInitialized() {
        if (!_initialized) {
            triggerError(SDCARD_ERR_NOT_INITIALIZED, "checkInitialized");
            return false;
        }
        return true;
    }

}

#endif