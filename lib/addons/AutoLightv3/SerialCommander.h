#ifndef SERIAL_COMMANDER_H
#define SERIAL_COMMANDER_H

#include "Channel/BaseChannel.h"
#include "Config/BaseConfig.h"
#include "Visualization/LEDMatrixVisualizer.h"
#include "ButtonInterrupt.h"
#include <Preferences.h>
#include <WiFi.h>
#include <ESPmDNS.h>

namespace AutoLight {

class SerialCommander {
private:
    BaseChannel* led_;
    BaseConfig* config_;
    LEDMatrixVisualizer* viz_;
    ButtonInterrupt* button_;
    
    struct WiFiAPConfig {
        char ssid[32];
        char password[64];
        bool is_loaded_from_preferences;
    } ap_config_;
    
    bool viz_enabled_;
    uint32_t last_viz_update_;
    uint32_t viz_refresh_rate_;
    
    void loadWiFiPreferences();
    void startAccessPoint();
    void parseQuotedArgs(String input, String& arg1, String& arg2);
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
    void init(BaseChannel* led, BaseConfig* config, LEDMatrixVisualizer* viz = nullptr, ButtonInterrupt* button = nullptr);
    void initWiFiAP();
    void process();
    
    void wifi(String args);
    void led_control(String args);
    void sequence(String args);
    void pcf_manage(String args);
    void visualize(String args);
    void button(String args);
    void help(String args);
};

}

#endif