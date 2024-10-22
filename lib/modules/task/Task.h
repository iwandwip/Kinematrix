/*
 *  Task.h
 *
 *  Created on: 2023. 4. 3
 */

#ifndef TASK_HANDLER_H
#define TASK_HANDLER_H

#define ENABLE_ARDUINO_RTOS 0

#if defined(ESP32)

#include "Arduino.h"

#pragma message("[COMPILED]: Task.h")

const int MAXNUM_TASK_NAME_CHAR = 20;

class TaskHandle {
public:
    TaskHandle();
    ~TaskHandle();

    void initialize(void (*_callback)() = nullptr);
    void setInitCoreID(uint8_t coreID);

    TaskHandle_t *
    createTask(uint32_t _stack_depth, void (*_task_callback)(void *pvParameter), const char *_task_name = "task");
    void deleteTask(TaskHandle_t _task);
    void delay(uint32_t _time = 20);

    void setUxPriority(uint8_t _ux_priority = 1);
    void setStackDepth(uint32_t _stack_depth = 10000);
private:
    char task_name_[MAXNUM_TASK_NAME_CHAR];
    uint8_t ux_priority_;
    uint8_t task_index_;
    uint32_t stack_depth_;

};

#else

#if ENABLE_ARDUINO_RTOS && ENABLE_ARDUINO_KINEMATRIX

#include "Arduino.h"
#include "Arduino_FreeRTOS.h"

const int MAXNUM_TASK_NAME_CHAR = 20;

class TaskHandle {
public:
    TaskHandle();
    ~TaskHandle();

    void initialize(void (*_callback)() = nullptr);
    TaskHandle_t *
    createTask(uint32_t _stack_depth, int8_t _priority, void (*_task_callback)(void *pvParameter),
               const char *_task_name = "task");
    void deleteTask(TaskHandle_t _task);
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

#endif

#endif // TASK_HANDLER_H