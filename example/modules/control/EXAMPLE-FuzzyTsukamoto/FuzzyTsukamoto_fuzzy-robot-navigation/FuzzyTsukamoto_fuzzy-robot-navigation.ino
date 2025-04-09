/*
 * FuzzyTsukamoto - Robot Navigation Example
 * 
 * This example demonstrates a robot navigation controller
 * that uses three distance sensors to control motor speeds
 */

#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

// Pin definitions
#define LEFT_SENSOR_PIN A0
#define FRONT_SENSOR_PIN A1
#define RIGHT_SENSOR_PIN A2
#define LEFT_MOTOR_PIN 9
#define RIGHT_MOTOR_PIN 10

// Global variables
FuzzyTsukamoto fuzzy(3, 2, 7, 3);
unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
  
  // Add distance sensor inputs (0-100cm)
  fuzzy.addInputVariable("LeftDistance", 0.0, 100.0);
  fuzzy.addInputVariable("FrontDistance", 0.0, 100.0);
  fuzzy.addInputVariable("RightDistance", 0.0, 100.0);
  
  // Define input sets for distance sensors
  float close[] = {0.0, 0.0, 30.0};
  float medium[] = {20.0, 50.0, 80.0};
  float far[] = {70.0, 100.0, 100.0};
  
  // Add fuzzy sets for left distance
  fuzzy.addFuzzySet(0, true, "Close", TRIANGULAR, close);
  fuzzy.addFuzzySet(0, true, "Medium", TRIANGULAR, medium);
  fuzzy.addFuzzySet(0, true, "Far", TRIANGULAR, far);
  
  // Add fuzzy sets for front distance
  fuzzy.addFuzzySet(1, true, "Close", TRIANGULAR, close);
  fuzzy.addFuzzySet(1, true, "Medium", TRIANGULAR, medium);
  fuzzy.addFuzzySet(1, true, "Far", TRIANGULAR, far);
  
  // Add fuzzy sets for right distance
  fuzzy.addFuzzySet(2, true, "Close", TRIANGULAR, close);
  fuzzy.addFuzzySet(2, true, "Medium", TRIANGULAR, medium);
  fuzzy.addFuzzySet(2, true, "Far", TRIANGULAR, far);
  
  // Add motor speed outputs (0-255 for PWM)
  fuzzy.addOutputVariable("LeftMotor", 0.0, 255.0);
  fuzzy.addOutputVariable("RightMotor", 0.0, 255.0);
  
  // Define output sets for motor speeds
  float slow[] = {255.0, 0.0};
  float normal[] = {0.0, 127.0};
  float fast[] = {127.0, 255.0};
  
  // Add fuzzy sets for left motor
  fuzzy.addFuzzySet(0, false, "Slow", MONOTONIC_DECREASING, slow);
  fuzzy.addFuzzySet(0, false, "Normal", MONOTONIC_INCREASING, normal);
  fuzzy.addFuzzySet(0, false, "Fast", MONOTONIC_INCREASING, fast);
  
  // Add fuzzy sets for right motor
  fuzzy.addFuzzySet(1, false, "Slow", MONOTONIC_DECREASING, slow);
  fuzzy.addFuzzySet(1, false, "Normal", MONOTONIC_INCREASING, normal);
  fuzzy.addFuzzySet(1, false, "Fast", MONOTONIC_INCREASING, fast);
  
  // Add the rules
  
  // Rule 1: IF front is close THEN left motor is slow AND right motor is slow
  int antecedentVars1[] = {1};
  int antecedentSets1[] = {0}; // Front close
  fuzzy.addRule(antecedentVars1, antecedentSets1, 1, 0, 0, true); // Left slow
  fuzzy.addRule(antecedentVars1, antecedentSets1, 1, 1, 0, true); // Right slow
  
  // Rule 2: IF left is close THEN left motor is fast AND right motor is slow
  int antecedentVars2[] = {0};
  int antecedentSets2[] = {0}; // Left close
  fuzzy.addRule(antecedentVars2, antecedentSets2, 1, 0, 2, true); // Left fast
  fuzzy.addRule(antecedentVars2, antecedentSets2, 1, 1, 0, true); // Right slow
  
  // Rule 3: IF right is close THEN left motor is slow AND right motor is fast
  int antecedentVars3[] = {2};
  int antecedentSets3[] = {0}; // Right close
  fuzzy.addRule(antecedentVars3, antecedentSets3, 1, 0, 0, true); // Left slow
  fuzzy.addRule(antecedentVars3, antecedentSets3, 1, 1, 2, true); // Right fast
  
  // Rule 4: IF all distances are far THEN left motor is normal AND right motor is normal
  int antecedentVars4[] = {0, 1, 2};
  int antecedentSets4[] = {2, 2, 2}; // All far
  fuzzy.addRule(antecedentVars4, antecedentSets4, 3, 0, 1, true); // Left normal
  fuzzy.addRule(antecedentVars4, antecedentSets4, 3, 1, 1, true); // Right normal
  
  Serial.println("Robot navigation controller initialized");
}

void loop() {
  // Simulated sensor readings
  // In a real robot, these would come from actual sensors
  float timeOffset = millis() / 1000.0;
  float leftDistance = 50.0 + 40.0 * sin(timeOffset * 0.5);
  float frontDistance = 50.0 + 40.0 * sin(timeOffset * 0.3 + 2.0);
  float rightDistance = 50.0 + 40.0 * sin(timeOffset * 0.7 + 4.0);
  
  // Prepare input array
  float inputs[] = {leftDistance, frontDistance, rightDistance};
  
  // Evaluate the fuzzy system
  float* outputs = fuzzy.evaluate(inputs);
  
  if (outputs != nullptr) {
    // Apply the outputs to motors
    analogWrite(LEFT_MOTOR_PIN, (int)outputs[0]);
    analogWrite(RIGHT_MOTOR_PIN, (int)outputs[1]);
    
    // Print results every second
    if (millis() - lastPrintTime > 1000) {
      Serial.print("Distances - L: ");
      Serial.print(leftDistance, 1);
      Serial.print("cm, F: ");
      Serial.print(frontDistance, 1);
      Serial.print("cm, R: ");
      Serial.print(rightDistance, 1);
      Serial.print("cm → Motors - L: ");
      Serial.print(outputs[0], 1);
      Serial.print(", R: ");
      Serial.println(outputs[1], 1);
      lastPrintTime = millis();
    }
    
    delete[] outputs; // Free the memory
  } else {
    if (fuzzy.hasError()) {
      Serial.print("Error: ");
      Serial.println(fuzzy.getErrorMessage());
    }
  }
  
  delay(100);
}
