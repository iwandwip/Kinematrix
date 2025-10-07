#include "lib/CeriaSensorINA219.h"
using namespace CeriaDevOP;

CeriaSensorINA219 battery(0x40);

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    Serial.println("=== INA219 Negative Readings Test ===");
    
    if (!battery.begin(&Wire, INA219_CAL_16V_400MA)) {
        Serial.println("Failed to initialize INA219!");
        while(1);
    }
    
    battery.setAllowNegativeReadings(true);
    
    Serial.println("INA219 initialized with negative readings enabled");
    Serial.println("Monitoring battery charging/discharging...");
}

void loop() {
    battery.update();
    
    if (battery.hasNewData()) {
        if (battery.isConnected()) {
            float voltage = battery.getVoltage();
            float currentRaw = battery.getCurrentRaw();
            float current = battery.getCurrent();
            float power = battery.getPower();
            
            Serial.println("\n=== Battery Monitor ===");
            Serial.println("Voltage: " + String(voltage, 3) + "V");
            Serial.println("Current (raw): " + String(currentRaw, 2) + "mA");
            Serial.println("Current (filtered): " + String(current, 2) + "mA");
            Serial.println("Power: " + String(power, 2) + "mW");
            Serial.println("Direction: " + battery.getCurrentDirection());
            Serial.println("Summary: " + battery.getPowerString());
            
            if (currentRaw > 0.5) {
                Serial.println("Status: DISCHARGING");
            } else if (currentRaw < -0.5) {
                Serial.println("Status: CHARGING");
            } else {
                Serial.println("Status: STANDBY");
            }
            
            if (voltage < 3.3) {
                Serial.println("WARNING: Low battery voltage!");
            }
            
        } else {
            Serial.println("INA219 not connected");
        }
    }
    
    delay(2000);
}

void testNegativeToggle() {
    Serial.println("\n=== Testing Negative Reading Toggle ===");
    
    battery.update();
    if (battery.hasNewData()) {
        float rawCurrent = battery.getCurrentRaw();
        
        Serial.println("Raw current: " + String(rawCurrent, 2) + "mA");
        
        battery.setAllowNegativeReadings(true);
        Serial.println("With negative enabled: " + String(battery.getCurrent(), 2) + "mA");
        
        battery.setAllowNegativeReadings(false);
        Serial.println("With negative disabled: " + String(battery.getCurrent(), 2) + "mA");
        
        battery.setAllowNegativeReadings(true);
    }
}