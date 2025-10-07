/*
 * Timer_Progress.ino
 * 
 * Contoh menampilkan progress timer sebagai persentase
 * dan menggunakan progress untuk aplikasi visual.
 */

#include "timer-manager.h"

// Definisikan pin untuk LED bar (8 LEDs)
#define NUM_LEDS 8
const int ledPins[NUM_LEDS] = {2, 3, 4, 5, 6, 7, 8, 9};

// Timer dengan durasi 10 detik
TimerManager progressTimer(10000);

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi semua pin LED sebagai output
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  Serial.println("Contoh Timer Progress");
  Serial.println("Timer berjalan selama 10 detik");
  Serial.println("LED bar akan menampilkan progress timer");
  
  // Mulai timer
  progressTimer.start();
  
  Serial.println("Timer dimulai!");
}

void loop() {
  // Hanya jalankan jika timer masih aktif
  if (progressTimer.isRunning() && !progressTimer.isExpired()) {
    // Dapatkan kemajuan timer sebagai persentase (0-100%)
    int percent = progressTimer.getProgressPercent();
    
    // Dapatkan kemajuan timer sebagai nilai float (0.0-1.0)
    float progress = progressTimer.getProgress();
    
    // Update tampilan LED bar berdasarkan progress
    updateLedBar(progress);
    
    // Tampilkan progress di Serial
    Serial.print("Waktu berlalu: ");
    Serial.print(progressTimer.getElapsedSeconds());
    Serial.print("s / ");
    Serial.print(percent);
    Serial.println("%");
    
    // Tampilkan bar ASCII untuk visualisasi
    displayProgressBar(percent);
    
    delay(250); // Update 4 kali per detik
  } else if (progressTimer.isExpired()) {
    // Timer selesai
    if (progressTimer.isRunning()) {
      Serial.println("\nTimer selesai!");
      
      // Nyalakan semua LED selama 1 detik untuk menunjukkan selesai
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(ledPins[i], HIGH);
      }
      delay(1000);
      
      // Matikan semua LED
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      
      progressTimer.stop();
    }
  }
}

// Fungsi untuk menampilkan progress bar ASCII di Serial Monitor
void displayProgressBar(int percent) {
  const int barWidth = 50;
  int filledWidth = barWidth * percent / 100;
  
  Serial.print("[");
  for (int i = 0; i < barWidth; i++) {
    if (i < filledWidth) {
      Serial.print("#");
    } else {
      Serial.print(" ");
    }
  }
  Serial.print("] ");
  Serial.print(percent);
  Serial.print("%\r"); // Kembali ke awal baris
}

// Fungsi untuk update LED bar berdasarkan progress
void updateLedBar(float progress) {
  // Hitung berapa banyak LED yang harus menyala
  int ledsToLight = (int)(progress * NUM_LEDS);
  
  // Update status LED
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < ledsToLight) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
}
