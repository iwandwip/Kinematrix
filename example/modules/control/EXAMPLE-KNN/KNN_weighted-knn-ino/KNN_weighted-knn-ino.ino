/*
 * KNN Weighted Voting Example
 * 
 * Membandingkan voting standar (majority) dengan voting berbobot (distance-weighted)
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Dataset prediksi harga rumah sederhana
// Format: {luas_tanah, jumlah_kamar, umur_bangunan}
float cheap_house_data[][3] = {
  {80, 1, 15},
  {85, 1, 12},
  {75, 2, 20},
  {90, 2, 18}
};

float medium_house_data[][3] = {
  {120, 2, 8},
  {130, 3, 10},
  {115, 2, 9},
  {125, 3, 7}
};

float expensive_house_data[][3] = {
  {200, 4, 3},
  {220, 5, 1},
  {180, 4, 2},
  {210, 5, 2}
};

// Data ambiguous untuk pengujian
// Data ini terdiri dari rumah-rumah borderline yang sulit diklasifikasikan
float test_samples[][3] = {
  {100, 2, 12},  // Antara cheap dan medium
  {170, 3, 5},   // Antara medium dan expensive
  {150, 3, 5}    // Lebih clarify antara medium dan expensive
};

// Parameter KNN
const int K = 5;
const int MAX_FEATURES = 3;
const int MAX_SAMPLES = 20;

// Dua model: satu dengan voting standard, satu dengan weighted voting
KNN knnStandard(K, MAX_FEATURES, MAX_SAMPLES);
KNN knnWeighted(K, MAX_FEATURES, MAX_SAMPLES);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("KNN Weighted Voting Example");
  Serial.println("--------------------------");
  
  // Aktifkan weighted voting untuk model kedua
  knnWeighted.setWeightedVoting(true);
  
  // Aktifkan normalisasi untuk kedua model
  knnStandard.enableNormalization(true);
  knnWeighted.enableNormalization(true);
  
  // Tambahkan data training ke kedua model
  addTrainingData();
  
  // Bandingkan prediksi standard vs weighted voting
  comparePredictions();
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

void addTrainingData() {
  Serial.println("Adding training data to both models...");
  
  // Tambahkan data untuk rumah murah
  for (int i = 0; i < 4; i++) {
    knnStandard.addTrainingData("cheap", cheap_house_data[i]);
    knnWeighted.addTrainingData("cheap", cheap_house_data[i]);
  }
  
  // Tambahkan data untuk rumah medium
  for (int i = 0; i < 4; i++) {
    knnStandard.addTrainingData("medium", medium_house_data[i]);
    knnWeighted.addTrainingData("medium", medium_house_data[i]);
  }
  
  // Tambahkan data untuk rumah mahal
  for (int i = 0; i < 4; i++) {
    knnStandard.addTrainingData("expensive", expensive_house_data[i]);
    knnWeighted.addTrainingData("expensive", expensive_house_data[i]);
  }
  
  Serial.print("Total training samples in each model: ");
  Serial.println(knnStandard.getDataCount());
  Serial.println();
}

void comparePredictions() {
  Serial.println("Comparing standard voting vs weighted voting:");
  Serial.println("-------------------------------------------");
  
  for (int i = 0; i < 3; i++) {
    Serial.print("Sample ");
    Serial.print(i+1);
    Serial.print(": [");
    for (int j = 0; j < MAX_FEATURES; j++) {
      Serial.print(test_samples[i][j]);
      if (j < MAX_FEATURES - 1) Serial.print(", ");
    }
    Serial.println("]");
    
    // Prediksi dengan standard voting
    const char* standardPrediction = knnStandard.predict(test_samples[i]);
    float standardConfidence = knnStandard.getPredictionConfidence(test_samples[i]);
    
    // Prediksi dengan weighted voting
    const char* weightedPrediction = knnWeighted.predict(test_samples[i]);
    float weightedConfidence = knnWeighted.getPredictionConfidence(test_samples[i]);
    
    // Tampilkan hasil
    Serial.print("  Standard Voting: ");
    Serial.print(standardPrediction);
    Serial.print(" (Confidence: ");
    Serial.print(standardConfidence * 100);
    Serial.println("%)");
    
    Serial.print("  Weighted Voting: ");
    Serial.print(weightedPrediction);
    Serial.print(" (Confidence: ");
    Serial.print(weightedConfidence * 100);
    Serial.println("%)");
    
    // Tampilkan tetangga terdekat untuk insight lebih dalam
    int indices[K];
    float distances[K];
    knnStandard.getNearestNeighbors(test_samples[i], indices, distances, K);
    
    Serial.println("  5 nearest neighbors:");
    for (int j = 0; j < K; j++) {
      int idx = indices[j];
      float invDistance = 1.0 / (distances[j] + 0.0001);
      
      Serial.print("    Neighbor ");
      Serial.print(j+1);
      Serial.print(": Distance = ");
      Serial.print(distances[j]);
      Serial.print(", Weight = ");
      Serial.print(invDistance);
      
      // Hitung data training dari index
      if (idx < 4) {
        Serial.println(" (cheap)");
      } else if (idx < 8) {
        Serial.println(" (medium)");
      } else {
        Serial.println(" (expensive)");
      }
    }
    
    Serial.println();
  }
}