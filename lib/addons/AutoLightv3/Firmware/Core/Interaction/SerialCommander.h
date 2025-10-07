#ifndef SERIAL_COMMANDER_H
#define SERIAL_COMMANDER_H

#include "../Channel/BaseChannel/BaseChannel.h"
#include "../Config/BaseConfig.h"
#include "../Config/ConfigManager.h"
#include "ButtonInterrupt.h"
#include <Preferences.h>
#include <WiFi.h>
#include <ESPmDNS.h>

namespace AutoLight {

    typedef enum {
        SERIAL_DISABLED = 0,
        SERIAL_READ_ONLY = 1,
        SERIAL_SAFE_CONTROL = 2,
        SERIAL_FULL_ACCESS = 3
    } SerialMode;

    class SerialCommander {
    private:
        BaseChannel *led_;
        BaseConfig *config_;
        ButtonInterrupt *button_;
        SerialMode current_mode_;
        uint32_t activation_timeout_;
        static const uint32_t TIMEOUT_MS = 300000;

        struct WiFiAPConfig {
            char ssid[32];
            char password[64];
            bool is_loaded_from_preferences;
        } ap_config_;

        void loadWiFiPreferences();
        void startAccessPoint();
        void startStationMode();
        void connectToWiFi();
        void parseQuotedArgs(String input, String &arg1, String &arg2);
        void testChannel(uint8_t channel);
        void showChannelStatus();
        void showWiFiInfo();
        void showConnectedClients();
        void restartAP();
        void resetToDefaults();
        uint8_t parseHexAddress(String args);
        void scanI2CBus();
        void testSinglePCF(uint8_t address);
        void showAllPCFStatus();
        void resetPCF(uint8_t address);
        bool writeToI2C(uint8_t address, uint8_t data);
        uint8_t readFromI2C(uint8_t address);

    public:
        SerialCommander();
        void init(BaseChannel *led, BaseConfig *config, ButtonInterrupt *button = nullptr);
        void initWiFi();
        void process();

        void wifi(String args);
        void led_control(String args);
        void sequence(String args);
        void pcf_manage(String args);
        void button(String args);
        void serial_manage(String args);
        void help(String args);

        void setMode(SerialMode mode);
        SerialMode getMode() const;
        bool isCommandAllowed(const String &command_type);
        void activate(String args);
        void deactivate(String args);
        void checkTimeout();
    };

}

#endif