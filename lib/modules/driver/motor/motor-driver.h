#pragma once

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#pragma message("[COMPILED]: motor-driver.h")

#include "Arduino.h"

namespace BTS7960 {
    class MotorDriver {
    private:
        int enableLeft;
        int enableRight;
        int pwmLeft;
        int pwmRight;
#if defined(ESP32)
        int pwmChannelLeft;
        int pwmChannelRight;
#endif
    public:
        MotorDriver(int L_PWM, int R_PWM);
        MotorDriver(int EN, int L_PWM, int R_PWM);
        MotorDriver(int L_EN, int R_EN, int L_PWM, int R_PWM);

#if defined(ESP32)
        void setup(int freq = 30000, int chLeft = 0, int chRight = 1, int resolution = 8);
#endif
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
#if defined(ESP32)
        int pwmChannel;
        int pwmFreq;
        int pwmResolution;
#endif

    public:
        MotorDriver(int IN1, int IN2);
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

#endif