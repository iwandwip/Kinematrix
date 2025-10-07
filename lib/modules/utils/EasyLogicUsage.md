# EasyLogic Ultra-Complete Documentation

The most comprehensive documentation for EasyLogic - every method, enum, parameter, and feature explained in detail.

## Table of Contents

1. [Class Overview](#class-overview)
2. [Constructor & Destructor](#constructor--destructor)
3. [Enums & Constants](#enums--constants)
4. [EasyLogic Main Class](#easylogic-main-class)
5. [ConditionBuilder Class](#conditionbuilder-class)
6. [CompositeBuilder Class](#compositebuilder-class)
7. [Advanced Access](#advanced-access)
8. [Complete Examples](#complete-examples)
9. [Error Codes & Debugging](#error-codes--debugging)
10. [Performance & Memory](#performance--memory)

---

## Class Overview

```cpp
class EasyLogic {
private:
    LogicConditionManager *core;  // Underlying engine

public:
    class ConditionBuilder;       // Fluent API for conditions
    class CompositeBuilder;       // Fluent API for composite logic
    
    // Main methods...
};
```

---

## Constructor & Destructor

### Constructor

```cpp
EasyLogic(int capacity = 20);
```

**Parameters:**

- `capacity` (int): Maximum number of conditions (default: 20)

**Description:** Creates an EasyLogic instance with specified capacity for conditions.

**Memory Usage:** Allocates `capacity * sizeof(LogicCondition)` bytes

**Example:**

```cpp
EasyLogic logic;           // Default 20 conditions
EasyLogic bigLogic(100);   // 100 conditions capacity
```

### Destructor

```cpp
~EasyLogic();
```

**Description:** Automatically cleans up allocated memory and stops all conditions.

---

## Enums & Constants

### TriggerType (from LogicConditionManager)

```cpp
enum TriggerType {
    WHEN_TRUE = 0,    // Continuously while condition is true
    WHEN_FALSE = 1,   // Continuously while condition is false
    TO_TRUE = 2,      // Once when condition becomes true (rising edge)
    TO_FALSE = 3,     // Once when condition becomes false (falling edge)
    ON_CHANGE = 4     // Once when condition changes (either direction)
};
```

**Usage in EasyLogic:**

- `WHEN_TRUE` → `.whileTrue()`
- `WHEN_FALSE` → `.whileFalse()`
- `TO_TRUE` → `.whenBecomesTrue()` or `.onRising()`
- `TO_FALSE` → `.whenBecomesFalse()` or `.onFalling()`
- `ON_CHANGE` → `.whenChanges()` or `.onChange()`

### ComparisonType (from LogicConditionManager)

```cpp
enum ComparisonType {
    GREATER_THAN = 0,     // >
    LESS_THAN = 1,        // <
    GREATER_EQUAL = 2,    // >=
    LESS_EQUAL = 3,       // <=
    EQUAL = 4,            // ==
    NOT_EQUAL = 5         // !=
};
```

**Usage in EasyLogic:**

- `GREATER_THAN` → `.greaterThan(value)`
- `LESS_THAN` → `.lessThan(value)`
- `GREATER_EQUAL` → `.greaterOrEqual(value)`
- `LESS_EQUAL` → `.lessOrEqual(value)`
- `EQUAL` → `.equalTo(value)`
- `NOT_EQUAL` → `.notEqualTo(value)`

### LogicType (from LogicConditionManager)

```cpp
enum LogicType {
    AND = 0,     // All conditions must be true
    OR = 1,      // Any condition must be true
    NOT = 2,     // Condition must be false
    XOR = 3,     // Exactly one condition must be true
    NAND = 4,    // Not all conditions are true
    NOR = 5      // No conditions are true
};
```

**Usage in EasyLogic:**

- `AND` → `.asAND(action)`
- `OR` → `.asOR(action)`
- `NOT` → `.asNOT(action)`
- `XOR` → `.asXOR(action)`
- `NAND` → Available via `.advanced()` access
- `NOR` → Available via `.advanced()` access

### Priority Levels (from LogicConditionManager)

```cpp
enum Priority {
    LOW_PRIORITY = 0,
    NORMAL_PRIORITY = 1,
    HIGH_PRIORITY = 2,
    CRITICAL_PRIORITY = 3
};
```

**Access:** Only available through `.advanced()` method

---

## EasyLogic Main Class

### Quick Setup Methods

#### button()

```cpp
int button(const String &name, bool (*condition)(), void (*action)() = nullptr);
int button(const String &name, bool *condition, void (*action)() = nullptr);
```

**Parameters:**

- `name` (String): Unique identifier for the button condition
- `condition` (bool(*)() or bool*): Function pointer or variable pointer
- `action` (void(*)()): Optional action to execute (default: nullptr)

**Returns:** Condition ID (≥0) on success, -1 on failure

**Default Behavior:**

- Trigger: `TO_TRUE` (when button becomes pressed)
- Debounce: 50ms
- Repeat: 1 time

**Examples:**

```cpp
// Function pointer with action
int btnId = logic.button("start", &isStartPressed, startSystem);

// Variable pointer without action  
logic.button("stop", &stopButton);

// Lambda action
logic.button("reset", &resetBtn, []() { 
    Serial.println("Reset pressed!"); 
});
```

#### sensor()

```cpp
int sensor(const String &name, float (*getValue)(), float threshold,
           void (*action)() = nullptr, const String &comparison = ">");
int sensor(const String &name, float *valuePtr, float threshold,
           void (*action)() = nullptr, const String &comparison = ">");
```

**Parameters:**

- `name` (String): Unique identifier
- `getValue` (float(*)() or float*): Function or variable pointer
- `threshold` (float): Comparison threshold value
- `action` (void(*)()): Optional action (default: nullptr)
- `comparison` (String): Comparison operator (default: ">")

**Supported Comparisons:** ">", "<", ">=", "<=", "==", "!="

**Returns:** Condition ID (≥0) on success, -1 on failure

**Default Behavior:**

- Trigger: `WHEN_TRUE` (continuous while condition met)
- No debounce
- Infinite repeat

**Examples:**

```cpp
// Temperature monitoring
logic.sensor("overheating", &getTemperature, 30.0, coolDown, ">");
logic.sensor("freezing", &temperature, 0.0, heatUp, "<=");

// Pressure with different comparisons
logic.sensor("low_pressure", &pressure, 10.0, alertLow, "<");
logic.sensor("exact_setpoint", &setpoint, 25.0, nullptr, "==");
```

#### timer()

```cpp
int timer(const String &name, unsigned long intervalMs, void (*action)(), bool repeat = false);
```

**Parameters:**

- `name` (String): Unique identifier
- `intervalMs` (unsigned long): Timer interval in milliseconds
- `action` (void(*)()): Action to execute when timer fires
- `repeat` (bool): Whether to repeat (default: false)

**Returns:** Condition ID (≥0) on success, -1 on failure

**Behavior:**

- One-shot: Executes once after interval
- Repeating: Executes every interval

**Examples:**

```cpp
// One-shot startup delay
logic.timer("startup", 5000, initializeSystem, false);

// Repeating heartbeat
logic.timer("heartbeat", 1000, sendHeartbeat, true);

// Data logging every 30 seconds
logic.timer("log_data", 30000, []() {
    logSensorData();
}, true);
```

#### toggle()

```cpp
int toggle(const String &name, bool (*condition)(),
           void (*onAction)() = nullptr, void (*offAction)() = nullptr);
int toggle(const String &name, bool *condition,
           void (*onAction)() = nullptr, void (*offAction)() = nullptr);
```

**Parameters:**

- `name` (String): Base name for toggle conditions
- `condition` (bool(*)() or bool*): Function or variable pointer
- `onAction` (void(*)()): Action when condition becomes true (default: nullptr)
- `offAction` (void(*)()): Action when condition becomes false (default: nullptr)

**Returns:** ID of the "on" condition (≥0) on success, -1 on failure

**Behavior:** Creates two conditions: `name_on` and `name_off`

**Examples:**

```cpp
// Power state monitoring
logic.toggle("power", &isPowerOn, 
    []() { Serial.println("Power ON"); },
    []() { Serial.println("Power OFF"); }
);

// LED state with variable
bool ledState = false;
logic.toggle("led", &ledState, turnOnLED, turnOffLED);
```

### Builder Methods

#### create()

```cpp
ConditionBuilder create(const String &name);
```

**Parameters:**

- `name` (String): Unique identifier for the condition

**Returns:** ConditionBuilder instance for fluent API

**Description:** Creates a new condition builder for complex condition setup.

**Example:**

```cpp
logic.create("complex_sensor")
    .when(&sensor)
    .greaterThan(threshold)
    .withHysteresis(2.0)
    .afterStable(1000)
    .then(action)
    .build();
```

#### combine()

```cpp
CompositeBuilder combine(const String &name);
```

**Parameters:**

- `name` (String): Unique identifier for the composite condition

**Returns:** CompositeBuilder instance for fluent API

**Description:** Creates a new composite builder for combining multiple conditions.

**Example:**

```cpp
logic.combine("emergency")
    .add(highTempId)
    .add(lowPressureId)
    .asAND(emergencyShutdown);
```

### Execution Method

#### run()

```cpp
void run();
```

**Parameters:** None

**Returns:** void

**Description:** Processes all active conditions. Must be called in main loop.

**Performance:** O(n) where n is number of active conditions

**Example:**

```cpp
void loop() {
    logic.run();  // Process all conditions
    delay(10);    // Small delay for stability
}
```

### Control Methods

#### enable()

```cpp
void enable(const String &name);
```

**Parameters:**

- `name` (String): Name of condition to enable

**Returns:** void

**Description:** Enables a previously disabled condition.

#### disable()

```cpp
void disable(const String &name);
```

**Parameters:**

- `name` (String): Name of condition to disable

**Returns:** void

**Description:** Disables a condition (stops checking but preserves state).

#### pause()

```cpp
void pause(const String &name);
```

**Parameters:**

- `name` (String): Name of condition to pause

**Returns:** void

**Description:** Pauses condition execution (stops actions but continues checking).

#### resume()

```cpp
void resume(const String &name);
```

**Parameters:**

- `name` (String): Name of condition to resume

**Returns:** void

**Description:** Resumes a paused condition.

#### reset()

```cpp
void reset(const String &name);
```

**Parameters:**

- `name` (String): Name of condition to reset

**Returns:** void

**Description:** Resets condition state (execution count, timers, etc.).

#### remove()

```cpp
bool remove(const String &name);
```

**Parameters:**

- `name` (String): Name of condition to remove

**Returns:** true if removed successfully, false if not found

**Description:** Permanently removes a condition and frees its resources.

### Query Methods

#### isActive()

```cpp
bool isActive(const String &name);
bool isActive(int conditionId);
```

**Parameters:**

- `name` (String): Name of condition
- `conditionId` (int): ID of condition

**Returns:** true if condition is currently active/met

**Description:** Checks if condition is currently in true state.

#### executions()

```cpp
int executions(const String &name);
int executions(int conditionId);
```

**Parameters:**

- `name` (String): Name of condition
- `conditionId` (int): ID of condition

**Returns:** Number of times condition has executed action

**Description:** Gets execution count for monitoring and debugging.

#### lastTrigger()

```cpp
unsigned long lastTrigger(const String &name);
unsigned long lastTrigger(int conditionId);
```

**Parameters:**

- `name` (String): Name of condition
- `conditionId` (int): ID of condition

**Returns:** Timestamp (millis()) of last trigger, 0 if never triggered

**Description:** Gets time of last condition trigger for timing analysis.

#### exists()

```cpp
bool exists(const String &name);
```

**Parameters:**

- `name` (String): Name of condition to check

**Returns:** true if condition exists

**Description:** Checks if a condition with given name exists.

### Debug Methods

#### status()

```cpp
void status();
void status(const String &name);
void status(int conditionId);
```

**Parameters:**

- None: Prints all conditions
- `name` (String): Name of specific condition
- `conditionId` (int): ID of specific condition

**Returns:** void

**Description:** Prints detailed status information to Serial.

**Output Format:**

```
Condition: temp_monitor [ID: 5]
Status: ACTIVE
Executions: 12
Last Trigger: 15234 ms
Enabled: YES
```

#### advanced()

```cpp
LogicConditionManager *advanced();
```

**Parameters:** None

**Returns:** Pointer to underlying LogicConditionManager

**Description:** Provides direct access to LogicConditionManager for advanced features.

---

## ConditionBuilder Class

### Constructor

```cpp
ConditionBuilder(EasyLogic *parent, const String &name);
```

**Parameters:**

- `parent` (EasyLogic*): Parent EasyLogic instance
- `name` (String): Name for the condition

**Note:** Constructor is private, use `logic.create(name)` instead.

### Input Source Methods

#### when() - Boolean Conditions

```cpp
ConditionBuilder &when(bool (*condition)());
ConditionBuilder &when(bool *condition);
```

**Parameters:**

- `condition` (bool(*)()): Function pointer returning bool
- `condition` (bool*): Pointer to bool variable

**Returns:** Reference to self for chaining

**Description:** Sets the condition source to a boolean function or variable.

**Examples:**

```cpp
// Function pointer
logic.create("button").when(&isButtonPressed);

// Lambda function
logic.create("complex").when([]() { 
    return (sensor1 > 10) && (sensor2 < 50); 
});

// Variable pointer
bool systemReady = false;
logic.create("ready").when(&systemReady);
```

#### when() - Value Conditions

```cpp
ConditionBuilder &when(float (*getValue)(), float thresh, const String &comp = ">");
ConditionBuilder &when(float *valuePtr, float thresh, const String &comp = ">");
ConditionBuilder &when(float (*getValue)());
ConditionBuilder &when(float *valuePtr);
```

**Parameters:**

- `getValue` (float(*)()): Function pointer returning float
- `valuePtr` (float*): Pointer to float variable
- `thresh` (float): Threshold value for comparison
- `comp` (String): Comparison operator (">", "<", ">=", "<=", "==", "!=")

**Returns:** Reference to self for chaining

**Description:** Sets up value-based conditions with optional immediate comparison.

**Examples:**

```cpp
// Function with immediate comparison
logic.create("temp").when(&getTemperature, 30.0, ">");

// Variable with comparison
logic.create("pressure").when(&pressureValue, 10.0, "<");

// Source (comparison set later)
logic.create("sensor").when(&getSensorValue);
```

### Comparison Methods

#### greaterThan()

```cpp
ConditionBuilder &greaterThan(float value);
```

**Parameters:**

- `value` (float): Threshold value

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source > value.

**Example:**

```cpp
logic.create("overheat").when(&temperature).greaterThan(30.0);
```

#### greaterOrEqual()

```cpp
ConditionBuilder &greaterOrEqual(float value);
```

**Parameters:**

- `value` (float): Threshold value

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source >= value.

#### lessThan()

```cpp
ConditionBuilder &lessThan(float value);
```

**Parameters:**

- `value` (float): Threshold value

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source < value.

#### lessOrEqual()

```cpp
ConditionBuilder &lessOrEqual(float value);
```

**Parameters:**

- `value` (float): Threshold value

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source <= value.

#### equalTo()

```cpp
ConditionBuilder &equalTo(float value);
```

**Parameters:**

- `value` (float): Target value

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source == value.

**Note:** For floating-point values, consider using `approximately()` instead.

#### notEqualTo()

```cpp
ConditionBuilder &notEqualTo(float value);
```

**Parameters:**

- `value` (float): Value to avoid

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source != value.

#### between()

```cpp
ConditionBuilder &between(float min, float max);
```

**Parameters:**

- `min` (float): Minimum value (inclusive)
- `max` (float): Maximum value (inclusive)

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when min <= source <= max.

**Example:**

```cpp
logic.create("comfort_zone").when(&temperature).between(20.0, 26.0);
```

#### outside()

```cpp
ConditionBuilder &outside(float min, float max);
```

**Parameters:**

- `min` (float): Lower bound
- `max` (float): Upper bound

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source < min OR source > max.

**Example:**

```cpp
logic.create("danger_zone").when(&pressure).outside(10.0, 50.0);
```

#### approximately()

```cpp
ConditionBuilder &approximately(float value, float tolerance = 0.1);
```

**Parameters:**

- `value` (float): Target value
- `tolerance` (float): Tolerance range (default: 0.1)

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when |source - value| <= tolerance.

**Example:**

```cpp
logic.create("setpoint").when(&actualValue).approximately(25.0, 0.5);
// Triggers when actualValue is between 24.5 and 25.5
```

#### percentageAbove()

```cpp
ConditionBuilder &percentageAbove(float baseValue, float percentage);
```

**Parameters:**

- `baseValue` (float): Reference value
- `percentage` (float): Percentage above base

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source > baseValue * (1 + percentage/100).

**Example:**

```cpp
logic.create("high_load").when(&currentLoad).percentageAbove(nominalLoad, 20);
// Triggers when currentLoad > nominalLoad * 1.2
```

#### percentageBelow()

```cpp
ConditionBuilder &percentageBelow(float baseValue, float percentage);
```

**Parameters:**

- `baseValue` (float): Reference value
- `percentage` (float): Percentage below base

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when source < baseValue * (1 - percentage/100).

#### deviatesFrom()

```cpp
ConditionBuilder &deviatesFrom(float reference, float deviation);
```

**Parameters:**

- `reference` (float): Reference value
- `deviation` (float): Maximum allowed deviation

**Returns:** Reference to self for chaining

**Description:** Sets condition to trigger when |source - reference| > deviation.

### Trigger Type Methods

#### onTrue()

```cpp
ConditionBuilder &onTrue();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to WHEN_TRUE (continuous while condition is true).

**Usage:** For conditions that need continuous monitoring.

#### onFalse()

```cpp
ConditionBuilder &onFalse();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to WHEN_FALSE (continuous while condition is false).

#### onChange()

```cpp
ConditionBuilder &onChange();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to ON_CHANGE (once when condition changes either way).

#### onRising()

```cpp
ConditionBuilder &onRising();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to TO_TRUE (once when condition becomes true).

**Alias:** Same as `whenBecomesTrue()`

#### onFalling()

```cpp
ConditionBuilder &onFalling();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to TO_FALSE (once when condition becomes false).

**Alias:** Same as `whenBecomesFalse()`

#### whenBecomesTrue()

```cpp
ConditionBuilder &whenBecomesTrue();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to TO_TRUE (once when condition becomes true).

**Usage:** Most common for event-based actions.

#### whenBecomesFalse()

```cpp
ConditionBuilder &whenBecomesFalse();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to TO_FALSE (once when condition becomes false).

#### whenChanges()

```cpp
ConditionBuilder &whenChanges();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to ON_CHANGE (once when condition changes).

#### whileTrue()

```cpp
ConditionBuilder &whileTrue();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to WHEN_TRUE (continuous while condition is true).

**Usage:** For conditions requiring continuous action.

#### whileFalse()

```cpp
ConditionBuilder &whileFalse();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to WHEN_FALSE (continuous while condition is false).

### Specialized Trigger Methods

#### whenExceeds()

```cpp
ConditionBuilder &whenExceeds(float thresh);
```

**Parameters:**

- `thresh` (float): Threshold value

**Returns:** Reference to self for chaining

**Description:** Combines `greaterThan(thresh)` with `whenBecomesTrue()`.

**Example:**

```cpp
logic.create("alarm").when(&sensor).whenExceeds(30.0);
// Equivalent to: .when(&sensor).greaterThan(30.0).whenBecomesTrue()
```

#### whenDropsBelow()

```cpp
ConditionBuilder &whenDropsBelow(float thresh);
```

**Parameters:**

- `thresh` (float): Threshold value

**Returns:** Reference to self for chaining

**Description:** Combines `lessThan(thresh)` with `whenBecomesTrue()`.

#### whenReturns()

```cpp
ConditionBuilder &whenReturns();
```

**Returns:** Reference to self for chaining

**Description:** Sets trigger to TO_FALSE (when condition returns to normal).

**Usage:** For recovery notifications.

#### whenCrosses()

```cpp
ConditionBuilder &whenCrosses(float thresh);
```

**Parameters:**

- `thresh` (float): Threshold value

**Returns:** Reference to self for chaining

**Description:** Triggers on any crossing of threshold (up or down).

#### whenStays()

```cpp
ConditionBuilder &whenStays(float thresh, const String &comp = ">");
```

**Parameters:**

- `thresh` (float): Threshold value
- `comp` (String): Comparison operator

**Returns:** Reference to self for chaining

**Description:** Continuous trigger while condition stays true.

### Stability & Reliability Methods

#### afterConsecutive()

```cpp
ConditionBuilder &afterConsecutive(int count);
```

**Parameters:**

- `count` (int): Number of consecutive triggers required

**Returns:** Reference to self for chaining

**Description:** Condition must be true for 'count' consecutive checks before triggering.

**Usage:** Prevents false alarms from noisy signals.

**Example:**

```cpp
logic.create("stable_high").when(&sensor).greaterThan(30)
    .afterConsecutive(3)  // Must be high 3 times in a row
    .then(action);
```

#### afterStable()

```cpp
ConditionBuilder &afterStable(unsigned long duration);
```

**Parameters:**

- `duration` (unsigned long): Stability time in milliseconds

**Returns:** Reference to self for chaining

**Description:** Condition must remain true for 'duration' ms before triggering.

**Example:**

```cpp
logic.create("stable_temp").when(&temp).greaterThan(25)
    .afterStable(5000)  // Must be stable for 5 seconds
    .then(action);
```

#### onRateOfChange()

```cpp
ConditionBuilder &onRateOfChange(float threshold);
```

**Parameters:**

- `threshold` (float): Rate threshold (units per second)

**Returns:** Reference to self for chaining

**Description:** Triggers when rate of change exceeds threshold.

**Note:** Requires multiple samples to calculate rate.

#### withHysteresis()

```cpp
ConditionBuilder &withHysteresis(float amount);
```

**Parameters:**

- `amount` (float): Hysteresis amount

**Returns:** Reference to self for chaining

**Description:** Adds hysteresis to prevent oscillation around threshold.

**Behavior:**

- For `greaterThan(30)` with `withHysteresis(2)`: triggers at 30, returns at 28
- For `lessThan(20)` with `withHysteresis(2)`: triggers at 20, returns at 22

**Example:**

```cpp
logic.create("thermostat").when(&temp).greaterThan(25)
    .withHysteresis(2.0)  // On at 25°C, off at 23°C
    .then(coolingOn);
```

#### withDeadzone()

```cpp
ConditionBuilder &withDeadzone(float zone);
```

**Parameters:**

- `zone` (float): Dead zone size

**Returns:** Reference to self for chaining

**Description:** Creates dead zone around threshold where no triggering occurs.

### Action Methods

#### then() - Basic Action

```cpp
ConditionBuilder &then(void (*actionFunc)());
```

**Parameters:**

- `actionFunc` (void(*)()): Function to execute

**Returns:** Reference to self for chaining

**Description:** Sets primary action to execute when condition triggers.

**Example:**

```cpp
logic.create("alarm").when(&sensor).greaterThan(30)
    .then([]() { Serial.println("ALARM!"); });
```

#### then() - Parameterized Action

```cpp
ConditionBuilder &then(void (*actionFunc)(void *), void *param);
```

**Parameters:**

- `actionFunc` (void(*)(void*)): Function accepting parameter
- `param` (void*): Parameter to pass to function

**Returns:** Reference to self for chaining

**Description:** Sets primary action with parameter.

**Example:**

```cpp
void alertWithValue(void* value) {
    float* temp = (float*)value;
    Serial.print("Alert: ");
    Serial.println(*temp);
}

logic.create("temp_alert").when(&temperature).greaterThan(30)
    .then(alertWithValue, &temperature);
```

#### thenAlso()

```cpp
ConditionBuilder &thenAlso(void (*secondaryFunc)());
```

**Parameters:**

- `secondaryFunc` (void(*)()): Secondary function to execute

**Returns:** Reference to self for chaining

**Description:** Adds secondary action executed after primary action.

**Example:**

```cpp
logic.create("critical").when(&sensor).greaterThan(50)
    .then(primaryAlert)
    .thenAlso([]() { digitalWrite(LED_PIN, HIGH); })
    .thenAlso(logEvent);
```

#### thenWith()

```cpp
ConditionBuilder &thenWith(void (*actionFunc)(void *), void *param);
```

**Parameters:**

- `actionFunc` (void(*)(void*)): Function accepting parameter
- `param` (void*): Parameter to pass

**Returns:** Reference to self for chaining

**Description:** Alias for parameterized `then()`.

#### andThenStop()

```cpp
ConditionBuilder &andThenStop(void (*stopFunc)());
```

**Parameters:**

- `stopFunc` (void(*)()): Function to execute when action stops

**Returns:** Reference to self for chaining

**Description:** Sets action to execute when primary action duration ends.

**Usage:** Used with `duration()` method.

### Lifecycle Action Methods

#### onStart()

```cpp
ConditionBuilder &onStart(void (*initFunc)());
```

**Parameters:**

- `initFunc` (void(*)()): Initialization function

**Returns:** Reference to self for chaining

**Description:** Executes once when condition first becomes active.

#### onComplete()

```cpp
ConditionBuilder &onComplete(void (*cleanupFunc)());
```

**Parameters:**

- `cleanupFunc` (void(*)()): Cleanup function

**Returns:** Reference to self for chaining

**Description:** Executes when condition sequence completes.

#### onError()

```cpp
ConditionBuilder &onError(void (*errorFunc)());
```

**Parameters:**

- `errorFunc` (void(*)()): Error handler function

**Returns:** Reference to self for chaining

**Description:** Executes if error occurs during condition processing.

#### onTimeout()

```cpp
ConditionBuilder &onTimeout(void (*timeoutFunc)());
```

**Parameters:**

- `timeoutFunc` (void(*)()): Timeout handler function

**Returns:** Reference to self for chaining

**Description:** Executes if condition times out (used with `timeout()`).

### Conditional Execution Methods

#### onlyIf()

```cpp
ConditionBuilder &onlyIf(bool (*guard)());
```

**Parameters:**

- `guard` (bool(*)()): Guard condition function

**Returns:** Reference to self for chaining

**Description:** Action only executes if guard condition is true.

**Example:**

```cpp
logic.create("safe_action").when(&sensor).greaterThan(30)
    .onlyIf([]() { return systemSafe; })
    .then(action);
```

#### unless()

```cpp
ConditionBuilder &unless(bool (*guard)());
```

**Parameters:**

- `guard` (bool(*)()): Negative guard condition

**Returns:** Reference to self for chaining

**Description:** Action only executes if guard condition is false.

**Example:**

```cpp
logic.create("normal_op").when(&sensor).greaterThan(30)
    .unless([]() { return maintenanceMode; })
    .then(normalAction);
```

#### during()

```cpp
ConditionBuilder &during(int startHour, int endHour);
```

**Parameters:**

- `startHour` (int): Start hour (0-23)
- `endHour` (int): End hour (0-23)

**Returns:** Reference to self for chaining

**Description:** Action only executes during specified time window.

**Example:**

```cpp
logic.create("business_hours").when(&sensor).greaterThan(30)
    .during(9, 17)  // 9 AM to 5 PM
    .then(businessAlert);
```

#### onWeekdays()

```cpp
ConditionBuilder &onWeekdays();
```

**Returns:** Reference to self for chaining

**Description:** Action only executes Monday through Friday.

#### onWeekends()

```cpp
ConditionBuilder &onWeekends();
```

**Returns:** Reference to self for chaining

**Description:** Action only executes Saturday and Sunday.

### Data Processing Methods

#### transform()

```cpp
ConditionBuilder &transform(float (*transformer)(float));
```

**Parameters:**

- `transformer` (float(*)(float)): Transformation function

**Returns:** Reference to self for chaining

**Description:** Applies transformation to input value before comparison.

**Example:**

```cpp
// Convert Celsius to Fahrenheit
logic.create("temp_f").when(&tempC)
    .transform([](float c) { return c * 1.8 + 32; })
    .greaterThan(86.0)  // 30°C in Fahrenheit
    .then(action);
```

#### filter()

```cpp
ConditionBuilder &filter(bool (*validator)(float));
```

**Parameters:**

- `validator` (bool(*)(float)): Validation function

**Returns:** Reference to self for chaining

**Description:** Filters input values; only valid values are processed.

**Example:**

```cpp
logic.create("valid_temp").when(&rawTemp)
    .filter([](float val) { 
        return val > -50 && val < 150;  // Valid temperature range
    })
    .greaterThan(30)
    .then(action);
```

#### smooth()

```cpp
ConditionBuilder &smooth(float factor);
```

**Parameters:**

- `factor` (float): Smoothing factor (0.0 to 1.0)

**Returns:** Reference to self for chaining

**Description:** Applies exponential smoothing (low-pass filter).

**Formula:** `smoothed = factor * new + (1-factor) * previous`

**Example:**

```cpp
logic.create("smooth_sensor").when(&noisySensor)
    .smooth(0.1)  // Heavy smoothing
    .greaterThan(threshold)
    .then(action);
```

#### average()

```cpp
ConditionBuilder &average(int samples);
```

**Parameters:**

- `samples` (int): Number of samples for moving average

**Returns:** Reference to self for chaining

**Description:** Applies moving average filter.

**Example:**

```cpp
logic.create("avg_sensor").when(&sensor)
    .average(10)  // 10-sample moving average
    .greaterThan(threshold)
    .then(action);
```

#### normalize()

```cpp
ConditionBuilder &normalize(float min, float max);
```

**Parameters:**

- `min` (float): Minimum input value
- `max` (float): Maximum input value

**Returns:** Reference to self for chaining

**Description:** Normalizes input to 0.0-1.0 range.

**Formula:** `normalized = (value - min) / (max - min)`

#### clamp()

```cpp
ConditionBuilder &clamp(float min, float max);
```

**Parameters:**

- `min` (float): Minimum allowed value
- `max` (float): Maximum allowed value

**Returns:** Reference to self for chaining

**Description:** Clamps input value to specified range.

### Timing Control Methods

#### repeat()

```cpp
ConditionBuilder &repeat(int times = -1);
```

**Parameters:**

- `times` (int): Number of repetitions (-1 for infinite, default: -1)

**Returns:** Reference to self for chaining

**Description:** Sets how many times condition can trigger.

**Example:**

```cpp
logic.create("limited").when(&sensor).greaterThan(30)
    .then(action)
    .repeat(5);  // Only trigger 5 times
```

#### every()

```cpp
ConditionBuilder &every(unsigned long ms);
```

**Parameters:**

- `ms` (unsigned long): Interval in milliseconds

**Returns:** Reference to self for chaining

**Description:** Sets minimum interval between action executions.

**Example:**

```cpp
logic.create("throttled").when(&sensor).greaterThan(30)
    .then(action)
    .every(1000);  // At most once per second
```

#### debounce()

```cpp
ConditionBuilder &debounce(unsigned long ms);
```

**Parameters:**

- `ms` (unsigned long): Debounce time in milliseconds

**Returns:** Reference to self for chaining

**Description:** Prevents rapid triggering; condition must be stable for ms.

**Example:**

```cpp
logic.create("button").when(&btnPressed)
    .whenBecomesTrue()
    .debounce(50)  // 50ms debounce
    .then(action);
```

#### delay()

```cpp
ConditionBuilder &delay(unsigned long ms);
```

**Parameters:**

- `ms` (unsigned long): Delay time in milliseconds

**Returns:** Reference to self for chaining

**Description:** Delays action execution after condition triggers.

#### duration()

```cpp
ConditionBuilder &duration(unsigned long ms);
```

**Parameters:**

- `ms` (unsigned long): Action duration in milliseconds

**Returns:** Reference to self for chaining

**Description:** Action runs for specified duration then stops.

**Usage:** Often used with `andThenStop()`.

#### timeout()

```cpp
ConditionBuilder &timeout(unsigned long ms);
```

**Parameters:**

- `ms` (unsigned long): Timeout duration in milliseconds

**Returns:** Reference to self for chaining

**Description:** Overall timeout for condition sequence.

#### precision()

```cpp
ConditionBuilder &precision(unsigned long ms);
```

**Parameters:**

- `ms` (unsigned long): Timing precision in milliseconds

**Returns:** Reference to self for chaining

**Description:** Sets timing precision for the condition.

#### jitter()

```cpp
ConditionBuilder &jitter(unsigned long maxMs);
```

**Parameters:**

- `maxMs` (unsigned long): Maximum jitter in milliseconds

**Returns:** Reference to self for chaining

**Description:** Adds random variation to timing (0 to maxMs).

**Usage:** Prevents synchronization issues with multiple conditions.

#### waitAfter()

```cpp
ConditionBuilder &waitAfter(unsigned long ms);
```

**Parameters:**

- `ms` (unsigned long): Wait time in milliseconds

**Returns:** Reference to self for chaining

**Description:** Wait time after action completes before checking condition again.

### Monitoring & Debug Methods

#### withLogging()

```cpp
ConditionBuilder &withLogging(bool enable = true);
```

**Parameters:**

- `enable` (bool): Enable logging (default: true)

**Returns:** Reference to self for chaining

**Description:** Enables detailed logging for this condition.

#### withMetrics()

```cpp
ConditionBuilder &withMetrics(bool enable = true);
```

**Parameters:**

- `enable` (bool): Enable metrics (default: true)

**Returns:** Reference to self for chaining

**Description:** Enables performance metrics collection.

#### withName()

```cpp
ConditionBuilder &withName(const String &debugName);
```

**Parameters:**

- `debugName` (String): Debug name for logging

**Returns:** Reference to self for chaining

**Description:** Sets debug name (different from condition name).

#### reportEvery()

```cpp
ConditionBuilder &reportEvery(int executions);
```

**Parameters:**

- `executions` (int): Report interval

**Returns:** Reference to self for chaining

**Description:** Reports status every N executions.

#### withWatchdog()

```cpp
ConditionBuilder &withWatchdog(unsigned long timeoutMs);
```

**Parameters:**

- `timeoutMs` (unsigned long): Watchdog timeout in milliseconds

**Returns:** Reference to self for chaining

**Description:** Sets watchdog timer for condition monitoring.

### Finalization Method

#### build()

```cpp
int build();
```

**Parameters:** None

**Returns:** Condition ID (≥0) on success, -1 on failure

**Description:** Finalizes the condition and adds it to the system.

**Note:** Must be called to activate the condition.

**Example:**

```cpp
int id = logic.create("test").when(&sensor).greaterThan(30).then(action).build();
if (id == -1) {
    Serial.println("Failed to create condition!");
}
```

---

## CompositeBuilder Class

### Constructor

```cpp
CompositeBuilder(EasyLogic *parent, const String &name);
```

**Note:** Constructor is private, use `logic.combine(name)` instead.

### Destructor

```cpp
~CompositeBuilder();
```

**Description:** Automatically cleans up allocated condition ID array.

### Building Methods

#### add() - By ID

```cpp
CompositeBuilder &add(int conditionId);
```

**Parameters:**

- `conditionId` (int): ID of condition to add

**Returns:** Reference to self for chaining

**Description:** Adds a condition by ID to the composite.

#### add() - By Name

```cpp
CompositeBuilder &add(const String &conditionName);
```

**Parameters:**

- `conditionName` (String): Name of condition to add

**Returns:** Reference to self for chaining

**Description:** Adds a condition by name to the composite.

### Logic Operation Methods

#### asAND()

```cpp
int asAND(void (*action)() = nullptr);
```

**Parameters:**

- `action` (void(*)()): Optional action to execute (default: nullptr)

**Returns:** Composite condition ID (≥0) on success, -1 on failure

**Description:** Creates AND logic - all conditions must be true.

**Example:**

```cpp
logic.combine("emergency")
    .add(highTempId)
    .add(lowPressureId)
    .add(powerFailId)
    .asAND(emergencyShutdown);  // All three must be true
```

#### asOR()

```cpp
int asOR(void (*action)() = nullptr);
```

**Parameters:**

- `action` (void(*)()): Optional action to execute (default: nullptr)

**Returns:** Composite condition ID (≥0) on success, -1 on failure

**Description:** Creates OR logic - any condition must be true.

**Example:**

```cpp
logic.combine("any_alarm")
    .add("temp_high")
    .add("pressure_low")
    .add("vibration_high")
    .asOR(generalAlert);  // Any one triggers alert
```

#### asNOT()

```cpp
int asNOT(void (*action)() = nullptr);
```

**Parameters:**

- `action` (void(*)()): Optional action to execute (default: nullptr)

**Returns:** Composite condition ID (≥0) on success, -1 on failure

**Description:** Creates NOT logic - condition must be false.

**Limitation:** Only works with single condition.

**Example:**

```cpp
logic.combine("power_off")
    .add(powerOnId)
    .asNOT([]() { Serial.println("Power is OFF"); });
```

#### asXOR()

```cpp
int asXOR(void (*action)() = nullptr);
```

**Parameters:**

- `action` (void(*)()): Optional action to execute (default: nullptr)

**Returns:** Composite condition ID (≥0) on success, -1 on failure

**Description:** Creates XOR logic - exactly one condition must be true.

**Example:**

```cpp
logic.combine("exclusive_mode")
    .add(mode1Id)
    .add(mode2Id)
    .asXOR([]() { Serial.println("Exclusive mode active"); });
```

---

## Advanced Access

### Direct LogicConditionManager Access

```cpp
LogicConditionManager *advanced();
```

Provides access to underlying LogicConditionManager for advanced features not exposed by EasyLogic.

#### Advanced Methods Available

```cpp
LogicConditionManager* core = logic.advanced();

// Priority control
core->setPriority("critical", HIGH_PRIORITY);

// Custom data attachment
core->setCustomData("sensor", userData);

// Advanced composite logic
core->addCompositeCondition("complex", LogicConditionManager::NAND, 
                            conditions, count, action);

// Memory and performance tuning
core->optimizeMemory();
core->setMaxExecutionTime(1000);

// Advanced debugging
core->enableProfiling(true);
core->getPerformanceStats();
```

---

## Complete Examples

### Home Automation System

```cpp
#include "EasyLogic.h"

EasyLogic logic(50);  // 50 condition capacity

// Sensors
float temperature = 22.0;
float humidity = 45.0;
float lightLevel = 300.0;
bool motionDetected = false;
bool doorOpen = false;

// System states
bool heatingOn = false;
bool coolingOn = false;
bool lightsOn = false;
bool securityArmed = false;

void setup() {
    Serial.begin(115200);
    
    // === CLIMATE CONTROL ===
    
    // Heating system with hysteresis and stability
    logic.create("heating_on")
        .when(&temperature)
        .lessThan(20.0)
        .withHysteresis(1.0)        // Off at 21°C
        .afterStable(30000)         // Stable for 30 seconds
        .onlyIf([]() { return !coolingOn; })  // Not if cooling
        .then([]() {
            heatingOn = true;
            Serial.println("🔥 Heating ON");
        })
        .withLogging(true)
        .build();
    
    logic.create("heating_off")
        .when(&temperature)
        .greaterOrEqual(21.0)
        .afterStable(10000)         // Stable for 10 seconds
        .onlyIf([]() { return heatingOn; })
        .then([]() {
            heatingOn = false;
            Serial.println("🔥 Heating OFF");
        })
        .build();
    
    // Cooling system
    logic.create("cooling_on")
        .when(&temperature)
        .greaterThan(26.0)
        .withHysteresis(1.0)        // Off at 25°C
        .afterConsecutive(3)        // 3 readings in a row
        .onlyIf([]() { return !heatingOn; })
        .then([]() {
            coolingOn = true;
            Serial.println("❄️ Cooling ON");
        })
        .build();
    
    // Humidity control with time restrictions
    logic.create("dehumidifier")
        .when(&humidity)
        .greaterThan(70.0)
        .during(6, 22)              // Only 6 AM to 10 PM
        .afterStable(60000)         // Stable for 1 minute
        .then([]() {
            Serial.println("💧 Dehumidifier ON");
        })
        .repeat(3)                  // Max 3 times per trigger
        .every(3600000)             // Once per hour max
        .build();
    
    // === LIGHTING CONTROL ===
    
    // Motion-activated lighting
    logic.create("motion_lights")
        .when(&motionDetected)
        .whenBecomesTrue()
        .onlyIf([]() { return lightLevel < 200; })  // Only in low light
        .during(18, 6)              // 6 PM to 6 AM (wraps midnight)
        .debounce(500)              // 500ms debounce
        .then([]() {
            lightsOn = true;
            Serial.println("💡 Lights ON (motion)");
        })
        .build();
    
    // Auto-off after no motion
    logic.create("lights_auto_off")
        .when(&motionDetected)
        .whenBecomesFalse()
        .delay(300000)              // 5 minute delay
        .onlyIf([]() { return lightsOn; })
        .then([]() {
            lightsOn = false;
            Serial.println("💡 Lights OFF (auto)");
        })
        .build();
    
    // === SECURITY SYSTEM ===
    
    // Door monitoring when armed
    logic.create("security_breach")
        .when(&doorOpen)
        .whenBecomesTrue()
        .onlyIf([]() { return securityArmed; })
        .then([]() {
            Serial.println("🚨 SECURITY BREACH!");
        })
        .thenAlso([]() {
            // Flash lights
            for(int i = 0; i < 10; i++) {
                digitalWrite(LED_BUILTIN, HIGH);
                delay(100);
                digitalWrite(LED_BUILTIN, LOW);
                delay(100);
            }
        })
        .withLogging(true)
        .build();
    
    // === COMPOSITE CONDITIONS ===
    
    // Comfortable environment (temperature AND humidity)
    int tempOK = logic.create("temp_ok")
        .when(&temperature).between(20.0, 26.0).build();
    int humidOK = logic.create("humid_ok")
        .when(&humidity).between(30.0, 60.0).build();
    
    logic.combine("comfort_zone")
        .add(tempOK)
        .add(humidOK)
        .asAND([]() {
            static unsigned long lastReport = 0;
            if (millis() - lastReport > 300000) {  // Every 5 minutes
                Serial.println("✅ Comfort zone maintained");
                lastReport = millis();
            }
        });
    
    // Away mode (no motion AND door closed)
    int noMotion = logic.create("no_motion")
        .when(&motionDetected).whenBecomesFalse()
        .afterStable(1800000)       // 30 minutes stable
        .build();
    int doorClosed = logic.create("door_closed")
        .when(&doorOpen).whenBecomesFalse().build();
    
    logic.combine("away_mode")
        .add(noMotion)
        .add(doorClosed)
        .asAND([]() {
            securityArmed = true;
            Serial.println("🏠 Entering away mode - security armed");
        });
    
    // === ENERGY SAVING ===
    
    // Night mode (low light AND late time)
    logic.create("night_mode")
        .when(&lightLevel)
        .lessThan(50.0)
        .during(22, 6)              // 10 PM to 6 AM
        .afterStable(600000)        // Stable for 10 minutes
        .then([]() {
            Serial.println("🌙 Night mode activated");
            // Reduce heating/cooling targets
        })
        .build();
    
    Serial.println("🏠 Smart Home System Ready!");
    Serial.println("Commands: 't' temp, 'h' humidity, 'l' light, 'm' motion, 'd' door, 's' status");
}

void loop() {
    logic.run();
    
    // Simulate sensor updates or handle serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        switch(cmd) {
            case 't': temperature += random(-50, 50) / 100.0; break;
            case 'h': humidity += random(-50, 50) / 100.0; break;
            case 'l': lightLevel = random(0, 1000); break;
            case 'm': motionDetected = !motionDetected; break;
            case 'd': doorOpen = !doorOpen; break;
            case 's': logic.status(); break;
        }
    }
    
    delay(100);
}
```

### Industrial Process Control

```cpp
#include "EasyLogic.h"

EasyLogic logic(100);  // Industrial scale

// Process variables
float pressure = 15.0;      // PSI
float temperature = 85.0;   // °C
float flowRate = 5.2;       // L/min
float pH = 7.0;
float vibration = 0.5;      // mm/s
bool emergencyStop = false;
bool maintenanceMode = false;

// Equipment states
bool pump1Running = false;
bool pump2Running = false;
bool coolingActive = false;
bool alarmActive = false;

// Process parameters
const float PRESSURE_MIN = 10.0;
const float PRESSURE_MAX = 20.0;
const float TEMP_MAX = 90.0;
const float FLOW_TARGET = 5.0;

void setup() {
    Serial.begin(115200);
    
    // === CRITICAL SAFETY MONITORING ===
    
    // Emergency pressure monitoring with multiple validation
    logic.create("pressure_emergency")
        .when(&pressure)
        .transform([](float raw) {
            // Apply sensor calibration
            return raw * 1.02 + 0.15;
        })
        .filter([](float val) {
            // Reject impossible readings
            return val >= 0 && val <= 100;
        })
        .smooth(0.2)                    // Smooth noise
        .average(3)                     // 3-sample average
        .lessThan(PRESSURE_MIN * 0.8)   // 80% of minimum
        .afterConsecutive(2)            // Must happen twice
        .afterStable(1000)              // Stable for 1 second
        .then([]() {
            emergencyStop = true;
            Serial.println("🚨 EMERGENCY: Critical low pressure!");
        })
        .thenAlso([]() {
            // Immediate pump shutdown
            pump1Running = pump2Running = false;
        })
        .onError([]() {
            Serial.println("❌ Pressure sensor error - check connections");
        })
        .withLogging(true)
        .withWatchdog(5000)             // 5-second watchdog
        .build();
    
    // High pressure safety
    logic.create("pressure_high")
        .when(&pressure)
        .greaterThan(PRESSURE_MAX * 1.1)  // 110% of maximum
        .withHysteresis(1.0)
        .afterConsecutive(3)
        .then([]() {
            Serial.println("⚠️ High pressure - reducing flow");
            // Gradual pressure reduction
        })
        .repeat(5)                      // Allow 5 interventions
        .every(10000)                   // Every 10 seconds
        .build();
    
    // === TEMPERATURE CONTROL ===
    
    // Multi-stage temperature management
    logic.create("temp_warning")
        .when(&temperature)
        .between(TEMP_MAX * 0.9, TEMP_MAX)  // Warning zone
        .whenBecomesTrue()
        .then([]() {
            Serial.println("🌡️ Temperature warning - activating cooling");
            coolingActive = true;
        })
        .build();
    
    logic.create("temp_critical")
        .when(&temperature)
        .greaterThan(TEMP_MAX)
        .withHysteresis(3.0)            // 3°C hysteresis
        .afterStable(2000)              // 2-second confirmation
        .then([]() {
            Serial.println("🚨 CRITICAL TEMPERATURE - Emergency cooling!");
        })
        .thenAlso([]() {
            // Emergency cooling protocol
            coolingActive = true;
            flowRate *= 1.5;  // Increase flow
        })
        .build();
    
    // === FLOW CONTROL ===
    
    // Adaptive flow control with PID-like behavior
    logic.create("flow_control")
        .when(&flowRate)
        .deviatesFrom(FLOW_TARGET, 0.5)  // ±0.5 L/min tolerance
        .afterStable(5000)               // 5-second stability
        .during(6, 18)                   // Business hours only
        .onWeekdays()
        .unless([]() { return maintenanceMode; })
        .then([]() {
            float error = flowRate - FLOW_TARGET;
            if (error > 0) {
                Serial.println("📈 Flow too high - adjusting");
            } else {
                Serial.println("📉 Flow too low - adjusting");
            }
        })
        .every(30000)                    // Max once per 30 seconds
        .build();
    
    // === QUALITY MONITORING ===
    
    // pH monitoring with complex validation
    logic.create("ph_monitor")
        .when(&pH)
        .outside(6.5, 7.5)              // Acceptable pH range
        .afterConsecutive(5)            // 5 consecutive readings
        .onlyIf([]() { 
            return !emergencyStop && !maintenanceMode; 
        })
        .then([]() {
            Serial.print("⚗️ pH out of range: ");
            Serial.println(pH);
        })
        .thenAlso([]() {
            // Automatic pH correction
            if (pH < 6.5) {
                Serial.println("Adding base...");
            } else {
                Serial.println("Adding acid...");
            }
        })
        .reportEvery(3)                 // Report every 3rd occurrence
        .build();
    
    // === PREDICTIVE MAINTENANCE ===
    
    // Vibration analysis for predictive maintenance
    logic.create("vibration_analysis")
        .when(&vibration)
        .percentageAbove(0.3, 50)       // 50% above baseline (0.45 mm/s)
        .smooth(0.1)                    // Heavy smoothing for vibration
        .afterStable(60000)             // 1 minute stable
        .then([]() {
            Serial.println("🔧 Elevated vibration - schedule maintenance");
        })
        .onComplete([]() {
            // Log maintenance prediction
            Serial.println("📊 Maintenance prediction logged");
        })
        .withMetrics(true)
        .build();
    
    // === PUMP MANAGEMENT ===
    
    // Pump rotation for even wear
    logic.create("pump_rotation")
        .when([]() { return pump1Running; })
        .whileTrue()
        .duration(7200000)              // 2 hours
        .then([]() {
            Serial.println("🔄 Rotating pumps for even wear");
            pump1Running = false;
            pump2Running = true;
        })
        .every(14400000)                // Every 4 hours
        .build();
    
    // === PROCESS OPTIMIZATION ===
    
    // Optimal operating conditions monitoring
    int pressureOK = logic.create("pressure_optimal")
        .when(&pressure).between(PRESSURE_MIN, PRESSURE_MAX).build();
    int tempOK = logic.create("temp_optimal")
        .when(&temperature).between(80.0, 88.0).build();
    int flowOK = logic.create("flow_optimal")
        .when(&flowRate).approximately(FLOW_TARGET, 0.2).build();
    int pHOK = logic.create("ph_optimal")
        .when(&pH).between(6.8, 7.2).build();
    
    logic.combine("optimal_conditions")
        .add(pressureOK)
        .add(tempOK)
        .add(flowOK)
        .add(pHOK)
        .asAND([]() {
            static unsigned long lastOptimal = 0;
            if (millis() - lastOptimal > 300000) {  // Every 5 minutes
                Serial.println("✅ Process running in optimal conditions");
                // Log efficiency metrics
                lastOptimal = millis();
            }
        });
    
    // === ALARM MANAGEMENT ===
    
    // Composite alarm condition
    int tempAlarm = logic.create("temp_alarm")
        .when(&temperature).greaterThan(TEMP_MAX * 0.95).build();
    int pressureAlarm = logic.create("pressure_alarm")
        .when(&pressure).outside(PRESSURE_MIN * 0.9, PRESSURE_MAX * 1.05).build();
    int vibrationAlarm = logic.create("vibration_alarm")
        .when(&vibration).greaterThan(1.0).build();
    
    logic.combine("any_alarm")
        .add(tempAlarm)
        .add(pressureAlarm)
        .add(vibrationAlarm)
        .asOR([]() {
            if (!alarmActive) {
                alarmActive = true;
                Serial.println("🚨 SYSTEM ALARM ACTIVATED");
            }
        });
    
    // === EMERGENCY MONITORING ===
    
    logic.create("emergency_stop_monitor")
        .when([]() { return emergencyStop; })
        .whenBecomesTrue()
        .then([]() {
            Serial.println("🛑 EMERGENCY STOP - Shutting down all systems");
            pump1Running = pump2Running = false;
            coolingActive = false;
            alarmActive = true;
        })
        .onStart([]() {
            Serial.println("🚨 Emergency stop sequence initiated");
        })
        .onComplete([]() {
            Serial.println("✅ Emergency stop sequence completed");
        })
        .withLogging(true)
        .build();
    
    Serial.println("🏭 Industrial Process Control System Ready!");
    Serial.println("Status monitoring active - use 's' for status report");
}

void loop() {
    logic.run();
    
    // Simulate process updates
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {  // Update every second
        // Simulate sensor drift and noise
        pressure += random(-10, 10) / 100.0;
        temperature += random(-20, 20) / 100.0;
        flowRate += random(-10, 10) / 100.0;
        pH += random(-5, 5) / 1000.0;
        vibration += random(-2, 2) / 100.0;
        
        // Keep values in reasonable ranges
        pressure = constrain(pressure, 0, 30);
        temperature = constrain(temperature, 20, 100);
        flowRate = constrain(flowRate, 0, 10);
        pH = constrain(pH, 5.0, 9.0);
        vibration = constrain(vibration, 0, 2.0);
        
        lastUpdate = millis();
    }
    
    // Handle commands
    if (Serial.available()) {
        char cmd = Serial.read();
        switch(cmd) {
            case 's': 
                Serial.println("\n=== PROCESS STATUS ===");
                Serial.printf("Pressure: %.2f PSI\n", pressure);
                Serial.printf("Temperature: %.2f °C\n", temperature);
                Serial.printf("Flow Rate: %.2f L/min\n", flowRate);
                Serial.printf("pH: %.2f\n", pH);
                Serial.printf("Vibration: %.2f mm/s\n", vibration);
                Serial.printf("Emergency Stop: %s\n", emergencyStop ? "ACTIVE" : "Normal");
                logic.status();
                break;
            case 'e': 
                emergencyStop = !emergencyStop;
                Serial.printf("Emergency stop: %s\n", emergencyStop ? "ACTIVATED" : "CLEARED");
                break;
            case 'm':
                maintenanceMode = !maintenanceMode;
                Serial.printf("Maintenance mode: %s\n", maintenanceMode ? "ON" : "OFF");
                break;
        }
    }
    
    delay(50);  // Fast update for industrial control
}
```

---

## Error Codes & Debugging

### Return Codes

| Return Value | Meaning                       |
|--------------|-------------------------------|
| ≥ 0          | Success - condition ID        |
| -1           | General failure               |
| -2           | Invalid parameters            |
| -3           | Memory allocation failed      |
| -4           | Capacity exceeded             |
| -5           | Condition name already exists |

### Debug Output Format

```
Condition: sensor_temp [ID: 3]
Status: ACTIVE
Executions: 15
Last Trigger: 12345 ms
Type: VALUE_CONDITION
Source: 0x40123456 (25.30)
Threshold: 30.00 (>)
Trigger: TO_TRUE
Enabled: YES
Paused: NO
```

### Common Error Scenarios

#### Memory Issues

```cpp
// Check for memory exhaustion
if (logic.create("test").when(&sensor).greaterThan(30).then(action).build() == -1) {
    Serial.println("ERROR: Failed to create condition - check memory/capacity");
}
```

#### Name Conflicts

```cpp
// Avoid duplicate names
if (logic.exists("temp_monitor")) {
    logic.remove("temp_monitor");  // Remove existing
}
logic.create("temp_monitor").when(&temp).greaterThan(30).then(action).build();
```

#### Invalid Configurations

```cpp
// Check for valid threshold values
if (isnan(threshold) || isinf(threshold)) {
    Serial.println("ERROR: Invalid threshold value");
    return;
}
```

---

## Performance & Memory

### Memory Usage

| Component           | Memory per Instance              |
|---------------------|----------------------------------|
| Basic Condition     | ~64 bytes                        |
| Value Condition     | ~68 bytes                        |
| Composite Condition | ~72 bytes + (4 × num_conditions) |
| ConditionBuilder    | ~200 bytes (temporary)           |

### Performance Characteristics

| Operation           | Time Complexity | Notes                       |
|---------------------|-----------------|-----------------------------|
| `run()`             | O(n)            | n = active conditions       |
| `create().build()`  | O(1)            | Constant time setup         |
| `combine().asAND()` | O(m)            | m = conditions in composite |
| `isActive()`        | O(1)            | Direct lookup               |
| `status()`          | O(n)            | Prints all conditions       |

### Optimization Tips

#### Memory Optimization

```cpp
// Use smaller capacity if possible
EasyLogic logic(10);  // Instead of default 20

// Remove unused conditions
logic.remove("temporary_condition");

// Use static data when possible
static const float TEMP_THRESHOLD = 30.0;
logic.create("temp").when(&temp).greaterThan(TEMP_THRESHOLD);
```

#### Performance Optimization

```cpp
// Minimize work in frequently called conditions
logic.create("fast_check")
    .when(&fastSensor)
    .greaterThan(threshold)
    .every(100)  // Throttle to 10Hz
    .then(lightweightAction);

// Use consecutive triggers for noisy signals
logic.create("stable_signal")
    .when(&noisySensor)
    .greaterThan(threshold)
    .afterConsecutive(3)  // Reduces false triggers
    .then(action);
```

### Capacity Planning

| Application Type     | Recommended Capacity |
|----------------------|----------------------|
| Home Automation      | 20-50 conditions     |
| Industrial Control   | 50-200 conditions    |
| IoT Sensor Network   | 10-30 conditions     |
| Laboratory Equipment | 30-100 conditions    |

---

This documentation covers every method, parameter, enum, and feature of EasyLogic in comprehensive detail.
Use it as
your complete reference guide for implementing any level of condition monitoring from simple home automation to complex
industrial control systems.