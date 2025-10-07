/*
 *  PID.h
 *
 *  PID lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef PID_LIB_H
#define PID_LIB_H

#pragma message("[COMPILED]: PID.h")

#include "Arduino.h"

float mapFloatPID(float x, float iMin, float iMax, float oMin, float oMax);

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

#endif
