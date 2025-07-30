/*
 *  BaseChannel.h - AutoLightV4 Fully Dynamic Channel Management
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#ifndef AUTOLIGHT_V4_BASE_CHANNEL_H
#define AUTOLIGHT_V4_BASE_CHANNEL_H

#include "../Config/ConfigData.h"
#include "../Config/BaseConfig.h"
#include "Wire.h"

namespace AutoLight {

    struct ChannelData {
        uint8_t sequence_index_;
        bool is_on_;
        bool is_mode_changed_;
        bool is_sequence_running_;
        uint32_t last_update_time_;
        uint32_t sequence_start_time_;
        uint32_t custom_delay_;
        void* custom_data_;
    };

    struct SequenceManager {
        SequenceDefinition sequences[MAXNUM_SEQUENCES_V4];
        uint8_t active_count;
        uint8_t current_sequence;
        uint32_t default_timing;
        bool auto_cycle_enabled;
        uint32_t auto_cycle_interval;
        uint32_t last_cycle_time;
    };

    struct DynamicChannelState {
        uint8_t active_channels[MAXNUM_CHANNEL_V4];
        bool channel_states[MAXNUM_CHANNEL_V4];
        uint32_t channel_timings[MAXNUM_CHANNEL_V4];
        uint8_t channel_count;
        bool is_running;
        uint32_t update_interval;
        uint32_t last_update;
    };

    class BaseChannel {
    public:
        BaseChannel(bool enable_debug = false);
        ~BaseChannel();

        bool begin();
        bool end();
        
        void setConfig(BaseConfig* config);
        BaseConfig* getConfig() const;
        
        bool isConfigured() const;
        bool isRunning() const;
        
        void update();
        void reset();
        
        bool set(uint8_t channel, bool state);
        bool toggle(uint8_t channel);
        bool get(uint8_t channel) const;
        
        bool setAll(bool state);
        bool toggleAll();
        void clearAll();
        
        void setSequenceIndex(uint8_t index);
        uint8_t getSequenceIndex() const;
        
        void nextSequence();
        void previousSequence();
        void stopSequence();
        void pauseSequence();
        void resumeSequence();
        
        bool addSequence(uint8_t id, const char* name, 
                        void (*callback)(uint8_t*, uint8_t, uint32_t),
                        uint32_t timing = 50);
        bool removeSequence(uint8_t id);
        bool hasSequence(uint8_t id) const;
        SequenceDefinition* getSequence(uint8_t id);
        
        void setDefaultTiming(uint32_t timing);
        uint32_t getDefaultTiming() const;
        
        void setCustomDelay(uint32_t delay);
        uint32_t getCustomDelay() const;
        
        void enableAutoCycle(bool enabled, uint32_t interval_ms = 5000);
        bool isAutoCycleEnabled() const;
        
        void setButtonMode(button_mode_v4_t mode);
        button_mode_v4_t getButtonMode() const;
        
        void smartButtonPress(uint8_t button_index);
        void singleButtonCycle();
        void dualButtonPress(uint8_t button_index);
        void tripleButtonPress(uint8_t button_index);
        void quadButtonPress(uint8_t button_index);
        
        void toggleOnOff();
        void forceOn();
        void forceOff();
        
        uint8_t getTotalChannels() const;
        uint8_t getActiveChannels() const;
        
        bool testChannel(uint8_t channel);
        bool testAllChannels();
        
        void setUpdateInterval(uint32_t interval_ms);
        uint32_t getUpdateInterval() const;
        
        void onSequenceChange(void (*callback)(uint8_t old_seq, uint8_t new_seq));
        void onChannelChange(void (*callback)(uint8_t channel, bool state));
        void onModeChange(void (*callback)(button_mode_v4_t old_mode, button_mode_v4_t new_mode));
        
        void printChannelStates();
        void printSequenceInfo();
        void printButtonModeInfo();
        
        void setDebugMode(bool enabled);
        bool getDebugMode() const;
        
        bool exportSequences(SequenceDefinition* buffer, uint8_t buffer_size);
        bool importSequences(const SequenceDefinition* sequences, uint8_t count);
        
        DynamicChannelState* getChannelState();
        SequenceManager* getSequenceManager();
        ChannelData* getChannelData();
        
    private:
        BaseConfig* config_ptr_;
        ChannelData channel_data_;
        SequenceManager sequence_manager_;
        DynamicChannelState channel_state_;
        
        bool debug_enabled_;
        bool is_initialized_;
        bool is_running_;
        
        uint32_t total_task_sequence_;
        uint32_t last_button_press_time_;
        button_mode_v4_t current_button_mode_;
        
        void (*sequence_change_callback_)(uint8_t old_seq, uint8_t new_seq);
        void (*channel_change_callback_)(uint8_t channel, bool state);
        void (*mode_change_callback_)(button_mode_v4_t old_mode, button_mode_v4_t new_mode);
        
        bool initializeChannels();
        bool initializeSequences();
        bool initializeButtonSystem();
        
        void updateSequences();
        void updateChannelStates();
        void updateAutoCycle();
        
        bool writeToI2C(uint8_t address, uint8_t pin, bool state);
        bool readFromI2C(uint8_t address, uint8_t pin);
        
        void executeSequence(uint8_t sequence_id);
        void stopCurrentSequence();
        
        bool validateChannelIndex(uint8_t channel) const;
        bool validateSequenceIndex(uint8_t sequence) const;
        
        PCFDevice* findPCFForChannel(uint8_t channel);
        ChannelMap* findChannelMap(uint8_t channel);
        
        void triggerSequenceChangeCallback(uint8_t old_seq, uint8_t new_seq);
        void triggerChannelChangeCallback(uint8_t channel, bool state);
        void triggerModeChangeCallback(button_mode_v4_t old_mode, button_mode_v4_t new_mode);
        
        void handleSingleButtonLogic();
        void handleDualButtonLogic(uint8_t button_index);
        void handleTripleButtonLogic(uint8_t button_index);
        void handleQuadButtonLogic(uint8_t button_index);
        
        void debugLog(const char* message);
        void debugPrint(const char* format, ...);
        
        void initializeDefaultSequences();
        void addDefaultSequence(uint8_t id, const char* name, 
                              void (*callback)(uint8_t*, uint8_t, uint32_t));
        
        static void defaultSequenceOff(uint8_t* channels, uint8_t count, uint32_t timing);
        static void defaultSequenceOn(uint8_t* channels, uint8_t count, uint32_t timing);
        static void defaultSequenceChase(uint8_t* channels, uint8_t count, uint32_t timing);
        static void defaultSequenceFade(uint8_t* channels, uint8_t count, uint32_t timing);
        static void defaultSequenceStrobe(uint8_t* channels, uint8_t count, uint32_t timing);
        static void defaultSequenceBlink(uint8_t* channels, uint8_t count, uint32_t timing);
        static void defaultSequenceRunning(uint8_t* channels, uint8_t count, uint32_t timing);
        static void defaultSequencePolice(uint8_t* channels, uint8_t count, uint32_t timing);
        
        void taskSequence2();
        void taskSequence3(); 
        void taskSequence4();
        void taskSequence5();
        void taskSequence6();
        void taskSequence7();
        void taskSequence8();
        void taskSequence9();
        void taskSequence10();
        void taskSequence11();
        void taskSequence12();
        void taskSequence13();
        void taskSequence14();
        void taskSequence15();
        
        void sleep(uint32_t ms);
        
        bool initialize();
        void enableI2CConfig();
        void attachConfig(void* config_data);
        void attachInterrupt(void* button_interrupt);
        
        void setParamCallback(void (*callback)());
        void setInitDelay(uint32_t time);
        void setInitSequence(uint8_t sequence);
        void setTotalSequence(uint8_t nums);
        void reverse(bool is_reverse = false);
        
        volatile uint32_t getDelayTime();
        void* getChannelData();
        
        void nextMode();
        void previousMode();
        void onMode();
        void offMode();
        
        void changeMode();
        void changeModeApp(uint32_t num);
        volatile bool isChangeMode();
        volatile bool isOn();
        
        void runAutoLight(void (*callbackRun)(uint32_t sequence) = nullptr);
        
        void setStateHigh(int index, ...);
        void setStateLow(int index, ...);
        
        void off();
        void on();
        
        bool isReady();
        void debug();
    };

    class ChannelBuilder {
    public:
        ChannelBuilder(BaseChannel* channel);
        
        ChannelBuilder& config(BaseConfig* config);
        ChannelBuilder& sequences(uint8_t count);
        ChannelBuilder& timing(uint32_t default_timing);
        ChannelBuilder& buttonMode(button_mode_v4_t mode);
        ChannelBuilder& autoCycle(bool enabled, uint32_t interval_ms = 5000);
        ChannelBuilder& updateInterval(uint32_t interval_ms);
        ChannelBuilder& debugMode(bool enabled);
        
        ChannelBuilder& addSequence(uint8_t id, const char* name, 
                                  void (*callback)(uint8_t*, uint8_t, uint32_t),
                                  uint32_t timing = 50);
        
        bool apply();
        bool validate();
        void reset();
        
    private:
        BaseChannel* channel_;
        bool is_building_;
        
        bool validateSequenceCount(uint8_t count);
        bool validateTiming(uint32_t timing);
        bool validateButtonMode(button_mode_v4_t mode);
    };
}

#endif // AUTOLIGHT_V4_BASE_CHANNEL_H