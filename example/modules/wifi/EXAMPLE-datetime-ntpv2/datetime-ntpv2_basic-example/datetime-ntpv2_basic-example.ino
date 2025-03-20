/*
 * BasicTimeExample
 * 
 * Contoh dasar penggunaan DateTimeNTPV2 untuk mendapatkan
 * waktu dari server NTP dan menampilkannya di Serial Monitor.
 */

#define ENABLE_MODULE_DATETIME_NTP_V2
#include "Kinematrix.h"

// Konfigurasi WiFi
const char* ssid = "NamaWiFiAnda";
const char* password = "PasswordWiFiAnda";

// Konfigurasi NTP - GMT+7 untuk WIB (Jakarta)
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // GMT+7 dalam detik
const int daylightOffset_sec = 0;     // Tidak menggunakan daylight saving

// Inisialisasi objek DateTimeNTPV2
DateTimeNTPV2 dateTime(ntpServer, gmtOffset_sec, daylightOffset_sec);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Memulai BasicTimeExample...");
  
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

void loop() {
  // Update waktu
  if (dateTime.update()) {
    // Dapatkan dan tampilkan waktu saat ini
    Serial.println("\n===== WAKTU SAAT INI =====");
    Serial.print("Tanggal: ");
    Serial.println(dateTime.getDateString());
    
    Serial.print("Waktu: ");
    Serial.println(dateTime.getTimeString());
    
    Serial.print("Hari: ");
    Serial.println(dateTime.getDayOfWeek());
    
    Serial.print("Tanggal & Waktu: ");
    Serial.println(dateTime.getDateTimeString());
    
    Serial.print("Unix Timestamp: ");
    Serial.println(dateTime.getUnixTime());
  } else {
    Serial.println("Gagal update waktu!");
  }
  
  delay(5000);  // Tampilkan waktu setiap 5 detik
}