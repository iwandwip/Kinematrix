/*
 * DecisionTree_Save_Load_Model.ino
 * 
 * Menunjukkan cara menyimpan dan memuat model Decision Tree
 * Catatan: Fitur ini hanya tersedia pada ESP32
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

#ifdef ESP32
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal mounting SPIFFS");
    return;
  }
  Serial.println("SPIFFS berhasil di-mount");
}
#endif

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDecision Tree - Save & Load Model Example");
  
#ifdef ESP32
  // Initialize SPIFFS
  initSPIFFS();
  
  // Membuat Decision Tree untuk klasifikasi iris
  DecisionTree dt(4, 30);
  
  // Menambahkan data training
  dt.addTrainingData("setosa", (float[]) {5.1, 3.5, 1.4, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.9, 3.0, 1.4, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.7, 3.2, 1.3, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.6, 3.1, 1.5, 0.2});
  dt.addTrainingData("setosa", (float[]) {5.0, 3.6, 1.4, 0.2});
  
  dt.addTrainingData("versicolor", (float[]) {7.0, 3.2, 4.7, 1.4});
  dt.addTrainingData("versicolor", (float[]) {6.4, 3.2, 4.5, 1.5});
  dt.addTrainingData("versicolor", (float[]) {6.9, 3.1, 4.9, 1.5});
  dt.addTrainingData("versicolor", (float[]) {5.5, 2.3, 4.0, 1.3});
  dt.addTrainingData("versicolor", (float[]) {6.5, 2.8, 4.6, 1.5});
  
  dt.addTrainingData("virginica", (float[]) {6.3, 3.3, 6.0, 2.5});
  dt.addTrainingData("virginica", (float[]) {5.8, 2.7, 5.1, 1.9});
  dt.addTrainingData("virginica", (float[]) {7.1, 3.0, 5.9, 2.1});
  dt.addTrainingData("virginica", (float[]) {6.3, 2.9, 5.6, 1.8});
  dt.addTrainingData("virginica", (float[]) {6.5, 3.0, 5.8, 2.2});
  
  // Training model
  Serial.println("Training model...");
  if (!dt.train(GINI)) {
    Serial.print("Gagal training model: ");
    Serial.println(dt.getErrorMessage());
    return;
  }
  
  // Menyimpan model ke file
  Serial.println("Menyimpan model ke SPIFFS...");
  if (dt.saveModel("/iris_model.dat")) {
    Serial.println("Model berhasil disimpan");
  } else {
    Serial.print("Gagal menyimpan model: ");
    Serial.println(dt.getErrorMessage());
    return;
  }
  
  // Sampel test
  float testSample[] = {5.0, 3.4, 1.5, 0.2}; // Contoh iris setosa
  Serial.print("Prediksi dengan model asli: ");
  Serial.println(dt.predict(testSample));
  
  // Memuat model dari file
  Serial.println("\nMemuat model dari SPIFFS...");
  DecisionTree loadedModel(4, 30);
  
  if (loadedModel.loadModel("/iris_model.dat")) {
    Serial.println("Model berhasil dimuat");
    
    // Test model yang dimuat
    Serial.print("Prediksi dengan model yang dimuat: ");
    Serial.println(loadedModel.predict(testSample));
    
    // Membersihkan file model jika diperlukan
    // SPIFFS.remove("/iris_model.dat");
  } else {
    Serial.print("Gagal memuat model: ");
    Serial.println(loadedModel.getErrorMessage());
  }
  
  Serial.println("\nMenguji penyimpanan dan pemuatan beberapa model...");
  
  // Membuat model dengan kriteria GINI
  DecisionTree dtGini(4, 30);
  for (int i = 0; i < 15; i++) {
    if (i < 5) {
      dtGini.addTrainingData("setosa", (float[]) {5.0, 3.5, 1.5, 0.2});
    } else if (i < 10) {
      dtGini.addTrainingData("versicolor", (float[]) {6.0, 3.0, 4.5, 1.5});
    } else {
      dtGini.addTrainingData("virginica", (float[]) {6.5, 3.0, 5.5, 2.0});
    }
  }
  dtGini.train(GINI);
  dtGini.saveModel("/model_gini.dat");
  
  // Membuat model dengan kriteria INFORMATION_GAIN
  DecisionTree dtIG(4, 30);
  for (int i = 0; i < 15; i++) {
    if (i < 5) {
      dtIG.addTrainingData("setosa", (float[]) {5.0, 3.5, 1.5, 0.2});
    } else if (i < 10) {
      dtIG.addTrainingData("versicolor", (float[]) {6.0, 3.0, 4.5, 1.5});
    } else {
      dtIG.addTrainingData("virginica", (float[]) {6.5, 3.0, 5.5, 2.0});
    }
  }
  dtIG.train(INFORMATION_GAIN);
  dtIG.saveModel("/model_ig.dat");
  
  // Memuat dan menguji kedua model
  DecisionTree loadedGini(4, 30);
  DecisionTree loadedIG(4, 30);
  
  loadedGini.loadModel("/model_gini.dat");
  loadedIG.loadModel("/model_ig.dat");
  
  float testSample2[] = {6.0, 3.0, 4.5, 1.5}; // Contoh iris versicolor
  
  Serial.print("Prediksi model GINI: ");
  Serial.println(loadedGini.predict(testSample2));
  
  Serial.print("Prediksi model INFORMATION_GAIN: ");
  Serial.println(loadedIG.predict(testSample2));
  
#else
  Serial.println("Fitur save/load model hanya tersedia pada ESP32");
  Serial.println("Silakan gunakan board ESP32 atau hilangkan kode yang terkait dengan SPIFFS");
#endif
}

void loop() {
  // Tidak digunakan dalam contoh ini
  delay(1000);
}
