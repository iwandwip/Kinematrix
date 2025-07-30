#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_DHT_V2
#define ENABLE_SENSOR_BME680_V2
#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("Advanced Calibration Features Example");
    
    calibrationModule.addSensor("temp1", new DHTSensV2(2, DHT22));
    calibrationModule.addSensor("temp2", new BME680SensV2());
    calibrationModule.addSensor("pressure", new AnalogSensV2(A0, 5.0, 1023));
    calibrationModule.addSensor("flow", new AnalogSensV2(A1, 3.3, 1023));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("temp1", "temperature");
    calibrationModule.addCalibrationEntry("temp2", "temperature");
    calibrationModule.addCalibrationEntry("pressure", "volt");
    calibrationModule.addCalibrationEntry("flow", "volt");
    
    calibrationModule.calibrateTwoPoint("temp1", "temperature", 20.0, 19.8, 40.0, 40.3);
    calibrationModule.calibrateTwoPoint("temp2", "temperature", 20.0, 19.8, 40.0, 40.3);
    calibrationModule.calibrateOnePoint("pressure", "volt", 2.5);
    calibrationModule.calibrateOnePoint("flow", "volt", 3.0);
    
    calibrationModule.enableValueCalibration("temp1", "temperature", true);
    calibrationModule.enableValueCalibration("temp2", "temperature", true);
    calibrationModule.enableValueCalibration("pressure", "volt", false);
    
    Serial.println("Advanced calibration features configured");
    Serial.println("temp1 & temp2: calibration ON");
    Serial.println("pressure: calibration OFF");
    Serial.println("flow: calibration ON");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("temp1")) {
        float rawTemp = calibrationModule["temp1"]["temperature"];
        float calTemp = calibrationModule.getCalibratedValue("temp1", "temperature");
        bool isEnabled = calibrationModule.isValueCalibrationEnabled("temp1", "temperature");
        
        Serial.print("Temp1 - Raw: ");
        Serial.print(rawTemp);
        Serial.print("°C, Cal: ");
        Serial.print(calTemp);
        Serial.print("°C, Enabled: ");
        Serial.println(isEnabled ? "YES" : "NO");
    }
    
    if (calibrationModule.isUpdated("pressure")) {
        float rawPressure = calibrationModule["pressure"]["volt"];
        float calPressure = calibrationModule.getCalibratedValue("pressure", "volt");
        bool isEnabled = calibrationModule.isValueCalibrationEnabled("pressure", "volt");
        
        Serial.print("Pressure - Raw: ");
        Serial.print(rawPressure);
        Serial.print("V, Cal: ");
        Serial.print(calPressure);
        Serial.print("V, Enabled: ");
        Serial.println(isEnabled ? "YES" : "NO");
    }
    
    static unsigned long lastToggle = 0;
    if (millis() - lastToggle > 10000) {
        bool currentState = calibrationModule.isValueCalibrationEnabled("pressure", "volt");
        calibrationModule.enableValueCalibration("pressure", "volt", !currentState);
        Serial.print("Toggled pressure calibration to: ");
        Serial.println(!currentState ? "ON" : "OFF");
        lastToggle = millis();
    }
    
    delay(2000);
}