void prepareData() {
  Serial.println("\n--- Preparing Data ---");

  float **X = new float *[MAX_SAMPLES];
  float *y = new float[MAX_SAMPLES];

  for (int i = 0; i < MAX_SAMPLES; i++) {
    X[i] = new float[MAX_FEATURES];
    X[i][0] = dataset[i].R;
    X[i][1] = dataset[i].G;
    X[i][2] = dataset[i].B;

    if (strcmp(dataset[i].label, "1000") == 0) y[i] = 1000;
    else if (strcmp(dataset[i].label, "2000") == 0) y[i] = 2000;
    else if (strcmp(dataset[i].label, "5000") == 0) y[i] = 5000;
  }

  DataSplit data = splitter.split(X, y, MAX_SAMPLES, MAX_FEATURES, 0.2f, true, 42);

  Serial.println("Data split completed:");
  Serial.println("Training samples: " + String(data.trainSize));
  Serial.println("Testing samples: " + String(data.testSize));

  Serial.println("\n--- Applying StandardScaler ---");
  scaler.fitTransform(data.X_train, data.trainSize);
  scaler.transform(data.X_test, data.testSize);

  Serial.println("Feature scaling completed");
  Serial.println("Mean values:");
  for (int i = 0; i < MAX_FEATURES; i++) {
    Serial.println("Feature " + String(i) + ": " + String(scaler.getMean(i), 2));
  }
  Serial.println("Std deviations:");
  for (int i = 0; i < MAX_FEATURES; i++) {
    Serial.println("Feature " + String(i) + ": " + String(scaler.getStdDev(i), 2));
  }

  Serial.println("\n--- Training KNN Model ---");
  knn.setDistanceMetric(EUCLIDEAN);
  knn.setWeightedVoting(true);
  knn.setDebugMode(false);

  for (int i = 0; i < data.trainSize; i++) {
    const char *label = (data.y_train[i] == 1000) ? "1000" : (data.y_train[i] == 2000) ? "2000"
                                                                                       : "5000";
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

  for (int i = 0; i < data.testSize; i++) {
    const char *prediction = knn.predict(data.X_test[i]);
    float confidence = knn.getPredictionConfidence(data.X_test[i]);

    const char *actual = (data.y_test[i] == 1000) ? "1000" : (data.y_test[i] == 2000) ? "2000"
                                                                                      : "5000";

    if (strcmp(prediction, actual) == 0) {
      correct++;
    }

    if (i < 5) {
      Serial.println("Sample " + String(i + 1) + ": Predicted=" + String(prediction) + ", Actual=" + String(actual) + ", Confidence=" + String(confidence * 100, 1) + "%");
    }
  }

  float accuracy = (float)correct / total * 100.0;
  Serial.println("\nTest Accuracy: " + String(accuracy, 1) + "% (" + String(correct) + "/" + String(total) + ")");

  if (accuracy >= 90.0) {
    Serial.println("Model Performance: EXCELLENT");
  } else if (accuracy >= 80.0) {
    Serial.println("Model Performance: GOOD");
  } else if (accuracy >= 70.0) {
    Serial.println("Model Performance: FAIR");
  } else {
    Serial.println("Model Performance: POOR - Consider parameter tuning");
  }

  printClassDistribution();
}

void trainAndEvaluateKNN() {
  Serial.println("\n--- Cross-Validation ---");

  float **allX = new float *[MAX_SAMPLES];
  for (int i = 0; i < MAX_SAMPLES; i++) {
    allX[i] = new float[MAX_FEATURES];
    allX[i][0] = dataset[i].R;
    allX[i][1] = dataset[i].G;
    allX[i][2] = dataset[i].B;
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

  for (int i = 0; i < MAX_SAMPLES; i++) {
    delete[] allX[i];
  }
  delete[] allX;
}

void testInteractivePrediction() {
  Serial.println("\n--- Sample Predictions ---");

  float testSamples[][3] = {
    { 45, 42, 135 },
    { 30, 30, 95 },
    { 48, 35, 120 },
    { 50, 48, 160 },
    { 40, 35, 130 },
    { 60, 50, 170 }
  };

  const char *expected[] = { "1000", "2000", "5000", "1000", "2000", "5000" };

  for (int i = 0; i < 6; i++) {
    float scaledSample[MAX_FEATURES];
    for (int j = 0; j < MAX_FEATURES; j++) {
      scaledSample[j] = (testSamples[i][j] - scaler.getMean(j)) / scaler.getStdDev(j);
    }

    const char *prediction = knn.predict(scaledSample);
    float confidence = knn.getPredictionConfidence(scaledSample);

    Serial.print("RGB(" + String((int)testSamples[i][0]) + "," + String((int)testSamples[i][1]) + "," + String((int)testSamples[i][2]) + ")");
    Serial.print(" -> " + String(prediction) + " Rupiah");
    Serial.print(" (Expected: " + String(expected[i]) + ")");
    Serial.print(" Confidence: " + String(confidence * 100, 1) + "%");

    if (strcmp(prediction, expected[i]) == 0) {
      Serial.println(" ✓");
    } else {
      Serial.println(" ✗");
    }
  }
}

void testUserInput(String input) {
  int comma1 = input.indexOf(',');
  int comma2 = input.indexOf(',', comma1 + 1);

  if (comma1 == -1 || comma2 == -1) {
    Serial.println("Invalid format! Use: R,G,B");
    return;
  }

  float R = input.substring(0, comma1).toFloat();
  float G = input.substring(comma1 + 1, comma2).toFloat();
  float B = input.substring(comma2 + 1).toFloat();

  if (R < 0 || R > 255 || G < 0 || G > 255 || B < 0 || B > 255) {
    Serial.println("Invalid values! RGB: 0-255");
    return;
  }

  float sample[MAX_FEATURES] = { R, G, B };
  float scaledSample[MAX_FEATURES];

  for (int i = 0; i < MAX_FEATURES; i++) {
    scaledSample[i] = (sample[i] - scaler.getMean(i)) / scaler.getStdDev(i);
  }

  const char *prediction = knn.predict(scaledSample);
  float confidence = knn.getPredictionConfidence(scaledSample);

  int indices[K];
  float distances[K];
  knn.getNearestNeighbors(scaledSample, indices, distances, K);

  Serial.println("\n=== Currency Classification Result ===");
  Serial.println("Input: RGB(" + String((int)R) + "," + String((int)G) + "," + String((int)B) + ")");
  Serial.println("Prediction: " + String(prediction) + " Rupiah");
  Serial.println("Confidence: " + String(confidence * 100, 1) + "%");

  Serial.print("Nearest distances: ");
  for (int i = 0; i < K; i++) {
    Serial.print(String(distances[i], 3));
    if (i < K - 1) Serial.print(", ");
  }
  Serial.println();

  if (confidence >= 0.8) {
    Serial.println("Reliability: HIGH");
  } else if (confidence >= 0.6) {
    Serial.println("Reliability: MEDIUM");
  } else {
    Serial.println("Reliability: LOW");
  }

  Serial.println("\nEnter next test (R,G,B):");
}

void printClassDistribution() {
  Serial.println("\n--- Dataset Distribution ---");

  int count_1000 = 0, count_2000 = 0, count_5000 = 0;

  for (int i = 0; i < MAX_SAMPLES; i++) {
    if (strcmp(dataset[i].label, "1000") == 0) count_1000++;
    else if (strcmp(dataset[i].label, "2000") == 0) count_2000++;
    else if (strcmp(dataset[i].label, "5000") == 0) count_5000++;
  }

  Serial.println("By Denomination:");
  Serial.println("1000 Rupiah: " + String(count_1000) + " samples");
  Serial.println("2000 Rupiah: " + String(count_2000) + " samples");
  Serial.println("5000 Rupiah: " + String(count_5000) + " samples");

  Serial.println("RGB-only classification (Emisi feature removed)");
}