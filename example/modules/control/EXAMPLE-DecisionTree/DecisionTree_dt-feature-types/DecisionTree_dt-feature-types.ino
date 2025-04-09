/*
 * DecisionTree_Feature_Types.ino
 * 
 * Menunjukkan cara menggunakan tipe fitur yang berbeda (numerik dan kategorikal)
 * Catatan: Dalam implementasi ini, fitur kategorikal direpresentasikan sebagai 
 * nilai numerik dengan indeks kategori
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDecision Tree - Feature Types Example");
  
  // Membuat Decision Tree dengan 3 fitur, max 30 sampel
  // Fitur: [suhu, kelembaban, cuaca] 
  // cuaca: 0=cerah, 1=berawan, 2=hujan
  DecisionTree dt(3, 30);
  
  // Set tipe fitur - fitur ke-2 (cuaca) adalah kategorikal
  dt.setFeatureType(0, NUMERIC);  // suhu - numerik
  dt.setFeatureType(1, NUMERIC);  // kelembaban - numerik
  dt.setFeatureType(2, CATEGORICAL); // cuaca - kategorikal
  
  // Dataset untuk memprediksi apakah bisa bermain tenis
  // Format: [suhu, kelembaban, cuaca]
  // Label: "ya" atau "tidak"
  dt.addTrainingData("ya", (float[]) {29.0, 70.0, 0.0}); // cerah
  dt.addTrainingData("tidak", (float[]) {32.0, 90.0, 0.0}); // cerah
  dt.addTrainingData("ya", (float[]) {21.0, 65.0, 0.0}); // cerah
  dt.addTrainingData("ya", (float[]) {20.0, 60.0, 1.0}); // berawan
  dt.addTrainingData("ya", (float[]) {25.0, 70.0, 1.0}); // berawan
  dt.addTrainingData("tidak", (float[]) {31.0, 80.0, 1.0}); // berawan
  dt.addTrainingData("ya", (float[]) {24.0, 80.0, 1.0}); // berawan
  dt.addTrainingData("tidak", (float[]) {33.0, 85.0, 2.0}); // hujan
  dt.addTrainingData("ya", (float[]) {23.0, 65.0, 2.0}); // hujan
  dt.addTrainingData("ya", (float[]) {24.0, 70.0, 2.0}); // hujan
  dt.addTrainingData("tidak", (float[]) {26.0, 90.0, 2.0}); // hujan
  dt.addTrainingData("ya", (float[]) {22.0, 75.0, 1.0}); // berawan
  dt.addTrainingData("tidak", (float[]) {35.0, 90.0, 0.0}); // cerah
  dt.addTrainingData("tidak", (float[]) {30.0, 95.0, 2.0}); // hujan

  Serial.print("Jumlah data training: ");
  Serial.println(dt.getDataCount());
  
  // Melatih model
  Serial.println("\nMelatih model Decision Tree...");
  dt.setDebugMode(true);
  
  if (dt.train(GINI)) {
    Serial.println("Training berhasil!");
  } else {
    Serial.print("Training gagal: ");
    Serial.println(dt.getErrorMessage());
    return;
  }
  
  // Melakukan prediksi
  Serial.println("\nMembuat prediksi:");
  
  // Kasus 1: Suhu sedang, kelembaban sedang, cuaca cerah
  float testCase1[] = {25.0, 70.0, 0.0};
  Serial.print("Cuaca cerah, suhu 25C, kelembaban 70% -> ");
  Serial.println(dt.predict(testCase1));
  
  // Kasus 2: Suhu tinggi, kelembaban tinggi, cuaca berawan
  float testCase2[] = {32.0, 85.0, 1.0};
  Serial.print("Cuaca berawan, suhu 32C, kelembaban 85% -> ");
  Serial.println(dt.predict(testCase2));
  
  // Kasus 3: Suhu rendah, kelembaban sedang, cuaca hujan
  float testCase3[] = {22.0, 70.0, 2.0};
  Serial.print("Cuaca hujan, suhu 22C, kelembaban 70% -> ");
  Serial.println(dt.predict(testCase3));
  
  // Cross-validation untuk mengevaluasi performa
  float accuracy = dt.crossValidate(3);
  Serial.print("\nAkurasi 3-fold cross-validation: ");
  Serial.print(accuracy * 100.0);
  Serial.println("%");
  
  // Menunjukkan pentingnya tipe fitur yang tepat
  Serial.println("\nMembandingkan model dengan dan tanpa definisi tipe fitur:");
  
  // Model dengan semua fitur dianggap numerik
  DecisionTree dtNumeric(3, 30);
  // Copy data training
  for (int i = 0; i < 14; i++) {
    if (i == 0 || i == 2 || i == 3 || i == 4 || i == 6 || i == 8 || i == 9 || i == 11) {
      dtNumeric.addTrainingData("ya", (float[]) {
        (i < 3) ? 25.0 : ((i < 7) ? 24.0 : 23.0),
        (i < 5) ? 70.0 : 75.0,
        (i % 3)
      });
    } else {
      dtNumeric.addTrainingData("tidak", (float[]) {
        (i < 10) ? 32.0 : 30.0,
        85.0,
        (i % 3)
      });
    }
  }
  
  dtNumeric.train(GINI);
  float accNumeric = dtNumeric.crossValidate(3);
  
  // Model dengan definisi tipe fitur
  DecisionTree dtMixed(3, 30);
  dtMixed.setFeatureType(2, CATEGORICAL);
  // Copy data training
  for (int i = 0; i < 14; i++) {
    if (i == 0 || i == 2 || i == 3 || i == 4 || i == 6 || i == 8 || i == 9 || i == 11) {
      dtMixed.addTrainingData("ya", (float[]) {
        (i < 3) ? 25.0 : ((i < 7) ? 24.0 : 23.0),
        (i < 5) ? 70.0 : 75.0,
        (i % 3)
      });
    } else {
      dtMixed.addTrainingData("tidak", (float[]) {
        (i < 10) ? 32.0 : 30.0,
        85.0,
        (i % 3)
      });
    }
  }
  
  dtMixed.train(GINI);
  float accMixed = dtMixed.crossValidate(3);
  
  Serial.print("Akurasi model dengan semua fitur numerik: ");
  Serial.print(accNumeric * 100.0);
  Serial.println("%");
  
  Serial.print("Akurasi model dengan fitur kategorikal: ");
  Serial.print(accMixed * 100.0);
  Serial.println("%");
}

void loop() {
  // Tidak digunakan dalam contoh ini
  delay(1000);
}
