/*
 * Dual_PCA9685_Control.ino
 * 
 * Contoh mengendalikan dua modul PCA9685 secara bersamaan dengan ESP32
 * 
 * Koneksi Hardware:
 * ESP32:
 * - GPIO21 -> SDA kedua PCA9685
 * - GPIO22 -> SCL kedua PCA9685
 * - 3.3V -> VCC kedua PCA9685
 * - GND -> GND kedua PCA9685
 * 
 * PCA9685 pertama - alamat I2C 0x40 (default)
 * PCA9685 kedua - alamat I2C 0x41 (dengan A0 terhubung ke VCC)
 */

#define ENABLE_MODULE_SERVO_HCPA9685
#include "Kinematrix.h"
#include <Wire.h>

// Definisi pin I2C ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Alamat I2C untuk kedua modul PCA9685
#define I2C_ADDRESS_1 0x40  // Alamat default
#define I2C_ADDRESS_2 0x41  // Alamat dengan A0 terhubung ke VCC

// Buat instance dari library untuk kedua modul
HCPCA9685 servoDriver1(I2C_ADDRESS_1);
HCPCA9685 servoDriver2(I2C_ADDRESS_2);

void setup() {
  Serial.begin(115200);
  
  // Konfigurasi pin I2C ESP32
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Inisialisasi kedua modul dalam mode servo
  servoDriver1.Init(SERVO_MODE);
  servoDriver2.Init(SERVO_MODE);
  
  // Aktifkan kedua modul (keluarkan dari mode sleep)
  servoDriver1.Sleep(false);
  servoDriver2.Sleep(false);
  
  Serial.println("Dual PCA9685 siap untuk mengontrol servo");
  Serial.println("Modul 1: Kanal 0-15");
  Serial.println("Modul 2: Kanal 16-31 (diakses sebagai 0-15 pada driver kedua)");
  
  // Reset semua servo ke posisi tengah
  resetAllServos();
}

void loop() {
  // Demo gerakan alternating (bergantian) antara dua modul
  Serial.println("Demo: Alternating movement");
  alternatingDemo();
  delay(1000);
  
  // Demo gerakan mirrored (cermin) antara dua modul
  Serial.println("Demo: Mirrored movement");
  mirroredDemo();
  delay(1000);
  
  // Demo gerakan sequential (berurutan) melalui semua servo
  Serial.println("Demo: Sequential all servos");
  sequentialAllDemo();
  delay(1000);
  
  // Demo gerakan wave (gelombang) melalui semua servo
  Serial.println("Demo: Wave through all servos");
  waveAllDemo();
  delay(1000);
}

// Reset semua servo ke posisi tengah
void resetAllServos() {
  Serial.println("Resetting all servos to center position");
  
  // Reset servo di modul 1
  for (int i = 0; i < 16; i++) {
    servoDriver1.Servo(i, 240);
    delay(50);
  }
  
  // Reset servo di modul 2
  for (int i = 0; i < 16; i++) {
    servoDriver2.Servo(i, 240);
    delay(50);
  }
  
  delay(1000);
}

// Demo gerakan bergantian antara dua modul
void alternatingDemo() {
  // Gerakkan semua servo di modul 1 ke minimum, modul 2 ke maksimum
  for (int i = 0; i < 16; i++) {
    servoDriver1.Servo(i, 0);
    servoDriver2.Servo(i, 480);
  }
  delay(1000);
  
  // Gerakkan semua servo di modul 1 ke maksimum, modul 2 ke minimum
  for (int i = 0; i < 16; i++) {
    servoDriver1.Servo(i, 480);
    servoDriver2.Servo(i, 0);
  }
  delay(1000);
  
  // Kembali ke posisi tengah
  resetAllServos();
}

// Demo gerakan cermin antara dua modul
void mirroredDemo() {
  // Sweep dari tengah ke ekstrem
  for (int pos = 240; pos <= 480; pos += 10) {
    for (int i = 0; i < 16; i++) {
      servoDriver1.Servo(i, pos);
      servoDriver2.Servo(i, 480 - pos);  // Gerakan cermin
    }
    delay(50);
  }
  
  // Sweep balik ke tengah
  for (int pos = 480; pos >= 240; pos -= 10) {
    for (int i = 0; i < 16; i++) {
      servoDriver1.Servo(i, pos);
      servoDriver2.Servo(i, 480 - pos);  // Gerakan cermin
    }
    delay(50);
  }
  
  // Sweep ke ekstrem lain
  for (int pos = 240; pos >= 0; pos -= 10) {
    for (int i = 0; i < 16; i++) {
      servoDriver1.Servo(i, pos);
      servoDriver2.Servo(i, 480 - pos);  // Gerakan cermin
    }
    delay(50);
  }
  
  // Sweep balik ke tengah
  for (int pos = 0; pos <= 240; pos += 10) {
    for (int i = 0; i < 16; i++) {
      servoDriver1.Servo(i, pos);
      servoDriver2.Servo(i, 480 - pos);  // Gerakan cermin
    }
    delay(50);
  }
}

// Demo gerakan berurutan melalui semua servo
void sequentialAllDemo() {
  // Gerakkan setiap servo secara berurutan
  
  // Modul 1
  for (int i = 0; i < 16; i++) {
    Serial.print("Modul 1, Servo ");
    Serial.println(i);
    
    servoDriver1.Servo(i, 0);    // Posisi minimum
    delay(200);
    servoDriver1.Servo(i, 480);  // Posisi maksimum
    delay(200);
    servoDriver1.Servo(i, 240);  // Kembali ke tengah
    delay(100);
  }
  
  // Modul 2
  for (int i = 0; i < 16; i++) {
    Serial.print("Modul 2, Servo ");
    Serial.println(i);
    
    servoDriver2.Servo(i, 0);    // Posisi minimum
    delay(200);
    servoDriver2.Servo(i, 480);  // Posisi maksimum
    delay(200);
    servoDriver2.Servo(i, 240);  // Kembali ke tengah
    delay(100);
  }
}

// Demo gerakan gelombang melalui semua servo
void waveAllDemo() {
  // Membuat efek gelombang di semua servo pada kedua modul
  for (int wave = 0; wave < 2; wave++) {
    for (int pos = 0; pos <= 480; pos += 10) {
      // Modul 1
      for (int i = 0; i < 16; i++) {
        // Tambahkan offset untuk efek gelombang
        int servoPos = pos + (i * 30);
        if (servoPos > 480) servoPos = servoPos - 480;
        servoDriver1.Servo(i, servoPos);
      }
      
      // Modul 2
      for (int i = 0; i < 16; i++) {
        // Tambahkan offset untuk efek gelombang
        int servoPos = pos + ((i + 16) * 30);
        if (servoPos > 480) servoPos = servoPos - 480;
        servoDriver2.Servo(i, servoPos);
      }
      
      delay(30);
    }
  }
  
  // Kembali ke posisi tengah
  resetAllServos();
}