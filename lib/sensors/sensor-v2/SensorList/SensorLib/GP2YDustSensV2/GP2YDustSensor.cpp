#include <Arduino.h>
#include "GP2YDustSensor.h"

GP2YDustSensor::GP2YDustSensor(
        GP2YDustSensorType type,
        uint8_t ledOutputPin,
        uint8_t analogReadPin,
        uint16_t runningAverageCount,
        float vRef,
        float adcResolution,
        float voltageDividerRatio) {
    this->ledOutputPin = ledOutputPin;
    this->analogReadPin = analogReadPin;
    this->type = type;
    this->vRef = vRef;
    this->adcResolution = adcResolution;
    this->voltageDividerRatio = voltageDividerRatio;
    this->sensitivity = 0.5;
    this->nextRunningAverageCounter = 0;
    this->hasBaselineCandidate = false;
    this->readCount = 0;

    switch (type) {
        case GP2Y1010AU0F:
            this->minZeroDustVoltage = 0;
            this->typZeroDustVoltage = 0.9;
            this->maxZeroDustVoltage = 1.5;
            this->zeroDustVoltage = this->minDustVoltage = this->typZeroDustVoltage;
            break;
        case GP2Y1014AU0F:
            this->minZeroDustVoltage = 0.1;
            this->typZeroDustVoltage = 0.6;
            this->maxZeroDustVoltage = 1.1;
            this->zeroDustVoltage = this->minDustVoltage = this->typZeroDustVoltage;
            break;
    }

    this->calibrationFactor = 1;
    this->currentBaselineCandidate = this->typZeroDustVoltage;

    this->runningAverageCount = runningAverageCount;
    if (this->runningAverageCount) {
        this->runningAverageBuffer = new int16_t[this->runningAverageCount];
        for (uint16_t i = 0; i < this->runningAverageCount; i++) {
            this->runningAverageBuffer[i] = -1;
        }
    }

    this->currentState = IDLE;
    this->stateStartTime = 0;
    this->lastCycleTime = 0;
    this->sampleSum = 0;
    this->sampleCount = 0;
    this->targetSamples = 20;
    this->lastDustDensity = 0;
    this->newDataAvailable = false;
    this->useBlockingMode = true;
}

GP2YDustSensor::~GP2YDustSensor() {
    if (this->runningAverageBuffer) {
        delete[] this->runningAverageBuffer;
    }
}

void GP2YDustSensor::begin() {
    pinMode(this->ledOutputPin, OUTPUT);
    digitalWrite(this->ledOutputPin, HIGH);
}

void GP2YDustSensor::update() {
    if (useBlockingMode) return;

    processStateMachine();
}

bool GP2YDustSensor::available() {
    bool wasAvailable = newDataAvailable;
    newDataAvailable = false;
    return wasAvailable;
}

uint16_t GP2YDustSensor::getDustDensity() {
    if (useBlockingMode) {
        return getDustDensity(targetSamples);
    }
    return lastDustDensity;
}

uint16_t GP2YDustSensor::getDustDensity(uint16_t numSamples) {
    if (!useBlockingMode) {
        return lastDustDensity;
    }

    uint32_t total = 0;
    uint16_t avgRaw;

    for (uint8_t i = 0; i < numSamples; i++) {
        total += this->readDustRawOnce();
        delayMicroseconds(9620);
    }

    avgRaw = total / numSamples;

    float measuredVoltage = avgRaw * (this->vRef / this->adcResolution);
    float actualSensorVoltage = measuredVoltage / this->voltageDividerRatio;
    float scaledVoltage = actualSensorVoltage * calibrationFactor;

    if (scaledVoltage < this->minDustVoltage && scaledVoltage >= minZeroDustVoltage && scaledVoltage <= maxZeroDustVoltage) {
        this->minDustVoltage = scaledVoltage;
    }

    uint16_t dustDensity;

    if (scaledVoltage < zeroDustVoltage) {
        dustDensity = 0;
    } else {
        dustDensity = (scaledVoltage - zeroDustVoltage) / this->sensitivity * 100;
    }

    if (this->runningAverageCount) {
        this->updateRunningAverage(dustDensity);
    }

    if (!hasBaselineCandidate) {
        readCount++;
        if (readCount > BASELINE_CANDIDATE_MIN_READINGS) {
            hasBaselineCandidate = true;
        }
    }

    return dustDensity;
}

uint16_t GP2YDustSensor::getRunningAverage() {
    if (!this->runningAverageCount) {
        return -1;
    }

    float runningAverage = 0;
    uint16_t sampleCount = 0;

    for (uint16_t i = 0; i < this->runningAverageCount; i++) {
        if (this->runningAverageBuffer[i] != -1) {
            runningAverage += this->runningAverageBuffer[i];
            sampleCount++;
        }
    }

    if (sampleCount == 0) {
        return 0;
    }

    runningAverage /= sampleCount;

    return round(runningAverage);
}

float GP2YDustSensor::getBaseline() {
    return this->zeroDustVoltage;
}

void GP2YDustSensor::setBaseline(float zeroDustVoltage) {
    this->zeroDustVoltage = zeroDustVoltage;
}

float GP2YDustSensor::getBaselineCandidate() {
    if (!hasBaselineCandidate) {
        return this->currentBaselineCandidate;
    }

    float candidate = this->minDustVoltage;
    this->minDustVoltage = this->maxZeroDustVoltage;
    this->currentBaselineCandidate = this->minDustVoltage;
    readCount = 0;
    hasBaselineCandidate = false;

    return candidate;
}

void GP2YDustSensor::setSensitivity(float sensitivity) {
    this->sensitivity = sensitivity;
}

float GP2YDustSensor::getSensitivity() {
    return this->sensitivity;
}

void GP2YDustSensor::setCalibrationFactor(float slope) {
    this->calibrationFactor = slope;
}

void GP2YDustSensor::setTargetSamples(uint16_t samples) {
    this->targetSamples = samples;
}

void GP2YDustSensor::setBlockingMode(bool blocking) {
    this->useBlockingMode = blocking;
    if (!blocking) {
        this->currentState = IDLE;
        this->sampleSum = 0;
        this->sampleCount = 0;
    }
}

uint16_t GP2YDustSensor::readDustRawOnce() {
    digitalWrite(this->ledOutputPin, LOW);
    delayMicroseconds(280);
    uint16_t VoRaw = analogRead(this->analogReadPin);
    digitalWrite(this->ledOutputPin, HIGH);
    return VoRaw;
}

void GP2YDustSensor::updateRunningAverage(uint16_t value) {
    this->runningAverageBuffer[this->nextRunningAverageCounter] = value;

    this->nextRunningAverageCounter++;
    if (this->nextRunningAverageCounter >= this->runningAverageCount) {
        this->nextRunningAverageCounter = 0;
    }
}

void GP2YDustSensor::processStateMachine() {
    unsigned long currentTime = micros();

    switch (currentState) {
        case IDLE:
            if (currentTime - lastCycleTime >= 10000) {
                currentState = LED_ON;
                stateStartTime = currentTime;
            }
            break;

        case LED_ON:
            digitalWrite(ledOutputPin, LOW);
            currentState = WAIT_BEFORE_READ;
            stateStartTime = currentTime;
            break;

        case WAIT_BEFORE_READ:
            if (currentTime - stateStartTime >= 280) {
                currentState = READING;
            }
            break;

        case READING:
            sampleSum += analogRead(analogReadPin);
            sampleCount++;
            currentState = LED_OFF;
            break;

        case LED_OFF:
            digitalWrite(ledOutputPin, HIGH);
            lastCycleTime = stateStartTime;

            if (sampleCount >= targetSamples) {
                calculateDustDensity();
                sampleSum = 0;
                sampleCount = 0;
            }

            currentState = IDLE;
            break;
    }
}

void GP2YDustSensor::calculateDustDensity() {
    uint16_t avgRaw = sampleSum / sampleCount;

    float measuredVoltage = avgRaw * (vRef / adcResolution);
    float actualSensorVoltage = measuredVoltage / voltageDividerRatio;
    float scaledVoltage = actualSensorVoltage * calibrationFactor;

    if (scaledVoltage < this->minDustVoltage && scaledVoltage >= minZeroDustVoltage && scaledVoltage <= maxZeroDustVoltage) {
        this->minDustVoltage = scaledVoltage;
    }

    if (scaledVoltage < zeroDustVoltage) {
        lastDustDensity = 0;
    } else {
        lastDustDensity = (scaledVoltage - zeroDustVoltage) / sensitivity * 100;
    }

    if (this->runningAverageCount) {
        this->updateRunningAverage(lastDustDensity);
    }

    if (!hasBaselineCandidate) {
        readCount++;
        if (readCount > BASELINE_CANDIDATE_MIN_READINGS) {
            hasBaselineCandidate = true;
        }
    }

    newDataAvailable = true;
}