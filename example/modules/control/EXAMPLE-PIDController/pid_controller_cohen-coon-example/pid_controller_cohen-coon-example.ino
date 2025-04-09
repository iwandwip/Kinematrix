/**
 * CohenCoonTuning.ino - Example of using Cohen-Coon tuning method
 * 
 * This example demonstrates how to use the autoTuneCohenCoon method to automatically
 * tune the PID parameters. Cohen-Coon is particularly useful for processes with
 * significant dead time.
 */

#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"

// Pin definitions
const int FLOW_SENSOR_PIN = 33;   // Analog input for flow rate
const int VALVE_CONTROL_PIN = 9;  // PWM output to control valve position

// Control parameters
const float SETPOINT = 50.0;          // Target flow rate (%)
const float INITIAL_KP = 1.0;         // Initial proportional gain
const float INITIAL_KI = 0.0;         // Initial integral gain
const float INITIAL_KD = 0.0;         // Initial derivative gain
const float TIME_STEP = 0.1;          // Control loop time step in seconds
const float MIN_OUTPUT = 0.0;         // Minimum output value
const float MAX_OUTPUT = 255.0;       // Maximum output value (PWM)

// Auto-tuning parameters
const float STEP_AMPLITUDE = 40.0;    // Step size for tuning (valve position)
const unsigned long MAX_TUNING_TIME = 300000; // Max tuning time (5 minutes)

// EEPROM settings
const int EEPROM_ADDRESS = 0;         // Address to store PID parameters

// Create the PID controller instance
PIDController pid(INITIAL_KP, INITIAL_KI, INITIAL_KD, TIME_STEP, MIN_OUTPUT, MAX_OUTPUT);

// Variables
float currentFlowRate = 0.0;
float controlOutput = 0.0;
unsigned long lastPrintTime = 0;    // Timer just for printing status
const unsigned long PRINT_INTERVAL = 1000; // Print every second
bool tuningComplete = false;
bool autoTuneRequested = false;
bool normalControlMode = true;

// Function prototypes
void processCommand(char command);
float readFlowRate();
void printPIDParameters();
void printStatus(unsigned long now);
void reportTuningResults();

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("Cohen-Coon Auto-Tuning Example");
    Serial.println("------------------------------");

    // Initialize pins
    pinMode(VALVE_CONTROL_PIN, OUTPUT);

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

    // Read the flow sensor
    currentFlowRate = readFlowRate();

    // If we're in tuning or normal control mode, compute the output
    if (normalControlMode || pid.isTuning()) {
        // Compute new control output - the PID controller handles timing internally
        controlOutput = pid.compute(currentFlowRate);

        // Apply the control output to the valve
        analogWrite(VALVE_CONTROL_PIN, (int) controlOutput);
    }

    // Print status information on a separate timer to avoid flooding the serial monitor
    unsigned long now = millis();
    if (now - lastPrintTime >= PRINT_INTERVAL) {
        lastPrintTime = now;
        printStatus(now);

        // If we were tuning, but it's now complete, report the results
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
            Serial.println("\nStarting Cohen-Coon auto-tuning...");
            tuningComplete = false;
            autoTuneRequested = true;
            normalControlMode = false;

            // Begin auto-tuning with Cohen-Coon method
            if (pid.autoTuneCohenCoon(currentFlowRate, STEP_AMPLITUDE, MAX_TUNING_TIME)) {
                Serial.println("Auto-tuning started. Please wait while system records step response...");
                Serial.println("This process will identify dead time and time constant of your system.");
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

// Function to read flow rate from sensor
float readFlowRate() {
    // Read analog value
    int rawValue = analogRead(FLOW_SENSOR_PIN);

    // Convert to flow rate percentage (0-100%)
    float flowRate = (rawValue / 1023.0) * 100.0;

    return flowRate;
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
    Serial.print("s, Flow: ");
    Serial.print(currentFlowRate);
    Serial.print("%, Output: ");
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
    Serial.println("New PID parameters based on Cohen-Coon method:");
    printPIDParameters();

    Serial.println("\nCohen-Coon works best for processes with significant dead time.");
    Serial.println("Type 's' to save these parameters to EEPROM");
    Serial.println("Type 'r' to start normal control with the new parameters");
}