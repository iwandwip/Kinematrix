#define ENABLE_MODULE_WIFI_MODULE
#include "Kinematrix.h"

// Inisialisasi objek WiFiModule
WiFiModule wifi;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("WiFiModule Basic Example");

    // Inisialisasi WiFi
    wifi.begin();

    // Koneksi ke jaringan WiFi
    Serial.println("Connecting to WiFi...");
    if (wifi.connect("your-ssid", "your-password")) {
        Serial.println("Connected successfully!");
        Serial.print("IP address: ");
        Serial.println(wifi.getIP().toString());
    } else {
        Serial.println("Connection failed!");
    }
}

void loop() {
    // Periksa koneksi dalam loop
    if (wifi.checkConnection()) {
        // Kode Anda ketika WiFi terhubung
        Serial.print(".");
    } else {
        // Kode Anda ketika WiFi tidak terhubung
        Serial.print("x");
    }

    delay(1000);
}
