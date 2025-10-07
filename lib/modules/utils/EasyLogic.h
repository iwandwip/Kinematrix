#ifndef EASY_LOGIC_H
#define EASY_LOGIC_H

#include "Arduino.h"
#include "LogicConditionManager.h"

class EasyLogic {
private:
    LogicConditionManager *core;

public:
    class ConditionBuilder {
    private:
        EasyLogic *parent;
        String name;

        bool (*boolFunc)();
        bool *boolPtr;
        float (*valueFunc)();
        float *valuePtr;
        float threshold;
        float secondThreshold;
        String comparison;

        LogicConditionManager::TriggerType trigger;
        void (*action)();
        void (*secondaryAction)();
        void (*stopAction)();
        void (*paramAction)(void *);
        void *actionParam;
        void (*initAction)();
        void (*cleanupAction)();
        void (*errorHandler)();
        void (*timeoutAction)();

        bool (*guardCondition)();
        bool (*unlessCondition)();
        float (*transformer)(float);
        bool (*validator)(float);

        int repeatCount;
        unsigned long interval;
        unsigned long debounceTime;
        unsigned long delayTime;
        unsigned long actionDuration;
        unsigned long timeoutDuration;
        unsigned long waitTime;
        unsigned long precisionTime;
        unsigned long jitterTime;

        int consecutiveCount;
        unsigned long stableDuration;
        float rateThreshold;
        float hysteresis;
        float deadzone;
        float smoothFactor;
        int averageSamples;
        float normalizeMin;
        float normalizeMax;
        float clampMin;
        float clampMax;

        int startHour;
        int endHour;
        bool weekdaysOnly;
        bool weekendsOnly;
        bool loggingEnabled;
        bool metricsEnabled;
        String debugName;
        int reportInterval;
        unsigned long watchdogTimeout;

        bool useTransform;
        bool useFilter;
        bool useSmooth;
        bool useAverage;
        bool useNormalize;
        bool useClamp;
        bool useGuard;
        bool useUnless;
        bool useTimeRange;
        bool useConsecutive;
        bool useStable;
        bool useHysteresis;
        bool useDeadzone;

    public:
        ConditionBuilder(EasyLogic *p, const String &n);

        ConditionBuilder &when(bool (*condition)());
        ConditionBuilder &when(bool *condition);
        ConditionBuilder &when(float (*getValue)(), float thresh, const String &comp = ">");
        ConditionBuilder &when(float *valuePtr, float thresh, const String &comp = ">");
        ConditionBuilder &when(float (*getValue)());
        ConditionBuilder &when(float *valuePtr);

        ConditionBuilder &greaterThan(float value);
        ConditionBuilder &greaterOrEqual(float value);
        ConditionBuilder &lessThan(float value);
        ConditionBuilder &lessOrEqual(float value);
        ConditionBuilder &equalTo(float value);
        ConditionBuilder &notEqualTo(float value);
        ConditionBuilder &between(float min, float max);
        ConditionBuilder &outside(float min, float max);
        ConditionBuilder &approximately(float value, float tolerance = 0.1);
        ConditionBuilder &percentageAbove(float baseValue, float percentage);
        ConditionBuilder &percentageBelow(float baseValue, float percentage);
        ConditionBuilder &deviatesFrom(float reference, float deviation);

        ConditionBuilder &onTrue();
        ConditionBuilder &onFalse();
        ConditionBuilder &onChange();
        ConditionBuilder &onRising();
        ConditionBuilder &onFalling();
        ConditionBuilder &whenBecomesTrue();
        ConditionBuilder &whenBecomesFalse();
        ConditionBuilder &whenChanges();
        ConditionBuilder &whileTrue();
        ConditionBuilder &whileFalse();

        ConditionBuilder &whenExceeds(float thresh);
        ConditionBuilder &whenDropsBelow(float thresh);
        ConditionBuilder &whenReturns();
        ConditionBuilder &whenCrosses(float thresh);
        ConditionBuilder &whenStays(float thresh, const String &comp = ">");

        ConditionBuilder &afterConsecutive(int count);
        ConditionBuilder &afterStable(unsigned long duration);
        ConditionBuilder &onRateOfChange(float threshold);
        ConditionBuilder &withHysteresis(float amount);
        ConditionBuilder &withDeadzone(float zone);

        ConditionBuilder &then(void (*actionFunc)());
        ConditionBuilder &then(void (*actionFunc)(void *), void *param);
        ConditionBuilder &thenAlso(void (*secondaryFunc)());
        ConditionBuilder &thenWith(void (*actionFunc)(void *), void *param);
        ConditionBuilder &andThenStop(void (*stopFunc)());
        ConditionBuilder &onStart(void (*initFunc)());
        ConditionBuilder &onComplete(void (*cleanupFunc)());
        ConditionBuilder &onError(void (*errorFunc)());
        ConditionBuilder &onTimeout(void (*timeoutFunc)());

        ConditionBuilder &onlyIf(bool (*guard)());
        ConditionBuilder &unless(bool (*guard)());
        ConditionBuilder &during(int startHr, int endHr);
        ConditionBuilder &onWeekdays();
        ConditionBuilder &onWeekends();

        ConditionBuilder &transform(float (*transformer)(float));
        ConditionBuilder &filter(bool (*validator)(float));
        ConditionBuilder &smooth(float factor);
        ConditionBuilder &average(int samples);
        ConditionBuilder &normalize(float min, float max);
        ConditionBuilder &clamp(float min, float max);

        ConditionBuilder &repeat(int times = -1);
        ConditionBuilder &every(unsigned long ms);
        ConditionBuilder &debounce(unsigned long ms);
        ConditionBuilder &delay(unsigned long ms);
        ConditionBuilder &duration(unsigned long ms);
        ConditionBuilder &timeout(unsigned long ms);
        ConditionBuilder &precision(unsigned long ms);
        ConditionBuilder &jitter(unsigned long maxMs);
        ConditionBuilder &waitAfter(unsigned long ms);

        ConditionBuilder &withLogging(bool enable = true);
        ConditionBuilder &withMetrics(bool enable = true);
        ConditionBuilder &withName(const String &debugName);
        ConditionBuilder &reportEvery(int executions);
        ConditionBuilder &withWatchdog(unsigned long timeoutMs);

        int build();
    };

    class CompositeBuilder {
    private:
        EasyLogic *parent;
        String name;
        int *conditionIds;
        int count;
        int capacity;

    public:
        CompositeBuilder(EasyLogic *p, const String &n);
        ~CompositeBuilder();

        CompositeBuilder &add(int conditionId);
        CompositeBuilder &add(const String &conditionName);

        int asAND(void (*action)() = nullptr);
        int asOR(void (*action)() = nullptr);
        int asNOT(void (*action)() = nullptr);
        int asXOR(void (*action)() = nullptr);
    };

    EasyLogic(int capacity = 20);
    ~EasyLogic();

    int button(const String &name, bool (*condition)(), void (*action)() = nullptr);
    int button(const String &name, bool *condition, void (*action)() = nullptr);

    int sensor(const String &name, float (*getValue)(), float threshold,
               void (*action)() = nullptr, const String &comparison = ">");
    int sensor(const String &name, float *valuePtr, float threshold,
               void (*action)() = nullptr, const String &comparison = ">");

    int timer(const String &name, unsigned long intervalMs, void (*action)(),
              bool repeat = false);

    int toggle(const String &name, bool (*condition)(),
               void (*onAction)() = nullptr, void (*offAction)() = nullptr);
    int toggle(const String &name, bool *condition,
               void (*onAction)() = nullptr, void (*offAction)() = nullptr);

    ConditionBuilder create(const String &name);
    CompositeBuilder combine(const String &name);

    void run();

    void enable(const String &name);
    void disable(const String &name);
    void pause(const String &name);
    void resume(const String &name);
    void reset(const String &name);
    bool remove(const String &name);

    bool isActive(const String &name);
    bool isActive(int conditionId);
    int executions(const String &name);
    int executions(int conditionId);
    unsigned long lastTrigger(const String &name);
    unsigned long lastTrigger(int conditionId);

    void status();
    void status(const String &name);
    void status(int conditionId);

    bool exists(const String &name);

    LogicConditionManager *advanced();
};

#endif