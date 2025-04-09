/*
 * KNN Model Evaluation Example
 * 
 * Menunjukkan cara mengevaluasi model KNN menggunakan cross-validation
 * dan evaluasi akurasi dengan dataset test
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Dataset Diabetes sederhana (glukosa, insulin, bmi)
// Format: {glukosa, insulin, bmi}
float diabetic_data[][3] = {
  {165, 55, 32},
  {172, 58, 34},
  {155, 50, 31},
  {160, 53, 30},
  {178, 60, 36},
  {182, 65, 38},
  {170, 57, 33},
  {175, 59, 35}
};

float nondiabetic_data[][3] = {
  {105, 10, 25},
  {98, 8, 24},
  {110, 12, 26},
  {95, 9, 23},
  {100, 10, 24},
  {112, 15, 27},
  {108, 11, 25},
  {102, 9, 23}
};

// Data test untuk evaluasi
float test_features[][3] = {
  {168, 56, 33}, // Diabetic
  {172, 59, 35}, // Diabetic
  {100, 11, 25}, // Non-diabetic
  {105, 12, 26}  // Non-diabetic
};

const char* test_labels[] = {
  "diabetic",
  "diabetic",
  "nondiabetic",
  "nondiabetic"
};

// Pointer array untuk evaluasi model
const float* test_feature_pointers[4];
const char* test_label_pointers[4];

// Parameter KNN
const int K = 3;
const int MAX_FEATURES = 3;
const int MAX_SAMPLES = 20;

KNN knn(K, MAX_FEATURES, MAX_SAMPLES);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("KNN Model Evaluation Example");
  Serial.println("---------------------------");
  
  // Prepare test pointers
  for (int i = 0; i < 4; i++) {
    test_feature_pointers[i] = test_features[i];
    test_label_pointers[i] = test_labels[i];
  }
  
  // Tambahkan data training
  addTrainingData();
  
  // Lakukan cross-validation
  performCrossValidation();
  
  // Evaluasi dengan dataset test
  evaluateAccuracy();
}

void loop() {
  // Tidak ada yang dilakukan di loop
}

void addTrainingData() {
  Serial.println("Adding training data...");
  
  // Tambahkan data untuk kelas diabetic
  for (int i = 0; i < 8; i++) {
    knn.addTrainingData("diabetic", diabetic_data[i]);
  }
  
  // Tambahkan data untuk kelas non-diabetic
  for (int i = 0; i < 8; i++) {
    knn.addTrainingData("nondiabetic", nondiabetic_data[i]);
  }
  
  Serial.print("Total training samples: ");
  Serial.println(knn.getDataCount());
  Serial.println();
}

void performCrossValidation() {
  Serial.println("Performing 4-fold cross-validation...");
  
  // Aktifkan normalisasi untuk lebih akurat
  knn.enableNormalization(true);
  
  // 4-fold cross-validation
  float accuracy = knn.crossValidate(4);
  
  Serial.print("Cross-validation accuracy: ");
  Serial.print(accuracy * 100);
  Serial.println("%");
  Serial.println();
}

void evaluateAccuracy() {
  Serial.println("Evaluating model with test dataset...");
  
  // Evaluasi dengan dataset test
  float accuracy = knn.evaluateAccuracy(test_feature_pointers, test_label_pointers, 4);
  
  Serial.print("Test dataset accuracy: ");
  Serial.print(accuracy * 100);
  Serial.println("%");
  Serial.println();
  
  // Tampilkan prediksi untuk setiap data test
  Serial.println("Individual test predictions:");
  
  for (int i = 0; i < 4; i++) {
    const char* prediction = knn.predict(test_features[i]);
    float confidence = knn.getPredictionConfidence(test_features[i]);
    
    Serial.print("Sample ");
    Serial.print(i+1);
    Serial.print(" (Actual: ");
    Serial.print(test_labels[i]);
    Serial.print("): Predicted = ");
    Serial.print(prediction);
    Serial.print(" (Confidence: ");
    Serial.print(confidence * 100);
    Serial.println("%)");
  }
}