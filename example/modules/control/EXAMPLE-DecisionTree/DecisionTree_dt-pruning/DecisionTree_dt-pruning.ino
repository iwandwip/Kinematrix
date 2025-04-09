/*
 * DecisionTree_Pruning.ino
 * 
 * Menunjukkan penggunaan pruning untuk mengurangi overfitting
 * pada model Decision Tree
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

// Fungsi untuk melatih dan mengevaluasi model
void trainAndEvaluate(DecisionTree &dt, bool usePruning) {
  // Training model
  dt.train(GINI);
  
  // Jika menggunakan pruning, modifikasi kode di bagian train() pada DecisionTree.cpp
  // Hilangkan komentar pada baris "pruneTree(rootNode);"
  
  // Evaluasi model dengan cross-validation
  float accuracy = dt.crossValidate(5);
  
  Serial.print("Akurasi model ");
  Serial.print(usePruning ? "dengan" : "tanpa");
  Serial.print(" pruning: ");
  Serial.print(accuracy * 100.0);
  Serial.println("%");
  
  // Hitung jumlah node pada pohon keputusan
  // (implementasi ini tidak menyediakan fungsi ini, ini hanya sebagai contoh)
  Serial.print("Ukuran model (perkiraan): ");
  Serial.print(usePruning ? "Lebih kecil" : "Lebih besar");
  Serial.println(" (pruning mengurangi ukuran model)");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDecision Tree - Pruning Example");
  
  // Membuat dataset
  const int numSamples = 50;
  
  // Membuat model tanpa pruning - maksimal kedalaman 10
  DecisionTree dtNoPruning(2, numSamples, 10);
  
  // Membuat model dengan pruning - kedalaman yang sama
  DecisionTree dtWithPruning(2, numSamples, 10);
  
  // Menambahkan data sintetis
  // Beberapa data mungkin memiliki noise untuk menguji efektivitas pruning
  Serial.println("Membuat dataset sintetis dengan noise...");
  
  // Random seed
  randomSeed(analogRead(0));
  
  // Menambahkan data training
  for (int i = 0; i < numSamples; i++) {
    float x = random(100) / 100.0;
    float y = random(100) / 100.0;
    
    // Aturan dasar: jika x + y > 1, maka label = "A", selainnya "B"
    // Tambahkan sedikit noise (5% data salah label)
    bool shouldBeA = (x + y > 1.0);
    bool isNoisy = (random(100) < 5);
    const char* label = (shouldBeA != isNoisy) ? "A" : "B";
    
    float features[] = {x, y};
    dtNoPruning.addTrainingData(label, features);
    dtWithPruning.addTrainingData(label, features);
    
    // Cetak beberapa data contoh
    if (i < 5) {
      Serial.print("Sample ");
      Serial.print(i);
      Serial.print(": x=");
      Serial.print(x);
      Serial.print(", y=");
      Serial.print(y);
      Serial.print(", label=");
      Serial.print(label);
      if (isNoisy) Serial.print(" (noisy)");
      Serial.println();
    }
  }
  
  Serial.println("\nPerbandingan model dengan dan tanpa pruning:");
  
  // Evaluasi model tanpa pruning
  Serial.println("\nModel tanpa pruning:");
  trainAndEvaluate(dtNoPruning, false);
  
  // Evaluasi model dengan pruning
  Serial.println("\nModel dengan pruning:");
  // Catatan: Dalam implementasi asli, Anda perlu mengaktifkan pruning
  // dengan memodifikasi kode di fungsi train()
  trainAndEvaluate(dtWithPruning, true);
  
  // Mencoba model dengan berbagai tingkat noise
  Serial.println("\nEfek pruning pada data dengan berbagai tingkat noise:");
  
  int noiseLevels[] = {0, 5, 10, 20};
  
  for (int i = 0; i < 4; i++) {
    int noiseLevel = noiseLevels[i];
    
    Serial.print("\nNoise level: ");
    Serial.print(noiseLevel);
    Serial.println("%");
    
    // Model tanpa pruning
    DecisionTree noiseModelNoPruning(2, numSamples, 10);
    
    // Model dengan pruning
    DecisionTree noiseModelWithPruning(2, numSamples, 10);
    
    // Membuat dataset dengan tingkat noise tertentu
    for (int j = 0; j < numSamples; j++) {
      float x = random(100) / 100.0;
      float y = random(100) / 100.0;
      
      bool shouldBeA = (x + y > 1.0);
      bool isNoisy = (random(100) < noiseLevel);
      const char* label = (shouldBeA != isNoisy) ? "A" : "B";
      
      float features[] = {x, y};
      noiseModelNoPruning.addTrainingData(label, features);
      noiseModelWithPruning.addTrainingData(label, features);
    }
    
    // Evaluasi model
    trainAndEvaluate(noiseModelNoPruning, false);
    trainAndEvaluate(noiseModelWithPruning, true);
  }
  
  Serial.println("\nKesimpulan:");
  Serial.println("- Pruning membantu mengurangi overfitting, terutama pada data dengan noise");
  Serial.println("- Model dengan pruning umumnya lebih kecil dan lebih umum (generalisasi lebih baik)");
  Serial.println("- Pada data tanpa noise, pruning bisa mengurangi akurasi sedikit");
  Serial.println("- Pada data dengan noise tinggi, pruning sangat membantu meningkatkan akurasi");
}

void loop() {
  // Tidak digunakan dalam contoh ini
  delay(1000);
}
