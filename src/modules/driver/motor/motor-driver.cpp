#ifdef USE_MOTOR_DRIVER
#include "motor-driver.h"

namespace BTS8960 {
    MotorDriver::MotorDriver(int EN, int L_PWM, int R_PWM)
            : MotorDriver(EN, 0, L_PWM, R_PWM) {

    }

    MotorDriver::MotorDriver(int L_EN, int R_EN, int L_PWM, int R_PWM) {
        pwmLeft = L_PWM;
        pwmRight = R_PWM;
        enableLeft = L_EN;
        enableRight = R_EN;
        pinMode(pwmRight, OUTPUT);
        pinMode(pwmLeft, OUTPUT);
        if (enableLeft == -1 || enableRight == 1) return;
        pinMode(enableLeft, OUTPUT);
        pinMode(enableRight, OUTPUT);
    }

    void MotorDriver::turnRight(int pwm) {
        analogWrite(pwmLeft, 0);
        analogWrite(pwmRight, pwm);
    }

    void MotorDriver::turnLeft(int pwm) {
        analogWrite(pwmLeft, pwm);
        analogWrite(pwmRight, 0);
    }

    void MotorDriver::enable() {
        if (enableLeft == -1 || enableRight == 1) return;
        digitalWrite(enableLeft, HIGH);
        if (enableRight != 0) digitalWrite(enableRight, HIGH);
    }

    void MotorDriver::disable() {
        if (enableLeft == -1 || enableRight == 1) return;
        digitalWrite(enableLeft, LOW);
        if (enableRight != 0) digitalWrite(enableRight, LOW);
    }

    void MotorDriver::stop() {
        analogWrite(pwmLeft, LOW);
        analogWrite(pwmRight, LOW);
    }

}

namespace L298N {
    MotorDriver::MotorDriver(int EN, int IN1, int IN2) {
        enablePin = EN;
        inputLeftPin = IN1;
        inputRightPin = IN2;

        if (enablePin != -1) pinMode(enablePin, OUTPUT);
        if (inputLeftPin != -1) pinMode(inputLeftPin, OUTPUT);
        if (inputRightPin != -1) pinMode(inputRightPin, OUTPUT);
    }

#if defined(ESP32)

    void MotorDriver::setup(int freq, int ch, int resolution) {
        if (enablePin == -1) return;
        pwmChannel = ch;
        ledcSetup(pwmChannel, freq, resolution);
        ledcAttachPin(enablePin, pwmChannel);
    }

#endif

    void MotorDriver::turnLeft(int pwm) {
        if (inputLeftPin != -1) digitalWrite(inputLeftPin, HIGH);
        if (inputRightPin != -1) digitalWrite(inputRightPin, LOW);
        if (enablePin == -1) return;
#if defined(ESP32)
        ledcWrite(pwmChannel, pwm);
#else
        analogWrite(enablePin, pwm);
#endif
    }

    void MotorDriver::turnRight(int pwm) {
        if (inputLeftPin != -1) digitalWrite(inputLeftPin, LOW);
        if (inputRightPin != -1) digitalWrite(inputRightPin, HIGH);
        if (enablePin == -1) return;
#if defined(ESP32)
        ledcWrite(pwmChannel, pwm);
#else
        analogWrite(enablePin, pwm);
#endif
    }

    void MotorDriver::stop() {
        if (inputLeftPin != -1) digitalWrite(inputLeftPin, LOW);
        if (inputRightPin != -1) digitalWrite(inputRightPin, LOW);
        if (enablePin == -1) return;
#if defined(ESP32)
        ledcWrite(pwmChannel, 0);
#else
        analogWrite(enablePin, 0);
#endif
    }
}

#endif