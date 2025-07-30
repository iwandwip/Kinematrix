/*
 *  BaseConfig.cpp - AutoLightV4 Fully Dynamic Configuration
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#include "BaseConfig.h"
#include <stdarg.h>

namespace AutoLight {

BaseConfig::BaseConfig(bool enable_debug) 
    : debug_enabled_(enable_debug)
    , is_initialized_(false)
    , is_running_(false)
    , config_changed_(false)
    , last_update_time_(0)
    , backup_time_(0)
    , hardware_change_callback_(nullptr)
    , validation_error_callback_(nullptr)
    , auto_fix_callback_(nullptr) {
    
    memset(&dynamic_config, 0, sizeof(DynamicConfig));
    memset(&stats, 0, sizeof(RuntimeStats));
    
    dynamic_config.mode = CONFIG_MODE_AUTO;
    dynamic_config.distribution_mode = DISTRIBUTE_AUTO_OPTIMAL;
    dynamic_config.button_mode = BUTTON_MODE_AUTO_DETECT;
    dynamic_config.auto_scan_enabled = true;
    dynamic_config.auto_fix_enabled = true;
    dynamic_config.hot_swap_enabled = false;
    dynamic_config.validation_enabled = true;
    dynamic_config.scan_interval_ms = 5000;
    dynamic_config.validation_interval_ms = 10000;
    
    builder.config_ptr = &dynamic_config;
    builder.is_building = false;
}

BaseConfig::~BaseConfig() {
    end();
}

bool BaseConfig::begin() {
    if (is_initialized_) return true;
    
    debugLog("BaseConfig::begin() - Initializing AutoLightV4 configuration");
    
    if (!initializeI2C()) {
        debugLog("BaseConfig::begin() - Failed to initialize I2C");
        return false;
    }
    
    if (dynamic_config.auto_scan_enabled) {
        scan_result_t result = autoScan();
        if (result == SCAN_RESULT_ERROR) {
            debugLog("BaseConfig::begin() - Auto-scan failed");
            return false;
        }
    }
    
    if (dynamic_config.validation_enabled) {
        validation_result_t result = validate();
        if (result == VALIDATION_CRITICAL) {
            debugLog("BaseConfig::begin() - Critical validation errors");
            return false;
        }
    }
    
    is_initialized_ = true;
    is_running_ = true;
    stats.is_running = true;
    
    debugLog("BaseConfig::begin() - Initialization complete");
    return true;
}

bool BaseConfig::end() {
    if (!is_initialized_) return true;
    
    debugLog("BaseConfig::end() - Shutting down configuration");
    
    is_running_ = false;
    stats.is_running = false;
    
    if (config_changed_) {
        saveConfigToEEPROM();
    }
    
    is_initialized_ = false;
    return true;
}

scan_result_t BaseConfig::autoScan() {
    debugLog("BaseConfig::autoScan() - Starting automatic I2C scan");
    
    AutoScanResult& scan = dynamic_config.last_scan;
    memset(&scan, 0, sizeof(AutoScanResult));
    
    uint32_t start_time = millis();
    uint8_t found_count = 0;
    
    for (uint8_t addr = 0x20; addr <= 0x27; addr++) {
        if (scanI2CAddress(addr)) {
            PCFDevice& pcf = scan.detected_pcfs[found_count];
            pcf.address = addr;
            pcf.allocated_channels = 8;
            pcf.is_responding = true;
            pcf.is_active = true;
            pcf.last_response_time = millis();
            pcf.error_count = 0;
            snprintf(pcf.name, sizeof(pcf.name), "PCF_%02X", addr);
            found_count++;
        }
    }
    
    for (uint8_t addr = 0x38; addr <= 0x3F; addr++) {
        if (found_count >= MAXNUM_IO_EXPANDER_V4) break;
        
        if (scanI2CAddress(addr)) {
            PCFDevice& pcf = scan.detected_pcfs[found_count];
            pcf.address = addr;
            pcf.allocated_channels = 8;
            pcf.is_responding = true;
            pcf.is_active = true;
            pcf.last_response_time = millis();
            pcf.error_count = 0;
            snprintf(pcf.name, sizeof(pcf.name), "PCF_%02X", addr);
            found_count++;
        }
    }
    
    scan.pcf_count = found_count;
    scan.total_channels = found_count * 8;
    scan.scan_duration_ms = millis() - start_time;
    
    if (found_count == 0) {
        scan.result = SCAN_RESULT_NO_DEVICE;
        strcpy(scan.error_message, "No PCF devices detected");
    } else {
        scan.result = SCAN_RESULT_SUCCESS;
        snprintf(scan.error_message, sizeof(scan.error_message), 
                "Found %d PCF devices", found_count);
    }
    
    memcpy(dynamic_config.pcf_devices, scan.detected_pcfs, 
           sizeof(PCFDevice) * found_count);
    dynamic_config.pcf_count = found_count;
    dynamic_config.total_channels = scan.total_channels;
    dynamic_config.last_scan_time = millis();
    
    stats.total_scans++;
    updateRuntimeStats();
    
    debugPrint("BaseConfig::autoScan() - Found %d devices in %d ms", 
               found_count, scan.scan_duration_ms);
    
    return scan.result;
}

validation_result_t BaseConfig::validate() {
    debugLog("BaseConfig::validate() - Starting configuration validation");
    
    ValidationReport& report = dynamic_config.last_validation;
    memset(&report, 0, sizeof(ValidationReport));
    
    bool has_errors = false;
    bool has_warnings = false;
    
    if (!validatePCFConfiguration()) {
        snprintf(report.messages[report.message_count++], 64, 
                "PCF configuration validation failed");
        has_errors = true;
    }
    
    if (!validateChannelMapping()) {
        snprintf(report.messages[report.message_count++], 64, 
                "Channel mapping validation failed");
        has_warnings = true;
    }
    
    if (!validateSequenceConfiguration()) {
        snprintf(report.messages[report.message_count++], 64, 
                "Sequence configuration validation failed");
        has_warnings = true;
    }
    
    if (has_errors) {
        report.result = VALIDATION_ERROR;
        report.error_count++;
    } else if (has_warnings) {
        report.result = VALIDATION_WARNING;
        report.warning_count++;
    } else {
        report.result = VALIDATION_OK;
    }
    
    report.auto_fix_available = dynamic_config.auto_fix_enabled && 
                               (has_errors || has_warnings);
    
    dynamic_config.last_validation_time = millis();
    stats.total_validations++;
    updateRuntimeStats();
    
    debugPrint("BaseConfig::validate() - Validation result: %d", report.result);
    
    return report.result;
}

bool BaseConfig::autoFix() {
    if (!dynamic_config.auto_fix_enabled) return false;
    
    debugLog("BaseConfig::autoFix() - Starting automatic configuration fix");
    
    bool fixed = false;
    
    if (!validatePCFConfiguration()) {
        if (autoFixPCFIssues()) {
            triggerAutoFixCallback("PCF configuration", true);
            fixed = true;
        }
    }
    
    if (!validateChannelMapping()) {
        if (autoFixChannelMapping()) {
            triggerAutoFixCallback("Channel mapping", true);
            fixed = true;
        }
    }
    
    if (!validateSequenceConfiguration()) {
        if (autoFixSequences()) {
            triggerAutoFixCallback("Sequence configuration", true);
            fixed = true;
        }
    }
    
    if (fixed) {
        stats.total_auto_fixes++;
        updateRuntimeStats();
        config_changed_ = true;
    }
    
    debugPrint("BaseConfig::autoFix() - Auto-fix %s", fixed ? "successful" : "failed");
    
    return fixed;
}

bool BaseConfig::autoDistribute() {
    if (dynamic_config.pcf_count == 0) return false;
    
    debugLog("BaseConfig::autoDistribute() - Starting automatic channel distribution");
    
    distribution_mode_t mode = selectOptimalDistribution(
        dynamic_config.total_channels, dynamic_config.pcf_count);
    
    uint8_t distribution[MAXNUM_IO_EXPANDER_V4];
    if (!calculateDistribution(dynamic_config.total_channels, 
                              dynamic_config.pcf_count, distribution)) {
        return false;
    }
    
    uint8_t current_channel = 0;
    for (uint8_t i = 0; i < dynamic_config.pcf_count; i++) {
        PCFDevice& pcf = dynamic_config.pcf_devices[i];
        pcf.start_channel = current_channel;
        pcf.allocated_channels = distribution[i];
        pcf.end_channel = current_channel + distribution[i] - 1;
        
        assignChannelsToPCF(&pcf, current_channel, distribution[i]);
        current_channel += distribution[i];
    }
    
    updateChannelMap();
    config_changed_ = true;
    
    debugLog("BaseConfig::autoDistribute() - Automatic distribution complete");
    return true;
}

ConfigBuilder& BaseConfig::configure() {
    builder.config_ptr = &dynamic_config;
    builder.is_building = true;
    return builder;
}

bool BaseConfig::addPCF(uint8_t address, uint8_t channels) {
    if (dynamic_config.pcf_count >= MAXNUM_IO_EXPANDER_V4) return false;
    
    debugPrint("BaseConfig::addPCF() - Adding PCF at address 0x%02X", address);
    
    if (!testPCFResponse(address)) {
        debugLog("BaseConfig::addPCF() - PCF not responding");
        return false;
    }
    
    PCFDevice& pcf = dynamic_config.pcf_devices[dynamic_config.pcf_count];
    pcf.address = address;
    pcf.allocated_channels = channels;
    pcf.is_responding = true;
    pcf.is_active = true;
    pcf.last_response_time = millis();
    pcf.error_count = 0;
    snprintf(pcf.name, sizeof(pcf.name), "PCF_%02X", address);
    
    dynamic_config.pcf_count++;
    dynamic_config.total_channels += channels;
    config_changed_ = true;
    
    if (dynamic_config.distribution_mode == DISTRIBUTE_AUTO_OPTIMAL) {
        autoDistribute();
    }
    
    triggerHardwareChangeCallback(address, true);
    
    debugLog("BaseConfig::addPCF() - PCF added successfully");
    return true;
}

bool BaseConfig::removePCF(uint8_t address) {
    debugPrint("BaseConfig::removePCF() - Removing PCF at address 0x%02X", address);
    
    for (uint8_t i = 0; i < dynamic_config.pcf_count; i++) {
        if (dynamic_config.pcf_devices[i].address == address) {
            dynamic_config.total_channels -= dynamic_config.pcf_devices[i].allocated_channels;
            
            for (uint8_t j = i; j < dynamic_config.pcf_count - 1; j++) {
                dynamic_config.pcf_devices[j] = dynamic_config.pcf_devices[j + 1];
            }
            
            dynamic_config.pcf_count--;
            config_changed_ = true;
            
            if (dynamic_config.distribution_mode == DISTRIBUTE_AUTO_OPTIMAL) {
                autoDistribute();
            }
            
            triggerHardwareChangeCallback(address, false);
            
            debugLog("BaseConfig::removePCF() - PCF removed successfully");
            return true;
        }
    }
    
    debugLog("BaseConfig::removePCF() - PCF not found");
    return false;
}

bool BaseConfig::redistributeChannels() {
    debugLog("BaseConfig::redistributeChannels() - Redistributing all channels");
    return autoDistribute();
}

void BaseConfig::update() {
    if (!is_running_) return;
    
    uint32_t current_time = millis();
    
    if (dynamic_config.auto_scan_enabled && 
        (current_time - dynamic_config.last_scan_time) > dynamic_config.scan_interval_ms) {
        autoScan();
    }
    
    if (dynamic_config.validation_enabled && 
        (current_time - dynamic_config.last_validation_time) > dynamic_config.validation_interval_ms) {
        validation_result_t result = validate();
        if (result != VALIDATION_OK && dynamic_config.auto_fix_enabled) {
            autoFix();
        }
    }
    
    updateRuntimeStats();
    last_update_time_ = current_time;
}

bool BaseConfig::scanI2CAddress(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0);
}

bool BaseConfig::testPCFResponse(uint8_t address) {
    Wire.beginTransmission(address);
    Wire.write(0xFF);
    bool success = (Wire.endTransmission() == 0);
    
    if (success) {
        Wire.requestFrom(address, (uint8_t)1);
        if (Wire.available()) {
            Wire.read();
        }
    }
    
    return success;
}

void BaseConfig::updateRuntimeStats() {
    stats.uptime_ms = millis();
}

void BaseConfig::logMessage(const char* message) {
    if (debug_enabled_) {
        Serial.print("[AutoLightV4] ");
        Serial.println(message);
    }
}

void BaseConfig::debugLog(const char* message) {
    logMessage(message);
}

void BaseConfig::debugPrint(const char* format, ...) {
    if (!debug_enabled_) return;
    
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    logMessage(buffer);
}

bool BaseConfig::initializeI2C() {
    Wire.begin();
    Wire.setClock(100000);
    return true;
}

distribution_mode_t BaseConfig::selectOptimalDistribution(uint8_t total_channels, uint8_t pcf_count) {
    if (pcf_count == 0) return DISTRIBUTE_AUTO_BALANCED;
    
    uint8_t channels_per_pcf = total_channels / pcf_count;
    uint8_t remainder = total_channels % pcf_count;
    
    if (remainder == 0 && channels_per_pcf <= 8) {
        return DISTRIBUTE_AUTO_BALANCED;
    } else if (total_channels <= (pcf_count - 1) * 8) {
        return DISTRIBUTE_AUTO_SEQUENTIAL;
    } else {
        return DISTRIBUTE_AUTO_OPTIMAL;
    }
}

bool BaseConfig::calculateDistribution(uint8_t total_channels, uint8_t pcf_count, uint8_t* distribution) {
    if (pcf_count == 0) return false;
    
    uint8_t base_channels = total_channels / pcf_count;
    uint8_t remainder = total_channels % pcf_count;
    
    for (uint8_t i = 0; i < pcf_count; i++) {
        distribution[i] = base_channels;
        if (i < remainder) {
            distribution[i]++;
        }
        
        if (distribution[i] > 8) {
            distribution[i] = 8;
        }
    }
    
    return true;
}

void BaseConfig::printConfiguration() {
    Serial.println("=== AutoLightV4 Configuration ===");
    Serial.printf("Mode: %d, PCFs: %d, Channels: %d\n", 
                  dynamic_config.mode, dynamic_config.pcf_count, dynamic_config.total_channels);
    
    for (uint8_t i = 0; i < dynamic_config.pcf_count; i++) {
        PCFDevice& pcf = dynamic_config.pcf_devices[i];
        Serial.printf("PCF[%d]: Addr=0x%02X, Channels=%d-%d (%d total)\n", 
                      i, pcf.address, pcf.start_channel, pcf.end_channel, pcf.allocated_channels);
    }
}

void BaseConfig::initialize() {
    begin();
}

int BaseConfig::readChannel() {
    autoScan();
    return dynamic_config.total_channels;
}

int BaseConfig::readVersion() {
    return 4;
}

void BaseConfig::setVersion(uint8_t version) {
}

void BaseConfig::setChannel(uint8_t channel) {
    dynamic_config.total_channels = channel;
}

int BaseConfig::getChannel() {
    return dynamic_config.total_channels;
}

int BaseConfig::getVersion() {
    return 4;
}

int BaseConfig::getIoSize() {
    return dynamic_config.pcf_count;
}

void BaseConfig::setConfigIndex(int config_index) {
    if (config_index == 2) {
        dynamic_config.mode = CONFIG_MODE_AUTO;
        enableAutoScan(true);
    }
}

void BaseConfig::setSequence(int ch_sequence_size, ...) {
    va_list args;
    va_start(args, ch_sequence_size);
    
    for (int i = 0; i < ch_sequence_size && i < MAXNUM_CHANNEL_V4; i++) {
        int sequence = va_arg(args, int);
    }
    
    va_end(args);
}

void BaseConfig::setChannelPin(int ch_pin_size, ...) {
    va_list args;
    va_start(args, ch_pin_size);
    
    for (int i = 0; i < ch_pin_size && i < MAXNUM_CHANNEL_V4; i++) {
        int pin = va_arg(args, int);
    }
    
    va_end(args);
}

void BaseConfig::setAddress(int address_size, ...) {
    va_list args;
    va_start(args, address_size);
    
    for (int i = 0; i < address_size && i < MAXNUM_IO_EXPANDER_V4; i++) {
        int address = va_arg(args, int);
        dynamic_config.pcf_devices[i].address = address;
        dynamic_config.pcf_devices[i].is_active = true;
    }
    dynamic_config.pcf_count = address_size;
    
    va_end(args);
}

void BaseConfig::setDynamicConfig(uint8_t total_channels) {
    dynamic_config.total_channels = total_channels;
    uint8_t pcf_count = (total_channels + 7) / 8;
    dynamic_config.pcf_count = pcf_count;
    
    for (uint8_t i = 0; i < pcf_count; i++) {
        dynamic_config.pcf_devices[i].address = 0x20 + i;
        dynamic_config.pcf_devices[i].allocated_channels = 8;
        dynamic_config.pcf_devices[i].is_active = true;
    }
    
    autoDistribute();
}

void BaseConfig::setDynamicConfig(uint8_t total_channels, uint8_t num_pcf) {
    dynamic_config.total_channels = total_channels;
    dynamic_config.pcf_count = num_pcf;
    
    for (uint8_t i = 0; i < num_pcf; i++) {
        dynamic_config.pcf_devices[i].address = 0x20 + i;
        dynamic_config.pcf_devices[i].is_active = true;
    }
    
    autoDistribute();
}

void BaseConfig::setDynamicConfig(uint8_t total_channels, uint8_t num_pcf, distribution_strategy_t strategy) {
    setDynamicConfig(total_channels, num_pcf);
    dynamic_config.strategy = strategy;
}

void BaseConfig::setDynamicDistribution(uint8_t total_channels, uint8_t num_pcf, ...) {
    dynamic_config.total_channels = total_channels;
    dynamic_config.pcf_count = num_pcf;
    
    va_list args;
    va_start(args, num_pcf);
    
    uint8_t current_channel = 0;
    for (uint8_t i = 0; i < num_pcf; i++) {
        uint8_t channels = va_arg(args, int);
        dynamic_config.pcf_devices[i].address = 0x20 + i;
        dynamic_config.pcf_devices[i].allocated_channels = channels;
        dynamic_config.pcf_devices[i].start_channel = current_channel;
        dynamic_config.pcf_devices[i].end_channel = current_channel + channels - 1;
        dynamic_config.pcf_devices[i].is_active = true;
        current_channel += channels;
    }
    
    va_end(args);
    updateChannelMap();
}

void BaseConfig::setDynamicWithAddresses(uint8_t total_channels, uint8_t num_pcf, uint8_t* addresses, uint8_t* channels_per_pcf) {
    dynamic_config.total_channels = total_channels;
    dynamic_config.pcf_count = num_pcf;
    
    uint8_t current_channel = 0;
    for (uint8_t i = 0; i < num_pcf; i++) {
        dynamic_config.pcf_devices[i].address = addresses[i];
        dynamic_config.pcf_devices[i].allocated_channels = channels_per_pcf[i];
        dynamic_config.pcf_devices[i].start_channel = current_channel;
        dynamic_config.pcf_devices[i].end_channel = current_channel + channels_per_pcf[i] - 1;
        dynamic_config.pcf_devices[i].is_active = true;
        current_channel += channels_per_pcf[i];
    }
    
    updateChannelMap();
}

void* BaseConfig::getConfigData() {
    return &dynamic_config;
}

BaseConfig* BaseConfig::getConfig() {
    return this;
}

int BaseConfig::getConfigIndex() {
    return (dynamic_config.mode == CONFIG_MODE_AUTO) ? 2 : 0;
}

uint8_t* BaseConfig::getPinList() {
    static uint8_t pin_list[MAXNUM_CHANNEL_V4];
    for (uint8_t i = 0; i < dynamic_config.total_channels; i++) {
        pin_list[i] = i;
    }
    return pin_list;
}

uint8_t BaseConfig::getOptimalPCFCount(uint8_t channels) {
    return (channels + 7) / 8;
}

void* BaseConfig::getCurrentDistribution() {
    return &dynamic_config;
}

bool BaseConfig::validateDistribution() {
    return validate() == VALIDATION_OK;
}

void BaseConfig::printDistribution() {
    printConfiguration();
}

BaseConfig* BaseConfig::operator()() {
    return this;
}

BaseConfig* BaseConfig::operator=(BaseConfig* other) {
    if (other && other != this) {
        dynamic_config = other->dynamic_config;
    }
    return this;
}

void BaseConfig::debugConfig() {
    printConfiguration();
}

}  // namespace AutoLight