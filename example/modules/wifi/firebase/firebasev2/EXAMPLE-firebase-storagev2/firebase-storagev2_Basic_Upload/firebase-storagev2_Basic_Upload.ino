/**
 * FirebaseV2Storage - Basic Upload Example
 * 
 * Contoh dasar mengunggah file dari SPIFFS ke Firebase Storage.
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

// Nama file untuk upload
#define LOCAL_FILE_PATH "/test.txt"
#define CLOUD_FILE_NAME "test.txt"

FirebaseV2Storage storage;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nFirebaseV2Storage Basic Upload Example");
  
  // Inisialisasi SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal menginisialisasi SPIFFS!");
    return;
  }
  
  // Buat file contoh untuk diupload
  createSampleFile();
  
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
  
  // Upload file secara sinkron (menunggu hingga selesai)
  Serial.println("\nMengunggah file...");
  if (storage.uploadFile(LOCAL_FILE_PATH, CLOUD_FILE_NAME, "text/plain", true)) {
    Serial.println("File berhasil diunggah!");
  } else {
    Serial.println("Gagal mengunggah file");
    Serial.println("Error: " + storage.getLastError());
  }
}

void loop() {
  // Tetap jalankan loop() untuk memproses tugas Firebase
  storage.loop();
  delay(100);
}

void createSampleFile() {
  // Hapus file jika sudah ada
  if (SPIFFS.exists(LOCAL_FILE_PATH)) {
    SPIFFS.remove(LOCAL_FILE_PATH);
  }
  
  // Buat dan tulis ke file
  File file = SPIFFS.open(LOCAL_FILE_PATH, "w");
  if (!file) {
    Serial.println("Gagal membuka file untuk ditulis!");
    return;
  }
  
  file.println("Ini adalah file contoh untuk Firebase Storage");
  file.println("Dibuat pada: " + String(millis()));
  file.println("Dari: ESP32");
  file.close();
  
  Serial.println("File contoh berhasil dibuat di " + String(LOCAL_FILE_PATH));
  
  // Verifikasi ukuran file
  File readFile = SPIFFS.open(LOCAL_FILE_PATH, "r");
  if (readFile) {
    Serial.print("Ukuran file: ");
    Serial.print(readFile.size());
    Serial.println(" bytes");
    readFile.close();
  }
}