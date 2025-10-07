/*
 * FuzzySugeno_Robot_Navigation.ino
 * 
 * Robot obstacle avoidance using FuzzySugeno
 * Created on: 2025. 3. 30
 */

#define ENABLE_MODULE_FUZZY_SUGENO
#include "Kinematrix.h"

// Create a FuzzySugeno instance
FuzzySugeno robot(3, 2, 27, 3);

// Pins for sensors and motors
const int leftSensorPin = 1;   // Left ultrasonic sensor
const int centerSensorPin = 2; // Center ultrasonic sensor
const int rightSensorPin = 3;  // Right ultrasonic sensor

const int leftMotorPin = 9;     // Left motor speed control
const int rightMotorPin = 10;   // Right motor speed control

// Sensor and motor parameters
const int maxDistance = 200;    // Maximum sensor reading in cm
const int minDistance = 5;      // Minimum safe distance in cm

void setup() {
  Serial.begin(115200);
  Serial.println("FuzzySugeno Robot Navigation");
  Serial.println("----------------------------");
  
  // Setup pins
  pinMode(leftMotorPin, OUTPUT);
  pinMode(rightMotorPin, OUTPUT);
  
  // 1. Define input variables (distance sensors)
  robot.addInputVariable("left_distance", 0, maxDistance);
  robot.addInputVariable("center_distance", 0, maxDistance);
  robot.addInputVariable("right_distance", 0, maxDistance);
  
  // 2. Define fuzzy sets for distances
  // Very close (obstacle)
  float veryCloseParams[] = {0, 0, 20};
  // Close (caution)
  float closeParams[] = {10, 30, 50};
  // Far (clear path)
  float farParams[] = {40, maxDistance, maxDistance};
  
  // Add sets for left sensor
  robot.addFuzzySet(0, "very_close", TRIANGULAR, veryCloseParams);
  robot.addFuzzySet(0, "close", TRIANGULAR, closeParams);
  robot.addFuzzySet(0, "far", TRIANGULAR, farParams);
  
  // Add sets for center sensor
  robot.addFuzzySet(1, "very_close", TRIANGULAR, veryCloseParams);
  robot.addFuzzySet(1, "close", TRIANGULAR, closeParams);
  robot.addFuzzySet(1, "far", TRIANGULAR, farParams);
  
  // Add sets for right sensor
  robot.addFuzzySet(2, "very_close", TRIANGULAR, veryCloseParams);
  robot.addFuzzySet(2, "close", TRIANGULAR, closeParams);
  robot.addFuzzySet(2, "far", TRIANGULAR, farParams);
  
  // 3. Define outputs using Sugeno linear equations
  // Motor speeds based on sensor readings
  // Speed = a + b*left + c*center + d*right
  
  // Linear output for left motor
  float leftMotorCoeffs[] = {0.5, 0.001, 0.002, -0.003};
  robot.addLinearOutput("left_motor", leftMotorCoeffs, 4);
  
  // Linear output for right motor
  float rightMotorCoeffs[] = {0.5, -0.003, 0.002, 0.001};
  robot.addLinearOutput("right_motor", rightMotorCoeffs, 4);
  
  // 4. Define constant outputs for specific situations
  robot.addConstantOutput("stop", 0.0);
  robot.addConstantOutput("slow", 0.3);
  robot.addConstantOutput("medium", 0.6);
  robot.addConstantOutput("fast", 1.0);
  
  // 5. Define rules for navigation
  
  // All clear - go straight at full speed
  int allClearVars[] = {0, 1, 2};
  int allClearSets[] = {2, 2, 2}; // far, far, far
  robot.addRule(allClearVars, allClearSets, 3, 0, true); // left_motor (linear)
  robot.addRule(allClearVars, allClearSets, 3, 1, true); // right_motor (linear)
  
  // Obstacle on left - turn right
  int leftObstacleVars[] = {0, 1, 2};
  int leftObstacleSets[] = {0, 2, 2}; // very_close, far, far
  robot.addRule(leftObstacleVars, leftObstacleSets, 3, 2, true); // left_motor = slow
  robot.addRule(leftObstacleVars, leftObstacleSets, 3, 5, true); // right_motor = fast
  
  // Obstacle on right - turn left
  int rightObstacleVars[] = {0, 1, 2};
  int rightObstacleSets[] = {2, 2, 0}; // far, far, very_close
  robot.addRule(rightObstacleVars, rightObstacleSets, 3, 5, true); // left_motor = fast
  robot.addRule(rightObstacleVars, rightObstacleSets, 3, 2, true); // right_motor = slow
  
  // Center obstacle - turn based on side clearance
  int centerObstacleVars[] = {0, 1, 2};
  int centerObstacleSets[] = {2, 0, 2}; // far, very_close, far
  robot.addRule(centerObstacleVars, centerObstacleSets, 3, 5, true); // left_motor = fast
  robot.addRule(centerObstacleVars, centerObstacleSets, 3, 2, true); // right_motor = slow
  
  // Add similar rules for other combinations (center+left, center+right, etc.)
  int centerLeftObstacleVars[] = {0, 1, 2};
  int centerLeftObstacleSets[] = {0, 0, 2}; // very_close, very_close, far
  robot.addRule(centerLeftObstacleVars, centerLeftObstacleSets, 3, 2, true);  // left_motor = slow
  robot.addRule(centerLeftObstacleVars, centerLeftObstacleSets, 3, 5, true);  // right_motor = fast
  
  int centerRightObstacleVars[] = {0, 1, 2};
  int centerRightObstacleSets[] = {2, 0, 0}; // far, very_close, very_close
  robot.addRule(centerRightObstacleVars, centerRightObstacleSets, 3, 5, true); // left_motor = fast
  robot.addRule(centerRightObstacleVars, centerRightObstacleSets, 3, 2, true); // right_motor = slow
  
  // Obstacles everywhere - stop
  int allObstaclesVars[] = {0, 1, 2};
  int allObstaclesSets[] = {0, 0, 0}; // very_close, very_close, very_close
  robot.addRule(allObstaclesVars, allObstaclesSets, 3, 2, true); // left_motor = stop
  robot.addRule(allObstaclesVars, allObstaclesSets, 3, 2, true); // right_motor = stop
  
  if (robot.hasError()) {
    Serial.print("Error: ");
    Serial.println(robot.getErrorMessage());
  } else {
    Serial.println("Robot navigation system initialized successfully!");
  }
}

void loop() {
  // Read distance sensors (in cm)
  // For simulation, we'll create patterns
  
  static int counter = 0;
  counter = (counter + 1) % 400;
  
  // Simulate different scenarios
  int leftDistance, centerDistance, rightDistance;
  
  if (counter < 100) {
    // All clear
    leftDistance = 100;
    centerDistance = 100;
    rightDistance = 100;
  } else if (counter < 200) {
    // Obstacle on left
    leftDistance = 10;
    centerDistance = 100;
    rightDistance = 100;
  } else if (counter < 300) {
    // Obstacle on right
    leftDistance = 100;
    centerDistance = 100;
    rightDistance = 10;
  } else {
    // Obstacle in center
    leftDistance = 100;
    centerDistance = 10;
    rightDistance = 100;
  }
  
  // Input array for fuzzy system
  float inputs[] = {(float)leftDistance, (float)centerDistance, (float)rightDistance};
  
  // Evaluate fuzzy system
  float* outputs = robot.evaluate(inputs);
  
  if (outputs != nullptr) {
    float leftMotorSpeed = outputs[0];
    float rightMotorSpeed = outputs[1];
    
    // Apply to motor outputs (PWM)
    analogWrite(leftMotorPin, (int)(leftMotorSpeed * 255));
    analogWrite(rightMotorPin, (int)(rightMotorSpeed * 255));
    
    // Display results
    Serial.print("Distances - L: ");
    Serial.print(leftDistance);
    Serial.print("cm, C: ");
    Serial.print(centerDistance);
    Serial.print("cm, R: ");
    Serial.print(rightDistance);
    Serial.print("cm | Motors - L: ");
    Serial.print(int(leftMotorSpeed * 100));
    Serial.print("%, R: ");
    Serial.print(int(rightMotorSpeed * 100));
    Serial.println("%");
    
    delete[] outputs;
  }
  
  delay(200);
}
