/**
 * BasicPID.ino - Basic example of using the PIDController
 * 
 * This example shows how to set up a basic PID control loop
 * for controlling a temperature with a heater element.
 */

#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"

// Pin definitions
const int TEMP_SENSOR_PIN = 33;
const int HEATER_PIN = 9;

// Control parameters
const float SETPOINT = 60.0;       // Target temperature in Celsius
const float KP = 2.0;              // Proportional gain
const float KI = 0.1;              // Integral gain
const float KD = 0.5;              // Derivative gain
const float TIME_STEP = 0.1;       // Control loop time step in seconds
const float MIN_OUTPUT = 0.0;      // Minimum output value
const float MAX_OUTPUT = 255.0;    // Maximum output value (PWM)

// Create the PID controller instance
PIDController pid(KP, KI, KD, TIME_STEP, MIN_OUTPUT, MAX_OUTPUT);

// Variables
float currentTemperature = 0.0;
float controlOutput = 0.0;
unsigned long lastPrintTime = 0;    // Timer just for printing status
const unsigned long PRINT_INTERVAL = 1000; // Print every second

// Function prototypes
float readTemperature();

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("Basic PID Controller Example");
    Serial.println("----------------------------");

    // Initialize pins
    pinMode(HEATER_PIN, OUTPUT);

    // Set the target setpoint
    pid.setSetPoint(SETPOINT);

    // Calculate optimal integral limit
    pid.calculateOptimalIntegralLimit();

    // Show initial parameters
    Serial.print("Setpoint: ");
    Serial.println(pid.getSetPoint());
    Serial.print("Kp: ");
    Serial.print(pid.getKp());
    Serial.print(", Ki: ");
    Serial.print(pid.getKi());
    Serial.print(", Kd: ");
    Serial.println(pid.getKd());
}

void loop() {
    // Read the temperature sensor
    currentTemperature = readTemperature();

    // Compute new control output - the PID controller handles timing internally
    controlOutput = pid.compute(currentTemperature);

    // Apply the control output to the heater
    analogWrite(HEATER_PIN, (int) controlOutput);

    // Print for monitoring on a separate timer to avoid flooding the serial monitor
    unsigned long now = millis();
    if (now - lastPrintTime >= PRINT_INTERVAL) {
        lastPrintTime = now;

        Serial.print("Time: ");
        Serial.print(now / 1000.0);
        Serial.print("s, Temp: ");
        Serial.print(currentTemperature);
        Serial.print("°C, Output: ");
        Serial.print(controlOutput);
        Serial.print(", P: ");
        Serial.print(pid.getProportionalComponent());
        Serial.print(", I: ");
        Serial.print(pid.getIntegralComponent());
        Serial.print(", D: ");
        Serial.println(pid.getDerivativeComponent());
    }
}

// Function to read temperature from sensor
float readTemperature() {
    // Read analog value
    int rawValue = analogRead(TEMP_SENSOR_PIN);

    // Convert to temperature (example for LM35)
    // LM35 outputs 10mV per degree Celsius
    // With Arduino's 10-bit ADC and 5V reference, each unit is 5/1024 = 0.0048828125V
    // So temperature = (ADC value * 0.0048828125V * 1000mV/V) / 10mV/°C
    float temperature = (rawValue * 0.0048828125 * 100.0);

    return temperature;
}