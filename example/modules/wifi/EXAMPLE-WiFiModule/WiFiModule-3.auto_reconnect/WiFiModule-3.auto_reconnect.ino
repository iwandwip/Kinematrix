#define ENABLE_MODULE_WIFI_MODULE
#include "Kinematrix.h"

WiFiModule wifi;

unsigned long lastConnectionTime = 0;
const unsigned long connectionInterval = 10000; // 10 detik

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("WiFiModule Auto Reconnect Example");

    // Inisialisasi WiFi
    wifi.begin();

    // Konfigurasi auto reconnect
    wifi.setAutoReconnect(true);
    wifi.setReconnectInterval(5000); // Mencoba reconnect setiap 5 detik

    // Koneksi ke jaringan WiFi
    Serial.println("Connecting to WiFi...");
    if (wifi.connect("your-ssid", "your-password")) {
        Serial.println("Connected successfully!");
    } else {
        Serial.println("Connection failed! Auto reconnect will try again.");
    }
}

void loop() {
    // Periksa koneksi - auto reconnect dihandle oleh WiFiModule
    bool isConnected = wifi.checkConnection();

    unsigned long currentTime = millis();

    // Cetak status setiap 10 detik
    if (currentTime - lastConnectionTime > connectionInterval) {
        lastConnectionTime = currentTime;

        if (isConnected) {
            Serial.println("WiFi is connected.");
            Serial.print("IP address: ");
            Serial.println(wifi.getIP().toString());
        } else {
            Serial.print("WiFi is disconnected. Status: ");
            Serial.println(wifi.getStatusString());

            if (wifi.getStatus() == WIFI_MODULE_CONNECTING) {
                Serial.println("Reconnection in progress...");
            }
        }
    }

    // Simulasi aplikasi Anda
    delay(100);
}
