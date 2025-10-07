#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <Arduino.h>

// Platform detection
#if defined(ESP32) || defined(ESP8266)
#include <EEPROM.h>
#define IS_ESP true
#else
#include <EEPROM.h>
#define IS_ESP false
#endif

class PIDController {
private:
    float kp;
    float ki;
    float kd;
    float setPoint;
    float targetSetPoint;
    float input;
    float output;
    float lastOutput;
    float error;
    float lastError;
    float integral;
    float derivative;
    float dt;
    float integralMax;
    float outputMin;
    float outputMax;
    unsigned long lastTime;
    bool firstRun;

    bool useDerivativeFilter;
    float derivativeFilterAlpha;
    float lastFilteredDerivative;

    float deadband;

    bool useSetpointRamping;
    float setpointRampRate;

    bool useOutputRateLimit;
    float outputRateLimit;

    bool isReverse;

    unsigned long startTime;
    float peakValue;
    float steadyStateValue;
    bool isSettled;
    float settlingThreshold;

    // Auto-tuning variables
    float ultimateGain;
    float ultimatePeriod;
    float testAmplitude;
    bool tuningInProgress;
    unsigned long tuningStartTime;
    float *tuningData;
    int tuningDataSize;
    int tuningDataIndex;
    int oscillationCount;
    int tuningMethod;  // 0=ZN1, 1=ZN2, 2=Cohen-Coon
    float kpStep;
    float initialKp;
    float prevInput;
    float maxInput;
    float minInput;
    unsigned long lastPeakTime;
    int peakCount;

    // Cohen-Coon specific variables
    float initialValue;
    float ccSteadyStateValue;  // Renamed to avoid duplicate
    float processGain;
    float deadTime;
    float timeConstant;
    bool foundDeadTime;
    bool foundResponseTime;

    // EEPROM settings
    int eepromAddress;
    const int eepromSize = 32;  // Bytes needed for parameters

    void updatePerformanceMetrics();
    void calculateZieglerNicholsParameters(char tuningType);
    void calculateCohenCoonParameters();

public:
    PIDController(float p, float i, float d, float timeStep, float minOut, float maxOut);
    ~PIDController();
    void setSetPoint(float sp);
    float getSetPoint() const;
    void setTunings(float kp, float ki, float kd);
    void setKp(float p);
    void setKi(float i);
    void setKd(float d);
    float getKp() const;
    float getKi() const;
    float getKd() const;
    void setIntegralLimit(float limit);
    float getIntegralLimit() const;
    void calculateOptimalIntegralLimit();
    void reset();
    float getProportionalComponent() const;
    float getIntegralComponent() const;
    float getDerivativeComponent() const;
    float compute(float currentInput);

    void enableDerivativeFilter(float alpha);
    void disableDerivativeFilter();
    float filterDerivative(float rawDerivative);

    void setDeadband(float band);
    float getDeadband() const;

    void enableSetpointRamping(float ratePerSecond);
    void disableSetpointRamping();
    float getSetpointRampRate() const;
    float getCurrentRampedSetpoint() const;

    void setOutputRateLimit(float maxChangePerSecond);
    void disableOutputRateLimit();
    float getOutputRateLimit() const;

    void setControllerDirection(bool reverse);
    bool getControllerDirection() const;

    void resetPerformanceMetrics();
    void setSettlingThreshold(float threshold);
    float getSettlingTime() const;
    float getOvershoot() const;
    bool isSystemSettled() const;

    // Auto-tuning methods
    bool autoTuneZN1(float testInput, float stepAmplitude, unsigned long maxTuningTime);
    bool autoTuneZN2(float testInput, float initialKp, float kpStep, unsigned long maxTuningTime);
    bool autoTuneCohenCoon(float testInput, float stepAmplitude, unsigned long maxTuningTime);
    bool isTuning() const;
    void cancelTuning();
    float getUltimateGain() const;
    float getUltimatePeriod() const;

    // EEPROM operations
    void setEEPROMAddress(int address);
    bool saveParametersToEEPROM() const;
    bool loadParametersFromEEPROM();
};

#endif