/*
 *  Task.cpp
 *
 *  Created on: 2023. 4. 3
 */

#include "Task.h"

#if defined(ESP32) || defined(ESP8266)

TaskHandle::TaskHandle()
        : ux_priority_(1),
          task_index_(0),
          stack_depth_(10000) {
}

TaskHandle::~TaskHandle() {
}

void TaskHandle::initialize(void (*_callback)()) {
    if (_callback != nullptr) _callback();
}

TaskHandle_t *
TaskHandle::createTask(uint32_t _stack_depth, void (*_task_callback)(void *pvParameter), const char *_task_name) {
    TaskHandle_t *taskHandler = new TaskHandle_t;
    xTaskCreatePinnedToCore(
            _task_callback,
            _task_name,
            _stack_depth,
            nullptr,
            1,
            taskHandler,
            task_index_);
    task_index_ = !task_index_;
    return taskHandler;
}

void TaskHandle::deleteTask(TaskHandle_t _task) {
    vTaskDelete(_task);
}

void TaskHandle::delay(uint32_t _time) {
    vTaskDelay(_time / portTICK_PERIOD_MS);
}

void TaskHandle::setUxPriority(uint8_t _ux_priority) {
    ux_priority_ = _ux_priority;
}

void TaskHandle::setStackDepth(uint32_t _stack_depth) {
    stack_depth_ = _stack_depth;
}

#else

#if ENABLE_ARDUINO_RTOS

TaskHandle::TaskHandle()
        : ux_priority_(1),
          task_index_(0),
          stack_depth_(10000) {
}

TaskHandle::~TaskHandle() {
}

void TaskHandle::initialize(void (*_callback)()) {
    if (_callback != nullptr) _callback();
}

TaskHandle_t *
TaskHandle::createTask(uint32_t _stack_depth, int8_t _priority, void (*_task_callback)(void *pvParameter),
                       const char *_task_name) {
    TaskHandle_t *taskHandler = new TaskHandle_t;
    xTaskCreate(
            _task_callback,
            _task_name,
            _stack_depth,
            nullptr,
            _priority,
            taskHandler);
    return taskHandler;
}

void TaskHandle::deleteTask(TaskHandle_t _task) {
    vTaskDelete(_task);
}

void TaskHandle::delay(uint32_t _time) {
    vTaskDelay(_time / portTICK_PERIOD_MS);
}

void TaskHandle::setUxPriority(uint8_t _ux_priority) {
    ux_priority_ = _ux_priority;
}

void TaskHandle::setStackDepth(uint32_t _stack_depth) {
    stack_depth_ = _stack_depth;
}

#endif

#endif