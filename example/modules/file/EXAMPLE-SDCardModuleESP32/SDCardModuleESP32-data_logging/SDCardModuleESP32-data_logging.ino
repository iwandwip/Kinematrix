#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"

SDCardModuleESP32 sdCard(5); // CS pin pada GPIO5

// Pin untuk sensor simulasi
const int sensorPin = 34; // ADC pin
const int ledPin = 2;     // Untuk indikasi log

// Konfigurasi logging
const char* logFileName = "/sensor_log.csv";
unsigned long logInterval = 5000; // Log setiap 5 detik
unsigned long lastLogTime = 0;
unsigned long logCount = 0;
bool sdCardAvailable = false;

// Buffer untuk format timestamp
char timeBuffer[30];

// Fungsi untuk mendapatkan timestamp
// Catatan: ESP32 tidak memiliki RTC internal kecuali jika ditambahkan.
// Ini hanya timestamp sejak booting.
String getTimestamp() {
  unsigned long currentMillis = millis();
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  seconds %= 60;
  minutes %= 60;
  
  snprintf(timeBuffer, sizeof(timeBuffer), "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(timeBuffer);
}

// Baca sensor (simulasi)
float readSensorValue() {
  // Baca nilai ADC dan konversi ke nilai temperatur simulasi (20-30°C)
  int adcValue = analogRead(sensorPin);
  float voltage = adcValue * (3.3 / 4095.0); // ESP32 ADC is 12-bit (0-4095)
  return 20.0 + (voltage * 3.0); // Simulasi temperatur
}

// Tulis data ke file log
bool logSensorData(float value) {
  if (!sdCardAvailable) {
    return false;
  }
  
  // Buat string data
  String timestamp = getTimestamp();
  String dataString = timestamp + "," + String(value, 2) + "\n";
  
  // Cek apakah file sudah ada
  bool fileExists = sdCard.exists(logFileName);
  
  // Jika file belum ada, buat file dan tambahkan header
  if (!fileExists) {
    String header = "Timestamp,Temperature\n";
    if (!sdCard.writeFile(logFileName, header.c_str())) {
      Serial.println("Error creating log file!");
      return false;
    }
  }
  
  // Tambahkan data ke file
  if (!sdCard.appendFile(logFileName, dataString.c_str())) {
    Serial.println("Error appending to log file!");
    return false;
  }
  
  return true;
}

void blinkLed() {
  digitalWrite(ledPin, HIGH);
  delay(50);
  digitalWrite(ledPin, LOW);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module Data Logging Example");
  
  // Setup pin
  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  // Inisialisasi kartu SD
  if (sdCard.begin()) {
    Serial.println("SD Card initialized successfully!");
    sdCardAvailable = true;
    
    // Cek apakah file log sudah ada
    if (sdCard.exists(logFileName)) {
      Serial.println("Existing log file found.");
      
      // Tampilkan beberapa data terakhir
      Serial.println("Last few lines of existing log:");
      String content = sdCard.readFileString(logFileName);
      int lastLines = 5; // Tampilkan 5 baris terakhir
      int found = 0;
      
      // Mulai dari akhir dan cari newline
      for (int i = content.length() - 1; i >= 0 && found < lastLines; i--) {
        if (content.charAt(i) == '\n') {
          found++;
          if (found == lastLines) {
            content = content.substring(i + 1);
            break;
          }
        }
      }
      
      Serial.println(content);
    } else {
      Serial.println("No existing log file. A new one will be created.");
    }
  } else {
    Serial.println("SD Card initialization failed!");
    Serial.println("Data logging will be disabled.");
  }
  
  Serial.println("Data logging will start in 5 seconds...");
  delay(5000);
  
  Serial.println("Logging started. Data format: Timestamp,Temperature");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Log data pada interval yang ditentukan
  if (currentMillis - lastLogTime >= logInterval) {
    lastLogTime = currentMillis;
    
    // Baca sensor
    float sensorValue = readSensorValue();
    
    // Tampilkan ke Serial
    String timestamp = getTimestamp();
    Serial.print(timestamp);
    Serial.print(", Temperature: ");
    Serial.print(sensorValue, 2);
    Serial.println(" °C");
    
    // Log ke SD Card
    if (sdCardAvailable) {
      if (logSensorData(sensorValue)) {
        blinkLed(); // Indikasi visual bahwa log berhasil
        logCount++;
        
        // Tampilkan statistik log setiap 10 entri
        if (logCount % 10 == 0) {
          Serial.print("Log entries: ");
          Serial.print(logCount);
          Serial.print(", Log file size: ");
          Serial.print(sdCard.fileSize(logFileName));
          Serial.println(" bytes");
          
          // Tampilkan persentase penggunaan kartu
          Serial.print("SD Card usage: ");
          Serial.print(sdCard.usedPercentage(), 1);
          Serial.println("%");
        }
      } else {
        Serial.println("Failed to log data!");
      }
    }
  }
  
  // Delay kecil untuk mencegah loop terlalu cepat
  delay(100);
}
