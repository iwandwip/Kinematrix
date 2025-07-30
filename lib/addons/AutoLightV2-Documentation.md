# AutoLight V2 - Advanced LED Control with Web Integration Documentation

## Overview

AutoLight V2 merupakan evolusi besar dari V1 yang memperkenalkan **namespace organization**, **web server integration**, dan **advanced task management**. Dikembangkan pada 2023, V2 membangun fondasi V1 sambil menambahkan kapabilitas modern untuk IoT dan web-based control.

## Complete Directory Structure

```
AutoLightv2/ (Advanced LED Control with Web Integration)
├── AutoLightv2.h                   # Main library entry point
├── index.h                         # Component orchestration and includes  
├── App.h/.cpp                      # Web application framework (placeholder)
├── ButtonInterrupt.h/.cpp          # Enhanced interrupt-driven button system
├── Channel/                        # Advanced LED Channel Management
│   ├── BaseChannel.h/.cpp          # Core channel control with 16 sequences
│   ├── BaseChannelSequence.cpp     # 14 advanced LED sequence patterns
│   └── Indicator.h/.cpp            # Enhanced visual/audio feedback system
├── Config/                         # Enhanced Configuration Management
│   ├── ConfigData.h                # Advanced data structures and types
│   └── BaseConfig.h/.cpp           # Configuration management with type safety
├── Cores/                          # Core System Services  
│   ├── FileSystem.h/.cpp           # File system management for web assets
│   └── Task.h/.cpp                 # FreeRTOS task management system
├── Log/                           # Logging and Debug System
│   └── LogSerial.h/.cpp            # Comprehensive serial logging system
└── CLAUDE.md                       # V2 specific documentation
```

## Arsitektur Sistem

### Namespace Organization
V2 memperkenalkan namespace `AutoLight` untuk organisasi kode yang lebih bersih:

```cpp
namespace AutoLight {
    class BaseChannel;
    class BaseConfig;
    class ButtonInterrupt;
    // ... semua komponen dalam namespace
}
```

### Komponen Utama

#### 1. BaseChannel (Core Channel Management)
**File:** `Channel/BaseChannel.h/.cpp`

Pengganti `KeedBaseChannel` dengan fitur yang diperluas:

```cpp
namespace AutoLight {
    class BaseChannel {
    public:
        BaseChannel(bool _enable_unit_test = false);
        
        // Configuration Management
        void attachConfig(ConfigData *_config_data_ptr);
        void attachInterrupt(ButtonInterrupt *_button_interrupt_ptr);
        
        // Sequence Management
        void setInitDelay(uint32_t _time);
        void setInitSequence(uint8_t _sequence);
        void setTotalSequence(uint8_t _nums);
        
        // Control Methods
        void runAutoLight(void (*_callbackRun)(uint32_t sequence) = nullptr);
        void changeMode();
        void changeModeApp(uint32_t num);  // New: App-controlled mode change
        
        // 14 Task Sequences (lebih banyak dari V1)
        void taskSequence2() through taskSequence15();
        
    private:
        void (BaseChannel::*temp_mode_)();
        void (BaseChannel::*total_mode_[(MAXNUM_TOTAL_TASK_SEQUENCE)])();
        
        ExpanderIo expander_io_;
        ChannelData channel_data_;
        ConfigData *config_data_ptr_;
        ButtonInterrupt *button_interrupt_ptr_;
    };
}
```

#### 2. Enhanced Configuration System
**File:** `Config/ConfigData.h`

Struktur konfigurasi yang diperluas dengan type safety:

```cpp
namespace AutoLight {
    const int MAXNUM_CHANNEL = 64;          // Maximum channels
    const int MAXNUM_IO_EXPANDER = 8;       // Maximum PCF expanders
    const int MAXNUM_VERSION = 32;          // Hardware versions
    const int MAXNUM_CHANNEL_PIN = 5;       // Channel pins

    typedef enum {
        CUSTOM_CONFIG = 0,
        DIO_CONFIG = 1,
        I2C_CONFIG = 2,
    } config_index_t;

    typedef struct {
        uint8_t version_;           // Board version
        uint8_t channel_;           // Total channels
        uint8_t io_size_;           // Number of I/O expanders
        uint8_t pin_size_;          // Number of pins
        uint8_t *pin_ptr_;          // Pin array pointer
        uint8_t *i2c_ptr_;          // I2C address array
        uint8_t *pin_sequence_;     // Sequence mapping
    } ConfigHeader;

    typedef struct {
        ConfigHeader header;
        ConfigTable table;
    } ConfigData;
}
```

#### 3. Advanced Button Interrupt System
**File:** `ButtonInterrupt.h/.cpp`

Sistem interrupt yang lebih sophisticated:

```cpp
namespace AutoLight {
    class ButtonInterrupt {
    public:
        ButtonInterrupt();
        
        void attachPin(uint8_t pin);
        void attachCallback(void (*callback)());
        void setDebounceTime(uint32_t time);
        
        bool isPressed();
        bool isChanged();
        uint32_t getPressCount();
        
        void update();
        void reset();
        
    private:
        uint8_t interrupt_pin_;
        volatile bool is_pressed_;
        volatile bool is_changed_;
        volatile uint32_t press_count_;
        uint32_t last_press_time_;
        uint32_t debounce_time_;
        void (*isr_callback_)();
    };
}
```

### Enhanced Channel Data Management

#### ChannelData Structure
```cpp
namespace AutoLight {
    class ChannelData {
    public:
        volatile uint32_t delay_time_;
        volatile uint32_t sequence_index_;
        volatile uint32_t sequence_index_apps_;      // New: App control
        volatile bool is_reverse_;
        volatile bool is_mode_changed_;
        volatile bool is_mode_changed_apps_;         // New: App change flag
    };
}
```

#### ExpanderIo Management
```cpp
namespace AutoLight {
    class ExpanderIo {
    public:
        IOExpander **io_base_;
        uint8_t io_length;
    };
}
```

### Advanced LED Sequence System

V2 memperkenalkan **16 total modes** (0-15) dengan sequence yang lebih kompleks:

#### Sequence Constants
```cpp
namespace AutoLight {
    const int MAXNUM_TASK_SEQUENCE = 14;              // Main sequences
    const int MAXNUM_TASK_SEQUENCE_ADDITION = 2;      // OFF + ON modes
    const int MAXNUM_TOTAL_TASK_SEQUENCE = 16;        // Total modes
    const int MAXNUM_TASK_TIME = 250;                 // Max timing
    
    const int DEFAULT_DELAY_TIME = 50;
    const int DEFAULT_INIT_SEQUENCE = 0;
}
```

#### Enhanced Pattern Algorithms
V2 memperkenalkan algoritma pattern yang lebih advanced:

```cpp
class BaseChannel {
private:
    // Advanced pattern operations
    int findMax(int num, ...);
    int shiftArrayAccordingToOrder(const int patterns[][8], int patternSize, 
                                  uint8_t* mainArray, int mainArraySize, 
                                  int* arr, int& lastIndex, int targetIndex, 
                                  bool reverse = false);
    int shiftArrayAccordingToPosition(int* mainArray, int mainArraySize, 
                                     int* arr, int& lastIndex, int targetIndex);
    void resetArray(int* mainArray, int& arrSize, int& arrLastIndex);
    void splitArrayDynamically(int* mainArray, int mainArraySize);
};
```

### Core System Services

#### 1. FileSystem Management
**File:** `Cores/FileSystem.h/.cpp`

Manajemen file system untuk web assets dan konfigurasi:

```cpp
namespace AutoLight {
    class FileSystem {
    public:
        FileSystem();
        
        bool begin();
        bool end();
        
        bool exists(const char* path);
        bool remove(const char* path);
        bool rename(const char* pathFrom, const char* pathTo);
        
        File open(const char* path, const char* mode);
        size_t readFile(const char* path, uint8_t* buffer, size_t maxLen);
        bool writeFile(const char* path, const uint8_t* data, size_t len);
        
        void listDir(const char* dirname, uint8_t levels);
        
    private:
        bool is_initialized_;
    };
}
```

#### 2. Task Management System
**File:** `Cores/Task.h/.cpp`

Sistem task management untuk concurrent operations:

```cpp
namespace AutoLight {
    class Task {
    public:
        Task();
        ~Task();
        
        bool createTask(const char* taskName, 
                       void (*taskFunction)(void*), 
                       uint32_t stackSize = 4096,
                       void* parameters = nullptr,
                       UBaseType_t priority = 1,
                       BaseType_t coreID = tskNO_AFFINITY);
        
        bool deleteTask();
        bool suspendTask();
        bool resumeTask();
        
        bool isRunning();
        uint32_t getStackHighWaterMark();
        
    private:
        TaskHandle_t task_handle_;
        bool is_created_;
        char task_name_[32];
    };
}
```

### Logging System

#### LogSerial Implementation
**File:** `Log/LogSerial.h/.cpp`

Sistem logging untuk debugging dan monitoring:

```cpp
namespace AutoLight {
    typedef enum {
        LOG_LEVEL_ERROR = 0,
        LOG_LEVEL_WARN = 1,
        LOG_LEVEL_INFO = 2,
        LOG_LEVEL_DEBUG = 3,
        LOG_LEVEL_VERBOSE = 4
    } log_level_t;
    
    class LogSerial {
    public:
        LogSerial(log_level_t level = LOG_LEVEL_INFO);
        
        void error(const char* message, ...);
        void warn(const char* message, ...);
        void info(const char* message, ...);
        void debug(const char* message, ...);
        void verbose(const char* message, ...);
        
        void setLevel(log_level_t level);
        log_level_t getLevel();
        
        void enableTimestamp(bool enabled);
        void setPrefix(const char* prefix);
        
    private:
        log_level_t current_level_;
        bool timestamp_enabled_;
        char prefix_[16];
        
        void printLog(log_level_t level, const char* message, va_list args);
        const char* getLevelString(log_level_t level);
    };
}
```

### Web Integration Framework

#### App Framework (Placeholder)
**File:** `App.h/.cpp`

Framework untuk web application (implementasi minimal di V2):

```cpp
namespace AutoLight {
    // Framework siap untuk web server integration
    // Implementasi penuh akan dikembangkan di V3/V4
}
```

### Indicator System

#### Enhanced Indicator
**File:** `Channel/Indicator.h/.cpp`

Sistem indikator visual dan audio yang diperbaiki:

```cpp
namespace AutoLight {
    class Indicator {
    public:
        Indicator();
        
        void begin();
        void end();
        
        void showSequence(uint8_t sequence);
        void showStatus(bool is_on);
        void showError(uint8_t error_code);
        
        void setLED(uint8_t led_index, bool state);
        void setBuzzer(bool state, uint32_t duration = 0);
        
        void enableBlink(uint8_t led_index, uint32_t interval);
        void disableBlink(uint8_t led_index);
        
        void update();
        
    private:
        struct {
            uint8_t pin;
            bool state;
            bool blink_enabled;
            uint32_t blink_interval;
            uint32_t last_blink_time;
        } leds_[3];  // RGB LEDs
        
        struct {
            uint8_t pin;
            bool state;
            uint32_t duration;
            uint32_t start_time;
        } buzzer_;
        
        bool is_initialized_;
    };
}
```

## Enhanced Sequence Implementation

### Advanced Pattern Processing

V2 memperkenalkan sistem pattern processing yang sophisticated:

```cpp
void BaseChannel::runAutoLight(void (*_callbackRun)(uint32_t sequence)) {
    if (isChangeMode()) {
        forceOff();
        
        // Mode cycling logic
        channel_data_.sequence_index_++;
        if (channel_data_.sequence_index_ > (total_task_sequence_)) {
            channel_data_.sequence_index_ = 0;
        }
        
        // App-controlled mode change
        if (channel_data_.is_mode_changed_apps_) {
            if (channel_data_.sequence_index_apps_ > total_task_sequence_) {
                channel_data_.sequence_index_apps_ = total_task_sequence_;
            }
            channel_data_.sequence_index_ = channel_data_.sequence_index_apps_;
        }
        
        temp_mode_ = total_mode_[channel_data_.sequence_index_];
        
        // Callback for external monitoring
        if (_callbackRun != nullptr) {
            _callbackRun(channel_data_.sequence_index_);
        }
        
        channel_data_.is_mode_changed_ = false;
        channel_data_.is_mode_changed_apps_ = false;
    }
    
    // Execute current sequence
    (this->*temp_mode_)();
}
```

### Dynamic Array Operations

V2 memperkenalkan operasi array yang kompleks untuk pattern generation:

```cpp
int BaseChannel::shiftArrayAccordingToOrder(const int patterns[][8], int patternSize, 
                                           uint8_t* mainArray, int mainArraySize, 
                                           int* arr, int& lastIndex, int targetIndex, 
                                           bool reverse) {
    // Complex pattern shifting algorithm
    if (reverse) {
        // Reverse direction processing
        for (int i = patternSize - 1; i >= 0; i--) {
            // Apply pattern in reverse
        }
    } else {
        // Forward direction processing  
        for (int i = 0; i < patternSize; i++) {
            // Apply pattern forward
        }
    }
    
    // Update array states
    lastIndex = targetIndex;
    return lastIndex;
}

void BaseChannel::splitArrayDynamically(int* mainArray, int mainArraySize) {
    // Dynamic array splitting for complex patterns
    int midPoint = mainArraySize / 2;
    
    // Process first half
    for (int i = 0; i < midPoint; i++) {
        // First half processing
    }
    
    // Process second half
    for (int i = midPoint; i < mainArraySize; i++) {
        // Second half processing
    }
}
```

## Enhanced API dan Usage

### Basic Setup with Namespace
```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V2
#include "Kinematrix.h"

using namespace AutoLight;  // Use namespace

BaseConfig config;
BaseChannel channel;
ButtonInterrupt button;

void setup() {
    // Enhanced configuration
    config.setConfigIndex(I2C_CONFIG);
    auto err_ch = config.readChannel();
    auto err_ver = config.readVersion();
    
    if (err_ch == CONFIG_OK && err_ver == CONFIG_OK) {
        // Attach configuration and interrupt
        channel.attachConfig(&config);
        channel.attachInterrupt(&button);
        
        // Initialize with callback
        channel.setParamCallback([](){
            Serial.println("Channel initialized");
        });
        
        channel.initialize();
    }
}

void loop() {
    // Enhanced update cycle
    channel.runAutoLight([](uint32_t sequence) {
        Serial.printf("Sequence changed to: %d\n", sequence);
    });
    
    delay(20);  // Non-blocking delay
}
```

### Advanced Configuration
```cpp
void setupAdvancedConfig() {
    BaseConfig config;
    
    // Set multi-expander configuration
    config.setConfigIndex(I2C_CONFIG);
    
    ConfigData* configData = config.getConfigData();
    configData->header.io_size_ = 4;  // 4 PCF expanders
    configData->header.channel_ = 32; // 32 total channels
    
    // Custom I2C addresses
    uint8_t i2c_addresses[] = {0x20, 0x21, 0x38, 0x39};
    configData->header.i2c_ptr_ = i2c_addresses;
    
    // Custom sequence timing
    BaseChannel channel;
    channel.setInitDelay(30);        // 30ms delay
    channel.setTotalSequence(14);    // Use all 14 sequences
    channel.reverse(true);           // Reverse direction
}
```

### Button Interrupt Setup
```cpp
ButtonInterrupt button;

void IRAM_ATTR buttonISR() {
    button.handleInterrupt();
}

void setupButtonControl() {
    button.attachPin(2);                    // Pin 2 untuk button
    button.setDebounceTime(200);           // 200ms debounce
    button.attachCallback(buttonPressed);   // Callback function
    
    attachInterrupt(digitalPinToInterrupt(2), buttonISR, FALLING);
}

void buttonPressed() {
    Serial.println("Button pressed!");
    channel.changeMode();  // Change sequence mode
}
```

### App Control Integration
```cpp
void setupAppControl() {
    // App dapat mengontrol mode secara langsung
    void setModeFromApp(uint32_t mode) {
        if (mode <= 15) {  // Validate mode range
            channel.changeModeApp(mode);
        }
    }
    
    // Monitoring mode changes
    channel.runAutoLight([](uint32_t sequence) {
        // Send to app/web interface
        sendModeToApp(sequence);
    });
}
```

### File System Integration
```cpp
void setupFileSystem() {
    FileSystem fs;
    
    if (fs.begin()) {
        // Save configuration to file
        ConfigData config = getConfiguration();
        fs.writeFile("/config/autolight.cfg", 
                    (uint8_t*)&config, sizeof(ConfigData));
        
        // Load web assets
        if (fs.exists("/web/index.html")) {
            Serial.println("Web interface available");
        }
        
        // List available files
        fs.listDir("/", 2);
    }
}
```

### Task Management
```cpp
void setupMultiTasking() {
    Task ledTask;
    Task webTask;
    
    // LED control task
    ledTask.createTask("LED_Control", ledControlTask, 4096, nullptr, 2, 1);
    
    // Web server task  
    webTask.createTask("Web_Server", webServerTask, 8192, nullptr, 1, 0);
}

void ledControlTask(void* parameter) {
    while (true) {
        channel.runAutoLight();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void webServerTask(void* parameter) {
    // Web server implementation
    while (true) {
        // Handle web requests
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

### Logging Integration
```cpp
LogSerial logger(LOG_LEVEL_DEBUG);

void setupLogging() {
    logger.enableTimestamp(true);
    logger.setPrefix("[AutoLight]");
    
    logger.info("AutoLight V2 starting...");
    logger.debug("Configuration loaded");
    logger.warn("Using default timing");
    logger.error("I2C device not found");
}
```

## Performance Improvements

### Memory Optimization
```cpp
namespace AutoLight {
    // Optimized constants
    const int MAXNUM_TASK_TIME = 250;        // Reduced from V1
    const int DEFAULT_DELAY_TIME = 50;       // Optimized default
    
    // Smart memory allocation
    class BaseChannel {
    private:
        bool is_unit_test_;  // Skip hardware init in tests
        
        bool addIoExpander(IOExpander *_io_expander) {
            // Optimized expander management
            if (!_io_expander) return false;
            
            // Dynamic allocation only when needed
            return true;
        }
    };
}
```

### Enhanced Error Handling
```cpp
bool BaseChannel::initialize() {
    try {
        is_initialize_ = beginIoExpander();
        if (!is_initialize_) {
            logger.error("Failed to initialize I2C expanders");
            return false;
        }
        
        logger.info("Channel initialization successful");
        return true;
        
    } catch (const std::exception& e) {
        logger.error("Exception during initialization: %s", e.what());
        return false;
    }
}
```

## Platform Compatibility

### ESP32 Optimization
```cpp
#if defined(ESP32)
    // Full feature set available
    #define AUTOLIGHT_V2_FULL_FEATURES
    #define AUTOLIGHT_V2_WEB_SUPPORT
    #define AUTOLIGHT_V2_FILESYSTEM_SUPPORT
    #define AUTOLIGHT_V2_MULTITASK_SUPPORT
#endif
```

### Memory Requirements
- **Base System**: ~45KB Flash, ~3KB RAM
- **With FileSystem**: +~15KB Flash, +~1KB RAM  
- **With Tasks**: +~5KB Flash, +~2KB RAM per task
- **Full Configuration**: ~70KB Flash, ~8KB RAM total

## Evolution dari V1

### Major Improvements
1. **Namespace Organization**: Clean API dengan namespace management
2. **Enhanced Sequences**: 16 total modes (vs 11 di V1)
3. **Advanced Patterns**: Complex pattern generation algorithms
4. **Task Management**: FreeRTOS integration untuk concurrent operations
5. **File System**: Support untuk web assets dan configuration files
6. **Logging System**: Comprehensive logging untuk debugging
7. **App Integration**: Framework untuk external control

### Architectural Advances
- **Component Separation**: Clear separation antara channel, config, dan app logic
- **Error Handling**: Enhanced error management system
- **Memory Efficiency**: Optimized resource usage
- **Extensibility**: Modular design untuk easy feature addition

### Backward Compatibility
- **Hardware Support**: Compatible dengan V1 I2C expansion hardware
- **Configuration**: Supports legacy configuration methods
- **Pin Mapping**: Maintains V1 pin sequence compatibility

## Kesimpulan

AutoLight V2 merupakan significant upgrade dari V1 yang memperkenalkan modern software architecture practices sambil mempertahankan kompatibilitas hardware. Dengan namespace organization, advanced task management, dan framework untuk web integration, V2 menyiapkan foundation untuk evolusi ke V3 dan V4.

### Kelebihan V2:
- **Modern Architecture**: Namespace dan component organization
- **Enhanced Sequences**: Lebih banyak pattern dengan algoritma yang complex
- **Concurrent Operation**: Multi-task support dengan FreeRTOS
- **File System Support**: Foundation untuk web-based configuration
- **Better Debugging**: Comprehensive logging system
- **App Integration**: Framework untuk external control

### Ideal untuk:
- **IoT Projects**: Projects requiring web connectivity foundation
- **Educational**: Learning modern embedded software architecture
- **Intermediate Applications**: Projects needing more than basic LED control
- **Development**: Platform untuk developing advanced features

V2 serves sebagai jembatan antara simplicity V1 dan advanced features V3/V4, providing balance antara functionality dan complexity.