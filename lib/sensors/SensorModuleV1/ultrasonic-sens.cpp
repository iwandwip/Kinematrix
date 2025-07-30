#include "ultrasonic-sens.h"
#include "Arduino.h"
#include "SPI.h"

UltrasonicSens::UltrasonicSens(uint8_t trigger_pin, uint8_t echo_pin,
                               unsigned int max_distance,
                               float _stabilityTolerance,
                               uint8_t _sampleCount,
                               uint32_t _stabilityTime,
                               float _resetThreshold)
        : NewPing(trigger_pin, echo_pin, max_distance),
          name(""),
          sensorTimer(0),
          stabilityTolerance(_stabilityTolerance),
          sampleCount(_sampleCount),
          stabilityTime(_stabilityTime),
          resetThreshold(_resetThreshold),
          lastDistances(nullptr),
          distanceIndex(0),
          isLocked(false),
          lockedDistance(0.0),
          stabilityTimer(0) {
    lastDistances = new float[sampleCount];
    for (uint8_t i = 0; i < sampleCount; i++) {
        lastDistances[i] = 0.0;
    }
}

UltrasonicSens::~UltrasonicSens() {
    if (lastDistances != nullptr) {
        delete[] lastDistances;
        lastDistances = nullptr;
    }
}

bool UltrasonicSens::init() {
    if (strcmp(name, "") == 0 && doc == nullptr) {
        name = "UltrasonicSens";
        doc = new JsonDocument;
    }
    (*doc)[name] = 0;
    return true;
}

bool UltrasonicSens::update() {
    if (millis() - sensorTimer >= 50) {
        float distance = this->ping_cm();

        if (distance == 0) {
            distance = lastDistances[distanceIndex > 0 ? distanceIndex - 1 : sampleCount - 1];
        }

        lastDistances[distanceIndex] = distance;
        distanceIndex = (distanceIndex + 1) % sampleCount;

        bool isStable = true;
        float firstDistance = lastDistances[0];
        for (uint8_t i = 1; i < sampleCount; i++) {
            if (abs(lastDistances[i] - firstDistance) > stabilityTolerance) {
                isStable = false;
                break;
            }
        }

        if (isLocked && abs(distance - lockedDistance) > resetThreshold) {
            isLocked = false;
            lockedDistance = 0.0;
            stabilityTimer = 0;
        }
        else if (isStable && !isLocked) {
            if (stabilityTimer == 0) {
                stabilityTimer = millis();
            } else if (millis() - stabilityTimer > stabilityTime) {
                isLocked = true;
                lockedDistance = distance;
            }
        }
        else if (!isStable) {
            stabilityTimer = 0;
        }

        float finalDistance = isLocked ? lockedDistance : distance;

        (*doc)[name] = distance;
        sensorTimer = millis();
        return true;
    }
    return false;
}

void UltrasonicSens::setDocument(const char *objName) {
    name = objName;
}

void UltrasonicSens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument UltrasonicSens::getDocument() {
    return (*doc);
}

JsonVariant UltrasonicSens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float UltrasonicSens::getValueCm() const {
    return (*doc)[name].as<float>();
}

float UltrasonicSens::getValueIn() {
    return (float) this->ping_in();
}

void UltrasonicSens::setPins(uint8_t _trigPin, uint8_t _echoPin) {
}

void UltrasonicSens::setStabilityTolerance(float tolerance) {
    stabilityTolerance = tolerance;
}

void UltrasonicSens::setSampleCount(uint8_t count) {
    if (lastDistances != nullptr) {
        delete[] lastDistances;
    }
    sampleCount = count;
    lastDistances = new float[sampleCount];
    for (uint8_t i = 0; i < sampleCount; i++) {
        lastDistances[i] = 0.0;
    }
    distanceIndex = 0;
}

void UltrasonicSens::setStabilityTime(uint32_t time) {
    stabilityTime = time;
}

void UltrasonicSens::setResetThreshold(float threshold) {
    resetThreshold = threshold;
}

void UltrasonicSens::resetLock() {
    isLocked = false;
    lockedDistance = 0.0;
    stabilityTimer = 0;
}

bool UltrasonicSens::isValueLocked() const {
    return isLocked;
}

float UltrasonicSens::getLockedValue() const {
    return lockedDistance;
}