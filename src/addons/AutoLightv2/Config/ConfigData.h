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

    typedef struct {
        uint8_t version_;           // version of the board
        uint8_t channel_;           // total channel
        uint8_t io_size_;           // number of io expander size
        uint8_t pin_size_;          // number of pin size
        uint8_t *pin_ptr_;          // array of all pin
        uint8_t *i2c_ptr_;          // array of i2c address
        uint8_t *pin_sequence_;     // array of sequence
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

    class ConfigI2CMaxData {
    public:
        static const int VERSION = 32;
        static const int PIN_ADDRESS = 5;
        static const int I2C_ADDRESS = 8;
    };
}

#endif // CONFIG_DATA_H