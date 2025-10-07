#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

struct AlertData {
        int pin;
        int duration;
        String message;
        int beepCount;
};

struct MotorData {
        int pin;
        int speed;
        String direction;
};

void parameterizedAlert(void* param) {
        AlertData* data = (AlertData*)param;
        digitalWrite(data->pin, HIGH);
        Serial.println(data->message);

        for (int i = 0; i < data->beepCount; i++) {
                tone(11, 1000, 100);
                delay(150);
        }

        delay(data->duration);
        digitalWrite(data->pin, LOW);
}

void motorControl(void* param) {
        MotorData* data = (MotorData*)param;
        Serial.print("Motor control: ");
        Serial.print(data->direction);
        Serial.print(" at speed ");
        Serial.println(data->speed);

        analogWrite(data->pin, data->speed);
}

void dataLogger(void* param) {
        String* logMessage = (String*)param;
        Serial.print("[LOG] ");
        Serial.print(millis());
        Serial.print(" - ");
        Serial.println(*logMessage);
}

AlertData warningAlert = {12, 500, "Warning: Sensor threshold exceeded", 2};
AlertData errorAlert = {13, 1000, "Error: Critical system failure", 5};
AlertData infoAlert = {LED_BUILTIN, 200, "Info: System status normal", 1};

MotorData fanControl = {5, 150, "Forward"};
MotorData pumpControl = {6, 200, "Reverse"};

String tempLogMsg = "Temperature reading logged";
String pressureLogMsg = "Pressure reading logged";

void setup() {
        Serial.begin(9600);
        pinMode(12, OUTPUT);
        pinMode(13, OUTPUT);
        pinMode(LED_BUILTIN, OUTPUT);
        pinMode(11, OUTPUT);

        logic.addValueCondition(
            "warningLevel",
            []() { return analogRead(A0); },
            [](float val) { return val > 300 && val <= 600; },
            LogicConditionManager::TO_TRUE,
            parameterizedAlert,
            &warningAlert);

        logic.addValueCondition(
            "errorLevel",
            []() { return analogRead(A0); },
            [](float val) { return val > 600; },
            LogicConditionManager::TO_TRUE,
            parameterizedAlert,
            &errorAlert);

        logic.addValueCondition(
            "normalLevel",
            []() { return analogRead(A0); },
            [](float val) { return val <= 300; },
            LogicConditionManager::TO_TRUE,
            parameterizedAlert,
            &infoAlert);

        logic.addValueCondition(
            "fanActivation",
            []() { return analogRead(A1); },
            [](float val) { return val > 400; },
            LogicConditionManager::WHEN_TRUE,
            motorControl,
            &fanControl);

        logic.addValueCondition(
            "pumpActivation",
            []() { return analogRead(A2); },
            [](float val) { return val > 500; },
            LogicConditionManager::WHEN_TRUE,
            motorControl,
            &pumpControl);

        logic.addTimerCondition(
            "tempLogger",
            5000,  // Every 5 seconds
            true,  // Auto reset
            dataLogger,
            &tempLogMsg);

        logic.addTimerCondition(
            "pressureLogger",
            7000,  // Every 7 seconds
            true,
            dataLogger,
            &pressureLogMsg);

        Serial.println("Parameterized action system initialized");
        Serial.println("A0: Warning(300-600), Error(>600), Normal(<=300)");
        Serial.println("A1: Fan activation (>400)");
        Serial.println("A2: Pump activation (>500)");
}

void loop() {
        logic.update();

        static unsigned long lastReading = 0;
        if (millis() - lastReading > 2000) {
                Serial.println("--- Sensor Readings ---");
                Serial.print("A0: ");
                Serial.println(analogRead(A0));
                Serial.print("A1: ");
                Serial.println(analogRead(A1));
                Serial.print("A2: ");
                Serial.println(analogRead(A2));

                Serial.println("--- Condition States ---");
                logic.printStatus("warningLevel");
                logic.printStatus("errorLevel");
                logic.printStatus("normalLevel");

                lastReading = millis();
        }

        if (logic.checkAndTrigger("fanActivation",
                                  []() { Serial.println("Custom fan callback triggered"); })) {
                Serial.println("Fan system activated via custom trigger");
        }
}