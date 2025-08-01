#include "ConfigManager.h"
#include <WiFi.h>

namespace AutoLight {

ConfigManager::ConfigManager() : on_change_callback_(nullptr) {}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

String ConfigManager::generateDefaultSSID() {
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    return "AutoLightv3-" + mac.substring(6);
}

void ConfigManager::updateCredentials(String ssid, String password) {
    current_creds_.ssid = ssid;
    current_creds_.password = password;
    
    saveToPreferences();
    
    if (on_change_callback_) {
        on_change_callback_();
    }
}

void ConfigManager::updateSerial(String serial) {
    current_creds_.serial = serial;
    saveToPreferences();
}

Credentials ConfigManager::getCredentials() {
    return current_creds_;
}

void ConfigManager::setChangeCallback(void (*callback)()) {
    on_change_callback_ = callback;
}

bool ConfigManager::loadFromPreferences() {
    prefs_.begin("autolight", false);
    
    String ssid = prefs_.getString("wifi_ssid", "");
    String password = prefs_.getString("wifi_pass", "");
    String serial = prefs_.getString("serial", "");
    
    prefs_.end();
    
    if (ssid.length() > 0) {
        current_creds_.ssid = ssid;
        current_creds_.password = password;
        current_creds_.serial = serial;
        return true;
    }
    
    return false;
}

void ConfigManager::initializeDefaults() {
    String default_ssid = generateDefaultSSID();
    current_creds_.ssid = default_ssid;
    current_creds_.password = "";
    current_creds_.serial = default_ssid;
    
    saveToPreferences();
}

void ConfigManager::saveToPreferences() {
    prefs_.begin("autolight", false);
    
    prefs_.putString("wifi_ssid", current_creds_.ssid);
    prefs_.putString("wifi_pass", current_creds_.password);
    prefs_.putString("serial", current_creds_.serial);
    
    prefs_.end();
}

}