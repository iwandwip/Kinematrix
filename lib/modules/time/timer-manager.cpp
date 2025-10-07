#include "timer-manager.h"

TimerManager::TimerManager() {
    _startTime = 0;
    _pausedTime = 0;
    _elapsedBeforePause = 0;
    _duration = 0;
    _timerType = TIMER_ONESHOT;
    memset(&_flags, 0, sizeof(_flags));
    _callback = nullptr;
}

TimerManager::TimerManager(unsigned long interval, TimerType type) {
    _startTime = 0;
    _pausedTime = 0;
    _elapsedBeforePause = 0;
    _duration = interval;
    _timerType = type;
    memset(&_flags, 0, sizeof(_flags));
    _callback = nullptr;
}

void TimerManager::start() {
    _startTime = millis();
    _elapsedBeforePause = 0;
    _flags._isRunning = 1;
    _flags._isPaused = 0;
    _flags._hasExpired = 0;
}

void TimerManager::stop() {
    _flags._isRunning = 0;
    _flags._isPaused = 0;
}

void TimerManager::pause() {
    if (_flags._isRunning && !_flags._isPaused) {
        _pausedTime = millis();
        _flags._isPaused = 1;
    }
}

void TimerManager::resume() {
    if (_flags._isRunning && _flags._isPaused) {
        unsigned long pauseDuration = millis() - _pausedTime;
        _startTime += pauseDuration;
        _flags._isPaused = 0;
    }
}

void TimerManager::reset() {
    _startTime = millis();
    _elapsedBeforePause = 0;
    _flags._hasExpired = 0;

    if (_flags._isPaused) {
        _pausedTime = _startTime;
    }
}

void TimerManager::restart() {
    stop();
    start();
}

bool TimerManager::isExpired() const {
    if (!_flags._isRunning) return _flags._hasExpired;
    if (_flags._isPaused) return false;

    if (getElapsedTime() >= _duration) {
        if (_timerType == TIMER_ONESHOT) {
            const_cast<TimerManager *>(this)->_flags._hasExpired = 1;
        }
        return true;
    }
    return false;
}

bool TimerManager::isRunning() const {
    return _flags._isRunning;
}

bool TimerManager::isPaused() const {
    return _flags._isPaused;
}

void TimerManager::setDuration(unsigned long milliseconds) {
    _duration = milliseconds;
}

void TimerManager::setDurationSeconds(unsigned long seconds) {
    setDuration(seconds * 1000UL);
}

void TimerManager::setDurationMinutes(unsigned long minutes) {
    setDuration(minutes * 60000UL);
}

void TimerManager::setDurationHours(unsigned long hours) {
    setDuration(hours * 3600000UL);
}

void TimerManager::setDurationDays(unsigned long days) {
    setDuration(days * 86400000UL);
}

void TimerManager::setType(TimerType type) {
    _timerType = type;
}

TimerType TimerManager::getType() const {
    return _timerType;
}

unsigned long TimerManager::getElapsedTime() const {
    if (!_flags._isRunning) return _elapsedBeforePause;

    if (_flags._isPaused) {
        return _elapsedBeforePause + (_pausedTime - _startTime);
    }

    unsigned long currentTime = millis();

    if (currentTime < _startTime) {
        return _elapsedBeforePause + ((0xFFFFFFFF - _startTime) + currentTime + 1);
    }

    return _elapsedBeforePause + (currentTime - _startTime);
}

unsigned long TimerManager::getElapsedMillis() const {
    return getElapsedTime();
}

unsigned long TimerManager::getElapsedSeconds() const {
    return getElapsedMillis() / 1000UL;
}

unsigned long TimerManager::getElapsedMinutes() const {
    return getElapsedMillis() / 60000UL;
}

unsigned long TimerManager::getElapsedHours() const {
    return getElapsedMillis() / 3600000UL;
}

unsigned long TimerManager::getElapsedDays() const {
    return getElapsedMillis() / 86400000UL;
}

void TimerManager::getElapsedTime(unsigned long &days, unsigned long &hours,
                                  unsigned long &minutes, unsigned long &seconds,
                                  unsigned long &millis) const {
    unsigned long total = getElapsedMillis();

    days = total / 86400000UL;
    total %= 86400000UL;

    hours = total / 3600000UL;
    total %= 3600000UL;

    minutes = total / 60000UL;
    total %= 60000UL;

    seconds = total / 1000UL;
    millis = total % 1000UL;
}

String TimerManager::getElapsedTimeString() const {
    unsigned long days, hours, minutes, seconds, millis;
    getElapsedTime(days, hours, minutes, seconds, millis);
    return formatTime(days, hours, minutes, seconds, millis, true);
}

unsigned long TimerManager::getRemainingMillis() const {
    if (!_flags._isRunning) return _duration;

    unsigned long elapsed = getElapsedTime();
    if (elapsed >= _duration) return 0;

    return _duration - elapsed;
}

unsigned long TimerManager::getRemainingSeconds() const {
    return getRemainingMillis() / 1000UL;
}

unsigned long TimerManager::getRemainingMinutes() const {
    return getRemainingMillis() / 60000UL;
}

unsigned long TimerManager::getRemainingHours() const {
    return getRemainingMillis() / 3600000UL;
}

unsigned long TimerManager::getRemainingDays() const {
    return getRemainingMillis() / 86400000UL;
}

void TimerManager::getRemainingTime(unsigned long &days, unsigned long &hours,
                                    unsigned long &minutes, unsigned long &seconds,
                                    unsigned long &millis) const {
    unsigned long total = getRemainingMillis();

    days = total / 86400000UL;
    total %= 86400000UL;

    hours = total / 3600000UL;
    total %= 3600000UL;

    minutes = total / 60000UL;
    total %= 60000UL;

    seconds = total / 1000UL;
    millis = total % 1000UL;
}

String TimerManager::getRemainingTimeString() const {
    unsigned long days, hours, minutes, seconds, millis;
    getRemainingTime(days, hours, minutes, seconds, millis);
    return formatTime(days, hours, minutes, seconds, millis, true);
}

float TimerManager::getProgress() const {
    if (_duration == 0) return 0.0f;

    unsigned long elapsed = getElapsedMillis();
    if (elapsed >= _duration) return 1.0f;

    return (float) elapsed / (float) _duration;
}

int TimerManager::getProgressPercent() const {
    return (int) (getProgress() * 100.0f);
}

unsigned long TimerManager::getDuration() const {
    return _duration;
}

unsigned long TimerManager::getDurationSeconds() const {
    return _duration / 1000UL;
}

unsigned long TimerManager::getDurationMinutes() const {
    return _duration / 60000UL;
}

unsigned long TimerManager::getDurationHours() const {
    return _duration / 3600000UL;
}

unsigned long TimerManager::getDurationDays() const {
    return _duration / 86400000UL;
}

void TimerManager::setCallback(TimerCallback callback) {
    _callback = callback;
}

void TimerManager::clearCallback() {
    _callback = nullptr;
}

bool TimerManager::hasCallback() const {
    return (_callback != nullptr);
}

void TimerManager::enableAutoExecuteCallback(bool enable) {
    _flags._autoCallback = enable ? 1 : 0;
}

bool TimerManager::isAutoExecuteCallbackEnabled() const {
    return (_flags._autoCallback != 0);
}

void TimerManager::executeCallback() {
    if (_callback != nullptr) {
        _callback(this);
    }
}

void TimerManager::update() {
    if (_flags._isRunning && !_flags._isPaused && _flags._autoCallback && _callback != nullptr) {
        if (isExpired()) {
            handleExpiry();
        }
    }
}

void TimerManager::handleExpiry() {
    if (_callback != nullptr) {
        _callback(this);
    }

    if (_timerType == TIMER_INTERVAL) {
        reset();
    } else {
        _flags._isRunning = 0;
    }
}

unsigned long TimerManager::millisToSeconds(unsigned long millis) {
    return millis / 1000UL;
}

unsigned long TimerManager::millisToMinutes(unsigned long millis) {
    return millis / 60000UL;
}

unsigned long TimerManager::millisToHours(unsigned long millis) {
    return millis / 3600000UL;
}

unsigned long TimerManager::millisToDays(unsigned long millis) {
    return millis / 86400000UL;
}

unsigned long TimerManager::secondsToMillis(unsigned long seconds) {
    return seconds * 1000UL;
}

unsigned long TimerManager::minutesToMillis(unsigned long minutes) {
    return minutes * 60000UL;
}

unsigned long TimerManager::hoursToMillis(unsigned long hours) {
    return hours * 3600000UL;
}

unsigned long TimerManager::daysToMillis(unsigned long days) {
    return days * 86400000UL;
}

String TimerManager::formatTime(unsigned long days, unsigned long hours,
                                unsigned long minutes, unsigned long seconds,
                                unsigned long millis, bool showMillis) {
    char buffer[24];

    if (days > 0) {
        if (showMillis) {
            snprintf(buffer, sizeof(buffer), "%lud %02luh %02lum %02lus %03lums",
                     days, hours, minutes, seconds, millis);
        } else {
            snprintf(buffer, sizeof(buffer), "%lud %02luh %02lum %02lus",
                     days, hours, minutes, seconds);
        }
    } else if (hours > 0) {
        if (showMillis) {
            snprintf(buffer, sizeof(buffer), "%luh %02lum %02lus %03lums",
                     hours, minutes, seconds, millis);
        } else {
            snprintf(buffer, sizeof(buffer), "%luh %02lum %02lus",
                     hours, minutes, seconds);
        }
    } else if (minutes > 0) {
        if (showMillis) {
            snprintf(buffer, sizeof(buffer), "%lum %02lus %03lums",
                     minutes, seconds, millis);
        } else {
            snprintf(buffer, sizeof(buffer), "%lum %02lus",
                     minutes, seconds);
        }
    } else {
        if (showMillis) {
            snprintf(buffer, sizeof(buffer), "%lus %03lums",
                     seconds, millis);
        } else {
            snprintf(buffer, sizeof(buffer), "%lus", seconds);
        }
    }

    return String(buffer);
}
