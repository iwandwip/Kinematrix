#include "CeriaModuleESP32RTOS.h"

#ifdef ESP32

namespace CeriaDevOP {

    portMUX_TYPE CeriaModuleESP32RTOS::_criticalMux = portMUX_INITIALIZER_UNLOCKED;

    CeriaModuleESP32RTOS::CeriaModuleESP32RTOS() {
        _taskCount = 0;
        _hasNewData = false;
        _lastUpdateTime = 0;
        _updateInterval = 1000;
        _onMemoryAlert = nullptr;
        _onTaskStateChange = nullptr;
        _memoryThreshold = RTOS_MEMORY_THRESHOLD;
        _fragmentationThreshold = RTOS_FRAGMENTATION_THRESHOLD;
        _enableMemoryMonitoring = false;
        _monitorTask = nullptr;
        _monitoringActive = false;
        
        for (uint8_t i = 0; i < MAX_TASKS; i++) {
            _tasks[i].handle = nullptr;
            _tasks[i].isActive = false;
        }
    }

    CeriaModuleESP32RTOS::~CeriaModuleESP32RTOS() {
        stopMonitoring();
    }

    void CeriaModuleESP32RTOS::begin(uint32_t updateInterval) {
        _updateInterval = updateInterval;
        _lastUpdateTime = millis();
        _hasNewData = true;
    }

    void CeriaModuleESP32RTOS::update() {
        if (millis() - _lastUpdateTime >= _updateInterval) {
            for (uint8_t i = 0; i < _taskCount; i++) {
                if (_tasks[i].isActive) {
                    updateTaskInfo(i);
                }
            }
            
            if (_enableMemoryMonitoring) {
                updateMemoryInfo();
            }
            
            _hasNewData = true;
            _lastUpdateTime = millis();
        }
    }

    bool CeriaModuleESP32RTOS::hasNewData() {
        bool result = _hasNewData;
        _hasNewData = false;
        return result;
    }

    void CeriaModuleESP32RTOS::setUpdateInterval(uint32_t interval) {
        _updateInterval = interval;
    }

    TaskHandle_t CeriaModuleESP32RTOS::createTask(
        RTOSTaskFunction taskFunction,
        const String &taskName,
        uint32_t stackSize,
        void *parameters,
        RTOSTaskPriority priority,
        RTOSTaskCore core
    ) {
        if (_taskCount >= MAX_TASKS) {
            return nullptr;
        }

        TaskHandle_t handle = nullptr;
        BaseType_t result;

        if (core == RTOS_NO_AFFINITY) {
            result = xTaskCreate(
                taskFunction,
                taskName.c_str(),
                stackSize,
                parameters,
                priority,
                &handle
            );
        } else {
            result = xTaskCreatePinnedToCore(
                taskFunction,
                taskName.c_str(),
                stackSize,
                parameters,
                priority,
                &handle,
                core
            );
        }

        if (result == pdPASS && handle != nullptr) {
            _tasks[_taskCount].handle = handle;
            _tasks[_taskCount].name = taskName;
            _tasks[_taskCount].stackSize = stackSize;
            _tasks[_taskCount].priority = priority;
            _tasks[_taskCount].coreId = core;
            _tasks[_taskCount].state = RTOS_TASK_RUNNING;
            _tasks[_taskCount].isActive = true;
            
            _taskCount++;
            
            if (_onTaskStateChange) {
                _onTaskStateChange(taskName, RTOS_TASK_RUNNING);
            }
            
            return handle;
        }
        
        return nullptr;
    }

    bool CeriaModuleESP32RTOS::deleteTask(TaskHandle_t handle) {
        int8_t index = findTaskIndex(handle);
        if (index >= 0) {
            String taskName = _tasks[index].name;
            vTaskDelete(handle);
            _tasks[index].isActive = false;
            
            if (_onTaskStateChange) {
                _onTaskStateChange(taskName, RTOS_TASK_DELETED);
            }
            
            return true;
        }
        return false;
    }

    bool CeriaModuleESP32RTOS::deleteTask(const String &taskName) {
        int8_t index = findTaskIndex(taskName);
        if (index >= 0) {
            return deleteTask(_tasks[index].handle);
        }
        return false;
    }

    bool CeriaModuleESP32RTOS::suspendTask(TaskHandle_t handle) {
        int8_t index = findTaskIndex(handle);
        if (index >= 0) {
            vTaskSuspend(handle);
            _tasks[index].state = RTOS_TASK_SUSPENDED;
            
            if (_onTaskStateChange) {
                _onTaskStateChange(_tasks[index].name, RTOS_TASK_SUSPENDED);
            }
            
            return true;
        }
        return false;
    }

    bool CeriaModuleESP32RTOS::suspendTask(const String &taskName) {
        int8_t index = findTaskIndex(taskName);
        if (index >= 0) {
            return suspendTask(_tasks[index].handle);
        }
        return false;
    }

    bool CeriaModuleESP32RTOS::resumeTask(TaskHandle_t handle) {
        int8_t index = findTaskIndex(handle);
        if (index >= 0) {
            vTaskResume(handle);
            _tasks[index].state = RTOS_TASK_RUNNING;
            
            if (_onTaskStateChange) {
                _onTaskStateChange(_tasks[index].name, RTOS_TASK_RUNNING);
            }
            
            return true;
        }
        return false;
    }

    bool CeriaModuleESP32RTOS::resumeTask(const String &taskName) {
        int8_t index = findTaskIndex(taskName);
        if (index >= 0) {
            return resumeTask(_tasks[index].handle);
        }
        return false;
    }

    bool CeriaModuleESP32RTOS::isTaskActive(TaskHandle_t handle) {
        int8_t index = findTaskIndex(handle);
        return (index >= 0) ? _tasks[index].isActive : false;
    }

    bool CeriaModuleESP32RTOS::isTaskActive(const String &taskName) {
        int8_t index = findTaskIndex(taskName);
        return (index >= 0) ? _tasks[index].isActive : false;
    }

    RTOSTaskInfo CeriaModuleESP32RTOS::getTaskInfo(TaskHandle_t handle) {
        int8_t index = findTaskIndex(handle);
        if (index >= 0) {
            updateTaskInfo(index);
            return _tasks[index];
        }
        return RTOSTaskInfo{};
    }

    RTOSTaskInfo CeriaModuleESP32RTOS::getTaskInfo(const String &taskName) {
        int8_t index = findTaskIndex(taskName);
        if (index >= 0) {
            updateTaskInfo(index);
            return _tasks[index];
        }
        return RTOSTaskInfo{};
    }

    uint32_t CeriaModuleESP32RTOS::getTaskFreeStack(TaskHandle_t handle) {
        if (handle != nullptr) {
            return uxTaskGetStackHighWaterMark(handle);
        }
        return 0;
    }

    uint32_t CeriaModuleESP32RTOS::getTaskFreeStack(const String &taskName) {
        int8_t index = findTaskIndex(taskName);
        if (index >= 0) {
            return getTaskFreeStack(_tasks[index].handle);
        }
        return 0;
    }

    void CeriaModuleESP32RTOS::delay(uint32_t ms) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

    void CeriaModuleESP32RTOS::delayUntil(uint32_t &lastWakeTime, uint32_t frequency) {
        TickType_t xLastWakeTime = pdMS_TO_TICKS(lastWakeTime);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(frequency));
        lastWakeTime = xLastWakeTime * portTICK_PERIOD_MS;
    }

    uint32_t CeriaModuleESP32RTOS::getTickCount() {
        return xTaskGetTickCount();
    }

    uint32_t CeriaModuleESP32RTOS::ticksToMs(uint32_t ticks) {
        return ticks * portTICK_PERIOD_MS;
    }

    uint32_t CeriaModuleESP32RTOS::msToTicks(uint32_t ms) {
        return pdMS_TO_TICKS(ms);
    }

    RTOSMemoryInfo CeriaModuleESP32RTOS::getMemoryInfo() {
        RTOSMemoryInfo info;
        info.freeHeap = esp_get_free_heap_size();
        info.minFreeHeap = esp_get_minimum_free_heap_size();
        info.largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        info.totalHeap = heap_caps_get_total_size(MALLOC_CAP_8BIT);
        
        #ifdef CONFIG_SPIRAM_SUPPORT
        info.totalPSRAM = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
        info.freePSRAM = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        #else
        info.totalPSRAM = 0;
        info.freePSRAM = 0;
        #endif
        
        info.fragmentation = calculateFragmentation(info.freeHeap, info.largestBlock);
        
        return info;
    }

    RTOSSystemInfo CeriaModuleESP32RTOS::getSystemInfo() {
        RTOSSystemInfo info;
        info.uptime = millis();
        info.taskCount = uxTaskGetNumberOfTasks();
        info.tickCount = xTaskGetTickCount();
        info.tickRate = configTICK_RATE_HZ;
        info.version = tskKERNEL_VERSION_NUMBER;
        info.memory = getMemoryInfo();
        
        return info;
    }

    uint16_t CeriaModuleESP32RTOS::getTaskCount() {
        return uxTaskGetNumberOfTasks();
    }

    uint32_t CeriaModuleESP32RTOS::getUptime() {
        return millis();
    }

    String CeriaModuleESP32RTOS::getVersionString() {
        return "FreeRTOS v" + String(tskKERNEL_VERSION_NUMBER);
    }

    String CeriaModuleESP32RTOS::getCoreVersionString() {
        String version = "Arduino Core ";
        
        #ifdef ESP_ARDUINO_VERSION_MAJOR
            version += String(ESP_ARDUINO_VERSION_MAJOR);
            version += ".";
            version += String(ESP_ARDUINO_VERSION_MINOR);
            version += ".";
            version += String(ESP_ARDUINO_VERSION_PATCH);
        #else
            version += "2.0.x (Legacy)";
        #endif
        
        return version;
    }

    uint32_t CeriaModuleESP32RTOS::getCoreVersion() {
        return CERIA_ESP32_CORE_VERSION;
    }

    String CeriaModuleESP32RTOS::getCompatibilityString() {
        String info = "Compatibility Info:\n";
        
        #ifdef ESP_ARDUINO_VERSION_MAJOR
            info += "  Version Detection: Available\n";
            info += "  Major: " + String(ESP_ARDUINO_VERSION_MAJOR) + "\n";
            info += "  Minor: " + String(ESP_ARDUINO_VERSION_MINOR) + "\n";
            info += "  Patch: " + String(ESP_ARDUINO_VERSION_PATCH) + "\n";
            
            #if CERIA_ESP32_CORE_VERSION >= CERIA_ESP32_CORE_3_0_X
                info += "  API Level: Modern (3.0+)\n";
            #else
                info += "  API Level: Legacy (2.0.x)\n";
            #endif
        #else
            info += "  Version Detection: Not Available\n";
            info += "  API Level: Legacy (Pre-2.0 or Unknown)\n";
            info += "  Fallback Mode: Enabled\n";
        #endif
        
        info += "  tskNO_AFFINITY: " + String(RTOS_NO_AFFINITY) + "\n";
        info += "  Critical Section: Custom Mutex\n";
        
        return info;
    }

    void CeriaModuleESP32RTOS::enableMemoryMonitoring(bool enable) {
        _enableMemoryMonitoring = enable;
    }

    void CeriaModuleESP32RTOS::setMemoryThreshold(uint32_t threshold) {
        _memoryThreshold = threshold;
    }

    void CeriaModuleESP32RTOS::setFragmentationThreshold(uint8_t threshold) {
        _fragmentationThreshold = threshold;
    }

    void CeriaModuleESP32RTOS::setMemoryCallback(RTOSMemoryCallback callback) {
        _onMemoryAlert = callback;
    }

    void CeriaModuleESP32RTOS::setTaskCallback(RTOSTaskCallback callback) {
        _onTaskStateChange = callback;
    }

    bool CeriaModuleESP32RTOS::startMonitoring() {
        if (_monitoringActive) return true;
        
        BaseType_t result = xTaskCreatePinnedToCore(
            monitorTaskFunction,
            "RTOSMonitor",
            2048,
            this,
            RTOS_PRIORITY_LOW,
            &_monitorTask,
            RTOS_CORE_0
        );
        
        if (result == pdPASS) {
            _monitoringActive = true;
            return true;
        }
        return false;
    }

    void CeriaModuleESP32RTOS::stopMonitoring() {
        if (_monitoringActive && _monitorTask != nullptr) {
            vTaskDelete(_monitorTask);
            _monitorTask = nullptr;
            _monitoringActive = false;
        }
    }

    bool CeriaModuleESP32RTOS::isMonitoringActive() {
        return _monitoringActive;
    }

    String CeriaModuleESP32RTOS::getTaskListString() {
        String result = "Tasks (" + String(_taskCount) + "):\n";
        for (uint8_t i = 0; i < _taskCount; i++) {
            if (_tasks[i].isActive) {
                result += "  " + _tasks[i].name;
                result += " | P:" + String(_tasks[i].priority);
                result += " | C:" + String(_tasks[i].coreId);
                result += " | S:" + String(_tasks[i].freeStack);
                result += " | State:" + String(_tasks[i].state);
                result += "\n";
            }
        }
        return result;
    }

    String CeriaModuleESP32RTOS::getMemoryString() {
        RTOSMemoryInfo info = getMemoryInfo();
        String result = "Memory:\n";
        result += "  Free: " + String(info.freeHeap) + " bytes\n";
        result += "  Min: " + String(info.minFreeHeap) + " bytes\n";
        result += "  Largest: " + String(info.largestBlock) + " bytes\n";
        result += "  Fragmentation: " + String(info.fragmentation) + "%\n";
        if (info.totalPSRAM > 0) {
            result += "  PSRAM: " + String(info.freePSRAM) + "/" + String(info.totalPSRAM) + " bytes\n";
        }
        return result;
    }

    String CeriaModuleESP32RTOS::getSystemString() {
        RTOSSystemInfo info = getSystemInfo();
        String result = "System:\n";
        result += "  FreeRTOS: " + info.version + "\n";
        result += "  Arduino Core: " + getCoreVersionString() + "\n";
        result += "  Uptime: " + String(info.uptime / 1000) + "s\n";
        result += "  Tasks: " + String(info.taskCount) + "\n";
        result += "  Tick Rate: " + String(info.tickRate) + " Hz\n";
        return result;
    }

    String CeriaModuleESP32RTOS::getRuntimeStatsString() {
        return "Runtime stats available via vTaskGetRunTimeStats()";
    }

    void CeriaModuleESP32RTOS::printTaskList() {
        Serial.print(getTaskListString());
    }

    void CeriaModuleESP32RTOS::printMemoryInfo() {
        Serial.print(getMemoryString());
    }

    void CeriaModuleESP32RTOS::printSystemInfo() {
        Serial.print(getSystemString());
    }

    void CeriaModuleESP32RTOS::printRuntimeStats() {
        Serial.println(getRuntimeStatsString());
    }

    void CeriaModuleESP32RTOS::taskDelay(uint32_t ms) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

    uint32_t CeriaModuleESP32RTOS::getCurrentTick() {
        return xTaskGetTickCount();
    }

    BaseType_t CeriaModuleESP32RTOS::getCurrentCore() {
        return xPortGetCoreID();
    }

    void CeriaModuleESP32RTOS::enterCritical() {
        portENTER_CRITICAL(&_criticalMux);
    }

    void CeriaModuleESP32RTOS::exitCritical() {
        portEXIT_CRITICAL(&_criticalMux);
    }

    void CeriaModuleESP32RTOS::updateTaskInfo(uint8_t index) {
        if (index >= _taskCount || !_tasks[index].isActive) return;
        
        TaskHandle_t handle = _tasks[index].handle;
        if (handle != nullptr) {
            _tasks[index].freeStack = uxTaskGetStackHighWaterMark(handle);
            
            eTaskState taskState = eTaskGetState(handle);
            switch (taskState) {
                case eRunning:
                    _tasks[index].state = RTOS_TASK_RUNNING;
                    break;
                case eReady:
                    _tasks[index].state = RTOS_TASK_READY;
                    break;
                case eBlocked:
                    _tasks[index].state = RTOS_TASK_BLOCKED;
                    break;
                case eSuspended:
                    _tasks[index].state = RTOS_TASK_SUSPENDED;
                    break;
                case eDeleted:
                    _tasks[index].state = RTOS_TASK_DELETED;
                    _tasks[index].isActive = false;
                    break;
            }
        }
    }

    void CeriaModuleESP32RTOS::updateMemoryInfo() {
        RTOSMemoryInfo info = getMemoryInfo();
        
        if (info.freeHeap < _memoryThreshold || info.fragmentation > _fragmentationThreshold) {
            triggerMemoryAlert(info.freeHeap, info.fragmentation);
        }
    }

    void CeriaModuleESP32RTOS::triggerMemoryAlert(uint32_t freeHeap, uint8_t fragmentation) {
        if (_onMemoryAlert) {
            _onMemoryAlert(freeHeap, fragmentation);
        }
    }

    void CeriaModuleESP32RTOS::triggerTaskStateChange(const String &taskName, RTOSTaskState state) {
        if (_onTaskStateChange) {
            _onTaskStateChange(taskName, state);
        }
    }

    uint8_t CeriaModuleESP32RTOS::calculateFragmentation(uint32_t freeHeap, uint32_t largestBlock) {
        if (freeHeap == 0) return 100;
        return 100 - ((largestBlock * 100) / freeHeap);
    }

    void CeriaModuleESP32RTOS::monitorTaskFunction(void *parameter) {
        CeriaModuleESP32RTOS *rtos = static_cast<CeriaModuleESP32RTOS*>(parameter);
        
        while (rtos->_monitoringActive) {
            rtos->update();
            vTaskDelay(pdMS_TO_TICKS(rtos->_updateInterval));
        }
        
        vTaskDelete(nullptr);
    }

    int8_t CeriaModuleESP32RTOS::findTaskIndex(TaskHandle_t handle) {
        for (uint8_t i = 0; i < _taskCount; i++) {
            if (_tasks[i].handle == handle && _tasks[i].isActive) {
                return i;
            }
        }
        return -1;
    }

    int8_t CeriaModuleESP32RTOS::findTaskIndex(const String &name) {
        for (uint8_t i = 0; i < _taskCount; i++) {
            if (_tasks[i].name == name && _tasks[i].isActive) {
                return i;
            }
        }
        return -1;
    }

}

#endif