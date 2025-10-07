#define ENABLE_MODULE_SD_CARD_MODULE_ESP32
#include "Kinematrix.h"

SDCardModuleESP32 sdCard(5); // CS pin pada GPIO5

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("SD Card Module Directory Operations Example");
  
  // Inisialisasi kartu SD
  if (!sdCard.begin()) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  
  Serial.println("SD Card initialized successfully!");
  
  // 1. Melihat direktori root
  Serial.println("\nListing root directory contents:");
  if (!sdCard.listDir("/", 0)) {
    Serial.println("Error listing root directory");
  }
  
  // 2. Membuat direktori baru
  const char *dirPath = "/test_dir";
  Serial.print("\nCreating directory: ");
  Serial.println(dirPath);
  
  if (sdCard.createDir(dirPath)) {
    Serial.println("Directory created successfully");
  } else {
    Serial.println("Error creating directory");
  }
  
  // 3. Membuat beberapa file di direktori baru
  char filePath[64];
  for (int i = 1; i <= 3; i++) {
    snprintf(filePath, sizeof(filePath), "%s/file%d.txt", dirPath, i);
    
    char fileContent[64];
    snprintf(fileContent, sizeof(fileContent), "This is test file %d in directory %s\n", i, dirPath);
    
    if (sdCard.writeFile(filePath, fileContent)) {
      Serial.print("Created file: ");
      Serial.println(filePath);
    } else {
      Serial.print("Error creating file: ");
      Serial.println(filePath);
    }
  }
  
  // 4. Melihat isi direktori baru
  Serial.print("\nListing contents of directory: ");
  Serial.println(dirPath);
  if (!sdCard.listDir(dirPath, 0)) {
    Serial.println("Error listing directory");
  }
  
  // 5. Membuat sub-direktori
  const char *subDirPath = "/test_dir/sub_dir";
  Serial.print("\nCreating sub-directory: ");
  Serial.println(subDirPath);
  
  if (sdCard.createDir(subDirPath)) {
    Serial.println("Sub-directory created successfully");
    
    // Membuat file dalam sub-direktori
    snprintf(filePath, sizeof(filePath), "%s/subfile.txt", subDirPath);
    sdCard.writeFile(filePath, "This is a file in the sub-directory\n");
  } else {
    Serial.println("Error creating sub-directory");
  }
  
  // 6. Melihat struktur direktori dengan level
  Serial.println("\nListing directory structure with 1 level depth:");
  if (!sdCard.listDir("/", 1)) {
    Serial.println("Error listing directory");
  }
  
  // 7. Memeriksa apakah path adalah direktori
  Serial.print("\nChecking if ");
  Serial.print(dirPath);
  Serial.println(" is a directory:");
  
  if (sdCard.isDirectory(dirPath)) {
    Serial.println("Yes, it is a directory");
  } else {
    Serial.println("No, it is not a directory");
  }
  
  // 8. Menghapus sub-direktori (harus kosong)
  Serial.print("\nDeleting sub-directory: ");
  Serial.println(subDirPath);
  
  // Pertama, hapus file di dalam sub-direktori
  snprintf(filePath, sizeof(filePath), "%s/subfile.txt", subDirPath);
  if (sdCard.deleteFile(filePath)) {
    Serial.println("File in sub-directory deleted");
    
    // Kemudian hapus sub-direktori
    if (sdCard.removeDir(subDirPath)) {
      Serial.println("Sub-directory removed successfully");
    } else {
      Serial.println("Error removing sub-directory");
    }
  } else {
    Serial.println("Error deleting file in sub-directory");
  }
  
  // 9. Melihat direktori root setelah operasi
  Serial.println("\nListing root directory after operations:");
  if (!sdCard.listDir("/", 1)) {
    Serial.println("Error listing root directory");
  }
  
  Serial.println("\nDirectory operations example completed");
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop
  delay(1000);
}
