#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"

SDCardModuleESP32 sdCard(5); // CS pin pada GPIO5

// Konfigurasi untuk pengujian performa
const char* testFileName = "/performance_test.bin";
const size_t testFileSizes[] = {1024, 10240, 102400, 1048576}; // 1KB, 10KB, 100KB, 1MB
const size_t bufferSizes[] = {64, 128, 256, 512, 1024, 4096}; // 64B - 4KB
const int testRuns = 3; // Jumlah pengulangan untuk setiap tes

// Struktur untuk menyimpan hasil
struct TestResult {
  size_t fileSize;
  size_t bufferSize;
  float writeSpeedKBps[testRuns];
  float readSpeedKBps[testRuns];
  float avgWriteSpeedKBps;
  float avgReadSpeedKBps;
};

// Array untuk hasil
TestResult results[sizeof(testFileSizes)/sizeof(testFileSizes[0]) * sizeof(bufferSizes)/sizeof(bufferSizes[0])];
int resultCount = 0;

// Alokasi buffer
uint8_t* buffer = NULL;
size_t maxBufferSize = 4096; // Ukuran maksimum buffer yang akan digunakan

// Fungsi untuk menguji write
float testWrite(const char* path, size_t fileSize, size_t bufferSize, uint8_t* buf) {
  // Isi buffer dengan data unik
  for (size_t i = 0; i < bufferSize; i++) {
    buf[i] = i & 0xFF;
  }
  
  // Buka file untuk menulis
  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return 0;
  }
  
  unsigned long startTime = millis();
  
  // Tulis data ke file
  size_t bytesWritten = 0;
  while (bytesWritten < fileSize) {
    size_t toWrite = min(bufferSize, fileSize - bytesWritten);
    size_t written = file.write(buf, toWrite);
    
    if (written != toWrite) {
      Serial.println("Write error");
      file.close();
      return 0;
    }
    
    bytesWritten += written;
  }
  
  file.flush(); // Memastikan semua data ditulis ke kartu
  file.close();
  
  unsigned long endTime = millis();
  float elapsedSec = (endTime - startTime) / 1000.0;
  float speedKBps = (fileSize / 1024.0) / elapsedSec;
  
  return speedKBps;
}

// Fungsi untuk menguji read
float testRead(const char* path, size_t fileSize, size_t bufferSize, uint8_t* buf) {
  // Buka file untuk membaca
  File file = SD.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return 0;
  }
  
  unsigned long startTime = millis();
  
  // Baca data dari file
  size_t bytesRead = 0;
  while (bytesRead < fileSize) {
    size_t toRead = min(bufferSize, fileSize - bytesRead);
    size_t read = file.read(buf, toRead);
    
    if (read != toRead) {
      if (file.available()) {
        Serial.println("Read error");
        file.close();
        return 0;
      } else {
        // EOF
        break;
      }
    }
    
    bytesRead += read;
  }
  
  file.close();
  
  unsigned long endTime = millis();
  float elapsedSec = (endTime - startTime) / 1000.0;
  float speedKBps = (fileSize / 1024.0) / elapsedSec;
  
  return speedKBps;
}

// Fungsi untuk menampilkan hasil
void printResults() {
  Serial.println("\n------ Performance Test Results ------");
  Serial.println("File Size | Buffer Size | Avg Write (KB/s) | Avg Read (KB/s)");
  Serial.println("-----------------------------------------------------");
  
  for (int i = 0; i < resultCount; i++) {
    char fileSizeStr[10];
    char bufferSizeStr[10];
    
    // Format ukuran file
    if (results[i].fileSize >= 1048576) {
      sprintf(fileSizeStr, "%.2f MB", results[i].fileSize / 1048576.0);
    } else if (results[i].fileSize >= 1024) {
      sprintf(fileSizeStr, "%.2f KB", results[i].fileSize / 1024.0);
    } else {
      sprintf(fileSizeStr, "%d B", (int)results[i].fileSize);
    }
    
    // Format ukuran buffer
    if (results[i].bufferSize >= 1024) {
      sprintf(bufferSizeStr, "%.2f KB", results[i].bufferSize / 1024.0);
    } else {
      sprintf(bufferSizeStr, "%d B", (int)results[i].bufferSize);
    }
    
    Serial.printf("%-9s | %-11s | %-15.2f | %-14.2f\n",
      fileSizeStr, bufferSizeStr, 
      results[i].avgWriteSpeedKBps, results[i].avgReadSpeedKBps);
  }
  
  Serial.println("-----------------------------------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module Performance Test");
  
  // Inisialisasi kartu SD
  if (!sdCard.begin()) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  
  Serial.println("SD Card initialized successfully!");
  
  // Alokasi buffer
  buffer = (uint8_t*)malloc(maxBufferSize);
  if (!buffer) {
    Serial.println("Failed to allocate buffer");
    return;
  }
  
  // Tampilkan informasi kartu
  Serial.print("Card Type: ");
  Serial.println(sdCard.cardTypeString());
  
  Serial.print("Card Size: ");
  Serial.print(sdCard.cardSize() / (1024 * 1024));
  Serial.println(" MB");
  
  Serial.println("\nStarting performance tests...");
  Serial.println("This may take several minutes depending on the card speed.");
  
  // Jalankan tes untuk setiap kombinasi ukuran file dan buffer
  for (size_t i = 0; i < sizeof(testFileSizes) / sizeof(testFileSizes[0]); i++) {
    for (size_t j = 0; j < sizeof(bufferSizes) / sizeof(bufferSizes[0]); j++) {
      size_t fileSize = testFileSizes[i];
      size_t bufferSize = bufferSizes[j];
      
      // Pastikan buffer cukup besar
      if (bufferSize > maxBufferSize) {
        continue;
      }
      
      // Tampilkan info tes
      Serial.print("\nTesting with file size: ");
      if (fileSize >= 1048576) {
        Serial.print(fileSize / 1048576.0);
        Serial.print(" MB");
      } else if (fileSize >= 1024) {
        Serial.print(fileSize / 1024.0);
        Serial.print(" KB");
      } else {
        Serial.print(fileSize);
        Serial.print(" B");
      }
      
      Serial.print(", buffer size: ");
      if (bufferSize >= 1024) {
        Serial.print(bufferSize / 1024.0);
        Serial.println(" KB");
      } else {
        Serial.print(bufferSize);
        Serial.println(" B");
      }
      
      results[resultCount].fileSize = fileSize;
      results[resultCount].bufferSize = bufferSize;
      
      float totalWriteSpeed = 0;
      float totalReadSpeed = 0;
      
      // Lakukan beberapa kali pengulangan
      for (int run = 0; run < testRuns; run++) {
        Serial.print("  Run ");
        Serial.print(run + 1);
        Serial.print(": ");
        
        // Tes menulis
        float writeSpeed = testWrite(testFileName, fileSize, bufferSize, buffer);
        results[resultCount].writeSpeedKBps[run] = writeSpeed;
        totalWriteSpeed += writeSpeed;
        
        Serial.print("Write: ");
        Serial.print(writeSpeed);
        Serial.print(" KB/s, ");
        
        // Tes membaca
        float readSpeed = testRead(testFileName, fileSize, bufferSize, buffer);
        results[resultCount].readSpeedKBps[run] = readSpeed;
        totalReadSpeed += readSpeed;
        
        Serial.print("Read: ");
        Serial.print(readSpeed);
        Serial.println(" KB/s");
        
        // Hapus file setelah satu run
        if (run < testRuns - 1) {
          sdCard.deleteFile(testFileName);
        }
      }
      
      // Hitung rata-rata
      results[resultCount].avgWriteSpeedKBps = totalWriteSpeed / testRuns;
      results[resultCount].avgReadSpeedKBps = totalReadSpeed / testRuns;
      
      Serial.print("  Average: Write: ");
      Serial.print(results[resultCount].avgWriteSpeedKBps);
      Serial.print(" KB/s, Read: ");
      Serial.print(results[resultCount].avgReadSpeedKBps);
      Serial.println(" KB/s");
      
      resultCount++;
    }
  }
  
  // Hapus file tes
  sdCard.deleteFile(testFileName);
  
  // Tampilkan hasil
  printResults();
  
  // Bersihkan
  free(buffer);
  
  Serial.println("\nPerformance test completed");
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
  delay(1000);
}
