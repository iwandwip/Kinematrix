/*
 * TimeComparisonExample
 * 
 * Contoh penggunaan operator perbandingan DateTimeNTPV2
 * dan pengecekan waktu untuk keperluan alarm atau timer.
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

// Definisikan beberapa waktu untuk perbandingan
DateTimeNTPV2 meetingTime;     // Waktu meeting
DateTimeNTPV2 alarmTime;       // Waktu alarm
DateTimeNTPV2 deadlineTime;    // Waktu deadline

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Memulai TimeComparisonExample...");
  
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
    
    // Setelah mendapatkan waktu, set waktu-waktu perbandingan
    dateTime.update();
    
    // 1. Set meeting time (pukul 10:00 hari ini)
    meetingTime = DateTimeNTPV2(dateTime.year(), dateTime.month(), dateTime.day(), 10, 0, 0);
    
    // 2. Set alarm time (pukul 07:30 hari ini)
    alarmTime = DateTimeNTPV2(dateTime.year(), dateTime.month(), dateTime.day(), 7, 30, 0);
    
    // 3. Set deadline time (pukul 17:00 hari ini)
    deadlineTime = DateTimeNTPV2(dateTime.year(), dateTime.month(), dateTime.day(), 17, 0, 0);
    
    // Jika alarm sudah berlalu hari ini, set untuk besok
    if (alarmTime < dateTime) {
      alarmTime = alarmTime + TimeSpan(1, 0, 0, 0); // Tambah 1 hari
      Serial.println("Alarm diset untuk besok");
    }
    
    // Cetak waktu-waktu yang telah diset
    char timeStr[25];
    
    Serial.println("\n===== JADWAL WAKTU =====");
    
    Serial.print("Alarm diset untuk: ");
    alarmTime.toString(timeStr);
    Serial.println(timeStr);
    
    Serial.print("Meeting diset untuk: ");
    meetingTime.toString(timeStr);
    Serial.println(timeStr);
    
    Serial.print("Deadline diset untuk: ");
    deadlineTime.toString(timeStr);
    Serial.println(timeStr);
  } else {
    Serial.println("Gagal memulai NTP Client!");
  }
}

void loop() {
  // Update waktu saat ini
  if (dateTime.update()) {
    char timeStr[25];
    dateTime.toString(timeStr);
    
    Serial.println("\n===== PERBANDINGAN WAKTU =====");
    Serial.print("Waktu saat ini: ");
    Serial.println(timeStr);
    
    // Perbandingan waktu menggunakan operator
    
    // 1. Periksa apakah sudah waktunya alarm
    if (dateTime >= alarmTime && dateTime <= alarmTime + TimeSpan(0, 0, 5, 0)) {
      Serial.println("ALARM BERBUNYI! Waktunya bangun!");
      // Di sini Anda bisa menambahkan kode untuk membunyikan buzzer, dll.
    }
    
    // 2. Periksa waktu meeting
    if (dateTime < meetingTime) {
      // Hitung waktu tersisa hingga meeting
      TimeSpan timeToMeeting = meetingTime - dateTime;
      Serial.print("Meeting akan dimulai dalam: ");
      Serial.print(timeToMeeting.hours());
      Serial.print(" jam, ");
      Serial.print(timeToMeeting.minutes());
      Serial.print(" menit, ");
      Serial.print(timeToMeeting.seconds());
      Serial.println(" detik");
    } else if (dateTime >= meetingTime && dateTime <= meetingTime + TimeSpan(0, 1, 0, 0)) {
      Serial.println("MEETING SEDANG BERLANGSUNG!");
    } else if (dateTime > meetingTime) {
      Serial.println("Meeting sudah selesai.");
    }
    
    // 3. Periksa deadline
    if (dateTime < deadlineTime) {
      // Hitung waktu tersisa hingga deadline
      TimeSpan timeToDeadline = deadlineTime - dateTime;
      Serial.print("Waktu tersisa hingga deadline: ");
      Serial.print(timeToDeadline.hours());
      Serial.print(" jam, ");
      Serial.print(timeToDeadline.minutes());
      Serial.print(" menit, ");
      Serial.print(timeToDeadline.seconds());
      Serial.println(" detik");
    } else {
      Serial.println("DEADLINE TELAH BERAKHIR!");
    }
    
    // 4. Periksa apakah sekarang hari kerja
    uint8_t dayOfWeek = dateTime.dayOfTheWeek();
    if (dayOfWeek >= 1 && dayOfWeek <= 5) {  // 1-5 = Senin-Jumat
      Serial.println("Sekarang hari kerja");
    } else {
      Serial.println("Sekarang akhir pekan");
    }
    
    // 5. Periksa apakah sekarang jam kerja (9:00 - 17:00)
    uint8_t currentHour = dateTime.hour();
    if (currentHour >= 9 && currentHour < 17) {
      Serial.println("Sekarang jam kerja");
    } else {
      Serial.println("Di luar jam kerja");
    }
  } else {
    Serial.println("Gagal update waktu!");
  }
  
  delay(5000);  // Check setiap 5 detik
}