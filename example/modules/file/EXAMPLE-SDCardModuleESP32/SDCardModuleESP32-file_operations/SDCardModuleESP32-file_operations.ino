#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"

SDCardModuleESP32 sdCard(5); // CS pin pada GPIO5

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module File Operations Example");
  
  // Inisialisasi kartu SD
  if (!sdCard.begin()) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  
  Serial.println("SD Card initialized successfully!");
  
  // 1. Menulis file baru
  const char *filePath = "/data.txt";
  
  Serial.println("Writing to new file...");
  if (sdCard.writeFile(filePath, "Line 1: This is a test file.\n")) {
    Serial.println("File created and written successfully");
  } else {
    Serial.println("Error writing file");
    return;
  }
  
  // 2. Menambahkan data ke file
  Serial.println("Appending to file...");
  if (sdCard.appendFile(filePath, "Line 2: More data appended to file.\n")) {
    Serial.println("Data appended successfully");
  } else {
    Serial.println("Error appending to file");
  }
  
  // Append beberapa baris lagi
  sdCard.appendFile(filePath, "Line 3: Third line of text.\n");
  sdCard.appendFile(filePath, "Line 4: Fourth line of text.\n");
  
  // 3. Membaca file
  Serial.println("Reading file content:");
  Serial.println("--------- File Content Start ---------");
  sdCard.readFile(filePath);
  Serial.println("--------- File Content End ---------");
  
  // 4. Mendapatkan ukuran file
  size_t fileSize = sdCard.fileSize(filePath);
  Serial.print("File size: ");
  Serial.print(fileSize);
  Serial.println(" bytes");
  
  // 5. Mengganti nama file
  const char *newFilePath = "/renamed_data.txt";
  Serial.println("Renaming file...");
  if (sdCard.renameFile(filePath, newFilePath)) {
    Serial.print("File renamed from ");
    Serial.print(filePath);
    Serial.print(" to ");
    Serial.println(newFilePath);
  } else {
    Serial.println("Error renaming file");
  }
  
  // 6. Memeriksa file ada
  if (sdCard.exists(newFilePath)) {
    Serial.println("The renamed file exists!");
  } else {
    Serial.println("Error: Renamed file not found");
  }
  
  // 7. Menulis file baru lagi untuk demo menghapus
  const char *tempFile = "/temp.txt";
  sdCard.writeFile(tempFile, "This is a temporary file that will be deleted.\n");
  
  // 8. Menghapus file
  Serial.println("Deleting temporary file...");
  if (sdCard.deleteFile(tempFile)) {
    Serial.print("File ");
    Serial.print(tempFile);
    Serial.println(" deleted successfully");
  } else {
    Serial.println("Error deleting file");
  }
  
  // 9. Memastikan file sudah terhapus
  if (!sdCard.exists(tempFile)) {
    Serial.println("Confirmed: Temporary file was deleted");
  } else {
    Serial.println("Error: Temporary file still exists");
  }
  
  Serial.println("File operations example completed");
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
  delay(1000);
}
