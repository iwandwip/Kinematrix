/*
 *  AutoLightV4.cpp - Fully Dynamic LED Matrix Control System
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#include "../AutoLightv4.h"

namespace AutoLight {

AutoLightV4::AutoLightV4(bool enable_debug) 
    : config_(nullptr)
    , channel_(nullptr)
    , debug_enabled_(enable_debug)
    , is_initialized_(false) {
    
    config_ = new BaseConfig(enable_debug);
    channel_ = new BaseChannel(enable_debug);
    
    if (config_ && channel_) {
        channel_->setConfig(config_);
        initializeCallbacks();
    }
}

AutoLightV4::~AutoLightV4() {
    end();
    
    if (channel_) {
        delete channel_;
        channel_ = nullptr;
    }
    
    if (config_) {
        delete config_;
        config_ = nullptr;
    }
}

bool AutoLightV4::begin() {
    if (is_initialized_) return true;
    
    if (!config_ || !channel_) {
        Serial.println("[AutoLightV4] ERROR: Configuration or Channel not initialized");
        return false;
    }
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Starting fully dynamic LED control system");
    }
    
    if (!config_->begin()) {
        Serial.println("[AutoLightV4] ERROR: Configuration initialization failed");
        return false;
    }
    
    if (!channel_->begin()) {
        Serial.println("[AutoLightV4] ERROR: Channel initialization failed");
        return false;
    }
    
    is_initialized_ = true;
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] System initialized successfully");
        printConfiguration();
    }
    
    return true;
}

bool AutoLightV4::end() {
    if (!is_initialized_) return true;
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Shutting down system");
    }
    
    if (channel_) {
        channel_->end();
    }
    
    if (config_) {
        config_->end();
    }
    
    is_initialized_ = false;
    return true;
}

void AutoLightV4::update() {
    if (!is_initialized_) return;
    
    if (config_) {
        config_->update();
    }
    
    if (channel_) {
        channel_->update();
    }
}

void AutoLightV4::reset() {
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Resetting system");
    }
    
    if (channel_) {
        channel_->reset();
    }
    
    if (config_) {
        config_->reset();
    }
}

scan_result_t AutoLightV4::autoScan() {
    if (!config_) return SCAN_RESULT_ERROR;
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Starting automatic hardware scan");
    }
    
    scan_result_t result = config_->autoScan();
    
    if (debug_enabled_) {
        printScanResult();
    }
    
    return result;
}

validation_result_t AutoLightV4::validate() {
    if (!config_) return VALIDATION_CRITICAL;
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Validating configuration");
    }
    
    validation_result_t result = config_->validate();
    
    if (debug_enabled_) {
        printValidationReport();
    }
    
    return result;
}

bool AutoLightV4::autoFix() {
    if (!config_) return false;
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Attempting automatic fixes");
    }
    
    bool result = config_->autoFix();
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Auto-fix %s\n", result ? "successful" : "failed");
    }
    
    return result;
}

bool AutoLightV4::autoDistribute() {
    if (!config_) return false;
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Performing automatic channel distribution");
    }
    
    bool result = config_->autoDistribute();
    
    if (debug_enabled_ && result) {
        printConfiguration();
    }
    
    return result;
}

ConfigBuilder& AutoLightV4::configure() {
    if (!config_) {
        static ConfigBuilder dummy_builder(nullptr);
        return dummy_builder;
    }
    
    return config_->configure();
}

bool AutoLightV4::addPCF(uint8_t address, uint8_t channels) {
    if (!config_) return false;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Adding PCF at address 0x%02X with %d channels\n", 
                      address, channels);
    }
    
    return config_->addPCF(address, channels);
}

bool AutoLightV4::removePCF(uint8_t address) {
    if (!config_) return false;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Removing PCF at address 0x%02X\n", address);
    }
    
    return config_->removePCF(address);
}

bool AutoLightV4::redistributeChannels() {
    if (!config_) return false;
    
    if (debug_enabled_) {
        Serial.println("[AutoLightV4] Redistributing channels");
    }
    
    return config_->redistributeChannels();
}

void AutoLightV4::setSequenceIndex(uint8_t index) {
    if (!channel_) return;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Setting sequence to %d\n", index);
    }
    
    channel_->setSequenceIndex(index);
}

uint8_t AutoLightV4::getSequenceIndex() const {
    if (!channel_) return 0;
    return channel_->getSequenceIndex();
}

void AutoLightV4::nextSequence() {
    if (!channel_) return;
    channel_->nextSequence();
}

void AutoLightV4::previousSequence() {
    if (!channel_) return;
    channel_->previousSequence();
}

void AutoLightV4::setButtonMode(button_mode_v4_t mode) {
    if (!channel_) return;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Setting button mode to %d\n", mode);
    }
    
    channel_->setButtonMode(mode);
}

button_mode_v4_t AutoLightV4::getButtonMode() const {
    if (!channel_) return BUTTON_MODE_AUTO_DETECT;
    return channel_->getButtonMode();
}

void AutoLightV4::smartButtonPress(uint8_t button_index) {
    if (!channel_) return;
    channel_->smartButtonPress(button_index);
}

void AutoLightV4::singleButtonCycle() {
    if (!channel_) return;
    channel_->singleButtonCycle();
}

void AutoLightV4::toggleOnOff() {
    if (!channel_) return;
    channel_->toggleOnOff();
}

bool AutoLightV4::set(uint8_t channel, bool state) {
    if (!channel_) return false;
    return channel_->set(channel, state);
}

bool AutoLightV4::toggle(uint8_t channel) {
    if (!channel_) return false;
    return channel_->toggle(channel);
}

bool AutoLightV4::get(uint8_t channel) const {
    if (!channel_) return false;
    return channel_->get(channel);
}

bool AutoLightV4::setAll(bool state) {
    if (!channel_) return false;
    return channel_->setAll(state);
}

void AutoLightV4::clearAll() {
    if (!channel_) return;
    channel_->clearAll();
}

bool AutoLightV4::addSequence(uint8_t id, const char* name, 
                             void (*callback)(uint8_t*, uint8_t, uint32_t),
                             uint32_t timing) {
    if (!channel_) return false;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Adding custom sequence %d: %s\n", id, name);
    }
    
    return channel_->addSequence(id, name, callback, timing);
}

bool AutoLightV4::removeSequence(uint8_t id) {
    if (!channel_) return false;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Removing sequence %d\n", id);
    }
    
    return channel_->removeSequence(id);
}

void AutoLightV4::enableHotSwap(bool enabled) {
    if (!config_) return;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Hot-swap %s\n", enabled ? "enabled" : "disabled");
    }
    
    config_->enableHotSwap(enabled);
}

void AutoLightV4::enableAutoScan(bool enabled, uint32_t interval_ms) {
    if (!config_) return;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Auto-scan %s (interval: %d ms)\n", 
                      enabled ? "enabled" : "disabled", interval_ms);
    }
    
    config_->enableAutoScan(enabled, interval_ms);
}

void AutoLightV4::enableAutoFix(bool enabled) {
    if (!config_) return;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Auto-fix %s\n", enabled ? "enabled" : "disabled");
    }
    
    config_->enableAutoFix(enabled);
}

void AutoLightV4::enableAutoCycle(bool enabled, uint32_t interval_ms) {
    if (!channel_) return;
    
    if (debug_enabled_) {
        Serial.printf("[AutoLightV4] Auto-cycle %s (interval: %d ms)\n", 
                      enabled ? "enabled" : "disabled", interval_ms);
    }
    
    channel_->enableAutoCycle(enabled, interval_ms);
}

uint8_t AutoLightV4::getTotalChannels() const {
    if (!config_) return 0;
    return config_->getTotalChannels();
}

uint8_t AutoLightV4::getPCFCount() const {
    if (!config_) return 0;
    return config_->getPCFCount();
}

uint8_t AutoLightV4::getActiveSequences() const {
    if (!config_) return 0;
    return config_->getActiveSequences();
}

bool AutoLightV4::isConfigured() const {
    return config_ && config_->isConfigured();
}

bool AutoLightV4::isRunning() const {
    return is_initialized_ && config_ && config_->isRunning() && 
           channel_ && channel_->isRunning();
}

void AutoLightV4::printConfiguration() {
    if (!config_) return;
    config_->printConfiguration();
}

void AutoLightV4::printScanResult() {
    if (!config_) return;
    config_->printScanResult();
}

void AutoLightV4::printValidationReport() {
    if (!config_) return;
    config_->printValidationReport();
}

void AutoLightV4::printChannelStates() {
    if (!channel_) return;
    channel_->printChannelStates();
}

void AutoLightV4::printRuntimeStats() {
    if (!config_) return;
    config_->printRuntimeStats();
}

void AutoLightV4::setDebugMode(bool enabled) {
    debug_enabled_ = enabled;
    
    if (config_) {
        config_->setDebugMode(enabled);
    }
    
    if (channel_) {
        channel_->setDebugMode(enabled);
    }
}

bool AutoLightV4::getDebugMode() const {
    return debug_enabled_;
}

BaseConfig* AutoLightV4::getConfig() {
    return config_;
}

BaseChannel* AutoLightV4::getChannel() {
    return channel_;
}

void AutoLightV4::onHardwareChange(void (*callback)(uint8_t address, bool connected)) {
    if (!config_) return;
    config_->onHardwareChange(callback);
}

void AutoLightV4::onValidationError(void (*callback)(validation_result_t result, const char* message)) {
    if (!config_) return;
    config_->onValidationError(callback);
}

void AutoLightV4::onAutoFix(void (*callback)(const char* action, bool success)) {
    if (!config_) return;
    config_->onAutoFix(callback);
}

void AutoLightV4::onSequenceChange(void (*callback)(uint8_t old_seq, uint8_t new_seq)) {
    if (!channel_) return;
    channel_->onSequenceChange(callback);
}

void AutoLightV4::onChannelChange(void (*callback)(uint8_t channel, bool state)) {
    if (!channel_) return;
    channel_->onChannelChange(callback);
}

void AutoLightV4::onModeChange(void (*callback)(button_mode_v4_t old_mode, button_mode_v4_t new_mode)) {
    if (!channel_) return;
    channel_->onModeChange(callback);
}

bool AutoLightV4::testConnection() {
    if (!config_) return false;
    return config_->testConnection();
}

bool AutoLightV4::testAllChannels() {
    if (!channel_) return false;
    return channel_->testAllChannels();
}

RuntimeStats AutoLightV4::getRuntimeStats() {
    if (!config_) {
        RuntimeStats empty = {};
        return empty;
    }
    return config_->getRuntimeStats();
}

AutoScanResult AutoLightV4::getLastScan() {
    if (!config_) {
        AutoScanResult empty = {};
        return empty;
    }
    return config_->getLastScan();
}

ValidationReport AutoLightV4::getLastValidation() {
    if (!config_) {
        ValidationReport empty = {};
        return empty;
    }
    return config_->getLastValidation();
}

void AutoLightV4::initializeCallbacks() {
}

}  // namespace AutoLight