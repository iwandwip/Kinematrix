/*
 * Currency Identification using KNN
 * 
 * Menggunakan sensor warna RGB untuk mengidentifikasi mata uang
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Parameter KNN
const int K = 5;
const int MAX_FEATURES = 3; // R, G, B values
const int MAX_SAMPLES = 50;

// Create KNN instance
KNN currencyKnn(K, MAX_FEATURES, MAX_SAMPLES);

// Color sensor pins (TCS3200 sensor)
const int S0 = 4;
const int S1 = 5;
const int S2 = 6;
const int S3 = 7;
const int sensorOut = 8;

// Variables for color values
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Currency Identification using KNN");
  Serial.println("--------------------------------");
  
  // Set the color sensor pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  
  // Train the model with sample currency data
  trainCurrencyModel();
  
  Serial.println("Model trained. Ready for currency identification.");
  Serial.println("Place a banknote under the sensor and press any key...");
}

void loop() {
  if (Serial.available() > 0) {
    // Clear the serial buffer
    while (Serial.available() > 0) {
      Serial.read();
    }
    
    // Read color
    readColor();
    
    // Create feature array
    float features[3] = {(float)redColor, (float)greenColor, (float)blueColor};
    
    // Make prediction
    const char* prediction = currencyKnn.predict(features);
    float confidence = currencyKnn.getPredictionConfidence(features);
    
    // Display results
    Serial.print("RGB: (");
    Serial.print(redColor);
    Serial.print(", ");
    Serial.print(greenColor);
    Serial.print(", ");
    Serial.print(blueColor);
    Serial.print("), Predicted Currency: ");
    Serial.print(prediction);
    Serial.print(" (Confidence: ");
    Serial.print(confidence * 100);
    Serial.println("%)");
    
    Serial.println("\nPlace another banknote and press any key...");
  }
}

void trainCurrencyModel() {
  Serial.println("Training currency identification model...");
  
  // Sample RGB values for IDR 100,000 (red)
  float idr100k[][3] = {
    {220, 45, 50}, {215, 50, 55}, {225, 40, 48}, {218, 47, 52},
    {222, 43, 49}, {217, 46, 53}, {223, 42, 48}, {219, 45, 51}
  };
  
  // Sample RGB values for IDR 50,000 (blue)
  float idr50k[][3] = {
    {50, 60, 200}, {55, 65, 205}, {48, 58, 195}, {52, 62, 202},
    {51, 63, 198}, {53, 64, 203}, {49, 61, 197}, {54, 63, 201}
  };
  
  // Sample RGB values for IDR 20,000 (green)
  float idr20k[][3] = {
    {40, 180, 60}, {45, 185, 55}, {38, 178, 62}, {42, 182, 58},
    {41, 179, 61}, {44, 183, 57}, {39, 177, 63}, {43, 181, 59}
  };
  
  // Sample RGB values for IDR 10,000 (purple)
  float idr10k[][3] = {
    {150, 40, 180}, {155, 45, 185}, {148, 38, 178}, {152, 42, 182},
    {151, 41, 179}, {154, 44, 183}, {149, 39, 177}, {153, 43, 181}
  };
  
  // Add IDR 100,000 data
  for (int i = 0; i < 8; i++) {
    currencyKnn.addTrainingData("IDR 100,000", idr100k[i]);
  }
  
  // Add IDR 50,000 data
  for (int i = 0; i < 8; i++) {
    currencyKnn.addTrainingData("IDR 50,000", idr50k[i]);
  }
  
  // Add IDR 20,000 data
  for (int i = 0; i < 8; i++) {
    currencyKnn.addTrainingData("IDR 20,000", idr20k[i]);
  }
  
  // Add IDR 10,000 data
  for (int i = 0; i < 8; i++) {
    currencyKnn.addTrainingData("IDR 10,000", idr10k[i]);
  }
  
  // Enable normalization for better accuracy
  currencyKnn.enableNormalization(true);
  
  Serial.print("Total training samples: ");
  Serial.println(currencyKnn.getDataCount());
}

void readColor() {
  // Read Red value
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redColor = pulseIn(sensorOut, LOW);
  redColor = map(redColor, 70, 120, 255, 0);
  delay(100);
  
  // Read Green value
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenColor = pulseIn(sensorOut, LOW);
  greenColor = map(greenColor, 100, 199, 255, 0);
  delay(100);
  
  // Read Blue value
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueColor = pulseIn(sensorOut, LOW);
  blueColor = map(blueColor, 38, 84, 255, 0);
  delay(100);
  
  // Constrain values to 0-255
  redColor = constrain(redColor, 0, 255);
  greenColor = constrain(greenColor, 0, 255);
  blueColor = constrain(blueColor, 0, 255);
}