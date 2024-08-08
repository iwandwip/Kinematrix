/*
 *  timer-duration.cpp
 *
 *  arduino timer duration c
 *  Created on: 2023. 4. 3
 */

#include "timer-duration.h"

#define TICKS 100

TimerDuration::TimerDuration() {
    _startMillis = 0;
    _elapsedMillis = 0;
    _interval = 0;
    _isRunning = false;
}

TimerDuration::TimerDuration(unsigned long interval) {
    _startMillis = 0;
    _elapsedMillis = 0;
    _interval = interval;
    _isRunning = false;
}

void TimerDuration::setDuration(unsigned long interval) {
    _interval = interval;
}

void TimerDuration::start() {
    _startMillis = millis();
    _elapsedMillis = 0;
    _isRunning = true;
}

void TimerDuration::stop() {
    if (_isRunning) {
        _elapsedMillis += millis() - _startMillis;
        _isRunning = false;
    }
}

void TimerDuration::resume() {
    if (!_isRunning) {
        _startMillis = millis();
        _isRunning = true;
    }
}

bool TimerDuration::isExpired() const {
    if (_isRunning) {
        return (millis() - _startMillis + _elapsedMillis >= _interval);
    } else {
        return (_elapsedMillis >= _interval);
    }
}

bool TimerDuration::isRunning() const {
    return _isRunning;
}

void TimerDuration::reset() {
    _startMillis = millis();
    _elapsedMillis = 0;
}

unsigned long TimerDuration::getHours() const {
    unsigned long elapsedTime = _isRunning ? millis() - _startMillis + _elapsedMillis : _elapsedMillis;
    return (elapsedTime / 1000) / 3600;
}

unsigned long TimerDuration::getMinutes() const {
    unsigned long elapsedTime = _isRunning ? millis() - _startMillis + _elapsedMillis : _elapsedMillis;
    return ((elapsedTime / 1000) / 60) % 60;
}

unsigned long TimerDuration::getSeconds() const {
    unsigned long elapsedTime = _isRunning ? millis() - _startMillis + _elapsedMillis : _elapsedMillis;
    return (elapsedTime / 1000) % 60;
}

unsigned long TimerDuration::getDuration() const {
    return _interval;
}

void TimerDuration::getRemainingTime(unsigned long &hours, unsigned long &minutes, unsigned long &seconds) const {
    unsigned long elapsedTime = _isRunning ? millis() - _startMillis + _elapsedMillis : _elapsedMillis;
    unsigned long remainingTime = (_interval > elapsedTime) ? (_interval - elapsedTime) : 0;

    hours = (remainingTime / 1000) / 3600;
    minutes = ((remainingTime / 1000) / 60) % 60;
    seconds = (remainingTime / 1000) % 60;
}
