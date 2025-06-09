#include "WiFiModule.h"
#include <WiFiMulti.h>

// Pendekatan 1: Menggunakan WiFiModule saja
WiFiModule wifiModule;

// Pendekatan 2: Menggunakan WiFiModule dengan WiFiMulti
WiFiMulti wifiMulti;
bool usingMulti = false;

// Jaringan backup
const char* ssid1 = "primary-ssid";
const char* password1 = "primary-password";
const char* ssid2 = "backup-ssid";
const char* password2 = "backup-password";
const char* ssid3 = "mobile-hotspot";
const char* password3 = "mobile-password";

void setupWiFiModule() {
  Serial.println("Setting up WiFiModule approach");
  
  wifiModule.begin();
  wifiModule.setAutoReconnect(true);
  wifiModule.setReconnectInterval(15000);
  
  Serial.println("Connecting to primary WiFi...");
  if (wifiModule.connect(ssid1, password1)) {
    Serial.println("Connected to primary network!");
  } else {
    Serial.println("Failed to connect to primary, trying backup...");
    
    if (wifiModule.connect(ssid2, password2)) {
      Serial.println("Connected to backup network!");
    } else {
      Serial.println("Failed to connect to backup, trying mobile hotspot...");
      
      if (wifiModule.connect(ssid3, password3)) {
        Serial.println("Connected to mobile hotspot!");
      } else {
        Serial.println("All connection attempts failed!");
      }
    }
  }
}

void setupWiFiMulti() {
  Serial.println("Setting up WiFiMulti approach");
  
  // Tambahkan semua jaringan tersedia
  wifiMulti.addAP(ssid1, password1);
  wifiMulti.addAP(ssid2, password2);
  wifiMulti.addAP(ssid3, password3);
  
  Serial.println("Attempting to connect to best available network...");
  
  // Coba koneksi dengan timeout
  for (int i = 0; i < 10; i++) {
    if (wifiMulti.run() == WL_CONNECTED) {
      Serial.print("Connected to: ");
      Serial.println(WiFi.SSID());
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    Serial.print(".");
    delay(1000);
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to any network with WiFiMulti!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("WiFiModule Multiple Networks Example");
  
  // Pilih salah satu pendekatan
  usingMulti = true; // false untuk WiFiModule, true untuk WiFiMulti
  
  if (usingMulti) {
    setupWiFiMulti();
  } else {
    setupWiFiModule();
  }
}

void loop() {
  if (usingMulti) {
    // Dengan WiFiMulti, kita perlu memanggil run() untuk memeriksa koneksi
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("WiFi connection lost, attempting to reconnect...");
      delay(5000);
    } else {
      // Koneksi aktif, lakukan sesuatu
      Serial.print(".");
      delay(5000);
    }
  } else {
    // Dengan WiFiModule, checkConnection() menangani semua yang dibutuhkan
    if (wifiModule.checkConnection()) {
      // Koneksi aktif, lakukan sesuatu
      Serial.print(".");
    } else {
      Serial.println("WiFi disconnected, status: " + String(wifiModule.getStatusString()));
    }
    delay(5000);
  }
}
