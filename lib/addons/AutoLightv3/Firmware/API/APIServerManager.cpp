#include "APIServerManager.h"
#include "../Core/Config/ConfigData.h"
#include "../Core/Channel/BaseChannel/BaseChannel.h"
#include "../Core/Config/BaseConfig.h"

#if defined(ESP32)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

namespace AutoLight {

    APIServerManager::APIServerManager(BaseChannel *led, BaseConfig *config)
            : led_(led), config_(config), owns_config_(false), server_(80), api_(8000),
              auto_task_(true), is_running_(false), web_task_handle_(NULL) {
    }

    APIServerManager::~APIServerManager() {
        if (web_task_handle_ != NULL) {
            vTaskDelete(web_task_handle_);
        }

        if (owns_config_ && config_ != nullptr) {
            delete config_;
            config_ = nullptr;
        }
    }

    void APIServerManager::enableWebServer(bool auto_task) {
        auto creds = ConfigManager::getInstance().getCredentials();
        device_name_ = creds.ssid;
        auto_task_ = auto_task;

        if (auto_task) {
            xTaskCreatePinnedToCore(
                    webServerTaskWrapper,
                    "autolight_web",
                    10000,
                    this,
                    1,
                    &web_task_handle_,
                    0
            );
        } else {
            initializeServers();
        }
    }

    void APIServerManager::enableWebServerManual() {
        enableWebServer(false);
    }

    void APIServerManager::webServerTaskWrapper(void *param) {
        APIServerManager *self = (APIServerManager *) param;
        self->webServerTask();
    }

    void APIServerManager::webServerTask() {
        initializeServers();

        for (;;) {
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

    void APIServerManager::runWebServer() {
        if (!is_running_) {
            initializeServers();
        }
    }


    void APIServerManager::initializeServers() {
        initCredentials();
        initSDCard();
        initWiFi();
        setupRoutes();
        setDefaultHeaders();

        // Only serve static files from SD card if SD card is available
        uint8_t cardType = SD.cardType();
        if (cardType != CARD_NONE) {
            server_.serveStatic("/", SD, "/");
        }
        server_.begin();
        api_.begin();

        is_running_ = true;

        Serial.println("==============================");
        Serial.println("| WEB SERVER STARTED");
        Serial.println("==============================");
        Serial.print("| Device Name: ");
        Serial.println(device_name_);
        
        // Print correct IP based on WiFi mode
        autolight_wifi_mode_t wifi_mode = ConfigManager::getInstance().getWiFiMode();
        String ip_address = (wifi_mode == AUTOLIGHT_WIFI_MODE_STA) ? 
                            WiFi.localIP().toString() : 
                            WiFi.softAPIP().toString();
                            
        Serial.print("| Web Server: http://");
        Serial.print(ip_address);
        Serial.print(" or http://als.local");
        Serial.println();
        Serial.print("| API Server: http://");
        Serial.print(ip_address);
        Serial.println(":8000");
        Serial.println("==============================");
    }

    void APIServerManager::initCredentials() {
        auto creds = ConfigManager::getInstance().getCredentials();
        credentials_ = creds;
    }

    void APIServerManager::initWiFi() {
        // Check WiFi mode from ConfigManager
        autolight_wifi_mode_t wifi_mode = ConfigManager::getInstance().getWiFiMode();
        
        if (wifi_mode == AUTOLIGHT_WIFI_MODE_STA) {
            Serial.printf("🔗 Connecting to WiFi: '%s'...\n", credentials_.ssid.c_str());
            
            if (credentials_.password.length() == 0) {
                WiFi.begin(credentials_.ssid);
            } else {
                WiFi.begin(credentials_.ssid, credentials_.password);
            }
            
            uint32_t timeout = millis();
            const uint32_t STA_TIMEOUT_MS = 30000;
            
            while (WiFi.status() != WL_CONNECTED && (millis() - timeout) < STA_TIMEOUT_MS) {
                delay(500);
                Serial.print(".");
            }
            Serial.println();
            
            if (WiFi.status() == WL_CONNECTED) {
                IPAddress ip = WiFi.localIP();
                Serial.printf("✓ STA Connected: '%s'\n", credentials_.ssid.c_str());
                Serial.printf("  IP: %s\n", ip.toString().c_str());
                Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI());
                Serial.println("  Web Interface: http://" + ip.toString());
                Serial.println("  mDNS: http://als.local");
            } else {
                Serial.println("❌ STA Connection failed - Falling back to AP mode");
                ConfigManager::getInstance().updateWiFiMode(AUTOLIGHT_WIFI_MODE_AP);
                // Switch to AP mode
                WiFi.mode(WIFI_OFF);
                delay(200);
                WiFi.mode(WIFI_AP);
                bool success;
                if (credentials_.password.length() == 0) {
                    success = WiFi.softAP(credentials_.ssid);
                } else {
                    success = WiFi.softAP(credentials_.ssid, credentials_.password);
                }
                
                if (success) {
                    IPAddress ip = WiFi.softAPIP();
                    Serial.printf("✓ AP Started: '%s'\n", credentials_.ssid.c_str());
                    Serial.printf("  IP: %s\n", ip.toString().c_str());
                    Serial.printf("  Security: %s\n", credentials_.password.length() == 0 ? "Open" : "WPA2");
                    Serial.println("  Web Interface: http://" + ip.toString());
                    Serial.println("  mDNS: http://als.local");
                } else {
                    Serial.println("❌ Failed to start AP");
                }
            }
        } else {
            WiFi.mode(WIFI_AP);
            bool success;
            if (credentials_.password.length() == 0) {
                success = WiFi.softAP(credentials_.ssid);
            } else {
                success = WiFi.softAP(credentials_.ssid, credentials_.password);
            }
            
            if (success) {
                IPAddress ip = WiFi.softAPIP();
                Serial.printf("✓ AP Started: '%s'\n", credentials_.ssid.c_str());
                Serial.printf("  IP: %s\n", ip.toString().c_str());
                Serial.printf("  Security: %s\n", credentials_.password.length() == 0 ? "Open" : "WPA2");
                Serial.println("  Web Interface: http://" + ip.toString());
                Serial.println("  mDNS: http://als.local");
            } else {
                Serial.println("❌ Failed to start AP");
            }
        }

        if (!MDNS.begin("als")) {
            Serial.println("Error starting mDNS");
            return;
        }
        Serial.println("mDNS started: als.local");
    }

    void APIServerManager::initSDCard() {
        Serial.println("| START DEBUG SD CARD");
        if (!SD.begin(5)) {
            Serial.println("| [ERROR] Card Mount Failed");
            // Don't return here, continue with web server initialization
            // SD card is optional for web interface
            return;
        }
        Serial.println("| [SUCCESS] Card Mount Success");
    }

    void APIServerManager::setDefaultHeaders() {
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    }

    void APIServerManager::setupRoutes() {
        server_.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
            // Check if SD card is available before trying to serve files
            uint8_t cardType = SD.cardType();
            if (cardType == CARD_NONE) {
                // SD card not available, send a simple response
                request->send(200, "text/html", 
                    "<html><head><title>AutoLight V3</title></head>"
                    "<body><h1>AutoLight V3 Web Interface</h1>"
                    "<p>SD card not available. Please check your SD card connection.</p>"
                    "<p>You can still control the device through the API endpoints:</p>"
                    "<ul><li>/api/v1/data/get/device/name</li>"
                    "<li>/api/v1/data/get/device/ch</li>"
                    "<li>/api/v1/data/get/mode</li>"
                    "<li>/api/v1/data/get/delay</li>"
                    "<li>/api/v1/data/set/mode?value={mode}</li>"
                    "<li>/api/v1/data/set/delay?value={delay}</li></ul>"
                    "</body></html>");
            } else {
                request->send(SD, "/index.html", "text/html");
            }
        });

        api_.on("/api/v1/data/get/device/name", HTTP_GET, [this](AsyncWebServerRequest *request) {
            auto creds = ConfigManager::getInstance().getCredentials();
            request->send(200, "text/plain", creds.ssid);
        });

        api_.on("/api/v1/data/get/device/ch", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", String(config_->getChannel()));
        });

        api_.on("/api/v1/data/get/device/serial", HTTP_GET, [this](AsyncWebServerRequest *request) {
            auto creds = ConfigManager::getInstance().getCredentials();
            request->send(200, "text/plain", creds.serial);
        });

        api_.on("/api/v1/data/set/device/name", HTTP_GET, [this](AsyncWebServerRequest *request) {
            if (request->hasArg("value")) {
                String deviceName = request->arg("value");

                ConfigManager::getInstance().updateCredentials(deviceName, "");

                auto creds = ConfigManager::getInstance().getCredentials();
                credentials_ = creds;

                request->send(200, "text/plain", "Berhasil Set device : " + creds.ssid);
            } else {
                request->send(400, "text/plain", "Parameter 'value' is missing");
            }
        });

        api_.on("/api/v1/data/get/mode", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", String(led_->getSequenceIndex()));
        });

        api_.on("/api/v1/data/set/mode", HTTP_GET, [this](AsyncWebServerRequest *request) {
            if (request->hasArg("value")) {
                String modeValue = request->arg("value");
                int modeValueInt = modeValue.toInt();
                Serial.println("API Set Mode: " + String(modeValueInt));
                led_->changeModeApp(modeValueInt);
                request->send(200, "text/plain", "Berhasil Set Mode : " + modeValue);
            } else {
                request->send(400, "text/plain", "Parameter 'value' is missing");
            }
        });

        api_.on("/api/v1/data/get/delay", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", String(led_->getDelayTime()));
        });

        api_.on("/api/v1/data/set/delay", HTTP_GET, [this](AsyncWebServerRequest *request) {
            if (request->hasArg("value")) {
                String delayValue = request->arg("value");
                led_->setInitDelay(delayValue.toInt());
                request->send(200, "text/plain", "Berhasil Set delay : " + delayValue);
            } else {
                request->send(400, "text/plain", "Parameter 'value' is missing");
            }
        });
    }
}