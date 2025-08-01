#include "WebManager.h"
#include "../Config/ConfigData.h"
#include "../Channel/BaseChannel.h"
#include "../Config/BaseConfig.h"

#if defined(ESP32)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#endif

namespace AutoLight {

    WebManager::WebManager(BaseChannel *led, BaseConfig *config)
            : led_(led), config_(config), owns_config_(false), server_(80), api_(8000),
              auto_task_(true), is_running_(false), web_task_handle_(NULL) {
    }

    WebManager::~WebManager() {
        if (web_task_handle_ != NULL) {
            vTaskDelete(web_task_handle_);
        }

        if (owns_config_ && config_ != nullptr) {
            delete config_;
            config_ = nullptr;
        }
    }

    void WebManager::enableWebServer(bool auto_task) {
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

    void WebManager::enableWebServerManual() {
        enableWebServer(false);
    }

    void WebManager::webServerTaskWrapper(void *param) {
        WebManager *self = (WebManager *) param;
        self->webServerTask();
    }

    void WebManager::webServerTask() {
        initializeServers();

        for (;;) {
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

    void WebManager::runWebServer() {
        if (!is_running_) {
            initializeServers();
        }
    }


    void WebManager::initializeServers() {
        initCredentials();
        initSDCard();
        initWiFi();
        setupRoutes();
        setDefaultHeaders();

        server_.serveStatic("/", SD, "/");
        server_.begin();
        api_.begin();

        is_running_ = true;

        Serial.println("==============================");
        Serial.println("| WEB SERVER STARTED");
        Serial.println("==============================");
        Serial.print("| Device Name: ");
        Serial.println(device_name_);
        Serial.print("| Web Server: http://");
        Serial.print(WiFi.localIP());
        Serial.print(" or http://als.local");
        Serial.println();
        Serial.print("| API Server: http://");
        Serial.print(WiFi.localIP());
        Serial.println(":8000");
        Serial.println("==============================");
    }

    void WebManager::initCredentials() {
        auto creds = ConfigManager::getInstance().getCredentials();
        credentials_ = creds;
    }

    void WebManager::initWiFi() {
        if (WiFi.begin(credentials_.ssid, credentials_.password) == WL_CONNECTED) {
            Serial.println("Connected to WiFi");
        } else {
            Serial.println("Failed to connect to WiFi, switching to SoftAP mode");
            WiFi.mode(WIFI_OFF);
            delay(200);
            WiFi.softAP(credentials_.ssid, credentials_.password);
            Serial.println("SoftAP activated, IP address: " + WiFi.softAPIP().toString());
        }

        if (!MDNS.begin("als")) {
            Serial.println("Error starting mDNS");
            return;
        }
        Serial.println("mDNS started: als.local");
    }

    void WebManager::initSDCard() {
        Serial.println("| START DEBUG SD CARD");
        if (!SD.begin(5)) {
            Serial.println("| [ERROR] Card Mount Failed");
            return;
        }
        Serial.println("| [SUCCESS] Card Mount Success");
    }

    void WebManager::setDefaultHeaders() {
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    }

    void WebManager::setupRoutes() {
        server_.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(SD, "/index.html", "text/html");
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