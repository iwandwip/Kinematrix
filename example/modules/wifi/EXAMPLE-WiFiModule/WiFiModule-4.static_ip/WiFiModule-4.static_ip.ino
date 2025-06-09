#define ENABLE_MODULE_WIFI_MODULE
#include "Kinematrix.h"

WiFiModule wifi;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("WiFiModule Static IP Example");

    // Inisialisasi WiFi
    wifi.begin();

    // Konfigurasi IP statis
    IPAddress local_ip(192, 168, 1, 200);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns1(8, 8, 8, 8);      // Google DNS
    IPAddress dns2(8, 8, 4, 4);      // Google DNS alternatif

    wifi.setStaticIP(local_ip, gateway, subnet, dns1, dns2);
    wifi.useStaticIP(true);

    // Koneksi ke jaringan WiFi
    Serial.println("Connecting to WiFi with static IP...");
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
        // Kode Anda ketika WiFi terhubung dengan IP statis
    } else {
        // Kode Anda ketika WiFi tidak terhubung
        Serial.println("Connection lost. Status: " + String(wifi.getStatusString()));
    }

    delay(5000);
}
