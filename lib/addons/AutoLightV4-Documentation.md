# AutoLight V4 - Fully Dynamic LED Matrix Control System Documentation

## Overview

**⚠️ DEVELOPMENT STATUS: IN PROGRESS ⚠️**

AutoLight V4 merupakan **evolusi ultimate** dari ekosistem AutoLight yang sedang dalam tahap pengembangan aktif (Juli 2024). V4 dirancang sebagai sistem yang **fully dynamic**, **super user-friendly**, dan **future-proof** dengan kemampuan **zero-configuration** dan **intelligent automation**. V4 mempertahankan **100% compatibility** dengan V3 sequence modes sambil memperkenalkan revolutionary dynamic configuration capabilities.

## Complete Directory Structure

```
AutoLightv4/ (Fully Dynamic LED Matrix Control System - IN DEVELOPMENT)
├── AutoLightv4.h                   # Main library entry point with unified API
├── AutoLightV4.cpp                 # Primary implementation with dynamic features  
├── index.h                         # Component orchestration and include management
├── Channel/                        # Advanced Dynamic Channel Management
│   ├── BaseChannel.h/.cpp          # Advanced channel management with smart automation
│   └── BaseChannelSequence.cpp     # **100% V3 Compatible** - All 15 sequence modes identical
├── Config/                         # Revolutionary Dynamic Configuration System
│   ├── ConfigData.h                # Revolutionary dynamic configuration data structures
│   └── BaseConfig.h/.cpp           # Intelligent configuration engine with auto-detection
├── Cores/                          # Core System Services (Minimal in current dev)
├── Log/                           # Logging System (Minimal in current dev)
├── EXAMPLE_USAGE.ino               # Comprehensive usage examples and demonstrations
└── CLAUDE.md                       # V4 comprehensive documentation
```

## Revolutionary Features (Target Implementation)

### 1. **Full Dynamic Auto-Detection**
- **Automatic I2C Scanning**: Detects all PCF8574 devices automatically
- **Intelligent Distribution**: Optimal channel distribution algorithms  
- **Hot-Swap Support**: Runtime PCF addition/removal
- **Zero-Configuration Setup**: Works out-of-the-box

### 2. **Super User-Friendly API**
- **Builder Pattern**: Fluent configuration API
- **Smart Auto-Fix**: Automatic error detection and correction
- **Runtime Validation**: Real-time configuration validation
- **Comprehensive Callbacks**: Event-driven architecture

### 3. **Advanced Capabilities (Target)**
- **128 Channels Maximum** (vs 64 in V3)
- **16 PCF Expanders** (vs 8 in V3) 
- **32 Sequences** (vs 15 in V3)
- **Multi-Modal Button System**: 1-4 button configurations
- **Backward Compatibility**: V1-V3 import/export support

## Revolutionary Data Structures

### Dynamic Configuration System
```cpp
namespace AutoLight {
    const int MAXNUM_CHANNEL_V4 = 128;          // Double V3 capacity
    const int MAXNUM_IO_EXPANDER_V4 = 16;       // Double V3 capacity
    const int MAXNUM_VERSION_V4 = 64;           // Double V3 capacity
    const int MAXNUM_SEQUENCES_V4 = 32;         // Double V3 capacity
    const int MAXNUM_CUSTOM_SEQUENCES_V4 = 16;  // User-defined sequences

    typedef enum {
        CONFIG_MODE_AUTO = 0,        // Fully automatic configuration
        CONFIG_MODE_MANUAL = 1,      // Manual configuration
        CONFIG_MODE_HYBRID = 2       // Auto-detect + manual override
    } config_mode_t;

    typedef enum {
        SCAN_RESULT_SUCCESS = 0,     // Hardware scan successful
        SCAN_RESULT_NO_DEVICE = 1,   // No PCF devices found
        SCAN_RESULT_PARTIAL = 2,     // Some PCFs found
        SCAN_RESULT_ERROR = 3        // Scan error occurred
    } scan_result_t;

    typedef enum {
        VALIDATION_OK = 0,           // Configuration valid
        VALIDATION_WARNING = 1,      // Minor issues detected
        VALIDATION_ERROR = 2,        // Configuration errors
        VALIDATION_CRITICAL = 3      // Critical system errors
    } validation_result_t;
}
```

### Enhanced PCF Device Management
```cpp
struct PCFDevice {
    uint8_t address;                // I2C address
    uint8_t allocated_channels;     // Channels assigned to this PCF
    uint8_t start_channel;          // Starting channel number
    uint8_t end_channel;            // Ending channel number
    bool is_responding;             // Real-time status
    bool is_active;                 // Device activation status
    uint32_t last_response_time;    // Health monitoring timestamp
    uint8_t error_count;            // Error tracking counter
    char name[16];                  // Human-readable identifier
};

struct ChannelMap {
    uint8_t channel_id;             // Logical channel ID
    uint8_t pcf_address;            // Associated PCF address
    uint8_t pcf_pin;                // Pin on PCF (0-7)
    bool is_active;                 // Channel active status
    char label[16];                 // Human-readable label
};
```

### Intelligent Auto-Scan System
```cpp
struct AutoScanResult {
    PCFDevice detected_pcfs[MAXNUM_IO_EXPANDER_V4];    // Detected PCF devices
    uint8_t pcf_count;                                  // Number of PCFs found
    uint8_t total_channels;                             // Total available channels
    scan_result_t result;                               // Scan result status
    uint32_t scan_duration_ms;                          // Time taken for scan
    char error_message[64];                             // Error description
};

struct ValidationReport {
    validation_result_t result;                         // Validation result
    uint8_t error_count;                               // Number of errors
    uint8_t warning_count;                             // Number of warnings
    char messages[8][64];                              // Detailed messages
    uint8_t message_count;                             // Total message count
    bool auto_fix_available;                           // Auto-fix possible
    bool hardware_change_detected;                     // Hardware changed
};
```

### Enhanced Dynamic Configuration
```cpp
struct DynamicConfig {
    config_mode_t mode;                                 // Configuration mode
    distribution_mode_t distribution_mode;              // Distribution strategy
    button_mode_v4_t button_mode;                       // Button interaction mode
    
    PCFDevice pcf_devices[MAXNUM_IO_EXPANDER_V4];      // PCF device management
    ChannelMap channel_map[MAXNUM_CHANNEL_V4];          // Channel mapping
    
    uint8_t pcf_count;                                  // Active PCF count
    uint8_t total_channels;                             // Total managed channels
    uint8_t active_sequences;                           // Available sequences
    
    bool auto_scan_enabled;                             // Runtime hardware detection
    bool auto_fix_enabled;                              // Automatic error correction
    bool hot_swap_enabled;                              // Hot-swap PCF support
    bool validation_enabled;                            // Real-time validation
    
    uint32_t scan_interval_ms;                          // Auto-scan interval
    uint32_t validation_interval_ms;                    // Validation interval
    uint32_t last_scan_time;                           // Last scan timestamp
    uint32_t last_validation_time;                     // Last validation timestamp
    
    AutoScanResult last_scan;                          // Last scan results
    ValidationReport last_validation;                   // Last validation report
};
```

### Enhanced Button System (V4)
```cpp
typedef enum {
    BUTTON_MODE_AUTO_DETECT = 0,    // Automatic detection
    BUTTON_MODE_SINGLE = 1,         // OFF→ON→Mode2-15→OFF cycling
    BUTTON_MODE_DUAL = 2,           // Toggle + Next
    BUTTON_MODE_TRIPLE = 3,         // ON/OFF/Next separate
    BUTTON_MODE_QUAD = 4,           // Full control (V3 compatible)
    BUTTON_MODE_CUSTOM = 99         // User-defined handlers
} button_mode_v4_t;
```

## V3 Sequence Compatibility

**🔴 CRITICAL COMPATIBILITY NOTE 🔴**

AutoLightV4 maintains **perfect 100% compatibility** with AutoLightV3 sequence modes:

- **Sequence 0**: OFF mode
- **Sequence 1**: Static ON mode  
- **Sequences 2-15**: **Identical** LED patterns from V3
  - Same timing, same effects, same visual output
  - `taskSequence2()` through `taskSequence15()` 
  - Backward compatible with all V3 configurations

```cpp
// V4 BaseChannelSequence.cpp maintains 100% V3 compatibility
void BaseChannel::taskSequence2() {
    // IDENTICAL implementation to V3
    // Same pattern, same timing, same behavior
}
// ... through taskSequence15()
```

## Unified API (Target Implementation)

### AutoLightV4 Main Class
```cpp
namespace AutoLight {
    class AutoLightV4 {
    public:
        AutoLightV4(bool enable_debug = false);
        ~AutoLightV4();
        
        // Core System Management
        bool begin();
        bool end();
        void update();
        void reset();
        
        // Intelligent Auto-Detection
        scan_result_t autoScan();
        validation_result_t validate();
        bool autoFix();
        bool autoDistribute();
        
        // Builder Pattern Configuration
        ConfigBuilder& configure();
        
        // Dynamic PCF Management
        bool addPCF(uint8_t address, uint8_t channels = 8);
        bool removePCF(uint8_t address);
        bool redistributeChannels();
        
        // Sequence Management (V3 Compatible)
        void setSequenceIndex(uint8_t index);
        uint8_t getSequenceIndex() const;
        void nextSequence();
        void previousSequence();
        
        // Enhanced Button System
        void setButtonMode(button_mode_v4_t mode);
        button_mode_v4_t getButtonMode() const;
        void smartButtonPress(uint8_t button_index);
        void singleButtonCycle();
        void toggleOnOff();
        
        // Channel Control
        bool set(uint8_t channel, bool state);
        bool toggle(uint8_t channel);
        bool get(uint8_t channel) const;
        bool setAll(bool state);
        void clearAll();
        
        // Custom Sequences
        bool addSequence(uint8_t id, const char* name, 
                        void (*callback)(uint8_t*, uint8_t, uint32_t),
                        uint32_t timing = 50);
        bool removeSequence(uint8_t id);
        
        // Advanced Automation
        void enableHotSwap(bool enabled);
        void enableAutoScan(bool enabled, uint32_t interval_ms = 5000);
        void enableAutoFix(bool enabled);
        void enableAutoCycle(bool enabled, uint32_t interval_ms = 5000);
        
        // System Information
        uint8_t getTotalChannels() const;
        uint8_t getPCFCount() const;
        uint8_t getActiveSequences() const;
        bool isConfigured() const;
        bool isRunning() const;
        
        // Debug and Monitoring
        void printConfiguration();
        void printScanResult();
        void printValidationReport();
        void printChannelStates();
        void printRuntimeStats();
        void setDebugMode(bool enabled);
        bool getDebugMode() const;
        
        // V3 Compatibility
        BaseConfig* getConfig();
        BaseChannel* getChannel();
        bool importFromV3(const void* v3_config);
        bool exportToV3(void* v3_config);
        
        // Event Callbacks
        void onHardwareChange(void (*callback)(uint8_t address, bool connected));
        void onValidationError(void (*callback)(validation_result_t result, const char* message));
        void onAutoFix(void (*callback)(const char* action, bool success));
        void onSequenceChange(void (*callback)(uint8_t old_seq, uint8_t new_seq));
        void onChannelChange(void (*callback)(uint8_t channel, bool state));
        void onModeChange(void (*callback)(button_mode_v4_t old_mode, button_mode_v4_t new_mode));
        
        // Testing and Validation
        bool testConnection();
        bool testAllChannels();
        RuntimeStats getRuntimeStats();
        AutoScanResult getLastScan();
        ValidationReport getLastValidation();
        
    private:
        BaseConfig* config_;
        BaseChannel* channel_;
        bool debug_enabled_;
        bool is_initialized_;
        
        void initializeCallbacks();
    };
}
```

## Usage Patterns (Target Implementation)

### 1. **Fully Automatic Mode** (Zero Configuration)
```cpp
#include "addons/AutoLightv4.h"
using namespace AutoLight;

AutoLightV4 led(true);  // Enable debug

void setup() {
    Serial.begin(115200);
    
    // Automatic everything!
    led.autoScan();       // Auto-detect hardware
    led.autoDistribute(); // Auto-distribute channels optimally
    led.begin();          // Ready to use!
    
    // Optional: Print discovered configuration
    led.printConfiguration();
}

void loop() {
    led.update();         // Handle all automation
}
```

### 2. **Builder Pattern Configuration**
```cpp
AutoLightV4 led;

void setup() {
    Serial.begin(115200);
    
    // Fluent configuration API
    led.configure()
        .channels(12)                      // Total channels
        .pcf(2)                           // Number of PCF expanders
        .addresses(0x38, 0x21)            // PCF I2C addresses
        .distribute(6, 6)                 // Channel distribution  
        .buttonMode(BUTTON_MODE_SINGLE)   // Single button mode
        .sequences(15)                    // V3 compatible sequences
        .autoScan(true)                   // Enable auto-scanning
        .autoFix(true)                    // Enable auto-fixing
        .hotSwap(false)                   // Disable hot-swap
        .debugMode(true)                  // Enable debug output
        .apply();                         // Apply configuration
    
    led.begin();
}
```

### 3. **Runtime Dynamic Changes**
```cpp
void setup() {
    led.begin();
    
    // Runtime hardware changes
    led.addPCF(0x22, 8);                 // Add PCF with 8 channels
    led.redistributeChannels();          // Auto-redistribute all channels
    
    // Runtime mode changes  
    led.setButtonMode(BUTTON_MODE_DUAL); // Switch to dual button mode
    
    // Add custom sequence
    led.addSequence(16, "Custom", customSequenceCallback, 100);
}
```

### 4. **Advanced Features with Callbacks**
```cpp
void setup() {
    led.begin();
    
    // Enable advanced automation
    led.enableHotSwap(true);             // Allow runtime PCF changes
    led.enableAutoScan(true, 3000);      // Auto-scan every 3 seconds
    led.enableAutoFix(true);             // Auto-fix configuration issues
    led.enableAutoCycle(true, 8000);     // Auto-cycle sequences every 8s
    
    // Event callbacks
    led.onHardwareChange([](uint8_t address, bool connected) {
        Serial.printf("PCF 0x%02X %s\n", address, connected ? "connected" : "disconnected");
    });
    
    led.onSequenceChange([](uint8_t old_seq, uint8_t new_seq) {
        Serial.printf("Sequence: %d → %d\n", old_seq, new_seq);
    });
    
    led.onValidationError([](validation_result_t result, const char* message) {
        Serial.printf("Validation %s: %s\n", 
                      result == VALIDATION_ERROR ? "ERROR" : "WARNING", message);
    });
}
```

## Button System Usage (Enhanced V4)

### Single Button Mode (Most Popular)
```cpp
led.setButtonMode(BUTTON_MODE_SINGLE);

// Button press cycles: OFF → ON → Mode2 → Mode3 → ... → Mode15 → OFF
void buttonPressed() {
    led.smartButtonPress(0);  // Auto-handles based on current mode
    // or directly:
    led.singleButtonCycle();
}
```

### Multiple Button Modes
```cpp
// Dual button: Toggle + Next
led.setButtonMode(BUTTON_MODE_DUAL);
led.smartButtonPress(0);  // Toggle ON/OFF
led.smartButtonPress(1);  // Next sequence

// Triple button: ON/OFF/Next separate  
led.setButtonMode(BUTTON_MODE_TRIPLE);

// Quad button: Full V3 compatibility
led.setButtonMode(BUTTON_MODE_QUAD);
```

## Development Status & Implementation Notes

### ✅ **Currently Implemented (Based on Code Analysis)**
1. **Basic Class Structure**: AutoLightV4 constructor and destructor
2. **Core System Management**: begin(), end(), update() methods
3. **Enhanced Data Structures**: ConfigData.h with advanced structures
4. **V3 Compatibility Framework**: Channel system maintains V3 sequence compatibility
5. **Debug System**: Debug mode support and logging

### 🔄 **In Development**
1. **Auto-Scan Implementation**: I2C scanning algorithms
2. **Builder Pattern**: Fluent configuration API
3. **Hot-Swap System**: Runtime PCF addition/removal
4. **Advanced Validation**: Real-time configuration validation
5. **Event Callbacks**: Complete event system implementation
6. **Custom Sequences**: User-defined sequence support

### 📋 **Planned Features**
1. **Zero-Configuration Setup**: Complete auto-detection
2. **Advanced Auto-Fix**: Intelligent error correction
3. **Runtime Statistics**: Comprehensive system monitoring
4. **V1-V3 Import/Export**: Configuration migration tools
5. **Advanced Testing**: Hardware validation suite

### 🧪 **Example Usage (Current Development)**
Based on the `EXAMPLE_USAGE.ino` file, current development focuses on:

```cpp
void automaticExample() {
    Serial.println("Auto-scanning hardware...");
    
    auto scan_result = led.autoScan();
    if (scan_result == SCAN_RESULT_SUCCESS) {
        Serial.println("✓ Hardware detected successfully");
        
        led.autoDistribute();
        Serial.println("✓ Channels distributed automatically");
        
        led.begin();
        Serial.println("✓ System ready!");
        
        led.printConfiguration();
    } else {
        Serial.println("✗ No hardware detected");
    }
}
```

## Memory Requirements (Estimated)

### Target Specifications
- **Base System**: ~70KB Flash, ~6KB RAM
- **With Full Auto-Detection**: +~15KB Flash, +~3KB RAM
- **With Advanced Features**: +~10KB Flash, +~2KB RAM
- **Full V4 Configuration**: ~100KB Flash, ~12KB RAM total

### Hardware Capabilities (Target)
- **Maximum Channels**: 128 (16 PCF × 8 pins)
- **Maximum PCF Expanders**: 16 with intelligent distribution
- **Button Modes**: 6 different interaction paradigms
- **Sequences**: 32 total modes (0-31) with V3 compatibility (0-15)
- **Custom Sequences**: 16 user-defined patterns

## Migration Path from V3

### Perfect V3 Compatibility
```cpp
// V3 code works unchanged in V4
BaseConfig config;
BaseChannel channel;

// V3 setup continues to work
config.setDistributionStrategy(DISTRIBUTE_OPTIMIZED);
channel.setButtonMode(BUTTON_MODE_4BUTTON);

// Plus V4 enhancements available
AutoLightV4 enhanced_system(true);
enhanced_system.importFromV3(&config);  // Import V3 configuration
enhanced_system.autoScan();             // Add V4 auto-detection
```

### Upgrade Benefits
1. **Zero Code Changes**: Existing V3 code works unchanged
2. **Enhanced Features**: Add V4 capabilities as needed
3. **Backward Export**: Export back to V3 format if needed
4. **Gradual Migration**: Adopt V4 features incrementally

## Future Roadmap (Development Vision)

### V4.1 (Target: Q4 2024)
- Complete auto-detection implementation
- Full builder pattern API
- Basic hot-swap support

### V4.2 (Target: Q1 2025)
- Advanced validation and auto-fix
- Complete event callback system
- Runtime statistics

### V4.3 (Target: Q2 2025)
- Custom sequence framework
- Advanced testing suite
- Performance optimizations

## Kesimpulan

AutoLight V4 represents the **future vision** of intelligent LED control systems. Meskipun masih dalam development, V4 sudah menunjukkan potensi revolutionary yang akan mengubah cara embedded LED systems dikonfigurasi dan dikelola.

### Revolutionary Vision:
- **Zero-Configuration**: True plug-and-play LED control
- **Intelligent Automation**: Self-configuring, self-healing systems
- **Perfect Compatibility**: 100% V3 compatibility dengan V4 enhancements
- **Future-Proof Architecture**: Designed for next-generation requirements
- **Developer-Friendly**: Intuitive APIs untuk semua skill levels

### Development Status:
- **Foundation**: ✅ Core architecture implemented  
- **V3 Compatibility**: ✅ Sequence compatibility maintained
- **Auto-Detection**: 🔄 In active development
- **Advanced Features**: 📋 Planned implementation

### Ideal For (When Complete):
- **Next-Generation Projects**: Cutting-edge LED installations
- **Zero-Config Applications**: Plug-and-play requirements
- **Production Systems**: Mission-critical installations
- **Future Development**: Projects requiring advanced automation
- **V3 Migration**: Seamless upgrade path from V3 systems

**V4 akan menjadi ultimate evolution** dari AutoLight ecosystem, combining proven V3 reliability dengan next-generation intelligent automation capabilities.