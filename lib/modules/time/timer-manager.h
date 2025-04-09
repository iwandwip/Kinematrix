#pragma once

#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <Arduino.h>

enum TimerType {
    TIMER_ONESHOT,
    TIMER_INTERVAL
};

class TimerManager;

typedef void (*TimerCallback)(TimerManager *timer);

class TimerManager {
public:
    TimerManager();
    explicit TimerManager(unsigned long interval, TimerType type = TIMER_ONESHOT);

    void start();
    void stop();
    void pause();
    void resume();
    void reset();
    void restart();

    bool isExpired() const;
    bool isRunning() const;
    bool isPaused() const;

    void setDuration(unsigned long milliseconds);
    void setDurationSeconds(unsigned long seconds);
    void setDurationMinutes(unsigned long minutes);
    void setDurationHours(unsigned long hours);
    void setDurationDays(unsigned long days);

    void setType(TimerType type);
    TimerType getType() const;

    unsigned long getElapsedMillis() const;
    unsigned long getElapsedSeconds() const;
    unsigned long getElapsedMinutes() const;
    unsigned long getElapsedHours() const;
    unsigned long getElapsedDays() const;

    void getElapsedTime(unsigned long &days, unsigned long &hours,
                        unsigned long &minutes, unsigned long &seconds,
                        unsigned long &millis) const;
    String getElapsedTimeString() const;

    unsigned long getRemainingMillis() const;
    unsigned long getRemainingSeconds() const;
    unsigned long getRemainingMinutes() const;
    unsigned long getRemainingHours() const;
    unsigned long getRemainingDays() const;

    void getRemainingTime(unsigned long &days, unsigned long &hours,
                          unsigned long &minutes, unsigned long &seconds,
                          unsigned long &millis) const;
    String getRemainingTimeString() const;

    float getProgress() const;
    int getProgressPercent() const;

    unsigned long getDuration() const;
    unsigned long getDurationSeconds() const;
    unsigned long getDurationMinutes() const;
    unsigned long getDurationHours() const;
    unsigned long getDurationDays() const;

    void setCallback(TimerCallback callback);
    void clearCallback();
    bool hasCallback() const;
    void enableAutoExecuteCallback(bool enable);
    bool isAutoExecuteCallbackEnabled() const;
    void executeCallback();

    void update();

    static unsigned long millisToSeconds(unsigned long millis);
    static unsigned long millisToMinutes(unsigned long millis);
    static unsigned long millisToHours(unsigned long millis);
    static unsigned long millisToDays(unsigned long millis);
    static unsigned long secondsToMillis(unsigned long seconds);
    static unsigned long minutesToMillis(unsigned long minutes);
    static unsigned long hoursToMillis(unsigned long hours);
    static unsigned long daysToMillis(unsigned long days);
    static String formatTime(unsigned long days, unsigned long hours,
                             unsigned long minutes, unsigned long seconds,
                             unsigned long millis, bool showMillis = false);

private:
    unsigned long getElapsedTime() const;
    void handleExpiry();

    unsigned long _startTime;
    unsigned long _pausedTime;
    unsigned long _elapsedBeforePause;
    unsigned long _duration;
    TimerType _timerType;

    struct {
        uint8_t _isRunning: 1;
        uint8_t _isPaused: 1;
        uint8_t _hasExpired: 1;
        uint8_t _autoCallback: 1;
        uint8_t _reserved: 4;
    } _flags;

    TimerCallback _callback;
};

#endif
