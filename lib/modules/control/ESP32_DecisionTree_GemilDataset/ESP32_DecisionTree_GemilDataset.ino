#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"
#include "Datasets.h"

DecisionTree waterQualityTree(3, 27, 6, 2);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== Decision Tree - Water Quality Analysis ===");
  Serial.println("Dataset: Embedded GemilDatasets3");
  Serial.println("Fresh training mode - model trains every startup");

  Serial.println("Dataset loaded successfully!");
  Serial.println("Total records: " + String(datasetSize));

  setupDecisionTree();
  trainDecisionTree();
  testDecisionTree();

  Serial.println("\n=== Interactive Testing Mode ===");
  Serial.println("Send data in format: pH,TDS,UV");
  Serial.println("Example: Normal,Baik,Sedang");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      testUserInput(input);
    }
  }
  delay(100);
}


void setupDecisionTree() {
  Serial.println("\n--- Setting up Decision Tree ---");

  waterQualityTree.setFeatureType(0, CATEGORICAL);
  waterQualityTree.setFeatureType(1, CATEGORICAL);
  waterQualityTree.setFeatureType(2, CATEGORICAL);

  waterQualityTree.addCategoricalValue(0, "Asam");
  waterQualityTree.addCategoricalValue(0, "Normal");
  waterQualityTree.addCategoricalValue(0, "Basa");

  waterQualityTree.addCategoricalValue(1, "Baik");
  waterQualityTree.addCategoricalValue(1, "Normal");
  waterQualityTree.addCategoricalValue(1, "Buruk");

  waterQualityTree.addCategoricalValue(2, "Sedikit");
  waterQualityTree.addCategoricalValue(2, "Sedang");
  waterQualityTree.addCategoricalValue(2, "Banyak");

  Serial.println("Feature types configured:");
  Serial.println("- pH: CATEGORICAL (Asam, Normal, Basa)");
  Serial.println("- TDS: CATEGORICAL (Baik, Normal, Buruk)");
  Serial.println("- UV: CATEGORICAL (Sedikit, Sedang, Banyak)");
  Serial.println("- Target: Microplastic Level (Rendah, Sedang, Tinggi)");
}

void trainDecisionTree() {
  Serial.println("\n--- Training Decision Tree ---");

  for (int i = 0; i < datasetSize; i++) {
    FeatureValue features[3] = {
      FeatureValue(dataset[i].ph),
      FeatureValue(dataset[i].tds),
      FeatureValue(dataset[i].uv)
    };

    if (!waterQualityTree.addTrainingSample(features, TargetValue(dataset[i].microplastic))) {
      Serial.println("Failed to add training data at index: " + String(i));
    }
  }

  Serial.println("Training data added: " + String(datasetSize) + " samples");

  unsigned long startTime = millis();
  bool trainSuccess = waterQualityTree.train(GINI);
  unsigned long trainTime = millis() - startTime;

  if (trainSuccess) {
    Serial.println("Training completed successfully!");
    Serial.println("Training time: " + String(trainTime) + " ms");

    printClassDistribution();

    Serial.println("\n--- Decision Tree Structure ---");
    waterQualityTree.printTreeStructure();

    Serial.println("\n--- Tree Statistics ---");
    waterQualityTree.printTreeStatistics();
  } else {
    Serial.println("Training failed!");
  }
}

void testDecisionTree() {
  Serial.println("\n--- Testing Decision Tree ---");

  int correctPredictions = 0;
  int totalTests = 0;

  Serial.println("Sample predictions:");
  Serial.println("Format: [pH, TDS, UV] -> Predicted: Actual");

  for (int i = 0; i < min(10, datasetSize); i++) {
    FeatureValue testFeatures[3] = {
      FeatureValue(dataset[i].ph),
      FeatureValue(dataset[i].tds),
      FeatureValue(dataset[i].uv)
    };

    TargetValue prediction = waterQualityTree.predict(testFeatures);

    Serial.print("[" + String(dataset[i].ph) + ", " + String(dataset[i].tds) + ", " + String(dataset[i].uv) + "] -> ");
    Serial.print("Predicted: " + String(prediction.classLabel) + ", ");
    Serial.println("Actual: " + String(dataset[i].microplastic));

    if (strcmp(prediction.classLabel, dataset[i].microplastic) == 0) {
      correctPredictions++;
    }
    totalTests++;
  }

  float accuracy = (float)correctPredictions / totalTests * 100.0;
  Serial.println("\nSample Accuracy: " + String(accuracy, 1) + "% (" + String(correctPredictions) + "/" + String(totalTests) + ")");
}

void testUserInput(String input) {
  int comma1 = input.indexOf(',');
  int comma2 = input.indexOf(',', comma1 + 1);

  if (comma1 == -1 || comma2 == -1) {
    Serial.println("Invalid format! Use: pH,TDS,UV");
    Serial.println("Example: Normal,Baik,Sedang");
    return;
  }

  String ph = input.substring(0, comma1);
  String tds = input.substring(comma1 + 1, comma2);
  String uv = input.substring(comma2 + 1);

  ph.trim();
  tds.trim();
  uv.trim();

  if (!isValidInput(ph, 0) || !isValidInput(tds, 1) || !isValidInput(uv, 2)) {
    Serial.println("Invalid values! Use:");
    Serial.println("pH: Asam, Normal, Basa");
    Serial.println("TDS: Baik, Normal, Buruk");
    Serial.println("UV: Sedikit, Sedang, Banyak");
    return;
  }

  FeatureValue features[3] = {
    FeatureValue(ph.c_str()),
    FeatureValue(tds.c_str()),
    FeatureValue(uv.c_str())
  };

  TargetValue prediction = waterQualityTree.predict(features);

  Serial.println("\n=== Water Quality Analysis ===");
  Serial.println("Input:");
  Serial.println("- pH Level: " + ph);
  Serial.println("- TDS Quality: " + tds);
  Serial.println("- UV Reflection: " + uv);
  Serial.println("\nPrediction:");
  Serial.println("- Microplastic Level: " + String(prediction.classLabel));

  printWaterQualityAdvice(String(prediction.classLabel));
  Serial.println("\nEnter next test (pH,TDS,UV):");
}

bool isValidInput(String value, int featureIndex) {
  if (featureIndex == 0) {
    return (value == "Asam" || value == "Normal" || value == "Basa");
  } else if (featureIndex == 1) {
    return (value == "Baik" || value == "Normal" || value == "Buruk");
  } else if (featureIndex == 2) {
    return (value == "Sedikit" || value == "Sedang" || value == "Banyak");
  }
  return false;
}

void printWaterQualityAdvice(String microplasticLevel) {
  Serial.println("\n--- Water Quality Assessment ---");

  if (microplasticLevel == "Rendah") {
    Serial.println("Status: GOOD - Low microplastic contamination");
    Serial.println("Advice: Water quality is acceptable for most uses");
  } else if (microplasticLevel == "Sedang") {
    Serial.println("Status: MODERATE - Medium microplastic contamination");
    Serial.println("Advice: Consider additional filtration or treatment");
  } else if (microplasticLevel == "Tinggi") {
    Serial.println("Status: POOR - High microplastic contamination");
    Serial.println("Advice: Water treatment strongly recommended");
  }
}

void printClassDistribution() {
  Serial.println("\n--- Dataset Class Distribution ---");

  int rendahCount = 0, sedangCount = 0, tinggiCount = 0;

  for (int i = 0; i < datasetSize; i++) {
    if (strcmp(dataset[i].microplastic, "Rendah") == 0) rendahCount++;
    else if (strcmp(dataset[i].microplastic, "Sedang") == 0) sedangCount++;
    else if (strcmp(dataset[i].microplastic, "Tinggi") == 0) tinggiCount++;
  }

  Serial.println("Rendah (Low): " + String(rendahCount) + " samples");
  Serial.println("Sedang (Medium): " + String(sedangCount) + " samples");
  Serial.println("Tinggi (High): " + String(tinggiCount) + " samples");
  Serial.println("Total: " + String(datasetSize) + " samples");
}