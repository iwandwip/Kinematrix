/*
 *  ConfigData.h - AutoLightV4 Fully Dynamic Configuration
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#ifndef AUTOLIGHT_V4_CONFIG_DATA_H
#define AUTOLIGHT_V4_CONFIG_DATA_H

#include "Arduino.h"

namespace AutoLight {

    const int MAXNUM_CHANNEL_V4 = 128;
    const int MAXNUM_IO_EXPANDER_V4 = 16;
    const int MAXNUM_VERSION_V4 = 64;
    const int MAXNUM_SEQUENCES_V4 = 32;
    const int MAXNUM_CUSTOM_SEQUENCES_V4 = 16;

    typedef enum {
        CONFIG_MODE_AUTO = 0,
        CONFIG_MODE_MANUAL = 1,
        CONFIG_MODE_HYBRID = 2
    } config_mode_t;

    typedef enum {
        SCAN_RESULT_SUCCESS = 0,
        SCAN_RESULT_NO_DEVICE = 1,
        SCAN_RESULT_PARTIAL = 2,
        SCAN_RESULT_ERROR = 3
    } scan_result_t;

    typedef enum {
        VALIDATION_OK = 0,
        VALIDATION_WARNING = 1,
        VALIDATION_ERROR = 2,
        VALIDATION_CRITICAL = 3
    } validation_result_t;

    typedef enum {
        DISTRIBUTE_AUTO_OPTIMAL = 0,
        DISTRIBUTE_AUTO_BALANCED = 1,
        DISTRIBUTE_AUTO_SEQUENTIAL = 2,
        DISTRIBUTE_MANUAL = 3,
        DISTRIBUTE_HYBRID = 4
    } distribution_mode_t;

    typedef enum {
        BUTTON_MODE_AUTO_DETECT = 0,
        BUTTON_MODE_SINGLE = 1,
        BUTTON_MODE_DUAL = 2,
        BUTTON_MODE_TRIPLE = 3,
        BUTTON_MODE_QUAD = 4,
        BUTTON_MODE_CUSTOM = 99
    } button_mode_v4_t;

    struct PCFDevice {
        uint8_t address;
        uint8_t allocated_channels;
        uint8_t start_channel;
        uint8_t end_channel;
        bool is_responding;
        bool is_active;
        uint32_t last_response_time;
        uint8_t error_count;
        char name[16];
    };

    struct ChannelMap {
        uint8_t channel_id;
        uint8_t pcf_address;
        uint8_t pcf_pin;
        bool is_active;
        char label[16];
    };

    struct AutoScanResult {
        PCFDevice detected_pcfs[MAXNUM_IO_EXPANDER_V4];
        uint8_t pcf_count;
        uint8_t total_channels;
        scan_result_t result;
        uint32_t scan_duration_ms;
        char error_message[64];
    };

    struct ValidationReport {
        validation_result_t result;
        uint8_t error_count;
        uint8_t warning_count;
        char messages[8][64];
        uint8_t message_count;
        bool auto_fix_available;
        bool hardware_change_detected;
    };

    struct DynamicConfig {
        config_mode_t mode;
        distribution_mode_t distribution_mode;
        button_mode_v4_t button_mode;
        
        PCFDevice pcf_devices[MAXNUM_IO_EXPANDER_V4];
        ChannelMap channel_map[MAXNUM_CHANNEL_V4];
        
        uint8_t pcf_count;
        uint8_t total_channels;
        uint8_t active_sequences;
        
        bool auto_scan_enabled;
        bool auto_fix_enabled;
        bool hot_swap_enabled;
        bool validation_enabled;
        
        uint32_t scan_interval_ms;
        uint32_t validation_interval_ms;
        uint32_t last_scan_time;
        uint32_t last_validation_time;
        
        AutoScanResult last_scan;
        ValidationReport last_validation;
    };

    struct ConfigBuilder {
        DynamicConfig* config_ptr;
        bool is_building;
        
        ConfigBuilder& channels(uint8_t count);
        ConfigBuilder& pcf(uint8_t count);
        ConfigBuilder& addresses(uint8_t addr1, ...);
        ConfigBuilder& distribute(uint8_t ch1, ...);
        ConfigBuilder& buttonMode(button_mode_v4_t mode);
        ConfigBuilder& sequences(uint8_t count);
        ConfigBuilder& autoScan(bool enabled);
        ConfigBuilder& autoFix(bool enabled);
        ConfigBuilder& hotSwap(bool enabled);
        bool apply();
        bool validate();
        void reset();
    };

    struct SequenceDefinition {
        uint8_t id;
        char name[32];
        void (*callback)(uint8_t* channels, uint8_t count, uint32_t timing);
        uint32_t default_timing;
        bool is_custom;
        bool is_active;
    };

    struct RuntimeStats {
        uint32_t uptime_ms;
        uint32_t total_scans;
        uint32_t total_validations;
        uint32_t total_auto_fixes;
        uint32_t pcf_errors;
        uint32_t sequence_switches;
        uint8_t current_sequence;
        bool is_running;
    };

    typedef struct {
        uint8_t version_;
        uint8_t channel_;
        uint8_t io_size_;
        uint8_t pin_size_;
        uint8_t *pin_ptr_;
        uint8_t *i2c_ptr_;
        uint8_t *pin_sequence_;
    } LegacyConfigHeader;

    typedef struct {
        uint8_t address_pin_[5];
        uint8_t i2c_address_[MAXNUM_IO_EXPANDER_V4];
        uint8_t version_[MAXNUM_VERSION_V4];
        uint8_t channel_[MAXNUM_VERSION_V4];
    } LegacyConfigTable;

    typedef struct {
        LegacyConfigHeader header;
        LegacyConfigTable table;
    } LegacyConfigData;

    class AutoLightV4Config {
    public:
        DynamicConfig dynamic_config;
        ConfigBuilder builder;
        RuntimeStats stats;
        
        AutoLightV4Config();
        
        scan_result_t autoScan();
        validation_result_t validate();
        bool autoFix();
        bool autoDistribute();
        
        ConfigBuilder& configure();
        
        bool addPCF(uint8_t address, uint8_t channels);
        bool removePCF(uint8_t address);
        bool redistributeChannels();
        
        bool switchButtonMode(button_mode_v4_t mode);
        bool addSequence(const char* name, void (*callback)(uint8_t*, uint8_t, uint32_t));
        bool removeSequence(uint8_t id);
        
        void enableHotSwap(bool enabled);
        void enableAutoScan(bool enabled, uint32_t interval_ms = 5000);
        void enableAutoFix(bool enabled);
        
        PCFDevice* getPCFDevice(uint8_t address);
        ChannelMap* getChannelMap(uint8_t channel);
        AutoScanResult getLastScan();
        ValidationReport getLastValidation();
        RuntimeStats getRuntimeStats();
        
        void printConfiguration();
        void printScanResult();
        void printValidationReport();
        void printRuntimeStats();
        
        bool importFromV3(const void* v3_config);
        bool exportToV3(void* v3_config);
        
    private:
        bool scanI2CAddress(uint8_t address);
        bool testPCFResponse(uint8_t address);
        void updateRuntimeStats();
        void logMessage(const char* message);
    };

    struct Credentials {
        String ssid;
        String password;
        String serial;
    };

    class ConfigI2CMaxData {
    public:
        static const int VERSION = MAXNUM_VERSION_V4;
        static const int PIN_ADDRESS = 5;
        static const int I2C_ADDRESS = MAXNUM_IO_EXPANDER_V4;
        static const int CHANNELS = MAXNUM_CHANNEL_V4;
        static const int SEQUENCES = MAXNUM_SEQUENCES_V4;
    };
}

#endif // AUTOLIGHT_V4_CONFIG_DATA_H