#pragma once
#ifndef API_SERVER_MANAGER_H
#define API_SERVER_MANAGER_H

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
#include "../Core/Config/ConfigData.h"
#include "../Core/Config/ConfigManager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace AutoLight {

    class BaseChannel;

    class BaseConfig;

    class APIServerManager {
    public:
        APIServerManager(BaseChannel *led, BaseConfig *config);
        virtual ~APIServerManager();

        void enableWebServer(bool auto_task = true);
        void enableWebServerManual();

        static void webServerTaskWrapper(void *param);
        void webServerTask();
        void runWebServer();

        void initWiFi();
        void initSDCard();
        void initCredentials();
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


        void initializeServers();
        void setDefaultHeaders();
    };
}


#endif

#endif
