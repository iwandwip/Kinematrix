/*
 * Pan_Tilt_Camera_Control.ino
 * 
 * Implementasi sistem pan-tilt untuk kamera menggunakan joystick analog dengan PCA9685 dan ESP32
 * 
 * Koneksi Hardware:
 * ESP32:
 * - GPIO21 -> SDA PCA9685
 * - GPIO22 -> SCL PCA9685
 * - 3.3V -> VCC PCA9685
 * - GND -> GND PCA9685
 * - GPIO34 -> VRx Joystick (analog X)
 * - GPIO35 -> VRy Joystick (analog Y)
 * - 3.3V -> +5V Joystick
 * - GND -> GND Joystick
 * 
 * Servo:
 * - Servo Pan (horizontal) terhubung ke channel 0 pada PCA9685
 * - Servo Tilt (vertikal) terhubung ke channel 1 pada PCA9685
 */

#define ENABLE_MODULE_SERVO_HCPA9685
#include "Kinematrix.h"
#include <Wire.h>

// Definisi pin I2C ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Alamat I2C PCA9685 (default 0x40)
#define I2C_ADDRESS 0x40

// Pin joystick
#define JOYSTICK_X 34  // Pin untuk pembacaan X
#define JOYSTICK_Y 35  // Pin untuk pembacaan Y

// Konstanta servo
#define PAN_SERVO 0   // Channel servo pan (horizontal)
#define TILT_SERVO 1  // Channel servo tilt (vertikal)

// Rentang gerakan servo
#define PAN_MIN 100
#define PAN_MAX 400
#define TILT_MIN 120
#define TILT_MAX 350

// Penyimpanan posisi servo
int panPosition = 240;   // Posisi awal pan (tengah)
int tiltPosition = 240;  // Posisi awal tilt (tengah)

// Variabel untuk kontrol melalui serial
bool serialControl = false;

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
  
  // Konfigurasi pin joystick sebagai input
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  
  // Set posisi awal servo
  servoDriver.Servo(PAN_SERVO, panPosition);
  servoDriver.Servo(TILT_SERVO, tiltPosition);
  
  Serial.println("Sistem pan-tilt siap");
  Serial.println("Tekan 's' untuk beralih antara kontrol joystick dan serial");
  Serial.println("Ketika dalam mode serial:");
  Serial.println("  'a'/'d' - Pan left/right");
  Serial.println("  'w'/'s' - Tilt up/down");
  Serial.println("  'r' - Reset ke posisi tengah");
}

void loop() {
  // Cek apakah ada input dari serial
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == 's') {
      // Toggle mode kontrol
      serialControl = !serialControl;
      Serial.print("Mode kontrol: ");
      Serial.println(serialControl ? "Serial" : "Joystick");
    }
    
    if (serialControl) {
      processSerialCommand(command);
    }
  }
  
  // Jika dalam mode joystick, baca input dari joystick
  if (!serialControl) {
    controlWithJoystick();
  }
  
  // Terapkan posisi baru ke servo
  servoDriver.Servo(PAN_SERVO, panPosition);
  servoDriver.Servo(TILT_SERVO, tiltPosition);
  
  // Sedikit delay untuk stabilitas
  delay(20);
}

// Fungsi untuk kontrol dengan joystick
void controlWithJoystick() {
  // Baca nilai joystick
  int xValue = analogRead(JOYSTICK_X);
  int yValue = analogRead(JOYSTICK_Y);
  
  // Konversi pembacaan analog (0-4095) ke arah gerakan (-5 hingga 5)
  int panDelta = map(xValue, 0, 4095, -5, 5);
  int tiltDelta = map(yValue, 0, 4095, -5, 5);
  
  // Terapkan zona mati (dead zone) untuk mencegah drift
  if (abs(panDelta) < 2) panDelta = 0;
  if (abs(tiltDelta) < 2) tiltDelta = 0;
  
  // Update posisi pan
  panPosition += panDelta;
  
  // Update posisi tilt
  tiltPosition -= tiltDelta;  // Inversi untuk arah yang intuitif
  
  // Batasi nilai ke rentang yang aman
  constrainPositions();
  
  // Debug output (hanya jika ada perubahan)
  if (panDelta != 0 || tiltDelta != 0) {
    printPositions();
  }
}

// Fungsi untuk memproses perintah serial
void processSerialCommand(char command) {
  int panStep = 10;
  int tiltStep = 10;
  
  switch (command) {
    case 'a':  // Pan left
      panPosition -= panStep;
      break;
    case 'd':  // Pan right
      panPosition += panStep;
      break;
    case 'w':  // Tilt up
      tiltPosition += tiltStep;
      break;
    case 's':  // Tilt down
      tiltPosition -= tiltStep;
      break;
    case 'r':  // Reset position
      panPosition = 240;
      tiltPosition = 240;
      break;
  }
  
  // Batasi nilai ke rentang yang aman
  constrainPositions();
  
  // Output posisi saat ini
  if (command == 'a' || command == 'd' || command == 'w' || command == 's' || command == 'r') {
    printPositions();
  }
}

// Fungsi untuk membatasi posisi dalam rentang yang aman
void constrainPositions() {
  // Batasi nilai pan ke rentang yang aman
  if (panPosition < PAN_MIN) panPosition = PAN_MIN;
  if (panPosition > PAN_MAX) panPosition = PAN_MAX;
  
  // Batasi nilai tilt ke rentang yang aman
  if (tiltPosition < TILT_MIN) tiltPosition = TILT_MIN;
  if (tiltPosition > TILT_MAX) tiltPosition = TILT_MAX;
}

// Fungsi untuk menampilkan posisi saat ini
void printPositions() {
  Serial.print("Pan: ");
  Serial.print(panPosition);
  Serial.print(" | Tilt: ");
  Serial.println(tiltPosition);
}