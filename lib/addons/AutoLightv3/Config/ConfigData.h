#pragma once
#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H

#include "Arduino.h"
#include "../Common/Constants.h"

namespace AutoLight {
    using namespace Constants;

    typedef enum {
        CUSTOM_CONFIG = 0,
        DIO_CONFIG = 1,
        I2C_CONFIG = 2,
    } config_index_t;

    typedef enum {
        DISTRIBUTE_BALANCED = 0,
        DISTRIBUTE_SEQUENTIAL = 1,
        DISTRIBUTE_OPTIMIZED = 2,
        DISTRIBUTE_CUSTOM = 3
    } distribution_strategy_t;

    typedef enum {
        BUTTON_MODE_4BUTTON = 0,
        BUTTON_MODE_1BUTTON = 1,
        BUTTON_MODE_2BUTTON = 2,
        BUTTON_MODE_3BUTTON = 3,
        BUTTON_MODE_CUSTOM = 99
    } button_mode_t;

    typedef enum {
        READ_ONLY = 0,
        WRITE_ONLY = 1,
        READ_WRITE = 2,
        OVERRIDE = 3
    } CredentialMode;

    struct PCFDistribution {
        uint8_t address;
        uint8_t used_pins;
        uint8_t start_channel;
        uint8_t pin_offset;
        bool is_active;
    };

    struct DynamicConfigData {
        PCFDistribution pcf_list[Constants::MAXNUM_IO_EXPANDER];
        uint8_t pcf_count;
        uint8_t total_channels;
        distribution_strategy_t strategy;
    };

    struct ButtonConfig {
        uint8_t button_count;
        button_mode_t mode;
        bool auto_cycle_enabled;
        uint32_t auto_cycle_timeout;
        void (*custom_handlers[4])();
    };

    struct SequenceMapper {
        uint8_t active_sequences[Constants::MAXNUM_TOTAL_TASK_SEQUENCE];
        uint8_t total_active;
        bool is_mapping_enabled;

        SequenceMapper() : total_active(0), is_mapping_enabled(false) {
            for (int i = 0; i < Constants::MAXNUM_TOTAL_TASK_SEQUENCE; i++) {
                active_sequences[i] = 0;
            }
        }
    };

    typedef struct {
        uint8_t version_;
        uint8_t channel_;
        uint8_t io_size_;
        uint8_t pin_size_;
        uint8_t *pin_ptr_;
        uint8_t *i2c_ptr_;
        uint8_t *pin_sequence_;
    } ConfigHeader;

    typedef struct {
        uint8_t address_pin_[Constants::MAXNUM_CHANNEL_PIN];
        uint8_t i2c_address_[Constants::MAXNUM_IO_EXPANDER];
        uint8_t version_[Constants::MAXNUM_VERSION];
        uint8_t channel_[Constants::MAXNUM_VERSION];
    } ConfigTable;

    typedef struct {
        ConfigHeader header;
        ConfigTable table;
    } ConfigData;

    struct Credentials {
        String ssid;
        String password;
        String serial;
    };

    class ConfigI2CMaxData {
    public:
        static const int VERSION = Constants::MAXNUM_VERSION;
        static const int PIN_ADDRESS = Constants::MAXNUM_CHANNEL_PIN;
        static const int I2C_ADDRESS = Constants::MAXNUM_IO_EXPANDER;
    };
}

#endif
