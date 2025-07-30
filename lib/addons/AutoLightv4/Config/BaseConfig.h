/*
 *  BaseConfig.h - AutoLightV4 Fully Dynamic Configuration
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#ifndef AUTOLIGHT_V4_BASE_CONFIG_H
#define AUTOLIGHT_V4_BASE_CONFIG_H

#include "ConfigData.h"
#include "Wire.h"

namespace AutoLight {

    class BaseConfig : public AutoLightV4Config {
    public:
        BaseConfig(bool enable_debug = false);
        ~BaseConfig();

        bool begin();
        bool end();

        scan_result_t autoScan() override;
        validation_result_t validate() override;
        bool autoFix() override;
        bool autoDistribute() override;

        ConfigBuilder& configure() override;

        bool addPCF(uint8_t address, uint8_t channels = 8) override;
        bool removePCF(uint8_t address) override;
        bool redistributeChannels() override;

        bool switchButtonMode(button_mode_v4_t mode) override;
        bool addSequence(const char* name, void (*callback)(uint8_t*, uint8_t, uint32_t)) override;
        bool removeSequence(uint8_t id) override;

        void enableHotSwap(bool enabled) override;
        void enableAutoScan(bool enabled, uint32_t interval_ms = 5000) override;
        void enableAutoFix(bool enabled) override;

        void update();
        void reset();

        uint8_t getTotalChannels() const;
        uint8_t getPCFCount() const;
        uint8_t getActiveSequences() const;
        bool isConfigured() const;
        bool isRunning() const;

        PCFDevice* findPCF(uint8_t address);
        ChannelMap* findChannel(uint8_t channel_id);

        bool testConnection();
        bool backupConfiguration();
        bool restoreConfiguration();

        void setDebugMode(bool enabled);
        bool getDebugMode() const;

        void printConfiguration() override;
        void printScanResult() override;
        void printValidationReport() override;
        void printRuntimeStats() override;

        bool importFromV3(const void* v3_config) override;
        bool exportToV3(void* v3_config) override;

        void onHardwareChange(void (*callback)(uint8_t address, bool connected));
        void onValidationError(void (*callback)(validation_result_t result, const char* message));
        void onAutoFix(void (*callback)(const char* action, bool success));

    private:
        bool debug_enabled_;
        bool is_initialized_;
        bool is_running_;
        bool config_changed_;
        
        uint32_t last_update_time_;
        uint32_t backup_time_;
        
        void (*hardware_change_callback_)(uint8_t address, bool connected);
        void (*validation_error_callback_)(validation_result_t result, const char* message);
        void (*auto_fix_callback_)(const char* action, bool success);

        bool scanI2CAddress(uint8_t address) override;
        bool testPCFResponse(uint8_t address) override;
        void updateRuntimeStats() override;
        void logMessage(const char* message) override;

        bool initializeI2C();
        bool scanI2CRange(uint8_t start_addr, uint8_t end_addr);
        bool detectPCFCapabilities(uint8_t address);
        
        bool validatePCFConfiguration();
        bool validateChannelMapping();
        bool validateSequenceConfiguration();
        
        bool autoFixPCFIssues();
        bool autoFixChannelMapping();
        bool autoFixSequences();
        
        distribution_mode_t selectOptimalDistribution(uint8_t total_channels, uint8_t pcf_count);
        bool calculateDistribution(uint8_t total_channels, uint8_t pcf_count, uint8_t* distribution);
        
        bool assignChannelsToPCF(PCFDevice* pcf, uint8_t start_channel, uint8_t channel_count);
        bool updateChannelMap();
        
        void triggerHardwareChangeCallback(uint8_t address, bool connected);
        void triggerValidationErrorCallback(validation_result_t result, const char* message);
        void triggerAutoFixCallback(const char* action, bool success);
        
        void debugLog(const char* message);
        void debugPrint(const char* format, ...);
        
        bool saveConfigToEEPROM();
        bool loadConfigFromEEPROM();
        bool validateEEPROMData();
        
        virtual void initialize();
        virtual int readChannel(); 
        virtual int readVersion();
        virtual void setVersion(uint8_t version);
        virtual void setChannel(uint8_t channel);
        virtual int getChannel();
        virtual int getVersion();
        virtual int getIoSize();
        
        void setConfigIndex(int config_index = 2);
        void setSequence(int ch_sequence_size, ...);
        void setChannelPin(int ch_pin_size, ...);
        void setAddress(int address_size, ...);
        
        void setDynamicConfig(uint8_t total_channels);
        void setDynamicConfig(uint8_t total_channels, uint8_t num_pcf);
        void setDynamicConfig(uint8_t total_channels, uint8_t num_pcf, distribution_strategy_t strategy);
        void setDynamicDistribution(uint8_t total_channels, uint8_t num_pcf, ...);
        void setDynamicWithAddresses(uint8_t total_channels, uint8_t num_pcf, uint8_t* addresses, uint8_t* channels_per_pcf);
        
        void* getConfigData();
        BaseConfig* getConfig();
        int getConfigIndex();
        
        uint8_t* getPinList();
        uint8_t getOptimalPCFCount(uint8_t channels);
        void* getCurrentDistribution();
        bool validateDistribution();
        void printDistribution();
        
        BaseConfig* operator()();
        BaseConfig* operator=(BaseConfig* other);
        
        virtual void debugConfig();
    };

    class ConfigBuilder : public AutoLight::ConfigBuilder {
    public:
        ConfigBuilder(BaseConfig* config);
        
        ConfigBuilder& channels(uint8_t count) override;
        ConfigBuilder& pcf(uint8_t count) override;
        ConfigBuilder& addresses(uint8_t addr1, ...) override;
        ConfigBuilder& distribute(uint8_t ch1, ...) override;
        ConfigBuilder& buttonMode(button_mode_v4_t mode) override;
        ConfigBuilder& sequences(uint8_t count) override;
        ConfigBuilder& autoScan(bool enabled) override;
        ConfigBuilder& autoFix(bool enabled) override;
        ConfigBuilder& hotSwap(bool enabled) override;
        
        ConfigBuilder& mode(config_mode_t mode);
        ConfigBuilder& distributionMode(distribution_mode_t mode);
        ConfigBuilder& scanInterval(uint32_t interval_ms);
        ConfigBuilder& validationInterval(uint32_t interval_ms);
        ConfigBuilder& enableValidation(bool enabled);
        ConfigBuilder& debugMode(bool enabled);
        
        bool apply() override;
        bool validate() override;
        void reset() override;
        
        ConfigBuilder& name(const char* config_name);
        ConfigBuilder& description(const char* config_desc);
        
        bool isDirty() const;
        bool canApply() const;
        
    private:
        BaseConfig* base_config_;
        bool is_dirty_;
        char config_name_[32];
        char config_description_[64];
        
        bool validateChannelCount(uint8_t count);
        bool validatePCFCount(uint8_t count);
        bool validateAddresses(uint8_t* addresses, uint8_t count);
        bool validateDistribution(uint8_t* distribution, uint8_t pcf_count);
    };
}

#endif // AUTOLIGHT_V4_BASE_CONFIG_H