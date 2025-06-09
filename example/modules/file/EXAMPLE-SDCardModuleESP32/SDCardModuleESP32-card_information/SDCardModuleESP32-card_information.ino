#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"

SDCardModuleESP32 sdCard(5); // CS pin pada GPIO5

void printCardType(uint8_t type) {
  Serial.print("Card Type: ");
  switch (type) {
    case CARD_NONE:
      Serial.println("No SD card attached");
      break;
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SDSC");
      break;
    case CARD_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("UNKNOWN");
      break;
  }
}

void formatBytes(uint64_t bytes, char* str, size_t size) {
  if (bytes < 1024) {
    snprintf(str, size, "%llu bytes", bytes);
  } else if (bytes < (1024 * 1024)) {
    snprintf(str, size, "%.2f KB", bytes / 1024.0);
  } else if (bytes < (1024 * 1024 * 1024)) {
    snprintf(str, size, "%.2f MB", bytes / 1024.0 / 1024.0);
  } else {
    snprintf(str, size, "%.2f GB", bytes / 1024.0 / 1024.0 / 1024.0);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module Information Example");
  
  // Inisialisasi kartu SD
  if (!sdCard.begin()) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  
  Serial.println("SD Card initialized successfully!");
  
  // 1. Informasi tipe kartu
  Serial.println("\n--- Card Type Information ---");
  printCardType(sdCard.cardType());
  Serial.print("Card Type String: ");
  Serial.println(sdCard.cardTypeString());
  
  // 2. Informasi ukuran kartu
  Serial.println("\n--- Card Size Information ---");
  uint64_t cardSize = sdCard.cardSize();
  char sizeStr[20];
  
  formatBytes(cardSize, sizeStr, sizeof(sizeStr));
  Serial.print("Card Size: ");
  Serial.println(sizeStr);
  
  Serial.print("Card Size in Bytes: ");
  Serial.println(cardSize);
  
  Serial.print("Card Size in MB: ");
  Serial.println(cardSize / (1024 * 1024));
  
  // 3. Informasi penggunaan
  Serial.println("\n--- Card Usage Information ---");
  uint64_t totalBytes = sdCard.totalBytes();
  uint64_t usedBytes = sdCard.usedBytes();
  
  formatBytes(totalBytes, sizeStr, sizeof(sizeStr));
  Serial.print("Total Space: ");
  Serial.println(sizeStr);
  
  formatBytes(usedBytes, sizeStr, sizeof(sizeStr));
  Serial.print("Used Space: ");
  Serial.println(sizeStr);
  
  formatBytes(totalBytes - usedBytes, sizeStr, sizeof(sizeStr));
  Serial.print("Free Space: ");
  Serial.println(sizeStr);
  
  Serial.print("Used Percentage: ");
  Serial.print(sdCard.usedPercentage());
  Serial.println("%");
  
  // 4. Menampilkan kapasitas kartu dalam format visual
  Serial.println("\n--- Visual Capacity Indicator ---");
  int barWidth = 40;  // Lebar bar dalam karakter
  int filledWidth = (int)(sdCard.usedPercentage() * barWidth / 100);
  
  Serial.print("[");
  for (int i = 0; i < barWidth; i++) {
    if (i < filledWidth) {
      Serial.print("#");  // Bagian yang terisi
    } else {
      Serial.print("-");  // Bagian yang kosong
    }
  }
  Serial.print("] ");
  Serial.print(sdCard.usedPercentage());
  Serial.println("%");
  
  // 5. Membuat file test untuk menunjukkan perubahan penggunaan
  Serial.println("\n--- Creating test file to show usage change ---");
  const char *testFilePath = "/test_usage.bin";
  
  // Buat file dengan ukuran 1MB (1048576 bytes)
  const size_t fileSize = 1048576;
  const size_t bufferSize = 512;
  uint8_t buffer[bufferSize];
  
  // Isi buffer dengan data test
  for (size_t i = 0; i < bufferSize; i++) {
    buffer[i] = i & 0xFF;
  }
  
  File file = SD.open(testFilePath, FILE_WRITE);
  if (file) {
    Serial.print("Creating test file of size ");
    formatBytes(fileSize, sizeStr, sizeof(sizeStr));
    Serial.println(sizeStr);
    
    // Tulis buffer berulang kali hingga mencapai ukuran yang diinginkan
    size_t bytesWritten = 0;
    while (bytesWritten < fileSize) {
      file.write(buffer, bufferSize);
      bytesWritten += bufferSize;
      
      // Tampilkan progress
      if (bytesWritten % (bufferSize * 100) == 0) {
        Serial.print(".");
      }
    }
    
    file.close();
    Serial.println("\nTest file created successfully");
    
    // Tampilkan perubahan penggunaan
    Serial.println("\n--- Card Usage After File Creation ---");
    usedBytes = sdCard.usedBytes();
    
    formatBytes(usedBytes, sizeStr, sizeof(sizeStr));
    Serial.print("Used Space: ");
    Serial.println(sizeStr);
    
    formatBytes(totalBytes - usedBytes, sizeStr, sizeof(sizeStr));
    Serial.print("Free Space: ");
    Serial.println(sizeStr);
    
    Serial.print("Used Percentage: ");
    Serial.print(sdCard.usedPercentage());
    Serial.println("%");
    
    // Hapus file test
    Serial.println("\nDeleting test file...");
    if (sdCard.deleteFile(testFilePath)) {
      Serial.println("Test file deleted successfully");
    } else {
      Serial.println("Error deleting test file");
    }
  } else {
    Serial.println("Error creating test file");
  }
  
  Serial.println("\nCard information example completed");
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
  delay(1000);
}
