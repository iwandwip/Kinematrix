#ifndef GP2Y_DUST_SENSOR_H
#define GP2Y_DUST_SENSOR_H

#include <stdint.h>

enum GP2YDustSensorType {
    GP2Y1010AU0F,
    GP2Y1014AU0F
};

class GP2YDustSensor {
private:
    enum State {
        IDLE,
        LED_ON,
        WAIT_BEFORE_READ,
        READING,
        LED_OFF,
        WAIT_NEXT_CYCLE
    };

    GP2YDustSensorType type;
    uint8_t ledOutputPin;
    uint8_t analogReadPin;
    float vRef;
    float adcResolution;
    float voltageDividerRatio;
    float zeroDustVoltage;
    float minDustVoltage;
    float minZeroDustVoltage;
    float maxZeroDustVoltage;
    float typZeroDustVoltage;
    float currentBaselineCandidate;
    bool hasBaselineCandidate;
    uint16_t readCount;
    float calibrationFactor;
    float sensitivity;
    int16_t *runningAverageBuffer;
    int runningAverageCount;
    int nextRunningAverageCounter;
    const uint8_t BASELINE_CANDIDATE_MIN_READINGS = 10;

    State currentState;
    unsigned long stateStartTime;
    unsigned long lastCycleTime;
    uint32_t sampleSum;
    uint16_t sampleCount;
    uint16_t targetSamples;
    uint16_t lastDustDensity;
    bool newDataAvailable;
    bool useBlockingMode;

protected:
    uint16_t readDustRawOnce();
    void updateRunningAverage(uint16_t dustDensity);
    void processStateMachine();
    void calculateDustDensity();

public:
    GP2YDustSensor(
            GP2YDustSensorType type,
            uint8_t ledOutputPin,
            uint8_t analogReadPin,
            uint16_t runningAverageCount = 60,
            float vRef = 5.0,
            float adcResolution = 1024.0,
            float voltageDividerRatio = 1.0
    );
    ~GP2YDustSensor();
    void begin();
    void update();
    bool available();
    uint16_t getDustDensity();
    uint16_t getDustDensity(uint16_t numSamples);
    uint16_t getRunningAverage();
    float getBaseline();
    void setBaseline(float zeroDustVoltage);
    float getBaselineCandidate();
    void setSensitivity(float sensitivity);
    float getSensitivity();
    void setCalibrationFactor(float slope);
    void setTargetSamples(uint16_t samples);
    void setBlockingMode(bool blocking);
};

#endif