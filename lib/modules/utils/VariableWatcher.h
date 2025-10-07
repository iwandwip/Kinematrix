#ifndef VARIABLE_WATCHER_H
#define VARIABLE_WATCHER_H

#include "Arduino.h"

class VariableWatcher {
private:
    struct VarInfo {
        void *ptr;
        void *lastValue;
        size_t size;
        String name;
        bool changed;
        void (*callback)(void *);
    };

    VarInfo *variables;
    int count;
    int capacity;

    int findVariableIndex(String name);

public:
    VariableWatcher(int initialCapacity = 5);
    ~VariableWatcher();

    template<typename T>
    int addVariable(T *var, String name = "", void (*callbackFn)(void *) = NULL) {
        if (count >= capacity) {
            capacity *= 2;
            VarInfo *newVars = new VarInfo[capacity];
            memcpy(newVars, variables, count * sizeof(VarInfo));
            delete[] variables;
            variables = newVars;
        }

        variables[count].ptr = (void *) var;
        variables[count].size = sizeof(T);
        variables[count].name = name;
        variables[count].callback = callbackFn;
        variables[count].changed = false;

        variables[count].lastValue = new byte[sizeof(T)];
        memcpy(variables[count].lastValue, var, sizeof(T));

        return count++;
    }

    void checkAll();
    bool checkIndex(int index);
    bool isChange(String name);
    bool hasChanged(String name);
    void *getValue(String name);

    template<typename T>
    T getValueAs(String name) {
        void *value = getValue(name);
        if (value != NULL) {
            return *(T *) value;
        }
        return T();
    }

    int getValueInt(String name);
    float getValueFloat(String name);
    double getValueDouble(String name);
    bool getValueBool(String name);
    String getValueString(String name);
    long getValueLong(String name);

    void resetChanges();
    void printChanges();
};

#endif