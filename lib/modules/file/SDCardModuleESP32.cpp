#include "SDCardModuleESP32.h"

#if defined(ESP32)
#include <new>

SDCardModuleESP32::SDCardModuleESP32(uint8_t csPin)
        : _csPin(csPin),
          _initialized(false),
          _bufferSize(1024),
          _primaryBuffer(nullptr),
          _secondaryBuffer(nullptr),
          _useDoubleBuffer(false),
          _lastError(SDError::None) {
    
    _primaryBuffer = new (std::nothrow) uint8_t[_bufferSize];
    if (_primaryBuffer) {
        memset(_primaryBuffer, 0, _bufferSize);
    }
    
    for (int i = 0; i < _maxFileHandles; i++) {
        _fileHandles[i].inUse = false;
        _fileHandles[i].lastAccess = 0;
    }
    
    for (int i = 0; i < _maxCacheEntries; i++) {
        _dirCache[i].valid = false;
        _dirCache[i].cacheTime = 0;
    }
    
    memset(&_stats, 0, sizeof(_stats));
}

SDCardModuleESP32::~SDCardModuleESP32() {
    end();
    if (_primaryBuffer) {
        delete[] _primaryBuffer;
        _primaryBuffer = nullptr;
    }
    if (_secondaryBuffer) {
        delete[] _secondaryBuffer;
        _secondaryBuffer = nullptr;
    }
    
    for (int i = 0; i < _maxFileHandles; i++) {
        if (_fileHandles[i].inUse && _fileHandles[i].file) {
            _fileHandles[i].file.close();
        }
    }
}

bool SDCardModuleESP32::begin() {
    if (_initialized) {
        return true;
    }
    
    if (!_primaryBuffer) {
        setError(SDError::OutOfMemory, "Primary buffer not allocated");
        return false;
    }

    _initialized = SD.begin(_csPin);
    if (!_initialized) {
        setError(SDError::NotInitialized, "SD card initialization failed");
    }
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

    return (usedBytes() * 100.0f) / total;
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
        entry.close();
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

    size_t fileSize = entry.size();
    if (fileSize > 32768) {
        entry.close();
        return "";
    }

    String content = "";
    content.reserve(fileSize);
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
        setError(SDError::NotInitialized, "SD card not initialized");
        return false;
    }
    
    if (!_primaryBuffer) {
        setError(SDError::OutOfMemory, "Primary buffer not allocated");
        return false;
    }

    fs::File entry = SD.open(path);
    if (!entry) {
        setError(SDError::FileNotFound, String("Cannot open file: ") + path);
        return false;
    }

    size_t len = entry.size();
    size_t flen = len;

    uint32_t start = millis();
    while (len) {
        size_t toRead = len > _bufferSize ? _bufferSize : len;
        entry.read(_primaryBuffer, toRead);
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
        entry.write(_primaryBuffer, _bufferSize);
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

    SPI.end();
    SPI.begin();
    
    bool result = SD.begin(_csPin);
    if (!result) {
        Serial.println("Format failed - card initialization error");
        return false;
    }
    
    _initialized = result;
    Serial.println("Format complete");
    return result;
}

void SDCardModuleESP32::setError(SDError error, const String& message) {
    _lastError = error;
    _lastErrorMessage = message;
    _stats.totalErrors++;
}

void SDCardModuleESP32::updateStats(bool isRead, size_t bytes, unsigned long duration) {
    if (isRead) {
        _stats.totalReads++;
        _stats.totalBytesRead += bytes;
        _stats.averageReadTime = (_stats.averageReadTime + duration) / 2;
    } else {
        _stats.totalWrites++;
        _stats.totalBytesWritten += bytes;
        _stats.averageWriteTime = (_stats.averageWriteTime + duration) / 2;
    }
}

FileHandle* SDCardModuleESP32::getAvailableHandle() {
    closeExpiredHandles();
    
    for (int i = 0; i < _maxFileHandles; i++) {
        if (!_fileHandles[i].inUse) {
            return &_fileHandles[i];
        }
    }
    
    unsigned long oldestTime = millis();
    int oldestIndex = 0;
    for (int i = 0; i < _maxFileHandles; i++) {
        if (_fileHandles[i].lastAccess < oldestTime) {
            oldestTime = _fileHandles[i].lastAccess;
            oldestIndex = i;
        }
    }
    
    if (_fileHandles[oldestIndex].inUse && _fileHandles[oldestIndex].file) {
        _fileHandles[oldestIndex].file.close();
    }
    _fileHandles[oldestIndex].inUse = false;
    
    return &_fileHandles[oldestIndex];
}

FileHandle* SDCardModuleESP32::findHandle(const String& path) {
    for (int i = 0; i < _maxFileHandles; i++) {
        if (_fileHandles[i].inUse && _fileHandles[i].path == path) {
            _fileHandles[i].lastAccess = millis();
            return &_fileHandles[i];
        }
    }
    return nullptr;
}

void SDCardModuleESP32::closeExpiredHandles() {
    unsigned long currentTime = millis();
    for (int i = 0; i < _maxFileHandles; i++) {
        if (_fileHandles[i].inUse && 
            (currentTime - _fileHandles[i].lastAccess) > _handleTimeout) {
            if (_fileHandles[i].file) {
                _fileHandles[i].file.close();
            }
            _fileHandles[i].inUse = false;
        }
    }
}

DirectoryCache* SDCardModuleESP32::getCachedDir(const String& path) {
    unsigned long currentTime = millis();
    for (int i = 0; i < _maxCacheEntries; i++) {
        if (_dirCache[i].valid && 
            _dirCache[i].path == path &&
            (currentTime - _dirCache[i].cacheTime) < _cacheTimeout) {
            return &_dirCache[i];
        }
    }
    return nullptr;
}

DirectoryCache* SDCardModuleESP32::getAvailableCacheSlot() {
    for (int i = 0; i < _maxCacheEntries; i++) {
        if (!_dirCache[i].valid) {
            return &_dirCache[i];
        }
    }
    
    unsigned long oldestTime = millis();
    int oldestIndex = 0;
    for (int i = 0; i < _maxCacheEntries; i++) {
        if (_dirCache[i].cacheTime < oldestTime) {
            oldestTime = _dirCache[i].cacheTime;
            oldestIndex = i;
        }
    }
    
    _dirCache[oldestIndex].files.clear();
    _dirCache[oldestIndex].dirs.clear();
    return &_dirCache[oldestIndex];
}

void SDCardModuleESP32::cacheDirectory(const String& path, const std::vector<String>& files, const std::vector<String>& dirs) {
    DirectoryCache* cache = getAvailableCacheSlot();
    if (cache) {
        cache->path = path;
        cache->files = files;
        cache->dirs = dirs;
        cache->cacheTime = millis();
        cache->valid = true;
    }
}

uint32_t SDCardModuleESP32::calculateCRC32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    static const uint32_t crcTable[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
        0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
        0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
        0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
        0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
        0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
        0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
        0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
        0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
        0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
        0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
        0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
        0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
        0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
        0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
        0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
        0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
        0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
        0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
        0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
        0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
        0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };
    
    for (size_t i = 0; i < length; i++) {
        crc = crcTable[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFF;
}

size_t SDCardModuleESP32::getOptimalBufferSize(size_t fileSize) {
    if (fileSize < 1024) return 512;
    if (fileSize < 10240) return 1024;
    if (fileSize < 102400) return 2048;
    return 4096;
}

SDError SDCardModuleESP32::getLastError() const {
    return _lastError;
}

String SDCardModuleESP32::getLastErrorMessage() const {
    return _lastErrorMessage;
}

void SDCardModuleESP32::clearError() {
    _lastError = SDError::None;
    _lastErrorMessage = "";
}

bool SDCardModuleESP32::setBufferSize(size_t size) {
    if (size < 512 || size > 8192) {
        setError(SDError::InvalidPath, "Buffer size must be between 512 and 8192 bytes");
        return false;
    }
    
    if (_primaryBuffer) {
        delete[] _primaryBuffer;
    }
    if (_secondaryBuffer) {
        delete[] _secondaryBuffer;
    }
    
    _bufferSize = size;
    _primaryBuffer = new (std::nothrow) uint8_t[_bufferSize];
    if (!_primaryBuffer) {
        setError(SDError::OutOfMemory, "Failed to allocate primary buffer");
        return false;
    }
    
    if (_useDoubleBuffer) {
        _secondaryBuffer = new (std::nothrow) uint8_t[_bufferSize];
        if (!_secondaryBuffer) {
            setError(SDError::OutOfMemory, "Failed to allocate secondary buffer");
            return false;
        }
    }
    
    return true;
}

void SDCardModuleESP32::enableDoubleBuffering(bool enable) {
    _useDoubleBuffer = enable;
    
    if (enable && !_secondaryBuffer) {
        _secondaryBuffer = new (std::nothrow) uint8_t[_bufferSize];
        if (_secondaryBuffer) {
            memset(_secondaryBuffer, 0, _bufferSize);
        }
    } else if (!enable && _secondaryBuffer) {
        delete[] _secondaryBuffer;
        _secondaryBuffer = nullptr;
    }
}

bool SDCardModuleESP32::readFileStream(const char* path, size_t offset, size_t length, uint8_t* buffer) {
    if (!_initialized) {
        setError(SDError::NotInitialized, "SD card not initialized");
        return false;
    }
    
    if (!buffer) {
        setError(SDError::InvalidPath, "Buffer is null");
        return false;
    }
    
    unsigned long startTime = millis();
    
    FileHandle* handle = findHandle(String(path));
    fs::File file;
    
    if (!handle) {
        file = SD.open(path, FILE_READ);
        if (!file) {
            setError(SDError::FileNotFound, String("Cannot open file: ") + path);
            return false;
        }
    } else {
        file = handle->file;
    }
    
    if (!file.seek(offset)) {
        setError(SDError::ReadError, "Cannot seek to offset");
        if (!handle) file.close();
        return false;
    }
    
    size_t bytesRead = file.read(buffer, length);
    
    if (!handle) {
        file.close();
    }
    
    updateStats(true, bytesRead, millis() - startTime);
    
    if (bytesRead != length) {
        setError(SDError::ReadError, "Read fewer bytes than requested");
        return false;
    }
    
    return true;
}

bool SDCardModuleESP32::writeFileStream(const char* path, const uint8_t* data, size_t length, bool append) {
    if (!_initialized) {
        setError(SDError::NotInitialized, "SD card not initialized");
        return false;
    }
    
    if (!data) {
        setError(SDError::InvalidPath, "Data is null");
        return false;
    }
    
    unsigned long startTime = millis();
    
    fs::File file = SD.open(path, append ? FILE_APPEND : FILE_WRITE);
    if (!file) {
        setError(SDError::WriteError, String("Cannot open file for writing: ") + path);
        return false;
    }
    
    size_t totalWritten = 0;
    size_t bufferSize = getOptimalBufferSize(length);
    
    while (totalWritten < length) {
        size_t toWrite = min(bufferSize, length - totalWritten);
        size_t written = file.write(data + totalWritten, toWrite);
        
        if (written != toWrite) {
            setError(SDError::WriteError, "Write operation failed");
            file.close();
            return false;
        }
        
        totalWritten += written;
    }
    
    file.close();
    updateStats(false, totalWritten, millis() - startTime);
    
    return true;
}

bool SDCardModuleESP32::copyFile(const char* source, const char* destination) {
    if (!_initialized) {
        setError(SDError::NotInitialized, "SD card not initialized");
        return false;
    }
    
    fs::File srcFile = SD.open(source, FILE_READ);
    if (!srcFile) {
        setError(SDError::FileNotFound, String("Source file not found: ") + source);
        return false;
    }
    
    fs::File dstFile = SD.open(destination, FILE_WRITE);
    if (!dstFile) {
        setError(SDError::WriteError, String("Cannot create destination file: ") + destination);
        srcFile.close();
        return false;
    }
    
    size_t bufferSize = getOptimalBufferSize(srcFile.size());
    uint8_t* buffer = _useDoubleBuffer ? _secondaryBuffer : _primaryBuffer;
    
    if (!buffer) {
        setError(SDError::OutOfMemory, "Buffer not allocated");
        srcFile.close();
        dstFile.close();
        return false;
    }
    
    while (srcFile.available()) {
        size_t bytesRead = srcFile.read(buffer, bufferSize);
        size_t bytesWritten = dstFile.write(buffer, bytesRead);
        
        if (bytesWritten != bytesRead) {
            setError(SDError::WriteError, "Copy operation failed");
            srcFile.close();
            dstFile.close();
            return false;
        }
    }
    
    srcFile.close();
    dstFile.close();
    
    return true;
}

bool SDCardModuleESP32::moveFile(const char* source, const char* destination) {
    if (copyFile(source, destination)) {
        return deleteFile(source);
    }
    return false;
}

bool SDCardModuleESP32::batchDelete(const std::vector<String>& paths) {
    bool allSuccess = true;
    
    for (const String& path : paths) {
        if (!deleteFile(path.c_str())) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool SDCardModuleESP32::batchCopy(const std::vector<String>& sources, const std::vector<String>& destinations) {
    if (sources.size() != destinations.size()) {
        setError(SDError::InvalidPath, "Source and destination arrays must have same size");
        return false;
    }
    
    bool allSuccess = true;
    
    for (size_t i = 0; i < sources.size(); i++) {
        if (!copyFile(sources[i].c_str(), destinations[i].c_str())) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

uint32_t SDCardModuleESP32::getFileCRC32(const char* path) {
    if (!_initialized) {
        setError(SDError::NotInitialized, "SD card not initialized");
        return 0;
    }
    
    if (!_primaryBuffer) {
        setError(SDError::OutOfMemory, "Primary buffer not allocated");
        return 0;
    }
    
    fs::File file = SD.open(path, FILE_READ);
    if (!file) {
        setError(SDError::FileNotFound, String("File not found: ") + path);
        return 0;
    }
    
    String content = "";
    while (file.available()) {
        size_t bytesRead = file.read(_primaryBuffer, _bufferSize);
        for (size_t i = 0; i < bytesRead; i++) {
            content += (char)_primaryBuffer[i];
        }
    }
    
    file.close();
    return calculateCRC32((const uint8_t*)content.c_str(), content.length());
}

bool SDCardModuleESP32::verifyFileIntegrity(const char* path, uint32_t expectedCRC) {
    uint32_t actualCRC = getFileCRC32(path);
    
    if (actualCRC != expectedCRC) {
        setError(SDError::ChecksumMismatch, "File integrity check failed");
        return false;
    }
    
    return true;
}

bool SDCardModuleESP32::writeFileWithChecksum(const char* path, const char* data) {
    String dataStr = String(data);
    uint32_t crc = calculateCRC32((const uint8_t*)data, dataStr.length());
    
    String content = dataStr + "\n#CRC32:" + String(crc, HEX);
    
    return writeFile(path, content.c_str());
}

bool SDCardModuleESP32::readFileWithChecksum(const char* path, String& data) {
    String content = readFileString(path);
    
    int crcPos = content.lastIndexOf("\n#CRC32:");
    if (crcPos == -1) {
        setError(SDError::ChecksumMismatch, "No checksum found in file");
        return false;
    }
    
    data = content.substring(0, crcPos);
    String crcStr = content.substring(crcPos + 8);
    
    uint32_t expectedCRC = strtoul(crcStr.c_str(), nullptr, 16);
    uint32_t actualCRC = calculateCRC32((const uint8_t*)data.c_str(), data.length());
    
    if (actualCRC != expectedCRC) {
        setError(SDError::ChecksumMismatch, "Checksum verification failed");
        return false;
    }
    
    return true;
}

void SDCardModuleESP32::printPerformanceStats() {
    Serial.println("=== SD Card Performance Statistics ===");
    Serial.printf("Total Reads: %lu\n", _stats.totalReads);
    Serial.printf("Total Writes: %lu\n", _stats.totalWrites);
    Serial.printf("Total Bytes Read: %lu\n", _stats.totalBytesRead);
    Serial.printf("Total Bytes Written: %lu\n", _stats.totalBytesWritten);
    Serial.printf("Total Errors: %lu\n", _stats.totalErrors);
    Serial.printf("Average Read Time: %lu ms\n", _stats.averageReadTime);
    Serial.printf("Average Write Time: %lu ms\n", _stats.averageWriteTime);
    
    if (_stats.totalReads > 0) {
        Serial.printf("Read Throughput: %.2f KB/s\n", 
                     (_stats.totalBytesRead / 1024.0) / (_stats.averageReadTime / 1000.0));
    }
    
    if (_stats.totalWrites > 0) {
        Serial.printf("Write Throughput: %.2f KB/s\n", 
                     (_stats.totalBytesWritten / 1024.0) / (_stats.averageWriteTime / 1000.0));
    }
}

void SDCardModuleESP32::resetPerformanceStats() {
    memset(&_stats, 0, sizeof(_stats));
}

PerformanceStats SDCardModuleESP32::getPerformanceStats() const {
    return _stats;
}

void SDCardModuleESP32::clearDirectoryCache() {
    for (int i = 0; i < _maxCacheEntries; i++) {
        _dirCache[i].valid = false;
        _dirCache[i].files.clear();
        _dirCache[i].dirs.clear();
    }
}

bool SDCardModuleESP32::refreshDirectoryCache(const char* path) {
    if (!_initialized) {
        setError(SDError::NotInitialized, "SD card not initialized");
        return false;
    }
    
    fs::File root = SD.open(path);
    if (!root || !root.isDirectory()) {
        setError(SDError::InvalidPath, String("Invalid directory: ") + path);
        return false;
    }
    
    std::vector<String> files;
    std::vector<String> dirs;
    
    fs::File entry = root.openNextFile();
    while (entry) {
        if (entry.isDirectory()) {
            dirs.push_back(String(entry.name()));
        } else {
            files.push_back(String(entry.name()));
        }
        entry.close();
        entry = root.openNextFile();
    }
    
    root.close();
    cacheDirectory(String(path), files, dirs);
    
    return true;
}

size_t SDCardModuleESP32::getAvailableMemory() {
    return ESP.getFreeHeap();
}

void SDCardModuleESP32::optimizeMemoryUsage() {
    closeExpiredHandles();
    
    unsigned long currentTime = millis();
    for (int i = 0; i < _maxCacheEntries; i++) {
        if (_dirCache[i].valid && 
            (currentTime - _dirCache[i].cacheTime) > (_cacheTimeout / 2)) {
            _dirCache[i].valid = false;
            _dirCache[i].files.clear();
            _dirCache[i].dirs.clear();
        }
    }
    
    size_t availableMemory = getAvailableMemory();
    if (availableMemory < 10240) {
        if (_useDoubleBuffer && _secondaryBuffer) {
            delete[] _secondaryBuffer;
            _secondaryBuffer = nullptr;
            _useDoubleBuffer = false;
        }
        
        if (_bufferSize > 512) {
            setBufferSize(512);
        }
    }
}

#endif