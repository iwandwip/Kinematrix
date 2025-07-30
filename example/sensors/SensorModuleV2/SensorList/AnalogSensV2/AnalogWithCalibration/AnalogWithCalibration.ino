#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 calibrationModule;

void setup() {
    Serial.begin(115200);
    Serial.println("AnalogSensV2 with Calibration Example");
    
    calibrationModule.addSensor("pressure", new AnalogSensV2(A0, 5.0, 1023));
    calibrationModule.addSensor("temperature", new AnalogSensV2(A1, 3.3, 1023));
    calibrationModule.addSensor("flow", new AnalogSensV2(A2, 5.0, 1023));
    
    calibrationModule.init();
    
    calibrationModule.addCalibrationEntry("pressure", "volt");
    calibrationModule.addCalibrationEntry("temperature", "volt");
    calibrationModule.addCalibrationEntry("flow", "volt");
    
    calibrationModule.calibrateTwoPoint("pressure", "volt", 1.0, 0.95, 4.0, 4.08);
    calibrationModule.calibrateOnePoint("temperature", "volt", 2.5);
    calibrationModule.calibrateOnePoint("flow", "volt", 3.0);
    
    Serial.println("Analog sensors with calibration ready");
}

void loop() {
    calibrationModule.update();
    
    if (calibrationModule.isUpdated("pressure")) {
        float rawVolt = calibrationModule["pressure"]["volt"];
        float calPressure = calibrationModule.getCalibratedValue("pressure", "volt");
        Serial.print("Pressure - Raw: ");
        Serial.print(rawVolt);
        Serial.print("V, Calibrated: ");
        Serial.print(calPressure);
        Serial.println(" bar");
    }
    
    if (calibrationModule.isUpdated("temperature")) {
        float rawVolt = calibrationModule["temperature"]["volt"];
        float calTemp = calibrationModule.getCalibratedValue("temperature", "volt");
        Serial.print("Temperature - Raw: ");
        Serial.print(rawVolt);
        Serial.print("V, Calibrated: ");
        Serial.print(calTemp);
        Serial.println("°C");
    }
    
    if (calibrationModule.isUpdated("flow")) {
        float rawVolt = calibrationModule["flow"]["volt"];
        float calFlow = calibrationModule.getCalibratedValue("flow", "volt");
        Serial.print("Flow - Raw: ");
        Serial.print(rawVolt);
        Serial.print("V, Calibrated: ");
        Serial.print(calFlow);
        Serial.println(" L/min");
    }
    
    delay(1500);
}