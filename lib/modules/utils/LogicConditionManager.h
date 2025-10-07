#ifndef LOGIC_CONDITION_MANAGER_H
#define LOGIC_CONDITION_MANAGER_H

#include "Arduino.h"

class LogicConditionManager {
public:
    enum TriggerType {
        WHEN_TRUE,
        WHEN_FALSE,
        ON_CHANGE,
        TO_TRUE,
        TO_FALSE
    };

    enum LogicOp {
        AND,
        OR,
        NOT,
        XOR,
        NAND,
        NOR
    };

    enum ConditionType {
        BOOLEAN_FUNCTION,
        BOOLEAN_POINTER,
        VALUE_FUNCTION,
        VALUE_POINTER,
        COMPOSITE,
        TIMER,
        SEQUENCE,
        EDGE_COUNTER
    };

    enum ConditionState {
        IDLE,
        TRIGGERED,
        EXECUTING,
        COMPLETED,
        PAUSED,
        ACTION_RUNNING,
        ACTION_WAITING
    };

    enum ComparisonType {
        GREATER_THAN,
        LESS_THAN,
        GREATER_EQUAL,
        LESS_EQUAL,
        EQUAL,
        NOT_EQUAL
    };

private:
    struct Condition {
        String name;
        ConditionType type;
        TriggerType triggerType;

        bool (*boolFunction)();
        bool *boolPointer;
        float (*valueFunction)();
        float *valuePointer;
        float threshold;
        ComparisonType comparison;

        void (*actionFunction)();
        void (*paramActionFunction)(void *);
        void *actionParam;
        void (*stopActionFunction)();

        LogicOp logicOp;
        int *childConditions;
        int childCount;

        unsigned long interval;
        unsigned long debounceTime;
        unsigned long lastDebounceTime;
        unsigned long lastTriggerTime;
        unsigned long lastExecutionTime;
        unsigned long duration;
        unsigned long startTime;

        unsigned long actionDuration;
        unsigned long actionStartTime;
        unsigned long waitInterval;
        unsigned long waitStartTime;

        int repeatCount;
        int executionCount;
        int minOccurrences;
        int occurrenceCount;

        bool enabled;
        bool lastBoolValue;
        bool currentBoolValue;
        bool wasTriggered;
        bool autoReset;
        bool actionActive;
        bool actionWaiting;

        float lastFloatValue;
        float currentFloatValue;
        bool hasValidFloatValue;

        ConditionState state;
    };

    Condition *conditions;
    int conditionCount;
    int capacity;

    String lastNameLookup;
    int lastIdFound;

    int findConditionIdByName(const String &name);
    void expandCapacity();
    bool evaluateCondition(int conditionId);
    bool evaluateOnChange(int conditionId);
    bool evaluateComposite(int conditionId);
    bool evaluateSequence(int conditionId);
    void updateConditionState(int conditionId, bool triggered);
    void handleActionDuration(int conditionId);
    bool evaluateValueCondition(float value, float threshold, ComparisonType comparison);

public:
    LogicConditionManager(int initialCapacity = 10);
    ~LogicConditionManager();

    int addCondition(const String &name, bool (*conditionFunction)(),
                     TriggerType triggerType, void (*actionFunction)() = NULL,
                     int repeatCount = 1, unsigned long interval = 0,
                     unsigned long debounceTime = 0);

    int addCondition(const String &name, bool (*conditionFunction)(),
                     TriggerType triggerType, void (*actionFunction)(),
                     int repeatCount, unsigned long interval,
                     unsigned long debounceTime, unsigned long actionDuration,
                     void (*stopActionFunction)() = NULL, unsigned long waitInterval = 0);

    int addCondition(const String &name, bool *conditionPointer,
                     TriggerType triggerType, void (*actionFunction)() = NULL,
                     int repeatCount = 1, unsigned long interval = 0,
                     unsigned long debounceTime = 0);

    int addCondition(const String &name, bool *conditionPointer,
                     TriggerType triggerType, void (*actionFunction)(),
                     int repeatCount, unsigned long interval,
                     unsigned long debounceTime, unsigned long actionDuration,
                     void (*stopActionFunction)() = NULL, unsigned long waitInterval = 0);

    int addCondition(const String &name, bool (*conditionFunction)(),
                     TriggerType triggerType, void (*actionFunction)(void *), void *param,
                     int repeatCount = 1, unsigned long interval = 0,
                     unsigned long debounceTime = 0);

    int addCondition(const String &name, bool (*conditionFunction)(),
                     TriggerType triggerType, void (*actionFunction)(void *), void *param,
                     int repeatCount, unsigned long interval,
                     unsigned long debounceTime, unsigned long actionDuration,
                     void (*stopActionFunction)() = NULL, unsigned long waitInterval = 0);

    int addCondition(const String &name, bool *conditionPointer,
                     TriggerType triggerType, void (*actionFunction)(void *), void *param,
                     int repeatCount = 1, unsigned long interval = 0,
                     unsigned long debounceTime = 0);

    int addCondition(const String &name, bool *conditionPointer,
                     TriggerType triggerType, void (*actionFunction)(void *), void *param,
                     int repeatCount, unsigned long interval,
                     unsigned long debounceTime, unsigned long actionDuration,
                     void (*stopActionFunction)() = NULL, unsigned long waitInterval = 0);

    int addValueCondition(const String &name, float (*valueFunction)(),
                          float threshold, ComparisonType comparison, TriggerType triggerType,
                          void (*actionFunction)() = NULL, int repeatCount = 1,
                          unsigned long interval = 0);

    int addValueCondition(const String &name, float (*valueFunction)(),
                          float threshold, ComparisonType comparison, TriggerType triggerType,
                          void (*actionFunction)(), int repeatCount,
                          unsigned long interval, unsigned long actionDuration,
                          void (*stopActionFunction)() = NULL, unsigned long waitInterval = 0);

    int addValueCondition(const String &name, float *valuePointer,
                          float threshold, ComparisonType comparison, TriggerType triggerType,
                          void (*actionFunction)() = NULL, int repeatCount = 1,
                          unsigned long interval = 0);

    int addValueCondition(const String &name, float *valuePointer,
                          float threshold, ComparisonType comparison, TriggerType triggerType,
                          void (*actionFunction)(), int repeatCount,
                          unsigned long interval, unsigned long actionDuration,
                          void (*stopActionFunction)() = NULL, unsigned long waitInterval = 0);

    int addCompositeCondition(const String &name, LogicOp logicOperator,
                              int conditionIds[], int count,
                              void (*actionFunction)() = NULL, int repeatCount = 1,
                              unsigned long interval = 0);

    int addCompositeCondition(const String &name, LogicOp logicOperator,
                              int conditionIds[], int count,
                              void (*actionFunction)(), int repeatCount,
                              unsigned long interval, unsigned long actionDuration,
                              void (*stopActionFunction)() = NULL, unsigned long waitInterval = 0);

    int addAndCondition(const String &name, int conditionIds[], int count,
                        void (*actionFunction)() = NULL, int repeatCount = 1,
                        unsigned long interval = 0);

    int addOrCondition(const String &name, int conditionIds[], int count,
                       void (*actionFunction)() = NULL, int repeatCount = 1,
                       unsigned long interval = 0);

    int addNotCondition(const String &name, int conditionId,
                        void (*actionFunction)() = NULL, int repeatCount = 1,
                        unsigned long interval = 0);

    int addTimerCondition(const String &name, unsigned long duration,
                          bool autoReset = false, void (*actionFunction)() = NULL,
                          int repeatCount = 1, unsigned long interval = 0);

    int addSequenceCondition(const String &name, int conditionIds[], int count,
                             unsigned long maxTimespan = 0, int minOccurrences = 1,
                             void (*actionFunction)() = NULL, int repeatCount = 1);

    int addEdgeCountCondition(const String &name, int conditionId,
                              int targetCount, unsigned long timeWindow,
                              void (*actionFunction)() = NULL, int repeatCount = 1);

    void update();

    bool isConditionMet(int conditionId);
    bool isConditionMet(const String &conditionName);

    bool checkAndTrigger(int conditionId, void (*callbackFunction)());
    bool checkAndTrigger(const String &conditionName, void (*callbackFunction)());

    ConditionState getConditionState(int conditionId);
    ConditionState getConditionState(const String &conditionName);

    bool getCurrentBooleanValue(int conditionId);
    bool getCurrentBooleanValue(const String &conditionName);

    int getExecutionCount(int conditionId);
    int getExecutionCount(const String &conditionName);

    unsigned long getLastTriggerTime(int conditionId);
    unsigned long getLastTriggerTime(const String &conditionName);

    void resetCondition(int conditionId);
    void resetCondition(const String &conditionName);

    void setEnabled(int conditionId, bool enabled);
    void setEnabled(const String &conditionName, bool enabled);

    void setRepeatCount(int conditionId, int repeatCount);
    void setRepeatCount(const String &conditionName, int repeatCount);

    void setInterval(int conditionId, unsigned long interval);
    void setInterval(const String &conditionName, unsigned long interval);

    void setDebounceTime(int conditionId, unsigned long debounceTime);
    void setDebounceTime(const String &conditionName, unsigned long debounceTime);

    void pauseExecution(int conditionId);
    void pauseExecution(const String &conditionName);

    void resumeExecution(int conditionId);
    void resumeExecution(const String &conditionName);

    void stopExecution(int conditionId);
    void stopExecution(const String &conditionName);

    bool hasCondition(const String &conditionName);
    bool removeCondition(int conditionId);
    bool removeCondition(const String &conditionName);

    void printStatus(int conditionId);
    void printStatus(const String &conditionName);
    void printAllStatus();
};

#endif