#pragma once
#ifndef WEB_MANAGER_H
#define WEB_MANAGER_H

#if defined(ESP32)

#include "Arduino.h"
#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "ESPmDNS.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Preferences.h"
#include "../Config/ConfigData.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace AutoLight {

    class BaseChannel;

    class BaseConfig;

    class WebManager {
    public:
        WebManager(BaseChannel *led, BaseConfig *config);
        virtual ~WebManager();

        void enableWebServer(const char *device_name, bool auto_task = true);
        void enableWebServerManual(const char *device_name);

        static void webServerTaskWrapper(void *param);
        void webServerTask();
        void runWebServer();

        void initWiFi();
        void initSDCard();
        void setCredentialConfig(CredentialMode mode, const String& ssid = "", const String& password = "");
        void initCredentials(CredentialMode mode = READ_WRITE, 
                            const String& ssid = "", 
                            const String& password = "");
        void setupRoutes();

        bool isRunning() const { return is_running_; }

        String getDeviceName() const { return device_name_; }

        void setConfigOwnership(bool owns) { owns_config_ = owns; }

    private:
        BaseChannel *led_;
        BaseConfig *config_;
        bool owns_config_;

        AsyncWebServer server_;
        AsyncWebServer api_;
        Preferences preferences_;
        Credentials credentials_;

        String device_name_;
        bool auto_task_;
        bool is_running_;
        TaskHandle_t web_task_handle_;
        
        CredentialMode credential_mode_;
        String credential_ssid_;
        String credential_password_;

        void initializeServers();
        void setDefaultHeaders();
    };
}


#endif

#endif
