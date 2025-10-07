/*
 * Soil Type Classification using KNN
 * 
 * Menggunakan sensor moisture dan pH untuk mengklasifikasikan jenis tanah
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Parameter KNN
const int K = 3;
const int MAX_FEATURES = 2; // Moisture and pH
const int MAX_SAMPLES = 30;

// Create KNN instance
KNN soilKnn(K, MAX_FEATURES, MAX_SAMPLES);

// Sensor pins
const int MOISTURE_PIN = A0;
const int PH_PIN = A1;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Soil Type Classification using KNN");
  Serial.println("---------------------------------");
  
  // Train the model with sample soil data
  trainSoilModel();
  
  Serial.println("Model trained. Ready for soil classification.");
  Serial.println("Insert the sensor into soil and press any key...");
}

void loop() {
  if (Serial.available() > 0) {
    // Clear the serial buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
    
    // Read moisture and pH
    float moisture = readMoisture();
    float pH = readPH();
    
    // Create feature array
    float features[2] = {moisture, pH};
    
    // Make prediction
    const char* prediction = soilKnn.predict(features);
    float confidence = soilKnn.getPredictionConfidence(features);
    
    // Display results
    Serial.print("Moisture: ");
    Serial.print(moisture);
    Serial.print("%, pH: ");
    Serial.print(pH);
    Serial.print(", Soil Type: ");
    Serial.print(prediction);
    Serial.print(" (Confidence: ");
    Serial.print(confidence * 100);
    Serial.println("%)");
    
    // Provide some gardening recommendations
    provideRecommendations(prediction);
    
    Serial.println("\nInsert sensor into another soil sample and press any key...");
  }
}

void trainSoilModel() {
  Serial.println("Training soil classification model...");
  
  // Sample data for sandy soil
  float sandy[][2] = {
    {10, 5.5}, {8, 5.7}, {12, 5.3}, {9, 5.8},
    {11, 5.4}, {7, 5.6}, {10, 5.2}, {8, 5.5}
  };
  
  // Sample data for clay soil
  float clay[][2] = {
    {40, 7.2}, {38, 7.0}, {42, 7.4}, {37, 7.1},
    {41, 7.3}, {39, 7.2}, {43, 7.5}, {36, 7.0}
  };
  
  // Sample data for loamy soil
  float loamy[][2] = {
    {25, 6.5}, {23, 6.3}, {27, 6.7}, {24, 6.4},
    {26, 6.6}, {22, 6.2}, {28, 6.8}, {25, 6.5}
  };
  
  // Add sandy soil data
  for (int i = 0; i < 8; i++) {
    soilKnn.addTrainingData("sandy", sandy[i]);
  }
  
  // Add clay soil data
  for (int i = 0; i < 8; i++) {
    soilKnn.addTrainingData("clay", clay[i]);
  }
  
  // Add loamy soil data
  for (int i = 0; i < 8; i++) {
    soilKnn.addTrainingData("loamy", loamy[i]);
  }
  
  // Enable normalization for better accuracy
  soilKnn.enableNormalization(true);
  
  Serial.print("Total training samples: ");
  Serial.println(soilKnn.getDataCount());
}

float readMoisture() {
  // In a real application, read from a soil moisture sensor
  // For this example, simulate with analog reading
  int rawValue = analogRead(MOISTURE_PIN);
  // Convert to percentage (0-100%)
  float moisturePercent = map(rawValue, 0, 1023, 0, 100);
  return moisturePercent;
}

float readPH() {
  // In a real application, read from a pH sensor
  // For this example, simulate with analog reading
  int rawValue = analogRead(PH_PIN);
  // Convert to pH scale (0-14)
  float pH = map(rawValue, 0, 1023, 0, 140) / 10.0;
  return pH;
}

void provideRecommendations(const char* soilType) {
  Serial.println("\nRecommendations based on soil type:");
  
  if (strcmp(soilType, "sandy") == 0) {
    Serial.println("- Sandy soil drains quickly and warms up fast in spring");
    Serial.println("- Add compost to improve water retention");
    Serial.println("- Good for root vegetables like carrots and potatoes");
    Serial.println("- Water more frequently but in smaller amounts");
  }
  else if (strcmp(soilType, "clay") == 0) {
    Serial.println("- Clay soil holds nutrients well but drains poorly");
    Serial.println("- Add organic matter to improve drainage");
    Serial.println("- Good for flowering plants and vegetables like cabbage");
    Serial.println("- Water less frequently but deeply");
  }
  else if (strcmp(soilType, "loamy") == 0) {
    Serial.println("- Loamy soil is ideal for most plants");
    Serial.println("- Maintain with compost additions yearly");
    Serial.println("- Good for most vegetables and flowers");
    Serial.println("- Water moderately and consistently");
  }
  else {
    Serial.println("- Unknown soil type");
    Serial.println("- Consider adding more training data");
  }
}