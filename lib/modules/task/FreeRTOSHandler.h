#ifndef FREERTOS_HANDLER_H
#define FREERTOS_HANDLER_H

#include <Arduino.h>

class FreeRTOSHandler {
public:
    FreeRTOSHandler();
    ~FreeRTOSHandler();

    class Task {
    private:
        TaskHandle_t handle;

    public:
        Task();
        ~Task();

        template<class T>
        bool create(void (*taskFunction)(T), const char *taskName, uint32_t stackSize, T parameter,
                    UBaseType_t priority, BaseType_t core = -1) {
            struct TaskData {
                void (*function)(T);
                T param;
            };

            TaskData *data = new TaskData{taskFunction, parameter};

            if (core >= 0) {
                return xTaskCreatePinnedToCore(
                        [](void *pvParams) {
                            TaskData *d = static_cast<TaskData *>(pvParams);
                            d->function(d->param);
                            delete d;
                        },
                        taskName,
                        stackSize,
                        data,
                        priority,
                        &handle,
                        core
                ) == pdPASS;
            } else {
                return xTaskCreate(
                        [](void *pvParams) {
                            TaskData *d = static_cast<TaskData *>(pvParams);
                            d->function(d->param);
                            delete d;
                        },
                        taskName,
                        stackSize,
                        data,
                        priority,
                        &handle
                ) == pdPASS;
            }
        }

        bool create(void (*taskFunction)(void *), const char *taskName, uint32_t stackSize,
                    UBaseType_t priority, BaseType_t core = -1);

        void suspend();
        void resume();
        void remove();

        bool isRunning();
        uint32_t getStackHighWaterMark();
        TaskHandle_t getHandle();
    };

    class Mutex {
    private:
        SemaphoreHandle_t handle;

    public:
        Mutex();
        ~Mutex();

        bool create();
        bool createRecursive();

        bool lock(uint32_t timeout = portMAX_DELAY);
        bool tryLock();
        void unlock();

        SemaphoreHandle_t getHandle();
    };

    class Semaphore {
    private:
        SemaphoreHandle_t handle;

    public:
        Semaphore();
        ~Semaphore();

        bool createBinary();
        bool createCounting(UBaseType_t maxCount, UBaseType_t initialCount);

        bool take(uint32_t timeout = portMAX_DELAY);
        bool tryTake();
        bool give();
        bool giveFromISR(BaseType_t *pxHigherPriorityTaskWoken = NULL);

        SemaphoreHandle_t getHandle();
    };

    template<typename T>
    class Queue {
    private:
        QueueHandle_t handle;

    public:
        Queue() : handle(NULL) {}

        ~Queue() {
            if (handle != NULL) {
                vQueueDelete(handle);
                handle = NULL;
            }
        }

        bool create(UBaseType_t length) {
            handle = xQueueCreate(length, sizeof(T));
            return handle != NULL;
        }

        bool send(const T &item, uint32_t timeout = portMAX_DELAY) {
            return xQueueSend(handle, &item, timeout) == pdTRUE;
        }

        bool sendToFront(const T &item, uint32_t timeout = portMAX_DELAY) {
            return xQueueSendToFront(handle, &item, timeout) == pdTRUE;
        }

        bool sendToBack(const T &item, uint32_t timeout = portMAX_DELAY) {
            return xQueueSendToBack(handle, &item, timeout) == pdTRUE;
        }

        bool sendFromISR(const T &item, BaseType_t *pxHigherPriorityTaskWoken = NULL) {
            return xQueueSendFromISR(handle, &item, pxHigherPriorityTaskWoken) == pdTRUE;
        }

        bool receive(T &item, uint32_t timeout = portMAX_DELAY) {
            return xQueueReceive(handle, &item, timeout) == pdTRUE;
        }

        bool peek(T &item, uint32_t timeout = portMAX_DELAY) {
            return xQueuePeek(handle, &item, timeout) == pdTRUE;
        }

        bool receiveFromISR(T &item, BaseType_t *pxHigherPriorityTaskWoken = NULL) {
            return xQueueReceiveFromISR(handle, &item, pxHigherPriorityTaskWoken) == pdTRUE;
        }

        bool isEmpty() {
            return uxQueueMessagesWaiting(handle) == 0;
        }

        bool isFull() {
            return uxQueueSpacesAvailable(handle) == 0;
        }

        UBaseType_t messagesWaiting() {
            return uxQueueMessagesWaiting(handle);
        }

        UBaseType_t spacesAvailable() {
            return uxQueueSpacesAvailable(handle);
        }

        void reset() {
            xQueueReset(handle);
        }

        QueueHandle_t getHandle() {
            return handle;
        }
    };

    class Timer {
    private:
        TimerHandle_t handle;

    public:
        Timer();
        ~Timer();

        bool create(const char *timerName, uint32_t period, bool autoReload,
                    void *timerID, TimerCallbackFunction_t callback);

        bool start(uint32_t timeout = portMAX_DELAY);
        bool stop(uint32_t timeout = portMAX_DELAY);
        bool reset(uint32_t timeout = portMAX_DELAY);
        bool changePeriod(uint32_t newPeriod, uint32_t timeout = portMAX_DELAY);

        bool isActive();
        void *getTimerID();
        void setTimerID(void *id);

        TimerHandle_t getHandle();
    };

    class EventGroup {
    private:
        EventGroupHandle_t handle;

    public:
        EventGroup();
        ~EventGroup();

        bool create();

        EventBits_t setBits(const EventBits_t bitsToSet);
        EventBits_t setBitsFromISR(const EventBits_t bitsToSet, BaseType_t *pxHigherPriorityTaskWoken = NULL);
        EventBits_t clearBits(const EventBits_t bitsToClear);
        EventBits_t clearBitsFromISR(const EventBits_t bitsToClear);

        EventBits_t waitBits(const EventBits_t bitsToWaitFor, bool clearOnExit,
                             bool waitForAllBits, uint32_t timeout = portMAX_DELAY);

        EventBits_t getBits();

        EventGroupHandle_t getHandle();
    };

    static void delayUntil(uint32_t *previousWakeTime, uint32_t timeIncrement) {
        vTaskDelayUntil(previousWakeTime, pdMS_TO_TICKS(timeIncrement));
    }

    static void delay(uint32_t ms) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

    static uint32_t getTickCount() {
        return xTaskGetTickCount();
    }

    static void startScheduler() {
        vTaskStartScheduler();
    }

    static void suspendAll() {
        vTaskSuspendAll();
    }

    static bool resumeAll() {
        return xTaskResumeAll() == pdTRUE;
    }

    static void yield() {
        taskYIELD();
    }
};

#endif