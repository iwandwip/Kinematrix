#include "lib/CeriaSensorINA219.h"
using namespace CeriaDevOP;

CeriaSensorINA219 sensor(0x40);

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    Serial.println("=== INA219 Unit Scaling Test ===");
    
    if (!sensor.begin(&Wire, INA219_CAL_32V_2A)) {
        Serial.println("Failed to initialize INA219!");
        while(1);
    }
    
    sensor.setAllowNegativeReadings(true);
    sensor.setDefaultUnits(INA219_UNIT_BASE, INA219_UNIT_MILLI, INA219_UNIT_MILLI);
    
    Serial.println("INA219 initialized successfully");
    Serial.println("Testing different unit scalings...");
}

void loop() {
    sensor.update();
    
    if (sensor.hasNewData()) {
        if (sensor.isConnected()) {
            
            Serial.println("\n======================================");
            Serial.println("=== VOLTAGE MEASUREMENTS ===");
            Serial.println("Base unit (V):  " + String(sensor.getVoltage(INA219_UNIT_BASE), 4) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_BASE, true, false));
            Serial.println("Milli (mV):     " + String(sensor.getVoltage(INA219_UNIT_MILLI), 1) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_MILLI, true, false));
            Serial.println("Micro (µV):     " + String(sensor.getVoltage(INA219_UNIT_MICRO), 0) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_MICRO, true, false));
            
            Serial.println("\n=== CURRENT MEASUREMENTS ===");
            Serial.println("Base unit (A):  " + String(sensor.getCurrent(INA219_UNIT_BASE), 6) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_BASE, false, false));
            Serial.println("Milli (mA):     " + String(sensor.getCurrent(INA219_UNIT_MILLI), 2) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_MILLI, false, false));
            Serial.println("Micro (µA):     " + String(sensor.getCurrent(INA219_UNIT_MICRO), 0) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_MICRO, false, false));
            
            Serial.println("\n=== POWER MEASUREMENTS ===");
            Serial.println("Base unit (W):  " + String(sensor.getPower(INA219_UNIT_BASE), 6) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_BASE, false, true));
            Serial.println("Milli (mW):     " + String(sensor.getPower(INA219_UNIT_MILLI), 2) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_MILLI, false, true));
            Serial.println("Micro (µW):     " + String(sensor.getPower(INA219_UNIT_MICRO), 0) + " " + 
                         sensor.getUnitSymbol(INA219_UNIT_MICRO, false, true));
            
            Serial.println("\n=== RAW vs FILTERED CURRENT ===");
            Serial.println("Raw current:     " + String(sensor.getCurrentRaw(INA219_UNIT_MILLI), 2) + "mA");
            Serial.println("Filtered current:" + String(sensor.getCurrent(INA219_UNIT_MILLI), 2) + "mA");
            Serial.println("Direction:       " + sensor.getCurrentDirection());
            
            Serial.println("\n=== FORMATTED STRINGS (Different Units) ===");
            Serial.println("V/mA/mW: " + sensor.getPowerString(INA219_UNIT_BASE, INA219_UNIT_MILLI, INA219_UNIT_MILLI));
            Serial.println("mV/µA/µW: " + sensor.getPowerString(INA219_UNIT_MILLI, INA219_UNIT_MICRO, INA219_UNIT_MICRO));
            Serial.println("V/A/W: " + sensor.getPowerString(INA219_UNIT_BASE, INA219_UNIT_BASE, INA219_UNIT_BASE));
            
            testLowPowerMeasurement();
            testHighPowerMeasurement();
            
        } else {
            Serial.println("INA219 not connected");
        }
    }
    
    delay(3000);
}

void testLowPowerMeasurement() {
    Serial.println("\n=== LOW POWER APPLICATION (µA/µW precision) ===");
    
    float currentMicroA = sensor.getCurrent(INA219_UNIT_MICRO);
    float powerMicroW = sensor.getPower(INA219_UNIT_MICRO);
    
    if (abs(currentMicroA) < 1000) {  // Less than 1mA
        Serial.println("✓ Suitable for low power measurement");
        Serial.println("  Current: " + String(currentMicroA, 0) + "µA");
        Serial.println("  Power: " + String(powerMicroW, 0) + "µW");
        
        if (abs(currentMicroA) < 100) {
            Serial.println("  → Ultra low power device");
        } else if (abs(currentMicroA) < 500) {
            Serial.println("  → Low power device");
        } else {
            Serial.println("  → Medium power device");
        }
    } else {
        Serial.println("✗ Current too high for µA precision");
        Serial.println("  Recommend using mA units");
    }
}

void testHighPowerMeasurement() {
    Serial.println("\n=== HIGH POWER APPLICATION (A/W precision) ===");
    
    float currentA = sensor.getCurrent(INA219_UNIT_BASE);
    float powerW = sensor.getPower(INA219_UNIT_BASE);
    
    if (abs(currentA) > 0.1) {  // More than 100mA
        Serial.println("✓ High current detected");
        Serial.println("  Current: " + String(currentA, 3) + "A");
        Serial.println("  Power: " + String(powerW, 3) + "W");
        
        if (abs(currentA) > 1.0) {
            Serial.println("  → High power device");
        } else {
            Serial.println("  → Medium power device");
        }
    } else {
        Serial.println("✓ Low current - mA precision recommended");
    }
}

void demonstrateUnitSelection() {
    Serial.println("\n=== SMART UNIT SELECTION ===");
    
    sensor.update();
    if (sensor.hasNewData()) {
        float currentA = sensor.getCurrent(INA219_UNIT_BASE);
        
        if (abs(currentA) < 0.001) {
            Serial.println("Recommendation: Use µA units");
            Serial.println("Current: " + String(sensor.getCurrent(INA219_UNIT_MICRO), 0) + "µA");
            Serial.println("Power: " + String(sensor.getPower(INA219_UNIT_MICRO), 0) + "µW");
        } else if (abs(currentA) < 1.0) {
            Serial.println("Recommendation: Use mA units");  
            Serial.println("Current: " + String(sensor.getCurrent(INA219_UNIT_MILLI), 1) + "mA");
            Serial.println("Power: " + String(sensor.getPower(INA219_UNIT_MILLI), 1) + "mW");
        } else {
            Serial.println("Recommendation: Use base units");
            Serial.println("Current: " + String(sensor.getCurrent(INA219_UNIT_BASE), 3) + "A");
            Serial.println("Power: " + String(sensor.getPower(INA219_UNIT_BASE), 3) + "W");
        }
    }
}

void compareCalibrationModes() {
    Serial.println("\n=== CALIBRATION MODE COMPARISON ===");
    
    INA219CalibrationMode modes[] = {INA219_CAL_32V_2A, INA219_CAL_32V_1A, INA219_CAL_16V_400MA};
    String modeNames[] = {"32V/2A", "32V/1A", "16V/400mA"};
    
    for (int i = 0; i < 3; i++) {
        sensor.setCalibrationMode(modes[i]);
        delay(100);
        sensor.update();
        
        if (sensor.hasNewData()) {
            Serial.println("Mode " + modeNames[i] + ":");
            Serial.println("  Current: " + String(sensor.getCurrent(INA219_UNIT_MILLI), 2) + "mA");
            Serial.println("  Resolution: " + getResolutionInfo(modes[i]));
        }
    }
    
    sensor.setCalibrationMode(INA219_CAL_32V_2A);
}

String getResolutionInfo(INA219CalibrationMode mode) {
    switch (mode) {
        case INA219_CAL_32V_2A:
            return "100µA per bit, max 3.2A";
        case INA219_CAL_32V_1A:
            return "40µA per bit, max 1.3A";
        case INA219_CAL_16V_400MA:
            return "50µA per bit, max 400mA";
        default:
            return "Unknown";
    }
}