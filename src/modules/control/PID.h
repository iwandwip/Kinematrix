/*
 *  PID.h
 *
 *  PID lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef PID_LIB_H
#define PID_LIB_H

#include "Arduino.h"

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
    float mapFloat(float x, float iMin, float iMax, float oMin, float oMax);
};

class PIDv2 {
private:
    float Kp, Ki, Kd;
    float prevError;
    float integral;
    float target;
    float outputMin, outputMax;
public:
    PIDv2(float kp, float ki, float kd, float setpoint) {
        Kp = kp;
        Ki = ki;
        Kd = kd;

        prevError = 0;
        integral = 0;
        target = setpoint;
        outputMin = 0;
        outputMax = 255;
    }

    void setSetpoint(float setpoint) {
        target = setpoint;
    }

    void setOutputLimits(float min, float max) {
        outputMin = min;
        outputMax = max;
    }

    float compute(float input) {
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
};

#endif
