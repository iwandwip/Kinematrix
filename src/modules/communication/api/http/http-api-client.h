#pragma once
#ifndef HTTP_API_CLIENT_H
#define HTTP_API_CLIENT_H

#if defined(ESP32) || defined(ESP8266)
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "response.h"

class HttpClient {
public:
    HttpClient();
    bool begin(const char* ssid, const char* password);
    void setAuthorization(const char* authToken);
    void addParam(const char* key, const char* value);
    void clearParams();
    void addFormData(const char* key, const char* value);
    void clearFormData();
    bool isReady();
    void get(const char* url, void (*responseCallback)(Response) = nullptr);
    void post(const char* url, void (*responseCallback)(Response) = nullptr);
    void put(const char* url, void (*responseCallback)(Response) = nullptr);
    void deleteReq(const char* url, void (*responseCallback)(Response) = nullptr);

private:
    WiFiClient wifiClient;
    HTTPClient httpClient;
    String authorizationToken;
    String urlParams;
    String formData;
    bool requestReady;
    SemaphoreHandle_t xSemaphore;

    static void httpGetTask(void* pvParameters);
    static void httpPostTask(void* pvParameters);
    static void httpPutTask(void* pvParameters);
    static void httpDeleteTask(void* pvParameters);

    // Helper method to collect headers
    String collectHeaders();
};
#endif
#endif
