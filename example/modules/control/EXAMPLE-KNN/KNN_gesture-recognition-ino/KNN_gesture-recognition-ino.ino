/*
 * Gesture Recognition using KNN
 * 
 * Menggunakan sensor accelerometer untuk mengenali gerakan tangan
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

#include <Wire.h>

// MPU6050 accelerometer addresses
const int MPU_ADDR = 0x68;
int16_t accelX, accelY, accelZ;

// Parameter KNN
const int K = 3;
const int MAX_FEATURES = 3; // AccelX, AccelY, AccelZ
const int MAX_SAMPLES = 40;

// Create KNN instance
KNN gestureKnn(K, MAX_FEATURES, MAX_SAMPLES);

// Tracking mode
bool isTraining = false;
char currentGesture[20] = "";
int sampleCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Wire.begin();
  initMPU();
  
  Serial.println("Gesture Recognition using KNN");
  Serial.println("-----------------------------");
  
  // Train with default gestures
  trainGestureModel();
  
  Serial.println("\nCommands:");
  Serial.println("  'p' - Predict gesture");
  Serial.println("  't [gesture_name]' - Start training a new gesture");
  Serial.println("  's' - Stop training");
  Serial.println("  'c' - Clear all training data");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    
    if (input.startsWith("t ")) {
      // Start training mode
      input.substring(2).toCharArray(currentGesture, 20);
      isTraining = true;
      sampleCount = 0;
      Serial.print("Training mode started for gesture '");
      Serial.print(currentGesture);
      Serial.println("'. Make the gesture repeatedly.");
      Serial.println("Type 's' to stop training.");
    } 
    else if (input == "s") {
      // Stop training mode
      isTraining = false;
      Serial.println("Training stopped.");
    }
    else if (input == "p") {
      // Prediction mode
      readAccelerometer();
      predictGesture();
    }
    else if (input == "c") {
      // Clear all training data
      gestureKnn.clearTrainingData();
      Serial.println("All training data cleared.");
    }
  }
  
  if (isTraining) {
    // Collect training data
    if (sampleCount < 10) { // Collect 10 samples per gesture
      readAccelerometer();
      
      // Normalize data by dividing by 16384 (full scale range)
      float features[3] = {
        accelX / 16384.0f, 
        accelY / 16384.0f, 
        accelZ / 16384.0f
      };
      
      gestureKnn.addTrainingData(currentGesture, features);
      sampleCount++;
      
      Serial.print("Sample ");
      Serial.print(sampleCount);
      Serial.println(" added");
      
      delay(500); // Wait between samples
    } else {
      Serial.print("Training complete for gesture '");
      Serial.print(currentGesture);
      Serial.print("'. Total samples: ");
      Serial.println(gestureKnn.getDataCount());
      isTraining = false;
    }
  }
}

void initMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Wake up MPU-6050
  Wire.endTransmission(true);
  
  // Configure accelerometer for +/- 2g range
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);  // ACCEL_CONFIG register
  Wire.write(0x00);  // 2g range
  Wire.endTransmission(true);
  
  Serial.println("MPU6050 initialized");
}

void readAccelerometer() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // Start with ACCEL_XOUT_H register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);  // Request 6 bytes
  
  accelX = Wire.read() << 8 | Wire.read();  // ACCEL_XOUT_H & ACCEL_XOUT_L
  accelY = Wire.read() << 8 | Wire.read();  // ACCEL_YOUT_H & ACCEL_YOUT_L
  accelZ = Wire.read() << 8 | Wire.read();  // ACCEL_ZOUT_H & ACCEL_ZOUT_L
}

void predictGesture() {
  // Normalize data
  float features[3] = {
    accelX / 16384.0f, 
    accelY / 16384.0f, 
    accelZ / 16384.0f
  };
  
  // Make prediction
  const char* prediction = gestureKnn.predict(features);
  float confidence = gestureKnn.getPredictionConfidence(features);
  
  // Display results
  Serial.print("Accel X: ");
  Serial.print(accelX);
  Serial.print(", Y: ");
  Serial.print(accelY);
  Serial.print(", Z: ");
  Serial.print(accelZ);
  Serial.print(" -> Gesture: ");
  Serial.print(prediction);
  Serial.print(" (Confidence: ");
  Serial.print(confidence * 100);
  Serial.println("%)");
}

void trainGestureModel() {
  Serial.println("Training gesture recognition model...");
  
  // Sample data for "right" gesture
  float right[][3] = {
    {0.7, 0.1, 0.3}, {0.8, 0.2, 0.2}, {0.6, 0.1, 0.3}, {0.7, 0.0, 0.4}
  };
  
  // Sample data for "left" gesture
  float left[][3] = {
    {-0.7, 0.1, 0.3}, {-0.8, 0.2, 0.2}, {-0.6, 0.1, 0.3}, {-0.7, 0.0, 0.4}
  };
  
  // Sample data for "up" gesture
  float up[][3] = {
    {0.1, 0.7, 0.3}, {0.2, 0.8, 0.2}, {0.1, 0.6, 0.3}, {0.0, 0.7, 0.4}
  };
  
  // Sample data for "down" gesture
  float down[][3] = {
    {0.1, -0.7, 0.3}, {0.2, -0.8, 0.2}, {0.1, -0.6, 0.3}, {0.0, -0.7, 0.4}
  };
  
  // Add gesture data
  for (int i = 0; i < 4; i++) {
    gestureKnn.addTrainingData("right", right[i]);
    gestureKnn.addTrainingData("left", left[i]);
    gestureKnn.addTrainingData("up", up[i]);
    gestureKnn.addTrainingData("down", down[i]);
  }
  
  // Enable normalization and weighted voting
  gestureKnn.enableNormalization(true);
  gestureKnn.setWeightedVoting(true);
  
  Serial.print("Default training samples: ");
  Serial.println(gestureKnn.getDataCount());
}