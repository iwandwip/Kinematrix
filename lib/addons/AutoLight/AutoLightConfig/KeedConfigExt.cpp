/*
 *  KeedConfigExt.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#include "KeedConfig.h"

configuration_t::configuration_t()
        : sequence(0), delay_time(40), version(0), channel(0), io_size(0), pin_size(0), pin_ptr(nullptr), i2c_ptr(nullptr), pin_sequence(nullptr), custom(false), custom_seq(false), reverse(false), display(false) {
}

void configuration_t::setPins(int _pin_size, ...) {
    va_list args;
    va_start(args, _pin_size);
    pin_size = _pin_size;
    pin_ptr = new uint8_t[_pin_size];
    for (int i = 0; i < _pin_size; i++) {
        pin_ptr[i] = static_cast<uint8_t>(va_arg(args, int));
    }
    va_end(args);
}

void configuration_t::setAddress(int _io_size, ...) {
    va_list args;
    va_start(args, _io_size);
    i2c_ptr = new uint8_t[_io_size];
    for (int i = 0; i < _io_size; i++) {
        i2c_ptr[i] = static_cast<uint8_t>(va_arg(args, int));
    }
    va_end(args);
}

void configuration_t::setDelay(uint32_t _time) {
    delay_time = _time;
    writeMEM(MODE_ADDRESS, String(sequence));
    writeMEM(DELAY_ADDRESS, String(delay_time));
}

void configuration_t::setPinSequence(int _pin_sequence_size, ...) {
    va_list args;
    va_start(args, _pin_sequence_size);
    custom_seq = true;
    pin_sequence = new uint8_t[_pin_sequence_size];
    for (int i = 0; i < _pin_sequence_size; i++) {
        pin_sequence[i] = static_cast<uint8_t>(va_arg(args, int));
    }
    va_end(args);
}

indicator_t::indicator_t() :
        outs{DigitalOut(LED_RED), DigitalOut(LED_GREEN), DigitalOut(LED_BLUE), DigitalOut(BUZZER)},
        times{TimerTask(500), TimerTask(500), TimerTask(100), TimerTask(100)} {
    outs[3].toggleInit(90, 5);
}

void indicator_t::show(uint8_t _seq) {
    uint8_t state = (_seq == 0) ? 1 : 0;
    static uint8_t _seq_before = 0;
    if (_seq != _seq_before) {
        outs[2].on();
        outs[2].offDelay(600);
        outs[3].on();
        outs[3].offDelay(50);
        _seq_before = _seq;
    }
    if (state) {
        outs[0].on();
        outs[1].off();
    } else {
        outs[1].on();
        outs[0].off();
    }
    //    if (times[!state].triggered()) {
    //        outs[!state].on();
    //        outs[!state].offDelay(100);
    //        outs[state].off();
    //        times[state].reset();
    //    }
    for (auto &out: outs) {
        out.update();
    }
}

#if defined(ESP32)
#else
void writeMEM(int addrOffset, const String &strToWrite) {
    byte len = strToWrite.length();
    EEPROM.write(addrOffset, len);
    for (int i = 0; i < len; i++) {
        EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
    }
}

String readMEM(int addrOffset) {
    int newStrLen = EEPROM.read(addrOffset);
    char data[newStrLen + 1];
    for (int i = 0; i < newStrLen; i++) {
        data[i] = EEPROM.read(addrOffset + 1 + i);
    }
    data[newStrLen] = '\0';
    return String(data);
}

String memstr(const byte* byteArray, size_t size) {
  String result = "";
  for (size_t i = 0; i < size; i++) {
    result += String(byteArray[i], HEX);
  }
  return result;
}
#endif
