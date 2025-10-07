/*
 * DecisionTree_Soil_Classification.ino
 * 
 * Aplikasi klasifikasi jenis tanah menggunakan sensor kelembaban dan pH
 * untuk rekomendasi pertanian
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

// Definisi pin sensor (dalam implementasi nyata)
// #define MOISTURE_PIN A0
// #define PH_PIN A1

// Fungsi untuk membaca sensor (simulasi)
float readMoisture() {
  // Simulasi pembacaan sensor kelembaban (0-100%)
  return random(0, 100);
}

float readPH() {
  // Simulasi pembacaan sensor pH (4.0-8.0)
  return 4.0 + (random(0, 400) / 100.0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDecision Tree - Soil Classification Example");
  
  // Inisialisasi random seed
  randomSeed(analogRead(0));
  
  // Membuat Decision Tree dengan 2 fitur: kelembaban dan pH
  DecisionTree soilTree(2, 30);
  
  // Data training berdasarkan jenis tanah
  // Format: [kelembaban, pH] -> jenis tanah
  // Data ini adalah contoh dan tidak merepresentasikan data sebenarnya
  
  // Tanah berpasir (sandy)
  soilTree.addTrainingData("sandy", (float[]) {10.0, 5.5});
  soilTree.addTrainingData("sandy", (float[]) {12.0, 5.8});
  soilTree.addTrainingData("sandy", (float[]) {8.0, 6.0});
  soilTree.addTrainingData("sandy", (float[]) {15.0, 5.7});
  soilTree.addTrainingData("sandy", (float[]) {11.0, 6.2});

  // Tanah liat (clay)
  soilTree.addTrainingData("clay", (float[]) {35.0, 6.5});
  soilTree.addTrainingData("clay", (float[]) {40.0, 6.8});
  soilTree.addTrainingData("clay", (float[]) {38.0, 7.0});
  soilTree.addTrainingData("clay", (float[]) {42.0, 6.7});
  soilTree.addTrainingData("clay", (float[]) {45.0, 7.2});

  // Tanah lempung (loam)
  soilTree.addTrainingData("loam", (float[]) {25.0, 6.2});
  soilTree.addTrainingData("loam", (float[]) {28.0, 6.4});
  soilTree.addTrainingData("loam", (float[]) {22.0, 6.6});
  soilTree.addTrainingData("loam", (float[]) {30.0, 6.3});
  soilTree.addTrainingData("loam", (float[]) {26.0, 6.5});
  
  // Tanah asam (acidic)
  soilTree.addTrainingData("acidic", (float[]) {20.0, 4.5});
  soilTree.addTrainingData("acidic", (float[]) {18.0, 4.8});
  soilTree.addTrainingData("acidic", (float[]) {22.0, 4.2});
  soilTree.addTrainingData("acidic", (float[]) {25.0, 4.6});
  soilTree.addTrainingData("acidic", (float[]) {28.0, 4.3});
  
  // Tanah basa (alkaline)
  soilTree.addTrainingData("alkaline", (float[]) {15.0, 7.8});
  soilTree.addTrainingData("alkaline", (float[]) {20.0, 8.0});
  soilTree.addTrainingData("alkaline", (float[]) {18.0, 7.6});
  soilTree.addTrainingData("alkaline", (float[]) {22.0, 7.9});
  soilTree.addTrainingData("alkaline", (float[]) {25.0, 7.5});
  
  // Melatih model
  Serial.println("Melatih model klasifikasi tanah...");
  if (!soilTree.train(GINI)) {
    Serial.print("Gagal melatih model: ");
    Serial.println(soilTree.getErrorMessage());
    return;
  }
  
  // Evaluasi model dengan cross-validation
  float accuracy = soilTree.crossValidate(3);
  Serial.print("Akurasi model: ");
  Serial.print(accuracy * 100.0);
  Serial.println("%");
  
  // Tabel rekomendasi tanaman berdasarkan jenis tanah
  Serial.println("\nRekomendasi tanaman berdasarkan jenis tanah:");
  Serial.println("--------------------------------------------");
  Serial.println("Tanah berpasir (sandy): Wortel, Ketimun, Radish");
  Serial.println("Tanah liat (clay): Kubis, Brokoli, Kacang polong");
  Serial.println("Tanah lempung (loam): Tomat, Bayam, Selada");
  Serial.println("Tanah asam (acidic): Blueberry, Kentang, Stroberi");
  Serial.println("Tanah basa (alkaline): Asparagus, Kembang kol, Bawang");
  
  // Simulasi klasifikasi tanah dengan beberapa pembacaan
  Serial.println("\nSimulasi klasifikasi tanah dari pembacaan sensor:");
  Serial.println("------------------------------------------------");
  
  for (int i = 0; i < 5; i++) {
    // Baca sensor (simulasi)
    float moisture = readMoisture();
    float ph = readPH();
    
    // Prediksi jenis tanah
    float features[] = {moisture, ph};
    const char* soilType = soilTree.predict(features);
    
    // Tampilkan hasil
    Serial.print("Pembacaan #");
    Serial.print(i+1);
    Serial.print(": Kelembaban = ");
    Serial.print(moisture);
    Serial.print("%, pH = ");
    Serial.print(ph);
    Serial.print(" => Jenis tanah: ");
    Serial.print(soilType);
    
    // Berikan rekomendasi tanaman
    Serial.print(" => Rekomendasi tanaman: ");
    if (strcmp(soilType, "sandy") == 0) {
      Serial.println("Wortel, Ketimun, Radish");
    } else if (strcmp(soilType, "clay") == 0) {
      Serial.println("Kubis, Brokoli, Kacang polong");
    } else if (strcmp(soilType, "loam") == 0) {
      Serial.println("Tomat, Bayam, Selada");
    } else if (strcmp(soilType, "acidic") == 0) {
      Serial.println("Blueberry, Kentang, Stroberi");
    } else if (strcmp(soilType, "alkaline") == 0) {
      Serial.println("Asparagus, Kembang kol, Bawang");
    } else {
      Serial.println("Tidak ada rekomendasi");
    }
    
    delay(1000);
  }
  
  // Menghitung rekomendasi pengkondisian tanah
  Serial.println("\nRekomendasi pengkondisian tanah:");
  Serial.println("--------------------------------");
  
  // Simulasi beberapa pembacaan dan berikan rekomendasi penyesuaian
  for (int i = 0; i < 3; i++) {
    float moisture = readMoisture();
    float ph = readPH();
    
    Serial.print("Tanah #");
    Serial.print(i+1);
    Serial.print(": Kelembaban = ");
    Serial.print(moisture);
    Serial.print("%, pH = ");
    Serial.println(ph);
    
    // Rekomendasi kelembaban
    Serial.print("  - Kelembaban: ");
    if (moisture < 15) {
      Serial.println("Terlalu kering. Tambahkan air dan bahan organik untuk meningkatkan retensi air.");
    } else if (moisture > 35) {
      Serial.println("Terlalu basah. Perbaiki drainase atau tambahkan pasir untuk meningkatkan aliran air.");
    } else {
      Serial.println("Kelembaban optimal.");
    }
    
    // Rekomendasi pH
    Serial.print("  - pH: ");
    if (ph < 5.5) {
      Serial.println("Terlalu asam. Tambahkan kapur untuk meningkatkan pH.");
    } else if (ph > 7.5) {
      Serial.println("Terlalu basa. Tambahkan sulfur atau bahan organik untuk menurunkan pH.");
    } else {
      Serial.println("pH optimal untuk kebanyakan tanaman.");
    }
    
    delay(1000);
  }
}

void loop() {
  // Dalam aplikasi nyata, pembacaan sensor dan klasifikasi tanah
  // dapat dilakukan secara periodik
  delay(5000);
  
  // Baca sensor
  float moisture = readMoisture();
  float ph = readPH();
  
  Serial.print("\nPembacaan realtime: Kelembaban = ");
  Serial.print(moisture);
  Serial.print("%, pH = ");
  Serial.println(ph);
  
  // Buat model sederhana untuk loop
  DecisionTree quickSoilModel(2, 15);
  
  // Data training minimal
  quickSoilModel.addTrainingData("sandy", (float[]) {10.0, 5.5});
  quickSoilModel.addTrainingData("clay", (float[]) {40.0, 6.8});
  quickSoilModel.addTrainingData("loam", (float[]) {25.0, 6.3});
  quickSoilModel.addTrainingData("acidic", (float[]) {20.0, 4.5});
  quickSoilModel.addTrainingData("alkaline", (float[]) {20.0, 7.8});
  
  // Latih dan prediksi
  if (quickSoilModel.train()) {
    float features[] = {moisture, ph};
    const char* soilType = quickSoilModel.predict(features);
    
    Serial.print("Jenis tanah: ");
    Serial.println(soilType);
    
    // Tampilkan rekomendasi sederhana
    Serial.println("Penyesuaian yang diperlukan:");
    
    // Kelembaban
    if (moisture < 15) {
      Serial.println("- Perlu lebih banyak air");
    } else if (moisture > 35) {
      Serial.println("- Perlu drainase lebih baik");
    }
    
    // pH
    if (ph < 5.5) {
      Serial.println("- Perlu meningkatkan pH (tambahkan kapur)");
    } else if (ph > 7.5) {
      Serial.println("- Perlu menurunkan pH (tambahkan sulfur)");
    }
  }
}
