#define ENABLE_MODULE_LOGIC_CONDITION_MANAGER
#include "Kinematrix.h"

LogicConditionManager logic;

// Input conditions for logic operations
bool inputA = false;
bool inputB = false;
bool inputC = false;

void setup() {
        Serial.begin(9600);

        pinMode(2, INPUT_PULLUP);  // Input A
        pinMode(3, INPUT_PULLUP);  // Input B
        pinMode(4, INPUT_PULLUP);  // Input C
        pinMode(5, OUTPUT);        // AND result LED
        pinMode(6, OUTPUT);        // OR result LED
        pinMode(7, OUTPUT);        // NOT result LED
        pinMode(8, OUTPUT);        // XOR result LED
        pinMode(9, OUTPUT);        // NAND result LED
        pinMode(10, OUTPUT);       // NOR result LED
        pinMode(11, OUTPUT);       // Buzzer

        Serial.println("=== LogicOp Demonstration System ===");
        Serial.println("Pin 2: Input A");
        Serial.println("Pin 3: Input B");
        Serial.println("Pin 4: Input C");
        Serial.println("LEDs 5-10: AND, OR, NOT, XOR, NAND, NOR results");
        Serial.println();

        initializeLogicOpExamples();
        printTruthTable();
}

void initializeLogicOpExamples() {
        // Create basic input conditions
        int inputA_id = logic.addCondition(
            "inputA",
            &inputA,
            LogicConditionManager::WHEN_TRUE);

        int inputB_id = logic.addCondition(
            "inputB",
            &inputB,
            LogicConditionManager::WHEN_TRUE);

        int inputC_id = logic.addCondition(
            "inputC",
            &inputC,
            LogicConditionManager::WHEN_TRUE);

        // 1. AND - True only when ALL inputs are true
        logic.addAndCondition(
            "logic_AND",
            new int[2]{inputA_id, inputB_id},
            2,
            []() {
                    Serial.println("✅ AND: Both A AND B are true");
                    digitalWrite(5, HIGH);
                    tone(11, 1000, 100);
            });

        // 2. OR - True when ANY input is true
        logic.addOrCondition(
            "logic_OR",
            new int[2]{inputA_id, inputB_id},
            2,
            []() {
                    Serial.println("✅ OR: A OR B (or both) is true");
                    digitalWrite(6, HIGH);
            });

        // 3. NOT - Inverts the input (true becomes false, false becomes true)
        logic.addNotCondition(
            "logic_NOT",
            inputA_id,
            []() {
                    Serial.println("✅ NOT: A is false (NOT A = true)");
                    digitalWrite(7, HIGH);
            });

        // 4. XOR - True when inputs are different (exclusive OR)
        logic.addCompositeCondition(
            "logic_XOR",
            LogicConditionManager::XOR,
            new int[2]{inputA_id, inputB_id},
            2,
            []() {
                    Serial.println("✅ XOR: A and B are different (exclusive OR)");
                    digitalWrite(8, HIGH);
                    tone(11, 1500, 150);
            });

        // 5. NAND - NOT AND (false only when ALL inputs are true)
        logic.addCompositeCondition(
            "logic_NAND",
            LogicConditionManager::NAND,
            new int[2]{inputA_id, inputB_id},
            2,
            []() {
                    Serial.println("✅ NAND: NOT(A AND B) - at least one is false");
                    digitalWrite(9, HIGH);
            });

        // 6. NOR - NOT OR (true only when ALL inputs are false)
        logic.addCompositeCondition(
            "logic_NOR",
            LogicConditionManager::NOR,
            new int[2]{inputA_id, inputB_id},
            2,
            []() {
                    Serial.println("✅ NOR: NOT(A OR B) - both are false");
                    digitalWrite(10, HIGH);
                    tone(11, 2000, 200);
            });

        // Complex example: 3-input AND
        logic.addCompositeCondition(
            "triple_AND",
            LogicConditionManager::AND,
            new int[3]{inputA_id, inputB_id, inputC_id},
            3,
            []() {
                    Serial.println("🎯 TRIPLE AND: A AND B AND C all true!");
                    // Special celebration
                    for (int i = 0; i < 3; i++) {
                            tone(11, 3000, 100);
                            delay(150);
                    }
            });

        // Complex example: Mixed logic - (A AND B) OR C
        int and_AB = logic.addCompositeCondition(
            "A_AND_B",
            LogicConditionManager::AND,
            new int[2]{inputA_id, inputB_id},
            2);

        logic.addCompositeCondition(
            "mixed_logic",
            LogicConditionManager::OR,
            new int[2]{and_AB, inputC_id},
            2,
            []() {
                    Serial.println("🧠 MIXED: (A AND B) OR C is true");
            });

        Serial.println("All LogicOp examples initialized!");
        Serial.println("Change pin states to see different logic operations");
        Serial.println();
}

void loop() {
        // Read input states
        bool newA = digitalRead(2) == LOW;  // Active when pressed (pullup)
        bool newB = digitalRead(3) == LOW;
        bool newC = digitalRead(4) == LOW;

        // Detect and announce changes
        if (newA != inputA) {
                inputA = newA;
                Serial.print("🔧 Input A changed to: ");
                Serial.println(inputA ? "TRUE" : "FALSE");
        }
        if (newB != inputB) {
                inputB = newB;
                Serial.print("🔧 Input B changed to: ");
                Serial.println(inputB ? "TRUE" : "FALSE");
        }
        if (newC != inputC) {
                inputC = newC;
                Serial.print("🔧 Input C changed to: ");
                Serial.println(inputC ? "TRUE" : "FALSE");
        }

        // Clear all LEDs first
        for (int pin = 5; pin <= 10; pin++) {
                digitalWrite(pin, LOW);
        }

        // Update logic manager
        logic.update();

        // Show current logic states
        static unsigned long lastDisplay = 0;
        if (millis() - lastDisplay > 3000) {
                displayLogicStates();
                lastDisplay = millis();
        }

        // Auto demo mode
        static unsigned long lastDemo = 0;
        if (millis() - lastDemo > 20000) {  // Every 20 seconds
                runLogicDemo();
                lastDemo = millis();
        }
}

void displayLogicStates() {
        Serial.println("\n--- Current Logic States ---");
        Serial.print("Inputs: A=");
        Serial.print(inputA ? "1" : "0");
        Serial.print(" B=");
        Serial.print(inputB ? "1" : "0");
        Serial.print(" C=");
        Serial.println(inputC ? "1" : "0");

        Serial.print("Results: ");
        Serial.print("AND=");
        Serial.print(logic.getCurrentBooleanValue("logic_AND") ? "1" : "0");
        Serial.print(" OR=");
        Serial.print(logic.getCurrentBooleanValue("logic_OR") ? "1" : "0");
        Serial.print(" NOT=");
        Serial.print(logic.getCurrentBooleanValue("logic_NOT") ? "1" : "0");
        Serial.print(" XOR=");
        Serial.print(logic.getCurrentBooleanValue("logic_XOR") ? "1" : "0");
        Serial.print(" NAND=");
        Serial.print(logic.getCurrentBooleanValue("logic_NAND") ? "1" : "0");
        Serial.print(" NOR=");
        Serial.println(logic.getCurrentBooleanValue("logic_NOR") ? "1" : "0");
        Serial.println("-------------------------");
}

void runLogicDemo() {
        Serial.println("\n🎭 AUTO DEMO: Testing all logic combinations");

        // Test all 4 combinations of A and B
        bool testInputs[4][2] = {
            {false, false},
            {false, true},
            {true, false},
            {true, true}};

        for (int i = 0; i < 4; i++) {
                inputA = testInputs[i][0];
                inputB = testInputs[i][1];
                inputC = false;  // Keep C false for basic demo

                Serial.print("Testing A=");
                Serial.print(inputA ? "1" : "0");
                Serial.print(" B=");
                Serial.print(inputB ? "1" : "0");
                Serial.print(" → ");

                logic.update();

                Serial.print("AND=");
                Serial.print(logic.getCurrentBooleanValue("logic_AND") ? "1" : "0");
                Serial.print(" OR=");
                Serial.print(logic.getCurrentBooleanValue("logic_OR") ? "1" : "0");
                Serial.print(" XOR=");
                Serial.print(logic.getCurrentBooleanValue("logic_XOR") ? "1" : "0");
                Serial.print(" NAND=");
                Serial.print(logic.getCurrentBooleanValue("logic_NAND") ? "1" : "0");
                Serial.print(" NOR=");
                Serial.println(logic.getCurrentBooleanValue("logic_NOR") ? "1" : "0");

                delay(1000);
        }

        // Reset to current pin states
        inputA = digitalRead(2) == LOW;
        inputB = digitalRead(3) == LOW;
        inputC = digitalRead(4) == LOW;

        Serial.println("Demo complete! Back to manual control.\n");
}

void printTruthTable() {
        Serial.println("\n📊 LOGIC OPERATIONS TRUTH TABLE");
        Serial.println("A | B | AND | OR | NOT A | XOR | NAND | NOR");
        Serial.println("--|---|-----|----|----- |----- |------|----");
        Serial.println("0 | 0 |  0  | 0  |  1   |  0  |  1   | 1");
        Serial.println("0 | 1 |  0  | 1  |  1   |  1  |  1   | 0");
        Serial.println("1 | 0 |  0  | 1  |  0   |  1  |  1   | 0");
        Serial.println("1 | 1 |  1  | 1  |  0   |  0  |  0   | 0");
        Serial.println("---------------------------------------");
        Serial.println("AND: True when both inputs true");
        Serial.println("OR:  True when any input true");
        Serial.println("NOT: Inverts the input");
        Serial.println("XOR: True when inputs different");
        Serial.println("NAND: NOT AND (opposite of AND)");
        Serial.println("NOR: NOT OR (opposite of OR)");
        Serial.println("---------------------------------------\n");
}