/*
 * Multiple_Servo_Control.ino
 * 
 * Mengendalikan beberapa servo secara bersamaan dengan PCA9685 dan ESP32
 * 
 * Koneksi Hardware:
 * ESP32:
 * - GPIO21 -> SDA PCA9685
 * - GPIO22 -> SCL PCA9685
 * - 3.3V -> VCC PCA9685
 * - GND -> GND PCA9685
 * 
 * Servo dihubungkan ke channel 0-3 pada PCA9685
 */

#define ENABLE_MODULE_SERVO_HCPA9685
#include "Kinematrix.h"
#include <Wire.h>

// Definisi pin I2C ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Alamat I2C PCA9685 (default 0x40)
#define I2C_ADDRESS 0x40

// Jumlah servo yang digunakan
#define NUM_SERVOS 4

// Buat instance dari library
HCPCA9685 servoDriver(I2C_ADDRESS);

void setup() {
  Serial.begin(115200);
  
  // Konfigurasi pin I2C ESP32
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Inisialisasi driver dalam mode servo
  servoDriver.Init(SERVO_MODE);
  
  // Aktifkan device (keluarkan dari mode sleep)
  servoDriver.Sleep(false);
  
  Serial.println("PCA9685 siap untuk mengontrol multiple servo");
  
  // Reset semua servo ke posisi tengah
  for (int i = 0; i < NUM_SERVOS; i++) {
    servoDriver.Servo(i, 240);
    delay(100);
  }
}

void loop() {
  // Pola gerakan wave (gelombang)
  wavePattern();
  
  // Pola gerakan synchronized (semua servo bergerak bersamaan)
  synchronizedPattern();
  
  // Pola gerakan sequential (berurutan)
  sequentialPattern();
}

// Fungsi untuk membuat pola gelombang
void wavePattern() {
  Serial.println("Pola: Wave");
  
  for (int pos = 0; pos <= 480; pos += 20) {
    for (int i = 0; i < NUM_SERVOS; i++) {
      // Terapkan offset untuk setiap servo agar membentuk gelombang
      int servoPos = pos + (i * 120);
      if (servoPos > 480) servoPos = servoPos - 480;
      servoDriver.Servo(i, servoPos);
    }
    delay(100);
  }
  delay(1000);
}

// Fungsi untuk menggerakkan semua servo secara bersamaan
void synchronizedPattern() {
  Serial.println("Pola: Synchronized");
  
  // Semua servo ke posisi minimum
  for (int i = 0; i < NUM_SERVOS; i++) {
    servoDriver.Servo(i, 0);
  }
  delay(1000);
  
  // Semua servo ke posisi tengah
  for (int i = 0; i < NUM_SERVOS; i++) {
    servoDriver.Servo(i, 240);
  }
  delay(1000);
  
  // Semua servo ke posisi maksimum
  for (int i = 0; i < NUM_SERVOS; i++) {
    servoDriver.Servo(i, 480);
  }
  delay(1000);
}

// Fungsi untuk menggerakkan servo secara berurutan
void sequentialPattern() {
  Serial.println("Pola: Sequential");
  
  // Gerakkan setiap servo secara berurutan
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print("Menggerakkan servo ");
    Serial.println(i);
    
    servoDriver.Servo(i, 0);
    delay(500);
    servoDriver.Servo(i, 240);
    delay(500);
    servoDriver.Servo(i, 480);
    delay(500);
    servoDriver.Servo(i, 240);
    delay(500);
  }
}