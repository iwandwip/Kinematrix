/*
 * KNN Basic Usage Example
 * 
 * Demonstrasi penggunaan dasar library KNN untuk klasifikasi iris dataset
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Data iris sederhana (sepal length, sepal width, petal length, petal width)
// Format: {sepal_length, sepal_width, petal_length, petal_width}
float iris_setosa[][4] = {
  {5.1, 3.5, 1.4, 0.2},
  {4.9, 3.0, 1.4, 0.2},
  {4.7, 3.2, 1.3, 0.2},
  {5.0, 3.6, 1.4, 0.2}
};

float iris_versicolor[][4] = {
  {7.0, 3.2, 4.7, 1.4},
  {6.4, 3.2, 4.5, 1.5},
  {6.9, 3.1, 4.9, 1.5},
  {6.3, 2.5, 4.9, 1.5}
};

float iris_virginica[][4] = {
  {6.3, 3.3, 6.0, 2.5},
  {5.8, 2.7, 5.1, 1.9},
  {7.1, 3.0, 5.9, 2.1},
  {6.5, 3.0, 5.8, 2.2}
};

// Contoh data pengujian
float test_samples[][4] = {
  {5.0, 3.4, 1.5, 0.2},  // Setosa
  {6.7, 3.1, 4.7, 1.5},  // Versicolor
  {6.7, 3.0, 5.2, 2.3}   // Virginica
};

// Parameter KNN
const int K = 3;             // Jumlah tetangga
const int MAX_FEATURES = 4;  // Jumlah fitur
const int MAX_SAMPLES = 20;  // Maksimum jumlah sampel

KNN knn(K, MAX_FEATURES, MAX_SAMPLES);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("KNN Classification Example");
  Serial.println("-------------------------");
  
  // Tambahkan data training
  addTrainingData();
  
  // Lakukan prediksi
  predictSamples();
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

void addTrainingData() {
  Serial.println("Adding training data...");
  
  // Tambahkan data untuk Iris Setosa
  for (int i = 0; i < 4; i++) {
    knn.addTrainingData("setosa", iris_setosa[i]);
  }
  
  // Tambahkan data untuk Iris Versicolor
  for (int i = 0; i < 4; i++) {
    knn.addTrainingData("versicolor", iris_versicolor[i]);
  }
  
  // Tambahkan data untuk Iris Virginica
  for (int i = 0; i < 4; i++) {
    knn.addTrainingData("virginica", iris_virginica[i]);
  }
  
  Serial.print("Total training samples: ");
  Serial.println(knn.getDataCount());
  Serial.println();
}

void predictSamples() {
  Serial.println("Predicting test samples with K=3...");
  Serial.println("-----------------------------------");
  
  for (int i = 0; i < 3; i++) {
    const char* prediction = knn.predict(test_samples[i]);
    float confidence = knn.getPredictionConfidence(test_samples[i]);
    
    Serial.print("Sample ");
    Serial.print(i+1);
    Serial.print(": [");
    for (int j = 0; j < MAX_FEATURES; j++) {
      Serial.print(test_samples[i][j]);
      if (j < MAX_FEATURES - 1) Serial.print(", ");
    }
    Serial.print("] -> Predicted: ");
    Serial.print(prediction);
    Serial.print(" (Confidence: ");
    Serial.print(confidence * 100);
    Serial.println("%)");
    
    // Tampilkan 3 tetangga terdekat
    int indices[3];
    float distances[3];
    knn.getNearestNeighbors(test_samples[i], indices, distances, 3);
    
    Serial.println("  3 nearest neighbors:");
    for (int j = 0; j < 3; j++) {
      Serial.print("  - Distance: ");
      Serial.print(distances[j]);
      Serial.print(", Class: ");
      Serial.println(knn.predict(test_samples[i]));
    }
    Serial.println();
  }
}