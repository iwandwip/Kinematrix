/**
 * FirebaseV2Storage - OTA Update Example
 * 
 * Contoh pembaruan OTA (Over-The-Air) menggunakan Firebase Storage.
 * Unggah berkas firmware.bin ke Firebase Storage dan ESP32 akan mengunduh
 * dan menginstal firmware baru.
 */

#include <Arduino.h>
#include <WiFi.h>
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

// Nama file firmware di Firebase Storage
#define FIRMWARE_FILENAME "firmware.bin"

// Versi firmware saat ini (untuk pengecekan)
#define FIRMWARE_VERSION "1.0.0"

FirebaseV2Storage storage;
bool updateRequested = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nFirebaseV2Storage OTA Update Example");
  Serial.println("Firmware saat ini: v" + String(FIRMWARE_VERSION));
  
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
  
  // Periksa metadata firmware untuk melihat apakah pembaruan diperlukan
  Serial.println("\nMemeriksa metadata firmware...");
  String metadata = storage.getFileMetadata(FIRMWARE_FILENAME, true);
  
  if (metadata.length() > 0 && !metadata.startsWith("Error:")) {
    Serial.println("Metadata firmware ditemukan. Memulai pembaruan OTA...");
    updateRequested = true;
  } else {
    Serial.println("Metadata firmware tidak ditemukan atau error. Tidak ada pembaruan.");
    if (metadata.startsWith("Error:")) {
      Serial.println(metadata);
    }
  }
}

void loop() {
  // Tetap jalankan loop() untuk memproses tugas Firebase
  storage.loop();
  
  // Lakukan pembaruan jika diminta
  if (updateRequested && storage.isReady()) {
    updateRequested = false;
    performUpdate();
  }
  
  // Tampilkan kemajuan jika hasil tersedia
  if (storage.isResultReady()) {
    Serial.println("Result: " + storage.getLastPayload());
    
    if (storage.getDownloadProgress() > 0) {
      Serial.print("Download progress: ");
      Serial.print(storage.getDownloadProgress());
      Serial.println("%");
    }
  }
  
  delay(100);
}

void performUpdate() {
  Serial.println("\nMemulai pembaruan OTA...");
  Serial.println("Mengunduh firmware dari Firebase Storage...");
  
  // Mulai OTA dengan fungsi asinkron
  if (storage.performOTA(FIRMWARE_FILENAME, false)) {
    Serial.println("Proses download firmware dimulai...");
    Serial.println("ESP32 akan restart setelah pembaruan selesai.");
  } else {
    Serial.println("Gagal memulai pembaruan OTA");
    Serial.println("Error: " + storage.getLastError());
  }
}

// Fungsi ini akan dipanggil setelah restart
void restart() {
  Serial.println("Pembaruan selesai");
  Serial.println("Restarting...");
  ESP.restart();
}