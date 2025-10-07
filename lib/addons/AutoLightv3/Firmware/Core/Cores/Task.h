#pragma once
#ifndef TASK_H
#define TASK_H

#if defined(ESP32)

#include "Arduino.h"
#include "../Common/Constants.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace AutoLight {
    using namespace Constants;

    class TaskHandle {
    public:
        TaskHandle();
        ~TaskHandle();

        void initialize(void (*_callback)() = nullptr);
        TaskHandle_t *createTask(uint32_t _stack_depth, void (*_task_callback)(void *pvParameter), const char *_task_name = "task");
        void deleteTask(TaskHandle_t _task);
        void delay(uint32_t _time = 20);

        void setUxPriority(uint8_t _ux_priority = 1);
        void setStackDepth(uint32_t _stack_depth = 10000);
    private:
        char task_name_[Constants::MAXNUM_TASK_NAME_CHAR];
        uint8_t ux_priority_;
        uint8_t task_index_;
        uint32_t stack_depth_;

    };
}


#endif

#endif
