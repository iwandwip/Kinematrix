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
        uint8_t enableLeft;
        uint8_t enableRight;
        uint8_t pwmLeft;
        uint8_t pwmRight;
    public:
        MotorDriver(uint8_t EN, uint8_t L_PWM, uint8_t R_PWM);
        MotorDriver(uint8_t L_EN, uint8_t R_EN, uint8_t L_PWM, uint8_t R_PWM);

        void enable();
        void disable();

        void turnLeft(uint8_t pwm);
        void turnRight(uint8_t pwm);
        void stop();
    };
}

namespace L298N {
    class MotorDriver {
    private:
        uint8_t enable;
        uint8_t inputLeft;
        uint8_t inputRight;
        uint8_t pwmChannel;

    public:
        MotorDriver(uint8_t EN, uint8_t IN1, uint8_t IN2);
#if defined(ESP32)
        void setup(int freq = 30000, int ch = 0, int resolution = 8);
#endif
        void turnLeft(uint8_t pwm);
        void turnRight(uint8_t pwm);
        void stop();
    };
}

#endif  // MOTOR_DRIVER_H