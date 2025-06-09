#include "StateActionManager.h"

StateActionManager::StateActionManager(int initialCapacity) {
    capacity = initialCapacity;
    monitors = new Monitor[capacity];
    monitorCount = 0;
}

StateActionManager::~StateActionManager() {
    delete[] monitors;
}

int StateActionManager::addThresholdMonitor(String name, float (*valueFunction)(), float threshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = THRESHOLD;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = valueFunction;
    monitors[id].valuePointer = NULL;
    monitors[id].actionFunction = actionFunction;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].threshold = threshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = valueFunction();
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasAboveThreshold = (monitors[id].lastValue > threshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addThresholdMonitor(String name, float (*valueFunction)(), float threshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = THRESHOLD;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = valueFunction;
    monitors[id].valuePointer = NULL;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = actionFunction;
    monitors[id].actionParam = param;
    monitors[id].threshold = threshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = valueFunction();
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasAboveThreshold = (monitors[id].lastValue > threshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addThresholdMonitor(String name, float (*valueFunction)(), float threshold, TriggerEvent triggerEvent, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = THRESHOLD;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = valueFunction;
    monitors[id].valuePointer = NULL;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].threshold = threshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = valueFunction();
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasAboveThreshold = (monitors[id].lastValue > threshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addThresholdMonitor(String name, float *valuePointer, float threshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = VALUE_POINTER;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = NULL;
    monitors[id].valuePointer = valuePointer;
    monitors[id].actionFunction = actionFunction;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].threshold = threshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = *valuePointer;
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasAboveThreshold = (monitors[id].lastValue > threshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addThresholdMonitor(String name, float *valuePointer, float threshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = VALUE_POINTER;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = NULL;
    monitors[id].valuePointer = valuePointer;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = actionFunction;
    monitors[id].actionParam = param;
    monitors[id].threshold = threshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = *valuePointer;
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasAboveThreshold = (monitors[id].lastValue > threshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addThresholdMonitor(String name, float *valuePointer, float threshold, TriggerEvent triggerEvent, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = VALUE_POINTER;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = NULL;
    monitors[id].valuePointer = valuePointer;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].threshold = threshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = *valuePointer;
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasAboveThreshold = (monitors[id].lastValue > threshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addRangeMonitor(String name, float (*valueFunction)(), float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = RANGE;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = valueFunction;
    monitors[id].valuePointer = NULL;
    monitors[id].actionFunction = actionFunction;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].lowerThreshold = lowerThreshold;
    monitors[id].upperThreshold = upperThreshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = valueFunction();
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasInRange = (monitors[id].lastValue >= lowerThreshold &&
                               monitors[id].lastValue <= upperThreshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addRangeMonitor(String name, float (*valueFunction)(), float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = RANGE;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = valueFunction;
    monitors[id].valuePointer = NULL;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = actionFunction;
    monitors[id].actionParam = param;
    monitors[id].lowerThreshold = lowerThreshold;
    monitors[id].upperThreshold = upperThreshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = valueFunction();
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasInRange = (monitors[id].lastValue >= lowerThreshold &&
                               monitors[id].lastValue <= upperThreshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addRangeMonitor(String name, float (*valueFunction)(), float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = RANGE;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = valueFunction;
    monitors[id].valuePointer = NULL;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].lowerThreshold = lowerThreshold;
    monitors[id].upperThreshold = upperThreshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = valueFunction();
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasInRange = (monitors[id].lastValue >= lowerThreshold &&
                               monitors[id].lastValue <= upperThreshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addRangeMonitor(String name, float *valuePointer, float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = VALUE_POINTER;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = NULL;
    monitors[id].valuePointer = valuePointer;
    monitors[id].actionFunction = actionFunction;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].lowerThreshold = lowerThreshold;
    monitors[id].upperThreshold = upperThreshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = *valuePointer;
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasInRange = (monitors[id].lastValue >= lowerThreshold &&
                               monitors[id].lastValue <= upperThreshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addRangeMonitor(String name, float *valuePointer, float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = VALUE_POINTER;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = NULL;
    monitors[id].valuePointer = valuePointer;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = actionFunction;
    monitors[id].actionParam = param;
    monitors[id].lowerThreshold = lowerThreshold;
    monitors[id].upperThreshold = upperThreshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = *valuePointer;
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasInRange = (monitors[id].lastValue >= lowerThreshold &&
                               monitors[id].lastValue <= upperThreshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addRangeMonitor(String name, float *valuePointer, float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = VALUE_POINTER;
    monitors[id].triggerEvent = triggerEvent;
    monitors[id].valueFunction = NULL;
    monitors[id].valuePointer = valuePointer;
    monitors[id].actionFunction = NULL;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].lowerThreshold = lowerThreshold;
    monitors[id].upperThreshold = upperThreshold;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = *valuePointer;
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasInRange = (monitors[id].lastValue >= lowerThreshold &&
                               monitors[id].lastValue <= upperThreshold);
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addChangeMonitor(String name, float (*valueFunction)(), float minChange, void (*actionFunction)(), int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = CHANGE_VALUE;
    monitors[id].valueFunction = valueFunction;
    monitors[id].valuePointer = NULL;
    monitors[id].actionFunction = actionFunction;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].minChange = minChange;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = valueFunction();
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasChanged = false;
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addChangeMonitor(String name, float *valuePointer, float minChange, void (*actionFunction)(), int repeatCount, unsigned long actionInterval) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = VALUE_POINTER;
    monitors[id].valueFunction = NULL;
    monitors[id].valuePointer = valuePointer;
    monitors[id].actionFunction = actionFunction;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].minChange = minChange;
    monitors[id].repeatCount = repeatCount;
    monitors[id].interval = actionInterval;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = 0;
    monitors[id].triggerTime = 0;
    monitors[id].lastValueTime = 0;
    monitors[id].lastValue = *valuePointer;
    monitors[id].currentValue = monitors[id].lastValue;
    monitors[id].enabled = true;
    monitors[id].wasChanged = false;
    monitors[id].state = IDLE;

    return id;
}

int StateActionManager::addTimeBasedMonitor(String name, unsigned long interval, void (*actionFunction)(), int repeatCount) {
    if (monitorCount >= capacity) {
        capacity *= 2;
        Monitor *newMonitors = new Monitor[capacity];
        memcpy(newMonitors, monitors, monitorCount * sizeof(Monitor));
        delete[] monitors;
        monitors = newMonitors;
    }

    int id = monitorCount++;
    monitors[id].name = name;
    monitors[id].type = TIME_BASED;
    monitors[id].actionFunction = actionFunction;
    monitors[id].paramActionFunction = NULL;
    monitors[id].actionParam = NULL;
    monitors[id].interval = interval;
    monitors[id].repeatCount = repeatCount;
    monitors[id].executionCount = 0;
    monitors[id].lastExecutionTime = millis();
    monitors[id].triggerTime = 0;
    monitors[id].enabled = true;
    monitors[id].state = IDLE;

    return id;
}

void StateActionManager::update() {
    unsigned long currentTime = millis();

    for (int i = 0; i < monitorCount; i++) {
        if (!monitors[i].enabled || monitors[i].state == PAUSED) {
            continue;
        }

        if (monitors[i].state == EXECUTING) {
            bool actionDue = (currentTime - monitors[i].lastExecutionTime >= monitors[i].interval);

            if (actionDue && monitors[i].executionCount < monitors[i].repeatCount) {
                if (monitors[i].actionFunction) {
                    monitors[i].actionFunction();
                } else if (monitors[i].paramActionFunction) {
                    monitors[i].paramActionFunction(monitors[i].actionParam);
                }

                monitors[i].executionCount++;
                monitors[i].lastExecutionTime = currentTime;

                if (monitors[i].executionCount >= monitors[i].repeatCount && monitors[i].repeatCount > 0) {
                    monitors[i].state = COMPLETED;
                }
            }
            continue;
        }

        if (monitors[i].type == TIME_BASED) {
            if (currentTime - monitors[i].lastExecutionTime >= monitors[i].interval) {
                monitors[i].triggerTime = currentTime;
                monitors[i].state = EXECUTING;
                monitors[i].executionCount = 0;
                monitors[i].lastExecutionTime = currentTime;

                if (monitors[i].actionFunction) {
                    monitors[i].actionFunction();
                } else if (monitors[i].paramActionFunction) {
                    monitors[i].paramActionFunction(monitors[i].actionParam);
                }

                monitors[i].executionCount++;

                if (monitors[i].repeatCount == 1) {
                    monitors[i].state = COMPLETED;
                }
            }
            continue;
        }

        if (monitors[i].valueFunction) {
            monitors[i].currentValue = monitors[i].valueFunction();
        } else if (monitors[i].valuePointer) {
            monitors[i].currentValue = *monitors[i].valuePointer;
        }

        bool triggered = false;

        if (monitors[i].type == THRESHOLD || (monitors[i].type == VALUE_POINTER && monitors[i].threshold > 0)) {
            bool isAboveThreshold = (monitors[i].currentValue > monitors[i].threshold);

            switch (monitors[i].triggerEvent) {
                case TRIG_RISING:
                    triggered = (isAboveThreshold && !monitors[i].wasAboveThreshold);
                    break;
                case TRIG_FALLING:
                    triggered = (!isAboveThreshold && monitors[i].wasAboveThreshold);
                    break;
                case TRIG_BOTH_WAYS:
                    triggered = (isAboveThreshold != monitors[i].wasAboveThreshold);
                    break;
                case TRIG_VALUE_CHANGE:
                    break;
            }

            monitors[i].wasAboveThreshold = isAboveThreshold;
        } else if (monitors[i].type == RANGE || (monitors[i].type == VALUE_POINTER &&
                                                 monitors[i].lowerThreshold < monitors[i].upperThreshold)) {
            bool isInRange = (monitors[i].currentValue >= monitors[i].lowerThreshold &&
                              monitors[i].currentValue <= monitors[i].upperThreshold);

            switch (monitors[i].triggerEvent) {
                case TRIG_RISING:
                    triggered = (isInRange && !monitors[i].wasInRange);
                    break;
                case TRIG_FALLING:
                    triggered = (!isInRange && monitors[i].wasInRange);
                    break;
                case TRIG_BOTH_WAYS:
                    triggered = (isInRange != monitors[i].wasInRange);
                    break;
                case TRIG_VALUE_CHANGE:
                    break;
            }

            monitors[i].wasInRange = isInRange;
        } else if (monitors[i].type == CHANGE_VALUE || (monitors[i].type == VALUE_POINTER && monitors[i].minChange > 0)) {
            float delta = abs(monitors[i].currentValue - monitors[i].lastValue);
            triggered = (delta >= monitors[i].minChange);
            monitors[i].lastValue = monitors[i].currentValue;
        }

        if (triggered && (monitors[i].state == IDLE || monitors[i].state == COMPLETED)) {
            monitors[i].triggerTime = currentTime;
            monitors[i].state = EXECUTING;
            monitors[i].executionCount = 0;
            monitors[i].lastExecutionTime = currentTime;

            if (monitors[i].actionFunction) {
                monitors[i].actionFunction();
            } else if (monitors[i].paramActionFunction) {
                monitors[i].paramActionFunction(monitors[i].actionParam);
            }

            monitors[i].executionCount++;

            if (monitors[i].repeatCount == 1) {
                monitors[i].state = COMPLETED;
            }
        }

        monitors[i].lastValueTime = currentTime;
    }
}

float StateActionManager::getCurrentValue(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        if (monitors[monitorId].valueFunction) {
            return monitors[monitorId].currentValue;
        } else if (monitors[monitorId].valuePointer) {
            return *monitors[monitorId].valuePointer;
        }
    }
    return 0;
}

bool StateActionManager::isActionRunning(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        return monitors[monitorId].state == EXECUTING;
    }
    return false;
}

StateActionManager::MonitorState StateActionManager::getMonitorState(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        return monitors[monitorId].state;
    }
    return IDLE;
}

int StateActionManager::getExecutionCount(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        return monitors[monitorId].executionCount;
    }
    return 0;
}

unsigned long StateActionManager::getLastTriggerTime(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        return monitors[monitorId].triggerTime;
    }
    return 0;
}

String StateActionManager::getMonitorName(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        return monitors[monitorId].name;
    }
    return "";
}

bool StateActionManager::isConditionTriggered(int monitorId) {
    if (monitorId < 0 || monitorId >= monitorCount || !monitors[monitorId].enabled) {
        return false;
    }

    if (monitors[monitorId].valueFunction) {
        monitors[monitorId].currentValue = monitors[monitorId].valueFunction();
    } else if (monitors[monitorId].valuePointer) {
        monitors[monitorId].currentValue = *monitors[monitorId].valuePointer;
    }

    bool triggered = false;

    if (monitors[monitorId].type == THRESHOLD ||
        (monitors[monitorId].type == VALUE_POINTER && monitors[monitorId].threshold > 0)) {
        bool isAboveThreshold = (monitors[monitorId].currentValue > monitors[monitorId].threshold);

        switch (monitors[monitorId].triggerEvent) {
            case TRIG_RISING:
                triggered = (isAboveThreshold && !monitors[monitorId].wasAboveThreshold);
                break;
            case TRIG_FALLING:
                triggered = (!isAboveThreshold && monitors[monitorId].wasAboveThreshold);
                break;
            case TRIG_BOTH_WAYS:
                triggered = (isAboveThreshold != monitors[monitorId].wasAboveThreshold);
                break;
            case TRIG_VALUE_CHANGE:
                break;
        }

        monitors[monitorId].wasAboveThreshold = isAboveThreshold;
    } else if (monitors[monitorId].type == RANGE ||
               (monitors[monitorId].type == VALUE_POINTER &&
                monitors[monitorId].lowerThreshold < monitors[monitorId].upperThreshold)) {
        bool isInRange = (monitors[monitorId].currentValue >= monitors[monitorId].lowerThreshold &&
                          monitors[monitorId].currentValue <= monitors[monitorId].upperThreshold);

        switch (monitors[monitorId].triggerEvent) {
            case TRIG_RISING:
                triggered = (isInRange && !monitors[monitorId].wasInRange);
                break;
            case TRIG_FALLING:
                triggered = (!isInRange && monitors[monitorId].wasInRange);
                break;
            case TRIG_BOTH_WAYS:
                triggered = (isInRange != monitors[monitorId].wasInRange);
                break;
            case TRIG_VALUE_CHANGE:
                break;
        }

        monitors[monitorId].wasInRange = isInRange;
    } else if (monitors[monitorId].type == CHANGE_VALUE ||
               (monitors[monitorId].type == VALUE_POINTER && monitors[monitorId].minChange > 0)) {
        float delta = abs(monitors[monitorId].currentValue - monitors[monitorId].lastValue);
        triggered = (delta >= monitors[monitorId].minChange);
    } else if (monitors[monitorId].type == TIME_BASED) {
        unsigned long currentTime = millis();
        triggered = (currentTime - monitors[monitorId].lastExecutionTime >= monitors[monitorId].interval);
    }

    return triggered;
}

bool StateActionManager::checkAndTrigger(int monitorId, void (*callbackFunction)()) {
    if (monitorId < 0 || monitorId >= monitorCount || !monitors[monitorId].enabled) {
        return false;
    }

    bool triggered = isConditionTriggered(monitorId);

    if (triggered) {
        unsigned long currentTime = millis();

        if (monitors[monitorId].state == EXECUTING &&
            currentTime - monitors[monitorId].lastExecutionTime < monitors[monitorId].interval) {
            return false;
        }

        if (monitors[monitorId].repeatCount > 0 &&
            monitors[monitorId].executionCount >= monitors[monitorId].repeatCount) {
            return false;
        }

        if (callbackFunction != NULL) {
            callbackFunction();
        }

        if (monitors[monitorId].state != EXECUTING) {
            monitors[monitorId].state = EXECUTING;
            monitors[monitorId].triggerTime = currentTime;
        }

        monitors[monitorId].executionCount++;
        monitors[monitorId].lastExecutionTime = currentTime;

        if (monitors[monitorId].repeatCount > 0 &&
            monitors[monitorId].executionCount >= monitors[monitorId].repeatCount) {
            monitors[monitorId].state = COMPLETED;
        }

        return true;
    }

    return false;
}

void StateActionManager::resetMonitor(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        monitors[monitorId].state = IDLE;
        monitors[monitorId].executionCount = 0;

        if (monitors[monitorId].valueFunction) {
            monitors[monitorId].currentValue = monitors[monitorId].valueFunction();
            monitors[monitorId].lastValue = monitors[monitorId].currentValue;
        } else if (monitors[monitorId].valuePointer) {
            monitors[monitorId].currentValue = *monitors[monitorId].valuePointer;
            monitors[monitorId].lastValue = monitors[monitorId].currentValue;
        }

        if (monitors[monitorId].type == THRESHOLD ||
            (monitors[monitorId].type == VALUE_POINTER && monitors[monitorId].threshold > 0)) {
            monitors[monitorId].wasAboveThreshold =
                    (monitors[monitorId].currentValue > monitors[monitorId].threshold);
        } else if (monitors[monitorId].type == RANGE ||
                   (monitors[monitorId].type == VALUE_POINTER &&
                    monitors[monitorId].lowerThreshold < monitors[monitorId].upperThreshold)) {
            monitors[monitorId].wasInRange =
                    (monitors[monitorId].currentValue >= monitors[monitorId].lowerThreshold &&
                     monitors[monitorId].currentValue <= monitors[monitorId].upperThreshold);
        }
    }
}

void StateActionManager::setEnabled(int monitorId, bool enabled) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        monitors[monitorId].enabled = enabled;

        if (enabled) {
            if (monitors[monitorId].valueFunction) {
                monitors[monitorId].currentValue = monitors[monitorId].valueFunction();
                monitors[monitorId].lastValue = monitors[monitorId].currentValue;
            } else if (monitors[monitorId].valuePointer) {
                monitors[monitorId].currentValue = *monitors[monitorId].valuePointer;
                monitors[monitorId].lastValue = monitors[monitorId].currentValue;
            }

            if (monitors[monitorId].type == THRESHOLD ||
                (monitors[monitorId].type == VALUE_POINTER && monitors[monitorId].threshold > 0)) {
                monitors[monitorId].wasAboveThreshold =
                        (monitors[monitorId].currentValue > monitors[monitorId].threshold);
            } else if (monitors[monitorId].type == RANGE ||
                       (monitors[monitorId].type == VALUE_POINTER &&
                        monitors[monitorId].lowerThreshold < monitors[monitorId].upperThreshold)) {
                monitors[monitorId].wasInRange =
                        (monitors[monitorId].currentValue >= monitors[monitorId].lowerThreshold &&
                         monitors[monitorId].currentValue <= monitors[monitorId].upperThreshold);
            }
        }
    }
}

void StateActionManager::setThreshold(int monitorId, float threshold) {
    if (monitorId >= 0 && monitorId < monitorCount &&
        (monitors[monitorId].type == THRESHOLD || monitors[monitorId].type == VALUE_POINTER)) {
        monitors[monitorId].threshold = threshold;

        monitors[monitorId].wasAboveThreshold =
                (monitors[monitorId].currentValue > threshold);
    }
}

void StateActionManager::setRange(int monitorId, float lowerThreshold, float upperThreshold) {
    if (monitorId >= 0 && monitorId < monitorCount &&
        (monitors[monitorId].type == RANGE || monitors[monitorId].type == VALUE_POINTER)) {
        monitors[monitorId].lowerThreshold = lowerThreshold;
        monitors[monitorId].upperThreshold = upperThreshold;

        monitors[monitorId].wasInRange =
                (monitors[monitorId].currentValue >= lowerThreshold &&
                 monitors[monitorId].currentValue <= upperThreshold);
    }
}

void StateActionManager::setRepeatCount(int monitorId, int repeatCount) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        monitors[monitorId].repeatCount = repeatCount;
    }
}

void StateActionManager::setInterval(int monitorId, unsigned long interval) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        monitors[monitorId].interval = interval;
    }
}

void StateActionManager::pauseAction(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount && monitors[monitorId].state == EXECUTING) {
        monitors[monitorId].state = PAUSED;
    }
}

void StateActionManager::resumeAction(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount && monitors[monitorId].state == PAUSED) {
        monitors[monitorId].state = EXECUTING;
        monitors[monitorId].lastExecutionTime = millis();
    }
}

void StateActionManager::stopAction(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount &&
        (monitors[monitorId].state == EXECUTING || monitors[monitorId].state == PAUSED)) {
        monitors[monitorId].state = COMPLETED;
    }
}

void StateActionManager::printStatus(int monitorId) {
    if (monitorId >= 0 && monitorId < monitorCount) {
        Serial.print("Monitor '");
        Serial.print(monitors[monitorId].name);
        Serial.print("': State=");

        switch (monitors[monitorId].state) {
            case IDLE:
                Serial.print("IDLE");
                break;
            case TRIGGERED:
                Serial.print("TRIGGERED");
                break;
            case EXECUTING:
                Serial.print("EXECUTING");
                break;
            case COMPLETED:
                Serial.print("COMPLETED");
                break;
            case PAUSED:
                Serial.print("PAUSED");
                break;
        }

        Serial.print(", Value=");
        Serial.print(monitors[monitorId].currentValue);

        Serial.print(", Executions=");
        Serial.print(monitors[monitorId].executionCount);

        if (monitors[monitorId].repeatCount > 0) {
            Serial.print("/");
            Serial.print(monitors[monitorId].repeatCount);
        }

        Serial.println();
    }
}

void StateActionManager::printAllStatus() {
    Serial.println("---- StateActionManager Status ----");

    for (int i = 0; i < monitorCount; i++) {
        Serial.print("[");
        Serial.print(i);
        Serial.print("] ");
        printStatus(i);
    }

    Serial.println("----------------------------------");
}