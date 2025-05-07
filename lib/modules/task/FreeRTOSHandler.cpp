#include "FreeRTOSHandler.h"

FreeRTOSHandler::FreeRTOSHandler() {
}

FreeRTOSHandler::~FreeRTOSHandler() {
}

FreeRTOSHandler::Task::Task() : handle(NULL) {
}

FreeRTOSHandler::Task::~Task() {
    if (handle != NULL) {
        vTaskDelete(handle);
        handle = NULL;
    }
}

bool FreeRTOSHandler::Task::create(void (*taskFunction)(void *), const char *taskName,
                                   uint32_t stackSize, UBaseType_t priority, BaseType_t core) {
    if (core >= 0) {
        return xTaskCreatePinnedToCore(
                taskFunction,
                taskName,
                stackSize,
                NULL,
                priority,
                &handle,
                core
        ) == pdPASS;
    } else {
        return xTaskCreate(
                taskFunction,
                taskName,
                stackSize,
                NULL,
                priority,
                &handle
        ) == pdPASS;
    }
}

void FreeRTOSHandler::Task::suspend() {
    if (handle != NULL) {
        vTaskSuspend(handle);
    }
}

void FreeRTOSHandler::Task::resume() {
    if (handle != NULL) {
        vTaskResume(handle);
    }
}

void FreeRTOSHandler::Task::remove() {
    if (handle != NULL) {
        vTaskDelete(handle);
        handle = NULL;
    }
}

bool FreeRTOSHandler::Task::isRunning() {
    return handle != NULL && eTaskGetState(handle) != eSuspended;
}

uint32_t FreeRTOSHandler::Task::getStackHighWaterMark() {
    if (handle != NULL) {
        return uxTaskGetStackHighWaterMark(handle);
    }
    return 0;
}

TaskHandle_t FreeRTOSHandler::Task::getHandle() {
    return handle;
}

FreeRTOSHandler::Mutex::Mutex() : handle(NULL) {
}

FreeRTOSHandler::Mutex::~Mutex() {
    if (handle != NULL) {
        vSemaphoreDelete(handle);
        handle = NULL;
    }
}

bool FreeRTOSHandler::Mutex::create() {
    handle = xSemaphoreCreateMutex();
    return handle != NULL;
}

bool FreeRTOSHandler::Mutex::createRecursive() {
    handle = xSemaphoreCreateRecursiveMutex();
    return handle != NULL;
}

bool FreeRTOSHandler::Mutex::lock(uint32_t timeout) {
    if (handle != NULL) {
        return xSemaphoreTake(handle, timeout) == pdTRUE;
    }
    return false;
}

bool FreeRTOSHandler::Mutex::tryLock() {
    if (handle != NULL) {
        return xSemaphoreTake(handle, 0) == pdTRUE;
    }
    return false;
}

void FreeRTOSHandler::Mutex::unlock() {
    if (handle != NULL) {
        xSemaphoreGive(handle);
    }
}

SemaphoreHandle_t FreeRTOSHandler::Mutex::getHandle() {
    return handle;
}

FreeRTOSHandler::Semaphore::Semaphore() : handle(NULL) {
}

FreeRTOSHandler::Semaphore::~Semaphore() {
    if (handle != NULL) {
        vSemaphoreDelete(handle);
        handle = NULL;
    }
}

bool FreeRTOSHandler::Semaphore::createBinary() {
    handle = xSemaphoreCreateBinary();
    return handle != NULL;
}

bool FreeRTOSHandler::Semaphore::createCounting(UBaseType_t maxCount, UBaseType_t initialCount) {
    handle = xSemaphoreCreateCounting(maxCount, initialCount);
    return handle != NULL;
}

bool FreeRTOSHandler::Semaphore::take(uint32_t timeout) {
    if (handle != NULL) {
        return xSemaphoreTake(handle, timeout) == pdTRUE;
    }
    return false;
}

bool FreeRTOSHandler::Semaphore::tryTake() {
    if (handle != NULL) {
        return xSemaphoreTake(handle, 0) == pdTRUE;
    }
    return false;
}

bool FreeRTOSHandler::Semaphore::give() {
    if (handle != NULL) {
        return xSemaphoreGive(handle) == pdTRUE;
    }
    return false;
}

bool FreeRTOSHandler::Semaphore::giveFromISR(BaseType_t *pxHigherPriorityTaskWoken) {
    if (handle != NULL) {
        return xSemaphoreGiveFromISR(handle, pxHigherPriorityTaskWoken) == pdTRUE;
    }
    return false;
}

SemaphoreHandle_t FreeRTOSHandler::Semaphore::getHandle() {
    return handle;
}

FreeRTOSHandler::Timer::Timer() : handle(NULL) {
}

FreeRTOSHandler::Timer::~Timer() {
    if (handle != NULL) {
        xTimerDelete(handle, portMAX_DELAY);
        handle = NULL;
    }
}

bool FreeRTOSHandler::Timer::create(const char *timerName, uint32_t period, bool autoReload,
                                    void *timerID, TimerCallbackFunction_t callback) {
    handle = xTimerCreate(
            timerName,
            pdMS_TO_TICKS(period),
            autoReload ? pdTRUE : pdFALSE,
            timerID,
            callback
    );
    return handle != NULL;
}

bool FreeRTOSHandler::Timer::start(uint32_t timeout) {
    if (handle != NULL) {
        return xTimerStart(handle, timeout) == pdPASS;
    }
    return false;
}

bool FreeRTOSHandler::Timer::stop(uint32_t timeout) {
    if (handle != NULL) {
        return xTimerStop(handle, timeout) == pdPASS;
    }
    return false;
}

bool FreeRTOSHandler::Timer::reset(uint32_t timeout) {
    if (handle != NULL) {
        return xTimerReset(handle, timeout) == pdPASS;
    }
    return false;
}

bool FreeRTOSHandler::Timer::changePeriod(uint32_t newPeriod, uint32_t timeout) {
    if (handle != NULL) {
        return xTimerChangePeriod(handle, pdMS_TO_TICKS(newPeriod), timeout) == pdPASS;
    }
    return false;
}

bool FreeRTOSHandler::Timer::isActive() {
    if (handle != NULL) {
        return xTimerIsTimerActive(handle) == pdTRUE;
    }
    return false;
}

void *FreeRTOSHandler::Timer::getTimerID() {
    if (handle != NULL) {
        return pvTimerGetTimerID(handle);
    }
    return NULL;
}

void FreeRTOSHandler::Timer::setTimerID(void *id) {
    if (handle != NULL) {
        vTimerSetTimerID(handle, id);
    }
}

TimerHandle_t FreeRTOSHandler::Timer::getHandle() {
    return handle;
}

FreeRTOSHandler::EventGroup::EventGroup() : handle(NULL) {
}

FreeRTOSHandler::EventGroup::~EventGroup() {
    if (handle != NULL) {
        vEventGroupDelete(handle);
        handle = NULL;
    }
}

bool FreeRTOSHandler::EventGroup::create() {
    handle = xEventGroupCreate();
    return handle != NULL;
}

EventBits_t FreeRTOSHandler::EventGroup::setBits(const EventBits_t bitsToSet) {
    if (handle != NULL) {
        return xEventGroupSetBits(handle, bitsToSet);
    }
    return 0;
}

EventBits_t FreeRTOSHandler::EventGroup::setBitsFromISR(const EventBits_t bitsToSet, BaseType_t *pxHigherPriorityTaskWoken) {
    if (handle != NULL) {
        return xEventGroupSetBitsFromISR(handle, bitsToSet, pxHigherPriorityTaskWoken);
    }
    return 0;
}

EventBits_t FreeRTOSHandler::EventGroup::clearBits(const EventBits_t bitsToClear) {
    if (handle != NULL) {
        return xEventGroupClearBits(handle, bitsToClear);
    }
    return 0;
}

EventBits_t FreeRTOSHandler::EventGroup::clearBitsFromISR(const EventBits_t bitsToClear) {
    if (handle != NULL) {
        return xEventGroupClearBitsFromISR(handle, bitsToClear);
    }
    return 0;
}

EventBits_t FreeRTOSHandler::EventGroup::waitBits(const EventBits_t bitsToWaitFor, bool clearOnExit,
                                                  bool waitForAllBits, uint32_t timeout) {
    if (handle != NULL) {
        return xEventGroupWaitBits(
                handle,
                bitsToWaitFor,
                clearOnExit ? pdTRUE : pdFALSE,
                waitForAllBits ? pdTRUE : pdFALSE,
                timeout
        );
    }
    return 0;
}

EventBits_t FreeRTOSHandler::EventGroup::getBits() {
    if (handle != NULL) {
        return xEventGroupGetBits(handle);
    }
    return 0;
}

EventGroupHandle_t FreeRTOSHandler::EventGroup::getHandle() {
    return handle;
}