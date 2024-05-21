/*
 *  PID.h
 *
 *  PID lib
 *  Created on: 2023. 4. 3
 */

#include "PID.h"

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

float PID::mapFloat(float x, float iMin, float iMax, float oMin, float oMax) {
    return (x - iMin) * (oMax - oMin) / (iMax - iMin) + oMin;
}