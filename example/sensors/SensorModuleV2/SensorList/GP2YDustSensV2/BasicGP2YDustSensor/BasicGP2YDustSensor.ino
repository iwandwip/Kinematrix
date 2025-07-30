#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_GP2Y_DUST_V2
#define ENABLE_SENSOR_UTILITY_V2
#include "Kinematrix.h"

GP2YDustSensV2 dustSensor(GP2Y1010AU0F, 7, A0);

void setup() {
    Serial.begin(115200);
    Serial.println("Basic GP2YDustSensV2 Example");
    
    JsonDocument doc;
    dustSensor.setDocumentValue(&doc);
    dustSensor.setDocument("dust");
    
    dustSensor.setUpdateInterval(1000);
    
    if (dustSensor.init()) {
        Serial.println("GP2Y Dust sensor initialized successfully");
        Serial.println("Warming up sensor... please wait 30 seconds");
        delay(30000);
        Serial.println("Sensor ready for measurements");
    } else {
        Serial.println("GP2Y Dust sensor initialization failed!");
    }
}

void loop() {
    if (dustSensor.update()) {
        Serial.print("Dust Density: ");
        Serial.print((float)dustSensor.getFloatValue("density"));
        Serial.print(" μg/m³, Voltage: ");
        Serial.print((float)dustSensor.getFloatValue("voltage"));
        Serial.print("V, Raw: ");
        Serial.println((int)dustSensor.getIntValue("raw"));
    }
    
    delay(500);
}