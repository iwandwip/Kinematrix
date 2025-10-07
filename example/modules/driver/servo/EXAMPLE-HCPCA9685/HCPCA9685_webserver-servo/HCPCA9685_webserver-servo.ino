/*
 * WebServer_Servo_Control.ino
 * 
 * Mengontrol servo melalui web server ESP32 dengan PCA9685
 * 
 * Koneksi Hardware:
 * ESP32:
 * - GPIO21 -> SDA PCA9685
 * - GPIO22 -> SCL PCA9685
 * - 3.3V -> VCC PCA9685
 * - GND -> GND PCA9685
 * 
 * Servo dihubungkan ke channel 0-3 pada PCA9685
 * 
 * Instruksi Penggunaan:
 * 1. Ubah SSID dan Password WiFi sesuai dengan jaringan Anda
 * 2. Upload kode ke ESP32
 * 3. Buka serial monitor untuk melihat IP address
 * 4. Buka browser dan akses IP address tersebut
 */

#define ENABLE_MODULE_SERVO_HCPA9685
#include "Kinematrix.h"
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>

// Definisi pin I2C ESP32
#define I2C_SDA 21
#define I2C_SCL 22

// Alamat I2C PCA9685 (default 0x40)
#define I2C_ADDRESS 0x40

// Konfigurasi WiFi - GANTI DENGAN SSID DAN PASSWORD ANDA
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

// Buat instance dari library
HCPCA9685 servoDriver(I2C_ADDRESS);
WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Konfigurasi pin I2C ESP32
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Inisialisasi driver dalam mode servo
  servoDriver.Init(SERVO_MODE);
  
  // Aktifkan device (keluarkan dari mode sleep)
  servoDriver.Sleep(false);
  
  // Hubungkan ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Setup route untuk web server
  server.on("/", handleRoot);
  server.on("/servo", HTTP_GET, handleServo);
  
  // Mulai web server
  server.begin();
  Serial.println("HTTP server dimulai");
  
  // Reset semua servo ke posisi tengah
  for (int i = 0; i < 4; i++) {
    servoDriver.Servo(i, 240);
  }
}

void loop() {
  server.handleClient();
}

// Handler untuk halaman root
void handleRoot() {
  String html = "<html><head><title>Servo Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin: 20px; }";
  html += ".slider { width: 300px; }";
  html += ".servo-control { margin: 20px; padding: 10px; border: 1px solid #ccc; border-radius: 5px; }";
  html += "h1 { color: #2c3e50; }";
  html += "h3 { color: #3498db; }";
  html += "</style>";
  html += "<script>";
  html += "function updateServo(channel, value) {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/servo?channel=' + channel + '&position=' + value, true);";
  html += "  xhr.send();";
  html += "  document.getElementById('pos'+channel).innerHTML = value;";
  html += "}";
  html += "</script>";
  html += "</head><body>";
  html += "<h1>ESP32 Servo Control</h1>";
  
  // Buat slider untuk 4 channel servo pertama
  for (int i = 0; i < 4; i++) {
    html += "<div class='servo-control'>";
    html += "<h3>Servo " + String(i) + "</h3>";
    html += "<input type='range' min='0' max='480' value='240' class='slider' id='servo" + String(i) + "' ";
    html += "oninput='updateServo(" + String(i) + ", this.value)'>";
    html += "<div>Position: <span id='pos" + String(i) + "'>240</span></div>";
    html += "</div>";
  }
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Handler untuk mengatur servo
void handleServo() {
  if (server.hasArg("channel") && server.hasArg("position")) {
    int channel = server.arg("channel").toInt();
    int position = server.arg("position").toInt();
    
    // Validasi input
    if (channel >= 0 && channel <= 15 && position >= 0 && position <= 480) {
      servoDriver.Servo(channel, position);
      server.send(200, "text/plain", "OK");
      
      Serial.print("Set servo ");
      Serial.print(channel);
      Serial.print(" to position ");
      Serial.println(position);
    } else {
      server.send(400, "text/plain", "Invalid parameters");
    }
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}