#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "Arduino.h"
#include "ConfigData.h"
#include <Preferences.h>

namespace AutoLight {

    class ConfigManager {
    private:
        Credentials current_creds_;
        void (*on_change_callback_)();
        Preferences prefs_;
        String generateDefaultSSID();

    public:
        ConfigManager();

        void updateCredentials(String ssid, String password);
        void updateSerial(String serial);
        void updateWiFiMode(autolight_wifi_mode_t mode);
        Credentials getCredentials();
        autolight_wifi_mode_t getWiFiMode();

        void setChangeCallback(void (*callback)());

        bool loadFromPreferences();
        void saveToPreferences();
        void initializeDefaults();

        static ConfigManager &getInstance();
    };

}

#endif