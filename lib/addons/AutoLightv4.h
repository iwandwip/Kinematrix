/*
 *  AutoLightv4.h - Fully Dynamic LED Matrix Control System
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 *  
 *  AutoLightV4 Features:
 *  - Full Dynamic Configuration (Auto + Manual)
 *  - Super User-Friendly API
 *  - Runtime Hardware Detection & Adaptation
 *  - Intelligent Auto-Fix & Validation
 *  - Multi-Modal Button System
 *  - Hot-Swap PCF Support
 *  - Backward Compatibility with V1-V3
 */

#ifndef AUTOLIGHT_V4_H
#define AUTOLIGHT_V4_H

#if defined(ESP32)

#include "Arduino.h"
#include "AutoLightv4/index.h"

namespace AutoLight {
    
    class AutoLightV4 {
    public:
        AutoLightV4(bool enable_debug = false);
        ~AutoLightV4();
        
        bool begin();
        bool end();
        void update();
        void reset();
        
        scan_result_t autoScan();
        validation_result_t validate();
        bool autoFix();
        bool autoDistribute();
        
        ConfigBuilder& configure();
        
        bool addPCF(uint8_t address, uint8_t channels = 8);
        bool removePCF(uint8_t address);
        bool redistributeChannels();
        
        void setSequenceIndex(uint8_t index);
        uint8_t getSequenceIndex() const;
        
        void nextSequence();
        void previousSequence();
        
        void setButtonMode(button_mode_v4_t mode);
        button_mode_v4_t getButtonMode() const;
        
        void smartButtonPress(uint8_t button_index);
        void singleButtonCycle();
        void toggleOnOff();
        
        bool set(uint8_t channel, bool state);
        bool toggle(uint8_t channel);
        bool get(uint8_t channel) const;
        
        bool setAll(bool state);
        void clearAll();
        
        bool addSequence(uint8_t id, const char* name, 
                        void (*callback)(uint8_t*, uint8_t, uint32_t),
                        uint32_t timing = 50);
        bool removeSequence(uint8_t id);
        
        void enableHotSwap(bool enabled);
        void enableAutoScan(bool enabled, uint32_t interval_ms = 5000);
        void enableAutoFix(bool enabled);
        void enableAutoCycle(bool enabled, uint32_t interval_ms = 5000);
        
        uint8_t getTotalChannels() const;
        uint8_t getPCFCount() const;
        uint8_t getActiveSequences() const;
        
        bool isConfigured() const;
        bool isRunning() const;
        
        void printConfiguration();
        void printScanResult();
        void printValidationReport();
        void printChannelStates();
        void printRuntimeStats();
        
        void setDebugMode(bool enabled);
        bool getDebugMode() const;
        
        BaseConfig* getConfig();
        BaseChannel* getChannel();
        
        void onHardwareChange(void (*callback)(uint8_t address, bool connected));
        void onValidationError(void (*callback)(validation_result_t result, const char* message));
        void onAutoFix(void (*callback)(const char* action, bool success));
        void onSequenceChange(void (*callback)(uint8_t old_seq, uint8_t new_seq));
        void onChannelChange(void (*callback)(uint8_t channel, bool state));
        void onModeChange(void (*callback)(button_mode_v4_t old_mode, button_mode_v4_t new_mode));
        
        bool testConnection();
        bool testAllChannels();
        
        bool importFromV3(const void* v3_config);
        bool exportToV3(void* v3_config);
        
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

#endif // ESP32

#endif // AUTOLIGHT_V4_H