#include "PIDController.h"

PIDController::PIDController(float p, float i, float d, float timeStep, float minOut, float maxOut) {
    kp = p;
    ki = i;
    kd = d;
    dt = timeStep;
    outputMin = minOut;
    outputMax = maxOut;
    setPoint = 0;
    targetSetPoint = 0;
    input = 0;
    output = 0;
    lastOutput = 0;
    error = 0;
    lastError = 0;
    integral = 0;
    derivative = 0;
    integralMax = 50.0;
    lastTime = 0;
    firstRun = true;

    useDerivativeFilter = false;
    derivativeFilterAlpha = 0.2;
    lastFilteredDerivative = 0;

    deadband = 0;

    useSetpointRamping = false;
    setpointRampRate = 0;

    useOutputRateLimit = false;
    outputRateLimit = 0;

    isReverse = false;

    startTime = 0;
    peakValue = 0;
    steadyStateValue = 0;
    isSettled = false;
    settlingThreshold = 0.05;

    ultimateGain = 0;
    ultimatePeriod = 0;
    testAmplitude = 0;
    tuningInProgress = false;
    tuningStartTime = 0;
    tuningData = NULL;
    tuningDataSize = 0;
    tuningDataIndex = 0;
    oscillationCount = 0;
    tuningMethod = 0;
    kpStep = 0;
    initialKp = 0;
    prevInput = 0;
    maxInput = 0;
    minInput = 0;
    lastPeakTime = 0;
    peakCount = 0;

    initialValue = 0;
    ccSteadyStateValue = 0;
    processGain = 0;
    deadTime = 0;
    timeConstant = 0;
    foundDeadTime = false;
    foundResponseTime = false;

    eepromAddress = 0;
}

PIDController::~PIDController() {
    if (tuningData != NULL) {
        delete[] tuningData;
        tuningData = NULL;
    }
}

void PIDController::setSetPoint(float sp) {
    targetSetPoint = sp;
    if (!useSetpointRamping) {
        setPoint = sp;
    }
    resetPerformanceMetrics();
}

float PIDController::getSetPoint() {
    return targetSetPoint;
}

void PIDController::setKp(float p) {
    kp = p;
}

void PIDController::setKi(float i) {
    ki = i;
    integral = 0;
}

void PIDController::setKd(float d) {
    kd = d;
}

float PIDController::getKp() {
    return kp;
}

float PIDController::getKi() {
    return ki;
}

float PIDController::getKd() {
    return kd;
}

void PIDController::setIntegralLimit(float limit) {
    integralMax = limit;
}

float PIDController::getIntegralLimit() {
    return integralMax;
}

void PIDController::calculateOptimalIntegralLimit() {
    float outputRange = outputMax - outputMin;
    float normalizationFactor = 5.0;

    if (ki > 0) {
        integralMax = outputRange / (ki * normalizationFactor);

        float maxAllowedContribution = 0.4 * outputRange;
        float maxAllowedLimit = maxAllowedContribution / ki;

        if (integralMax > maxAllowedLimit) {
            integralMax = maxAllowedLimit;
        }
    } else {
        integralMax = outputRange;
    }
}

void PIDController::reset() {
    integral = 0;
    lastError = 0;
    firstRun = true;
    lastFilteredDerivative = 0;
    lastOutput = 0;
    resetPerformanceMetrics();
}

float PIDController::getProportionalComponent() {
    return kp * error;
}

float PIDController::getIntegralComponent() {
    return ki * integral;
}

float PIDController::getDerivativeComponent() {
    return kd * derivative;
}

float PIDController::compute(float currentInput) {
    if (tuningInProgress) {
        unsigned long now = millis();

        if (now - lastTime >= (dt * 1000)) {
            lastTime = now;

            if (tuningMethod == 1) { // ZN2
                float amplitude = maxInput - minInput;

                // Check if max tuning time has been exceeded
                if (now - tuningStartTime > tuningDataSize * dt * 1000) {
                    cancelTuning();
                    return output;
                }

                if (prevInput < currentInput && currentInput > input) {
                    maxInput = currentInput;
                }

                if (prevInput > currentInput && currentInput < input) {
                    minInput = currentInput;

                    if (maxInput - minInput > 0.1) {
                        unsigned long peakInterval = now - lastPeakTime;
                        if (lastPeakTime > 0 && peakInterval > 100) {
                            if (peakCount == 0) {
                                ultimatePeriod = peakInterval;
                            } else {
                                ultimatePeriod = (ultimatePeriod * peakCount + peakInterval) / (peakCount + 1);
                            }
                            peakCount++;
                        }
                        lastPeakTime = now;
                    }
                }

                prevInput = currentInput;
                input = currentInput;

                if (peakCount >= 5) {
                    ultimateGain = kp;
                    cancelTuning();
                    calculateZieglerNicholsParameters('D');
                    return output;
                }

                if (now - tuningStartTime >= 5000 && amplitude < 0.1) {
                    kp += kpStep;
                }

                error = setPoint - input;
                output = kp * error;

                if (output > outputMax) output = outputMax;
                if (output < outputMin) output = outputMin;

                return output;
            } else if (tuningMethod == 2) { // Cohen-Coon
                input = currentInput;
                unsigned long elapsedTime = now - tuningStartTime;

                // First save initial value
                if (elapsedTime < 500) {
                    initialValue = currentInput;
                    return 0; // Hold outputs at zero initially
                }

                // Apply step after getting stable initial value
                if (elapsedTime < 1000) {
                    output = testAmplitude;
                    return output;
                }

                // Detect when response starts (dead time detection)
                if (!foundDeadTime && abs(currentInput - initialValue) > (0.05 * testAmplitude)) {
                    deadTime = (elapsedTime - 1000) / 1000.0; // Convert to seconds
                    foundDeadTime = true;
                }

                // Store response data
                if (tuningDataIndex < tuningDataSize) {
                    tuningData[tuningDataIndex++] = currentInput;
                }

                // After enough time, analyze data
                if (elapsedTime > 30000 || tuningDataIndex >= tuningDataSize) {
                    // Find steady state value
                    float sum = 0;
                    int count = 0;
                    for (int i = tuningDataIndex - 10; i < tuningDataIndex; i++) {
                        if (i >= 0) {
                            sum += tuningData[i];
                            count++;
                        }
                    }

                    if (count > 0) {
                        ccSteadyStateValue = sum / count;
                    }

                    // Calculate process gain
                    processGain = (ccSteadyStateValue - initialValue) / testAmplitude;

                    // Find time constant (63.2% of change)
                    float targetValue = initialValue + 0.632 * (ccSteadyStateValue - initialValue);
                    for (int i = 0; i < tuningDataIndex; i++) {
                        if (tuningData[i] >= targetValue && !foundResponseTime) {
                            timeConstant = (i * dt) - deadTime;
                            foundResponseTime = true;
                            break;
                        }
                    }

                    // Calculate Cohen-Coon parameters
                    calculateCohenCoonParameters();
                    cancelTuning();
                }

                return output;
            } else { // ZN1
                if (tuningDataIndex < tuningDataSize) {
                    tuningData[tuningDataIndex++] = currentInput;

                    if (tuningDataIndex >= tuningDataSize || (now - tuningStartTime > 60000)) {
                        cancelTuning();
                    }
                }
            }
        }

        return output;
    }

    unsigned long now = millis();

    if (firstRun || (now - lastTime >= (dt * 1000))) {
        lastTime = now;

        if (firstRun) {
            firstRun = false;
            lastOutput = currentInput;
            startTime = now;
        }

        if (useSetpointRamping && setPoint != targetSetPoint) {
            float maxChange = setpointRampRate * dt;
            if (targetSetPoint > setPoint) {
                setPoint += maxChange;
                if (setPoint > targetSetPoint) setPoint = targetSetPoint;
            } else {
                setPoint -= maxChange;
                if (setPoint < targetSetPoint) setPoint = targetSetPoint;
            }
        }

        input = currentInput;

        updatePerformanceMetrics();

        error = setPoint - input;
        if (isReverse) error = -error;

        if (abs(error) < deadband) {
            error = 0;
        }

        integral += error * dt;

        if (integral > integralMax) integral = integralMax;
        if (integral < -integralMax) integral = -integralMax;

        if (abs(error) > (abs(setPoint) * 0.5)) {
            integral = 0;
        }

        float rawDerivative = (error - lastError) / dt;

        if (useDerivativeFilter) {
            derivative = filterDerivative(rawDerivative);
        } else {
            derivative = rawDerivative;
        }

        lastError = error;

        output = kp * error + ki * integral + kd * derivative;

        if (output > outputMax) output = outputMax;
        if (output < outputMin) output = outputMin;

        if (useOutputRateLimit) {
            float maxChange = outputRateLimit * dt;
            if (output > lastOutput + maxChange) {
                output = lastOutput + maxChange;
            } else if (output < lastOutput - maxChange) {
                output = lastOutput - maxChange;
            }
        }

        lastOutput = output;
    }

    return output;
}

void PIDController::enableDerivativeFilter(float alpha) {
    useDerivativeFilter = true;
    derivativeFilterAlpha = alpha;
    lastFilteredDerivative = 0;
}

void PIDController::disableDerivativeFilter() {
    useDerivativeFilter = false;
}

float PIDController::filterDerivative(float rawDerivative) {
    float filteredDerivative = derivativeFilterAlpha * rawDerivative +
                               (1.0 - derivativeFilterAlpha) * lastFilteredDerivative;
    lastFilteredDerivative = filteredDerivative;
    return filteredDerivative;
}

void PIDController::setDeadband(float band) {
    deadband = band;
}

float PIDController::getDeadband() {
    return deadband;
}

void PIDController::enableSetpointRamping(float ratePerSecond) {
    useSetpointRamping = true;
    setpointRampRate = ratePerSecond;
}

void PIDController::disableSetpointRamping() {
    useSetpointRamping = false;
    setPoint = targetSetPoint;
}

float PIDController::getSetpointRampRate() {
    return setpointRampRate;
}

float PIDController::getCurrentRampedSetpoint() {
    return setPoint;
}

void PIDController::setOutputRateLimit(float maxChangePerSecond) {
    useOutputRateLimit = true;
    outputRateLimit = maxChangePerSecond;
}

void PIDController::disableOutputRateLimit() {
    useOutputRateLimit = false;
}

float PIDController::getOutputRateLimit() {
    return outputRateLimit;
}

void PIDController::setControllerDirection(bool reverse) {
    isReverse = reverse;
}

bool PIDController::getControllerDirection() {
    return isReverse;
}

void PIDController::resetPerformanceMetrics() {
    startTime = millis();
    peakValue = input;
    steadyStateValue = 0;
    isSettled = false;
}

void PIDController::setSettlingThreshold(float threshold) {
    settlingThreshold = threshold;
}

void PIDController::updatePerformanceMetrics() {
    if (abs(input - setPoint) > abs(peakValue - setPoint)) {
        peakValue = input;
    }

    float errorPercent = abs(input - setPoint) / abs(setPoint);
    if (!isSettled && errorPercent <= settlingThreshold) {
        isSettled = true;
        steadyStateValue = input;
    }
}

float PIDController::getSettlingTime() {
    if (isSettled) {
        return (millis() - startTime) / 1000.0;
    }
    return -1.0;
}

float PIDController::getOvershoot() {
    if (steadyStateValue != 0) {
        float overshoot = (peakValue - steadyStateValue) / abs(setPoint - input) * 100.0;
        return overshoot;
    }
    return 0.0;
}

bool PIDController::isSystemSettled() {
    return isSettled;
}

bool PIDController::autoTuneZN1(float testInput, float stepAmplitude, unsigned long maxTuningTime) {
    if (tuningInProgress) {
        cancelTuning();
    }

    tuningInProgress = true;
    tuningMethod = 0;
    testAmplitude = stepAmplitude;
    tuningStartTime = millis();

    tuningDataSize = maxTuningTime / (dt * 1000) + 10;
    tuningData = new float[tuningDataSize];
    if (tuningData == NULL) {
        tuningInProgress = false;
        return false;
    }

    tuningDataIndex = 0;
    oscillationCount = 0;

    // Save old values if needed later
    // float oldKi = ki;
    // float oldKd = kd;

    ki = 0;
    kd = 0;

    kp = 0.5;

    setSetPoint(testInput + stepAmplitude);

    return true;
}

bool PIDController::autoTuneZN2(float testInput, float initialKpVal, float kpStepValue, unsigned long maxTuningTime) {
    if (tuningInProgress) {
        cancelTuning();
    }

    tuningInProgress = true;
    tuningMethod = 1;
    tuningStartTime = millis();

    initialKp = initialKpVal;
    kpStep = kpStepValue;

    // Store maxTuningTime for timeout check
    tuningDataSize = maxTuningTime / (dt * 1000);

    // Save old values if needed later
    // float oldKi = ki;
    // float oldKd = kd;

    ki = 0;
    kd = 0;

    kp = initialKp;

    setSetPoint(testInput);

    prevInput = testInput;
    input = testInput;
    maxInput = testInput;
    minInput = testInput;
    lastPeakTime = 0;
    peakCount = 0;

    return true;
}

bool PIDController::autoTuneCohenCoon(float testInput, float stepAmplitude, unsigned long maxTuningTime) {
    if (tuningInProgress) {
        cancelTuning();
    }

    tuningInProgress = true;
    tuningMethod = 2;
    testAmplitude = stepAmplitude;
    tuningStartTime = millis();

    tuningDataSize = maxTuningTime / (dt * 1000) + 10;
    tuningData = new float[tuningDataSize];
    if (tuningData == NULL) {
        tuningInProgress = false;
        return false;
    }

    tuningDataIndex = 0;
    initialValue = 0;
    ccSteadyStateValue = 0;
    processGain = 0;
    deadTime = 0;
    timeConstant = 0;
    foundDeadTime = false;
    foundResponseTime = false;

    // Save old values if needed later
    // float oldKi = ki;
    // float oldKd = kd;

    ki = 0;
    kd = 0;
    kp = 0;

    output = 0;
    setSetPoint(testInput);

    return true;
}

bool PIDController::isTuning() {
    return tuningInProgress;
}

void PIDController::cancelTuning() {
    tuningInProgress = false;

    if (tuningData != NULL) {
        if (tuningMethod == 0 && tuningDataIndex > 10) { // ZN1
            int peaks = 0;
            // int valleys = 0; // Unused variable
            unsigned long firstPeakTime = 0;
            unsigned long lastPeakTimeVal = 0;

            for (int i = 2; i < tuningDataIndex - 1; i++) {
                if (tuningData[i] > tuningData[i - 1] && tuningData[i] > tuningData[i + 1]) {
                    peaks++;
                    if (peaks == 1) {
                        firstPeakTime = i * dt * 1000;
                    }
                    lastPeakTimeVal = i * dt * 1000;
                }

                // Valleys code is kept but the variable is unused
                // if (tuningData[i] < tuningData[i - 1] && tuningData[i] < tuningData[i + 1]) {
                //     valleys++;
                // }
            }

            if (peaks >= 3) {
                ultimatePeriod = (lastPeakTimeVal - firstPeakTime) / float(peaks - 1);
                ultimateGain = kp * 1.1;

                calculateZieglerNicholsParameters('D');
            }
        }

        delete[] tuningData;
        tuningData = NULL;
        tuningDataIndex = 0;
    }
}

float PIDController::getUltimateGain() {
    return ultimateGain;
}

float PIDController::getUltimatePeriod() {
    return ultimatePeriod;
}

void PIDController::calculateZieglerNicholsParameters(char tuningType) {
    if (ultimateGain == 0 || ultimatePeriod == 0) {
        return;
    }

    switch (tuningType) {
        case 'P': // P controller
            kp = 0.5 * ultimateGain;
            ki = 0;
            kd = 0;
            break;

        case 'I': // PI controller
            kp = 0.45 * ultimateGain;
            ki = kp / (0.83 * ultimatePeriod);
            kd = 0;
            break;

        case 'D': // PID controller
            kp = 0.6 * ultimateGain;
            ki = kp / (0.5 * ultimatePeriod);
            kd = kp * (0.125 * ultimatePeriod);
            break;

        default:
            break;
    }

    reset();
}

void PIDController::calculateCohenCoonParameters() {
    if (processGain == 0 || deadTime == 0 || timeConstant == 0) {
        return;
    }

    float r = deadTime / timeConstant;

    // PID parameters according to Cohen-Coon method
    kp = (1.35 / processGain) * (1 + (0.18 * r));
    ki = kp / (deadTime * (2.5 - 2 * r) / (1 + 0.39 * r));
    kd = kp * deadTime * (0.37 - 0.37 * r) / (1 + 0.18 * r);

    reset();
}

void PIDController::setEEPROMAddress(int address) {
    eepromAddress = address;
}

bool PIDController::saveParametersToEEPROM() {
    int addr = eepromAddress;

#if IS_ESP
    if (!EEPROM.begin(eepromSize)) {
        return false;
    }
#endif

    EEPROM.put(addr, kp);
    addr += sizeof(float);
    EEPROM.put(addr, ki);
    addr += sizeof(float);
    EEPROM.put(addr, kd);
    addr += sizeof(float);

    EEPROM.put(addr, outputMin);
    addr += sizeof(float);
    EEPROM.put(addr, outputMax);
    addr += sizeof(float);

    EEPROM.put(addr, integralMax);
    addr += sizeof(float);

    EEPROM.put(addr, deadband);
    addr += sizeof(float);

    EEPROM.put(addr, ultimateGain);
    addr += sizeof(float);
    EEPROM.put(addr, ultimatePeriod);
    addr += sizeof(float);

#if IS_ESP
    return EEPROM.commit();
#else
    return true;
#endif
}

bool PIDController::loadParametersFromEEPROM() {
    int addr = eepromAddress;

#if IS_ESP
    if (!EEPROM.begin(eepromSize)) {
        return false;
    }
#endif

    EEPROM.get(addr, kp);
    addr += sizeof(float);
    EEPROM.get(addr, ki);
    addr += sizeof(float);
    EEPROM.get(addr, kd);
    addr += sizeof(float);

    EEPROM.get(addr, outputMin);
    addr += sizeof(float);
    EEPROM.get(addr, outputMax);
    addr += sizeof(float);

    EEPROM.get(addr, integralMax);
    addr += sizeof(float);

    EEPROM.get(addr, deadband);
    addr += sizeof(float);

    EEPROM.get(addr, ultimateGain);
    addr += sizeof(float);
    EEPROM.get(addr, ultimatePeriod);
    addr += sizeof(float);

    return true;
}