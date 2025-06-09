#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

// ==============================================
// GLOBAL VARIABLES
// ==============================================
LogicConditionManager manager(30);  // Kapasitas lebih besar untuk banyak kondisi

// Sensor variables
bool buttonState = false;
bool motionDetected = false;
bool doorOpen = false;
float temperature = 25.0;
float humidity = 60.0;
float pressure = 1013.25;

// Pin definitions
const int BUTTON_PIN = 2;
const int MOTION_PIN = 4;
const int DOOR_PIN = 5;
const int LED_PIN = 13;
const int BUZZER_PIN = 12;

// Parameterized action data
String alertMessage = "System Alert!";
int fanSpeed = 50;
int lightLevel = 75;

// ==============================================
// SETUP FUNCTION
// ==============================================
void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("\n🚀 SEMUA JENIS addCondition - PARAMETER LENGKAP");
  Serial.println("==================================================");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MOTION_PIN, INPUT);
  pinMode(DOOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // ==============================================
  // 1. BASIC BOOLEAN FUNCTION CONDITIONS
  // ==============================================
  Serial.println("\n 1. BASIC BOOLEAN FUNCTION CONDITIONS");
  Serial.println("=====================================");

  // 1A. Minimal parameters - hanya action
  int basicId = manager.addCondition(
    "BasicButton",
    []() -> bool {
      return digitalRead(BUTTON_PIN) == LOW;
    },
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("🔘 Basic button pressed!");
    });
  Serial.println("✓ 1A. Minimal: name, function, trigger, action");
  Serial.println("     Fungsi: Trigger sekali saat kondisi true");

  // 1B. Dengan repeatCount - action berulang
  int repeatId = manager.addCondition(
    "RepeatButton",
    []() -> bool {
      return digitalRead(BUTTON_PIN) == LOW;
    },
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("🔁 Repeat action executed!");
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // Toggle LED
    },
    3);  // repeatCount = 3
  Serial.println("✓ 1B. Dengan repeatCount=3");
  Serial.println("     Fungsi: Action dijalankan 3x saat trigger");

  // 1C. Dengan interval - jeda antar action
  int intervalId = manager.addCondition(
    "IntervalButton",
    []() -> bool {
      return digitalRead(BUTTON_PIN) == LOW;
    },
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("⏱️ Interval action: " + String(millis()));
      tone(BUZZER_PIN, 1000, 100);
    },
    5,      // repeatCount = 5
    1000);  // interval = 1000ms
  Serial.println("✓ 1C. Dengan repeatCount=5, interval=1000ms");
  Serial.println("     Fungsi: Action dijalankan 5x dengan jeda 1 detik");

  // 1D. Dengan debounceTime - anti-noise
  int debounceId = manager.addCondition(
    "DebounceButton",
    []() -> bool {
      return digitalRead(BUTTON_PIN) == LOW;
    },
    LogicConditionManager::TO_TRUE,
    []() {
      Serial.println("🚫 Debounced button press!");
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
    },
    1,     // repeatCount = 1
    0,     // interval = 0 (tidak ada jeda)
    300);  // debounceTime = 300ms
  Serial.println("✓ 1D. Dengan debounceTime=300ms");
  Serial.println("     Fungsi: Ignore trigger dalam 300ms setelah trigger terakhir");

  // 1E. Parameter lengkap - semua parameter
  int fullParamId = manager.addCondition(
    "FullParamButton",
    []() -> bool {
      return digitalRead(BUTTON_PIN) == LOW;
    },
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("🎯 Full parameter action: " + String(millis()));
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
    },
    10,    // repeatCount = 10 (action 10x)
    500,   // interval = 500ms (jeda 0.5 detik)
    200);  // debounceTime = 200ms (anti-noise)
  Serial.println("✓ 1E. Parameter lengkap: repeat=10, interval=500ms, debounce=200ms");
  Serial.println("     Fungsi: 10x action dengan jeda 0.5s, anti-noise 200ms");

  // ==============================================
  // 2. BOOLEAN POINTER CONDITIONS
  // ==============================================
  Serial.println("\n 2. BOOLEAN POINTER CONDITIONS");
  Serial.println("==============================");

  // 2A. Basic pointer condition
  int ptrBasicId = manager.addCondition("BasicPointer", &buttonState,
                                        LogicConditionManager::WHEN_TRUE,
                                        []() {
                                          Serial.println("📍 Pointer condition true!");
                                        });
  Serial.println("✓ 2A. Basic pointer: name, pointer, trigger, action");
  Serial.println("     Fungsi: Monitor variable langsung, bukan function call");

  // 2B. Pointer dengan parameter lengkap
  int ptrFullId = manager.addCondition(
    "FullPointer", &motionDetected,
    LogicConditionManager::ON_CHANGE,
    []() {
      Serial.println("🔄 Motion state changed!");
      tone(BUZZER_PIN, 2000, 150);
    },
    -1,    // repeatCount = -1 (unlimited)
    800,   // interval = 800ms
    100);  // debounceTime = 100ms
  Serial.println("✓ 2B. Pointer lengkap: repeat=unlimited, interval=800ms, debounce=100ms");
  Serial.println("     Fungsi: Monitor perubahan variable, unlimited repeat");

  // ==============================================
  // 3. PARAMETERIZED ACTION CONDITIONS
  // ==============================================
  Serial.println("\n3. PARAMETERIZED ACTION CONDITIONS");
  Serial.println("===================================");

  // 3A. Function dengan parameterized action
  int paramFuncId = manager.addCondition(
    "ParamFunction",
    []() -> bool {
      return digitalRead(MOTION_PIN) == HIGH;
    },
    LogicConditionManager::TO_TRUE,
    [](void* param) {
      String* msg = (String*)param;
      Serial.println("📨 Parameterized: " + *msg);
      Serial.println("🎯 Custom message delivered!");
    },
    &alertMessage,
    2,     // repeatCount = 2
    1500,  // interval = 1500ms
    250);  // debounceTime = 250ms
  Serial.println("✓ 3A. Function + param action: repeat=2, interval=1.5s, debounce=250ms");
  Serial.println("     Fungsi: Kirim parameter ke action function");

  // 3B. Pointer dengan parameterized action
  int paramPtrId = manager.addCondition(
    "ParamPointer", &doorOpen,
    LogicConditionManager::WHEN_FALSE,
    [](void* param) {
      int* speed = (int*)param;
      Serial.println("🌀 Fan speed set to: " + String(*speed) + "%");
      *speed += 10;                   // Modify parameter
      if (*speed > 100) *speed = 50;  // Reset if too high
    },
    &fanSpeed,
    5,     // repeatCount = 5
    2000,  // interval = 2000ms
    0);    // debounceTime = 0 (no debounce)
  Serial.println("✓ 3B. Pointer + param action: repeat=5, interval=2s, no debounce");
  Serial.println("     Fungsi: Modifikasi parameter dalam action");

  // ==============================================
  // 4. VALUE CONDITIONS
  // ==============================================
  Serial.println("\n4️⃣ VALUE CONDITIONS");
  Serial.println("===================");

  // 4A. Value function - basic
  int valueBasicId = manager.addValueCondition(
    "ValueBasic",
    []() -> float {
      int raw = analogRead(A0);
      return (raw * 3.3 / 4095.0 - 0.5) * 100.0;  // Temperature calculation
    },
    [](float temp) -> bool {
      return temp > 30.0;
    },  // Evaluator
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("🌡️ Temperature above 30°C!");
    });
  Serial.println("✓ 4A. Value function basic: temp sensor");
  Serial.println("     Fungsi: Monitor nilai analog dan evaluasi kondisi");

  // 4B. Value function dengan parameter lengkap
  int valueFullId = manager.addValueCondition(
    "ValueFull",
    []() -> float {
      return random(0, 100);  // Simulate humidity sensor
    },
    [](float hum) -> bool {
      return hum < 40.0 || hum > 80.0;
    },  // Out of range
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("💧 Humidity out of range!");
      digitalWrite(LED_PIN, HIGH);
      delay(300);
      digitalWrite(LED_PIN, LOW);
    },
    3,      // repeatCount = 3
    2500);  // interval = 2500ms
  Serial.println("✓ 4B. Value function lengkap: repeat=3, interval=2.5s");
  Serial.println("     Fungsi: Monitor range nilai dengan repeat action");

  // 4C. Value pointer
  int valuePtrId = manager.addValueCondition(
    "ValuePointer", &temperature,
    [](float temp) -> bool {
      return temp >= 20.0 && temp <= 25.0;
    },  // Optimal range
    LogicConditionManager::WHEN_TRUE,
    []() {
      Serial.println("✅ Temperature in optimal range!");
    },
    -1,     // repeatCount = unlimited
    1000);  // interval = 1000ms
  Serial.println("✓ 4C. Value pointer: monitor variable, unlimited repeat");
  Serial.println("     Fungsi: Monitor variable float dengan range check");

  // ==============================================
  // 5. COMPOSITE CONDITIONS
  // ==============================================
  Serial.println("\n5️⃣ COMPOSITE CONDITIONS");
  Serial.println("=======================");

  // Prepare condition arrays
  int sensorIds[] = { basicId, valueBasicId, ptrBasicId };
  int motionIds[] = { paramFuncId, ptrFullId };

  // 5A. AND Condition
  int andId = manager.addCompositeCondition(
    "AndCondition", LogicConditionManager::AND,
    sensorIds, 3,
    []() {
      Serial.println("🤝 ALL conditions met (AND)!");
      for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
      }
    },
    2,      // repeatCount = 2
    3000);  // interval = 3000ms
  Serial.println("✓ 5A. AND composite: repeat=2, interval=3s");
  Serial.println("     Fungsi: Semua kondisi harus true, action 2x dengan jeda 3s");

  // 5B. OR Condition dengan parameter lengkap
  int orId = manager.addCompositeCondition(
    "OrCondition", LogicConditionManager::OR,
    motionIds, 2,
    []() {
      Serial.println("🎯 ANY condition met (OR)!");
      tone(BUZZER_PIN, 1500, 200);
    },
    -1,     // repeatCount = unlimited
    1200);  // interval = 1200ms
  Serial.println("✓ 5B. OR composite: unlimited repeat, interval=1.2s");
  Serial.println("     Fungsi: Salah satu kondisi true, unlimited repeat");

  // 5C. XOR Condition
  int xorId = manager.addCompositeCondition(
    "XorCondition", LogicConditionManager::XOR,
    sensorIds, 3,
    []() {
      Serial.println("⚡ EXACTLY ONE condition true (XOR)!");
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
    },
    1,   // repeatCount = 1
    0);  // interval = 0 (immediate)
  Serial.println("✓ 5C. XOR composite: single action, immediate");
  Serial.println("     Fungsi: Hanya satu kondisi true, action langsung");

  // ==============================================
  // 6. SHORTCUT COMPOSITE CONDITIONS
  // ==============================================
  Serial.println("\n6️⃣ SHORTCUT COMPOSITE CONDITIONS");
  Serial.println("=================================");

  // 6A. AND shortcut
  int andShortId = manager.addAndCondition(
    "AndShortcut", sensorIds, 2,
    []() {
      Serial.println("🔗 AND shortcut triggered!");
    },
    4,     // repeatCount = 4
    800);  // interval = 800ms
  Serial.println("✓ 6A. addAndCondition shortcut: repeat=4, interval=800ms");

  // 6B. OR shortcut
  int orShortId = manager.addOrCondition(
    "OrShortcut", motionIds, 2,
    []() {
      Serial.println("🌟 OR shortcut triggered!");
    },
    1,   // repeatCount = 1
    0);  // interval = 0
  Serial.println("✓ 6B. addOrCondition shortcut: single immediate action");

  // 6C. NOT shortcut
  int notShortId = manager.addNotCondition(
    "NotShortcut", basicId,
    []() {
      Serial.println("🚫 NOT shortcut triggered!");
    },
    -1,     // repeatCount = unlimited
    2000);  // interval = 2000ms
  Serial.println("✓ 6C. addNotCondition shortcut: unlimited, interval=2s");

  // ==============================================
  // 7. TIMER CONDITIONS
  // ==============================================
  Serial.println("\n7️⃣ TIMER CONDITIONS");
  Serial.println("===================");

  // 7A. Basic timer
  int timerBasicId = manager.addTimerCondition("BasicTimer", 5000, false,
                                               []() {
                                                 Serial.println("⏰ 5-second timer completed!");
                                               });
  Serial.println("✓ 7A. Basic timer: 5s, one-shot");
  Serial.println("     Fungsi: Timer 5 detik, trigger sekali");

  // 7B. Auto-reset timer
  int timerAutoId = manager.addTimerCondition(
    "AutoTimer", 3000, true,
    []() {
      Serial.println("🔄 Auto-reset timer: " + String(millis()));
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
    },
    -1,    // repeatCount = unlimited
    500);  // interval = 500ms between repeats
  Serial.println("✓ 7B. Auto-reset timer: 3s, unlimited repeat, 500ms interval");
  Serial.println("     Fungsi: Timer reset otomatis, action unlimited dengan jeda");

  // 7C. Timer dengan repeat terbatas
  int timerRepeatId = manager.addTimerCondition(
    "RepeatTimer", 2000, true,
    []() {
      Serial.println("🎯 Limited repeat timer!");
      tone(BUZZER_PIN, 800, 150);
    },
    5,      // repeatCount = 5
    1000);  // interval = 1000ms
  Serial.println("✓ 7C. Timer dengan repeat=5, interval=1s");
  Serial.println("     Fungsi: Timer auto-reset, action 5x dengan jeda 1s");

  // ==============================================
  // 8. SEQUENCE CONDITIONS
  // ==============================================
  Serial.println("\n8️⃣ SEQUENCE CONDITIONS");
  Serial.println("======================");

  // 8A. Basic sequence
  int sequenceIds1[] = { basicId, paramFuncId, valueBasicId };
  int seqBasicId = manager.addSequenceCondition("BasicSequence", sequenceIds1, 3,
                                                0, 3,  // maxTimespan=0 (unlimited), minOccurrences=3
                                                []() {
                                                  Serial.println("📝 Basic sequence completed!");
                                                });
  Serial.println("✓ 8A. Basic sequence: 3 kondisi berurutan, unlimited time");
  Serial.println("     Fungsi: Tunggu 3 kondisi terjadi berurutan tanpa batas waktu");

  // 8B. Sequence dengan timespan
  int sequenceIds2[] = { ptrBasicId, valueFullId };
  int seqTimeId = manager.addSequenceCondition(
    "TimedSequence", sequenceIds2, 2,
    10000, 2,  // maxTimespan=10s, minOccurrences=2
    []() {
      Serial.println("⏱️ Timed sequence completed in 10s!");
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        delay(50);
      }
    },
    2);  // repeatCount = 2
  Serial.println("✓ 8B. Timed sequence: 2 kondisi dalam 10s, repeat=2");
  Serial.println("     Fungsi: Sequence harus selesai dalam 10 detik, bisa repeat 2x");

  // 8C. Sequence dengan minimum occurrences
  int sequenceIds3[] = { basicId, ptrFullId, paramFuncId, valueBasicId };
  int seqMinId = manager.addSequenceCondition(
    "MinSequence", sequenceIds3, 4,
    15000, 2,  // maxTimespan=15s, minOccurrences=2 (tidak perlu semua)
    []() {
      Serial.println("🎪 Minimum sequence achieved!");
      tone(BUZZER_PIN, 1200, 300);
    },
    1);  // repeatCount = 1
  Serial.println("✓ 8C. Min sequence: 4 kondisi, minimal 2 dalam 15s");
  Serial.println("     Fungsi: Dari 4 kondisi, minimal 2 harus terjadi dalam 15s");

  // ==============================================
  // 9. EDGE COUNTER CONDITIONS
  // ==============================================
  Serial.println("\n9️⃣ EDGE COUNTER CONDITIONS");
  Serial.println("===========================");

  // 9A. Basic edge counter
  int edgeBasicId = manager.addEdgeCountCondition("BasicEdge", basicId,
                                                  3, 5000,  // 3 edges in 5 seconds
                                                  []() {
                                                    Serial.println("🔢 3 button presses in 5s!");
                                                  });
  Serial.println("✓ 9A. Basic edge: 3 triggers dalam 5s");
  Serial.println("     Fungsi: Hitung edge/trigger dalam time window");

  // 9B. Edge counter dengan repeat
  int edgeRepeatId = manager.addEdgeCountCondition(
    "RepeatEdge", ptrFullId,
    5, 10000,  // 5 edges in 10 seconds
    []() {
      Serial.println("🚀 Rapid activity detected!");
      for (int i = 0; i < 10; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(30);
        digitalWrite(LED_PIN, LOW);
        delay(30);
      }
    },
    3);  // repeatCount = 3
  Serial.println("✓ 9B. Edge repeat: 5 triggers dalam 10s, repeat=3");
  Serial.println("     Fungsi: Deteksi aktivitas cepat, bisa repeat 3x");

  // 9C. High-frequency edge counter
  int edgeHighId = manager.addEdgeCountCondition(
    "HighFreqEdge", debounceId,
    10, 20000,  // 10 edges in 20 seconds
    []() {
      Serial.println("⚡ HIGH FREQUENCY ACTIVITY!");
      tone(BUZZER_PIN, 2000, 500);
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
    },
    1);  // repeatCount = 1
  Serial.println("✓ 9C. High freq edge: 10 triggers dalam 20s, single action");
  Serial.println("     Fungsi: Deteksi aktivitas frekuensi tinggi");

  // ==============================================
  // SUMMARY DAN STATUS
  // ==============================================
  Serial.println("\n📊 RINGKASAN PARAMETER FUNCTIONS:");
  Serial.println("==================================");
  Serial.println("🔁 repeatCount:");
  Serial.println("   - 1: Action sekali saja");
  Serial.println("   - >1: Action sebanyak N kali");
  Serial.println("   - -1: Unlimited (sampai disabled)");
  Serial.println("");
  Serial.println("⏱️ interval:");
  Serial.println("   - 0: Action langsung tanpa jeda");
  Serial.println("   - >0: Jeda antar action dalam ms");
  Serial.println("   - Hanya berlaku jika repeatCount > 1");
  Serial.println("");
  Serial.println("🚫 debounceTime:");
  Serial.println("   - 0: Tidak ada debounce");
  Serial.println("   - >0: Ignore trigger dalam N ms setelah trigger terakhir");
  Serial.println("   - Berguna untuk anti-noise/bouncing");
  Serial.println("");
  Serial.println("🎯 Parameter khusus:");
  Serial.println("   - Timer: duration, autoReset");
  Serial.println("   - Sequence: maxTimespan, minOccurrences");
  Serial.println("   - EdgeCounter: targetCount, timeWindow");

  Serial.println("\n📋 Initial status:");
  manager.printAllStatus();

  Serial.println("\n🎮 Testing all addCondition variations...");
}

// ==============================================
// MAIN LOOP
// ==============================================
void loop() {
  static unsigned long lastUpdate = 0;
  static int loopCount = 0;

  // Update all conditions
  manager.update();

  // Simulate sensor changes every 2 seconds
  if (millis() - lastUpdate > 2000) {
    // Simulate random sensor states
    buttonState = random(0, 10) > 7;     // 30% chance
    motionDetected = random(0, 10) > 6;  // 40% chance
    doorOpen = random(0, 10) > 8;        // 20% chance
    temperature = random(15, 35);        // 15-35°C
    humidity = random(30, 90);           // 30-90%
    pressure = random(990, 1030);        // 990-1030 hPa

    Serial.println("\n📊 Sensor Update #" + String(loopCount++));
    Serial.println("🔘 Button: " + String(buttonState ? "PRESSED" : "RELEASED"));
    Serial.println("👁️ Motion: " + String(motionDetected ? "DETECTED" : "NONE"));
    Serial.println("🚪 Door: " + String(doorOpen ? "OPEN" : "CLOSED"));
    Serial.println("🌡️ Temp: " + String(temperature) + "°C");
    Serial.println("💧 Humidity: " + String(humidity) + "%");

    lastUpdate = millis();
  }

  // Demonstrate condition states every 10 seconds
  if (loopCount > 0 && loopCount % 5 == 0) {
    Serial.println("\n🔍 Condition Status Sample:");
    manager.printStatus("BasicButton");
    manager.printStatus("FullParamButton");
    manager.printStatus("AutoTimer");
    manager.printStatus("BasicSequence");
    manager.printStatus("BasicEdge");
  }

  // Reset demo every 50 updates
  if (loopCount >= 50) {
    Serial.println("\n🔄 Demo reset - all conditions reset");
    loopCount = 0;
  }

  delay(100);
}