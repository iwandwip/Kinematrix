#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "ESPmDNS.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Preferences.h"

Preferences preferences;

AsyncWebServer server(80);
AsyncWebServer api(8000);

// ALS024021, ALS-09:18-2024/06/16, ALS20240902-001, TEST-1-BUTTON, ALS024030
const String SERIAL_NUMBER = "TEST-1-BUTTON";  

void initCredentials() {
  auto writeCredentials = []() -> void {
    Serial.println("==============================");
    Serial.println("| [WRITE] credentials");
    preferences.begin("credentials", false);
    preferences.putString("serial", SERIAL_NUMBER);
    preferences.putString("ssid", "SSID-" + SERIAL_NUMBER);
    preferences.putString("password", "");
    preferences.end();
  };
  auto readCredentials = []() -> void {
    Serial.println("==============================");
    Serial.println("| [READ] credentials");
    Serial.println("==============================");

    preferences.begin("credentials", false);
    credentials.serial = preferences.getString("serial", "");
    credentials.ssid = preferences.getString("ssid", "");
    credentials.password = preferences.getString("password", "");
    preferences.end();

    Serial.print("| credentials.ssid             : ");
    Serial.println(credentials.ssid);
    Serial.print("| credentials.password         : ");
    Serial.println(credentials.password);
    Serial.print("| credentials.serial           : ");
    Serial.println(credentials.serial);
  };

  writeCredentials();
  readCredentials();
}

void initServer() {
  initSDCard();
  initWiFi();
  setRoutes();

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "content-type");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  server.serveStatic("/", SD, "/");
  server.begin();
  api.begin();
}

void setRoutes() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SD, "/index.html", "text/html");
  });

  api.on("/api/v1/data/get/device/name", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", credentials.ssid);
  });

  api.on("/api/v1/data/get/device/ch", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", String(config.getChannel()));
  });

  api.on("/api/v1/data/get/device/serial", HTTP_GET, [](AsyncWebServerRequest* request) {
    preferences.begin("credentials", false);
    credentials.serial = preferences.getString("serial", "");
    request->send(200, "text/plain", credentials.serial);
    preferences.end();
  });

  api.on("/api/v1/data/set/device/name", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasArg("value")) {
      String deviceName = request->arg("value");
      // Serial.println("DEVICE NAME REQUEST : " + deviceName);

      credentials.ssid = deviceName;
      credentials.password = "";

      preferences.begin("credentials", false);
      preferences.putString("ssid", credentials.ssid);
      preferences.putString("password", credentials.password);
      preferences.end();

      request->send(200, "text/plain", "Berhasil Set device : " + credentials.ssid);

      WiFi.mode(WIFI_OFF);
      delay(200);
      // Serial.println("SoftAP activated, IP address: " + WiFi.softAPIP().toString());
      WiFi.softAP(credentials.ssid, credentials.password);
    } else {
      request->send(400, "text/plain", "Parameter 'value' is missing");
    }
  });

  api.on("/api/v1/data/get/mode", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", String(led.getSequenceIndex()));
  });

  api.on("/api/v1/data/set/mode", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasArg("value")) {
      String modeValue = request->arg("value");
      int modeValueInt = modeValue.toInt();
      Serial.println(modeValueInt);
      led.changeModeApp(modeValueInt);
      request->send(200, "text/plain", "Berhasil Set Mode : " + modeValue);
    } else {
      request->send(400, "text/plain", "Parameter 'value' is missing");
    }
  });

  api.on("/api/v1/data/get/delay", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", String(led.getDelayTime()));
  });

  api.on("/api/v1/data/set/delay", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (request->hasArg("value")) {
      String delayValue = request->arg("value");
      led.setInitDelay(delayValue.toInt());
      // Serial.println("DELAY REQUEST : " + delayValue);
      request->send(200, "text/plain", "Berhasil Set delay : " + delayValue);
    } else {
      request->send(400, "text/plain", "Parameter 'value' is missing");
    }
  });
}