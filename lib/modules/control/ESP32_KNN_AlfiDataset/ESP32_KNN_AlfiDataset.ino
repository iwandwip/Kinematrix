#define ENABLE_MODULE_KNN
#define ENABLE_MODULE_STANDARD_SCALER
#define ENABLE_MODULE_TRAIN_TEST_SPLIT
#include "Kinematrix.h"
#include "Datasets3.h"

const int MAX_FEATURES = 3;
const int K = 3;
const int MAX_K = 20;

KNN knn(K, MAX_FEATURES, MAX_SAMPLES);
StandardScaler scaler(MAX_FEATURES);
TrainTestSplit splitter;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== KNN Currency Classification ===");
  Serial.println("Dataset: AlfiDatasets4.csv");
  Serial.println("Features: R, G, B (RGB Color Values)");
  Serial.println("Classes: 1000, 2000, 5000 Rupiah");
  Serial.println("Total samples: " + String(MAX_SAMPLES));
  Serial.println("=====================================");

  prepareData();
  trainAndEvaluateKNN();
  testInteractivePrediction();

  Serial.println("\n=== Interactive Testing Mode ===");
  Serial.println("Send data in format: R,G,B");
  Serial.println("Example: 45,42,135");
  Serial.println("Commands:");
  Serial.println("- FIND_K : Find optimal K value (1-20)");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      if (input.equalsIgnoreCase("FIND_K") || input.equalsIgnoreCase("find_k")) {
        handleFindOptimalK();
      } else {
        testUserInput(input);
      }
    }
  }
  delay(100);
}