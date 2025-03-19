/**
 * FirebaseV2Storage - List Files Example
 * 
 * Contoh untuk mendapatkan daftar file dalam bucket Firebase Storage.
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

// Prefix untuk mencari file (kosong = semua file)
#define FILE_PREFIX ""
// Jumlah maksimum file yang akan diambil
#define MAX_RESULTS 20

FirebaseV2Storage storage;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nFirebaseV2Storage List Files Example");
  
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
  
  // Dapatkan daftar file
  getFilesList();
}

void loop() {
  // Tetap jalankan loop() untuk memproses tugas Firebase
  storage.loop();
  delay(100);
}

void getFilesList() {
  Serial.println("\nMendapatkan daftar file...");
  
  // Metode 1: Sinkron - menunggu respons
  String filesList = storage.listFiles(FILE_PREFIX, MAX_RESULTS, true);
  
  if (filesList.length() > 0 && !filesList.startsWith("Error:")) {
    Serial.println("\nDaftar file dalam bucket " + String(BUCKET_ID) + ":");
    Serial.println("--------------------------------------------");
    Serial.println(filesList);
    Serial.println("--------------------------------------------");
    
    // Anda dapat memproses atau mem-parsing respons JSON di sini jika diperlukan
    
  } else {
    Serial.println("Gagal mendapatkan daftar file atau bucket kosong");
    if (filesList.startsWith("Error:")) {
      Serial.println(filesList);
    }
  }
  
  // Metode 2: Asinkron - untuk penggunaan non-blocking
  // Jika ingin menggunakan metode asinkron, uncomment kode berikut:
  /*
  Serial.println("\nMendapatkan daftar file secara asinkron...");
  if (storage.listFiles(FILE_PREFIX, MAX_RESULTS, false)) {
    Serial.println("Permintaan daftar file dikirim, menunggu hasil...");
  } else {
    Serial.println("Gagal mengirim permintaan daftar file");
  }
  
  // Anda perlu memeriksa hasilnya di loop()
  // menggunakan storage.isResultReady() dan storage.getLastPayload()
  */
}