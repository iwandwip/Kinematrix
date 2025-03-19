/**
 * FirebaseV2Storage - Delete File Example
 * 
 * Contoh untuk menghapus file dari Firebase Storage.
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

// Nama file yang akan dihapus
#define CLOUD_FILE_NAME "test.txt"

FirebaseV2Storage storage;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nFirebaseV2Storage Delete File Example");
  
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
  
  // Daftar file sebelum penghapusan
  Serial.println("\nDaftar file sebelum penghapusan:");
  String filesBefore = storage.listFiles("", 10, true);
  Serial.println(filesBefore);
  
  // Hapus file
  Serial.println("\nMenghapus file " + String(CLOUD_FILE_NAME) + "...");
  if (storage.deleteFile(CLOUD_FILE_NAME, true)) {
    Serial.println("File berhasil dihapus!");
  } else {
    Serial.println("Gagal menghapus file");
    Serial.println("Error: " + storage.getLastError());
  }
  
  // Daftar file setelah penghapusan
  Serial.println("\nDaftar file setelah penghapusan:");
  String filesAfter = storage.listFiles("", 10, true);
  Serial.println(filesAfter);
}

void loop() {
  // Tetap jalankan loop() untuk memproses tugas Firebase
  storage.loop();
  delay(100);
}