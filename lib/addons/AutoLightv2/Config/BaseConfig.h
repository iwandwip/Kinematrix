/*
 *  BaseConfig.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

#include "Arduino.h"
#include "ConfigData.h"
#include "Wire.h"

namespace AutoLight {
    const int MAXNUM_I2C_SLAVE_ADDRESS = 127;

    class I2CScanner {
    public:
        I2CScanner();
        virtual ~I2CScanner();

        void beginTransmission();
        void scanI2CAddress();
        void endTransmission();
    private:
        int n_address_[8];
        int n_devices_;
    };

    class BaseConfig {
    public:
        BaseConfig(bool _enable_unit_test = false);
        virtual ~BaseConfig();

        virtual void initialize();

        virtual int readChannel();
        virtual int readVersion();

        virtual void setVersion(uint8_t _version);
        virtual void setChannel(uint8_t _channel);

        virtual int getChannel();
        virtual int getVersion();
        virtual int getIoSize();

        void setConfigIndex(int _config_index = I2C_CONFIG);
        void setSequence(int _ch_sequence_size, ...);
        void setChannelPin(int _ch_pin_size, ...);
        void setAddress(int _address_size, ...);

        ConfigData *getConfigData();
        BaseConfig *getConfig();
        int getConfigIndex();

        uint8_t *getPinList();

        BaseConfig *operator()();
        BaseConfig *operator=(BaseConfig *_other);

        virtual void debugConfig();

    private:
        int config_index_;
        bool is_custom_sequence_;
        bool is_unit_test_;

        ConfigData config_data_;
        I2CScanner i2c_scanner_;
    };
}


#endif // BASE_CONFIG_H