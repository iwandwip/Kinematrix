# EasyLogic User-Friendly Documentation

The easiest way to create smart conditions for Arduino projects with readable, natural language API.

## Quick Start

```cpp
#include "EasyLogic.h"

EasyLogic logic;
float temperature = 25.0;

void setup() {
    // Create a simple temperature monitor
    logic.create("hot_alert")
        .when(&temperature)
        .greaterThan(30.0)
        .then([]() { Serial.println("Too hot!"); })
        .build();
}

void loop() {
    logic.run();  // Check all conditions
}
```

## Why EasyLogic?

### ❌ Old Way (Confusing)

```cpp
// Hard to read and error-prone
logic.addCondition("temp", &temp, 30.0, ">", action);
```

### ✅ New Way (Crystal Clear)

```cpp
// Reads like English!
logic.create("temp")
.when(&temperature)
.greaterThan(30.0)
.then(action)
.build();
```

---

## Basic Setup

### Constructor

```cpp
EasyLogic logic;           // Default (20 conditions)
EasyLogic logic(50);       // Custom capacity
```

### Essential Method

```cpp
void loop() {
    logic.run();           // Always call this in your loop!
}
```

---

## Simple Methods

Perfect for common scenarios without any complexity.

### Button Monitoring

```cpp
logic.button("start", &buttonPressed, startAction);
```

**Parameters:**

- `name`: Unique name for the button
- `condition`: Pointer to bool variable or function
- `action`: What to do when pressed (optional)

**Features:** Automatic 50ms debounce, triggers on button press.

### Sensor Monitoring

```cpp
logic.sensor("temperature", &tempValue, 30.0, alertAction, ">");
```

**Parameters:**

- `name`: Unique name
- `value`: Pointer to sensor value
- `threshold`: Limit value
- `action`: What to do when exceeded (optional)
- `comparison`: ">", "<", ">=", "<=", "==", "!=" (optional)

### Timer

```cpp
logic.timer("heartbeat", 1000, sendPing, true);  // Repeat every 1 second
logic.timer("startup", 5000, initialize, false); // One-time after 5 seconds
```

---

## Fluent API (The New Way!)

Create complex conditions that read like natural language.

### Basic Structure

```cpp
logic.create("condition_name")
    .when(source)           // What to monitor
    .comparison(value)      // How to compare
    .trigger()              // When to trigger
    .then(action)           // What to do
    .build();               // Finish and activate
```

---

## Input Sources

### Boolean Sources

```cpp
// Monitor a boolean variable
.when(&buttonPressed)

// Monitor a function that returns true/false
.when([]() {
return digitalRead(BUTTON_PIN); })
```

### Value Sources

```cpp
// Monitor a sensor variable
.when(&temperature)

// Monitor a function that returns a number
.when([]() {
return analogRead(A0) * 0.1; })
```

---

## Comparisons (Method-Based)

Replace confusing operators with clear, readable methods!

### Basic Comparisons

```cpp
.greaterThan(30.0)         // temperature > 30
.lessThan(10.0)            // temperature < 10
.greaterOrEqual(25.0)      // temperature >= 25
.lessOrEqual(0.0)          // temperature <= 0
.equalTo(22.5)             // temperature == 22.5
.notEqualTo(0.0)           // temperature != 0
```

### Smart Comparisons

```cpp
.between(20.0, 30.0)       // temperature between 20-30°C
.outside(10.0, 40.0)       // temperature outside safe range
.approximately(25.0)       // temperature ≈ 25°C (±0.1)
.approximately(25.0, 0.5)  // temperature ≈ 25°C (±0.5)
```

### Examples

```cpp
// Temperature monitoring
logic.create("comfort_zone")
.when(&temperature)
.between(20.0, 26.0)    // Comfortable range
.then([]() {
Serial.println("Perfect temperature!"); });

// Pressure safety
logic.create("low_pressure")
.when(&pressure)
.lessThan(10.0)         // Below minimum
.then(activateAlarm);
```

---

## Triggers (When to Act)

### One-Time Triggers

```cpp
.whenBecomesTrue()         // Once when condition becomes true
.whenBecomesFalse()        // Once when condition becomes false  
.whenChanges()             // Once when condition changes (either way)
```

### Continuous Triggers

```cpp
.whileTrue()               // Continuously while condition is true
.whileFalse()              // Continuously while condition is false
```

### Aliases (Same Functionality)

```cpp
.onRising()                // Same as .whenBecomesTrue()
.onFalling()               // Same as .whenBecomesFalse()
.onChange()                // Same as .whenChanges()
.onTrue()                  // Same as .whileTrue()
.onFalse()                 // Same as .whileFalse()
```

### Examples

```cpp
// Door monitor - alert once when opened
logic.create("door_alert")
.when(&doorOpen)
.whenBecomesTrue()      // Only when a door opens
.then([]() {
Serial.println("Door opened!"); });

// Fan control - run while hot
logic.create("cooling")
.when(&temperature)
.greaterThan(28.0)
.whileTrue()            // Keep running while hot
.then(runFan);
```

---

## Actions

### Single Action

```cpp
.then(myFunction)          // Call a function
.then([]() {               // Lambda function
    Serial.println("Action executed!");
    digitalWrite(LED_PIN, HIGH);
})
```

### Multiple Actions

```cpp
.then(primaryAction)       // Main action
.thenAlso(secondaryAction) // Additional action
```

### Example

```cpp
logic.create("alarm_system")
    .when(&smokeDetected)
    .whenBecomesTrue()
    .then([]() { 
        Serial.println("🚨 SMOKE DETECTED!"); 
    })
    .thenAlso([]() {
        digitalWrite(ALARM_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
    })
    .build();
```

---

## Timing Controls

### Debouncing

```cpp
.debounce(50)              // Wait 50ms for stable signal
```

### Repetition

```cpp
.repeat(5)                 // Trigger maximum 5 times
.repeat(-1)                // Trigger unlimited times (default)
```

### Intervals

```cpp
.every(1000)               // At most once per second
```

### Examples

```cpp
// Button with debouncing
logic.create("clean_button")
.when(&buttonPressed)
.whenBecomesTrue()
.debounce(50)           // Ignore bouncing
.then(handleClick);

// Limited alarm
logic.create("limited_alarm")
.when(&sensorValue)
.greaterThan(100)
.then(soundAlarm)
.repeat(3)              // Only alarm 3 times
.every(5000);           // Maximum once per 5 seconds
```

---

## Combining Conditions

Create logic with multiple conditions using AND, OR NOT.

### Step 1: Create Individual Conditions

```cpp
int tempHigh = logic.create("temp_high")
    .when(&temperature).greaterThan(30.0).build();

int pressureLow = logic.create("pressure_low")
    .when(&pressure).lessThan(10.0).build();
```

### Step 2: Combine Them

```cpp
// AND: Both conditions must be true
logic.combine("emergency")
.add(tempHigh)
.add(pressureLow)
.asAND(emergencyShutdown);

// OR: Either condition can be true
logic.combine("any_problem")
.add(tempHigh)
.add(pressureLow)
.asOR(generalAlert);

// NOT: Condition must be false
logic.combine("temp_normal")
.add(tempHigh)
.asNOT([]() {
Serial.println("Temperature normal"); });
```

---

## Control Methods

### Runtime Control

```cpp
logic.enable("condition_name");    // Turn on
logic.disable("condition_name");   // Turn off
logic.reset("condition_name");     // Reset counters
logic.remove("condition_name");    // Delete permanently
```

### Status Checking

```cpp
if (logic.isActive("alarm")) {
    Serial.println("Alarm is currently triggered!");
}

int count = logic.executions("button");
Serial.print("Button pressed ");
Serial.print(count);
Serial.println(" times");
```

### Debug Information

```cpp
logic.status();                    // Show all conditions
logic.status("specific_condition"); // Show one condition
```

---

## Complete Examples

### Home Temperature Control

```cpp
#include "EasyLogic.h"

EasyLogic logic;

float temperature = 22.0;
bool heatingOn = false;
bool coolingOn = false;

void setup() {
    Serial.begin(115200);
    
    // Heating control
    logic.create("heating")
        .when(&temperature)
        .lessThan(20.0)
        .whenBecomesTrue()
        .then([]() {
            heatingOn = true;
            Serial.println("🔥 Heating ON");
        })
        .build();
    
    // Cooling control  
    logic.create("cooling")
        .when(&temperature)
        .greaterThan(26.0)
        .whenBecomesTrue()
        .then([]() {
            coolingOn = true;
            Serial.println("❄️ Cooling ON");
        })
        .build();
    
    // Comfort zone monitoring
    logic.create("comfort")
        .when(&temperature)
        .between(20.0, 26.0)
        .whenBecomesTrue()
        .then([]() {
            heatingOn = coolingOn = false;
            Serial.println("✅ Perfect temperature!");
        })
        .build();
    
    Serial.println("🌡️ Temperature control ready!");
}

void loop() {
    logic.run();
    
    // Simulate temperature sensor or read real sensor
    // temperature = readTemperatureSensor();
    
    delay(100);
}
```

### Garden Irrigation System

```cpp
#include "EasyLogic.h"

EasyLogic logic;

float soilMoisture = 50.0;    // Percentage
float temperature = 25.0;     // Celsius
bool isRaining = false;
bool wateringActive = false;

void setup() {
    Serial.begin(115200);
    
    // Dry soil detection
    logic.create("soil_dry")
        .when(&soilMoisture)
        .lessThan(30.0)          // Below 30%
        .whenBecomesTrue()
        .then([]() {
            Serial.println("🌱 Soil is dry - watering needed");
        })
        .build();
    
    // Hot weather extra watering
    logic.create("hot_weather")
        .when(&temperature)
        .greaterThan(30.0)       // Above 30°C
        .whileTrue()             // Continuous monitoring
        .then([]() {
            static unsigned long lastAlert = 0;
            if (millis() - lastAlert > 300000) {  // Every 5 minutes
                Serial.println("🌡️ Hot weather - extra water needed");
                lastAlert = millis();
            }
        })
        .build();
    
    // Don't water when raining
    logic.create("rain_detected")
        .when(&isRaining)
        .whenBecomesTrue()
        .then([]() {
            wateringActive = false;
            Serial.println("🌧️ Rain detected - stopping irrigation");
        })
        .build();
    
    // Automatic watering logic
    int dryCondition = logic.create("dry_soil")
        .when(&soilMoisture).lessThan(25.0).build();
    int noRain = logic.create("no_rain")
        .when(&isRaining).whenBecomesFalse().build();
    
    logic.combine("start_watering")
        .add(dryCondition)
        .add(noRain)
        .asAND([]() {
            if (!wateringActive) {
                wateringActive = true;
                Serial.println("💧 Starting automatic irrigation");
            }
        });
    
    // Stop when soil is wet enough
    logic.create("soil_wet")
        .when(&soilMoisture)
        .greaterThan(60.0)       // Above 60%
        .whenBecomesTrue()
        .then([]() {
            wateringActive = false;
            Serial.println("✅ Soil moisture sufficient - stopping irrigation");
        })
        .build();
    
    Serial.println("🌿 Garden irrigation system ready!");
}

void loop() {
    logic.run();
    
    // Simulate sensors or read real values
    // soilMoisture = readSoilSensor();
    // temperature = readTemperatureSensor();
    // isRaining = readRainSensor();
    
    delay(1000);
}
```

### Security System

```cpp
#include "EasyLogic.h"

EasyLogic logic;

bool motionDetected = false;
bool doorOpen = false;
bool windowOpen = false;
bool systemArmed = false;
bool silentMode = false;

void setup() {
    Serial.begin(115200);
    
    // Motion detection when armed
    logic.create("motion_alarm")
        .when(&motionDetected)
        .whenBecomesTrue()
        .then([]() {
            if (systemArmed) {
                Serial.println("🚨 MOTION DETECTED!");
                if (!silentMode) {
                    // Sound alarm
                    for(int i = 0; i < 5; i++) {
                        digitalWrite(LED_BUILTIN, HIGH);
                        delay(200);
                        digitalWrite(LED_BUILTIN, LOW);
                        delay(200);
                    }
                }
            }
        })
        .debounce(1000)          // 1 second debounce
        .build();
    
    // Door monitoring
    logic.create("door_alert")
        .when(&doorOpen)
        .whenBecomesTrue()
        .then([]() {
            if (systemArmed) {
                Serial.println("🚪 DOOR OPENED - Security breach!");
            } else {
                Serial.println("🚪 Door opened");
            }
        })
        .build();
    
    // Window monitoring
    logic.create("window_alert")
        .when(&windowOpen)
        .whenBecomesTrue()
        .then([]() {
            if (systemArmed) {
                Serial.println("🪟 WINDOW OPENED - Security breach!");
            }
        })
        .build();
    
    // Any breach detection (composite)
    int motionBreach = logic.create("motion_breach")
        .when(&motionDetected).whenBecomesTrue().build();
    int doorBreach = logic.create("door_breach")
        .when(&doorOpen).whenBecomesTrue().build();
    int windowBreach = logic.create("window_breach")
        .when(&windowOpen).whenBecomesTrue().build();
    
    logic.combine("any_breach")
        .add(motionBreach)
        .add(doorBreach)
        .add(windowBreach)
        .asOR([]() {
            if (systemArmed) {
                Serial.println("🚨 SECURITY BREACH DETECTED!");
                // Send notification, call authorities, etc.
            }
        });
    
    // System status monitoring
    logic.create("system_armed")
        .when(&systemArmed)
        .whenBecomesTrue()
        .then([]() {
            Serial.println("🛡️ Security system ARMED");
        })
        .build();
    
    logic.create("system_disarmed")
        .when(&systemArmed)
        .whenBecomesFalse()
        .then([]() {
            Serial.println("🏠 Security system DISARMED");
        })
        .build();
    
    Serial.println("🛡️ Security system ready!");
    Serial.println("Commands: 'a' arm/disarm, 'm' motion, 'd' door, 'w' window, 's' status");
}

void loop() {
    logic.run();
    
    // Handle commands
    if (Serial.available()) {
        char cmd = Serial.read();
        switch(cmd) {
            case 'a': 
                systemArmed = !systemArmed;
                break;
            case 'm':
                motionDetected = !motionDetected;
                Serial.printf("Motion: %s\n", motionDetected ? "DETECTED" : "Clear");
                break;
            case 'd':
                doorOpen = !doorOpen;
                Serial.printf("Door: %s\n", doorOpen ? "OPEN" : "Closed");
                break;
            case 'w':
                windowOpen = !windowOpen;
                Serial.printf("Window: %s\n", windowOpen ? "OPEN" : "Closed");
                break;
            case 's':
                logic.status();
                break;
        }
    }
    
    delay(100);
}
```

---

## Best Practices

### 1. Use Descriptive Names

```cpp
// ✅ Good - clear intent
logic.create("temperature_overheat_warning");

// ❌ Bad - unclear
logic.create("temp1");
```

### 2. Choose the Right Triggers

```cpp
// For events (happens once)
.whenBecomesTrue()

// For continuous monitoring  
.whileTrue()

// For any change
.whenChanges()
```

### 3. Add Debouncing for Noisy Signals

```cpp
// Buttons always need debouncing
logic.create("button")
.when(&buttonPressed)
.whenBecomesTrue()
.debounce(50)        // 50ms debounce
.then(action);
```

### 4. Use Simple Methods for Common Cases

```cpp
// Simple sensor monitoring
logic.sensor("temperature", &temp, 30.0, alertAction);

// Instead of complex builder for basic cases
logic.create("temperature").when(&temp).greaterThan(30.0).then(alertAction).build();
```

### 5. Check Return Values

```cpp
int id = logic.create("critical")
    .when(&sensor).greaterThan(threshold)
    .then(action)
    .build();

if (id == -1) {
    Serial.println("ERROR: Failed to create condition!");
}
```

---

## Common Patterns

### Thermostat (with hysteresis)

```cpp
// Turn on heating when cold
logic.create("heating_on")
.when(&temperature)
.lessThan(20.0)
.whenBecomesTrue()
.then(turnHeatingOn);

// Turn off heating when warm enough (prevents rapid cycling)
logic.create("heating_off")
.when(&temperature)
.greaterOrEqual(22.0)    // 2°C higher than turn-on
.whenBecomesTrue()
.then(turnHeatingOff);
```

### Button with Multiple Actions

```cpp
logic.create("menu_button")
    .when(&menuButtonPressed)
    .whenBecomesTrue()
    .debounce(50)
    .then([]() {
        Serial.println("Menu button pressed");
    })
    .thenAlso([]() {
        updateDisplay();
        playBeep();
    })
    .build();
```

### Safety Monitoring

```cpp
// Multiple safety conditions
int overheating = logic.create("temp_high")
.when(&temperature).greaterThan(80.0).build();
int overpressure = logic.create("pressure_high")
.when(&pressure).greaterThan(50.0).build();

// Emergency if ANY safety condition
logic.combine("emergency")
.add(overheating)
.add(overpressure)
.asOR(emergencyShutdown);
```

---

## Troubleshooting

### Condition Not Triggering

1. Check if condition is enabled: `logic.isActive("name")`
2. Verify trigger type (`.whenBecomesTrue()` vs `.whileTrue()`)
3. Check threshold values
4. Add debug output in action

### Memory Issues

```cpp
// Create with smaller capacity if needed
EasyLogic logic(10);  // Only 10 conditions

// Remove unused conditions
logic.remove("old_condition");
```

### Timing Issues

```cpp
// Add debouncing for noisy signals
.debounce(50)

// Limit trigger frequency
.every(1000)

// Always call logic.run() in loop
void loop() {
logic.run();  // Essential!
delay(10);
}
```

---

EasyLogic makes creating smart, responsive systems simple and readable. Start with basic patterns and gradually add more
sophisticated logic as your projects grow!