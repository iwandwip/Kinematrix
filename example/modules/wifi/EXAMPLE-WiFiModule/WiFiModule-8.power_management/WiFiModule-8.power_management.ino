#define ENABLE_MODULE_WIFI_MODULE
#include "Kinematrix.h"

// Definisi perangkat IoT 
#define DEVICE_NAME "ESP32-Sensor-01"
#define MQTT_TOPIC "sensors/temperature"

// Konstanta untuk mode daya 
#define NORMAL_MODE 0
#define POWER_SAVE_MODE 1

WiFiModule wifi;

// Variabel untuk mode daya dan status koneksi
int powerMode = NORMAL_MODE;
bool networkAvailable = false;
unsigned long lastConnectionTime = 0;
unsigned long powerSaveInterval = 300000; // 5 menit dalam mode hemat daya

// Konfigurasi koneksi
const char *primarySSID = "home-network";
const char *primaryPassword = "home-password";
const char *backupSSID = "backup-network";
const char *backupPassword = "backup-password";

// LED untuk indikasi status
const int statusLedPin = 2;

// WiFi event handler
void onWifiEvent(wifi_module_status_t status) {
    switch (status) {
        case WIFI_MODULE_CONNECTED:
            networkAvailable = true;
            digitalWrite(statusLedPin, HIGH);
            Serial.println("WiFi connected!");
            // Pindah ke mode daya normal jika sebelumnya dalam mode hemat daya
            if (powerMode == POWER_SAVE_MODE) {
                powerMode = NORMAL_MODE;
                Serial.println("Returning to normal power mode");
                // Reset interval reconnect ke lebih cepat untuk mode normal
                wifi.setReconnectInterval(10000); // 10 detik
            }
            break;

        case WIFI_MODULE_DISCONNECTED:
        case WIFI_MODULE_CONNECTION_LOST:
            networkAvailable = false;
            digitalWrite(statusLedPin, LOW);
            Serial.println("WiFi connection lost!");
            break;

        case WIFI_MODULE_CONNECTION_FAILED:
            networkAvailable = false;
            digitalWrite(statusLedPin, LOW);

            // Setelah beberapa kali gagal, pindah ke mode hemat daya
            static int failureCount = 0;
            failureCount++;

            if (failureCount > 3) {
                Serial.println("Multiple connection failures. Switching to power save mode");
                powerMode = POWER_SAVE_MODE;
                // Interval reconnect lebih lama untuk hemat daya
                wifi.setReconnectInterval(powerSaveInterval);
                failureCount = 0;
            }
            break;

        default:
            break;
    }
}

// Coba jaringan utama, lalu jaringan cadangan jika gagal
bool connectToAvailableNetwork() {
    Serial.println("Attempting connection to primary network...");
    if (wifi.connect(primarySSID, primaryPassword)) {
        Serial.println("Connected to primary network!");
        return true;
    }

    Serial.println("Primary network unavailable, trying backup...");
    if (wifi.connect(backupSSID, backupPassword)) {
        Serial.println("Connected to backup network!");
        return true;
    }

    Serial.println("All networks unavailable!");
    return false;
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("WiFiModule Power Management Example");

    // Setup LED
    pinMode(statusLedPin, OUTPUT);
    digitalWrite(statusLedPin, LOW);

    // Inisialisasi WiFi
    wifi.begin();
    wifi.setAutoReconnect(true);
    wifi.setReconnectInterval(10000); // 10 detik
    wifi.onEvent(onWifiEvent);

    // Coba koneksi
    connectToAvailableNetwork();
}

void loop() {
    // Update status koneksi
    wifi.checkConnection();

    // Algoritma manajemen daya
    if (powerMode == POWER_SAVE_MODE) {
        // Dalam mode hemat daya, kita perlu memeriksa secara berkala
        // untuk melihat apakah jaringan tersedia lagi
        unsigned long currentTime = millis();
        if (currentTime - lastConnectionTime > powerSaveInterval) {
            lastConnectionTime = currentTime;
            Serial.println("Power save mode: checking for network availability");

            // Coba koneksi kembali (pendekatan agresif)
            if (!networkAvailable) {
                connectToAvailableNetwork();
            }
        }

        // Tambahkan sleep untuk hemat daya
        delay(5000); // Cek lebih jarang dalam mode hemat daya
    } else {
        // Dalam mode normal, pemeriksaan lebih sering
        // Anda bisa menambahkan logika untuk transmisi data, dll.

        // Simulasi aktivitas IoT
        if (networkAvailable) {
            // Baca sensor dan kirim data (simulasi)
            static unsigned long lastDataTime = 0;
            unsigned long currentTime = millis();

            if (currentTime - lastDataTime > 30000) { // Kirim data setiap 30 detik
                lastDataTime = currentTime;

                // Simulasi pembacaan sensor
                float temperature = 22.5 + (random(100) / 10.0);

                Serial.print("Sending sensor data: Temperature = ");
                Serial.println(temperature);

                // Di sini Anda dapat menambahkan kode untuk mengirim data melalui MQTT, HTTP, dll.
            }
        }

        delay(1000); // Cek lebih sering dalam mode normal
    }
}
