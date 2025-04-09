/**
 * AdvancedFeatures.ino - Demonstrating advanced features of the PIDController
 * 
 * This example shows how to use advanced features such as:
 * - Derivative filter
 * - Deadband
 * - Setpoint ramping
 * - Output rate limiting
 * - Performance metrics
 */

#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"

// Pin definitions
const int POSITION_SENSOR_PIN = 33;   // Position feedback sensor
const int MOTOR_PWM_PIN = 9;          // PWM output for motor
const int MOTOR_DIR_PIN = 8;          // Direction control pin

// Control parameters
const float KP = 2.0;              // Proportional gain
const float KI = 0.1;              // Integral gain
const float KD = 0.8;              // Derivative gain
const float TIME_STEP = 0.05;      // Control loop time step in seconds
const float MIN_OUTPUT = -255.0;   // Minimum output value
const float MAX_OUTPUT = 255.0;    // Maximum output value (PWM)

// Create the PID controller instance
PIDController pid(KP, KI, KD, TIME_STEP, MIN_OUTPUT, MAX_OUTPUT);

// Variables
float currentPosition = 0.0;
float controlOutput = 0.0;
unsigned long lastPrintTime = 0;    // Separate timer just for printing status
const unsigned long PRINT_INTERVAL = 500; // Print every 500ms
int currentMode = 0;         // 0=Off, 1=Basic, 2=DerivFilter, 3=Deadband, 4=Ramping, 5=RateLimit

// Function prototypes
void setupMode(int mode);
void printMenu();
float readPosition();
void applyMotorControl(float output);
void printStatus(unsigned long now);

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("PID Controller Advanced Features Example");
    Serial.println("----------------------------------------");

    // Initialize pins
    pinMode(MOTOR_PWM_PIN, OUTPUT);
    pinMode(MOTOR_DIR_PIN, OUTPUT);

    // Initialize to off state
    analogWrite(MOTOR_PWM_PIN, 0);
    digitalWrite(MOTOR_DIR_PIN, LOW);

    // Set up performance monitoring
    pid.setSettlingThreshold(0.02);  // 2% threshold for settling time

    // Print the menu
    printMenu();
}

void loop() {
    // Check for commands from serial
    if (Serial.available() > 0) {
        int command = Serial.parseInt();
        if (command >= 0 && command <= 5) {
            currentMode = command;
            setupMode(currentMode);
        }
    }

    // Read the position sensor
    currentPosition = readPosition();

    // Only compute and apply control if in an active mode
    if (currentMode > 0) {
        // Compute new control output - the PID controller handles timing internally
        controlOutput = pid.compute(currentPosition);

        // Apply the control output to the motor
        applyMotorControl(controlOutput);

        // Print status information on a separate timer to avoid flooding the serial monitor
        unsigned long now = millis();
        if (now - lastPrintTime >= PRINT_INTERVAL) {
            lastPrintTime = now;
            printStatus(now);

            // Check if settled
            if (pid.isSystemSettled()) {
                Serial.print("System settled! Settling time: ");
                Serial.print(pid.getSettlingTime());
                Serial.print(" seconds, Overshoot: ");
                Serial.print(pid.getOvershoot());
                Serial.println("%");
            }
        }
    }
}

// Set up the controller mode
void setupMode(int mode) {
    // Reset the controller for the new mode
    pid.reset();

    // Apply the appropriate settings for each mode
    switch (mode) {
        case 0:  // Off
            Serial.println("\nMode 0: Control OFF");
            analogWrite(MOTOR_PWM_PIN, 0);
            break;

        case 1:  // Basic PID
            Serial.println("\nMode 1: Basic PID Control");
            pid.setSetPoint(512.0);
            pid.disableDerivativeFilter();
            pid.setDeadband(0.0);
            pid.disableSetpointRamping();
            pid.disableOutputRateLimit();
            break;

        case 2:  // With Derivative Filter
            Serial.println("\nMode 2: PID with Derivative Filter");
            pid.setSetPoint(512.0);
            pid.enableDerivativeFilter(0.2);  // Alpha = 0.2 (lower = more filtering)
            pid.setDeadband(0.0);
            pid.disableSetpointRamping();
            pid.disableOutputRateLimit();
            Serial.println("Derivative filtering helps reduce noise sensitivity");
            break;

        case 3:  // With Deadband
            Serial.println("\nMode 3: PID with Deadband");
            pid.setSetPoint(512.0);
            pid.enableDerivativeFilter(0.2);
            pid.setDeadband(5.0);  // Errors less than 5 units are ignored
            pid.disableSetpointRamping();
            pid.disableOutputRateLimit();
            Serial.println("Deadband prevents control action for small errors");
            break;

        case 4:  // With Setpoint Ramping
            Serial.println("\nMode 4: PID with Setpoint Ramping");
            pid.setSetPoint(800.0);  // A larger setpoint to demonstrate ramping
            pid.enableDerivativeFilter(0.2);
            pid.setDeadband(0.0);
            pid.enableSetpointRamping(100.0);  // 100 units per second
            pid.disableOutputRateLimit();
            Serial.println("Setpoint ramping creates a gradual transition to the target");
            break;

        case 5:  // With Output Rate Limiting
            Serial.println("\nMode 5: PID with Output Rate Limiting");
            pid.setSetPoint(200.0);  // A smaller setpoint to demonstrate rate limiting
            pid.enableDerivativeFilter(0.2);
            pid.setDeadband(0.0);
            pid.disableSetpointRamping();
            pid.setOutputRateLimit(50.0);  // 50 units per second max change
            Serial.println("Output rate limiting prevents abrupt changes in control output");
            break;
    }

    if (mode > 0) {
        // For all active modes, show the PID parameters
        Serial.print("PID Parameters: Kp=");
        Serial.print(pid.getKp());
        Serial.print(", Ki=");
        Serial.print(pid.getKi());
        Serial.print(", Kd=");
        Serial.println(pid.getKd());
    }
}

// Print the menu of available modes
void printMenu() {
    Serial.println("\nAvailable Modes:");
    Serial.println("0 - Off");
    Serial.println("1 - Basic PID Control");
    Serial.println("2 - PID with Derivative Filter");
    Serial.println("3 - PID with Deadband");
    Serial.println("4 - PID with Setpoint Ramping");
    Serial.println("5 - PID with Output Rate Limiting");
    Serial.println("\nEnter mode number (0-5):");
}

// Function to read position from sensor
float readPosition() {
    // Read analog value (0-1023)
    int rawValue = analogRead(POSITION_SENSOR_PIN);
    return (float) rawValue;
}

// Apply motor control output
void applyMotorControl(float output) {
    if (output > 0) {
        // Forward direction
        digitalWrite(MOTOR_DIR_PIN, HIGH);
        analogWrite(MOTOR_PWM_PIN, (int) output);
    } else {
        // Reverse direction
        digitalWrite(MOTOR_DIR_PIN, LOW);
        analogWrite(MOTOR_PWM_PIN, (int) abs(output));
    }
}

// Print the current status
void printStatus(unsigned long now) {
    Serial.print("Time: ");
    Serial.print(now / 1000.0);
    Serial.print("s, Position: ");
    Serial.print(currentPosition);

    if (currentMode > 0) {
        Serial.print(", Target: ");
        if (currentMode == 4) {
            // Show ramped setpoint if in ramping mode
            Serial.print(pid.getCurrentRampedSetpoint());
            Serial.print(" (ramping to ");
            Serial.print(pid.getSetPoint());
            Serial.print(")");
        } else {
            Serial.print(pid.getSetPoint());
        }

        Serial.print(", Output: ");
        Serial.print(controlOutput);
        Serial.print(", P: ");
        Serial.print(pid.getProportionalComponent());
        Serial.print(", I: ");
        Serial.print(pid.getIntegralComponent());
        Serial.print(", D: ");
        Serial.println(pid.getDerivativeComponent());
    } else {
        Serial.println(" (Control OFF)");
    }
}