/*
 * DecisionTree_Cross_Validation.ino
 * 
 * Menunjukkan cara melakukan evaluasi model dengan cross-validation
 * untuk mendapatkan estimasi performa yang lebih baik
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDecision Tree - Cross Validation Example");
  
  // Membuat instance Decision Tree dengan 4 fitur, max 30 sampel
  DecisionTree dt(4, 30);
  
  // Menambahkan data training untuk iris dataset
  // Iris-setosa samples
  dt.addTrainingData("setosa", (float[]) {5.1, 3.5, 1.4, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.9, 3.0, 1.4, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.7, 3.2, 1.3, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.6, 3.1, 1.5, 0.2});
  dt.addTrainingData("setosa", (float[]) {5.0, 3.6, 1.4, 0.2});
  dt.addTrainingData("setosa", (float[]) {5.4, 3.9, 1.7, 0.4});
  dt.addTrainingData("setosa", (float[]) {4.6, 3.4, 1.4, 0.3});
  dt.addTrainingData("setosa", (float[]) {5.0, 3.4, 1.5, 0.2});
  
  // Iris-versicolor samples
  dt.addTrainingData("versicolor", (float[]) {7.0, 3.2, 4.7, 1.4});
  dt.addTrainingData("versicolor", (float[]) {6.4, 3.2, 4.5, 1.5});
  dt.addTrainingData("versicolor", (float[]) {6.9, 3.1, 4.9, 1.5});
  dt.addTrainingData("versicolor", (float[]) {5.5, 2.3, 4.0, 1.3});
  dt.addTrainingData("versicolor", (float[]) {6.5, 2.8, 4.6, 1.5});
  dt.addTrainingData("versicolor", (float[]) {5.7, 2.8, 4.5, 1.3});
  dt.addTrainingData("versicolor", (float[]) {6.3, 3.3, 4.7, 1.6});
  dt.addTrainingData("versicolor", (float[]) {4.9, 2.4, 3.3, 1.0});
  
  // Iris-virginica samples
  dt.addTrainingData("virginica", (float[]) {6.3, 3.3, 6.0, 2.5});
  dt.addTrainingData("virginica", (float[]) {5.8, 2.7, 5.1, 1.9});
  dt.addTrainingData("virginica", (float[]) {7.1, 3.0, 5.9, 2.1});
  dt.addTrainingData("virginica", (float[]) {6.3, 2.9, 5.6, 1.8});
  dt.addTrainingData("virginica", (float[]) {6.5, 3.0, 5.8, 2.2});
  dt.addTrainingData("virginica", (float[]) {7.6, 3.0, 6.6, 2.1});
  dt.addTrainingData("virginica", (float[]) {4.9, 2.5, 4.5, 1.7});
  dt.addTrainingData("virginica", (float[]) {7.3, 2.9, 6.3, 1.8});
  
  Serial.print("Total data training: ");
  Serial.println(dt.getDataCount());

  // Melakukan cross-validation dengan berbagai fold
  Serial.println("\nPerforma model dengan cross-validation:");
  
  // 3-fold cross-validation
  float accuracy3Fold = dt.crossValidate(3);
  Serial.print("Akurasi dengan 3-fold CV: ");
  Serial.print(accuracy3Fold * 100.0);
  Serial.println("%");
  
  // 5-fold cross-validation
  float accuracy5Fold = dt.crossValidate(5);
  Serial.print("Akurasi dengan 5-fold CV: ");
  Serial.print(accuracy5Fold * 100.0);
  Serial.println("%");
  
  // Membandingkan performa antara kriteria GINI dan INFORMATION_GAIN
  Serial.println("\nPerbandingan kriteria split:");
  
  // Model dengan kriteria GINI
  dt.setDebugMode(false); // Mematikan debug mode untuk output yang lebih bersih
  dt.train(GINI);
  float accuracyGini = dt.crossValidate(5);
  Serial.print("Akurasi dengan GINI: ");
  Serial.print(accuracyGini * 100.0);
  Serial.println("%");
  
  // Model dengan kriteria INFORMATION_GAIN
  dt.train(INFORMATION_GAIN);
  float accuracyIG = dt.crossValidate(5);
  Serial.print("Akurasi dengan INFORMATION_GAIN: ");
  Serial.print(accuracyIG * 100.0);
  Serial.println("%");
  
  // Mencoba beberapa nilai kedalaman pohon
  Serial.println("\nPengaruh kedalaman pohon terhadap akurasi:");
  
  int maxDepths[] = {2, 3, 5, 10};
  for (int i = 0; i < 4; i++) {
    int depth = maxDepths[i];
    
    // Membuat model baru dengan kedalaman berbeda
    DecisionTree dtDepth(4, 30, depth);
    
    // Mengcopy data training
    for (int j = 0; j < dt.getDataCount(); j++) {
      // Dalam implementasi nyata, Anda perlu mengakses data dan label dari dt,
      // tetapi untuk contoh ini kita akan menambahkan data yang sama kembali
      float features[4];
      // Tambahkan data training yang sama
      if (j < 8) {
        dtDepth.addTrainingData("setosa", (float[]) {5.0, 3.5, 1.5, 0.2});
      } else if (j < 16) {
        dtDepth.addTrainingData("versicolor", (float[]) {6.0, 3.0, 4.5, 1.5});
      } else {
        dtDepth.addTrainingData("virginica", (float[]) {6.5, 3.0, 5.5, 2.0});
      }
    }
    
    dtDepth.train(GINI);
    float accuracyDepth = dtDepth.crossValidate(5);
    
    Serial.print("Akurasi dengan kedalaman ");
    Serial.print(depth);
    Serial.print(": ");
    Serial.print(accuracyDepth * 100.0);
    Serial.println("%");
  }
}

void loop() {
  // Tidak digunakan dalam contoh ini
  delay(1000);
}
