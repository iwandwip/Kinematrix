#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"

SDCardModuleESP32 sdCard(5); // CS pin pada GPIO5

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module Basic Example");
  
  // Inisialisasi kartu SD
  if (sdCard.begin()) {
    Serial.println("SD Card initialized successfully!");
    
    // Periksa tipe kartu
    Serial.print("Card Type: ");
    Serial.println(sdCard.cardTypeString());
    
    // Periksa ukuran kartu
    uint64_t cardSize = sdCard.cardSize() / (1024 * 1024);
    Serial.print("SD Card Size: ");
    Serial.print(cardSize);
    Serial.println(" MB");
    
    // Tulis file dasar
    const char *path = "/hello.txt";
    const char *message = "Hello SD Card from Kinematrix!";
    
    if (sdCard.writeFile(path, message)) {
      Serial.println("File written successfully");
      
      // Baca file
      Serial.println("Reading file content:");
      Serial.println(sdCard.readFileString(path));
      
      // Hapus file
      if (sdCard.deleteFile(path)) {
        Serial.println("File deleted successfully");
      } else {
        Serial.println("Error deleting file");
      }
    } else {
      Serial.println("Error writing file");
    }
  } else {
    Serial.println("SD Card initialization failed!");
    Serial.println("Check if the card is inserted correctly");
    Serial.println("Check if the connection pins are correct");
  }
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
  delay(1000);
}
