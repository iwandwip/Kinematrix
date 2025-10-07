/*
 * KNN Distance Metrics Example
 * 
 * Membandingkan berbagai metrik jarak untuk klasifikasi
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Dataset aktivitas fisik sederhana
// Format: {accelerometer_x, accelerometer_y, accelerometer_z}
float walking_data[][3] = {
  {0.1, 1.1, 0.2},
  {0.2, 1.0, 0.3},
  {0.0, 1.2, 0.1},
  {0.1, 0.9, 0.2}
};

float running_data[][3] = {
  {1.2, 1.8, 0.9},
  {1.1, 1.9, 0.8},
  {1.3, 2.0, 0.9},
  {1.0, 1.7, 0.7}
};

float idle_data[][3] = {
  {0.0, 0.1, 0.0},
  {0.1, 0.0, 0.1},
  {0.0, 0.0, 0.1},
  {0.1, 0.1, 0.0}
};

// Data pengujian
float test_samples[][3] = {
  {0.2, 1.0, 0.2},  // Walking
  {1.1, 1.8, 0.8},  // Running
  {0.1, 0.1, 0.1}   // Idle
};

// Parameter KNN
const int K = 3;
const int MAX_FEATURES = 3;
const int MAX_SAMPLES = 20;

// Kita akan membuat tiga model KNN dengan metrik jarak berbeda
KNN knnEuclidean(K, MAX_FEATURES, MAX_SAMPLES);
KNN knnManhattan(K, MAX_FEATURES, MAX_SAMPLES);
KNN knnCosine(K, MAX_FEATURES, MAX_SAMPLES);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("KNN Distance Metrics Comparison");
  Serial.println("------------------------------");
  
  // Konfigurasi metrik jarak
  knnEuclidean.setDistanceMetric(EUCLIDEAN);    // Default
  knnManhattan.setDistanceMetric(MANHATTAN);
  knnCosine.setDistanceMetric(COSINE);
  
  // Tambahkan data training ke semua model
  addTrainingData();
  
  // Bandingkan prediksi dengan berbagai metrik jarak
  comparePredictions();
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

void addTrainingData() {
  Serial.println("Adding training data to all models...");
  
  // Tambahkan data untuk aktivitas berjalan
  for (int i = 0; i < 4; i++) {
    knnEuclidean.addTrainingData("walking", walking_data[i]);
    knnManhattan.addTrainingData("walking", walking_data[i]);
    knnCosine.addTrainingData("walking", walking_data[i]);
  }
  
  // Tambahkan data untuk aktivitas berlari
  for (int i = 0; i < 4; i++) {
    knnEuclidean.addTrainingData("running", running_data[i]);
    knnManhattan.addTrainingData("running", running_data[i]);
    knnCosine.addTrainingData("running", running_data[i]);
  }
  
  // Tambahkan data untuk aktivitas diam
  for (int i = 0; i < 4; i++) {
    knnEuclidean.addTrainingData("idle", idle_data[i]);
    knnManhattan.addTrainingData("idle", idle_data[i]);
    knnCosine.addTrainingData("idle", idle_data[i]);
  }
  
  Serial.print("Total training samples in each model: ");
  Serial.println(knnEuclidean.getDataCount());
  Serial.println();
}

void comparePredictions() {
  Serial.println("Comparing predictions with different distance metrics:");
  Serial.println("------------------------------------------------------");
  
  for (int i = 0; i < 3; i++) {
    Serial.print("Sample ");
    Serial.print(i+1);
    Serial.print(": [");
    for (int j = 0; j < MAX_FEATURES; j++) {
      Serial.print(test_samples[i][j]);
      if (j < MAX_FEATURES - 1) Serial.print(", ");
    }
    Serial.println("]");
    
    // Prediksi dengan Euclidean distance
    const char* euclideanPrediction = knnEuclidean.predict(test_samples[i]);
    float euclideanConfidence = knnEuclidean.getPredictionConfidence(test_samples[i]);
    
    // Prediksi dengan Manhattan distance
    const char* manhattanPrediction = knnManhattan.predict(test_samples[i]);
    float manhattanConfidence = knnManhattan.getPredictionConfidence(test_samples[i]);
    
    // Prediksi dengan Cosine distance
    const char* cosinePrediction = knnCosine.predict(test_samples[i]);
    float cosineConfidence = knnCosine.getPredictionConfidence(test_samples[i]);
    
    // Tampilkan hasil
    Serial.print("  Euclidean: ");
    Serial.print(euclideanPrediction);
    Serial.print(" (Confidence: ");
    Serial.print(euclideanConfidence * 100);
    Serial.println("%)");
    
    Serial.print("  Manhattan: ");
    Serial.print(manhattanPrediction);
    Serial.print(" (Confidence: ");
    Serial.print(manhattanConfidence * 100);
    Serial.println("%)");
    
    Serial.print("  Cosine: ");
    Serial.print(cosinePrediction);
    Serial.print(" (Confidence: ");
    Serial.print(cosineConfidence * 100);
    Serial.println("%)");
    
    Serial.println();
  }
}