/*
 *  motor-driver.h
 *
 *  motor-driver driver
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "Arduino.h"

namespace BTS8960 {
    class MotorDriver {
    private:
        int enableLeft;
        int enableRight;
        int pwmLeft;
        int pwmRight;
    public:
        MotorDriver(int EN, int L_PWM, int R_PWM);
        MotorDriver(int L_EN, int R_EN, int L_PWM, int R_PWM);

        void enable();
        void disable();

        void turnLeft(int pwm);
        void turnRight(int pwm);
        void stop();

        static const int NC = -1;
    };
}

namespace L298N {
    class MotorDriver {
    private:
        int enablePin;
        int inputLeftPin;
        int inputRightPin;
        int pwmChannel;

    public:
        MotorDriver(int EN, int IN1, int IN2);
#if defined(ESP32)
        void setup(int freq = 30000, int ch = 0, int resolution = 8);
#endif
        void turnLeft(int pwm);
        void turnRight(int pwm);
        void stop();

        static const int NC = -1;
    };
}

#endif  // MOTOR_DRIVER_H