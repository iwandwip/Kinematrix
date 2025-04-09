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
    float getSetPoint();
    void setKp(float p);
    void setKi(float i);
    void setKd(float d);
    float getKp();
    float getKi();
    float getKd();
    void setIntegralLimit(float limit);
    float getIntegralLimit();
    void calculateOptimalIntegralLimit();
    void reset();
    float getProportionalComponent();
    float getIntegralComponent();
    float getDerivativeComponent();
    float compute(float currentInput);

    void enableDerivativeFilter(float alpha);
    void disableDerivativeFilter();
    float filterDerivative(float rawDerivative);

    void setDeadband(float band);
    float getDeadband();

    void enableSetpointRamping(float ratePerSecond);
    void disableSetpointRamping();
    float getSetpointRampRate();
    float getCurrentRampedSetpoint();

    void setOutputRateLimit(float maxChangePerSecond);
    void disableOutputRateLimit();
    float getOutputRateLimit();

    void setControllerDirection(bool reverse);
    bool getControllerDirection();

    void resetPerformanceMetrics();
    void setSettlingThreshold(float threshold);
    float getSettlingTime();
    float getOvershoot();
    bool isSystemSettled();

    // Auto-tuning methods
    bool autoTuneZN1(float testInput, float stepAmplitude, unsigned long maxTuningTime);
    bool autoTuneZN2(float testInput, float initialKp, float kpStep, unsigned long maxTuningTime);
    bool autoTuneCohenCoon(float testInput, float stepAmplitude, unsigned long maxTuningTime);
    bool isTuning();
    void cancelTuning();
    float getUltimateGain();
    float getUltimatePeriod();

    // EEPROM operations
    void setEEPROMAddress(int address);
    bool saveParametersToEEPROM();
    bool loadParametersFromEEPROM();
};

#endif