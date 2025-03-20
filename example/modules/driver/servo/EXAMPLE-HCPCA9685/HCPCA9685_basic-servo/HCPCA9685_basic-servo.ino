/*
 * Basic_Servo_Control.ino
 * 
 * Contoh dasar mengendalikan satu servo menggunakan PCA9685 dan ESP32
 * 
 * Koneksi Hardware:
 * ESP32:
 * - GPIO21 -> SDA PCA9685
 * - GPIO22 -> SCL PCA9685
 * - 3.3V -> VCC PCA9685
 * - GND -> GND PCA9685
 * 
 * Servo dihubungkan ke channel 0 pada PCA9685
 */

#define ENABLE_MODULE_SERVO_HCPA9685
#include "Kinematrix.h"
#include <Wire.h>

// Definisi pin I2C ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Alamat I2C PCA9685 (default 0x40)
#define I2C_ADDRESS 0x40

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
  
  Serial.println("PCA9685 siap untuk mengontrol servo");
}

void loop() {
  // Gerakan servo dari minimum ke maksimum dan kembali
  
  // Posisi minimum (0 derajat)
  Serial.println("Posisi minimum");
  servoDriver.Servo(0, 0);
  delay(1000);
  
  // Posisi tengah (90 derajat)
  Serial.println("Posisi tengah");
  servoDriver.Servo(0, 240);
  delay(1000);
  
  // Posisi maksimum (180 derajat)
  Serial.println("Posisi maksimum");
  servoDriver.Servo(0, 480);
  delay(1000);
}