/*
 * TimeManipulationExample
 * 
 * Contoh manipulasi waktu menggunakan kelas TimeSpan
 * untuk menambah atau mengurangi waktu.
 */

#define ENABLE_MODULE_DATETIME_NTP_V2
#include "Kinematrix.h"

// Konfigurasi WiFi
const char* ssid = "NamaWiFiAnda";
const char* password = "PasswordWiFiAnda";

// Konfigurasi NTP - GMT+7 untuk WIB (Jakarta)
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// Inisialisasi objek DateTimeNTPV2
DateTimeNTPV2 dateTime("pool.ntp.org", gmtOffset_sec, daylightOffset_sec);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Memulai TimeManipulationExample...");
  
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
    // Tampilkan waktu saat ini
    Serial.println("\n===== MANIPULASI WAKTU =====");
    
    char timeStr[25];
    
    // Waktu saat ini
    Serial.print("Waktu saat ini: ");
    dateTime.toString(timeStr);
    Serial.println(timeStr);
    
    // Contoh penggunaan TimeSpan
    // 1. Tambahkan 1 hari
    DateTimeNTPV2 tomorrow = dateTime + TimeSpan(1, 0, 0, 0);
    Serial.print("Besok: ");
    tomorrow.toString(timeStr);
    Serial.println(timeStr);
    
    // 2. Tambahkan 1 jam
    DateTimeNTPV2 nextHour = dateTime + TimeSpan(0, 1, 0, 0);
    Serial.print("1 jam dari sekarang: ");
    nextHour.toString(timeStr);
    Serial.println(timeStr);
    
    // 3. Tambahkan 30 menit
    DateTimeNTPV2 nextHalfHour = dateTime + TimeSpan(0, 0, 30, 0);
    Serial.print("30 menit dari sekarang: ");
    nextHalfHour.toString(timeStr);
    Serial.println(timeStr);
    
    // 4. Tambahkan 1 minggu
    DateTimeNTPV2 nextWeek = dateTime + TimeSpan(7, 0, 0, 0);
    Serial.print("1 minggu dari sekarang: ");
    nextWeek.toString(timeStr);
    Serial.println(timeStr);
    
    // 5. Kurangi 2 hari
    DateTimeNTPV2 twoDaysAgo = dateTime - TimeSpan(2, 0, 0, 0);
    Serial.print("2 hari yang lalu: ");
    twoDaysAgo.toString(timeStr);
    Serial.println(timeStr);
    
    // 6. Hitung selisih antara dua waktu (menggunakan operator -)
    TimeSpan diff = nextWeek - dateTime;
    Serial.print("Selisih waktu hingga minggu depan: ");
    Serial.print(diff.days());
    Serial.print(" hari, ");
    Serial.print(diff.hours());
    Serial.print(" jam, ");
    Serial.print(diff.minutes());
    Serial.print(" menit, ");
    Serial.print(diff.seconds());
    Serial.println(" detik");
    
    // 7. Total detik dalam TimeSpan
    Serial.print("Total detik dalam 1 minggu: ");
    Serial.println(diff.totalseconds());
  } else {
    Serial.println("Gagal update waktu!");
  }
  
  delay(10000);  // Tampilkan waktu setiap 10 detik
}