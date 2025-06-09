#include "VariableWatcher.h"

VariableWatcher::VariableWatcher(int initialCapacity) {
    capacity = initialCapacity;
    variables = new VarInfo[capacity];
    count = 0;
}

VariableWatcher::~VariableWatcher() {
    for (int i = 0; i < count; i++) {
        delete[] (byte *) variables[i].lastValue;
    }
    delete[] variables;
}

void VariableWatcher::checkAll() {
    for (int i = 0; i < count; i++) {
        checkIndex(i);
    }
}

bool VariableWatcher::checkIndex(int index) {
    if (index < 0 || index >= count) return false;

    void *currentValue = variables[index].ptr;
    void *lastValue = variables[index].lastValue;
    size_t size = variables[index].size;

    if (memcmp(currentValue, lastValue, size) != 0) {
        memcpy(lastValue, currentValue, size);
        variables[index].changed = true;

        if (variables[index].callback != NULL) {
            variables[index].callback(currentValue);
        }

        return true;
    }

    return false;
}

bool VariableWatcher::isChange(String name) {
    int index = findVariableIndex(name);
    if (index >= 0) {
        return variables[index].changed;
    }
    return false;
}

bool VariableWatcher::hasChanged(String name) {
    int index = findVariableIndex(name);
    if (index >= 0) {
        bool changed = variables[index].changed;
        variables[index].changed = false;
        return changed;
    }
    return false;
}

void *VariableWatcher::getValue(String name) {
    int index = findVariableIndex(name);
    if (index >= 0) {
        return variables[index].ptr;
    }
    return NULL;
}

int VariableWatcher::getValueInt(String name) {
    void *value = getValue(name);
    if (value != NULL) {
        return *(int *) value;
    }
    return 0;
}

float VariableWatcher::getValueFloat(String name) {
    void *value = getValue(name);
    if (value != NULL) {
        return *(float *) value;
    }
    return 0.0f;
}

double VariableWatcher::getValueDouble(String name) {
    void *value = getValue(name);
    if (value != NULL) {
        return *(double *) value;
    }
    return 0.0;
}

bool VariableWatcher::getValueBool(String name) {
    void *value = getValue(name);
    if (value != NULL) {
        return *(bool *) value;
    }
    return false;
}

String VariableWatcher::getValueString(String name) {
    void *value = getValue(name);
    if (value != NULL) {
        return *(String *) value;
    }
    return "";
}

long VariableWatcher::getValueLong(String name) {
    void *value = getValue(name);
    if (value != NULL) {
        return *(long *) value;
    }
    return 0L;
}

void VariableWatcher::resetChanges() {
    for (int i = 0; i < count; i++) {
        variables[i].changed = false;
    }
}

void VariableWatcher::printChanges() {
    for (int i = 0; i < count; i++) {
        if (variables[i].changed) {
            void *currentValue = variables[i].ptr;
            size_t size = variables[i].size;

            Serial.print(variables[i].name);
            Serial.print(" change: ");

            if (size == sizeof(int)) {
                Serial.println(*(int *) currentValue);
            } else if (size == sizeof(float)) {
                Serial.println(*(float *) currentValue);
            } else if (size == sizeof(bool)) {
                Serial.println(*(bool *) currentValue ? "true" : "false");
            } else if (size == sizeof(long)) {
                Serial.println(*(long *) currentValue);
            } else if (size == sizeof(double)) {
                Serial.println(*(double *) currentValue);
            } else if (size == sizeof(String)) {
                Serial.println(*(String *) currentValue);
            } else {
                Serial.println("[data]");
            }
        }
    }
}

int VariableWatcher::findVariableIndex(String name) {
    for (int i = 0; i < count; i++) {
        if (variables[i].name == name) {
            return i;
        }
    }
    return -1;
}