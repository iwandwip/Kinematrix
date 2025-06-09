#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

// ==============================================
// GLOBAL VARIABLES ONLY
// ==============================================
LogicConditionManager manager(20);

// Sensor variables
bool buttonPressed = false;
bool motionDetected = false;
bool lightOn = false;
float temperature = 25.0;
float humidity = 60.0;
int sensorValue = 512;
int deviceSetting = 75;
String notificationMsg = "System Alert!";

// Pin definitions
const int BUTTON_PIN = 2;
const int MOTION_PIN = 4;
const int LED_PIN = 13;
const int TEMP_SENSOR_PIN = 36;
const int LIGHT_SENSOR_PIN = 39;

// Condition IDs
int buttonCondId, motionCondId, tempCondId, humidityCondId;
int andCondId, orCondId, notCondId, xorCondId, nandCondId, norCondId;
int timerCondId, sequenceCondId, edgeCountCondId;

// ==============================================
// SETUP FUNCTION
// ==============================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("\n🚀 LogicConditionManager - Lambda Functions Only");
  Serial.println("=================================================");

  // Initialize pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTION_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // ==============================================
  // BOOLEAN CONDITIONS WITH LAMBDA FUNCTIONS
  // ==============================================
  Serial.println("\n📋 Creating Boolean conditions with lambda functions:");

  // WHEN_TRUE - Button pressed with lambda action
  buttonCondId = manager.addCondition(
    "ButtonPressed",
    []() -> bool {
      return digitalRead(BUTTON_PIN) == LOW;
    },
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("🚨 ALARM! Button pressed detected!");
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
      }
    },
    1, 0, 100);
  Serial.println("✓ WHEN_TRUE: Button condition with lambda action");

  // WHEN_FALSE - No motion detected
  motionCondId = manager.addCondition(
    "NoMotion",
    []() -> bool {
      return digitalRead(MOTION_PIN) == HIGH;
    },
    LogicConditionManager::WHEN_FALSE,
    []() {
      Serial.println("⏹️ No motion - stopping recording");
      digitalWrite(LED_PIN, LOW);
    },
    1, 0, 500);
  Serial.println("✓ WHEN_FALSE: No motion condition with lambda");

  // ON_CHANGE - Light sensor change
  int lightChangeId = manager.addCondition(
    "LightChange",
    []() -> bool {
      return analogRead(LIGHT_SENSOR_PIN) < 300;
    },
    LogicConditionManager::ON_CHANGE,
    []() {
      Serial.println("💡 Light condition changed - sending notification");
      Serial.println("📱 Notification: Light level changed!");
    });
  Serial.println("✓ ON_CHANGE: Light sensor with lambda notification");

  // TO_TRUE - Motion start detection
  int motionStartId = manager.addCondition(
    "MotionStart",
    []() -> bool {
      return digitalRead(MOTION_PIN) == HIGH;
    },
    LogicConditionManager::TO_TRUE,
    []() {
      Serial.println("🎥 Motion detected - starting recording");
      digitalWrite(LED_PIN, HIGH);
    });
  Serial.println("✓ TO_TRUE: Motion start with lambda recording");

  // TO_FALSE - Motion end detection
  int motionEndId = manager.addCondition(
    "MotionEnd",
    []() -> bool {
      return digitalRead(MOTION_PIN) == HIGH;
    },
    LogicConditionManager::TO_FALSE,
    []() {
      Serial.println("👤 Motion ended - logging event");
      Serial.print("📊 Event logged at: ");
      Serial.println(millis());
    });
  Serial.println("✓ TO_FALSE: Motion end with lambda logging");

  // ==============================================
  // BOOLEAN POINTER CONDITIONS WITH LAMBDAS
  // ==============================================
  Serial.println("\n📍 Boolean Pointer conditions with lambdas:");

  int boolPtrId = manager.addCondition("LightStatus",
                                       &lightOn,
                                       LogicConditionManager::WHEN_TRUE,
                                       []() {
                                         Serial.println("🔧 Light is ON - calibrating sensors");
                                         Serial.println("⚙️ Sensor calibration complete");
                                       });
  Serial.println("✓ Boolean pointer with lambda calibration");

  // ==============================================
  // VALUE CONDITIONS WITH LAMBDA EVALUATORS
  // ==============================================
  Serial.println("\n🌡️ Value conditions with lambda evaluators:");

  // Temperature monitoring with lambda evaluator and action
  tempCondId = manager.addValueCondition(
    "HighTemperature",
    []() -> float {
      int raw = analogRead(TEMP_SENSOR_PIN);
      return (raw * 3.3 / 4095.0 - 0.5) * 100.0;  // Convert to Celsius
    },
    [](float temp) -> bool {
      return temp > 30.0;
    },  // Lambda evaluator
    LogicConditionManager::WHEN_TRUE,
    []() {  // Lambda action
      Serial.println("🌀 High temperature detected - turning ON fan");
      digitalWrite(LED_PIN, HIGH);
    },
    1, 1000);
  Serial.println("✓ VALUE_FUNCTION: Temperature with lambda evaluator & action");

  // Humidity monitoring with complex lambda
  humidityCondId = manager.addValueCondition(
    "OptimalHumidity",
    []() -> float {
      return random(30, 90);
    },  // Simulate humidity sensor
    [](float hum) -> bool {
      return hum >= 40.0 && hum <= 70.0;  // Optimal range check
    },
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("💧 Humidity is optimal");
      Serial.print("✅ Current conditions are perfect for operation");
    },
    1, 2000);
  Serial.println("✓ VALUE_FUNCTION: Humidity with lambda range check");

  // VALUE_POINTER with lambda
  int valuePtrId = manager.addValueCondition(
    "TemperaturePtr", &temperature,
    [](float temp) -> bool {
      return temp < 15.0;
    },  // Low temp check
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("❄️ Low temperature detected - turning OFF fan");
      digitalWrite(LED_PIN, LOW);
    });
  Serial.println("✓ VALUE_POINTER: Temperature pointer with lambda");

  // ==============================================
  // PARAMETERIZED ACTIONS WITH LAMBDAS
  // ==============================================
  Serial.println("\n🎯 Parameterized actions with lambdas:");

  int paramCondId = manager.addCondition(
    "ParamAction",
    []() -> bool {
      return digitalRead(BUTTON_PIN) == LOW;
    },
    LogicConditionManager::TO_TRUE,
    [](void* param) {
      String* msg = (String*)param;
      Serial.println("📝 Parameterized Lambda: " + *msg);
      Serial.println("🔔 Custom notification system activated");
    },
    &notificationMsg, 2, 1000, 200);
  Serial.println("✓ Parameterized lambda with custom message");

  int paramPtrId = manager.addCondition(
    "DeviceAdjust", &buttonPressed,
    LogicConditionManager::WHEN_TRUE,
    [](void* param) {
      int* setting = (int*)param;
      Serial.println("⚙️ Device setting adjusted via lambda to: " + String(*setting));
      *setting += 5;  // Modify the setting
      Serial.println("📈 New setting value: " + String(*setting));
    },
    &deviceSetting, 1, 0, 300);
  Serial.println("✓ Parameterized lambda with device adjustment");

  // ==============================================
  // COMPOSITE CONDITIONS WITH LAMBDA ACTIONS
  // ==============================================
  Serial.println("\n🔗 Composite conditions with lambda actions:");

  int tempHumidCondIds[] = { tempCondId, humidityCondId };
  int motionLightIds[] = { motionStartId, lightChangeId };
  int allSensorIds[] = { buttonCondId, motionCondId, tempCondId };

  // AND condition with lambda
  andCondId = manager.addAndCondition(
    "TempAndHumidity", tempHumidCondIds, 2,
    []() {
      Serial.println("🌡️💧 Both temperature AND humidity conditions met!");
      Serial.println("📱 Sending comprehensive environmental alert");
    },
    1, 500);
  Serial.println("✓ AND condition with lambda environmental alert");

  // OR condition with lambda
  orCondId = manager.addOrCondition(
    "MotionOrLight", motionLightIds, 2,
    []() {
      Serial.println("👁️ Motion OR Light change detected!");
      Serial.println("🚨 Security system activated via lambda");
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
    },
    1, 1000);
  Serial.println("✓ OR condition with lambda security alert");

  // NOT condition with lambda
  notCondId = manager.addNotCondition(
    "NotButton", buttonCondId,
    []() {
      Serial.println("🔄 Button NOT pressed - performing maintenance");
      Serial.println("🧹 System maintenance routine executed");
    },
    -1, 2000);
  Serial.println("✓ NOT condition with lambda maintenance");

  // XOR condition with lambda
  xorCondId = manager.addCompositeCondition(
    "XorSensors", LogicConditionManager::XOR,
    tempHumidCondIds, 2,
    []() {
      Serial.println("⚡ XOR: Only ONE of temp/humidity conditions true");
      Serial.println("⚖️ System balance check via lambda");
    },
    1, 800);
  Serial.println("✓ XOR condition with lambda balance check");

  // NAND condition with lambda
  nandCondId = manager.addCompositeCondition(
    "NandSensors", LogicConditionManager::NAND,
    motionLightIds, 2,
    []() {
      Serial.println("🚫 NAND: NOT (Motion AND Light)");
      Serial.println("🔒 Security standby mode via lambda");
    },
    1, 600);
  Serial.println("✓ NAND condition with lambda standby mode");

  // NOR condition with lambda
  norCondId = manager.addCompositeCondition(
    "NorAllSensors", LogicConditionManager::NOR,
    allSensorIds, 3,
    []() {
      Serial.println("😴 NOR: All sensors inactive");
      Serial.println("💤 Sleep mode preparation via lambda");
    },
    1, 1200);
  Serial.println("✓ NOR condition with lambda sleep mode");

  // ==============================================
  // TIMER CONDITIONS WITH LAMBDAS
  // ==============================================
  Serial.println("\n⏰ Timer conditions with lambdas:");

  // Auto-reset timer with lambda
  timerCondId = manager.addTimerCondition(
    "AutoResetTimer", 5000, true,
    []() {
      Serial.println("⏰ Auto-reset timer triggered!");
      Serial.print("🕐 Current time: ");
      Serial.println(millis());
      Serial.println("📊 Periodic system health check completed");
    },
    -1, 1000);
  Serial.println("✓ Auto-reset timer with lambda health check");

  // One-shot timer with lambda
  int oneShotTimerId = manager.addTimerCondition(
    "OneShotTimer", 3000, false,
    []() {
      Serial.println("🎯 One-shot timer completed!");
      Serial.println("🚀 Initialization sequence finished");
    },
    1);
  Serial.println("✓ One-shot timer with lambda initialization");

  // ==============================================
  // SEQUENCE CONDITIONS WITH LAMBDAS
  // ==============================================
  Serial.println("\n📝 Sequence conditions with lambdas:");

  int sequenceIds[] = { buttonCondId, motionStartId, tempCondId };
  sequenceCondId = manager.addSequenceCondition(
    "ButtonMotionTemp", sequenceIds, 3,
    10000, 3,
    []() {
      Serial.println("🎪 SEQUENCE COMPLETED: Button→Motion→Temperature!");
      Serial.println("🏆 Complex interaction pattern detected");
      Serial.println("🔥 Executing advanced response protocol");
      // Complex sequence response
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        delay(50);
      }
    },
    1);
  Serial.println("✓ Sequence condition with lambda pattern detection");

  // ==============================================
  // EDGE COUNTER CONDITIONS WITH LAMBDAS
  // ==============================================
  Serial.println("\n🔢 Edge counter conditions with lambdas:");

  edgeCountCondId = manager.addEdgeCountCondition(
    "ButtonCount", buttonCondId,
    5, 15000,
    []() {
      Serial.println("🔥 RAPID BUTTON PRESS DETECTED!");
      Serial.println("🚨 5 button presses in 15 seconds!");
      Serial.println("🎮 Entering turbo mode via lambda");
      // Turbo mode indication
      for (int i = 0; i < 10; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(30);
        digitalWrite(LED_PIN, LOW);
        delay(30);
      }
    },
    1);
  Serial.println("✓ Edge counter with lambda turbo mode");

  // ==============================================
  // ADVANCED LAMBDA EXAMPLES
  // ==============================================
  Serial.println("\n🚀 Advanced lambda examples:");

  // Lambda with complex logic and multiple operations
  int advancedId = manager.addCondition(
    "AdvancedLambda",
    []() -> bool {
      // Complex condition logic in lambda
      static unsigned long lastCheck = 0;
      unsigned long now = millis();
      if (now - lastCheck > 2000) {
        lastCheck = now;
        return (analogRead(TEMP_SENSOR_PIN) > 2000) && (digitalRead(BUTTON_PIN) == LOW);
      }
      return false;
    },
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("🎯 Advanced lambda condition triggered!");
      Serial.println("🔬 Complex analysis complete");
      Serial.print("📊 System uptime: ");
      Serial.print(millis() / 1000);
      Serial.println(" seconds");

      // Complex response pattern
      int pattern[] = { 100, 50, 100, 50, 200 };
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(pattern[i]);
        digitalWrite(LED_PIN, LOW);
        delay(50);
      }
    });
  Serial.println("✓ Advanced lambda with complex logic & response");

  // Lambda with capture by reference
  static int triggerCount = 0;
  int captureId = manager.addCondition(
    "CaptureExample",
    [&triggerCount]() -> bool {
      return (triggerCount % 10 == 0) && (triggerCount > 0);
    },
    LogicConditionManager::WHEN_TRUE,
    [&triggerCount]() {
      Serial.println("📈 Capture lambda triggered!");
      Serial.print("🔢 Trigger count reached milestone: ");
      Serial.println(triggerCount);
      triggerCount = 0;  // Reset counter
    });
  Serial.println("✓ Lambda with capture by reference");

  // Configuration with lambdas
  manager.setRepeatCount(tempCondId, 3);
  manager.setInterval(humidityCondId, 2500);
  manager.setDebounceTime(buttonCondId, 150);
  Serial.println("✓ Conditions configured");

  Serial.println("\n📊 Initial status:");
  manager.printAllStatus();

  Serial.println("\n🎮 Starting main loop with lambda-only callbacks...");
  Serial.println("🔘 Press button on pin 2 to see lambda functions in action!");
}

// ==============================================
// MAIN LOOP
// ==============================================
void loop() {
  static unsigned long lastStatusPrint = 0;
  static unsigned long lastTempUpdate = 0;
  static int loopCounter = 0;
  static int triggerCount = 0;

  // Update all conditions
  manager.update();

  // Simulate sensor value changes
  if (millis() - lastTempUpdate > 3000) {
    temperature = random(10, 40);
    humidity = random(30, 90);
    lightOn = random(0, 2);
    buttonPressed = random(0, 10) > 7;   // 30% chance
    motionDetected = random(0, 10) > 6;  // 40% chance
    triggerCount++;

    lastTempUpdate = millis();
  }

  // Manual check and trigger with lambda callback
  if (loopCounter % 2000 == 0) {
    bool triggered = manager.checkAndTrigger("HighTemperature", []() {
      Serial.println("🔥 Manual lambda callback executed!");
      Serial.println("🎯 Direct trigger system working");
    });

    if (triggered) {
      Serial.println("✅ Manual trigger with lambda successful");
    }
  }

  // Control methods demonstration
  if (loopCounter == 5000) {
    Serial.println("\n⏸️ Pausing timer via lambda control...");
    manager.pauseExecution(timerCondId);
  }

  if (loopCounter == 7000) {
    Serial.println("\n▶️ Resuming timer via lambda control...");
    manager.resumeExecution(timerCondId);
  }

  if (loopCounter == 10000) {
    Serial.println("\n🔄 Resetting conditions...");
    manager.resetCondition(buttonCondId);
    manager.resetCondition(tempCondId);
    manager.resetCondition(timerCondId);
  }

  if (loopCounter == 15000) {
    Serial.println("\n❌ Disabling motion condition...");
    manager.setEnabled(motionCondId, false);
  }

  if (loopCounter == 18000) {
    Serial.println("\n✅ Re-enabling motion condition...");
    manager.setEnabled(motionCondId, true);
  }

  // Query methods demonstration
  if (loopCounter % 1000 == 0) {
    if (loopCounter % 5000 == 0) {  // Less frequent detailed info
      Serial.println("\n🔍 Lambda system status:");
      Serial.print("🌡️ Temp condition: ");
      Serial.println(manager.getCurrentBooleanValue(tempCondId) ? "HIGH" : "NORMAL");
      Serial.print("⏰ Timer executions: ");
      Serial.println(manager.getExecutionCount(timerCondId));
      Serial.print("🔘 Button state: ");
      Serial.println(manager.getConditionState(buttonCondId));
    }
  }

  // Periodic status
  if (millis() - lastStatusPrint > 15000) {  // Every 15 seconds
    Serial.println("\n📊 Lambda System Status:");
    Serial.println("Current sensor readings:");
    Serial.println("  🌡️ Temperature: " + String(temperature) + "°C");
    Serial.println("  💧 Humidity: " + String(humidity) + "%");
    Serial.println("  💡 Light: " + String(lightOn ? "ON" : "OFF"));
    Serial.println("  🔘 Button: " + String(buttonPressed ? "PRESSED" : "RELEASED"));
    Serial.println("  👁️ Motion: " + String(motionDetected ? "DETECTED" : "NONE"));
    Serial.println("  🔢 Trigger count: " + String(triggerCount));

    // Key condition statuses
    manager.printStatus("HighTemperature");
    manager.printStatus("AutoResetTimer");
    manager.printStatus("TempAndHumidity");
    manager.printStatus("ButtonMotionTemp");

    lastStatusPrint = millis();
  }

  // Condition management with lambdas
  if (loopCounter == 25000) {
    Serial.println("\n🔧 Lambda condition management:");

    if (manager.hasCondition("HighTemperature")) {
      Serial.println("✅ Temperature condition exists");
    }

    // Demonstrate remove and re-add with new lambda
    if (manager.removeCondition("ButtonCount")) {
      Serial.println("🗑️ Removed old ButtonCount condition");

      // Re-add with different lambda
      edgeCountCondId = manager.addEdgeCountCondition(
        "ButtonCount", buttonCondId,
        3, 10000,
        []() {
          Serial.println("🎪 NEW LAMBDA: 3 quick presses detected!");
          Serial.println("🌟 Enhanced response system activated");
          // New enhanced response
          for (int i = 0; i < 7; i++) {
            digitalWrite(LED_PIN, HIGH);
            delay(100);
            digitalWrite(LED_PIN, LOW);
            delay(100);
          }
        },
        2);
      Serial.println("✅ Re-added ButtonCount with enhanced lambda");
    }
  }

  // Reset demonstration
  if (loopCounter == 30000) {
    Serial.println("\n🏁 Lambda system demonstration complete!");
    Serial.println("📊 Final status with all lambda callbacks:");
    manager.printAllStatus();

    Serial.println("\n🔄 Resetting for next cycle...");
    loopCounter = 0;

    Serial.println("\n🚀 All callbacks are pure lambda functions!");
    Serial.println("✨ No additional functions beyond setup() and loop()");
  }

  loopCounter++;
  delay(10);
}