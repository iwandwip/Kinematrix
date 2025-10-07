/*
 * KNN Data Normalization Example
 * 
 * Menunjukkan efek dari normalisasi data pada fitur dengan skala yang berbeda
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Data training dengan fitur-fitur yang memiliki skala sangat berbeda
// Format: {fitur_kecil (-1 sampai 1), fitur_besar (0 sampai 1000)}
float class_a_data[][2] = {
  {0.2, 800.0},
  {0.3, 820.0},
  {0.1, 780.0},
  {0.25, 805.0}
};

float class_b_data[][2] = {
  {-0.2, 200.0},
  {-0.3, 220.0},
  {-0.1, 180.0},
  {-0.25, 210.0}
};

float class_c_data[][2] = {
  {0.8, 600.0},
  {0.7, 580.0},
  {0.9, 620.0},
  {0.75, 590.0}
};

// Data pengujian
float test_samples[][2] = {
  {0.22, 790.0},  // Seharusnya kelas A
  {-0.15, 200.0}, // Seharusnya kelas B
  {0.82, 605.0}   // Seharusnya kelas C
};

// Parameter KNN
const int K = 3;
const int MAX_FEATURES = 2;
const int MAX_SAMPLES = 20;

// Dua model: satu dengan normalisasi, satu tanpa
KNN knnWithoutNorm(K, MAX_FEATURES, MAX_SAMPLES);
KNN knnWithNorm(K, MAX_FEATURES, MAX_SAMPLES);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("KNN Data Normalization Example");
  Serial.println("-----------------------------");
  
  // Aktifkan normalisasi untuk model kedua
  knnWithNorm.enableNormalization(true);
  
  // Tambahkan data training ke kedua model
  addTrainingData();
  
  // Bandingkan prediksi dengan dan tanpa normalisasi
  comparePredictions();
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

void addTrainingData() {
  Serial.println("Adding training data to both models...");
  
  // Tambahkan data untuk kelas A
  for (int i = 0; i < 4; i++) {
    knnWithoutNorm.addTrainingData("A", class_a_data[i]);
    knnWithNorm.addTrainingData("A", class_a_data[i]);
  }
  
  // Tambahkan data untuk kelas B
  for (int i = 0; i < 4; i++) {
    knnWithoutNorm.addTrainingData("B", class_b_data[i]);
    knnWithNorm.addTrainingData("B", class_b_data[i]);
  }
  
  // Tambahkan data untuk kelas C
  for (int i = 0; i < 4; i++) {
    knnWithoutNorm.addTrainingData("C", class_c_data[i]);
    knnWithNorm.addTrainingData("C", class_c_data[i]);
  }
  
  Serial.print("Total training samples in each model: ");
  Serial.println(knnWithoutNorm.getDataCount());
  Serial.println();
}

void comparePredictions() {
  Serial.println("Comparing predictions with and without normalization:");
  Serial.println("--------------------------------------------------");
  
  for (int i = 0; i < 3; i++) {
    Serial.print("Sample ");
    Serial.print(i+1);
    Serial.print(": [");
    for (int j = 0; j < MAX_FEATURES; j++) {
      Serial.print(test_samples[i][j]);
      if (j < MAX_FEATURES - 1) Serial.print(", ");
    }
    Serial.println("]");
    
    // Prediksi tanpa normalisasi
    const char* withoutNormPrediction = knnWithoutNorm.predict(test_samples[i]);
    float withoutNormConfidence = knnWithoutNorm.getPredictionConfidence(test_samples[i]);
    
    // Prediksi dengan normalisasi
    const char* withNormPrediction = knnWithNorm.predict(test_samples[i]);
    float withNormConfidence = knnWithNorm.getPredictionConfidence(test_samples[i]);
    
    // Tampilkan hasil
    Serial.print("  Without Normalization: ");
    Serial.print(withoutNormPrediction);
    Serial.print(" (Confidence: ");
    Serial.print(withoutNormConfidence * 100);
    Serial.println("%)");
    
    Serial.print("  With Normalization: ");
    Serial.print(withNormPrediction);
    Serial.print(" (Confidence: ");
    Serial.print(withNormConfidence * 100);
    Serial.println("%)");
    
    // Dapatkan tetangga terdekat tanpa normalisasi
    int indices1[3];
    float distances1[3];
    knnWithoutNorm.getNearestNeighbors(test_samples[i], indices1, distances1, 3);
    
    // Dapatkan tetangga terdekat dengan normalisasi
    int indices2[3];
    float distances2[3];
    knnWithNorm.getNearestNeighbors(test_samples[i], indices2, distances2, 3);
    
    Serial.println("  3 nearest neighbors without normalization:");
    for (int j = 0; j < 3; j++) {
      Serial.print("    Distance: ");
      Serial.println(distances1[j]);
    }
    
    Serial.println("  3 nearest neighbors with normalization:");
    for (int j = 0; j < 3; j++) {
      Serial.print("    Distance: ");
      Serial.println(distances2[j]);
    }
    
    Serial.println();
  }
  
  Serial.println("EXPLANATION:");
  Serial.println("Without normalization, the second feature (0-1000 range) dominates");
  Serial.println("the distance calculation, essentially ignoring the first feature.");
  Serial.println("With normalization, both features contribute equally to the model.");
}