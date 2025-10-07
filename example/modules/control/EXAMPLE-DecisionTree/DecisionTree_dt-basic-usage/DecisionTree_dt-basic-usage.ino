/*
 * DecisionTree_Basic_Usage.ino
 * 
 * Menunjukkan penggunaan dasar dari Decision Tree library
 * untuk melakukan klasifikasi dataset iris
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDecision Tree - Basic Usage Example");
  
  // Membuat instance Decision Tree dengan 4 fitur, max 30 sampel
  DecisionTree dt(4, 30);
  
  // Menambahkan data training untuk iris dataset
  // Format: addTrainingData("class_label", {feature1, feature2, feature3, feature4})
  
  // Iris-setosa samples
  dt.addTrainingData("setosa", (float[]) {5.1, 3.5, 1.4, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.9, 3.0, 1.4, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.7, 3.2, 1.3, 0.2});
  dt.addTrainingData("setosa", (float[]) {4.6, 3.1, 1.5, 0.2});
  dt.addTrainingData("setosa", (float[]) {5.0, 3.6, 1.4, 0.2});
  
  // Iris-versicolor samples
  dt.addTrainingData("versicolor", (float[]) {7.0, 3.2, 4.7, 1.4});
  dt.addTrainingData("versicolor", (float[]) {6.4, 3.2, 4.5, 1.5});
  dt.addTrainingData("versicolor", (float[]) {6.9, 3.1, 4.9, 1.5});
  dt.addTrainingData("versicolor", (float[]) {5.5, 2.3, 4.0, 1.3});
  dt.addTrainingData("versicolor", (float[]) {6.5, 2.8, 4.6, 1.5});
  
  // Iris-virginica samples
  dt.addTrainingData("virginica", (float[]) {6.3, 3.3, 6.0, 2.5});
  dt.addTrainingData("virginica", (float[]) {5.8, 2.7, 5.1, 1.9});
  dt.addTrainingData("virginica", (float[]) {7.1, 3.0, 5.9, 2.1});
  dt.addTrainingData("virginica", (float[]) {6.3, 2.9, 5.6, 1.8});
  dt.addTrainingData("virginica", (float[]) {6.5, 3.0, 5.8, 2.2});
  
  // Mencetak informasi data
  Serial.print("Data training: ");
  Serial.println(dt.getDataCount());
  Serial.print("Sampel setosa: ");
  Serial.println(dt.getDataCountByLabel("setosa"));
  Serial.print("Sampel versicolor: ");
  Serial.println(dt.getDataCountByLabel("versicolor"));
  Serial.print("Sampel virginica: ");
  Serial.println(dt.getDataCountByLabel("virginica"));

  // Mengaktifkan mode debug untuk melihat output training
  dt.setDebugMode(true);
  
  // Melatih model dengan kriteria GINI
  Serial.println("\nTraining decision tree...");
  if (dt.train(GINI)) {
    Serial.println("Training berhasil!");
  } else {
    Serial.print("Training gagal: ");
    Serial.println(dt.getErrorMessage());
    return;
  }

  // Membuat prediksi untuk sampel baru
  Serial.println("\nMembuat prediksi:");
  
  // Sampel setosa
  float testSetosa[] = {5.0, 3.4, 1.5, 0.2};
  Serial.print("Sampel [5.0, 3.4, 1.5, 0.2] -> ");
  Serial.println(dt.predict(testSetosa));
  
  // Sampel versicolor
  float testVersicolor[] = {6.0, 2.9, 4.5, 1.5};
  Serial.print("Sampel [6.0, 2.9, 4.5, 1.5] -> ");
  Serial.println(dt.predict(testVersicolor));
  
  // Sampel virginica
  float testVirginica[] = {6.7, 3.1, 5.6, 2.4};
  Serial.print("Sampel [6.7, 3.1, 5.6, 2.4] -> ");
  Serial.println(dt.predict(testVirginica));
}

void loop() {
  // Tidak digunakan dalam contoh ini
  delay(1000);
}
