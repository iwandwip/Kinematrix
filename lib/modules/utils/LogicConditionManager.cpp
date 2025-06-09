#include "LogicConditionManager.h"

LogicConditionManager::LogicConditionManager(int initialCapacity) {
    capacity = initialCapacity;
    conditions = new Condition[capacity];
    conditionCount = 0;
    lastNameLookup = "";
    lastIdFound = -1;
}

LogicConditionManager::~LogicConditionManager() {
    for (int i = 0; i < conditionCount; i++) {
        if (conditions[i].childConditions != NULL) {
            delete[] conditions[i].childConditions;
        }
    }
    delete[] conditions;
}

int LogicConditionManager::findConditionIdByName(const String &name) {
    if (name.equals(lastNameLookup) && lastIdFound >= 0 && lastIdFound < conditionCount) {
        return lastIdFound;
    }
    for (int i = 0; i < conditionCount; i++) {
        if (conditions[i].name.equals(name)) {
            lastNameLookup = name;
            lastIdFound = i;
            return i;
        }
    }
    return -1;
}

void LogicConditionManager::expandCapacity() {
    capacity *= 2;
    Condition *newConditions = new Condition[capacity];
    memcpy(newConditions, conditions, conditionCount * sizeof(Condition));
    delete[] conditions;
    conditions = newConditions;
}

bool LogicConditionManager::evaluateCondition(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount || !conditions[conditionId].enabled) {
        return false;
    }

    Condition &cond = conditions[conditionId];
    bool result = false;

    switch (cond.type) {
        case BOOLEAN_FUNCTION:
            if (cond.boolFunction) {
                cond.currentBoolValue = cond.boolFunction();
            }
            break;

        case BOOLEAN_POINTER:
            if (cond.boolPointer) {
                cond.currentBoolValue = *cond.boolPointer;
            }
            break;

        case VALUE_FUNCTION:
            if (cond.valueFunction) {
                cond.currentFloatValue = cond.valueFunction();
                cond.currentBoolValue = evaluateValueCondition(cond.currentFloatValue, cond.threshold, cond.comparison);
            }
            break;

        case VALUE_POINTER:
            if (cond.valuePointer) {
                cond.currentFloatValue = *cond.valuePointer;
                cond.currentBoolValue = evaluateValueCondition(cond.currentFloatValue, cond.threshold, cond.comparison);
            }
            break;

        case COMPOSITE:
            cond.currentBoolValue = evaluateComposite(conditionId);
            break;

        case TIMER: {
            unsigned long currentTime = millis();
            if (cond.state == IDLE) {
                cond.currentBoolValue = false;
            } else if (cond.state == TRIGGERED || cond.state == EXECUTING) {
                if (cond.duration > 0 && currentTime - cond.startTime >= cond.duration) {
                    cond.currentBoolValue = true;
                } else {
                    cond.currentBoolValue = false;
                }
            }
        }
            break;

        case SEQUENCE:
            cond.currentBoolValue = evaluateSequence(conditionId);
            break;

        case EDGE_COUNTER:
            if (cond.childCount > 0 && cond.childConditions != NULL) {
                int targetId = cond.childConditions[0];
                if (targetId >= 0 && targetId < conditionCount) {
                    bool targetTriggered = evaluateCondition(targetId);
                    unsigned long currentTime = millis();

                    if (cond.state == IDLE && targetTriggered) {
                        cond.startTime = currentTime;
                        cond.occurrenceCount = 1;
                        cond.state = TRIGGERED;
                    } else if (cond.state == TRIGGERED || cond.state == EXECUTING) {
                        if (targetTriggered && !conditions[targetId].wasTriggered) {
                            cond.occurrenceCount++;
                        }
                        if (cond.occurrenceCount >= cond.minOccurrences) {
                            cond.currentBoolValue = true;
                        }
                        if (cond.duration > 0 && currentTime - cond.startTime >= cond.duration) {
                            if (cond.occurrenceCount < cond.minOccurrences) {
                                resetCondition(conditionId);
                            }
                        }
                    }
                }
            }
            break;
    }

    switch (cond.triggerType) {
        case WHEN_TRUE:
            result = cond.currentBoolValue;
            break;
        case WHEN_FALSE:
            result = !cond.currentBoolValue;
            break;
        case ON_CHANGE:
            result = evaluateOnChange(conditionId);
            break;
        case TO_TRUE:
            result = (cond.currentBoolValue && !cond.lastBoolValue);
            break;
        case TO_FALSE:
            result = (!cond.currentBoolValue && cond.lastBoolValue);
            break;
    }

    if (cond.debounceTime > 0 && result) {
        unsigned long currentTime = millis();
        if (currentTime - cond.lastDebounceTime < cond.debounceTime) {
            result = false;
        } else {
            cond.lastDebounceTime = currentTime;
        }
    }

    return result;
}

bool LogicConditionManager::evaluateOnChange(int conditionId) {
    Condition &cond = conditions[conditionId];

    switch (cond.type) {
        case BOOLEAN_FUNCTION:
        case BOOLEAN_POINTER:
        case COMPOSITE:
        case TIMER:
        case SEQUENCE:
        case EDGE_COUNTER:
            return (cond.currentBoolValue != cond.lastBoolValue);

        case VALUE_FUNCTION:
        case VALUE_POINTER: {
            if (!cond.hasValidFloatValue) {
                cond.lastFloatValue = cond.currentFloatValue;
                cond.hasValidFloatValue = true;
                return false;
            }

            bool changed = (cond.currentFloatValue != cond.lastFloatValue);
            if (changed) {
                cond.lastFloatValue = cond.currentFloatValue;
            }
            return changed;
        }

        default:
            return false;
    }
}

bool LogicConditionManager::evaluateComposite(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return false;
    }

    Condition &cond = conditions[conditionId];
    if (cond.type != COMPOSITE || cond.childConditions == NULL || cond.childCount <= 0) {
        return false;
    }

    bool result = false;

    switch (cond.logicOp) {
        case AND:
            result = true;
            for (int i = 0; i < cond.childCount; i++) {
                int childId = cond.childConditions[i];
                if (childId >= 0 && childId < conditionCount) {
                    if (!evaluateCondition(childId)) {
                        result = false;
                        break;
                    }
                }
            }
            break;

        case OR:
            result = false;
            for (int i = 0; i < cond.childCount; i++) {
                int childId = cond.childConditions[i];
                if (childId >= 0 && childId < conditionCount) {
                    if (evaluateCondition(childId)) {
                        result = true;
                        break;
                    }
                }
            }
            break;

        case NOT:
            if (cond.childCount > 0) {
                int childId = cond.childConditions[0];
                if (childId >= 0 && childId < conditionCount) {
                    result = !evaluateCondition(childId);
                }
            }
            break;

        case XOR: {
            int trueCount = 0;
            for (int i = 0; i < cond.childCount; i++) {
                int childId = cond.childConditions[i];
                if (childId >= 0 && childId < conditionCount) {
                    if (evaluateCondition(childId)) {
                        trueCount++;
                    }
                }
            }
            result = (trueCount == 1);
        }
            break;

        case NAND:
            result = true;
            for (int i = 0; i < cond.childCount; i++) {
                int childId = cond.childConditions[i];
                if (childId >= 0 && childId < conditionCount) {
                    if (!evaluateCondition(childId)) {
                        result = false;
                        break;
                    }
                }
            }
            result = !result;
            break;

        case NOR:
            result = true;
            for (int i = 0; i < cond.childCount; i++) {
                int childId = cond.childConditions[i];
                if (childId >= 0 && childId < conditionCount) {
                    if (evaluateCondition(childId)) {
                        result = false;
                        break;
                    }
                }
            }
            break;
    }

    return result;
}

bool LogicConditionManager::evaluateSequence(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return false;
    }

    Condition &cond = conditions[conditionId];
    if (cond.type != SEQUENCE || cond.childConditions == NULL || cond.childCount <= 0) {
        return false;
    }

    unsigned long currentTime = millis();

    if (cond.state == IDLE) {
        for (int i = 0; i < cond.childCount; i++) {
            int childId = cond.childConditions[i];
            if (childId >= 0 && childId < conditionCount) {
                if (evaluateCondition(childId)) {
                    cond.startTime = currentTime;
                    cond.occurrenceCount = 1;
                    cond.state = TRIGGERED;
                    break;
                }
            }
        }
        return false;
    } else if (cond.state == TRIGGERED) {
        if (cond.duration > 0 && currentTime - cond.startTime >= cond.duration) {
            cond.state = IDLE;
            return false;
        }

        for (int i = cond.occurrenceCount; i < cond.childCount; i++) {
            int childId = cond.childConditions[i];
            if (childId >= 0 && childId < conditionCount) {
                if (evaluateCondition(childId)) {
                    cond.occurrenceCount++;
                    if (cond.occurrenceCount >= cond.childCount) {
                        return true;
                    }
                    break;
                }
            }
        }
    }

    return false;
}

void LogicConditionManager::updateConditionState(int conditionId, bool triggered) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }

    Condition &cond = conditions[conditionId];
    unsigned long currentTime = millis();

    if (cond.actionDuration > 0) {
        handleActionDuration(conditionId);
    }

    if (triggered && (cond.state == IDLE || cond.state == COMPLETED)) {
        if (cond.actionWaiting) {
            return;
        }

        cond.state = TRIGGERED;
        cond.lastTriggerTime = currentTime;
        cond.wasTriggered = true;

        if (cond.actionFunction || cond.paramActionFunction) {
            cond.state = EXECUTING;
            cond.executionCount = 0;
            cond.lastExecutionTime = currentTime;

            if (cond.actionDuration > 0) {
                cond.actionStartTime = currentTime;
                cond.actionActive = true;
                cond.state = ACTION_RUNNING;
            }

            if (cond.actionFunction) {
                cond.actionFunction();
            } else if (cond.paramActionFunction) {
                cond.paramActionFunction(cond.actionParam);
            }

            cond.executionCount++;

            if (cond.repeatCount == 1 && cond.actionDuration == 0) {
                cond.state = COMPLETED;
            }
        }
    } else if (cond.state == EXECUTING) {
        bool actionDue = (currentTime - cond.lastExecutionTime >= cond.interval);

        if (actionDue && cond.executionCount < cond.repeatCount) {
            if (cond.actionFunction) {
                cond.actionFunction();
            } else if (cond.paramActionFunction) {
                cond.paramActionFunction(cond.actionParam);
            }

            cond.executionCount++;
            cond.lastExecutionTime = currentTime;

            if (cond.repeatCount > 0 && cond.executionCount >= cond.repeatCount) {
                cond.state = COMPLETED;
            }
        }
    }

    if (cond.autoReset && cond.state == COMPLETED) {
        resetCondition(conditionId);
    }
}

void LogicConditionManager::handleActionDuration(int conditionId) {
    Condition &cond = conditions[conditionId];
    unsigned long currentTime = millis();

    if (cond.actionActive && currentTime - cond.actionStartTime >= cond.actionDuration) {
        if (cond.stopActionFunction) {
            cond.stopActionFunction();
        }
        cond.actionActive = false;

        if (cond.waitInterval > 0) {
            cond.actionWaiting = true;
            cond.waitStartTime = currentTime;
            cond.state = ACTION_WAITING;
        } else {
            if (cond.repeatCount == 1) {
                cond.state = COMPLETED;
            } else {
                cond.state = IDLE;
            }
        }
    }

    if (cond.actionWaiting && currentTime - cond.waitStartTime >= cond.waitInterval) {
        cond.actionWaiting = false;
        if (cond.repeatCount == 1) {
            cond.state = COMPLETED;
        } else {
            cond.state = IDLE;
        }
    }
}

bool LogicConditionManager::evaluateValueCondition(float value, float threshold, ComparisonType comparison) {
    switch (comparison) {
        case GREATER_THAN:
            return value > threshold;
        case LESS_THAN:
            return value < threshold;
        case GREATER_EQUAL:
            return value >= threshold;
        case LESS_EQUAL:
            return value <= threshold;
        case EQUAL:
            return value == threshold;
        case NOT_EQUAL:
            return value != threshold;
        default:
            return false;
    }
}

int LogicConditionManager::addCondition(const String &name, bool (*conditionFunction)(),
                                        TriggerType triggerType, void (*actionFunction)(),
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_FUNCTION;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = conditionFunction;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = conditionFunction();
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCondition(const String &name, bool (*conditionFunction)(),
                                        TriggerType triggerType, void (*actionFunction)(),
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime, unsigned long actionDuration,
                                        void (*stopActionFunction)(), unsigned long waitInterval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_FUNCTION;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = conditionFunction;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = stopActionFunction;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = actionDuration;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = waitInterval;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = conditionFunction();
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCondition(const String &name, bool *conditionPointer,
                                        TriggerType triggerType, void (*actionFunction)(),
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_POINTER;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = conditionPointer;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = *conditionPointer;
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCondition(const String &name, bool *conditionPointer,
                                        TriggerType triggerType, void (*actionFunction)(),
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime, unsigned long actionDuration,
                                        void (*stopActionFunction)(), unsigned long waitInterval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_POINTER;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = conditionPointer;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = stopActionFunction;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = actionDuration;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = waitInterval;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = *conditionPointer;
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCondition(const String &name, bool (*conditionFunction)(),
                                        TriggerType triggerType, void (*actionFunction)(void *), void *param,
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_FUNCTION;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = conditionFunction;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = NULL;
    conditions[id].paramActionFunction = actionFunction;
    conditions[id].actionParam = param;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = conditionFunction();
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCondition(const String &name, bool (*conditionFunction)(),
                                        TriggerType triggerType, void (*actionFunction)(void *), void *param,
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime, unsigned long actionDuration,
                                        void (*stopActionFunction)(), unsigned long waitInterval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_FUNCTION;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = conditionFunction;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = NULL;
    conditions[id].paramActionFunction = actionFunction;
    conditions[id].actionParam = param;
    conditions[id].stopActionFunction = stopActionFunction;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = actionDuration;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = waitInterval;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = conditionFunction();
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCondition(const String &name, bool *conditionPointer,
                                        TriggerType triggerType, void (*actionFunction)(void *), void *param,
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_POINTER;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = conditionPointer;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = NULL;
    conditions[id].paramActionFunction = actionFunction;
    conditions[id].actionParam = param;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = *conditionPointer;
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCondition(const String &name, bool *conditionPointer,
                                        TriggerType triggerType, void (*actionFunction)(void *), void *param,
                                        int repeatCount, unsigned long interval,
                                        unsigned long debounceTime, unsigned long actionDuration,
                                        void (*stopActionFunction)(), unsigned long waitInterval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = BOOLEAN_POINTER;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = conditionPointer;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = NULL;
    conditions[id].paramActionFunction = actionFunction;
    conditions[id].actionParam = param;
    conditions[id].stopActionFunction = stopActionFunction;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = debounceTime;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = actionDuration;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = waitInterval;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = *conditionPointer;
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addValueCondition(const String &name, float (*valueFunction)(),
                                             float threshold, ComparisonType comparison, TriggerType triggerType,
                                             void (*actionFunction)(), int repeatCount,
                                             unsigned long interval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = VALUE_FUNCTION;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = valueFunction;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = threshold;
    conditions[id].comparison = comparison;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].currentFloatValue = valueFunction();
    conditions[id].lastBoolValue = evaluateValueCondition(conditions[id].currentFloatValue, threshold, comparison);
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = conditions[id].currentFloatValue;
    conditions[id].hasValidFloatValue = true;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addValueCondition(const String &name, float (*valueFunction)(),
                                             float threshold, ComparisonType comparison, TriggerType triggerType,
                                             void (*actionFunction)(), int repeatCount,
                                             unsigned long interval, unsigned long actionDuration,
                                             void (*stopActionFunction)(), unsigned long waitInterval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = VALUE_FUNCTION;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = valueFunction;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = threshold;
    conditions[id].comparison = comparison;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = stopActionFunction;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = actionDuration;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = waitInterval;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].currentFloatValue = valueFunction();
    conditions[id].lastBoolValue = evaluateValueCondition(conditions[id].currentFloatValue, threshold, comparison);
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = conditions[id].currentFloatValue;
    conditions[id].hasValidFloatValue = true;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addValueCondition(const String &name, float *valuePointer,
                                             float threshold, ComparisonType comparison, TriggerType triggerType,
                                             void (*actionFunction)(), int repeatCount,
                                             unsigned long interval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = VALUE_POINTER;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = valuePointer;
    conditions[id].threshold = threshold;
    conditions[id].comparison = comparison;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].currentFloatValue = *valuePointer;
    conditions[id].lastBoolValue = evaluateValueCondition(conditions[id].currentFloatValue, threshold, comparison);
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = conditions[id].currentFloatValue;
    conditions[id].hasValidFloatValue = true;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addValueCondition(const String &name, float *valuePointer,
                                             float threshold, ComparisonType comparison, TriggerType triggerType,
                                             void (*actionFunction)(), int repeatCount,
                                             unsigned long interval, unsigned long actionDuration,
                                             void (*stopActionFunction)(), unsigned long waitInterval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = VALUE_POINTER;
    conditions[id].triggerType = triggerType;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = valuePointer;
    conditions[id].threshold = threshold;
    conditions[id].comparison = comparison;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = stopActionFunction;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = actionDuration;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = waitInterval;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].currentFloatValue = *valuePointer;
    conditions[id].lastBoolValue = evaluateValueCondition(conditions[id].currentFloatValue, threshold, comparison);
    conditions[id].currentBoolValue = conditions[id].lastBoolValue;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = conditions[id].currentFloatValue;
    conditions[id].hasValidFloatValue = true;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCompositeCondition(const String &name, LogicOp logicOperator,
                                                 int conditionIds[], int count,
                                                 void (*actionFunction)(), int repeatCount,
                                                 unsigned long interval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = COMPOSITE;
    conditions[id].triggerType = WHEN_TRUE;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].logicOp = logicOperator;
    conditions[id].childCount = count;
    conditions[id].childConditions = new int[count];
    memcpy(conditions[id].childConditions, conditionIds, count * sizeof(int));
    conditions[id].interval = interval;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = false;
    conditions[id].currentBoolValue = false;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addCompositeCondition(const String &name, LogicOp logicOperator,
                                                 int conditionIds[], int count,
                                                 void (*actionFunction)(), int repeatCount,
                                                 unsigned long interval, unsigned long actionDuration,
                                                 void (*stopActionFunction)(), unsigned long waitInterval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = COMPOSITE;
    conditions[id].triggerType = WHEN_TRUE;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = stopActionFunction;
    conditions[id].logicOp = logicOperator;
    conditions[id].childCount = count;
    conditions[id].childConditions = new int[count];
    memcpy(conditions[id].childConditions, conditionIds, count * sizeof(int));
    conditions[id].interval = interval;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].actionDuration = actionDuration;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = waitInterval;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = false;
    conditions[id].currentBoolValue = false;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addAndCondition(const String &name, int conditionIds[], int count,
                                           void (*actionFunction)(), int repeatCount,
                                           unsigned long interval) {
    return addCompositeCondition(name, AND, conditionIds, count, actionFunction, repeatCount, interval);
}

int LogicConditionManager::addOrCondition(const String &name, int conditionIds[], int count,
                                          void (*actionFunction)(), int repeatCount,
                                          unsigned long interval) {
    return addCompositeCondition(name, OR, conditionIds, count, actionFunction, repeatCount, interval);
}

int LogicConditionManager::addNotCondition(const String &name, int conditionId,
                                           void (*actionFunction)(), int repeatCount,
                                           unsigned long interval) {
    int ids[1] = {conditionId};
    return addCompositeCondition(name, NOT, ids, 1, actionFunction, repeatCount, interval);
}

int LogicConditionManager::addTimerCondition(const String &name, unsigned long duration,
                                             bool autoReset, void (*actionFunction)(),
                                             int repeatCount, unsigned long interval) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = TIMER;
    conditions[id].triggerType = WHEN_TRUE;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childConditions = NULL;
    conditions[id].childCount = 0;
    conditions[id].interval = interval;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].duration = duration;
    conditions[id].startTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = false;
    conditions[id].currentBoolValue = false;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = autoReset;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addSequenceCondition(const String &name, int conditionIds[], int count,
                                                unsigned long maxTimespan, int minOccurrences,
                                                void (*actionFunction)(), int repeatCount) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = SEQUENCE;
    conditions[id].triggerType = WHEN_TRUE;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childCount = count;
    conditions[id].childConditions = new int[count];
    memcpy(conditions[id].childConditions, conditionIds, count * sizeof(int));
    conditions[id].interval = 0;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].duration = maxTimespan;
    conditions[id].startTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].minOccurrences = minOccurrences;
    conditions[id].occurrenceCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = false;
    conditions[id].currentBoolValue = false;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

int LogicConditionManager::addEdgeCountCondition(const String &name, int conditionId,
                                                 int targetCount, unsigned long timeWindow,
                                                 void (*actionFunction)(), int repeatCount) {
    if (conditionCount >= capacity) {
        expandCapacity();
    }

    int id = conditionCount++;
    conditions[id].name = name;
    conditions[id].type = EDGE_COUNTER;
    conditions[id].triggerType = TO_TRUE;
    conditions[id].boolFunction = NULL;
    conditions[id].boolPointer = NULL;
    conditions[id].valueFunction = NULL;
    conditions[id].valuePointer = NULL;
    conditions[id].threshold = 0;
    conditions[id].comparison = GREATER_THAN;
    conditions[id].actionFunction = actionFunction;
    conditions[id].paramActionFunction = NULL;
    conditions[id].actionParam = NULL;
    conditions[id].stopActionFunction = NULL;
    conditions[id].childCount = 1;
    conditions[id].childConditions = new int[1];
    conditions[id].childConditions[0] = conditionId;
    conditions[id].interval = 0;
    conditions[id].debounceTime = 0;
    conditions[id].lastDebounceTime = 0;
    conditions[id].lastTriggerTime = 0;
    conditions[id].lastExecutionTime = 0;
    conditions[id].duration = timeWindow;
    conditions[id].startTime = 0;
    conditions[id].actionDuration = 0;
    conditions[id].actionStartTime = 0;
    conditions[id].waitInterval = 0;
    conditions[id].waitStartTime = 0;
    conditions[id].repeatCount = repeatCount;
    conditions[id].executionCount = 0;
    conditions[id].minOccurrences = targetCount;
    conditions[id].occurrenceCount = 0;
    conditions[id].enabled = true;
    conditions[id].lastBoolValue = false;
    conditions[id].currentBoolValue = false;
    conditions[id].wasTriggered = false;
    conditions[id].autoReset = false;
    conditions[id].actionActive = false;
    conditions[id].actionWaiting = false;
    conditions[id].lastFloatValue = 0.0;
    conditions[id].currentFloatValue = 0.0;
    conditions[id].hasValidFloatValue = false;
    conditions[id].state = IDLE;

    return id;
}

void LogicConditionManager::update() {
    for (int i = 0; i < conditionCount; i++) {
        if (!conditions[i].enabled || conditions[i].state == PAUSED) {
            continue;
        }

        bool triggered = evaluateCondition(i);
        updateConditionState(i, triggered);

        conditions[i].lastBoolValue = conditions[i].currentBoolValue;
        conditions[i].wasTriggered = triggered;
    }
}

bool LogicConditionManager::isConditionMet(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount || !conditions[conditionId].enabled) {
        return false;
    }
    return evaluateCondition(conditionId);
}

bool LogicConditionManager::isConditionMet(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    return (id >= 0) ? isConditionMet(id) : false;
}

bool LogicConditionManager::checkAndTrigger(int conditionId, void (*callbackFunction)()) {
    if (conditionId < 0 || conditionId >= conditionCount || !conditions[conditionId].enabled) {
        return false;
    }

    bool triggered = evaluateCondition(conditionId);
    if (triggered) {
        if (callbackFunction) {
            callbackFunction();
        }
        return true;
    }
    return false;
}

bool LogicConditionManager::checkAndTrigger(const String &conditionName, void (*callbackFunction)()) {
    int id = findConditionIdByName(conditionName);
    return (id >= 0) ? checkAndTrigger(id, callbackFunction) : false;
}

LogicConditionManager::ConditionState LogicConditionManager::getConditionState(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return IDLE;
    }
    return conditions[conditionId].state;
}

LogicConditionManager::ConditionState LogicConditionManager::getConditionState(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    return (id >= 0) ? getConditionState(id) : IDLE;
}

bool LogicConditionManager::getCurrentBooleanValue(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return false;
    }
    return conditions[conditionId].currentBoolValue;
}

bool LogicConditionManager::getCurrentBooleanValue(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    return (id >= 0) ? getCurrentBooleanValue(id) : false;
}

int LogicConditionManager::getExecutionCount(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return 0;
    }
    return conditions[conditionId].executionCount;
}

int LogicConditionManager::getExecutionCount(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    return (id >= 0) ? getExecutionCount(id) : 0;
}

unsigned long LogicConditionManager::getLastTriggerTime(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return 0;
    }
    return conditions[conditionId].lastTriggerTime;
}

unsigned long LogicConditionManager::getLastTriggerTime(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    return (id >= 0) ? getLastTriggerTime(id) : 0;
}

void LogicConditionManager::resetCondition(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }

    Condition &cond = conditions[conditionId];
    cond.state = IDLE;
    cond.executionCount = 0;
    cond.occurrenceCount = 0;
    cond.wasTriggered = false;
    cond.actionActive = false;
    cond.actionWaiting = false;
    cond.hasValidFloatValue = false;
    cond.lastFloatValue = 0.0;
    cond.currentFloatValue = 0.0;

    if (cond.type == BOOLEAN_FUNCTION && cond.boolFunction) {
        cond.lastBoolValue = cond.boolFunction();
        cond.currentBoolValue = cond.lastBoolValue;
    } else if (cond.type == BOOLEAN_POINTER && cond.boolPointer) {
        cond.lastBoolValue = *cond.boolPointer;
        cond.currentBoolValue = cond.lastBoolValue;
    } else if (cond.type == VALUE_FUNCTION && cond.valueFunction) {
        cond.currentFloatValue = cond.valueFunction();
        cond.lastBoolValue = evaluateValueCondition(cond.currentFloatValue, cond.threshold, cond.comparison);
        cond.currentBoolValue = cond.lastBoolValue;
        cond.lastFloatValue = cond.currentFloatValue;
        cond.hasValidFloatValue = true;
    } else if (cond.type == VALUE_POINTER && cond.valuePointer) {
        cond.currentFloatValue = *cond.valuePointer;
        cond.lastBoolValue = evaluateValueCondition(cond.currentFloatValue, cond.threshold, cond.comparison);
        cond.currentBoolValue = cond.lastBoolValue;
        cond.lastFloatValue = cond.currentFloatValue;
        cond.hasValidFloatValue = true;
    }
}

void LogicConditionManager::resetCondition(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        resetCondition(id);
    }
}

void LogicConditionManager::setEnabled(int conditionId, bool enabled) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }
    conditions[conditionId].enabled = enabled;
    if (enabled) {
        resetCondition(conditionId);
    }
}

void LogicConditionManager::setEnabled(const String &conditionName, bool enabled) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        setEnabled(id, enabled);
    }
}

void LogicConditionManager::setRepeatCount(int conditionId, int repeatCount) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }
    conditions[conditionId].repeatCount = repeatCount;
}

void LogicConditionManager::setRepeatCount(const String &conditionName, int repeatCount) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        setRepeatCount(id, repeatCount);
    }
}

void LogicConditionManager::setInterval(int conditionId, unsigned long interval) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }
    conditions[conditionId].interval = interval;
}

void LogicConditionManager::setInterval(const String &conditionName, unsigned long interval) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        setInterval(id, interval);
    }
}

void LogicConditionManager::setDebounceTime(int conditionId, unsigned long debounceTime) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }
    conditions[conditionId].debounceTime = debounceTime;
}

void LogicConditionManager::setDebounceTime(const String &conditionName, unsigned long debounceTime) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        setDebounceTime(id, debounceTime);
    }
}

void LogicConditionManager::pauseExecution(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }
    if (conditions[conditionId].state == EXECUTING) {
        conditions[conditionId].state = PAUSED;
    }
}

void LogicConditionManager::pauseExecution(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        pauseExecution(id);
    }
}

void LogicConditionManager::resumeExecution(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }
    if (conditions[conditionId].state == PAUSED) {
        conditions[conditionId].state = EXECUTING;
        conditions[conditionId].lastExecutionTime = millis();
    }
}

void LogicConditionManager::resumeExecution(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        resumeExecution(id);
    }
}

void LogicConditionManager::stopExecution(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }
    if (conditions[conditionId].state == EXECUTING || conditions[conditionId].state == PAUSED) {
        conditions[conditionId].state = COMPLETED;
    }
}

void LogicConditionManager::stopExecution(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        stopExecution(id);
    }
}

bool LogicConditionManager::hasCondition(const String &conditionName) {
    return findConditionIdByName(conditionName) >= 0;
}

bool LogicConditionManager::removeCondition(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return false;
    }

    if (conditions[conditionId].childConditions != NULL) {
        delete[] conditions[conditionId].childConditions;
    }

    for (int i = conditionId; i < conditionCount - 1; i++) {
        conditions[i] = conditions[i + 1];
    }

    conditionCount--;

    for (int i = 0; i < conditionCount; i++) {
        if (conditions[i].type == COMPOSITE || conditions[i].type == SEQUENCE || conditions[i].type == EDGE_COUNTER) {
            for (int j = 0; j < conditions[i].childCount; j++) {
                if (conditions[i].childConditions[j] > conditionId) {
                    conditions[i].childConditions[j]--;
                } else if (conditions[i].childConditions[j] == conditionId) {
                    conditions[i].childConditions[j] = -1;
                }
            }
        }
    }

    lastNameLookup = "";
    lastIdFound = -1;
    return true;
}

bool LogicConditionManager::removeCondition(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    return (id >= 0) ? removeCondition(id) : false;
}

void LogicConditionManager::printStatus(int conditionId) {
    if (conditionId < 0 || conditionId >= conditionCount) {
        return;
    }

    Condition &cond = conditions[conditionId];
    Serial.print("Condition '");
    Serial.print(cond.name);
    Serial.print("': State=");

    switch (cond.state) {
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
        case ACTION_RUNNING:
            Serial.print("ACTION_RUNNING");
            break;
        case ACTION_WAITING:
            Serial.print("ACTION_WAITING");
            break;
    }

    Serial.print(", Value=");
    Serial.print(cond.currentBoolValue ? "TRUE" : "FALSE");
    Serial.print(", Executions=");
    Serial.print(cond.executionCount);

    if (cond.repeatCount > 0) {
        Serial.print("/");
        Serial.print(cond.repeatCount);
    }

    if (cond.actionDuration > 0) {
        Serial.print(", ActionDur=");
        Serial.print(cond.actionDuration);
        Serial.print("ms");
        if (cond.actionActive) {
            unsigned long elapsed = millis() - cond.actionStartTime;
            Serial.print(", Remaining=");
            Serial.print(cond.actionDuration - elapsed);
            Serial.print("ms");
        }
    }

    if (cond.waitInterval > 0 && cond.actionWaiting) {
        unsigned long elapsed = millis() - cond.waitStartTime;
        Serial.print(", WaitRemaining=");
        Serial.print(cond.waitInterval - elapsed);
        Serial.print("ms");
    }

    Serial.println();
}

void LogicConditionManager::printStatus(const String &conditionName) {
    int id = findConditionIdByName(conditionName);
    if (id >= 0) {
        printStatus(id);
    }
}

void LogicConditionManager::printAllStatus() {
    Serial.println("---- LogicConditionManager Status ----");
    for (int i = 0; i < conditionCount; i++) {
        Serial.print("[");
        Serial.print(i);
        Serial.print("] ");
        printStatus(i);
    }
    Serial.println("-----------------------------------");
}