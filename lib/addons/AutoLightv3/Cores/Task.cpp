/*
 *  Task.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

// Task.cpp - Implementation file
// Included by Task.h - do not include Task.h here to avoid circular dependency

#if defined(ESP32)

namespace AutoLight {
    TaskHandle::TaskHandle() {
        ux_priority_ = 1;
        task_index_ = 0;
        stack_depth_ = 10000;
        strcpy(task_name_, "autolight_task");
    }

    TaskHandle::~TaskHandle() {
        // Task cleanup handled by FreeRTOS
    }

    void TaskHandle::initialize(void (*_callback)()) {
        if (_callback != nullptr) {
            _callback();
        }
    }

    TaskHandle_t* TaskHandle::createTask(uint32_t _stack_depth, void (*_task_callback)(void *pvParameter), const char *_task_name) {
        TaskHandle_t* task_handle = new TaskHandle_t;
        
        BaseType_t result = xTaskCreate(
            _task_callback,
            _task_name ? _task_name : task_name_,
            _stack_depth,
            nullptr,
            ux_priority_,
            task_handle
        );
        
        if (result != pdPASS) {
            delete task_handle;
            return nullptr;
        }
        
        task_index_++;
        return task_handle;
    }

    void TaskHandle::deleteTask(TaskHandle_t _task) {
        if (_task != nullptr) {
            vTaskDelete(_task);
        }
    }

    void TaskHandle::delay(uint32_t _time) {
        vTaskDelay(pdMS_TO_TICKS(_time));
    }

    void TaskHandle::setUxPriority(uint8_t _ux_priority) {
        ux_priority_ = _ux_priority;
    }

    void TaskHandle::setStackDepth(uint32_t _stack_depth) {
        stack_depth_ = _stack_depth;
    }
}

#endif