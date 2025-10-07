#define ENABLE_MODULE_WIFI_MODULE
#include "Kinematrix.h"

WiFiModule wifi;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("WiFiModule Status Monitoring Example");

    // Inisialisasi WiFi
    wifi.begin();

    // Koneksi ke jaringan WiFi dengan timeout kustom (20 detik)
    Serial.println("Connecting to WiFi...");
    if (wifi.connectWithTimeout("your-ssid", "your-password", 20000)) {
        Serial.println("Connected successfully!");
    } else {
        Serial.println("Connection failed or timeout!");
    }
}

void loop() {
    // Periksa koneksi dan dapatkan status
    wifi.checkConnection();
    wifi_module_status_t status = wifi.getStatus();

    // Tampilkan informasi status
    Serial.print("WiFi Status: ");
    Serial.println(wifi.getStatusString());

    // Tampilkan informasi jaringan jika terhubung
    if (status == WIFI_MODULE_CONNECTED) {
        Serial.print("Connected to: ");
        Serial.println(wifi.getSSID());
        Serial.print("IP address: ");
        Serial.println(wifi.getIP().toString());
        Serial.print("Signal strength (RSSI): ");
        Serial.print(wifi.getRSSI());
        Serial.println(" dBm");
    }

    delay(5000);
}
