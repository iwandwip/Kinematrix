/**
 * ZieglerNicholsMethod2.ino - Example of using Ziegler-Nichols Method 2 tuning
 * 
 * This example demonstrates how to use the autoTuneZN2 method to automatically
 * tune the PID parameters using the Ziegler-Nichols second method (ultimate gain).
 */

#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"

// Pin definitions
const int MOTOR_POSITION_PIN = 33;    // For reading position feedback
const int MOTOR_CONTROL_PIN = 9;      // PWM output to motor driver

// Control parameters
const float SETPOINT = 512.0;         // Target position (0-1023)
const float INITIAL_KP = 0.1;         // Initial proportional gain
const float INITIAL_KI = 0.0;         // Initial integral gain
const float INITIAL_KD = 0.0;         // Initial derivative gain
const float TIME_STEP = 0.05;         // Control loop time step in seconds
const float MIN_OUTPUT = -255.0;      // Minimum output value
const float MAX_OUTPUT = 255.0;       // Maximum output value (PWM)

// Auto-tuning parameters
const float INITIAL_KP_TUNING = 0.2;  // Starting Kp for tuning
const float KP_STEP = 0.2;            // Increment for Kp during tuning
const unsigned long MAX_TUNING_TIME = 120000; // Max tuning time (2 minutes)

// EEPROM settings
const int EEPROM_ADDRESS = 0;         // Address to store PID parameters

// Create the PID controller instance
PIDController pid(INITIAL_KP, INITIAL_KI, INITIAL_KD, TIME_STEP, MIN_OUTPUT, MAX_OUTPUT);

// Variables
float currentPosition = 0.0;
float controlOutput = 0.0;
unsigned long lastPrintTime = 0;    // Timer just for printing status
const unsigned long PRINT_INTERVAL = 500; // Print every 500ms
bool tuningComplete = false;
bool autoTuneRequested = false;
bool normalControlMode = true;

// Function prototypes
void processCommand(char command);
float readPosition();
void applyMotorControl(float output);
void printPIDParameters();
void printStatus(unsigned long now);
void reportTuningResults();

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("Ziegler-Nichols Method 2 Auto-Tuning Example");
    Serial.println("---------------------------------------------");

    // Initialize pins
    pinMode(MOTOR_CONTROL_PIN, OUTPUT);

    // Set up EEPROM for parameter storage
    pid.setEEPROMAddress(EEPROM_ADDRESS);

    // Try to load parameters from EEPROM
    if (pid.loadParametersFromEEPROM()) {
        Serial.println("Loaded PID parameters from EEPROM:");
    } else {
        Serial.println("No valid parameters in EEPROM, using defaults:");
    }

    // Set the target setpoint
    pid.setSetPoint(SETPOINT);

    // Show initial parameters
    printPIDParameters();

    Serial.println("\nCommands:");
    Serial.println("  't' - Start auto-tuning");
    Serial.println("  'c' - Cancel auto-tuning");
    Serial.println("  's' - Save parameters to EEPROM");
    Serial.println("  'r' - Run normal control with current parameters");
}

void loop() {
    // Check for commands from serial
    if (Serial.available() > 0) {
        char command = Serial.read();
        processCommand(command);
    }

    // Read the position sensor
    currentPosition = readPosition();

    // If we're in tuning or normal control mode, compute the output
    if (normalControlMode || pid.isTuning()) {
        // Compute new control output - the PID controller handles timing internally
        controlOutput = pid.compute(currentPosition);

        // Apply the control output to the motor
        applyMotorControl(controlOutput);
    }

    // Print status information on a separate timer to avoid flooding the serial monitor
    unsigned long now = millis();
    if (now - lastPrintTime >= PRINT_INTERVAL) {
        lastPrintTime = now;
        printStatus(now);

        // If we were tuning but it's now complete, report the results
        if (autoTuneRequested && !pid.isTuning() && !tuningComplete) {
            tuningComplete = true;
            reportTuningResults();
        }
    }
}

// Process serial commands
void processCommand(char command) {
    switch (command) {
        case 't': // Start auto-tuning
            Serial.println("\nStarting Ziegler-Nichols Method 2 auto-tuning...");
            tuningComplete = false;
            autoTuneRequested = true;
            normalControlMode = false;

            // Begin auto-tuning with ZN2 method
            if (pid.autoTuneZN2(SETPOINT, INITIAL_KP_TUNING, KP_STEP, MAX_TUNING_TIME)) {
                Serial.println("Auto-tuning started. Kp will increase gradually until oscillation.");
                Serial.println("This may cause the system to oscillate strongly. Be prepared to cancel if necessary.");
            } else {
                Serial.println("Failed to start auto-tuning!");
                autoTuneRequested = false;
                normalControlMode = true;
            }
            break;

        case 'c': // Cancel auto-tuning
            if (pid.isTuning()) {
                pid.cancelTuning();
                Serial.println("\nAuto-tuning cancelled.");
                autoTuneRequested = false;
                normalControlMode = true;
            }
            break;

        case 's': // Save parameters to EEPROM
            if (pid.saveParametersToEEPROM()) {
                Serial.println("\nParameters saved to EEPROM successfully.");
            } else {
                Serial.println("\nFailed to save parameters to EEPROM!");
            }
            break;

        case 'r': // Run normal control
            Serial.println("\nRunning normal control with current parameters.");
            pid.setSetPoint(SETPOINT);
            normalControlMode = true;
            break;
    }
}

// Function to read position from sensor
float readPosition() {
    // Read analog value (0-1023)
    int rawValue = analogRead(MOTOR_POSITION_PIN);
    return (float) rawValue;
}

// Apply motor control output
void applyMotorControl(float output) {
    // For a bidirectional motor control
    if (output > 0) {
        // Forward direction at specified speed
        analogWrite(MOTOR_CONTROL_PIN, (int) output);
    } else {
        // Reverse direction at specified speed (using absolute value)
        analogWrite(MOTOR_CONTROL_PIN, (int) abs(output));
    }
}

// Print the current PID parameters
void printPIDParameters() {
    Serial.print("Kp: ");
    Serial.print(pid.getKp());
    Serial.print(", Ki: ");
    Serial.print(pid.getKi());
    Serial.print(", Kd: ");
    Serial.println(pid.getKd());
}

// Print the current status
void printStatus(unsigned long now) {
    Serial.print("Time: ");
    Serial.print(now / 1000.0);
    Serial.print("s, Position: ");
    Serial.print(currentPosition);
    Serial.print(", Output: ");
    Serial.print(controlOutput);

    if (pid.isTuning()) {
        Serial.print(" (TUNING IN PROGRESS, Kp = ");
        Serial.print(pid.getKp());
        Serial.println(")");
    } else if (normalControlMode) {
        Serial.print(", Setpoint: ");
        Serial.print(pid.getSetPoint());
        Serial.print(", P: ");
        Serial.print(pid.getProportionalComponent());
        Serial.print(", I: ");
        Serial.print(pid.getIntegralComponent());
        Serial.print(", D: ");
        Serial.println(pid.getDerivativeComponent());
    } else {
        Serial.println(" (IDLE)");
    }
}

// Report the results of the tuning process
void reportTuningResults() {
    Serial.println("\n-------- TUNING COMPLETE --------");
    Serial.println("Auto-tuning results:");
    Serial.print("Ultimate Gain (Ku): ");
    Serial.println(pid.getUltimateGain());
    Serial.print("Ultimate Period (Pu): ");
    Serial.print(pid.getUltimatePeriod());
    Serial.println(" ms");

    Serial.println("\nNew PID parameters based on Ziegler-Nichols Method 2:");
    printPIDParameters();

    Serial.println("\nType 's' to save these parameters to EEPROM");
    Serial.println("Type 'r' to start normal control with the new parameters");
}