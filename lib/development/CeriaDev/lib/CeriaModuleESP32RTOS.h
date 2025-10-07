#ifndef CERIA_MODULE_ESP32_RTOS_H
#define CERIA_MODULE_ESP32_RTOS_H

#pragma message("[COMPILED]: CeriaModuleESP32RTOS.h")

#include "Arduino.h"

#ifdef ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_heap_caps.h"

#ifdef ESP_ARDUINO_VERSION_MAJOR
  #ifndef ESP_ARDUINO_VERSION_VAL
    #define ESP_ARDUINO_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))
  #endif
  
  #define CERIA_ESP32_CORE_VERSION ESP_ARDUINO_VERSION_VAL(ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH)
  #define CERIA_ESP32_CORE_VERSION_MAJOR ESP_ARDUINO_VERSION_MAJOR
  #define CERIA_ESP32_CORE_VERSION_MINOR ESP_ARDUINO_VERSION_MINOR
  #define CERIA_ESP32_CORE_VERSION_PATCH ESP_ARDUINO_VERSION_PATCH
#else
  #define CERIA_ESP32_CORE_VERSION 200000
  #define CERIA_ESP32_CORE_VERSION_MAJOR 2
  #define CERIA_ESP32_CORE_VERSION_MINOR 0
  #define CERIA_ESP32_CORE_VERSION_PATCH 0
#endif

#define CERIA_ESP32_CORE_2_0_X 200000
#define CERIA_ESP32_CORE_3_0_X 300000

namespace CeriaDevOP {

    enum RTOSTaskCore : int32_t {
        RTOS_CORE_0 = 0,
        RTOS_CORE_1 = 1,
        RTOS_NO_AFFINITY = tskNO_AFFINITY
    };

    enum RTOSTaskPriority : uint8_t {
        RTOS_PRIORITY_IDLE = 0,
        RTOS_PRIORITY_LOW = 1,
        RTOS_PRIORITY_NORMAL = 5,
        RTOS_PRIORITY_HIGH = 10,
        RTOS_PRIORITY_CRITICAL = 20,
        RTOS_PRIORITY_MAXIMUM = 24
    };

    enum RTOSTaskState : uint8_t {
        RTOS_TASK_RUNNING = 0,
        RTOS_TASK_READY = 1,
        RTOS_TASK_BLOCKED = 2,
        RTOS_TASK_SUSPENDED = 3,
        RTOS_TASK_DELETED = 4
    };

    struct RTOSTaskInfo {
        TaskHandle_t handle;
        String name;
        uint32_t stackSize;
        UBaseType_t priority;
        BaseType_t coreId;
        RTOSTaskState state;
        uint32_t freeStack;
        uint32_t runtime;
        bool isActive;
    };

    struct RTOSMemoryInfo {
        uint32_t totalHeap;
        uint32_t freeHeap;
        uint32_t minFreeHeap;
        uint32_t largestBlock;
        uint32_t totalPSRAM;
        uint32_t freePSRAM;
        uint8_t fragmentation;
    };

    struct RTOSSystemInfo {
        uint32_t uptime;
        uint16_t taskCount;
        uint32_t tickCount;
        uint32_t tickRate;
        String version;
        RTOSMemoryInfo memory;
    };

    typedef void (*RTOSTaskFunction)(void *parameter);
    typedef void (*RTOSMemoryCallback)(uint32_t freeHeap, uint8_t fragmentation);
    typedef void (*RTOSTaskCallback)(const String &taskName, RTOSTaskState state);

    class CeriaModuleESP32RTOS {
    private:
        static const uint8_t MAX_TASKS = 16;
        
        RTOSTaskInfo _tasks[MAX_TASKS];
        uint8_t _taskCount;
        
        bool _hasNewData;
        uint32_t _lastUpdateTime;
        uint32_t _updateInterval;
        
        RTOSMemoryCallback _onMemoryAlert;
        RTOSTaskCallback _onTaskStateChange;
        
        uint32_t _memoryThreshold;
        uint8_t _fragmentationThreshold;
        bool _enableMemoryMonitoring;
        
        TaskHandle_t _monitorTask;
        bool _monitoringActive;
        
        static portMUX_TYPE _criticalMux;
        
        void updateTaskInfo(uint8_t index);
        void updateMemoryInfo();
        void triggerMemoryAlert(uint32_t freeHeap, uint8_t fragmentation);
        void triggerTaskStateChange(const String &taskName, RTOSTaskState state);
        uint8_t calculateFragmentation(uint32_t freeHeap, uint32_t largestBlock);
        
        static void monitorTaskFunction(void *parameter);
        int8_t findTaskIndex(TaskHandle_t handle);
        int8_t findTaskIndex(const String &name);

    public:
        CeriaModuleESP32RTOS();
        ~CeriaModuleESP32RTOS();

        void begin(uint32_t updateInterval = 1000);
        void update();
        void tick() { update(); }

        bool hasNewData();
        void setUpdateInterval(uint32_t interval);

        TaskHandle_t createTask(
            RTOSTaskFunction taskFunction,
            const String &taskName,
            uint32_t stackSize = 2048,
            void *parameters = nullptr,
            RTOSTaskPriority priority = RTOS_PRIORITY_NORMAL,
            RTOSTaskCore core = RTOS_NO_AFFINITY
        );

        bool deleteTask(TaskHandle_t handle);
        bool deleteTask(const String &taskName);
        bool suspendTask(TaskHandle_t handle);
        bool suspendTask(const String &taskName);
        bool resumeTask(TaskHandle_t handle);
        bool resumeTask(const String &taskName);

        bool isTaskActive(TaskHandle_t handle);
        bool isTaskActive(const String &taskName);
        RTOSTaskInfo getTaskInfo(TaskHandle_t handle);
        RTOSTaskInfo getTaskInfo(const String &taskName);
        uint32_t getTaskFreeStack(TaskHandle_t handle);
        uint32_t getTaskFreeStack(const String &taskName);

        void delay(uint32_t ms);
        void delayUntil(uint32_t &lastWakeTime, uint32_t frequency);
        uint32_t getTickCount();
        uint32_t ticksToMs(uint32_t ticks);
        uint32_t msToTicks(uint32_t ms);

        RTOSMemoryInfo getMemoryInfo();
        RTOSSystemInfo getSystemInfo();
        uint16_t getTaskCount();
        uint32_t getUptime();
        String getVersionString();
        String getCoreVersionString();
        uint32_t getCoreVersion();
        String getCompatibilityString();

        void enableMemoryMonitoring(bool enable = true);
        void setMemoryThreshold(uint32_t threshold);
        void setFragmentationThreshold(uint8_t threshold);
        void setMemoryCallback(RTOSMemoryCallback callback);
        void setTaskCallback(RTOSTaskCallback callback);

        bool startMonitoring();
        void stopMonitoring();
        bool isMonitoringActive();

        String getTaskListString();
        String getMemoryString();
        String getSystemString();
        String getRuntimeStatsString();

        void printTaskList();
        void printMemoryInfo();
        void printSystemInfo();
        void printRuntimeStats();

        static void taskDelay(uint32_t ms);
        static uint32_t getCurrentTick();
        static BaseType_t getCurrentCore();
        static void enterCritical();
        static void exitCritical();
    };

#if defined(ESP32)
#define RTOS_DEFAULT_STACK_SIZE    2048
#define RTOS_DEFAULT_PRIORITY      RTOS_PRIORITY_NORMAL
#define RTOS_DEFAULT_CORE          RTOS_NO_AFFINITY
#define RTOS_MEMORY_THRESHOLD      10240
#define RTOS_FRAGMENTATION_THRESHOLD 80
#else
#define RTOS_DEFAULT_STACK_SIZE    1024
#define RTOS_DEFAULT_PRIORITY      RTOS_PRIORITY_NORMAL
#define RTOS_DEFAULT_CORE          0
#define RTOS_MEMORY_THRESHOLD      2048
#define RTOS_FRAGMENTATION_THRESHOLD 70
#endif

}

#endif

#endif