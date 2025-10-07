/*
 * CustomDateTimeExample
 * 
 * Contoh pembuatan objek DateTimeNTPV2 kustom tanpa sinkronisasi NTP
 * untuk situasi di mana tidak ada koneksi internet.
 */

#define ENABLE_MODULE_DATETIME_NTP_V2
#include "Kinematrix.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Memulai CustomDateTimeExample...");

  // Contoh membuat objek DateTimeNTPV2 dengan waktu kustom
  // tanpa bergantung pada NTP

  // 1. Membuat objek dengan tanggal dan waktu spesifik
  DateTimeNTPV2 customDate1(2025, 3, 16, 14, 30, 0);  // 16 Maret 2025, 14:30:00
  
  // 2. Membuat objek dari timestamp Unix
  uint32_t timestamp = 1742345678;  // Unix timestamp tertentu
  DateTimeNTPV2 customDate2(timestamp);
  
  // 3. Membuat objek kemudian memanipulasinya
  DateTimeNTPV2 futureDate(2025, 1, 1, 0, 0, 0);  // 1 Januari 2025 00:00:00
  futureDate = futureDate + TimeSpan(100, 0, 0, 0);  // Tambah 100 hari
  
  // 4. Membuat salinan dari objek lain
  DateTimeNTPV2 dateCopy(customDate1);
  
  // Menampilkan semua tanggal kustom
  char buffer[25];
  
  Serial.println("\n===== CONTOH TANGGAL KUSTOM =====");
  
  Serial.print("Custom Date 1: ");
  customDate1.toString(buffer);
  Serial.println(buffer);
  
  Serial.print("Custom Date 2 (dari timestamp): ");
  customDate2.toString(buffer);
  Serial.println(buffer);
  
  Serial.print("Future Date (100 hari setelah 1 Jan 2025): ");
  futureDate.toString(buffer);
  Serial.println(buffer);
  
  Serial.print("Date Copy: ");
  dateCopy.toString(buffer);
  Serial.println(buffer);
  
  // Menampilkan properti tanggal kustom
  Serial.println("\n===== PROPERTI TANGGAL KUSTOM =====");
  
  Serial.print("Tahun: ");
  Serial.println(customDate1.year());
  
  Serial.print("Bulan: ");
  Serial.println(customDate1.month());
  
  Serial.print("Tanggal: ");
  Serial.println(customDate1.day());
  
  Serial.print("Jam: ");
  Serial.println(customDate1.hour());
  
  Serial.print("Jam (format 12 jam): ");
  Serial.println(customDate1.twelveHour());
  
  Serial.print("AM/PM: ");
  Serial.println(customDate1.isPM() ? "PM" : "AM");
  
  Serial.print("Menit: ");
  Serial.println(customDate1.minute());
  
  Serial.print("Detik: ");
  Serial.println(customDate1.second());
  
  Serial.print("Hari dalam seminggu (0=Minggu, 6=Sabtu): ");
  Serial.println(customDate1.dayOfTheWeek());
  
  Serial.print("Unix Timestamp: ");
  Serial.println(customDate1.unixtime());
  
  // Perbandingan tanggal kustom
  Serial.println("\n===== PERBANDINGAN TANGGAL KUSTOM =====");
  
  if (customDate1 == dateCopy) {
    Serial.println("customDate1 sama dengan dateCopy");
  }
  
  if (customDate1 != customDate2) {
    Serial.println("customDate1 tidak sama dengan customDate2");
  }
  
  if (futureDate > customDate1) {
    Serial.println("futureDate lebih besar dari customDate1");
  }
  
  if (customDate2 < futureDate) {
    Serial.println("customDate2 lebih kecil dari futureDate");
  }
  
  // Validasi tanggal
  Serial.println("\n===== VALIDASI TANGGAL =====");
  
  DateTimeNTPV2 emptyDate;  // Objek tanpa inisialisasi
  
  if (customDate1.isValid()) {
    Serial.println("customDate1 valid");
  }
  
  if (!emptyDate.isValid()) {
    Serial.println("emptyDate tidak valid");
  }
}

void loop() {
  // Tidak ada yang perlu diupdate di loop karena tidak menggunakan NTP
  delay(10000);
}