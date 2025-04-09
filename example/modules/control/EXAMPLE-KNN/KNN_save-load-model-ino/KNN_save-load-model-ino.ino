/*
 * KNN Save and Load Model Example (ESP32 Only)
 * 
 * Contoh menyimpan dan memuat model KNN ke SPIFFS pada ESP32
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"
#include "SPIFFS.h"

// Dataset sensor suhu-kelembaban sederhana
// Format: {suhu, kelembaban}
float normal_condition[][2] = {
  {25.0, 60.0},
  {26.2, 58.5},
  {24.8, 62.1},
  {25.5, 59.8}
};

float warning_condition[][2] = {
  {32.5, 40.0},
  {31.8, 42.3},
  {33.0, 38.5},
  {31.2, 41.7}
};

float alarm_condition[][2] = {
  {38.2, 25.1},
  {39.5, 22.8},
  {37.8, 26.3},
  {39.0, 23.5}
};

// Parameter KNN
const int K = 3;
const int MAX_FEATURES = 2;
const int MAX_SAMPLES = 20;

KNN knn(K, MAX_FEATURES, MAX_SAMPLES);
const char* MODEL_PATH = "/knn_model.dat";

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("KNN Save/Load Model Example (ESP32)");
  Serial.println("----------------------------------");
  
  // Inisialisasi SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }
  
  // Bersihkan model sebelumnya jika ada
  if (SPIFFS.exists(MODEL_PATH)) {
    Serial.println("Removing previous model...");
    SPIFFS.remove(MODEL_PATH);
  }
  
  // Tambahkan data training
  Serial.println("Adding training data...");
  addTrainingData();
  
  // Simpan model ke SPIFFS
  Serial.println("Saving model to SPIFFS...");
  if (knn.saveModel(MODEL_PATH)) {
    Serial.println("Model saved successfully!");
  } else {
    Serial.print("Failed to save model: ");
    Serial.println(knn.getErrorMessage());
    return;
  }
  
  // Buat KNN baru untuk memuat model
  KNN loadedKnn(K, MAX_FEATURES, MAX_SAMPLES);
  
  // Muat model dari SPIFFS
  Serial.println("\nLoading model from SPIFFS...");
  if (loadedKnn.loadModel(MODEL_PATH)) {
    Serial.println("Model loaded successfully!");
    Serial.print("Loaded training samples: ");
    Serial.println(loadedKnn.getDataCount());
    
    // Lakukan prediksi dengan model yang dimuat
    float test_sample[2] = {32.0, 41.0};  // Mendekati "warning"
    Serial.println("\nPredicting with loaded model...");
    Serial.print("Sample [32.0, 41.0] -> Predicted: ");
    Serial.println(loadedKnn.predict(test_sample));
    
    // Menampilkan confidence
    float confidence = loadedKnn.getPredictionConfidence(test_sample);
    Serial.print("Prediction confidence: ");
    Serial.print(confidence * 100);
    Serial.println("%");
  } else {
    Serial.print("Failed to load model: ");
    Serial.println(loadedKnn.getErrorMessage());
  }
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

void addTrainingData() {
  // Tambahkan data untuk kondisi normal
  for (int i = 0; i < 4; i++) {
    knn.addTrainingData("normal", normal_condition[i]);
  }
  
  // Tambahkan data untuk kondisi peringatan
  for (int i = 0; i < 4; i++) {
    knn.addTrainingData("warning", warning_condition[i]);
  }
  
  // Tambahkan data untuk kondisi alarm
  for (int i = 0; i < 4; i++) {
    knn.addTrainingData("alarm", alarm_condition[i]);
  }
  
  Serial.print("Total training samples: ");
  Serial.println(knn.getDataCount());
}