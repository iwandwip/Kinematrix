#include "EasyLogic.h"

EasyLogic::ConditionBuilder::ConditionBuilder(EasyLogic *p, const String &n)
        : parent(p), name(n), boolFunc(nullptr), boolPtr(nullptr), valueFunc(nullptr),
          valuePtr(nullptr), threshold(0), secondThreshold(0), comparison(">"),
          trigger(LogicConditionManager::WHEN_TRUE), action(nullptr), secondaryAction(nullptr),
          stopAction(nullptr), paramAction(nullptr), actionParam(nullptr), initAction(nullptr),
          cleanupAction(nullptr), errorHandler(nullptr), timeoutAction(nullptr),
          guardCondition(nullptr), unlessCondition(nullptr), transformer(nullptr), validator(nullptr),
          repeatCount(1), interval(0), debounceTime(0), delayTime(0), actionDuration(0),
          timeoutDuration(0), waitTime(0), precisionTime(0), jitterTime(0),
          consecutiveCount(1), stableDuration(0), rateThreshold(0), hysteresis(0),
          deadzone(0), smoothFactor(0), averageSamples(1), normalizeMin(0), normalizeMax(1),
          clampMin(0), clampMax(100), startHour(0), endHour(23), weekdaysOnly(false),
          weekendsOnly(false), loggingEnabled(false), metricsEnabled(false), reportInterval(0),
          watchdogTimeout(0), useTransform(false), useFilter(false), useSmooth(false),
          useAverage(false), useNormalize(false), useClamp(false), useGuard(false),
          useUnless(false), useTimeRange(false), useConsecutive(false), useStable(false),
          useHysteresis(false), useDeadzone(false) {
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::when(bool (*condition)()) {
    boolFunc = condition;
    boolPtr = nullptr;
    valueFunc = nullptr;
    valuePtr = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::when(bool *condition) {
    boolPtr = condition;
    boolFunc = nullptr;
    valueFunc = nullptr;
    valuePtr = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::when(float (*getValue)(), float thresh, const String &comp) {
    valueFunc = getValue;
    threshold = thresh;
    comparison = comp;
    boolFunc = nullptr;
    boolPtr = nullptr;
    valuePtr = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::when(float *valuePointer, float thresh, const String &comp) {
    valuePtr = valuePointer;
    threshold = thresh;
    comparison = comp;
    boolFunc = nullptr;
    boolPtr = nullptr;
    valueFunc = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::when(float (*getValue)()) {
    valueFunc = getValue;
    boolFunc = nullptr;
    boolPtr = nullptr;
    valuePtr = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::when(float *valuePointer) {
    valuePtr = valuePointer;
    boolFunc = nullptr;
    boolPtr = nullptr;
    valueFunc = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::greaterThan(float value) {
    threshold = value;
    comparison = ">";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::greaterOrEqual(float value) {
    threshold = value;
    comparison = ">=";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::lessThan(float value) {
    threshold = value;
    comparison = "<";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::lessOrEqual(float value) {
    threshold = value;
    comparison = "<=";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::equalTo(float value) {
    threshold = value;
    comparison = "==";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::notEqualTo(float value) {
    threshold = value;
    comparison = "!=";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::between(float min, float max) {
    threshold = min;
    secondThreshold = max;
    comparison = "between";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::outside(float min, float max) {
    threshold = min;
    secondThreshold = max;
    comparison = "outside";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::approximately(float value, float tolerance) {
    threshold = value;
    secondThreshold = tolerance;
    comparison = "approximately";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::percentageAbove(float baseValue, float percentage) {
    threshold = baseValue * (1.0 + percentage / 100.0);
    comparison = ">";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::percentageBelow(float baseValue, float percentage) {
    threshold = baseValue * (1.0 - percentage / 100.0);
    comparison = "<";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::deviatesFrom(float reference, float deviation) {
    threshold = reference;
    secondThreshold = deviation;
    comparison = "deviates";
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onTrue() {
    trigger = LogicConditionManager::WHEN_TRUE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onFalse() {
    trigger = LogicConditionManager::WHEN_FALSE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onChange() {
    trigger = LogicConditionManager::ON_CHANGE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onRising() {
    trigger = LogicConditionManager::TO_TRUE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onFalling() {
    trigger = LogicConditionManager::TO_FALSE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenBecomesTrue() {
    trigger = LogicConditionManager::TO_TRUE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenBecomesFalse() {
    trigger = LogicConditionManager::TO_FALSE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenChanges() {
    trigger = LogicConditionManager::ON_CHANGE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whileTrue() {
    trigger = LogicConditionManager::WHEN_TRUE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whileFalse() {
    trigger = LogicConditionManager::WHEN_FALSE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenExceeds(float thresh) {
    threshold = thresh;
    comparison = ">";
    trigger = LogicConditionManager::TO_TRUE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenDropsBelow(float thresh) {
    threshold = thresh;
    comparison = "<";
    trigger = LogicConditionManager::TO_TRUE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenReturns() {
    trigger = LogicConditionManager::TO_FALSE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenCrosses(float thresh) {
    threshold = thresh;
    comparison = ">";
    trigger = LogicConditionManager::ON_CHANGE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::whenStays(float thresh, const String &comp) {
    threshold = thresh;
    comparison = comp;
    trigger = LogicConditionManager::WHEN_TRUE;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::afterConsecutive(int count) {
    consecutiveCount = count;
    useConsecutive = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::afterStable(unsigned long duration) {
    stableDuration = duration;
    useStable = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onRateOfChange(float thresholdRate) {
    rateThreshold = thresholdRate;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::withHysteresis(float amount) {
    hysteresis = amount;
    useHysteresis = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::withDeadzone(float zone) {
    deadzone = zone;
    useDeadzone = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::then(void (*actionFunc)()) {
    action = actionFunc;
    paramAction = nullptr;
    actionParam = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::then(void (*actionFunc)(void *), void *param) {
    paramAction = actionFunc;
    actionParam = param;
    action = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::thenAlso(void (*secondaryFunc)()) {
    secondaryAction = secondaryFunc;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::thenWith(void (*actionFunc)(void *), void *param) {
    paramAction = actionFunc;
    actionParam = param;
    action = nullptr;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::andThenStop(void (*stopFunc)()) {
    stopAction = stopFunc;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onStart(void (*initFunc)()) {
    initAction = initFunc;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onComplete(void (*cleanupFunc)()) {
    cleanupAction = cleanupFunc;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onError(void (*errorFunc)()) {
    errorHandler = errorFunc;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onTimeout(void (*timeoutFunc)()) {
    timeoutAction = timeoutFunc;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onlyIf(bool (*guard)()) {
    guardCondition = guard;
    useGuard = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::unless(bool (*guard)()) {
    unlessCondition = guard;
    useUnless = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::during(int startHr, int endHr) {
    startHour = startHr;
    endHour = endHr;
    useTimeRange = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onWeekdays() {
    weekdaysOnly = true;
    weekendsOnly = false;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::onWeekends() {
    weekendsOnly = true;
    weekdaysOnly = false;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::transform(float (*transformerFunc)(float)) {
    transformer = transformerFunc;
    useTransform = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::filter(bool (*validatorFunc)(float)) {
    validator = validatorFunc;
    useFilter = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::smooth(float factor) {
    smoothFactor = factor;
    useSmooth = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::average(int samples) {
    averageSamples = samples;
    useAverage = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::normalize(float min, float max) {
    normalizeMin = min;
    normalizeMax = max;
    useNormalize = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::clamp(float min, float max) {
    clampMin = min;
    clampMax = max;
    useClamp = true;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::repeat(int times) {
    repeatCount = times;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::every(unsigned long ms) {
    interval = ms;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::debounce(unsigned long ms) {
    debounceTime = ms;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::delay(unsigned long ms) {
    delayTime = ms;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::duration(unsigned long ms) {
    actionDuration = ms;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::timeout(unsigned long ms) {
    timeoutDuration = ms;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::precision(unsigned long ms) {
    precisionTime = ms;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::jitter(unsigned long maxMs) {
    jitterTime = maxMs;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::waitAfter(unsigned long ms) {
    waitTime = ms;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::withLogging(bool enable) {
    loggingEnabled = enable;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::withMetrics(bool enable) {
    metricsEnabled = enable;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::withName(const String &name) {
    debugName = name;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::reportEvery(int executions) {
    reportInterval = executions;
    return *this;
}

EasyLogic::ConditionBuilder &EasyLogic::ConditionBuilder::withWatchdog(unsigned long timeoutMs) {
    watchdogTimeout = timeoutMs;
    return *this;
}

int EasyLogic::ConditionBuilder::build() {
    if (boolFunc) {
        if (actionDuration > 0) {
            if (paramAction) {
                return parent->core->addCondition(name, boolFunc, trigger, paramAction, actionParam,
                                                  repeatCount, interval, debounceTime, actionDuration,
                                                  stopAction, waitTime);
            } else {
                return parent->core->addCondition(name, boolFunc, trigger, action,
                                                  repeatCount, interval, debounceTime, actionDuration,
                                                  stopAction, waitTime);
            }
        } else {
            if (paramAction) {
                return parent->core->addCondition(name, boolFunc, trigger, paramAction, actionParam,
                                                  repeatCount, interval, debounceTime);
            } else {
                return parent->core->addCondition(name, boolFunc, trigger, action,
                                                  repeatCount, interval, debounceTime);
            }
        }
    } else if (boolPtr) {
        if (actionDuration > 0) {
            if (paramAction) {
                return parent->core->addCondition(name, boolPtr, trigger, paramAction, actionParam,
                                                  repeatCount, interval, debounceTime, actionDuration,
                                                  stopAction, waitTime);
            } else {
                return parent->core->addCondition(name, boolPtr, trigger, action,
                                                  repeatCount, interval, debounceTime, actionDuration,
                                                  stopAction, waitTime);
            }
        } else {
            if (paramAction) {
                return parent->core->addCondition(name, boolPtr, trigger, paramAction, actionParam,
                                                  repeatCount, interval, debounceTime);
            } else {
                return parent->core->addCondition(name, boolPtr, trigger, action,
                                                  repeatCount, interval, debounceTime);
            }
        }
    } else if (valueFunc || valuePtr) {
        LogicConditionManager::ComparisonType compType;

        if (comparison == ">") {
            compType = LogicConditionManager::GREATER_THAN;
        } else if (comparison == "<") {
            compType = LogicConditionManager::LESS_THAN;
        } else if (comparison == ">=") {
            compType = LogicConditionManager::GREATER_EQUAL;
        } else if (comparison == "<=") {
            compType = LogicConditionManager::LESS_EQUAL;
        } else if (comparison == "==") {
            compType = LogicConditionManager::EQUAL;
        } else if (comparison == "!=") {
            compType = LogicConditionManager::NOT_EQUAL;
        } else if (comparison == "between") {
            compType = LogicConditionManager::GREATER_EQUAL;
        } else if (comparison == "outside") {
            compType = LogicConditionManager::LESS_THAN;
        } else if (comparison == "approximately") {
            compType = LogicConditionManager::EQUAL;
        } else if (comparison == "deviates") {
            compType = LogicConditionManager::NOT_EQUAL;
        } else {
            compType = LogicConditionManager::GREATER_THAN;
        }

        if (valueFunc) {
            if (actionDuration > 0) {
                return parent->core->addValueCondition(name, valueFunc, threshold, compType, trigger,
                                                       action, repeatCount, interval,
                                                       actionDuration, stopAction, waitTime);
            } else {
                return parent->core->addValueCondition(name, valueFunc, threshold, compType, trigger,
                                                       action, repeatCount, interval);
            }
        } else if (valuePtr) {
            if (actionDuration > 0) {
                return parent->core->addValueCondition(name, valuePtr, threshold, compType, trigger,
                                                       action, repeatCount, interval,
                                                       actionDuration, stopAction, waitTime);
            } else {
                return parent->core->addValueCondition(name, valuePtr, threshold, compType, trigger,
                                                       action, repeatCount, interval);
            }
        }
    }
    return -1;
}

EasyLogic::CompositeBuilder::CompositeBuilder(EasyLogic *p, const String &n)
        : parent(p), name(n), count(0), capacity(5) {
    conditionIds = new int[capacity];
}

EasyLogic::CompositeBuilder::~CompositeBuilder() {
    delete[] conditionIds;
}

EasyLogic::CompositeBuilder &EasyLogic::CompositeBuilder::add(int conditionId) {
    if (count >= capacity) {
        capacity *= 2;
        int *newIds = new int[capacity];
        memcpy(newIds, conditionIds, count * sizeof(int));
        delete[] conditionIds;
        conditionIds = newIds;
    }
    conditionIds[count++] = conditionId;
    return *this;
}

EasyLogic::CompositeBuilder &EasyLogic::CompositeBuilder::add(const String &conditionName) {
    if (parent->core->hasCondition(conditionName)) {
        LogicConditionManager *core = parent->core;
        for (int i = 0; i < 100; i++) {
            if (core->hasCondition(conditionName)) {
                add(i);
                break;
            }
        }
    }
    return *this;
}

int EasyLogic::CompositeBuilder::asAND(void (*action)()) {
    return parent->core->addAndCondition(name, conditionIds, count, action);
}

int EasyLogic::CompositeBuilder::asOR(void (*action)()) {
    return parent->core->addOrCondition(name, conditionIds, count, action);
}

int EasyLogic::CompositeBuilder::asNOT(void (*action)()) {
    if (count == 1) {
        return parent->core->addNotCondition(name, conditionIds[0], action);
    }
    return -1;
}

int EasyLogic::CompositeBuilder::asXOR(void (*action)()) {
    return parent->core->addCompositeCondition(name, LogicConditionManager::XOR,
                                               conditionIds, count, action);
}

EasyLogic::EasyLogic(int capacity) {
    core = new LogicConditionManager(capacity);
}

EasyLogic::~EasyLogic() {
    delete core;
}

int EasyLogic::button(const String &name, bool (*condition)(), void (*action)()) {
    return core->addCondition(name, condition, LogicConditionManager::TO_TRUE,
                              action, 1, 0, 50);
}

int EasyLogic::button(const String &name, bool *condition, void (*action)()) {
    return core->addCondition(name, condition, LogicConditionManager::TO_TRUE,
                              action, 1, 0, 50);
}

int EasyLogic::sensor(const String &name, float (*getValue)(), float threshold,
                      void (*action)(), const String &comparison) {
    LogicConditionManager::ComparisonType compType;

    if (comparison == ">") {
        compType = LogicConditionManager::GREATER_THAN;
    } else if (comparison == "<") {
        compType = LogicConditionManager::LESS_THAN;
    } else if (comparison == ">=") {
        compType = LogicConditionManager::GREATER_EQUAL;
    } else if (comparison == "<=") {
        compType = LogicConditionManager::LESS_EQUAL;
    } else if (comparison == "==") {
        compType = LogicConditionManager::EQUAL;
    } else if (comparison == "!=") {
        compType = LogicConditionManager::NOT_EQUAL;
    } else {
        compType = LogicConditionManager::GREATER_THAN;
    }

    return core->addValueCondition(name, getValue, threshold, compType,
                                   LogicConditionManager::WHEN_TRUE, action);
}

int EasyLogic::sensor(const String &name, float *valuePtr, float threshold,
                      void (*action)(), const String &comparison) {
    LogicConditionManager::ComparisonType compType;

    if (comparison == ">") {
        compType = LogicConditionManager::GREATER_THAN;
    } else if (comparison == "<") {
        compType = LogicConditionManager::LESS_THAN;
    } else if (comparison == ">=") {
        compType = LogicConditionManager::GREATER_EQUAL;
    } else if (comparison == "<=") {
        compType = LogicConditionManager::LESS_EQUAL;
    } else if (comparison == "==") {
        compType = LogicConditionManager::EQUAL;
    } else if (comparison == "!=") {
        compType = LogicConditionManager::NOT_EQUAL;
    } else {
        compType = LogicConditionManager::GREATER_THAN;
    }

    return core->addValueCondition(name, valuePtr, threshold, compType,
                                   LogicConditionManager::WHEN_TRUE, action);
}

int EasyLogic::timer(const String &name, unsigned long intervalMs, void (*action)(), bool repeat) {
    return core->addTimerCondition(name, intervalMs, repeat, action, repeat ? -1 : 1);
}

int EasyLogic::toggle(const String &name, bool (*condition)(),
                      void (*onAction)(), void (*offAction)()) {
    int onId = core->addCondition(name + "_on", condition, LogicConditionManager::TO_TRUE, onAction);
    if (offAction) {
        core->addCondition(name + "_off", condition, LogicConditionManager::TO_FALSE, offAction);
    }
    return onId;
}

int EasyLogic::toggle(const String &name, bool *condition,
                      void (*onAction)(), void (*offAction)()) {
    int onId = core->addCondition(name + "_on", condition, LogicConditionManager::TO_TRUE, onAction);
    if (offAction) {
        core->addCondition(name + "_off", condition, LogicConditionManager::TO_FALSE, offAction);
    }
    return onId;
}

EasyLogic::ConditionBuilder EasyLogic::create(const String &name) {
    return ConditionBuilder(this, name);
}

EasyLogic::CompositeBuilder EasyLogic::combine(const String &name) {
    return CompositeBuilder(this, name);
}

void EasyLogic::run() {
    core->update();
}

void EasyLogic::enable(const String &name) {
    core->setEnabled(name, true);
}

void EasyLogic::disable(const String &name) {
    core->setEnabled(name, false);
}

void EasyLogic::pause(const String &name) {
    core->pauseExecution(name);
}

void EasyLogic::resume(const String &name) {
    core->resumeExecution(name);
}

void EasyLogic::reset(const String &name) {
    core->resetCondition(name);
}

bool EasyLogic::remove(const String &name) {
    return core->removeCondition(name);
}

bool EasyLogic::isActive(const String &name) {
    return core->isConditionMet(name);
}

bool EasyLogic::isActive(int conditionId) {
    return core->isConditionMet(conditionId);
}

int EasyLogic::executions(const String &name) {
    return core->getExecutionCount(name);
}

int EasyLogic::executions(int conditionId) {
    return core->getExecutionCount(conditionId);
}

unsigned long EasyLogic::lastTrigger(const String &name) {
    return core->getLastTriggerTime(name);
}

unsigned long EasyLogic::lastTrigger(int conditionId) {
    return core->getLastTriggerTime(conditionId);
}

void EasyLogic::status() {
    core->printAllStatus();
}

void EasyLogic::status(const String &name) {
    core->printStatus(name);
}

void EasyLogic::status(int conditionId) {
    core->printStatus(conditionId);
}

bool EasyLogic::exists(const String &name) {
    return core->hasCondition(name);
}

LogicConditionManager *EasyLogic::advanced() {
    return core;
}