void handleFindOptimalK() {
  Serial.println("\\n=== K Optimization Requested ===");
  Serial.println("Testing K values from 1 to 20...");
  Serial.println("This may take a few minutes...");

  float **allX = new float *[MAX_SAMPLES];
  for (int i = 0; i < MAX_SAMPLES; i++) {
    allX[i] = new float[MAX_FEATURES];
    allX[i][0] = dataset[i].Red;
    allX[i][1] = dataset[i].Green;
    allX[i][2] = dataset[i].Blue;
  }

  StandardScaler tempScaler(MAX_FEATURES);
  tempScaler.fitTransform(allX, MAX_SAMPLES);

  float bestAccuracy = 0.0;
  int bestK = 1;

  Serial.println("\\n--- K Testing Results ---");
  for (int testK = 1; testK <= MAX_K; testK++) {
    KNN tempKnn(testK, MAX_FEATURES, MAX_SAMPLES);
    tempKnn.setDistanceMetric(EUCLIDEAN);
    tempKnn.setWeightedVoting(true);

    for (int i = 0; i < MAX_SAMPLES; i++) {
      tempKnn.addTrainingData(dataset[i].label, allX[i]);
    }

    float accuracy = tempKnn.crossValidate(5);

    // Run multiple times for more stable results
    float totalAcc = accuracy;
    for (int run = 1; run < 3; run++) {
      KNN multiKnn(testK, MAX_FEATURES, MAX_SAMPLES);
      multiKnn.setDistanceMetric(EUCLIDEAN);
      multiKnn.setWeightedVoting(true);

      for (int j = 0; j < MAX_SAMPLES; j++) {
        multiKnn.addTrainingData(dataset[j].label, allX[j]);
      }

      totalAcc += multiKnn.crossValidate(5);
    }
    accuracy = totalAcc / 3.0;  // Average of 3 runs

    Serial.print("K=" + String(testK) + ": " + String(accuracy * 100, 2) + "%");

    if (accuracy > bestAccuracy) {
      bestAccuracy = accuracy;
      bestK = testK;
      Serial.print(" <- NEW BEST!");
    }
    Serial.println();

    delay(100);
  }

  Serial.println("\\n=== OPTIMIZATION COMPLETE ===");
  Serial.println("Best K found: " + String(bestK));
  Serial.println("Best Accuracy: " + String(bestAccuracy * 100, 2) + "%");
  Serial.println("==============================");

  Serial.println("\\nUpdating model with optimal K...");
  KNN optimizedKnn(bestK, MAX_FEATURES, MAX_SAMPLES);
  optimizedKnn.setDistanceMetric(EUCLIDEAN);
  optimizedKnn.setWeightedVoting(true);

  for (int i = 0; i < MAX_SAMPLES; i++) {
    optimizedKnn.addTrainingData(dataset[i].label, allX[i]);
  }

  knn = optimizedKnn;
  Serial.println("✓ Model successfully updated with K=" + String(bestK));

  for (int i = 0; i < MAX_SAMPLES; i++) {
    delete[] allX[i];
  }
  delete[] allX;

  Serial.println("\\nReady for predictions with optimized model!");
  Serial.println("Enter Red,Green,Blue values or FIND_K command:");
}