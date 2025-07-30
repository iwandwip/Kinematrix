void prepareData() {
  Serial.println("\n--- Preparing Data ---");

  float **X = new float *[MAX_SAMPLES];
  float *y = new float[MAX_SAMPLES];

  for (int i = 0; i < MAX_SAMPLES; i++) {
    X[i] = new float[MAX_FEATURES];
    X[i][0] = dataset[i].Red;
    X[i][1] = dataset[i].Green;
    X[i][2] = dataset[i].Blue;

    if (strcmp(dataset[i].label, "5000") == 0) y[i] = 5000;
    else if (strcmp(dataset[i].label, "10000") == 0) y[i] = 10000;
    else if (strcmp(dataset[i].label, "20000") == 0) y[i] = 20000;
  }

  DataSplit data = splitter.split(X, y, MAX_SAMPLES, MAX_FEATURES, 0.25f, true, 42);

  Serial.println("Data split completed:");
  Serial.println("Training samples: " + String(data.trainSize));
  Serial.println("Testing samples: " + String(data.testSize));
  Serial.println("Test ratio: 25%");

  Serial.println("\n--- Applying StandardScaler ---");
  scaler.fitTransform(data.X_train, data.trainSize);
  scaler.transform(data.X_test, data.testSize);

  Serial.println("Feature scaling completed");
  Serial.println("Feature statistics after scaling:");
  Serial.println("Red   - Mean: " + String(scaler.getMean(0), 2) + ", Std: " + String(scaler.getStdDev(0), 2));
  Serial.println("Green - Mean: " + String(scaler.getMean(1), 2) + ", Std: " + String(scaler.getStdDev(1), 2));
  Serial.println("Blue  - Mean: " + String(scaler.getMean(2), 2) + ", Std: " + String(scaler.getStdDev(2), 2));

  Serial.println("\n--- Training KNN Model ---");
  knn.setDistanceMetric(EUCLIDEAN);
  knn.setWeightedVoting(true);
  knn.setDebugMode(false);

  for (int i = 0; i < data.trainSize; i++) {
    const char *label = (data.y_train[i] == 5000) ? "5000" : (data.y_train[i] == 10000) ? "10000"
                                                                                        : "20000";
    knn.addTrainingData(label, data.X_train[i]);
  }

  Serial.println("KNN training completed");
  Serial.println("Training samples: " + String(knn.getDataCount()));

  evaluateModel(data);

  for (int i = 0; i < MAX_SAMPLES; i++) {
    delete[] X[i];
  }
  delete[] X;
  delete[] y;
  splitter.freeDataSplit(data);
}

void evaluateModel(const DataSplit &data) {
  Serial.println("\n--- Model Evaluation ---");

  int correct = 0;
  int total = data.testSize;
  int confusion[3][3] = { { 0 } };
  const char *classNames[] = { "5000", "10000", "20000" };

  Serial.println("Sample predictions (first 8):");
  for (int i = 0; i < data.testSize && i < 8; i++) {
    const char *prediction = knn.predict(data.X_test[i]);
    float confidence = knn.getPredictionConfidence(data.X_test[i]);

    const char *actual = (data.y_test[i] == 5000) ? "5000" : (data.y_test[i] == 10000) ? "10000"
                                                                                       : "20000";

    if (strcmp(prediction, actual) == 0) {
      correct++;
    }

    int actualIdx = (data.y_test[i] == 5000) ? 0 : (data.y_test[i] == 10000) ? 1
                                                                             : 2;
    int predIdx = (strcmp(prediction, "5000") == 0) ? 0 : (strcmp(prediction, "10000") == 0) ? 1
                                                                                             : 2;
    confusion[actualIdx][predIdx]++;

    Serial.println("Sample " + String(i + 1) + ": Predicted=" + String(prediction) + ", Actual=" + String(actual) + ", Confidence=" + String(confidence * 100, 1) + "%");
  }

  for (int i = 8; i < data.testSize; i++) {
    const char *prediction = knn.predict(data.X_test[i]);
    const char *actual = (data.y_test[i] == 5000) ? "5000" : (data.y_test[i] == 10000) ? "10000"
                                                                                       : "20000";

    if (strcmp(prediction, actual) == 0) {
      correct++;
    }

    int actualIdx = (data.y_test[i] == 5000) ? 0 : (data.y_test[i] == 10000) ? 1
                                                                             : 2;
    int predIdx = (strcmp(prediction, "5000") == 0) ? 0 : (strcmp(prediction, "10000") == 0) ? 1
                                                                                             : 2;
    confusion[actualIdx][predIdx]++;
  }

  float accuracy = (float)correct / total * 100.0;
  Serial.println("\nTest Accuracy: " + String(accuracy, 1) + "% (" + String(correct) + "/" + String(total) + ")");

  Serial.println("\nConfusion Matrix:");
  Serial.println("       Pred:");
  Serial.print("Actual   ");
  for (int i = 0; i < 3; i++) {
    Serial.print(String(classNames[i]) + "  ");
  }
  Serial.println();

  for (int i = 0; i < 3; i++) {
    Serial.print(String(classNames[i]) + "   ");
    for (int j = 0; j < 3; j++) {
      Serial.print(String(confusion[i][j]) + "     ");
    }
    Serial.println();
  }

  if (accuracy >= 92.0) {
    Serial.println("Model Performance: EXCELLENT");
  } else if (accuracy >= 85.0) {
    Serial.println("Model Performance: VERY GOOD");
  } else if (accuracy >= 78.0) {
    Serial.println("Model Performance: GOOD");
  } else if (accuracy >= 70.0) {
    Serial.println("Model Performance: FAIR");
  } else {
    Serial.println("Model Performance: POOR - Consider parameter tuning");
  }

  printClassDistribution();
}

void trainAndEvaluateKNN() {
  Serial.println("\n--- Cross-Validation Analysis ---");

  float **allX = new float *[MAX_SAMPLES];
  for (int i = 0; i < MAX_SAMPLES; i++) {
    allX[i] = new float[MAX_FEATURES];
    allX[i][0] = dataset[i].Red;
    allX[i][1] = dataset[i].Green;
    allX[i][2] = dataset[i].Blue;
  }

  scaler.reset();
  scaler.fitTransform(allX, MAX_SAMPLES);

  KNN cvKNN(K, MAX_FEATURES, MAX_SAMPLES);
  cvKNN.setDistanceMetric(EUCLIDEAN);
  cvKNN.setWeightedVoting(true);

  for (int i = 0; i < MAX_SAMPLES; i++) {
    cvKNN.addTrainingData(dataset[i].label, allX[i]);
  }

  float cvAccuracy = cvKNN.crossValidate(5);
  Serial.println("5-Fold Cross-Validation Accuracy: " + String(cvAccuracy * 100, 1) + "%");

  Serial.println("\nK-Value Analysis (current K=" + String(K) + "):");
  for (int testK = 3; testK <= 11; testK += 2) {
    KNN testKNN(testK, MAX_FEATURES, MAX_SAMPLES);
    testKNN.setDistanceMetric(EUCLIDEAN);
    testKNN.setWeightedVoting(true);

    for (int i = 0; i < MAX_SAMPLES; i++) {
      testKNN.addTrainingData(dataset[i].label, allX[i]);
    }

    float testAcc = testKNN.crossValidate(3);
    Serial.println("K=" + String(testK) + ": " + String(testAcc * 100, 1) + "%");
  }

  for (int i = 0; i < MAX_SAMPLES; i++) {
    delete[] allX[i];
  }
  delete[] allX;
}

void testInteractivePrediction() {
  Serial.println("\n--- Sample Predictions ---");

  float testSamples[][3] = {
    { 75, 200, 175 },  // Should predict 5000
    { 65, 170, 170 },  // Should predict 10000
    { 85, 240, 240 },  // Should predict 20000
    { 80, 220, 185 },  // Should predict 5000
    { 55, 155, 155 },  // Should predict 10000
    { 90, 250, 250 },  // Should predict 20000
    { 60, 165, 150 },  // Edge case
    { 70, 190, 200 }   // Edge case
  };

  const char *expected[] = { "5000", "10000", "20000", "5000", "10000", "20000", "?", "?" };

  for (int i = 0; i < 8; i++) {
    float scaledSample[MAX_FEATURES];
    for (int j = 0; j < MAX_FEATURES; j++) {
      scaledSample[j] = (testSamples[i][j] - scaler.getMean(j)) / scaler.getStdDev(j);
    }

    const char *prediction = knn.predict(scaledSample);
    float confidence = knn.getPredictionConfidence(scaledSample);

    int indices[K];
    float distances[K];
    knn.getNearestNeighbors(scaledSample, indices, distances, K);

    Serial.print("RGB(" + String((int)testSamples[i][0]) + "," + String((int)testSamples[i][1]) + "," + String((int)testSamples[i][2]) + ")");
    Serial.print(" -> " + String(prediction) + " Rupiah");

    if (strcmp(expected[i], "?") != 0) {
      Serial.print(" (Expected: " + String(expected[i]) + ")");
      if (strcmp(prediction, expected[i]) == 0) {
        Serial.print(" ✓");
      } else {
        Serial.print(" ✗");
      }
    }

    Serial.println(" Conf: " + String(confidence * 100, 1) + "%");

    Serial.print("  Nearest distances: ");
    for (int j = 0; j < min(3, K); j++) {
      Serial.print(String(distances[j], 2));
      if (j < min(2, K - 1)) Serial.print(", ");
    }
    Serial.println();
  }
}

void testUserInput(String input) {
  int comma1 = input.indexOf(',');
  int comma2 = input.indexOf(',', comma1 + 1);

  if (comma1 == -1 || comma2 == -1) {
    Serial.println("Invalid format! Use: Red,Green,Blue");
    Serial.println("Example: 75,200,175");
    return;
  }

  float Red = input.substring(0, comma1).toFloat();
  float Green = input.substring(comma1 + 1, comma2).toFloat();
  float Blue = input.substring(comma2 + 1).toFloat();

  if (Red < 0 || Red > 100 || Green < 125 || Green > 255 || Blue < 125 || Blue > 255) {
    Serial.println("Invalid ranges!");
    Serial.println("Red: 0-100, Green: 125-255, Blue: 125-255");
    return;
  }

  float sample[MAX_FEATURES] = { Red, Green, Blue };
  float scaledSample[MAX_FEATURES];

  for (int i = 0; i < MAX_FEATURES; i++) {
    scaledSample[i] = (sample[i] - scaler.getMean(i)) / scaler.getStdDev(i);
  }

  const char *prediction = knn.predict(scaledSample);
  float confidence = knn.getPredictionConfidence(scaledSample);

  int indices[K];
  float distances[K];
  knn.getNearestNeighbors(scaledSample, indices, distances, K);

  Serial.println("\n=== Higher Denomination Currency Classification ===");
  Serial.println("Input: RGB(" + String((int)Red) + "," + String((int)Green) + "," + String((int)Blue) + ")");
  Serial.println("Prediction: " + String(prediction) + " Rupiah");
  Serial.println("Confidence: " + String(confidence * 100, 1) + "%");

  Serial.print("K=" + String(K) + " Nearest distances: ");
  for (int i = 0; i < K; i++) {
    Serial.print(String(distances[i], 3));
    if (i < K - 1) Serial.print(", ");
  }
  Serial.println();

  String reliability = "";
  if (confidence >= 0.85) {
    reliability = "VERY HIGH";
  } else if (confidence >= 0.75) {
    reliability = "HIGH";
  } else if (confidence >= 0.65) {
    reliability = "MEDIUM";
  } else if (confidence >= 0.55) {
    reliability = "LOW";
  } else {
    reliability = "VERY LOW";
  }

  Serial.println("Reliability: " + reliability);

  if (strcmp(prediction, "5000") == 0) {
    Serial.println("Currency Info: 5000 Rupiah - Green dominant colors");
  } else if (strcmp(prediction, "10000") == 0) {
    Serial.println("Currency Info: 10000 Rupiah - Balanced RGB values");
  } else if (strcmp(prediction, "20000") == 0) {
    Serial.println("Currency Info: 20000 Rupiah - High RGB intensity");
  }

  Serial.println("\nEnter next test (Red,Green,Blue):");
}

void printClassDistribution() {
  Serial.println("\n--- Dataset Distribution Analysis ---");

  int count_5000 = 0, count_10000 = 0, count_20000 = 0;

  for (int i = 0; i < MAX_SAMPLES; i++) {
    if (strcmp(dataset[i].label, "5000") == 0) {
      count_5000++;
    } else if (strcmp(dataset[i].label, "10000") == 0) {
      count_10000++;
    } else if (strcmp(dataset[i].label, "20000") == 0) {
      count_20000++;
    }
  }

  Serial.println("By Denomination:");
  Serial.println("5000 Rupiah:  " + String(count_5000) + " samples");
  Serial.println("10000 Rupiah: " + String(count_10000) + " samples");
  Serial.println("20000 Rupiah: " + String(count_20000) + " samples");

  Serial.println("\nRGB-only classification (Emisi feature removed)");

  Serial.println("\nDataset Balance:");
  float balance_5000 = (float)count_5000 / MAX_SAMPLES * 100;
  float balance_10000 = (float)count_10000 / MAX_SAMPLES * 100;
  float balance_20000 = (float)count_20000 / MAX_SAMPLES * 100;

  Serial.println("5000 Rupiah:  " + String(balance_5000, 1) + "%");
  Serial.println("10000 Rupiah: " + String(balance_10000, 1) + "%");
  Serial.println("20000 Rupiah: " + String(balance_20000, 1) + "%");
}