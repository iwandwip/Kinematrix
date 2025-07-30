/*
 *  BaseConfig.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#include "BaseConfig.h"

namespace AutoLight {
    BaseConfig::BaseConfig(bool _enable_unit_test)
            : is_unit_test_(_enable_unit_test),
              is_custom_sequence_(false),
              resources_allocated_(false),
              config_data_{.header = {
                      .version_ = 0,
                      .channel_ = 0,
                      .io_size_ = 0,
                      .pin_size_ = 0,
                      .pin_ptr_ = nullptr,
                      .i2c_ptr_ = nullptr,
                      .pin_sequence_ = nullptr
              }, .table = {
                      .address_pin_ = {0x0001B, 0x0001A, 0x00019, 0x00021, 0x00020},
                      .i2c_address_ = {0x00020, 0x00024, 0x00022, 0x00023, 0x00024, 0x00025, 0x00026, 0x00027},
                      .version_ = {},
                      .channel_ = {}
              }} {
        // initialize version and channel table
        for (int i = 0; i < MAXNUM_VERSION; ++i) {
            config_data_.table.version_[i] = (MAXNUM_VERSION - 1) - i;
            config_data_.table.channel_[i] = (i + 1) * 2;
        }
        for (int i = 0; i < MAXNUM_IO_EXPANDER; i++) {
            dynamic_config_.pcf_list[i].is_active = false;
        }
        dynamic_config_.pcf_count = 0;
        dynamic_config_.total_channels = 0;
        dynamic_config_.strategy = DISTRIBUTE_BALANCED;
    }

    BaseConfig::~BaseConfig() {
        // Clean up allocated memory safely
        cleanup();
    }

    void BaseConfig::initialize() {
        // scanning I2C
        if (is_unit_test_) {
            i2c_scanner_.beginTransmission();
            i2c_scanner_.scanI2CAddress();
            i2c_scanner_.endTransmission();
        }

        switch (config_index_) {
            case CUSTOM_CONFIG:
                break;
            case DIO_CONFIG:
                for (int i = 0; i < config_data_.header.pin_size_; ++i) {
                    pinMode(config_data_.header.pin_ptr_[i], OUTPUT);
                    digitalWrite(config_data_.header.pin_ptr_[i], HIGH);
                }
                break;
            case I2C_CONFIG:
                config_data_.header.pin_size_ = config_data_.header.channel_;
                
                // Clean up existing allocation before creating new one
                if (config_data_.header.pin_ptr_) {
                    delete[] config_data_.header.pin_ptr_;
                    config_data_.header.pin_ptr_ = nullptr;
                }
                
                config_data_.header.pin_ptr_ = new uint8_t[config_data_.header.pin_size_];
                resources_allocated_ = true;
                
                for (uint8_t i = 0; i < config_data_.header.pin_size_; ++i) {
                    if (is_custom_sequence_) config_data_.header.pin_ptr_[i] = config_data_.header.pin_sequence_[i] - 1;
                    else config_data_.header.pin_ptr_[i] = i;
                }
                break;
        }
    }

    int BaseConfig::readChannel() {
        for (int i = 0; i < MAXNUM_CHANNEL_PIN; i++) {
            pinMode(config_data_.table.address_pin_[i], INPUT_PULLUP);
        }
        int read_value = 0;
        for (int i = 0; i < MAXNUM_CHANNEL_PIN; i++) {
            read_value = (read_value << 1) | digitalRead(config_data_.table.address_pin_[i]); // int read_value = (value_pin[0] << 4) | (value_pin[1] << 3) | (value_pin[2] << 2) | (value_pin[3] << 1) | value_pin[4];
        }
        for (int i = 0; i < MAXNUM_VERSION; i++) {
            if (read_value == config_data_.table.version_[i]) {
                config_data_.header.channel_ = config_data_.table.channel_[i];
            }
        }
        // calculate the io size
        config_data_.header.io_size_ = 0;
        for (int i = 0; i <= 64; i += 8) {
            if (config_data_.header.channel_ > i && !(config_data_.header.channel_ % 2)) {
                config_data_.header.io_size_++;
            }
        }

        return (config_data_.header.channel_ == 0) ? 0 : 1;
    }

    int BaseConfig::readVersion() {
        config_data_.header.version_ = (config_data_.header.channel_ >= 0x00002 && config_data_.header.channel_ <= 0x00010) ? 0x0001 : (config_data_.header.channel_ >= 0x00012 && config_data_.header.channel_ <= 0x00018) ? 0x0002 : 0x0003;
        return (config_data_.header.version_ == 0) ? 0 : 1;
    }

    void BaseConfig::setVersion(uint8_t _version) {
        config_data_.header.version_ = _version;
    }

    void BaseConfig::setChannel(uint8_t _channel) {
        config_data_.header.channel_ = _channel;

        // calculate the io size
        config_data_.header.io_size_ = 0;
        for (int i = 0; i <= 64; i += 8) {
            if (config_data_.header.channel_ > i && !(config_data_.header.channel_ % 2)) {
                config_data_.header.io_size_++;
            }
        }
    }

    int BaseConfig::getChannel() {
        return config_data_.header.channel_;
    }

    int BaseConfig::getVersion() {
        return config_data_.header.version_;
    }

    int BaseConfig::getIoSize() {
        return config_data_.header.io_size_;
    }

    void BaseConfig::setConfigIndex(int _config_index) {
        config_index_ = _config_index;
    }

    void BaseConfig::setSequence(int _ch_sequence_size, ...) {
        va_list args;
        va_start(args, _ch_sequence_size);
        is_custom_sequence_ = true;
        
        // Clean up existing allocation before creating new one
        if (config_data_.header.pin_sequence_) {
            delete[] config_data_.header.pin_sequence_;
            config_data_.header.pin_sequence_ = nullptr;
        }
        
        config_data_.header.pin_sequence_ = new uint8_t[_ch_sequence_size];
        resources_allocated_ = true;
        
        for (int i = 0; i < _ch_sequence_size; i++) {
            config_data_.header.pin_sequence_[i] = static_cast<uint8_t>(va_arg(args, int));
        }
        va_end(args);
    }

    void BaseConfig::setChannelPin(int _ch_pin_size, ...) {
        va_list args;
        va_start(args, _ch_pin_size);
        for (int i = 0; i < _ch_pin_size; i++) {
            config_data_.table.address_pin_[i] = static_cast<uint8_t>(va_arg(args, int));
        }
        va_end(args);
    }

    void BaseConfig::setAddress(int _address_size, ...) {
        va_list args;
        va_start(args, _address_size);
        for (int i = 0; i < _address_size; i++) {
            config_data_.table.i2c_address_[i] = static_cast<uint8_t>(va_arg(args, int));
        }
        va_end(args);
    }

    ConfigData *BaseConfig::getConfigData() {
        return &config_data_;
    }

    BaseConfig *BaseConfig::getConfig() {
        return this;
    }

    int BaseConfig::getConfigIndex() {
        return config_index_;
    }

    uint8_t *BaseConfig::getPinList() {
        return config_data_.header.pin_ptr_;
    }

    BaseConfig *BaseConfig::operator()() {
        return this;
    }

    BaseConfig *BaseConfig::operator=(BaseConfig *_other) {
        return _other;
    }

    void BaseConfig::debugConfig() {
        if (!is_unit_test_) return;
        Serial.println("==============================");
        Serial.println("| START DEBUG CONFIG");
        Serial.println("==============================");

        Serial.println("| STATIC CONFIG DATA");

        Serial.print("| STATIC address_pin_          : ");
        for (int i = 0; i < MAXNUM_CHANNEL_PIN; ++i) {
            Serial.print("| ");
            Serial.print(config_data_.table.address_pin_[i]);
            Serial.print("(0x000");
            Serial.print(config_data_.table.address_pin_[i], HEX);
            Serial.print(")");
        }
        Serial.println();

        Serial.print("| STATIC i2c_address_          : ");
        for (int i = 0; i < MAXNUM_IO_EXPANDER; ++i) {
            Serial.print("| 0x000");
            Serial.print(config_data_.table.i2c_address_[i], HEX);
        }
        Serial.println();

        Serial.print("| version_                     : ");
        for (int i = 0; i < MAXNUM_VERSION; ++i) {
            Serial.print("| 0x000");
            Serial.print(config_data_.table.version_[i], HEX);
        }
        Serial.println();

        Serial.print("| channel_                     : ");
        for (int i = 0; i < MAXNUM_VERSION; ++i) {
            Serial.print("| 0x000");
            Serial.print(config_data_.table.channel_[i], HEX);
        }
        Serial.println();

        Serial.print("| config_data_.header.channel_ : ");
        Serial.print("| ");
        Serial.print(config_data_.header.channel_);
        Serial.println();

        Serial.print("| config_data_.header.io_size_ : ");
        Serial.print("| ");
        Serial.print(config_data_.header.io_size_);
        Serial.println();

        Serial.print("| config_data_.header.version_ : ");
        Serial.print("| ");
        Serial.print(config_data_.header.version_);
        Serial.println();

        Serial.print("| config_data_.header.pin_size_: ");
        Serial.print("| ");
        Serial.print(config_data_.header.pin_size_);
        Serial.println();

        Serial.print("| config_data_.header.pin_ptr_ : ");
        for (int i = 0; i < config_data_.header.pin_size_; ++i) {
            Serial.print("| ");
            Serial.print(config_data_.header.pin_ptr_[i]);
        }
        Serial.println();

        Serial.println("| END DEBUG CONFIG");
    }

    void BaseConfig::setDynamicConfig(uint8_t total_channels) {
        uint8_t optimal_pcf = getOptimalPCFCount(total_channels);
        setDynamicConfig(total_channels, optimal_pcf, DISTRIBUTE_BALANCED);
    }

    void BaseConfig::setDynamicConfig(uint8_t total_channels, uint8_t num_pcf) {
        setDynamicConfig(total_channels, num_pcf, DISTRIBUTE_BALANCED);
    }

    void BaseConfig::setDynamicConfig(uint8_t total_channels, uint8_t num_pcf, distribution_strategy_t strategy) {
        dynamic_config_.total_channels = total_channels;
        dynamic_config_.pcf_count = num_pcf;
        dynamic_config_.strategy = strategy;

        for (int i = 0; i < MAXNUM_IO_EXPANDER; i++) {
            dynamic_config_.pcf_list[i].is_active = false;
        }

        generateDefaultAddresses(num_pcf);

        switch(strategy) {
            case DISTRIBUTE_BALANCED:
                calculateBalancedDistribution(total_channels, num_pcf);
                break;
            case DISTRIBUTE_OPTIMIZED:
                calculateOptimizedDistribution(total_channels, num_pcf);
                break;
            case DISTRIBUTE_SEQUENTIAL:
                calculateSequentialDistribution(total_channels, num_pcf);
                break;
        }

        applyDynamicConfig();
    }

    void BaseConfig::setDynamicDistribution(uint8_t total_channels, uint8_t num_pcf, ...) {
        va_list args;
        va_start(args, num_pcf);

        dynamic_config_.total_channels = total_channels;
        dynamic_config_.pcf_count = num_pcf;
        dynamic_config_.strategy = DISTRIBUTE_CUSTOM;

        generateDefaultAddresses(num_pcf);

        uint8_t current_channel = 1;
        for (int i = 0; i < num_pcf; i++) {
            uint8_t channels_for_this_pcf = va_arg(args, int);

            dynamic_config_.pcf_list[i].address = 0x20 + i;
            dynamic_config_.pcf_list[i].used_pins = channels_for_this_pcf;
            dynamic_config_.pcf_list[i].start_channel = current_channel;
            dynamic_config_.pcf_list[i].pin_offset = 0;
            dynamic_config_.pcf_list[i].is_active = true;

            current_channel += channels_for_this_pcf;
        }

        va_end(args);
        applyDynamicConfig();
    }

    void BaseConfig::setDynamicWithAddresses(uint8_t total_channels, uint8_t num_pcf, uint8_t* addresses, uint8_t* channels_per_pcf) {
        dynamic_config_.total_channels = total_channels;
        dynamic_config_.pcf_count = num_pcf;
        dynamic_config_.strategy = DISTRIBUTE_CUSTOM;

        uint8_t current_channel = 1;
        for (int i = 0; i < num_pcf; i++) {
            dynamic_config_.pcf_list[i].address = addresses[i];
            dynamic_config_.pcf_list[i].used_pins = channels_per_pcf[i];
            dynamic_config_.pcf_list[i].start_channel = current_channel;
            dynamic_config_.pcf_list[i].pin_offset = 0;
            dynamic_config_.pcf_list[i].is_active = true;

            current_channel += channels_per_pcf[i];
        }

        applyDynamicConfig();
    }

    void BaseConfig::setCustomDistribution(PCFDistribution* pcf_array, uint8_t pcf_count) {
        dynamic_config_.pcf_count = pcf_count;
        dynamic_config_.strategy = DISTRIBUTE_CUSTOM;
        dynamic_config_.total_channels = 0;

        for (int i = 0; i < MAXNUM_IO_EXPANDER; i++) {
            dynamic_config_.pcf_list[i].is_active = false;
        }

        for (int i = 0; i < pcf_count; i++) {
            dynamic_config_.pcf_list[i] = pcf_array[i];
            dynamic_config_.total_channels += pcf_array[i].used_pins;
        }

        applyDynamicConfig();
    }

    uint8_t BaseConfig::getOptimalPCFCount(uint8_t channels) {
        if (channels <= 8) return 1;
        if (channels <= 16) return 2;
        if (channels <= 24) return 3;
        if (channels <= 32) return 4;
        return (channels + 7) / 8;
    }

    DynamicConfigData* BaseConfig::getCurrentDistribution() {
        return &dynamic_config_;
    }

    bool BaseConfig::validateDistribution() {
        uint8_t total_calculated = 0;
        for (int i = 0; i < dynamic_config_.pcf_count; i++) {
            if (dynamic_config_.pcf_list[i].is_active) {
                total_calculated += dynamic_config_.pcf_list[i].used_pins;
                if (dynamic_config_.pcf_list[i].used_pins > 8) return false;
                if (dynamic_config_.pcf_list[i].pin_offset + dynamic_config_.pcf_list[i].used_pins > 8) return false;
            }
        }
        return total_calculated == dynamic_config_.total_channels;
    }

    void BaseConfig::printDistribution() {
        Serial.println(F("=== Dynamic PCF Distribution ==="));
        Serial.print(F("Total Channels: ")); Serial.println(dynamic_config_.total_channels);
        Serial.print(F("Total PCFs: ")); Serial.println(dynamic_config_.pcf_count);

        for (int i = 0; i < dynamic_config_.pcf_count; i++) {
            if (dynamic_config_.pcf_list[i].is_active) {
                Serial.print(F("PCF")); Serial.print(i+1);
                Serial.print(F(" (0x")); Serial.print(dynamic_config_.pcf_list[i].address, HEX);
                Serial.print(F("): Channels "));
                Serial.print(dynamic_config_.pcf_list[i].start_channel);
                Serial.print(F("-"));
                Serial.print(dynamic_config_.pcf_list[i].start_channel + dynamic_config_.pcf_list[i].used_pins - 1);
                Serial.print(F(" → Pins P"));
                Serial.print(dynamic_config_.pcf_list[i].pin_offset);
                Serial.print(F("-P"));
                Serial.println(dynamic_config_.pcf_list[i].pin_offset + dynamic_config_.pcf_list[i].used_pins - 1);
            }
        }
        Serial.println(F("================================"));
    }

    void BaseConfig::calculateBalancedDistribution(uint8_t total_channels, uint8_t num_pcf) {
        uint8_t base_channels = total_channels / num_pcf;
        uint8_t extra_channels = total_channels % num_pcf;

        uint8_t current_channel = 0;  // Use 0-based indexing for consistency
        for (int i = 0; i < num_pcf; i++) {
            dynamic_config_.pcf_list[i].used_pins = base_channels + (i < extra_channels ? 1 : 0);
            dynamic_config_.pcf_list[i].start_channel = current_channel;
            dynamic_config_.pcf_list[i].pin_offset = 0;
            dynamic_config_.pcf_list[i].is_active = true;

            current_channel += dynamic_config_.pcf_list[i].used_pins;
        }
    }

    void BaseConfig::calculateOptimizedDistribution(uint8_t total_channels, uint8_t num_pcf) {
        // Optimized: Use minimum number of PCFs needed
        uint8_t needed_pcfs = (total_channels + 7) / 8;  // Ceiling division
        needed_pcfs = (needed_pcfs > num_pcf) ? num_pcf : needed_pcfs;          // Don't exceed available
        
        // Distribute channels across minimum needed PCFs
        calculateBalancedDistribution(total_channels, needed_pcfs);
        
        // Mark unused PCFs as inactive
        for (int i = needed_pcfs; i < num_pcf; i++) {
            dynamic_config_.pcf_list[i].is_active = false;
            dynamic_config_.pcf_list[i].used_pins = 0;
        }
    }

    void BaseConfig::calculateSequentialDistribution(uint8_t total_channels, uint8_t num_pcf) {
        // Sequential: Fill PCFs one by one with 8 channels each
        uint8_t remaining = total_channels;
        uint8_t current_channel = 0;  // Use 0-based indexing for consistency

        for (int i = 0; i < num_pcf; i++) {
            if (remaining > 0) {
                uint8_t channels_for_this_pcf = (remaining > 8) ? 8 : remaining;

                dynamic_config_.pcf_list[i].used_pins = channels_for_this_pcf;
                dynamic_config_.pcf_list[i].start_channel = current_channel;
                dynamic_config_.pcf_list[i].pin_offset = 0;
                dynamic_config_.pcf_list[i].is_active = true;

                current_channel += channels_for_this_pcf;
                remaining -= channels_for_this_pcf;
            } else {
                // Mark unused PCFs as inactive
                dynamic_config_.pcf_list[i].is_active = false;
                dynamic_config_.pcf_list[i].used_pins = 0;
            }
        }
    }

    void BaseConfig::generateDefaultAddresses(uint8_t num_pcf, uint8_t start_addr) {
        for (int i = 0; i < num_pcf; i++) {
            dynamic_config_.pcf_list[i].address = start_addr + i;
        }
    }

    void BaseConfig::applyDynamicConfig() {
        config_index_ = CUSTOM_CONFIG;
        is_custom_sequence_ = true;

        config_data_.header.channel_ = dynamic_config_.total_channels;
        config_data_.header.io_size_ = dynamic_config_.pcf_count;

        // Clean up existing allocations
        if (config_data_.header.pin_ptr_) {
            delete[] config_data_.header.pin_ptr_;
            config_data_.header.pin_ptr_ = nullptr;
        }
        if (config_data_.header.pin_sequence_) {
            delete[] config_data_.header.pin_sequence_;
            config_data_.header.pin_sequence_ = nullptr;
        }

        config_data_.header.pin_size_ = dynamic_config_.total_channels;
        config_data_.header.pin_ptr_ = new uint8_t[dynamic_config_.total_channels];
        config_data_.header.pin_sequence_ = new uint8_t[dynamic_config_.total_channels];
        resources_allocated_ = true;
        config_data_.header.i2c_ptr_ = config_data_.table.i2c_address_;

        for (int i = 0; i < dynamic_config_.pcf_count; i++) {
            config_data_.table.i2c_address_[i] = dynamic_config_.pcf_list[i].address;
        }

        uint8_t pin_index = 0;
        for (int pcf_idx = 0; pcf_idx < dynamic_config_.pcf_count; pcf_idx++) {
            if (dynamic_config_.pcf_list[pcf_idx].is_active) {
                uint8_t pcf_base = pcf_idx * 8;
                for (int pin = 0; pin < dynamic_config_.pcf_list[pcf_idx].used_pins; pin++) {
                    uint8_t actual_pin = pcf_base + dynamic_config_.pcf_list[pcf_idx].pin_offset + pin;
                    config_data_.header.pin_ptr_[pin_index] = actual_pin;
                    config_data_.header.pin_sequence_[pin_index] = actual_pin + 1;
                    pin_index++;
                }
            }
        }
    }

    I2CScanner::I2CScanner()
            : n_devices_(0) {
    }

    I2CScanner::~I2CScanner() {
        delete this;
    }

    void I2CScanner::beginTransmission() {
        Serial.println("==============================");
        Serial.println("| START SCANNING I2C");
        Serial.println("==============================");

        Wire.begin();
        Serial.println("| WIRE BEGIN");
    }

    void I2CScanner::scanI2CAddress() {
        byte error;
        int address;
        Serial.println("| Scanning...");
        n_devices_ = 0;
        for (address = 1; address < MAXNUM_I2C_SLAVE_ADDRESS; address++) {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0) {
                // Serial.print("| I2C device found at address [HEX] 0x");
                // if (address < 16)
                //     Serial.print("0");
                // Serial.print(address, HEX);

                // Serial.print("| [DEC] ");
                // Serial.print(address);
                // Serial.println("  !");
                n_address_[n_devices_] = address;
                n_devices_++;
            } else if (error == 4) {
                Serial.print("| Unknown error at address 0x");
                if (address < 16)
                    Serial.print("0");
                Serial.println(address, HEX);
            }
        }
        if (n_devices_ == 0)
            Serial.println("| No I2C devices found");
        else
            Serial.println("| Done Scanning");
    }

    void I2CScanner::endTransmission() {
        if (n_devices_ > 0) {
            Serial.print("| I2C devices found at address : ");
            for (int i = 0; i < n_devices_; ++i) {
                Serial.print("| 0x000");
                Serial.print(n_address_[i], HEX);
            }
            Serial.println();

            Serial.print("| Total devices found          : ");
            Serial.print("| ");
            Serial.print(n_devices_);
            Serial.println();
        }

        Wire.end();
        Serial.println("| WIRE END");
    }

    void BaseConfig::cleanup() {
        if (resources_allocated_) {
            if (config_data_.header.pin_ptr_) {
                delete[] config_data_.header.pin_ptr_;
                config_data_.header.pin_ptr_ = nullptr;
            }
            if (config_data_.header.pin_sequence_) {
                delete[] config_data_.header.pin_sequence_;
                config_data_.header.pin_sequence_ = nullptr;
            }
            resources_allocated_ = false;
        }
    }

    bool BaseConfig::validateConfiguration() {
        // Check basic constraints
        if (dynamic_config_.total_channels == 0) {
            Serial.println("ERROR: Zero channels configured");
            return false;
        }
        
        if (dynamic_config_.pcf_count == 0) {
            Serial.println("ERROR: No PCFs configured");
            return false;
        }
        
        // Check I2C address ranges
        for (int i = 0; i < dynamic_config_.pcf_count; i++) {
            if (dynamic_config_.pcf_list[i].is_active) {
                uint8_t addr = dynamic_config_.pcf_list[i].address;
                if (addr < 0x20 || addr > 0x27) {
                    Serial.printf("ERROR: Invalid I2C address 0x%02X for PCF %d\n", addr, i);
                    return false;
                }
            }
        }
        
        // Check channel mapping
        return validateDistribution();
    }
}