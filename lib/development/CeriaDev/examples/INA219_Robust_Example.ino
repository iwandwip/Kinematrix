#include "lib/CeriaSensorINA219.h"
using namespace CeriaDevOP;

CeriaSensorINA219 power(0x40);
CeriaSensorINA219 battery(0x41);

unsigned long lastPrint = 0;
unsigned long printInterval = 1000;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    Serial.println("=== INA219 Robust Reading Test ===");
    
    if (!power.begin(&Wire, INA219_CAL_32V_2A)) {
        Serial.println("Failed to initialize Power INA219!");
    } else {
        Serial.println("Power INA219 initialized");
    }
    
    if (!battery.begin(&Wire, INA219_CAL_16V_400MA)) {
        Serial.println("Failed to initialize Battery INA219!");  
    } else {
        Serial.println("Battery INA219 initialized");
    }
    
    power.setAllowNegativeReadings(true);
    battery.setAllowNegativeReadings(true);
    
    Serial.println("Starting robust monitoring...");
}

void loop() {
    power.update();
    battery.update();
    
    unsigned long now = millis();
    if (now - lastPrint >= printInterval) {
        printSensorData();
        lastPrint = now;
    }
    
    delay(10);
}

void printSensorData() {
    Serial.print("| ");
    
    // Power sensor
    if (power.isConnected(2000)) {
        if (power.hasNewData()) {
            Serial.print("power.voltage: ");
            Serial.print(power.getVoltage(INA219_UNIT_BASE), 2);
            Serial.print("| power.current: ");
            Serial.print(power.getCurrent(INA219_UNIT_MILLI), 2);
            Serial.print("| power.power: ");
            Serial.print(power.getPower(INA219_UNIT_MILLI), 2);
        } else {
            Serial.print("power: NO_NEW_DATA");
        }
    } else {
        Serial.print("power: DISCONNECTED (errors: ");
        Serial.print(power.getErrorCount());
        Serial.print(")");
        
        if (power.getErrorCount() > 10) {
            Serial.print(" - RESETTING");
            power.resetSensor();
        }
    }
    
    Serial.print("| ");
    
    // Battery sensor  
    if (battery.isConnected(2000)) {
        if (battery.hasNewData()) {
            Serial.print("battery.voltage: ");
            Serial.print(battery.getVoltage(INA219_UNIT_BASE), 2);
            Serial.print("| battery.current: ");
            Serial.print(battery.getCurrent(INA219_UNIT_MILLI), 2);
            Serial.print("| battery.power: ");
            Serial.print(battery.getPower(INA219_UNIT_MILLI), 2);
            Serial.print("| battery.direction: ");
            Serial.print(battery.getCurrentDirection());
        } else {
            Serial.print("battery: NO_NEW_DATA");
        }
    } else {
        Serial.print("battery: DISCONNECTED (errors: ");
        Serial.print(battery.getErrorCount());
        Serial.print(")");
        
        if (battery.getErrorCount() > 10) {
            Serial.print(" - RESETTING");
            battery.resetSensor();
        }
    }
    
    Serial.println();
}

void diagnosticMode() {
    Serial.println("\n=== DIAGNOSTIC MODE ===");
    
    Serial.println("Power sensor I2C test:");
    if (power.checkI2CConnection()) {
        Serial.println("  ✓ I2C Connection OK");
    } else {
        Serial.println("  ✗ I2C Connection FAILED");
        Serial.println("  → Attempting reset...");
        if (power.resetSensor()) {
            Serial.println("  ✓ Reset successful");
        } else {
            Serial.println("  ✗ Reset failed");
        }
    }
    
    Serial.println("Battery sensor I2C test:");
    if (battery.checkI2CConnection()) {
        Serial.println("  ✓ I2C Connection OK");
    } else {
        Serial.println("  ✗ I2C Connection FAILED");
        Serial.println("  → Attempting reset...");
        if (battery.resetSensor()) {
            Serial.println("  ✓ Reset successful");
        } else {
            Serial.println("  ✗ Reset failed");
        }
    }
    
    Serial.println("Error counts:");
    Serial.println("  Power errors: " + String(power.getErrorCount()));
    Serial.println("  Battery errors: " + String(battery.getErrorCount()));
}

void i2cScanner() {
    Serial.println("\n=== I2C Scanner ===");
    
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t result = Wire.endTransmission();
        
        if (result == 0) {
            Serial.print("Found device at 0x");
            if (addr < 16) Serial.print("0");
            Serial.println(addr, HEX);
        }
    }
}