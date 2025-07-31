/*
 *  WebManager.h
 *
 *  AutoLight V3 - Integrated Web Server Manager
 *  Kastara Electronics Embedded Development
 *  Created on: 2024. 7. 30
 */

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
#include "../Config/ConfigData.h"  // For Credentials struct
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace AutoLight {
    
    class BaseChannel; // Forward declaration
    class BaseConfig;  // Forward declaration


    class WebManager {
    public:
        WebManager(BaseChannel* led, BaseConfig* config);
        virtual ~WebManager();

        // Hybrid task management
        void enableWebServer(const char* device_name, bool auto_task = true);
        void enableWebServerManual(const char* device_name);
        
        // Task functions
        static void webServerTaskWrapper(void* param);
        void webServerTask();
        void runWebServer(); // Non-blocking for manual task mode

        // Utilities
        void initWiFi();
        void initSDCard();
        void initCredentials();
        void setupRoutes();

        // Getters
        bool isRunning() const { return is_running_; }
        String getDeviceName() const { return device_name_; }
        
        // Setters
        void setConfigOwnership(bool owns) { owns_config_ = owns; }

    private:
        BaseChannel* led_;
        BaseConfig* config_;
        bool owns_config_;  // Track if we need to delete config
        
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

#endif // WEB_MANAGER_H