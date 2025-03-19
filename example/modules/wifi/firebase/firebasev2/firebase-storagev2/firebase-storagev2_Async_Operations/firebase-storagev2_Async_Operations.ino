/**
 * FirebaseV2Storage - Async Operations Example
 * 
 * Contoh operasi asinkron dengan FirebaseV2Storage.
 * Menunjukkan cara menggunakan callback dan polling untuk operasi non-blocking.
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
#define CLOUD_FILE_NAME "test_async.txt"

FirebaseV2Storage storage;

// Status operasi
bool uploadStarted = false;
bool uploadCompleted = false;
bool listStarted = false;
bool listCompleted = false;
bool downloadStarted = false;
bool downloadCompleted = false;
bool deleteStarted = false;
bool deleteCompleted = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nFirebaseV2Storage Async Operations Example");
  
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
}

void loop() {
  // Update Firebase tasks
  storage.loop();
  
  // Jalankan operasi storage jika Firebase siap
  if (storage.isReady()) {
    // 1. Upload file secara asinkron
    if (!uploadStarted) {
      Serial.println("\nMemulai upload file secara asinkron...");
      if (storage.uploadFile(LOCAL_FILE_PATH, CLOUD_FILE_NAME, "text/plain", false)) {
        Serial.println("Upload dimulai");
        uploadStarted = true;
      } else {
        Serial.println("Gagal memulai upload: " + storage.getLastError());
      }
    }
    
    // 2. List files secara asinkron setelah upload selesai
    if (uploadCompleted && !listStarted) {
      Serial.println("\nMemulai listing files secara asinkron...");
      if (storage.listFiles("", 10, false)) {
        Serial.println("Listing dimulai");
        listStarted = true;
      } else {
        Serial.println("Gagal memulai listing: " + storage.getLastError());
      }
    }
    
    // 3. Download file secara asinkron setelah listing selesai
    if (listCompleted && !downloadStarted) {
      Serial.println("\nMemulai download file secara asinkron...");
      if (storage.downloadFile(CLOUD_FILE_NAME, "/downloaded_async.txt", false)) {
        Serial.println("Download dimulai");
        downloadStarted = true;
      } else {
        Serial.println("Gagal memulai download: " + storage.getLastError());
      }
    }
    
    // 4. Delete file secara asinkron setelah download selesai
    if (downloadCompleted && !deleteStarted) {
      Serial.println("\nMemulai delete file secara asinkron...");
      if (storage.deleteFile(CLOUD_FILE_NAME, false)) {
        Serial.println("Delete dimulai");
        deleteStarted = true;
      } else {
        Serial.println("Gagal memulai delete: " + storage.getLastError());
      }
    }
    
    // Cek hasil operasi asinkron
    if (storage.isResultReady()) {
      String result = storage.getLastPayload();
      Serial.println("\nHasil operasi: " + result);
      
      // Perbarui status berdasarkan progres upload/download
      if (!uploadCompleted && uploadStarted && storage.getUploadProgress() == 100) {
        uploadCompleted = true;
        Serial.println("Upload selesai!");
        
        // Beri jeda sebelum operasi berikutnya
        delay(1000);
      }
      
      if (!listCompleted && listStarted && result.length() > 0 && !result.startsWith("Error:")) {
        listCompleted = true;
        Serial.println("Listing selesai!");
        
        // Beri jeda sebelum operasi berikutnya
        delay(1000);
      }
      
      if (!downloadCompleted && downloadStarted && storage.getDownloadProgress() == 100) {
        downloadCompleted = true;
        Serial.println("Download selesai!");
        
        // Baca file yang didownload
        readAndDisplayFile("/downloaded_async.txt");
        
        // Beri jeda sebelum operasi berikutnya
        delay(1000);
      }
      
      if (!deleteCompleted && deleteStarted && result.indexOf("complete") > 0) {
        deleteCompleted = true;
        Serial.println("Delete selesai!");
        Serial.println("\nSemua operasi asinkron selesai!");
      }
    }
    
    // Monitor progres upload/download
    if (uploadStarted && !uploadCompleted && storage.getUploadProgress() > 0) {
      Serial.printf("Upload progress: %d%%\n", storage.getUploadProgress());
    }
    
    if (downloadStarted && !downloadCompleted && storage.getDownloadProgress() > 0) {
      Serial.printf("Download progress: %d%%\n", storage.getDownloadProgress());
    }
  }
  
  delay(500);
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
  
  file.println("Ini adalah file contoh untuk operasi asinkron Firebase Storage");
  file.println("Dibuat pada: " + String(millis()));
  file.println("Dari: ESP32");
  file.close();
  
  Serial.println("File contoh berhasil dibuat di " + String(LOCAL_FILE_PATH));
}

void readAndDisplayFile(const char* filePath) {
  File file = SPIFFS.open(filePath, "r");
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
}