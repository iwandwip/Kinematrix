#include "motor-driver.h"

namespace BTS8960 {
    MotorDriver::MotorDriver(uint8_t EN, uint8_t L_PWM, uint8_t R_PWM)
            : MotorDriver(EN, 0, L_PWM, R_PWM) {

    }

    MotorDriver::MotorDriver(uint8_t L_EN, uint8_t R_EN, uint8_t L_PWM, uint8_t R_PWM) {
        pwmLeft = L_PWM;
        pwmRight = R_PWM;
        enableLeft = L_EN;
        enableRight = R_EN;
        pinMode(pwmRight, OUTPUT);
        pinMode(pwmLeft, OUTPUT);
        pinMode(enableLeft, OUTPUT);
        pinMode(enableRight, OUTPUT);
    }

    void MotorDriver::turnRight(uint8_t pwm) {
        analogWrite(pwmLeft, 0);
        analogWrite(pwmRight, pwm);
    }

    void MotorDriver::turnLeft(uint8_t pwm) {
        analogWrite(pwmLeft, pwm);
        analogWrite(pwmRight, 0);
    }

    void MotorDriver::enable() {
        digitalWrite(enableLeft, HIGH);
        if (enableRight != 0) digitalWrite(enableRight, HIGH);
    }

    void MotorDriver::disable() {
        digitalWrite(enableLeft, LOW);
        if (enableRight != 0) digitalWrite(enableRight, LOW);
    }

    void MotorDriver::stop() {
        analogWrite(pwmLeft, LOW);
        analogWrite(pwmRight, LOW);
    }

}

namespace L298N {
    MotorDriver::MotorDriver(uint8_t EN, uint8_t IN1, uint8_t IN2) {
        enable = EN;
        inputLeft = IN1;
        inputRight = IN2;

        pinMode(enable, OUTPUT);
        pinMode(inputLeft, OUTPUT);
        pinMode(inputRight, OUTPUT);
    }

#if defined(ESP32)

    void MotorDriver::setup(int freq, int ch, int resolution) {
        pwmChannel = ch;
        ledcSetup(pwmChannel, freq, resolution);
        ledcAttachPin(enable, pwmChannel);
    }

#endif

    void MotorDriver::turnLeft(uint8_t pwm) {
        digitalWrite(inputLeft, HIGH);
        digitalWrite(inputRight, LOW);
#if defined(ESP32)
        ledcWrite(pwmChannel, pwm);
#else
        analogWrite(pwmChannel, pwm);
#endif
    }

    void MotorDriver::turnRight(uint8_t pwm) {
        digitalWrite(inputLeft, LOW);
        digitalWrite(inputRight, HIGH);
#if defined(ESP32)
        ledcWrite(pwmChannel, pwm);
#else
        analogWrite(pwmChannel, pwm);
#endif
    }

    void MotorDriver::stop() {
        digitalWrite(inputLeft, LOW);
        digitalWrite(inputRight, LOW);
#if defined(ESP32)
        ledcWrite(pwmChannel, 0);
#else
        analogWrite(pwmChannel, 0);
#endif
    }
}