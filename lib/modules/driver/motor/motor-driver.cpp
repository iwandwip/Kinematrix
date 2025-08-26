#include "motor-driver.h"

namespace BTS7960 {
    MotorDriver::MotorDriver(int L_PWM)
            : MotorDriver(NC, NC, L_PWM, NC) {
    }

    MotorDriver::MotorDriver(int L_PWM, int R_PWM)
            : MotorDriver(NC, NC, L_PWM, R_PWM) {
    }

    MotorDriver::MotorDriver(int EN, int L_PWM, int R_PWM)
            : MotorDriver(EN, EN, L_PWM, R_PWM) {
    }

    MotorDriver::MotorDriver(int L_EN, int R_EN, int L_PWM, int R_PWM) {
        pwmLeft = L_PWM;
        pwmRight = R_PWM;
        enableLeft = L_EN;
        enableRight = R_EN;
#if defined(ESP32)
        if (pwmLeft != NC) pinMode(pwmLeft, OUTPUT);
        if (pwmRight != NC) pinMode(pwmRight, OUTPUT);
#else
        if (pwmLeft != NC) pinMode(pwmLeft, OUTPUT);
        if (pwmRight != NC) pinMode(pwmRight, OUTPUT);
#endif
        if (enableLeft != NC) {
            pinMode(enableLeft, OUTPUT);
        }
        if (enableRight != NC) {
            pinMode(enableRight, OUTPUT);
        }
    }

#if defined(ESP32)

    void MotorDriver::setup(int chLeft, int freq, int resolution) {
        pwmChannelLeft = chLeft;
        if (pwmLeft != NC) {
            ledcSetup(pwmChannelLeft, freq, resolution);
            ledcAttachPin(pwmLeft, pwmChannelLeft);
        }
    }

    void MotorDriver::setup(int chLeft, int chRight, int freq, int resolution) {
        pwmChannelLeft = chLeft;
        pwmChannelRight = chRight;
        if (pwmLeft != NC) {
            ledcSetup(pwmChannelLeft, freq, resolution);
            ledcAttachPin(pwmLeft, pwmChannelLeft);
        }
        if (pwmRight != NC) {
            ledcSetup(pwmChannelRight, freq, resolution);
            ledcAttachPin(pwmRight, pwmChannelRight);
        }
    }

#endif

    void MotorDriver::run(int pwm) {
#if defined(ESP32)
        if (pwmLeft != NC) ledcWrite(pwmChannelLeft, pwm);
#else
        if (pwmLeft != NC) analogWrite(pwmLeft, pwm);
#endif
    }

    void MotorDriver::turnLeft(int pwm) {
#if defined(ESP32)
        if (pwmLeft != NC) ledcWrite(pwmChannelLeft, pwm);
        if (pwmRight != NC) ledcWrite(pwmChannelRight, 0);
#else
        if (pwmLeft != NC) analogWrite(pwmLeft, pwm);
        if (pwmRight != NC) analogWrite(pwmRight, 0);
#endif
    }

    void MotorDriver::turnRight(int pwm) {
#if defined(ESP32)
        if (pwmLeft != NC) ledcWrite(pwmChannelLeft, 0);
        if (pwmRight != NC) ledcWrite(pwmChannelRight, pwm);
#else
        if (pwmLeft != NC) analogWrite(pwmLeft, 0);
        if (pwmRight != NC) analogWrite(pwmRight, pwm);
#endif
    }

    void MotorDriver::enable() {
        if (enableLeft != NC) {
            digitalWrite(enableLeft, HIGH);
        }
        if (enableRight != NC) {
            digitalWrite(enableRight, HIGH);
        }
    }

    void MotorDriver::disable() {
        if (enableLeft != NC) {
            digitalWrite(enableLeft, LOW);
        }
        if (enableRight != NC) {
            digitalWrite(enableRight, LOW);
        }
    }

    void MotorDriver::stop() {
#if defined(ESP32)
        if (pwmLeft != NC) ledcWrite(pwmChannelLeft, 0);
        if (pwmRight != NC) ledcWrite(pwmChannelRight, 0);
#else
        if (pwmLeft != NC) analogWrite(pwmLeft, 0);
        if (pwmRight != NC) analogWrite(pwmRight, 0);
#endif
    }
}

namespace L298N {
    MotorDriver::MotorDriver(int IN1, int IN2)
            : MotorDriver(NC, IN1, IN2) {
    }

    MotorDriver::MotorDriver(int EN, int IN1, int IN2) {
        enablePin = EN;
        inputLeftPin = IN1;
        inputRightPin = IN2;

        if (enablePin != NC) pinMode(enablePin, OUTPUT);
        if (inputLeftPin != NC) pinMode(inputLeftPin, OUTPUT);
        if (inputRightPin != NC) pinMode(inputRightPin, OUTPUT);
    }

#if defined(ESP32)

    void MotorDriver::setup(int freq, int ch, int resolution) {
        if (enablePin == NC) return;
        pwmChannel = ch;
        pwmFreq = freq;
        pwmResolution = resolution;
        ledcSetup(pwmChannel, freq, resolution);
        ledcAttachPin(enablePin, pwmChannel);
    }

#endif

    void MotorDriver::turnLeft(int pwm) {
        if (inputLeftPin != NC) digitalWrite(inputLeftPin, HIGH);
        if (inputRightPin != NC) digitalWrite(inputRightPin, LOW);
        if (enablePin == NC) return;
#if defined(ESP32)
        ledcWrite(pwmChannel, pwm);
#else
        analogWrite(enablePin, pwm);
#endif
    }

    void MotorDriver::turnRight(int pwm) {
        if (inputLeftPin != NC) digitalWrite(inputLeftPin, LOW);
        if (inputRightPin != NC) digitalWrite(inputRightPin, HIGH);
        if (enablePin == NC) return;
#if defined(ESP32)
        ledcWrite(pwmChannel, pwm);
#else
        analogWrite(enablePin, pwm);
#endif
    }

    void MotorDriver::stop() {
        if (inputLeftPin != NC) digitalWrite(inputLeftPin, LOW);
        if (inputRightPin != NC) digitalWrite(inputRightPin, LOW);
        if (enablePin == NC) return;
#if defined(ESP32)
        ledcWrite(pwmChannel, 0);
#else
        analogWrite(enablePin, 0);
#endif
    }
}