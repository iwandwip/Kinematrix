/**
 * FirebaseV2Storage - Service Account Authentication Example
 * 
 * Contoh menggunakan autentikasi Service Account untuk Firebase Storage.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include "firebase-storagev2.h"

// Ganti dengan kredensial WiFi Anda
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// Ganti dengan kredensial Firebase Service Account Anda
#define PROJECT_ID "your-project-id"
#define CLIENT_EMAIL "firebase-adminsdk-xxxxx@your-project-id.iam.gserviceaccount.com"
// Private key (ganti dengan private key Anda)
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\n"
                                  "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQC+Z8IYbx+ZnxnQ\n"
                                  "... MANY LINES OF PRIVATE KEY HERE ...\n"
                                  "-----END PRIVATE KEY-----\n";

// Storage bucket ID (biasanya project_id.appspot.com)
#define BUCKET_ID "your-project-id.appspot.com"

// Nama file untuk upload
#define LOCAL_FILE_PATH "/test.txt"
#define CLOUD_FILE_NAME "test_serviceaccount.txt"

FirebaseV2Storage storage;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nFirebaseV2Storage Service Account Authentication Example");
  
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
  
  // Inisialisasi Firebase Storage dengan Service Account
  Serial.println("\nMenginisialisasi Firebase Storage dengan Service Account...");
  if (storage.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY, BUCKET_ID)) {
    Serial.println("Firebase Storage berhasil diinisialisasi!");
  } else {
    Serial.println("Gagal menginisialisasi Firebase Storage");
    Serial.println("Error: " + storage.getLastError());
    return;
  }

  // Tunggu Firebase siap
  Serial.println("\nMenunggu Firebase siap...");
  unsigned long startTime = millis();
  while (!storage.isReady()) {
    storage.loop();
    delay(100);
    
    // Timeout setelah 30 detik
    if (millis() - startTime > 30000) {
      Serial.println("Timeout menunggu Firebase siap");
      return;
    }
  }
  
  // Upload file
  Serial.println("\nMengunggah file...");
  if (storage.uploadFile(LOCAL_FILE_PATH, CLOUD_FILE_NAME, "text/plain", true)) {
    Serial.println("File berhasil diunggah!");
  } else {
    Serial.println("Gagal mengunggah file");
    Serial.println("Error: " + storage.getLastError());
    return;
  }
  
  // List files untuk memverifikasi upload
  Serial.println("\nMendapatkan daftar file...");
  String filesList = storage.listFiles("", 10, true);
  Serial.println("Daftar file:");
  Serial.println(filesList);
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
  
  file.println("Ini adalah file contoh untuk autentikasi Service Account Firebase Storage");
  file.println("Dibuat pada: " + String(millis()));
  file.println("Dari: ESP32");
  file.close();
  
  Serial.println("File contoh berhasil dibuat di " + String(LOCAL_FILE_PATH));
}