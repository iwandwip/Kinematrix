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
    }

    BaseConfig::~BaseConfig() {
        free(this);
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
                config_data_.header.pin_ptr_ = new uint8_t[config_data_.header.pin_size_];
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
        config_data_.header.pin_sequence_ = new uint8_t[_ch_sequence_size];
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
}