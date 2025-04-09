/*
 * DecisionTree_Weather_Prediction.ino
 * 
 * Contoh aplikasi prediksi cuaca sederhana menggunakan Decision Tree
 * dengan sensor suhu dan kelembaban
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

// Ganti dengan library sensor yang sesuai
// Contoh: DHT11/DHT22, BME280, dll.
// #include <DHT.h>

// Definisi pin dan tipe sensor
// #define DHTPIN 2
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

// Untuk simulasi, akan menggunakan nilai random dari fungsi simulateReading()
void simulateReading(float &temperature, float &humidity, float &pressure) {
  // Simulasi bacaan sensor
  temperature = random(150, 350) / 10.0; // 15.0 - 35.0 C
  humidity = random(400, 900) / 10.0;    // 40.0 - 90.0 %
  pressure = random(9800, 10300) / 10.0; // 980.0 - 1030.0 hPa
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDecision Tree - Weather Prediction Example");
  
  // Inisialisasi sensor
  // dht.begin();
  randomSeed(analogRead(0));
  
  // Membuat Decision Tree dengan 3 fitur: suhu, kelembaban, tekanan
  DecisionTree weatherTree(3, 30);
  
  // Data cuaca historis (data latih)
  // Format: [suhu, kelembaban, tekanan] -> kondisi cuaca
  Serial.println("Menambahkan data historis cuaca...");
  
  // Cerah
  weatherTree.addTrainingData("cerah", (float[]) {30.0, 50.0, 1013.0});
  weatherTree.addTrainingData("cerah", (float[]) {29.5, 45.0, 1012.0});
  weatherTree.addTrainingData("cerah", (float[]) {31.0, 48.0, 1010.0});
  weatherTree.addTrainingData("cerah", (float[]) {28.0, 55.0, 1015.0});
  weatherTree.addTrainingData("cerah", (float[]) {27.5, 60.0, 1014.0});
  weatherTree.addTrainingData("cerah", (float[]) {32.0, 40.0, 1011.0});
  
  // Berawan
  weatherTree.addTrainingData("berawan", (float[]) {27.0, 65.0, 1008.0});
  weatherTree.addTrainingData("berawan", (float[]) {26.5, 70.0, 1007.0});
  weatherTree.addTrainingData("berawan", (float[]) {25.0, 68.0, 1005.0});
  weatherTree.addTrainingData("berawan", (float[]) {28.0, 72.0, 1006.0});
  weatherTree.addTrainingData("berawan", (float[]) {24.5, 75.0, 1004.0});
  weatherTree.addTrainingData("berawan", (float[]) {26.0, 67.0, 1009.0});
  
  // Hujan
  weatherTree.addTrainingData("hujan", (float[]) {23.0, 85.0, 1000.0});
  weatherTree.addTrainingData("hujan", (float[]) {22.5, 88.0, 998.0});
  weatherTree.addTrainingData("hujan", (float[]) {21.0, 90.0, 995.0});
  weatherTree.addTrainingData("hujan", (float[]) {24.0, 82.0, 1002.0});
  weatherTree.addTrainingData("hujan", (float[]) {20.5, 92.0, 997.0});
  weatherTree.addTrainingData("hujan", (float[]) {22.0, 87.0, 1001.0});
  
  // Train model
  Serial.println("Melatih model prediksi cuaca...");
  if (!weatherTree.train(GINI)) {
    Serial.print("Gagal melatih model: ");
    Serial.println(weatherTree.getErrorMessage());
    return;
  }
  
  // Evaluasi model dengan cross-validation
  float accuracy = weatherTree.crossValidate(3);
  Serial.print("Akurasi model: ");
  Serial.print(accuracy * 100.0);
  Serial.println("%");
  
  // Simulasi prediksi cuaca dengan bacaan sensor
  Serial.println("\nSimulasi prediksi cuaca real-time:");
  Serial.println("----------------------------------");
  
  for (int i = 0; i < 10; i++) {
    // Baca sensor (simulasi)
    float temperature, humidity, pressure;
    simulateReading(temperature, humidity, pressure);
    
    // Prediksi cuaca
    float features[] = {temperature, humidity, pressure};
    const char* prediction = weatherTree.predict(features);
    
    // Tampilkan hasil
    Serial.print("Pembacaan #");
    Serial.print(i+1);
    Serial.print(": Suhu = ");
    Serial.print(temperature);
    Serial.print("°C, Kelembaban = ");
    Serial.print(humidity);
    Serial.print("%, Tekanan = ");
    Serial.print(pressure);
    Serial.print(" hPa => Prediksi cuaca: ");
    Serial.println(prediction);
    
    delay(1000);
  }
  
  // Tips untuk implementasi nyata
  Serial.println("\nTips untuk implementasi nyata:");
  Serial.println("1. Gunakan sensor yang sesuai (DHT22, BME280, dll)");
  Serial.println("2. Kumpulkan data historis yang lebih banyak");
  Serial.println("3. Tambahkan fitur seperti perubahan tekanan, suhu, dll");
  Serial.println("4. Gunakan pruning untuk model yang lebih robust");
  
  // Contoh integrasi dengan sistem lain
  Serial.println("\nContoh integrasi dengan sistem lain:");
  Serial.println("- Sistem irigasi otomatis berdasarkan prediksi cuaca");
  Serial.println("- Penjadwalan kipas/AC berdasarkan prediksi suhu");
  Serial.println("- Peringatan dini cuaca buruk melalui notifikasi");
}

void loop() {
  // Dalam aplikasi nyata, Anda akan membaca sensor secara berkala
  // dan menggunakan model untuk memprediksi cuaca
  
  delay(5000);
  
  // Simulasi pembacaan sensor
  float temperature, humidity, pressure;
  simulateReading(temperature, humidity, pressure);
  
  Serial.print("\nPembacaan realtime: Suhu = ");
  Serial.print(temperature);
  Serial.print("°C, Kelembaban = ");
  Serial.print(humidity);
  Serial.print("%, Tekanan = ");
  Serial.print(pressure);
  Serial.println(" hPa");
  
  // Buat instance Decision Tree baru karena ini contoh
  // Dalam aplikasi nyata, Anda akan menggunakan model yang sudah dilatih
  DecisionTree quickModel(3, 20);
  
  // Data latih sederhana
  quickModel.addTrainingData("cerah", (float[]) {30.0, 50.0, 1013.0});
  quickModel.addTrainingData("cerah", (float[]) {28.0, 55.0, 1015.0});
  quickModel.addTrainingData("berawan", (float[]) {27.0, 65.0, 1008.0});
  quickModel.addTrainingData("berawan", (float[]) {26.5, 70.0, 1007.0});
  quickModel.addTrainingData("hujan", (float[]) {23.0, 85.0, 1000.0});
  quickModel.addTrainingData("hujan", (float[]) {22.0, 87.0, 1001.0});
  
  // Latih dan prediksi
  if (quickModel.train()) {
    float features[] = {temperature, humidity, pressure};
    const char* prediction = quickModel.predict(features);
    
    Serial.print("Prediksi cuaca: ");
    Serial.println(prediction);
    
    // Dalam aplikasi nyata, Anda bisa mengambil tindakan berdasarkan prediksi
    if (strcmp(prediction, "hujan") == 0) {
      Serial.println("Peringatan: Cuaca hujan diprediksi, pertimbangkan untuk menutup jendela!");
    }
  }
}
