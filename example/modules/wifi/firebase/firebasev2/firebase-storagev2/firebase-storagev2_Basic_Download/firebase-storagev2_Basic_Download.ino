/**
 * FirebaseV2Storage - Basic Download Example
 * 
 * Contoh dasar mengunduh file dari Firebase Storage ke SPIFFS.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include "firebase-storagev2.h"

// Ganti dengan kredensial WiFi Anda
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// Ganti dengan kredensial Firebase Anda
#define API_KEY "FIREBASE_API_KEY"
#define USER_EMAIL "user@example.com"
#define USER_PASSWORD "password"
#define PROJECT_ID "your-project-id"
#define BUCKET_ID "your-project-id.appspot.com"

// Nama file untuk download
#define CLOUD_FILE_NAME "test.txt"
#define LOCAL_FILE_PATH "/downloaded.txt"

FirebaseV2Storage storage;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nFirebaseV2Storage Basic Download Example");
  
  // Inisialisasi SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal menginisialisasi SPIFFS!");
    return;
  }
  
  // Hapus file jika sudah ada
  if (SPIFFS.exists(LOCAL_FILE_PATH)) {
    SPIFFS.remove(LOCAL_FILE_PATH);
    Serial.println("File lama dihapus");
  }
  
  // Koneksi ke WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Terhubung dengan IP: ");
  Serial.println(WiFi.localIP());
  
  // Inisialisasi Firebase Storage
  Serial.println("\nMenginisialisasi Firebase Storage...");
  if (storage.begin(API_KEY, USER_EMAIL, USER_PASSWORD, PROJECT_ID, BUCKET_ID)) {
    Serial.println("Firebase Storage berhasil diinisialisasi!");
  } else {
    Serial.println("Gagal menginisialisasi Firebase Storage");
    Serial.println("Error: " + storage.getLastError());
    return;
  }

  // Tunggu Firebase siap
  Serial.println("\nMenunggu Firebase siap...");
  while (!storage.isReady()) {
    storage.loop();
    delay(100);
  }
  
  // Download file secara sinkron (menunggu hingga selesai)
  Serial.println("\nMengunduh file...");
  if (storage.downloadFile(CLOUD_FILE_NAME, LOCAL_FILE_PATH, true)) {
    Serial.println("File berhasil diunduh!");
    
    // Baca dan tampilkan isi file
    readAndDisplayFile();
  } else {
    Serial.println("Gagal mengunduh file");
    Serial.println("Error: " + storage.getLastError());
  }
}

void loop() {
  // Tetap jalankan loop() untuk memproses tugas Firebase
  storage.loop();
  delay(100);
}

void readAndDisplayFile() {
  File file = SPIFFS.open(LOCAL_FILE_PATH, "r");
  if (!file) {
    Serial.println("Gagal membuka file untuk dibaca!");
    return;
  }
  
  Serial.println("\nIsi file yang diunduh:");
  Serial.println("--------------------------------------------");
  
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Serial.println(line);
  }
  
  Serial.println("--------------------------------------------");
  
  file.close();
  
  // Tampilkan informasi file
  File fileInfo = SPIFFS.open(LOCAL_FILE_PATH, "r");
  if (fileInfo) {
    Serial.print("Ukuran file: ");
    Serial.print(fileInfo.size());
    Serial.println(" bytes");
    fileInfo.close();
  }
}