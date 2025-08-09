# CeriaModuleESP32SDCard - Comprehensive ESP32 SD Card Management

## Overview

CeriaModuleESP32SDCard adalah wrapper canggih untuk manajemen SD card pada ESP32 yang menyediakan unified API untuk interface SPI dan SDMMC. Module ini dirancang untuk aplikasi data logging, file storage, dan manajemen sistem file yang reliable dengan fitur enterprise-grade.

## Key Features

### Multi-Interface Support
- **SPI Interface**: Compatible dengan semua ESP32 variants, flexible pin configuration
- **SDMMC 1-bit Mode**: Faster performance, minimal pins (CLK, CMD, D0)
- **SDMMC 4-bit Mode**: Maximum performance, requires 6 pins (CLK, CMD, D0-D3)
- **Auto Interface Detection**: Optimal interface selection berdasarkan hardware

### Advanced File Operations
- **Standard Operations**: Create, read, write, delete, rename, copy, move files
- **Directory Management**: Create, delete, rename directories dengan recursive operations
- **Specialized Formats**: CSV, JSON, dan log file writing dengan timestamp
- **Batch Operations**: Efficient multiple file processing
- **Working Directory**: Current directory support untuk relative paths

### Performance & Reliability
- **Comprehensive Error Handling**: 15 error types dengan callback system
- **Statistics Tracking**: Detailed operation statistics dan performance metrics
- **Connection Monitoring**: Real-time card presence dan health monitoring
- **Benchmark Tools**: Built-in speed testing dan performance analysis
- **Format Support**: FAT16/FAT32 dengan format capability

### Data Management Features
- **File Backup/Restore**: Automated backup system dengan versioning
- **Directory Archiving**: Compress dan archive directory structures
- **Cleanup Tools**: Age-based file cleanup dan space management
- **Unique Filename Generation**: Collision-free filename generation
- **Path Validation**: Comprehensive path dan filename validation

## Technical Specifications

### Performance Characteristics
- **SPI Mode**: Up to 40MHz (default 4MHz), ~2-4MB/s transfer rates
- **SDMMC 1-bit**: ~8-12MB/s transfer rates, minimal pin usage
- **SDMMC 4-bit**: ~15-25MB/s transfer rates, maximum performance
- **Memory Overhead**: ~2KB RAM untuk statistics dan buffers

### Platform Compatibility
- **ESP32 Classic**: Full SPI + SDMMC support (fixed pins)
- **ESP32-S3**: Full SPI + SDMMC support dengan flexible pin assignment
- **ESP32-C3/C6**: SPI only (SDMMC not available)

### Card Support
- **Types**: SD, SDHC, SDXC, MMC cards
- **Capacity**: 4GB to 2TB (FAT32 limitation: up to 32GB efficient)
- **Speed Classes**: Class 4 to UHS-1, optimal performance dengan Class 10+
- **File Systems**: FAT16, FAT32 (exFAT tidak supported)

## Hardware Connections

### SPI Interface (Default Pins)
```cpp
// ESP32 Default SPI Pins
SCK  (CLK)  -> GPIO 18
MISO (DO)   -> GPIO 19  
MOSI (DI)   -> GPIO 23
CS   (CS)   -> GPIO 5
VCC         -> 3.3V
GND         -> GND
```

### SDMMC Interface (ESP32 Fixed Pins)
```cpp
// ESP32 Default SDMMC Pins
CLK  -> GPIO 14
CMD  -> GPIO 15
D0   -> GPIO 2
D1   -> GPIO 4  (4-bit mode only)
D2   -> GPIO 12 (4-bit mode only)
D3   -> GPIO 13 (4-bit mode only)
VCC  -> 3.3V
GND  -> GND
```

**Important**: SDMMC pins need external 10kΩ pull-up resistors pada data lines.

### ESP32-S3 Custom Pin Assignment
```cpp
// ESP32-S3 Custom SDMMC Pins (example)
CLK  -> GPIO 33
CMD  -> GPIO 34
D0   -> GPIO 35
D1   -> GPIO 36 (4-bit mode)
D2   -> GPIO 37 (4-bit mode)  
D3   -> GPIO 38 (4-bit mode)
```

## API Reference

### Initialization Methods

#### Basic Initialization
```cpp
// Auto-detect interface (SDMMC 1-bit default)
bool begin(SDCardInterface interface = SDCARD_SDMMC_1BIT, const String &mountPoint = "/sdcard");

// SPI with custom pins
bool beginSPI(int8_t sck = 18, int8_t miso = 19, int8_t mosi = 23, int8_t cs = 5, uint32_t frequency = 4000000);

// SDMMC with mode selection
bool beginSDMMC(bool mode1bit = true, int8_t clk = -1, int8_t cmd = -1, int8_t d0 = -1, int8_t d1 = -1, int8_t d2 = -1, int8_t d3 = -1);

// SDMMC 1-bit simplified
bool beginSDMMC1Bit(int8_t clk = 14, int8_t cmd = 15, int8_t d0 = 2);

// SDMMC 4-bit simplified  
bool beginSDMMC4Bit(int8_t clk = 14, int8_t cmd = 15, int8_t d0 = 2, int8_t d1 = 4, int8_t d2 = 12, int8_t d3 = 13);
```

### File Operations

#### Basic File Operations
```cpp
// Check existence
bool exists(const String &path);
bool isFile(const String &path);
bool isDirectory(const String &path);

// File information
uint32_t getFileSize(const String &path);
time_t getLastModified(const String &path);

// Open file (returns Arduino File object)
File open(const String &path, SDCardFileMode mode = SDCARD_FILE_READ);

// Create/delete/rename
bool createFile(const String &path);
bool deleteFile(const String &path);
bool renameFile(const String &oldPath, const String &newPath);
bool copyFile(const String &sourcePath, const String &destPath);
bool moveFile(const String &oldPath, const String &newPath);
```

#### Directory Operations
```cpp
bool createDirectory(const String &path);
bool deleteDirectory(const String &path);
bool renameDirectory(const String &oldPath, const String &newPath);

// Directory listing
String listFiles(const String &directory = "/");
String listDirectories(const String &directory = "/");
String listAll(const String &directory = "/", bool recursive = false);

// Counting
uint32_t countFiles(const String &directory = "/", bool recursive = false);
uint32_t countDirectories(const String &directory = "/", bool recursive = false);
```

#### Content Operations
```cpp
// Write operations
bool writeFile(const String &path, const String &content);
bool writeFile(const String &path, const uint8_t *data, uint32_t length);
bool appendFile(const String &path, const String &content);
bool appendFile(const String &path, const uint8_t *data, uint32_t length);

// Read operations
String readFile(const String &path);
bool readFile(const String &path, uint8_t *buffer, uint32_t maxLength, uint32_t &bytesRead);

// Specialized formats
bool writeCSV(const String &path, const String &data, bool append = true);
bool writeJSON(const String &path, const String &jsonData, bool pretty = false);
bool writeLog(const String &path, const String &message, bool timestamp = true);
```

### Card Information & Statistics

#### Card Information
```cpp
SDCardInfo getCardInfo();
uint64_t getCardSize();
uint64_t getTotalBytes();
uint64_t getUsedBytes();  
uint64_t getFreeBytes();
SDCardType getCardType();
String getCardTypeString();
```

#### Statistics & Monitoring
```cpp
SDCardStats getStats();
void resetStats();
void printInfo();
void printStats();
void printDirectory(const String &path = "/", bool recursive = false);

// String outputs
String getSDCardString();
String getInfoString();
String getStatsString();
String getStatusString();
```

### Advanced Features

#### Working Directory
```cpp
bool setWorkingDirectory(const String &path);
String getWorkingDirectory();
String getCurrentPath(const String &relativePath);
```

#### Maintenance Operations
```cpp
bool format();
bool test();
bool benchmark();
bool cleanupOldFiles(const String &directory, uint32_t maxAgeMs, uint32_t maxFiles = 0);
```

#### Backup & Archive
```cpp
bool backupFile(const String &sourcePath, const String &backupDir = "/backup");
bool restoreFile(const String &backupPath, const String &destPath);
bool archiveDirectory(const String &sourceDir, const String &archivePath);
```

#### Utility Functions
```cpp
String generateUniqueFilename(const String &basePath, const String &extension = ".txt");
bool ensureDirectory(const String &path);
bool waitForCard(uint32_t timeoutMs = 10000);
bool waitForFile(const String &path, uint32_t timeoutMs = 5000);

// Static utilities
static String formatBytes(uint64_t bytes);
static String formatSpeed(float bytesPerSecond);
static bool isValidFilename(const String &filename);
static String sanitizeFilename(const String &filename);
```

### Error Handling & Callbacks

#### Error Management
```cpp
SDCardError getLastError();
String getLastOperation();
void clearLastError();
String getErrorString(SDCardError error);

// Error callback
void setErrorCallback(SDCardErrorCallback callback);
void setFileOperationCallback(SDCardFileCallback callback);
```

#### Error Types
```cpp
enum SDCardError {
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
```

## Usage Examples

### Basic SPI Initialization
```cpp
#include "CeriaModuleESP32SDCard.h"
using namespace CeriaDevOP;

CeriaModuleESP32SDCard sdcard;

void setup() {
    Serial.begin(115200);
    
    // Initialize dengan SPI interface
    if (!sdcard.beginSPI(18, 19, 23, 5, 4000000)) {
        Serial.println("SD Card initialization failed!");
        return;
    }
    
    // Print card information
    sdcard.printInfo();
    
    // Test basic operations
    if (sdcard.test()) {
        Serial.println("SD Card test passed!");
    }
}

void loop() {
    sdcard.update();
    
    if (sdcard.hasNewData()) {
        Serial.println("Card info updated: " + sdcard.getInfoString());
    }
    
    delay(1000);
}
```

### Advanced SDMMC Usage
```cpp
#include "CeriaModuleESP32SDCard.h"
using namespace CeriaDevOP;

CeriaModuleESP32SDCard sdcard;

void setup() {
    Serial.begin(115200);
    
    // Initialize dengan SDMMC 4-bit untuk maximum performance
    if (!sdcard.beginSDMMC4Bit()) {
        Serial.println("SDMMC initialization failed, trying SPI...");
        if (!sdcard.beginSPI()) {
            Serial.println("Both interfaces failed!");
            return;
        }
    }
    
    // Setup error callback
    sdcard.setErrorCallback([](SDCardError error, const String &operation) {
        Serial.println("SD Error: " + sdcard.getErrorString(error) + " during " + operation);
    });
    
    // Setup file operation callback
    sdcard.setFileOperationCallback([](const String &filename, uint32_t size) {
        Serial.println("File operation: " + filename + " (" + String(size) + " bytes)");
    });
    
    // Set working directory
    sdcard.setWorkingDirectory("/data");
    sdcard.ensureDirectory("/data");
    
    Serial.println("SD Card ready: " + sdcard.getStatusString());
}

void loop() {
    sdcard.update();
    delay(100);
}
```

### Data Logging Example
```cpp
#include "CeriaModuleESP32SDCard.h"
using namespace CeriaDevOP;

CeriaModuleESP32SDCard sdcard;

void setup() {
    Serial.begin(115200);
    
    if (!sdcard.begin(SDCARD_SDMMC_1BIT)) {
        Serial.println("SD Card failed!");
        return;
    }
    
    // Create logs directory
    sdcard.ensureDirectory("/logs");
    sdcard.setWorkingDirectory("/logs");
}

void loop() {
    // Log sensor data dengan timestamp
    float temperature = 25.6;
    float humidity = 60.2;
    
    String logData = String(millis()) + "," + String(temperature) + "," + String(humidity);
    
    // Write to CSV file dengan auto-append
    if (sdcard.writeCSV("sensor_data.csv", logData, true)) {
        Serial.println("Data logged: " + logData);
    }
    
    // Write to log file dengan timestamp
    String logMessage = "Temperature: " + String(temperature) + "°C, Humidity: " + String(humidity) + "%";
    sdcard.writeLog("system.log", logMessage, true);
    
    delay(10000); // Log every 10 seconds
}
```

### File Management Example
```cpp
void fileManagementDemo() {
    // Create directory structure
    sdcard.ensureDirectory("/data/sensors");
    sdcard.ensureDirectory("/data/config");
    sdcard.ensureDirectory("/backup");
    
    // Write configuration
    String config = "{\"interval\":1000,\"sensors\":[\"temp\",\"hum\"]}";
    sdcard.writeJSON("/data/config/settings.json", config, true);
    
    // Backup important files
    sdcard.backupFile("/data/config/settings.json", "/backup");
    
    // List all files
    Serial.println("Files in /data:");
    Serial.print(sdcard.listAll("/data", true));
    
    // Cleanup old log files (older than 7 days)
    uint32_t sevenDaysMs = 7 * 24 * 60 * 60 * 1000;
    sdcard.cleanupOldFiles("/logs", sevenDaysMs);
    
    // Generate unique filename untuk new data
    String uniqueFile = sdcard.generateUniqueFilename("/data/measurement", ".csv");
    sdcard.createFile(uniqueFile);
    
    // Print statistics
    sdcard.printStats();
}
```

### Performance Benchmarking
```cpp
void performanceBenchmark() {
    Serial.println("Running SD Card benchmark...");
    
    if (sdcard.benchmark()) {
        Serial.println("Benchmark completed successfully!");
    }
    
    // Get performance metrics
    SDCardStats stats = sdcard.getStats();
    Serial.println("Total operations: " + String(stats.operationsTotal));
    Serial.println("Files written: " + String(stats.filesWritten));
    Serial.println("Bytes written: " + CeriaModuleESP32SDCard::formatBytes(stats.bytesWritten));
    Serial.println("Error count: " + String(stats.errorCount));
    
    // Interface comparison
    Serial.println("Current interface: " + String(
        sdcard.getInterface() == SDCARD_SPI ? "SPI" : "SDMMC"
    ));
}
```

### Error Handling Example
```cpp
void robustFileOperations() {
    // Setup comprehensive error handling
    sdcard.setErrorCallback([](SDCardError error, const String &operation) {
        Serial.println("SD Error [" + String(error) + "]: " + 
                      sdcard.getErrorString(error) + " in " + operation);
        
        // Implement recovery strategies
        switch (error) {
            case SDCARD_ERR_MOUNT_FAILED:
                Serial.println("Attempting remount...");
                // Attempt reinitialize
                break;
            case SDCARD_ERR_INSUFFICIENT_SPACE:
                Serial.println("Cleaning up old files...");
                // Implement cleanup
                break;
            case SDCARD_ERR_FILE_NOT_FOUND:
                Serial.println("Creating missing file...");
                // Create file or directory
                break;
        }
    });
    
    // Try operation dengan error checking
    if (!sdcard.writeFile("/test.txt", "Hello World!")) {
        SDCardError lastError = sdcard.getLastError();
        Serial.println("Write failed: " + sdcard.getErrorString(lastError));
        Serial.println("Last operation: " + sdcard.getLastOperation());
        
        // Clear error state
        sdcard.clearLastError();
    }
}
```

## Best Practices

### Performance Optimization
1. **Interface Selection**: Use SDMMC untuk maximum performance, SPI untuk flexibility
2. **Buffer Size**: Use larger buffers (512B+) untuk bulk operations
3. **Minimize File Operations**: Batch writes when possible
4. **Card Selection**: Use Class 10 atau UHS-1 cards untuk optimal speed

### Reliability Guidelines  
1. **Error Handling**: Always check return values dan implement error callbacks
2. **Path Validation**: Use ensureDirectory() before creating files
3. **Connection Monitoring**: Regular update() calls untuk health checking
4. **Graceful Shutdown**: Call end() before power-off atau reset

### Memory Management
1. **File Handles**: Always close File objects when done
2. **Statistics**: Reset stats periodically untuk memory management
3. **Buffer Allocation**: Use stack buffers untuk small operations

### Data Integrity
1. **Atomic Operations**: Use temporary files untuk critical writes
2. **Backup Strategy**: Regular backups dengan versioning
3. **Validation**: Verify writes dengan read-back checking
4. **Cleanup**: Regular cleanup of temporary dan old files

## Troubleshooting

### Common Issues

#### Mount Failed
- **Symptoms**: `SDCARD_ERR_MOUNT_FAILED` during initialization
- **Solutions**: 
  - Check wiring dan connections
  - Verify card format (FAT32 required)
  - Try different SPI speed (lower frequency)
  - Check pull-up resistors on SDMMC data lines

#### File Operations Fail
- **Symptoms**: Read/write operations return false
- **Solutions**:
  - Check available space dengan `getFreeBytes()`
  - Verify path validity dengan `validatePath()`
  - Ensure parent directories exist dengan `ensureDirectory()`
  - Check for filename length limitations

#### Performance Issues
- **Symptoms**: Slow read/write speeds
- **Solutions**:
  - Use SDMMC interface instead of SPI
  - Increase SPI frequency (up to 40MHz)
  - Use Class 10 atau higher speed cards
  - Minimize small file operations

#### Card Detection Issues
- **Symptoms**: Card not detected atau intermittent connection
- **Solutions**:
  - Check power supply stability (3.3V required)
  - Verify card compatibility (SD/SDHC supported)
  - Try different card brands/types
  - Check for card corruption (format recommended)

### Debug Tips

#### Enable Verbose Logging
```cpp
sdcard.setErrorCallback([](SDCardError error, const String &operation) {
    Serial.println("DEBUG: Error " + String(error) + " in " + operation);
    Serial.println("Card info: " + sdcard.getStatusString());
    Serial.println("Stats: " + sdcard.getStatsString());
});
```

#### Connection Monitoring
```cpp
void monitorConnection() {
    if (!sdcard.isConnected()) {
        Serial.println("WARNING: SD Card connection lost!");
        // Attempt recovery
        sdcard.end();
        delay(1000);
        sdcard.begin();
    }
}
```

#### Performance Analysis
```cpp
void analyzePerformance() {
    uint32_t startTime = millis();
    
    // Perform operations
    sdcard.writeFile("/test.txt", "Performance test data");
    String data = sdcard.readFile("/test.txt");
    sdcard.deleteFile("/test.txt");
    
    uint32_t elapsed = millis() - startTime;
    Serial.println("Operations took: " + String(elapsed) + "ms");
}
```

## Integration dengan CeriaDev

CeriaModuleESP32SDCard mengikuti pola CeriaDev standard dan dapat diintegrasikan dengan komponen lain:

### Multi-Component Usage
```cpp
#include "CeriaDevOP.h"
using namespace CeriaDevOP;

CeriaSerial comm;
CeriaModuleESP32SDCard sdcard;
CeriaSensorGPS gps;

void setup() {
    // Initialize semua components
    comm.begin(Serial1, 115200);
    sdcard.begin(SDCARD_SDMMC_1BIT);
    gps.begin(Serial2, 9600);
    
    // Setup data logging
    sdcard.ensureDirectory("/logs");
    sdcard.setWorkingDirectory("/logs");
}

void loop() {
    // Update all components
    comm.tick();
    sdcard.update();
    gps.update();
    
    // Log GPS data to SD card
    if (gps.hasNewData() && gps.hasFix()) {
        String gpsData = String(millis()) + "," + 
                        String(gps.getLatitude(), 6) + "," + 
                        String(gps.getLongitude(), 6) + "," + 
                        String(gps.getAltitude(), 2);
        
        sdcard.writeCSV("gps_track.csv", gpsData, true);
        
        // Send via communication
        comm.send("gps_location", gps.getLocation());
    }
}
```

CeriaModuleESP32SDCard menyediakan solusi comprehensive untuk SD card management pada ESP32 dengan fokus pada performance, reliability, dan ease of use dalam ecosystem CeriaDev.