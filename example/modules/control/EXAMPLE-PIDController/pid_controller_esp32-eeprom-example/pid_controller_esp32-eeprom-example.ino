/**
 * ESP32EEPROMSaving.ino - Example of saving and loading PID parameters on ESP32
 * 
 * This example demonstrates how to save and load PID parameters to/from
 * EEPROM on an ESP32 device. It also shows how to manage multiple PID
 * controllers with different EEPROM addresses.
 */

#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"

// Pin definitions - First PID controller (e.g., temperature control)
const int TEMP_SENSOR_PIN = 36;
const int HEATER_PIN = 25;

// Pin definitions - Second PID controller (e.g., humidity control)
const int HUMIDITY_SENSOR_PIN = 39;
const int HUMIDIFIER_PIN = 26;

// Control parameters - Temperature PID
const float TEMP_KP = 2.0;
const float TEMP_KI = 0.1;
const float TEMP_KD = 0.5;
const float TEMP_SETPOINT = 25.0;     // 25°C

// Control parameters - Humidity PID
const float HUM_KP = 1.0;
const float HUM_KI = 0.05;
const float HUM_KD = 0.2;
const float HUM_SETPOINT = 45.0;      // 45%

// Common parameters
const float TIME_STEP = 0.1;          // Control loop time step in seconds
const float MIN_OUTPUT = 0.0;         // Minimum output value
const float MAX_OUTPUT = 255.0;       // Maximum output value (PWM)

// EEPROM addresses for each controller
const int TEMP_PID_EEPROM_ADDR = 0;
const int HUM_PID_EEPROM_ADDR = 32;   // Leave space for temperature PID

// Create the PID controller instances
PIDController tempPID(TEMP_KP, TEMP_KI, TEMP_KD, TIME_STEP, MIN_OUTPUT, MAX_OUTPUT);
PIDController humPID(HUM_KP, HUM_KI, HUM_KD, TIME_STEP, MIN_OUTPUT, MAX_OUTPUT);

// Variables
float currentTemp = 0.0;
float currentHumidity = 0.0;
float tempOutput = 0.0;
float humOutput = 0.0;
unsigned long lastPrintTime = 0;    // Timer just for printing status
const unsigned long PRINT_INTERVAL = 1000; // Print every second
bool loadedFromEEPROM = false;

// Function prototypes
void processCommand(char command);
void saveParameters();
void loadParameters();
void resetParameters();
void modifyTemperaturePID();
void modifyHumidityPID();
void printPIDParameters();
void printStatus(unsigned long now);
float readTemperature();
float readHumidity();

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);

    Serial.println("ESP32 PID Parameters EEPROM Example");
    Serial.println("-----------------------------------");

    // Initialize pins
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(HUMIDIFIER_PIN, OUTPUT);
    ledcSetup(0, 5000, 8);  // Channel 0, 5kHz, 8-bit resolution
    ledcSetup(1, 5000, 8);  // Channel 1, 5kHz, 8-bit resolution
    ledcAttachPin(HEATER_PIN, 0);
    ledcAttachPin(HUMIDIFIER_PIN, 1);

    // Set up EEPROM addresses
    tempPID.setEEPROMAddress(TEMP_PID_EEPROM_ADDR);
    humPID.setEEPROMAddress(HUM_PID_EEPROM_ADDR);

    // Try to load parameters from EEPROM
    bool tempLoaded = tempPID.loadParametersFromEEPROM();
    bool humLoaded = humPID.loadParametersFromEEPROM();

    if (tempLoaded && humLoaded) {
        Serial.println("Loaded PID parameters from EEPROM:");
        loadedFromEEPROM = true;
    } else {
        Serial.println("No valid parameters in EEPROM, using defaults:");
    }

    // Set the target set points
    tempPID.setSetPoint(TEMP_SETPOINT);
    humPID.setSetPoint(HUM_SETPOINT);

    // Print current parameters
    printPIDParameters();

    Serial.println("\nCommands:");
    Serial.println("  's' - Save all parameters to EEPROM");
    Serial.println("  'l' - Load all parameters from EEPROM");
    Serial.println("  'r' - Reset to default parameters");
    Serial.println("  't' - Modify temperature PID parameters");
    Serial.println("  'h' - Modify humidity PID parameters");
}

void loop() {
    // Check for commands from serial
    if (Serial.available() > 0) {
        char command = Serial.read();
        processCommand(command);
    }

    // Read sensors
    currentTemp = readTemperature();
    currentHumidity = readHumidity();

    // Compute control outputs - the PID controllers handle timing internally
    tempOutput = tempPID.compute(currentTemp);
    humOutput = humPID.compute(currentHumidity);

    // Apply the control outputs
    ledcWrite(0, (int) tempOutput);
    ledcWrite(1, (int) humOutput);

    // Print status on a separate timer to avoid flooding the serial monitor
    unsigned long now = millis();
    if (now - lastPrintTime >= PRINT_INTERVAL) {
        lastPrintTime = now;
        printStatus(now);
    }
}

// Process serial commands
void processCommand(char command) {
    switch (command) {
        case 's': // Save parameters
            saveParameters();
            break;

        case 'l': // Load parameters
            loadParameters();
            break;

        case 'r': // Reset to defaults
            resetParameters();
            break;

        case 't': // Modify temperature PID
            modifyTemperaturePID();
            break;

        case 'h': // Modify humidity PID
            modifyHumidityPID();
            break;
    }
}

// Save all parameters to EEPROM
void saveParameters() {
    Serial.println("\nSaving parameters to EEPROM...");

    bool tempSaved = tempPID.saveParametersToEEPROM();
    bool humSaved = humPID.saveParametersToEEPROM();

    if (tempSaved && humSaved) {
        Serial.println("All parameters saved successfully!");
    } else {
        Serial.println("Failed to save one or more parameter sets!");
    }
}

// Load all parameters from EEPROM
void loadParameters() {
    Serial.println("\nLoading parameters from EEPROM...");

    bool tempLoaded = tempPID.loadParametersFromEEPROM();
    bool humLoaded = humPID.loadParametersFromEEPROM();

    if (tempLoaded && humLoaded) {
        Serial.println("All parameters loaded successfully!");
        printPIDParameters();
    } else {
        Serial.println("Failed to load one or more parameter sets!");
    }
}

// Reset to default parameters
void resetParameters() {
    Serial.println("\nResetting to default parameters...");

    tempPID.setKp(TEMP_KP);
    tempPID.setKi(TEMP_KI);
    tempPID.setKd(TEMP_KD);

    humPID.setKp(HUM_KP);
    humPID.setKi(HUM_KI);
    humPID.setKd(HUM_KD);

    printPIDParameters();
}

// Interactively modify temperature PID parameters
void modifyTemperaturePID() {
    Serial.println("\nModify Temperature PID Parameters");
    Serial.println("--------------------------------");

    Serial.print("Current Kp: ");
    Serial.println(tempPID.getKp());
    Serial.print("Enter new Kp (or -1 to keep current): ");

    while (!Serial.available()) { delay(10); }
    float newKp = Serial.parseFloat();
    Serial.println(newKp);

    if (newKp >= 0) {
        tempPID.setKp(newKp);
    }

    Serial.print("Current Ki: ");
    Serial.println(tempPID.getKi());
    Serial.print("Enter new Ki (or -1 to keep current): ");

    while (!Serial.available()) { delay(10); }
    float newKi = Serial.parseFloat();
    Serial.println(newKi);

    if (newKi >= 0) {
        tempPID.setKi(newKi);
    }

    Serial.print("Current Kd: ");
    Serial.println(tempPID.getKd());
    Serial.print("Enter new Kd (or -1 to keep current): ");

    while (!Serial.available()) { delay(10); }
    float newKd = Serial.parseFloat();
    Serial.println(newKd);

    if (newKd >= 0) {
        tempPID.setKd(newKd);
    }

    Serial.println("\nUpdated Temperature PID Parameters:");
    Serial.print("Kp: ");
    Serial.print(tempPID.getKp());
    Serial.print(", Ki: ");
    Serial.print(tempPID.getKi());
    Serial.print(", Kd: ");
    Serial.println(tempPID.getKd());

    Serial.println("Type 's' to save these parameters to EEPROM");
}

// Interactively modify humidity PID parameters
void modifyHumidityPID() {
    Serial.println("\nModify Humidity PID Parameters");
    Serial.println("------------------------------");

    Serial.print("Current Kp: ");
    Serial.println(humPID.getKp());
    Serial.print("Enter new Kp (or -1 to keep current): ");

    while (!Serial.available()) { delay(10); }
    float newKp = Serial.parseFloat();
    Serial.println(newKp);

    if (newKp >= 0) {
        humPID.setKp(newKp);
    }

    Serial.print("Current Ki: ");
    Serial.println(humPID.getKi());
    Serial.print("Enter new Ki (or -1 to keep current): ");

    while (!Serial.available()) { delay(10); }
    float newKi = Serial.parseFloat();
    Serial.println(newKi);

    if (newKi >= 0) {
        humPID.setKi(newKi);
    }

    Serial.print("Current Kd: ");
    Serial.println(humPID.getKd());
    Serial.print("Enter new Kd (or -1 to keep current): ");

    while (!Serial.available()) { delay(10); }
    float newKd = Serial.parseFloat();
    Serial.println(newKd);

    if (newKd >= 0) {
        humPID.setKd(newKd);
    }

    Serial.println("\nUpdated Humidity PID Parameters:");
    Serial.print("Kp: ");
    Serial.print(humPID.getKp());
    Serial.print(", Ki: ");
    Serial.print(humPID.getKi());
    Serial.print(", Kd: ");
    Serial.println(humPID.getKd());

    Serial.println("Type 's' to save these parameters to EEPROM");
}

// Print all PID parameters
void printPIDParameters() {
    Serial.println("\nTemperature PID Parameters:");
    Serial.print("Kp: ");
    Serial.print(tempPID.getKp());
    Serial.print(", Ki: ");
    Serial.print(tempPID.getKi());
    Serial.print(", Kd: ");
    Serial.println(tempPID.getKd());

    Serial.println("Humidity PID Parameters:");
    Serial.print("Kp: ");
    Serial.print(humPID.getKp());
    Serial.print(", Ki: ");
    Serial.print(humPID.getKi());
    Serial.print(", Kd: ");
    Serial.println(humPID.getKd());
}

// Print current status
void printStatus(unsigned long now) {
    Serial.print("Time: ");
    Serial.print(now / 1000.0);
    Serial.print("s, Temp: ");
    Serial.print(currentTemp);
    Serial.print("°C (SP: ");
    Serial.print(tempPID.getSetPoint());
    Serial.print("°C), Humidity: ");
    Serial.print(currentHumidity);
    Serial.print("% (SP: ");
    Serial.print(humPID.getSetPoint());
    Serial.println("%)");
}

// Read temperature sensor
float readTemperature() {
    // Read analog value
    int rawValue = analogRead(TEMP_SENSOR_PIN);

    // Convert to temperature (example for LM35)
    // Adjust, according to your specific sensor and ADC range
    float voltage = rawValue * (3.3 / 4095.0);  // ESP32 has 12-bit ADC (0-4095)
    float temperature = voltage * 100.0;        // LM35: 10mV per degree

    return temperature;
}

// Read humidity sensor
float readHumidity() {
    // Read analog value
    int rawValue = analogRead(HUMIDITY_SENSOR_PIN);

    // Convert to humidity percentage (example for a generic sensor)
    // Adjust, according to your specific sensor and ADC range
    float humidity = map(rawValue, 0, 4095, 0, 100);

    return humidity;
}