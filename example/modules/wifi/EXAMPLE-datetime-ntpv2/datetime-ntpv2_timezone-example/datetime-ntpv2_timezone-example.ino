/*
 * TimeZonesExample
 * 
 * Contoh penggunaan DateTimeNTPV2 untuk menampilkan waktu
 * dalam berbagai zona waktu.
 */

#define ENABLE_MODULE_DATETIME_NTP_V2
#include "Kinematrix.h"

// Konfigurasi WiFi
const char* ssid = "NamaWiFiAnda";
const char* password = "PasswordWiFiAnda";

// Definisi zona waktu (dalam detik)
const long UTC_OFFSET = 0;
const long WIB_OFFSET = 7 * 3600;    // GMT+7 Indonesia Barat
const long WITA_OFFSET = 8 * 3600;   // GMT+8 Indonesia Tengah
const long WIT_OFFSET = 9 * 3600;    // GMT+9 Indonesia Timur
const long JST_OFFSET = 9 * 3600;    // GMT+9 Japan
const long AEST_OFFSET = 10 * 3600;  // GMT+10 Australia Eastern
const long PST_OFFSET = -8 * 3600;   // GMT-8 Pacific Standard Time
const long EST_OFFSET = -5 * 3600;   // GMT-5 Eastern Standard Time

// Variabel untuk menyimpan zona waktu default
long currentOffset = UTC_OFFSET;
int currentDaylightOffset = 0;

// Inisialisasi objek DateTimeNTPV2 dengan UTC default
DateTimeNTPV2 dateTime("pool.ntp.org", UTC_OFFSET, 0);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Memulai TimeZonesExample...");

    // Menghubungkan ke WiFi
    Serial.printf("Menghubungkan ke %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" CONNECTED!");

    // Inisialisasi NTP
    if (dateTime.begin()) {
        Serial.println("NTP Client dimulai!");
    } else {
        Serial.println("Gagal memulai NTP Client!");
    }
}

void displayTimeInZone(const String& zoneName, long zoneOffset) {
    // Simpan zona waktu saat ini
    dateTime.setTimezone(zoneOffset, 0);

    // Tampilkan waktu di zona waktu baru
    Serial.print(zoneName);
    Serial.print(": ");
    Serial.println(dateTime.getDateTimeString());

    // Kembalikan ke zona waktu default
    dateTime.setTimezone(currentOffset, currentDaylightOffset);
}

void loop() {
    // Update waktu
    if (dateTime.update()) {
        Serial.println("\n===== WAKTU DI BERBAGAI ZONA =====");

        // Tampilkan waktu di berbagai zona waktu
        displayTimeInZone("UTC/GMT", UTC_OFFSET);
        displayTimeInZone("WIB (Jakarta)", WIB_OFFSET);
        displayTimeInZone("WITA (Makassar)", WITA_OFFSET);
        displayTimeInZone("WIT (Jayapura)", WIT_OFFSET);
        displayTimeInZone("JST (Tokyo)", JST_OFFSET);
        displayTimeInZone("AEST (Sydney)", AEST_OFFSET);
        displayTimeInZone("PST (Los Angeles)", PST_OFFSET);
        displayTimeInZone("EST (New York)", EST_OFFSET);

        // Tampilkan dengan format yang lebih lengkap
        Serial.println("\nFormat RFC3339 di berbagai zona waktu:");

        // Atur dan tampilkan waktu di berbagai zona
        dateTime.setTimezone(UTC_OFFSET, 0);
        Serial.print("UTC: ");
        Serial.println(dateTime.getRFC3339Time());

        dateTime.setTimezone(WIB_OFFSET, 0);
        Serial.print("WIB: ");
        Serial.println(dateTime.getRFC3339Time());

        dateTime.setTimezone(EST_OFFSET, 0);
        Serial.print("EST: ");
        Serial.println(dateTime.getRFC3339Time());

        // Kembalikan ke zona waktu default
        dateTime.setTimezone(currentOffset, currentDaylightOffset);
    } else {
        Serial.println("Gagal update waktu!");
    }

    delay(10000);  // Tampilkan waktu setiap 10 detik
}