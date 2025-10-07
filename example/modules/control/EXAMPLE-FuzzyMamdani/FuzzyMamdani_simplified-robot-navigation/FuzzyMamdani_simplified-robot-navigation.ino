/*
 * FuzzyMamdani_Simplified_Robot_Navigation.ino
 * 
 * Example of using the FuzzyMamdani library to control
 * a robot's navigation based on obstacle distance sensors
 * All code contained in setup() and loop() only
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Create fuzzy system with:
// - 3 input variables (left, front, right sensors)
// - 2 output variables (speed, steering)
// - 27 rules maximum
// - 3 fuzzy sets per variable
FuzzyMamdani fuzzy(3, 2, 27, 3);

// Motor control pins (for demonstration only)
const int LEFT_MOTOR_PIN = 5;   // PWM pin for left motor
const int RIGHT_MOTOR_PIN = 6;  // PWM pin for right motor

// Test scenarios
struct SensorScenario {
  float leftDist;
  float frontDist;
  float rightDist;
  const char* description;
};

// Define test scenarios
SensorScenario scenarios[] = {
  {50, 15, 50, "Obstacle directly ahead"},
  {20, 50, 50, "Obstacle on left side"},
  {50, 50, 20, "Obstacle on right side"},
  {10, 10, 80, "Tight passage on left"},
  {80, 10, 10, "Tight passage on right"},
  {80, 80, 80, "Clear path in all directions"},
  {10, 10, 10, "Surrounded by obstacles"}
};

int currentScenario = 0;
unsigned long lastTestTime = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Fuzzy Logic Robot Navigation Controller");
  Serial.println("---------------------------------------");
  
  // Setup motor control pins
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
  
  // Add input variables for the three distance sensors (in cm)
  fuzzy.addInputVariable("left_distance", 0, 100);
  fuzzy.addInputVariable("front_distance", 0, 100);
  fuzzy.addInputVariable("right_distance", 0, 100);
  
  // Add output variables
  fuzzy.addOutputVariable("speed", 0, 100);         // Speed as percentage
  fuzzy.addOutputVariable("steering", -100, 100);   // -100 (full left) to 100 (full right)
  
  // Add fuzzy sets for distance sensors
  // Using trapezoidal membership functions for smoother transitions
  float closeParams[4] = {0, 0, 20, 40};
  float mediumParams[4] = {20, 40, 60, 80};
  float farParams[4] = {60, 80, 100, 100};
  
  // Left sensor fuzzy sets
  fuzzy.addFuzzySet(0, true, "close", TRAPEZOIDAL, closeParams);
  fuzzy.addFuzzySet(0, true, "medium", TRAPEZOIDAL, mediumParams);
  fuzzy.addFuzzySet(0, true, "far", TRAPEZOIDAL, farParams);
  
  // Front sensor fuzzy sets
  fuzzy.addFuzzySet(1, true, "close", TRAPEZOIDAL, closeParams);
  fuzzy.addFuzzySet(1, true, "medium", TRAPEZOIDAL, mediumParams);
  fuzzy.addFuzzySet(1, true, "far", TRAPEZOIDAL, farParams);
  
  // Right sensor fuzzy sets
  fuzzy.addFuzzySet(2, true, "close", TRAPEZOIDAL, closeParams);
  fuzzy.addFuzzySet(2, true, "medium", TRAPEZOIDAL, mediumParams);
  fuzzy.addFuzzySet(2, true, "far", TRAPEZOIDAL, farParams);
  
  // Add fuzzy sets for speed output
  float stopParams[4] = {0, 0, 10, 20};
  float slowParams[4] = {10, 30, 50, 70};
  float fastParams[4] = {50, 70, 100, 100};
  
  fuzzy.addFuzzySet(0, false, "stop", TRAPEZOIDAL, stopParams);
  fuzzy.addFuzzySet(0, false, "slow", TRAPEZOIDAL, slowParams);
  fuzzy.addFuzzySet(0, false, "fast", TRAPEZOIDAL, fastParams);
  
  // Add fuzzy sets for steering output
  float hardLeftParams[4] = {-100, -100, -70, -40};
  float leftParams[4] = {-60, -30, -10, 0};
  float centerParams[4] = {-20, 0, 0, 20};
  float rightParams[4] = {0, 10, 30, 60};
  float hardRightParams[4] = {40, 70, 100, 100};
  
  fuzzy.addFuzzySet(1, false, "hard_left", TRAPEZOIDAL, hardLeftParams);
  fuzzy.addFuzzySet(1, false, "left", TRAPEZOIDAL, leftParams);
  fuzzy.addFuzzySet(1, false, "center", TRAPEZOIDAL, centerParams);
  fuzzy.addFuzzySet(1, false, "right", TRAPEZOIDAL, rightParams);
  fuzzy.addFuzzySet(1, false, "hard_right", TRAPEZOIDAL, hardRightParams);
  
  // Define key rules for the robot navigation
  
  // Rule 1: If front obstacle is close, stop and look for space
  int vars1[1] = {1}; // Front sensor
  int sets1[1] = {0}; // Close
  fuzzy.addRule(vars1, sets1, 1, 0, 0, true); // Output: speed = stop
  
  // Rule 2: If front is close and left is closer than right, turn hard right
  int vars2[3] = {0, 1, 2}; // Left, front, right sensors
  int sets2[3] = {0, 0, 1}; // Left close, front close, right medium
  fuzzy.addRule(vars2, sets2, 3, 1, 4, true); // Output: steering = hard_right
  
  // Rule 3: If front is close and right is closer than left, turn hard left
  int vars3[3] = {0, 1, 2};
  int sets3[3] = {1, 0, 0}; // Left medium, front close, right close
  fuzzy.addRule(vars3, sets3, 3, 1, 0, true); // Output: steering = hard_left
  
  // Rule 4: If front is medium and left is close, turn right
  int vars4[2] = {0, 1};
  int sets4[2] = {0, 1}; // Left close, front medium
  fuzzy.addRule(vars4, sets4, 2, 1, 3, true); // Output: steering = right
  
  // Rule 5: If front is medium and right is close, turn left
  int vars5[2] = {1, 2};
  int sets5[2] = {1, 0}; // Front medium, right close
  fuzzy.addRule(vars5, sets5, 2, 1, 1, true); // Output: steering = left
  
  // Rule 6: If all sensors read far, go fast straight ahead
  int vars6[3] = {0, 1, 2};
  int sets6[3] = {2, 2, 2}; // All far
  fuzzy.addRule(vars6, sets6, 3, 0, 2, true); // Output: speed = fast
  fuzzy.addRule(vars6, sets6, 3, 1, 2, true); // Output: steering = center
  
  // Rule 7: If front is far and (left or right is close), go at medium speed
  int vars7[2] = {0, 1};
  int sets7[2] = {0, 2}; // Left close, front far
  fuzzy.addRule(vars7, sets7, 2, 0, 1, true); // Output: speed = slow
  
  int vars8[2] = {1, 2};
  int sets8[2] = {2, 0}; // Front far, right close
  fuzzy.addRule(vars8, sets8, 2, 0, 1, true); // Output: speed = slow
  
  // Rule 8: If left and right are medium or far but front is medium, go slow
  int vars9[1] = {1};
  int sets9[1] = {1}; // Front medium
  fuzzy.addRule(vars9, sets9, 1, 0, 1, true); // Output: speed = slow
  
  // Set defuzzification method
  fuzzy.setDefuzzificationMethod(CENTROID);
  
  if (fuzzy.hasError()) {
    Serial.print("Error setting up fuzzy system: ");
    Serial.println(fuzzy.getErrorMessage());
    return;
  }
  
  Serial.println("Robot controller initialized successfully");
  Serial.println("Test with various sensor readings:");
  
  lastTestTime = millis();
}

void loop() {
  // Run a new scenario every 3 seconds
  if (millis() - lastTestTime >= 3000) {
    lastTestTime = millis();
    
    // Get current scenario
    SensorScenario scenario = scenarios[currentScenario];
    
    // Move to next scenario for next iteration
    currentScenario = (currentScenario + 1) % (sizeof(scenarios) / sizeof(SensorScenario));
    
    // Print scenario information
    Serial.println("\n--- " + String(scenario.description) + " ---");
    Serial.println("Left sensor: " + String(scenario.leftDist, 1) + " cm");
    Serial.println("Front sensor: " + String(scenario.frontDist, 1) + " cm");
    Serial.println("Right sensor: " + String(scenario.rightDist, 1) + " cm");
    
    // Create input array and evaluate the fuzzy system
    float inputs[3] = {scenario.leftDist, scenario.frontDist, scenario.rightDist};
    float* outputs = fuzzy.evaluate(inputs);
    
    if (outputs != nullptr) {
      float speed = outputs[0];
      float steering = outputs[1];
      
      Serial.println("Speed output: " + String(speed, 1) + "%");
      Serial.println("Steering output: " + String(steering, 1) + " (-100=left, 100=right)");
      
      // Calculate motor power (demonstration of how to use the outputs)
      float leftPower, rightPower;
      
      if (steering < 0) {  // Turn left
        leftPower = speed * (1 + steering/100); // Reduce left motor
        rightPower = speed;
      } else {             // Turn right or straight
        leftPower = speed;
        rightPower = speed * (1 - steering/100); // Reduce right motor
      }
      
      // Ensure values are in 0-100 range
      leftPower = constrain(leftPower, 0, 100);
      rightPower = constrain(rightPower, 0, 100);
      
      Serial.println("Left motor power: " + String(leftPower, 1) + "%");
      Serial.println("Right motor power: " + String(rightPower, 1) + "%");
      
      // In a real system, you would set the motor PWM values here:
      // analogWrite(LEFT_MOTOR_PIN, map(leftPower, 0, 100, 0, 255));
      // analogWrite(RIGHT_MOTOR_PIN, map(rightPower, 0, 100, 0, 255));
      
      delete[] outputs; // Free the allocated memory
    } else {
      Serial.print("Error during evaluation: ");
      Serial.println(fuzzy.getErrorMessage());
    }
  }
  
  // Add a small delay to avoid busy-looping
  delay(100);
}
