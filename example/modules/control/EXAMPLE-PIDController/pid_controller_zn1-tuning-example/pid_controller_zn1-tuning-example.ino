/**
 * ZieglerNicholsMethod1.ino - Example of using Ziegler-Nichols Method 1 tuning
 * 
 * This example demonstrates how to use the autoTuneZN1 method to automatically
 * tune the PID parameters using the Ziegler-Nichols first method (step response).
 */

#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"

// Pin definitions
const int TEMP_SENSOR_PIN = A0;
const int HEATER_PIN = 9;

// Control parameters
const float INITIAL_SETPOINT = 30.0;  // Starting temperature in Celsius
const float TARGET_SETPOINT = 60.0;   // Target temperature in Celsius
const float INITIAL_KP = 1.0;         // Initial proportional gain
const float INITIAL_KI = 0.0;         // Initial integral gain
const float INITIAL_KD = 0.0;         // Initial derivative gain
const float TIME_STEP = 0.1;          // Control loop time step in seconds
const float MIN_OUTPUT = 0.0;         // Minimum output value
const float MAX_OUTPUT = 255.0;       // Maximum output value (PWM)

// Auto-tuning parameters
const float STEP_AMPLITUDE = 50.0;    // Step size for tuning
const unsigned long MAX_TUNING_TIME = 180000; // Max tuning time (3 minutes)

// EEPROM settings
const int EEPROM_ADDRESS = 0;         // Address to store PID parameters

// Create the PID controller instance
PIDController pid(INITIAL_KP, INITIAL_KI, INITIAL_KD, TIME_STEP, MIN_OUTPUT, MAX_OUTPUT);

// Variables
float currentTemperature = 0.0;
float controlOutput = 0.0;
unsigned long lastPrintTime = 0;    // Timer just for printing status
const unsigned long PRINT_INTERVAL = 1000; // Print every second
bool tuningComplete = false;
bool autoTuneRequested = false;
bool normalControlMode = true;

// Function prototypes
void processCommand(char command);
float readTemperature();
void printPIDParameters();
void printStatus(unsigned long now);
void reportTuningResults();

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("Ziegler-Nichols Method 1 Auto-Tuning Example");
    Serial.println("---------------------------------------------");

    // Initialize pins
    pinMode(HEATER_PIN, OUTPUT);

    // Set up EEPROM for parameter storage
    pid.setEEPROMAddress(EEPROM_ADDRESS);

    // Try to load parameters from EEPROM
    if (pid.loadParametersFromEEPROM()) {
        Serial.println("Loaded PID parameters from EEPROM:");
    } else {
        Serial.println("No valid parameters in EEPROM, using defaults:");
    }

    // Set the initial setpoint
    pid.setSetPoint(INITIAL_SETPOINT);

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

    // Read the temperature sensor
    currentTemperature = readTemperature();

    // If we're in tuning or normal control mode, compute the output
    if (normalControlMode || pid.isTuning()) {
        // Compute new control output - the PID controller handles timing internally
        controlOutput = pid.compute(currentTemperature);

        // Apply the control output to the heater
        analogWrite(HEATER_PIN, (int) controlOutput);
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
            Serial.println("\nStarting Ziegler-Nichols Method 1 auto-tuning...");
            tuningComplete = false;
            autoTuneRequested = true;
            normalControlMode = false;

            // Begin auto-tuning with ZN1 method
            if (pid.autoTuneZN1(currentTemperature, STEP_AMPLITUDE, MAX_TUNING_TIME)) {
                Serial.println("Auto-tuning started. Please wait...");
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
            pid.setSetPoint(TARGET_SETPOINT);
            normalControlMode = true;
            break;
    }
}

// Function to read temperature from sensor
float readTemperature() {
    // Read analog value
    int rawValue = analogRead(TEMP_SENSOR_PIN);

    // Convert to temperature (example for LM35)
    float temperature = (rawValue * 0.0048828125 * 100.0);

    return temperature;
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
    Serial.print("s, Temp: ");
    Serial.print(currentTemperature);
    Serial.print("°C, Output: ");
    Serial.print(controlOutput);

    if (pid.isTuning()) {
        Serial.println(" (TUNING IN PROGRESS)");
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
    Serial.println("New PID parameters based on Ziegler-Nichols Method 1:");
    printPIDParameters();
    Serial.println("\nType 's' to save these parameters to EEPROM");
    Serial.println("Type 'r' to start normal control with the new parameters");
}