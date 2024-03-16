/*
 *  Task.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

#include "Arduino.h"

const int MAXNUM_TASK_NAME_CHAR = 20;

class TaskHandle {
public:
    TaskHandle();
    ~TaskHandle();

    bool initialize(void (*_callback)() = nullptr);
    void createTask(uint32_t _stack_depth, void (*_task_callback)(void *pvParameter), const char *_task_name = "task");
    void delay(uint32_t _time = 20);

    void setUxPriority(uint8_t _ux_priority = 1);
    void setStackDepth(uint32_t _stack_depth = 10000);
private:
    char task_name_[MAXNUM_TASK_NAME_CHAR];
    uint8_t ux_priority_;
    uint8_t task_index_;
    uint32_t stack_depth_;

};


#endif

#endif // TASK_HANDLER_H