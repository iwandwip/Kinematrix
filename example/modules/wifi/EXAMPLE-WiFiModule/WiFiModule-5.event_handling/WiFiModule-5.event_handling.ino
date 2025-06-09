#define ENABLE_MODULE_WIFI_MODULE
#include "Kinematrix.h"

WiFiModule wifi;

// Fungsi callback untuk event WiFi
void onWifiEvent(wifi_module_status_t status) {
    Serial.print("WiFi event triggered: ");

    switch (status) {
        case WIFI_MODULE_IDLE:
            Serial.println("IDLE - WiFi initialized but not connected");
            break;

        case WIFI_MODULE_CONNECTING:
            Serial.println("CONNECTING - Attempting to connect to network");
            break;

        case WIFI_MODULE_CONNECTED:
            Serial.println("CONNECTED - Successfully connected to network");
            Serial.print("  IP Address: ");
            Serial.println(wifi.getIP().toString());
            Serial.print("  Network: ");
            Serial.println(wifi.getSSID());
            break;

        case WIFI_MODULE_DISCONNECTED:
            Serial.println("DISCONNECTED - Manually disconnected from network");
            break;

        case WIFI_MODULE_CONNECTION_LOST:
            Serial.println("CONNECTION_LOST - Connection was lost");
            break;

        case WIFI_MODULE_CONNECTION_FAILED:
            Serial.println("CONNECTION_FAILED - Could not connect to network");
            break;

        default:
            Serial.println("UNKNOWN");
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("WiFiModule Event Handling Example");

    // Inisialisasi WiFi
    wifi.begin();

    // Register event callback
    wifi.onEvent(onWifiEvent);

    // Koneksi ke jaringan WiFi
    Serial.println("Connecting to WiFi...");
    wifi.connect("your-ssid", "your-password");
}

void loop() {
    // Periksa koneksi WiFi - ini akan memicu onWifiEvent jika status berubah
    wifi.checkConnection();

    // Simulasi disconnect dan reconnect
    static unsigned long lastToggle = 0;
    static bool shouldDisconnect = false;

    if (millis() - lastToggle > 30000) { // Setiap 30 detik
        lastToggle = millis();

        if (shouldDisconnect) {
            Serial.println("Manually triggering disconnect...");
            wifi.disconnect();
        } else {
            Serial.println("Manually triggering reconnect...");
            wifi.reconnect();
        }

        shouldDisconnect = !shouldDisconnect;
    }

    delay(1000);
}
