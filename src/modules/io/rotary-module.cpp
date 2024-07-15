/*  
 *  rotary-module.cpp
 *
 *  rotary module c
 *  Created on: 2023. 4. 3
 */

#ifdef USE_ROTARY_MODULE
#include "rotary-module.h"

//RotaryModule::RotaryModule(uint8_t pin_clock_, uint8_t pin_data_)
//        : encoder(nullptr),
//          _pin_clock(pin_clock_),
//          _pin_data(pin_data_) {
//}
//
//void RotaryModule::init(void (*pin_callback_)()) {
//    encoder = new RotaryEncoder(_pin_clock, _pin_data, RotaryEncoder::LatchMode::TWO03);
//    attachInterrupt(digitalPinToInterrupt(_pin_clock), pin_callback_, CHANGE);
//    attachInterrupt(digitalPinToInterrupt(_pin_data), pin_callback_, CHANGE);
//}
//
//void RotaryModule::readPosition() {
//    encoder->tick();
//}
//
//int RotaryModule::getPosition() {
//    return encoder->getPosition();
//}
//
//int RotaryModule::getDirection() {
//    direction = (int) (encoder->getDirection());
//    return direction;
//}
//
//bool RotaryModule::isMoveCW() const {
//    return direction == 1;
//}
//
//bool RotaryModule::isMoveCCW() const {
//    return direction == -1;
//}
//
//bool RotaryModule::isStop() const {
//    return direction == 0;
//}
#endif