#ifndef STATE_ACTION_MANAGER_H
#define STATE_ACTION_MANAGER_H

#include "Arduino.h"

class StateActionManager {
public:
    enum TriggerEvent {
        TRIG_RISING,
        TRIG_FALLING,
        TRIG_BOTH_WAYS,
        TRIG_VALUE_CHANGE
    };

    enum MonitorType {
        THRESHOLD,
        RANGE,
        CHANGE_VALUE,
        TIME_BASED,
        VALUE_POINTER
    };

    enum MonitorState {
        IDLE,
        TRIGGERED,
        EXECUTING,
        COMPLETED,
        PAUSED
    };

private:
    struct Monitor {
        String name;
        MonitorType type;
        TriggerEvent triggerEvent;

        float (*valueFunction)();
        float *valuePointer;
        void (*actionFunction)();
        void (*paramActionFunction)(void *);
        void *actionParam;

        float threshold;
        float lowerThreshold;
        float upperThreshold;
        float minChange;

        unsigned long interval;
        unsigned long triggerTime;
        unsigned long lastExecutionTime;
        unsigned long lastValueTime;

        int repeatCount;
        int executionCount;

        float lastValue;
        float currentValue;

        bool enabled;
        bool wasAboveThreshold;
        bool wasInRange;
        bool wasChanged;

        MonitorState state;
    };

    Monitor *monitors;
    int monitorCount;
    int capacity;

public:
    StateActionManager(int initialCapacity = 10);
    ~StateActionManager();

    int addThresholdMonitor(String name, float (*valueFunction)(), float threshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount = 1, unsigned long actionInterval = 0);
    int addThresholdMonitor(String name, float (*valueFunction)(), float threshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount = 1, unsigned long actionInterval = 0);
    int addThresholdMonitor(String name, float (*valueFunction)(), float threshold, TriggerEvent triggerEvent, int repeatCount = 1, unsigned long actionInterval = 0);
    int addThresholdMonitor(String name, float *valuePointer, float threshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount = 1, unsigned long actionInterval = 0);
    int addThresholdMonitor(String name, float *valuePointer, float threshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount = 1, unsigned long actionInterval = 0);
    int addThresholdMonitor(String name, float *valuePointer, float threshold, TriggerEvent triggerEvent, int repeatCount = 1, unsigned long actionInterval = 0);
    int addRangeMonitor(String name, float (*valueFunction)(), float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount = 1, unsigned long actionInterval = 0);
    int addRangeMonitor(String name, float (*valueFunction)(), float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount = 1, unsigned long actionInterval = 0);
    int addRangeMonitor(String name, float (*valueFunction)(), float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, int repeatCount = 1, unsigned long actionInterval = 0);
    int addRangeMonitor(String name, float *valuePointer, float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(), int repeatCount = 1, unsigned long actionInterval = 0);
    int addRangeMonitor(String name, float *valuePointer, float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, void (*actionFunction)(void *), void *param, int repeatCount = 1, unsigned long actionInterval = 0);
    int addRangeMonitor(String name, float *valuePointer, float lowerThreshold, float upperThreshold, TriggerEvent triggerEvent, int repeatCount = 1, unsigned long actionInterval = 0);
    int addChangeMonitor(String name, float (*valueFunction)(), float minChange, void (*actionFunction)(), int repeatCount = 1, unsigned long actionInterval = 0);
    int addChangeMonitor(String name, float *valuePointer, float minChange, void (*actionFunction)(), int repeatCount = 1, unsigned long actionInterval = 0);
    int addTimeBasedMonitor(String name, unsigned long interval, void (*actionFunction)(), int repeatCount = 0);

    void update();

    float getCurrentValue(int monitorId);
    bool isActionRunning(int monitorId);
    MonitorState getMonitorState(int monitorId);
    int getExecutionCount(int monitorId);
    unsigned long getLastTriggerTime(int monitorId);
    String getMonitorName(int monitorId);

    bool isConditionTriggered(int monitorId);
    bool checkAndTrigger(int monitorId, void (*callbackFunction)());

    void resetMonitor(int monitorId);
    void setEnabled(int monitorId, bool enabled);
    void setThreshold(int monitorId, float threshold);
    void setRange(int monitorId, float lowerThreshold, float upperThreshold);
    void setRepeatCount(int monitorId, int repeatCount);
    void setInterval(int monitorId, unsigned long interval);

    void pauseAction(int monitorId);
    void resumeAction(int monitorId);
    void stopAction(int monitorId);

    void printStatus(int monitorId);
    void printAllStatus();
};

#endif