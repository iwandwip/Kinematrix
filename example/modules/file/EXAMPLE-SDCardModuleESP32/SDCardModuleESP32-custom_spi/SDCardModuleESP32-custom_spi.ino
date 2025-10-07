#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"

// Konfigurasi pin SPI custom
#define CUSTOM_SPI_SCK  18
#define CUSTOM_SPI_MISO 19
#define CUSTOM_SPI_MOSI 23
#define CUSTOM_SPI_CS   5

SDCardModuleESP32 sdCard(CUSTOM_SPI_CS); // Tentukan pin CS

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module Custom SPI Configuration Example");
  
  Serial.println("Using custom SPI pinout:");
  Serial.print("SCK:  GPIO"); Serial.println(CUSTOM_SPI_SCK);
  Serial.print("MISO: GPIO"); Serial.println(CUSTOM_SPI_MISO);
  Serial.print("MOSI: GPIO"); Serial.println(CUSTOM_SPI_MOSI);
  Serial.print("CS:   GPIO"); Serial.println(CUSTOM_SPI_CS);
  
  // Konfigurasi SPI custom dan inisialisasi kartu SD
  if (sdCard.setCustomSPI(CUSTOM_SPI_SCK, CUSTOM_SPI_MISO, CUSTOM_SPI_MOSI)) {
    Serial.println("SD Card initialized successfully with custom SPI pins!");
    
    // Menampilkan informasi kartu
    Serial.print("Card Type: ");
    Serial.println(sdCard.cardTypeString());
    
    Serial.print("Card Size: ");
    Serial.print(sdCard.cardSize() / (1024 * 1024));
    Serial.println(" MB");
    
    // Test file I/O dasar
    const char* testFile = "/custom_spi_test.txt";
    const char* testMessage = "This file was created using custom SPI pins on ESP32!\n";
    
    Serial.println("\nWriting test file...");
    if (sdCard.writeFile(testFile, testMessage)) {
      Serial.println("File written successfully");
      
      Serial.println("\nReading test file:");
      Serial.println("--------- File Content Start ---------");
      sdCard.readFile(testFile);
      Serial.println("--------- File Content End ---------");
      
      // List direktori root untuk verifikasi
      Serial.println("\nListing root directory:");
      sdCard.listDir("/", 0);
      
      // Hapus file test
      Serial.println("\nDeleting test file...");
      if (sdCard.deleteFile(testFile)) {
        Serial.println("Test file deleted successfully");
      } else {
        Serial.println("Error deleting test file");
      }
    } else {
      Serial.println("Error writing test file");
    }
  } else {
    Serial.println("SD Card initialization failed with custom SPI pins!");
    Serial.println("Possible issues:");
    Serial.println("1. Pin connections are incorrect");
    Serial.println("2. SD card is not inserted properly");
    Serial.println("3. SD card is not compatible");
    Serial.println("4. Custom SPI pins are not compatible with your ESP32 board");
    
    // Coba inisialisasi dengan pin default
    Serial.println("\nTrying with default SPI pins...");
    if (sdCard.begin()) {
      Serial.println("SD Card initialized successfully with default SPI pins!");
      Serial.println("This indicates the issue is with the custom SPI configuration.");
    } else {
      Serial.println("SD Card initialization also failed with default SPI pins.");
      Serial.println("This indicates a general issue with the SD card or connections.");
    }
  }
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
  delay(1000);
}
