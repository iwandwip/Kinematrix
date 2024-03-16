/*
 *  timer-duration.h
 *
 *  arduino timer duration lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KADITA_TIMER_DURATION_H
#define KADITA_TIMER_DURATION_H

#include "Arduino.h"

class TimerDuration {
public:
    TimerDuration();
    explicit TimerDuration(unsigned long interval);
    void setDuration(unsigned long interval);
    void start();
    void stop();
    void resume();
    bool isExpired() const;
    void reset();
    unsigned long getHours() const;
    unsigned long getMinutes() const;
    unsigned long getSeconds() const;
    unsigned long getDuration() const;

private:
    unsigned long _startMillis;
    unsigned long _interval;  // in milliseconds
    bool _isRunning;
};

#endif // KADITA_TIMER_DURATION_H