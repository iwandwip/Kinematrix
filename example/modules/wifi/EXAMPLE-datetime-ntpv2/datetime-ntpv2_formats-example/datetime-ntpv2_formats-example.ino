/*
 * TimeFormatsExample
 * 
 * Contoh penggunaan DateTimeNTPV2 untuk menampilkan waktu
 * dalam berbagai format standar.
 */

#define ENABLE_MODULE_DATETIME_NTP_V2
#include "Kinematrix.h"

// Konfigurasi WiFi
const char* ssid = "NamaWiFiAnda";
const char* password = "PasswordWiFiAnda";

// Konfigurasi NTP - GMT+7 untuk WIB (Jakarta)
const long gmtOffset_sec = 7 * 3600;  // GMT+7 dalam detik
const int daylightOffset_sec = 0;     // Tidak menggunakan daylight saving

// Inisialisasi objek DateTimeNTPV2
DateTimeNTPV2 dateTime("pool.ntp.org", gmtOffset_sec, daylightOffset_sec);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Memulai TimeFormatsExample...");
  
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
    // Tampilkan waktu dalam berbagai format
    Serial.println("\n===== FORMAT WAKTU =====");
    
    Serial.print("Format Default: ");
    char timeStr[25];
    dateTime.toString(timeStr);
    Serial.println(timeStr);
    
    Serial.print("ISO8601: ");
    Serial.println(dateTime.getISO8601Time());
    
    Serial.print("RFC2822: ");
    Serial.println(dateTime.getRFC2822Time());
    
    Serial.print("RFC3339: ");
    Serial.println(dateTime.getRFC3339Time());
    
    Serial.print("Timestamp (full): ");
    Serial.println(dateTime.timestamp(DateTimeNTPV2::TIMESTAMP_FULL));
    
    Serial.print("Timestamp (date only): ");
    Serial.println(dateTime.timestamp(DateTimeNTPV2::TIMESTAMP_DATE));
    
    Serial.print("Timestamp (time only): ");
    Serial.println(dateTime.timestamp(DateTimeNTPV2::TIMESTAMP_TIME));
    
    Serial.print("Custom Format (dd/mm/yyyy): ");
    Serial.println(dateTime.getFormattedTime("%d/%m/%Y"));
    
    Serial.print("Custom Format (hh:mm:ss AM/PM): ");
    Serial.println(dateTime.getFormattedTime("%I:%M:%S %p"));
    
    Serial.print("Day of Week: ");
    Serial.println(dateTime.getDayOfWeek());
    
    Serial.print("Short Day of Week: ");
    Serial.println(dateTime.getShortDayOfWeek());
    
    Serial.print("Month Name: ");
    Serial.println(dateTime.getMonthName());
    
    Serial.print("Short Month Name: ");
    Serial.println(dateTime.getShortMonthName());
  } else {
    Serial.println("Gagal update waktu!");
  }
  
  delay(10000);  // Tampilkan waktu setiap 10 detik
}