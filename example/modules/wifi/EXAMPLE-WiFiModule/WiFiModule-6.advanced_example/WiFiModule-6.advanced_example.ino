#define ENABLE_MODULE_WIFI_MODULE
#include "Kinematrix.h"
#include <WiFiClient.h>

WiFiModule wifi;
WiFiClient client;

// Domain dan path untuk HTTP request
const char* server = "example.com";
const char* path = "/";
const int port = 80;

// Variabel untuk pelacakan status
unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 60000; // 1 menit
bool httpRequestPending = false;
String httpResponse = "";

// LED Indicator pins
const int connectLed = 2;    // LED untuk status koneksi WiFi
const int activityLed = 4;   // LED untuk aktivitas HTTP

// WiFi Event Handler
void onWifiStatusChange(wifi_module_status_t status) {
  if (status == WIFI_MODULE_CONNECTED) {
    digitalWrite(connectLed, HIGH);
    Serial.println("WiFi terhubung dan siap.");
    Serial.print("  IP: ");
    Serial.println(wifi.getIP().toString());
  } else {
    digitalWrite(connectLed, LOW);
    
    // Reset state HTTP jika koneksi terputus
    if (httpRequestPending) {
      httpRequestPending = false;
      httpResponse = "";
      client.stop();
    }
  }
}

// Melakukan HTTP request
void performHttpRequest() {
  if (!wifi.checkConnection()) {
    Serial.println("Tidak dapat melakukan HTTP request: WiFi tidak terhubung");
    return;
  }
  
  Serial.print("Menghubungi ");
  Serial.println(server);
  
  if (client.connect(server, port)) {
    digitalWrite(activityLed, HIGH);
    httpRequestPending = true;
    
    // Kirim HTTP request
    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                "Host: " + server + "\r\n" +
                "Connection: close\r\n\r\n");
    
    lastRequestTime = millis();
    Serial.println("HTTP request berhasil dikirim");
  } else {
    Serial.println("Koneksi ke server gagal");
  }
}

// Proses respons HTTP
void processHttpResponse() {
  if (httpRequestPending && client.available()) {
    // Baca respons
    while (client.available()) {
      char c = client.read();
      httpResponse += c;
    }
  }
  
  // Cek apakah koneksi selesai
  if (httpRequestPending && !client.connected()) {
    digitalWrite(activityLed, LOW);
    httpRequestPending = false;
    
    Serial.println("Respons HTTP diterima:");
    Serial.println("----- Response Start -----");
    Serial.println(httpResponse);
    Serial.println("----- Response End -----");
    
    httpResponse = "";
    client.stop();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Setup LED indicators
  pinMode(connectLed, OUTPUT);
  pinMode(activityLed, OUTPUT);
  digitalWrite(connectLed, LOW);
  digitalWrite(activityLed, LOW);
  
  Serial.println("WiFiModule Advanced Example");
  
  // Inisialisasi WiFi dengan konfigurasi lanjutan
  wifi.begin();
  wifi.setAutoReconnect(true);
  wifi.setReconnectInterval(10000); // 10 detik
  wifi.onEvent(onWifiStatusChange);
  
  // Gunakan IP statis (opsional)
  IPAddress local_ip(192, 168, 1, 200);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  wifi.setStaticIP(local_ip, gateway, subnet);
  wifi.useStaticIP(true);
  
  // Koneksi ke jaringan WiFi
  Serial.println("Connecting to WiFi...");
  wifi.connect("your-ssid", "your-password");
}

void loop() {
  // Periksa koneksi WiFi & tangani reconnect jika diperlukan
  wifi.checkConnection();
  
  // Proses respons HTTP jika ada
  processHttpResponse();
  
  // Buat HTTP request periodik
  unsigned long currentTime = millis();
  if (wifi.getStatus() == WIFI_MODULE_CONNECTED && 
      !httpRequestPending && 
      currentTime - lastRequestTime > requestInterval) {
    performHttpRequest();
  }
  
  // Tampilkan status setiap 10 detik
  static unsigned long lastStatusTime = 0;
  if (currentTime - lastStatusTime > 10000) {
    lastStatusTime = currentTime;
    
    Serial.print("WiFi Status: ");
    Serial.println(wifi.getStatusString());
    
    if (wifi.getStatus() == WIFI_MODULE_CONNECTED) {
      Serial.print("Signal Strength: ");
      Serial.print(wifi.getRSSI());
      Serial.println(" dBm");
    }
  }
  
  delay(50); // Latensi rendah untuk respons yang lebih baik
}
