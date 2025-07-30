/*
 *  BaseChannel.cpp - AutoLightV4 Fully Dynamic Channel Management
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 23
 */

#include "BaseChannel.h" 
#include <stdarg.h>

namespace AutoLight {

BaseChannel::BaseChannel(bool enable_debug)
    : config_ptr_(nullptr)
    , debug_enabled_(enable_debug)
    , is_initialized_(false)
    , is_running_(false)
    , total_task_sequence_(15)
    , last_button_press_time_(0)
    , current_button_mode_(BUTTON_MODE_AUTO_DETECT)
    , sequence_change_callback_(nullptr)
    , channel_change_callback_(nullptr)
    , mode_change_callback_(nullptr) {
    
    memset(&channel_data_, 0, sizeof(ChannelData));
    memset(&sequence_manager_, 0, sizeof(SequenceManager));
    memset(&channel_state_, 0, sizeof(DynamicChannelState));
    
    channel_data_.sequence_index_ = 0;
    channel_data_.is_on_ = false;
    channel_data_.is_mode_changed_ = false;
    channel_data_.is_sequence_running_ = false;
    channel_data_.custom_delay_ = 50;
    
    sequence_manager_.active_count = 0;
    sequence_manager_.current_sequence = 0;
    sequence_manager_.default_timing = 50;
    sequence_manager_.auto_cycle_enabled = false;
    sequence_manager_.auto_cycle_interval = 5000;
    sequence_manager_.last_cycle_time = 0;
    
    channel_state_.channel_count = 0;
    channel_state_.is_running = false;
    channel_state_.update_interval = 20;
    channel_state_.last_update = 0;
}

BaseChannel::~BaseChannel() {
    end();
}

bool BaseChannel::begin() {
    if (is_initialized_) return true;
    
    debugLog("BaseChannel::begin() - Initializing dynamic channel system");
    
    if (!config_ptr_) {
        debugLog("BaseChannel::begin() - No configuration provided");
        return false;
    }
    
    if (!config_ptr_->isConfigured()) {
        debugLog("BaseChannel::begin() - Configuration not ready");
        return false;
    }
    
    if (!initializeChannels()) {
        debugLog("BaseChannel::begin() - Failed to initialize channels");
        return false;
    }
    
    if (!initializeSequences()) {
        debugLog("BaseChannel::begin() - Failed to initialize sequences");
        return false;
    }
    
    if (!initializeButtonSystem()) {
        debugLog("BaseChannel::begin() - Failed to initialize button system");
        return false;
    }
    
    channel_state_.channel_count = config_ptr_->getTotalChannels();
    channel_state_.is_running = true;
    
    is_initialized_ = true;
    is_running_ = true;
    
    debugPrint("BaseChannel::begin() - Initialized with %d channels", channel_state_.channel_count);
    return true;
}

bool BaseChannel::end() {
    if (!is_initialized_) return true;
    
    debugLog("BaseChannel::end() - Shutting down channel system");
    
    stopSequence();
    clearAll();
    
    is_running_ = false;
    channel_state_.is_running = false;
    is_initialized_ = false;
    
    return true;
}

void BaseChannel::setConfig(BaseConfig* config) {
    config_ptr_ = config;
    if (is_initialized_ && config_ptr_) {
        channel_state_.channel_count = config_ptr_->getTotalChannels();
    }
}

BaseConfig* BaseChannel::getConfig() const {
    return config_ptr_;
}

bool BaseChannel::isConfigured() const {
    return config_ptr_ && config_ptr_->isConfigured();
}

bool BaseChannel::isRunning() const {
    return is_running_ && channel_state_.is_running;
}

void BaseChannel::update() {
    if (!is_running_) return;
    
    uint32_t current_time = millis();
    
    if ((current_time - channel_state_.last_update) >= channel_state_.update_interval) {
        updateSequences();
        updateChannelStates();
        updateAutoCycle();
        
        channel_state_.last_update = current_time;
        channel_data_.last_update_time_ = current_time;
    }
}

bool BaseChannel::set(uint8_t channel, bool state) {
    if (!validateChannelIndex(channel)) return false;
    
    ChannelMap* ch_map = findChannelMap(channel);
    if (!ch_map) return false;
    
    bool success = writeToI2C(ch_map->pcf_address, ch_map->pcf_pin, state);
    if (success) {
        channel_state_.channel_states[channel] = state;
        triggerChannelChangeCallback(channel, state);
        
        debugPrint("BaseChannel::set() - Channel %d = %s", channel, state ? "ON" : "OFF");
    }
    
    return success;
}

bool BaseChannel::toggle(uint8_t channel) {
    if (!validateChannelIndex(channel)) return false;
    return set(channel, !channel_state_.channel_states[channel]);
}

bool BaseChannel::get(uint8_t channel) const {
    if (!validateChannelIndex(channel)) return false;
    return channel_state_.channel_states[channel];
}

bool BaseChannel::setAll(bool state) {
    bool success = true;
    for (uint8_t i = 0; i < channel_state_.channel_count; i++) {
        if (!set(i, state)) {
            success = false;
        }
    }
    return success;
}

void BaseChannel::clearAll() {
    setAll(false);
}

void BaseChannel::setSequenceIndex(uint8_t index) {
    if (!validateSequenceIndex(index)) return;
    
    uint8_t old_sequence = channel_data_.sequence_index_;
    
    stopCurrentSequence();
    
    channel_data_.sequence_index_ = index;
    sequence_manager_.current_sequence = index;
    channel_data_.is_mode_changed_ = true;
    
    if (index == 0) {
        channel_data_.is_on_ = false;
        clearAll();
    } else if (index == 1) {
        channel_data_.is_on_ = true;
        setAll(true);
    } else {
        channel_data_.is_on_ = true;
        channel_data_.is_sequence_running_ = true;
        channel_data_.sequence_start_time_ = millis();
    }
    
    triggerSequenceChangeCallback(old_sequence, index);
    
    debugPrint("BaseChannel::setSequenceIndex() - Changed to sequence %d", index);
}

uint8_t BaseChannel::getSequenceIndex() const {
    return channel_data_.sequence_index_;
}

void BaseChannel::nextSequence() {
    uint8_t next = channel_data_.sequence_index_ + 1;
    if (next > total_task_sequence_) {
        next = 0;
    }
    setSequenceIndex(next);
}

void BaseChannel::previousSequence() {
    uint8_t prev = channel_data_.sequence_index_;
    if (prev == 0) {
        prev = total_task_sequence_;
    } else {
        prev--;
    }
    setSequenceIndex(prev);
}

void BaseChannel::stopSequence() {
    channel_data_.is_sequence_running_ = false;
    debugLog("BaseChannel::stopSequence() - Sequence stopped");
}

void BaseChannel::pauseSequence() {
    channel_data_.is_sequence_running_ = false;
    debugLog("BaseChannel::pauseSequence() - Sequence paused");
}

void BaseChannel::resumeSequence() {
    if (channel_data_.sequence_index_ > 1) {
        channel_data_.is_sequence_running_ = true;
        debugLog("BaseChannel::resumeSequence() - Sequence resumed");
    }
}

void BaseChannel::smartButtonPress(uint8_t button_index) {
    uint32_t current_time = millis();
    if ((current_time - last_button_press_time_) < 200) {
        return;
    }
    last_button_press_time_ = current_time;
    
    switch (current_button_mode_) {
        case BUTTON_MODE_SINGLE:
            handleSingleButtonLogic();
            break;
        case BUTTON_MODE_DUAL:
            handleDualButtonLogic(button_index);
            break;
        case BUTTON_MODE_TRIPLE:
            handleTripleButtonLogic(button_index);
            break;
        case BUTTON_MODE_QUAD:
            handleQuadButtonLogic(button_index);
            break;
        default:
            handleSingleButtonLogic();
            break;
    }
    
    debugPrint("BaseChannel::smartButtonPress() - Button %d pressed in mode %d", 
               button_index, current_button_mode_);
}

void BaseChannel::singleButtonCycle() {
    channel_data_.is_mode_changed_ = true;
    
    if (!channel_data_.is_on_) {
        setSequenceIndex(1);
    } else {
        uint8_t next_seq = channel_data_.sequence_index_ + 1;
        if (next_seq > total_task_sequence_) {
            setSequenceIndex(0);
        } else {
            setSequenceIndex(next_seq);
        }
    }
    
    debugPrint("BaseChannel::singleButtonCycle() - Cycled to sequence %d", 
               channel_data_.sequence_index_);
}

void BaseChannel::toggleOnOff() {
    if (channel_data_.is_on_) {
        setSequenceIndex(0);
    } else {
        setSequenceIndex(1);
    }
}

bool BaseChannel::addSequence(uint8_t id, const char* name, 
                             void (*callback)(uint8_t*, uint8_t, uint32_t),
                             uint32_t timing) {
    if (id >= MAXNUM_SEQUENCES_V4) return false;
    
    SequenceDefinition& seq = sequence_manager_.sequences[id];
    seq.id = id;
    strncpy(seq.name, name, sizeof(seq.name) - 1);
    seq.name[sizeof(seq.name) - 1] = '\0';
    seq.callback = callback;
    seq.default_timing = timing;
    seq.is_custom = true;
    seq.is_active = true;
    
    if (id >= sequence_manager_.active_count) {
        sequence_manager_.active_count = id + 1;
    }
    
    debugPrint("BaseChannel::addSequence() - Added sequence %d: %s", id, name);
    return true;
}

void BaseChannel::setButtonMode(button_mode_v4_t mode) {
    button_mode_v4_t old_mode = current_button_mode_;
    current_button_mode_ = mode;
    
    triggerModeChangeCallback(old_mode, mode);
    
    debugPrint("BaseChannel::setButtonMode() - Changed to mode %d", mode);
}

button_mode_v4_t BaseChannel::getButtonMode() const {
    return current_button_mode_;
}

uint8_t BaseChannel::getTotalChannels() const {
    return channel_state_.channel_count;
}

bool BaseChannel::writeToI2C(uint8_t address, uint8_t pin, bool state) {
    if (!config_ptr_) return false;
    
    PCFDevice* pcf = config_ptr_->findPCF(address);
    if (!pcf || !pcf->is_responding) return false;
    
    Wire.beginTransmission(address);
    Wire.write(state ? (1 << pin) : 0);
    bool success = (Wire.endTransmission() == 0);
    
    if (!success) {
        pcf->error_count++;
        if (pcf->error_count > 3) {
            pcf->is_responding = false;
            debugPrint("BaseChannel::writeToI2C() - PCF 0x%02X marked as non-responding", address);
        }
    } else {
        pcf->last_response_time = millis();
        pcf->error_count = 0;
    }
    
    return success;
}

bool BaseChannel::initializeChannels() {
    for (uint8_t i = 0; i < MAXNUM_CHANNEL_V4; i++) {
        channel_state_.channel_states[i] = false;
        channel_state_.channel_timings[i] = sequence_manager_.default_timing;
    }
    return true;
}

bool BaseChannel::initializeSequences() {
    initializeDefaultSequences();
    total_task_sequence_ = 15;
    return true;
}

bool BaseChannel::initializeButtonSystem() {
    if (current_button_mode_ == BUTTON_MODE_AUTO_DETECT) {
        current_button_mode_ = BUTTON_MODE_SINGLE;
    }
    return true;
}

void BaseChannel::updateSequences() {
    if (!channel_data_.is_sequence_running_) return;
    
    uint8_t seq_id = channel_data_.sequence_index_;
    if (seq_id < 2 || seq_id > 15) return;
    
    switch (seq_id) {
        case 2: taskSequence2(); break;
        case 3: taskSequence3(); break;
        case 4: taskSequence4(); break;
        case 5: taskSequence5(); break;
        case 6: taskSequence6(); break;
        case 7: taskSequence7(); break;
        case 8: taskSequence8(); break;
        case 9: taskSequence9(); break;
        case 10: taskSequence10(); break;
        case 11: taskSequence11(); break;
        case 12: taskSequence12(); break;
        case 13: taskSequence13(); break;
        case 14: taskSequence14(); break;
        case 15: taskSequence15(); break;
        default: break;
    }
}

void BaseChannel::updateAutoCycle() {
    if (!sequence_manager_.auto_cycle_enabled) return;
    
    uint32_t current_time = millis();
    if ((current_time - sequence_manager_.last_cycle_time) >= sequence_manager_.auto_cycle_interval) {
        nextSequence();
        sequence_manager_.last_cycle_time = current_time;
    }
}

void BaseChannel::handleSingleButtonLogic() {
    singleButtonCycle();
}

void BaseChannel::handleDualButtonLogic(uint8_t button_index) {
    if (button_index == 0) {
        toggleOnOff();
    } else if (button_index == 1) {
        if (channel_data_.is_on_) {
            nextSequence();
        }
    }
}

bool BaseChannel::validateChannelIndex(uint8_t channel) const {
    return channel < channel_state_.channel_count;
}

bool BaseChannel::validateSequenceIndex(uint8_t sequence) const {
    return sequence <= total_task_sequence_;
}

ChannelMap* BaseChannel::findChannelMap(uint8_t channel) {
    if (!config_ptr_) return nullptr;
    return config_ptr_->findChannel(channel);
}

void BaseChannel::triggerSequenceChangeCallback(uint8_t old_seq, uint8_t new_seq) {
    if (sequence_change_callback_) {
        sequence_change_callback_(old_seq, new_seq);
    }
}

void BaseChannel::triggerChannelChangeCallback(uint8_t channel, bool state) {
    if (channel_change_callback_) {
        channel_change_callback_(channel, state);
    }
}

void BaseChannel::triggerModeChangeCallback(button_mode_v4_t old_mode, button_mode_v4_t new_mode) {
    if (mode_change_callback_) {
        mode_change_callback_(old_mode, new_mode);
    }
}

void BaseChannel::debugLog(const char* message) {
    if (debug_enabled_) {
        Serial.print("[AutoLightV4 Channel] ");
        Serial.println(message);
    }
}

void BaseChannel::debugPrint(const char* format, ...) {
    if (!debug_enabled_) return;
    
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    debugLog(buffer);
}

void BaseChannel::initializeDefaultSequences() {
    addDefaultSequence(0, "OFF", defaultSequenceOff);
    addDefaultSequence(1, "ON", defaultSequenceOn);
    addDefaultSequence(2, "Sequence2", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(3, "Sequence3", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(4, "Sequence4", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(5, "Sequence5", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(6, "Sequence6", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(7, "Sequence7", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(8, "Sequence8", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(9, "Sequence9", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(10, "Sequence10", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(11, "Sequence11", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(12, "Sequence12", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(13, "Sequence13", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(14, "Sequence14", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
    addDefaultSequence(15, "Sequence15", [](uint8_t* channels, uint8_t count, uint32_t timing) {});
}

void BaseChannel::addDefaultSequence(uint8_t id, const char* name, 
                                   void (*callback)(uint8_t*, uint8_t, uint32_t)) {
    SequenceDefinition& seq = sequence_manager_.sequences[id];
    seq.id = id;
    strncpy(seq.name, name, sizeof(seq.name) - 1);
    seq.name[sizeof(seq.name) - 1] = '\0';
    seq.callback = callback;
    seq.default_timing = sequence_manager_.default_timing;
    seq.is_custom = false;
    seq.is_active = true;
    
    if (id >= sequence_manager_.active_count) {
        sequence_manager_.active_count = id + 1;
    }
}

void BaseChannel::defaultSequenceOff(uint8_t* channels, uint8_t count, uint32_t timing) {
}

void BaseChannel::defaultSequenceOn(uint8_t* channels, uint8_t count, uint32_t timing) {
}

void BaseChannel::defaultSequenceChase(uint8_t* channels, uint8_t count, uint32_t timing) {
    static uint8_t current_channel = 0;
    static uint32_t last_time = 0;
    
    uint32_t now = millis();
    if ((now - last_time) >= timing) {
        current_channel = (current_channel + 1) % count;
        last_time = now;
    }
}

void BaseChannel::defaultSequenceStrobe(uint8_t* channels, uint8_t count, uint32_t timing) {
    static bool strobe_state = false;
    static uint32_t last_time = 0;
    
    uint32_t now = millis();
    if ((now - last_time) >= timing) {
        strobe_state = !strobe_state;
        last_time = now;
    }
}

void BaseChannel::printChannelStates() {
    Serial.println("=== Channel States ===");
    Serial.printf("Total Channels: %d, Running: %s\n", 
                  channel_state_.channel_count, 
                  channel_state_.is_running ? "Yes" : "No");
    
    for (uint8_t i = 0; i < channel_state_.channel_count; i++) {
        Serial.printf("CH[%d]: %s\n", i, 
                      channel_state_.channel_states[i] ? "ON" : "OFF");
    }
}

bool BaseChannel::initialize() {
    return begin();
}

void BaseChannel::enableI2CConfig() {
    if (config_ptr_) {
        config_ptr_->enableAutoScan(true);
    }
}

void BaseChannel::attachConfig(void* config_data) {
    config_ptr_ = static_cast<BaseConfig*>(config_data);
}

void BaseChannel::attachInterrupt(void* button_interrupt) {
}

void BaseChannel::setParamCallback(void (*callback)()) {
}

void BaseChannel::setInitDelay(uint32_t time) {
    channel_data_.custom_delay_ = time;
}

void BaseChannel::setInitSequence(uint8_t sequence) {
    channel_data_.sequence_index_ = sequence;
}

void BaseChannel::setTotalSequence(uint8_t nums) {
    total_task_sequence_ = nums;
}

void BaseChannel::reverse(bool is_reverse) {
}

volatile uint32_t BaseChannel::getDelayTime() {
    return channel_data_.custom_delay_;
}

void* BaseChannel::getChannelData() {
    return &channel_data_;
}

void BaseChannel::nextMode() {
    nextSequence();
}

void BaseChannel::previousMode() {
    previousSequence();
}

void BaseChannel::onMode() {
    setSequenceIndex(1);
}

void BaseChannel::offMode() {
    setSequenceIndex(0);
}

void BaseChannel::changeMode() {
    nextSequence();
}

void BaseChannel::changeModeApp(uint32_t num) {
    setSequenceIndex(num);
}

volatile bool BaseChannel::isChangeMode() {
    return channel_data_.is_mode_changed_;
}

volatile bool BaseChannel::isOn() {
    return channel_data_.is_on_;
}

void BaseChannel::runAutoLight(void (*callbackRun)(uint32_t sequence)) {
    update();
    if (callbackRun) {
        callbackRun(channel_data_.sequence_index_);
    }
}

void BaseChannel::setStateHigh(int index, ...) {
    va_list args;
    va_start(args, index);
    
    set(index, HIGH);
    
    int next_index;
    while ((next_index = va_arg(args, int)) != -1) {
        set(next_index, HIGH);
    }
    
    va_end(args);
}

void BaseChannel::setStateLow(int index, ...) {
    va_list args;
    va_start(args, index);
    
    set(index, LOW);
    
    int next_index;
    while ((next_index = va_arg(args, int)) != -1) {
        set(next_index, LOW);
    }
    
    va_end(args);
}

void BaseChannel::off() {
    setSequenceIndex(0);
}

void BaseChannel::on() {
    setSequenceIndex(1);
}

bool BaseChannel::isReady() {
    return is_initialized_ && is_running_;
}

void BaseChannel::debug() {
    printChannelStates();
    if (config_ptr_) {
        config_ptr_->printConfiguration();
    }
}

}  // namespace AutoLight