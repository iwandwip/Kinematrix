/*
 *  ConfigData.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H

#include "Arduino.h"

namespace AutoLight {

    const int MAXNUM_CHANNEL = 64;
    const int MAXNUM_IO_EXPANDER = 8;
    const int MAXNUM_VERSION = 32;
    const int MAXNUM_CHANNEL_PIN = 5;

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

    struct PCFDistribution {
        uint8_t address;
        uint8_t used_pins;
        uint8_t start_channel;
        uint8_t pin_offset;
        bool is_active;
    };

    struct DynamicConfigData {
        PCFDistribution pcf_list[MAXNUM_IO_EXPANDER];
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

    typedef struct {
        uint8_t version_;           // version of the board
        uint8_t channel_;           // total channel
        uint8_t io_size_;           // number of io expander sizes
        uint8_t pin_size_;          // number of pin sizes
        uint8_t *pin_ptr_;          // array of all pins
        uint8_t *i2c_ptr_;          // array of i2c address
        uint8_t *pin_sequence_;     // array of a sequence
    } ConfigHeader;

    typedef struct {
        uint8_t address_pin_[MAXNUM_CHANNEL_PIN];
        uint8_t i2c_address_[MAXNUM_IO_EXPANDER];
        uint8_t version_[MAXNUM_VERSION];
        uint8_t channel_[MAXNUM_VERSION];
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
        static const int VERSION = 32;
        static const int PIN_ADDRESS = 5;
        static const int I2C_ADDRESS = 8;
    };
}

#endif // CONFIG_DATA_H