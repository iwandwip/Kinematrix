/*
 *  PID.h
 *
 *  PID lib
 *  Created on: 2023. 4. 3
 */
#ifdef USE_PID
#pragma once

#ifndef PID_LIB_H
#define PID_LIB_H

#include "Arduino.h"

float mapFloat(float x, float iMin, float iMax, float oMin, float oMax);

class PID {
private:
    float kp, ki, kd, td;
    float e, le, se, de;
    float outMin, outMax;
    float u;
public:
    void setConstants(float kp, float ki, float kd, float td);
    void setOutputRange(float min, float max);
    void calculate(float sp, float av);
    float getError(void);
    float getOutput(void);
    float getDeltaError(void);
    float getNegOutput(void);
    void reset(void);
};

class PIDv2 {
private:
    float Kp, Ki, Kd;
    float prevError;
    float integral;
    float target;
    float outputMin, outputMax;
public:
    PIDv2(float kp, float ki, float kd, float setpoint);
    void setSetpoint(float setpoint);
    void setOutputLimits(float min, float max);
    float compute(float input);
};

#define AUTOMATIC    1
#define MANUAL       0
#define DIRECT       0
#define REVERSE      1
#define P_ON_M       0
#define P_ON_E       1

class PIDv3 {
public:
    PIDv3();
    void setMode(int Mode);
    bool compute(float mySetpoint, float myInput);
    void setOutputLimits(float Min, float Max);
    void setTunings(float Kp, float Ki, float Kd, int POn = P_ON_E);
    void setControllerDirection(int Direction);
    void setSampleTime(float NewSampleTime);
    float getKp();
    float getKi();
    float getKd();
    float getOutput();
    int getMode();
    int getDirection();

private:
    void initialize();
    float dispKp;
    float dispKi;
    float dispKd;
    float kp;
    float ki;
    float kd;
    float myOutput;
    int controllerDirection;
    int pOn;
    unsigned long lastTime;
    float outputSum, lastInput;
    float SampleTime;
    float outMin, outMax;
    bool inAuto, pOnE;
};



float mapFloat(float x, float iMin, float iMax, float oMin, float oMax) {
    return (x - iMin) * (oMax - oMin) / (iMax - iMin) + oMin;
}

void PID::setConstants(float kp, float ki, float kd, float td) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->td = td;
}

void PID::setOutputRange(float min, float max) {
    outMin = min;
    outMax = max;
}

void PID::calculate(float sp, float av) {
    e = sp - av;
    de = e - le;

    u = (kp * e) + (ki * se * td) + (kd * de / td);
    if ((u <= outMin && e > 0) || (u >= outMax && e < 0) || (u > outMin && u < outMax)) {
        se += e;
    }
    le = e;

    u = (u < outMin ? outMin : (u > outMax ? outMax : u));
}

float PID::getError(void) {
    return e;
}

float PID::getOutput() {
    return u;
}

float PID::getDeltaError(void) {
    return de;
}

float PID::getNegOutput(void) {
    return mapFloat(u, outMin, outMax, outMax, outMin);
}

void PID::reset() {
    e = 0;
    le = 0;
    se = 0;
    u = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PIDv2::PIDv2(float kp, float ki, float kd, float setpoint) {
    Kp = kp;
    Ki = ki;
    Kd = kd;

    prevError = 0;
    integral = 0;
    target = setpoint;
    outputMin = 0;
    outputMax = 255;
}

void PIDv2::setSetpoint(float setpoint) {
    target = setpoint;
}

void PIDv2::setOutputLimits(float min, float max) {
    outputMin = min;
    outputMax = max;
}

float PIDv2::compute(float input) {
    float error = target - input;
    integral += error;
    float derivative = error - prevError;
    prevError = error;

    float output = Kp * error + Ki * integral + Kd * derivative;

    if (output > outputMax)
        output = outputMax;
    else if (output < outputMin)
        output = outputMin;

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PIDv3::PIDv3() {
    pOn = P_ON_E;
    setMode(AUTOMATIC);
    inAuto = false;
    SampleTime = 100;
    lastTime = millis() - SampleTime;
}

bool PIDv3::compute(float mySetpoint, float myInput) {
    if (!inAuto) return false;
    unsigned long now = millis();
    unsigned long timeChange = (now - lastTime);
    if (timeChange >= SampleTime) {
        /*Compute all the working error variables*/
        float input = myInput;
        float error = mySetpoint - input;
        float dInput = (input - lastInput);
        outputSum += (ki * error);
        /*Add Proportional on Measurement, if P_ON_M is specified*/
        if (!pOnE) outputSum -= kp * dInput;
        if (outputSum > outMax) outputSum = outMax;
        else if (outputSum < outMin) outputSum = outMin;
        /*Add Proportional on Error, if P_ON_E is specified*/
        float output;
        if (pOnE) output = kp * error;
        else output = 0;
        /*Compute Rest of PIDv3 Output*/
        output += outputSum - kd * dInput;
        if (output > outMax) output = outMax;
        else if (output < outMin) output = outMin;
        myOutput = output;
        /*Remember some variables for next time*/
        lastInput = input;
        lastTime = now;
        return true;
    } else return false;
}

void PIDv3::setTunings(float Kp, float Ki, float Kd, int POn) {
    if (Kp < 0 || Ki < 0 || Kd < 0) return;
    pOn = POn;
    pOnE = POn == P_ON_E;
    dispKp = Kp;
    dispKi = Ki;
    dispKd = Kd;
    float SampleTimeInSec = ((float) SampleTime) / 1000;
    kp = Kp;
    ki = Ki * SampleTimeInSec;
    kd = Kd / SampleTimeInSec;
    if (controllerDirection == REVERSE) {
        kp = (0 - kp);
        ki = (0 - ki);
        kd = (0 - kd);
    }
}

void PIDv3::setSampleTime(float NewSampleTime) {
    if (NewSampleTime > 0) {
        float ratio = (float) NewSampleTime / (float) SampleTime;
        ki *= ratio;
        kd /= ratio;
        SampleTime = NewSampleTime;
    }
}

void PIDv3::setOutputLimits(float Min, float Max) {
    if (Min >= Max) return;
    outMin = Min;
    outMax = Max;
    if (inAuto) {
        if (myOutput > outMax) myOutput = outMax;
        else if (myOutput < outMin) myOutput = outMin;
        if (outputSum > outMax) outputSum = outMax;
        else if (outputSum < outMin) outputSum = outMin;
    }
}

void PIDv3::setMode(int Mode) {
    bool newAuto = (Mode == AUTOMATIC);
    if (newAuto && !inAuto) {
        PIDv3::initialize();
    }
    inAuto = newAuto;
}

void PIDv3::initialize() {
    outputSum = myOutput;
    lastInput = 0; //
    if (outputSum > outMax) outputSum = outMax;
    else if (outputSum < outMin) outputSum = outMin;
}

void PIDv3::setControllerDirection(int Direction) {
    if (inAuto && Direction != controllerDirection) {
        kp = (0 - kp);
        ki = (0 - ki);
        kd = (0 - kd);
    }
    controllerDirection = Direction;
}

float PIDv3::getKp() {
    return dispKp;
}

float PIDv3::getKi() {
    return dispKi;
}

float PIDv3::getKd() {
    return dispKd;
}

float PIDv3::getOutput() {
    return myOutput;
}

int PIDv3::getMode() {
    return inAuto ? AUTOMATIC : MANUAL;
}

int PIDv3::getDirection() {
    return controllerDirection;
}
#endif
#endif