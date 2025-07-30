# Fuzzy Tsukamoto: Comprehensive Technical Documentation - Part 1

## Table of Contents
1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Introduction and Definitions](#introduction-and-definitions)
3. [Theoretical Foundation](#theoretical-foundation)
4. [Mathematical Framework](#mathematical-framework)
5. [Implementation Architecture](#implementation-architecture)

---

## 🚀 Quick Start for Beginners

### Apa itu Fuzzy Tsukamoto? (In Simple Terms)

Bayangkan Anda **mengatur kecepatan motor listrik** berdasarkan suhu dan beban:

**Tanpa Fuzzy (Traditional Logic):**
```
IF temperature > 30°C THEN motor_speed = 100%
ELSE motor_speed = 50%
```
Hasil: **Motor berubah tiba-tiba!**

**Dengan Fuzzy Tsukamoto:**
```
IF temperature is HOT AND load is HEAVY THEN speed is FAST
IF temperature is WARM OR load is MEDIUM THEN speed is MEDIUM  
IF temperature is COOL THEN speed is SLOW
```
Hasil: **Motor berubah smooth dengan respons monoton yang stabil!**

### Visual Example: Smart Motor Speed Control

Mari lihat bagaimana Fuzzy Tsukamoto bekerja untuk kontrol kecepatan motor:

```
Temperature (°C)
    ^
    |  COOL    WARM    HOT
 1.0|   /\      /\      /\
    |  /  \    /  \    /  \
 0.5| /    \  /    \  /    \
    |/      \/      \/      \
 0.0+--------+--------+--------→
   15      25      35      45

Load (%)
    ^
    |  LIGHT  MEDIUM  HEAVY  
 1.0|   /\      /\      /\
    |  /  \    /  \    /  \
 0.5| /    \  /    \  /    \
    |/      \/      \/      \
 0.0+--------+--------+--------→
   20      50      80     100

Motor Speed (%) - MONOTONIC OUTPUT FUNCTIONS
    ^
    | SLOW(Z)  MEDIUM(S) FAST(S)
 1.0|  \        /        /
    |   \      /        /
 0.5|    \    /        /
    |     \  /        /
 0.0+------\/--------/--------→
    0     50       100

Note: Z = Z-shaped (decreasing), S = S-shaped (increasing)
```

**Step-by-Step Tsukamoto Process:**
1. **Fuzzification**: Input 32°C, 70% load → "HOT" (0.6), "HEAVY" (0.7)
2. **Rule Evaluation**: Check aturan dengan AND/OR
3. **Height Defuzzification**: Cari nilai crisp dimana membership = rule strength
4. **Weighted Average**: Kombinasi semua output → Final speed 78%

### 🔍 Step-by-Step Example: Temperature-Based Fan Control

Mari kita buat sistem kontrol kipas berdasarkan suhu step by step:

**Target**: Mengatur kecepatan kipas berdasarkan suhu ruangan dengan respons monoton

**Step 1: Setup Variables**
```cpp
#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

FuzzyTsukamoto fanController(1, 1, 3, 3);  // 1 input, 1 output, 3 rules, 3 sets

void setup() {
    // Input: Temperature (15-40°C)
    fanController.addInputVariable("temperature", 15.0, 40.0);
    
    // Output: Fan Speed (0-100%)
    fanController.addOutputVariable("fan_speed", 0.0, 100.0);
}
```

**Step 2: Define Input Fuzzy Sets (Non-monotonic OK)**
```cpp
void setupInputSets() {
    // Temperature fuzzy sets (triangular membership functions)
    float tempCool[] = {15, 15, 25};    fanController.addFuzzySet(0, true, "COOL", TRIANGULAR, tempCool);
    float tempWarm[] = {20, 27, 34};    fanController.addFuzzySet(0, true, "WARM", TRIANGULAR, tempWarm);
    float tempHot[] = {30, 40, 40};     fanController.addFuzzySet(0, true, "HOT", TRIANGULAR, tempHot);
}
```

**Step 3: Define Output Fuzzy Sets (MUST BE MONOTONIC)**
```cpp
void setupOutputSets() {
    // Fan speed fuzzy sets - ALL MUST BE MONOTONIC!
    
    // Z-shaped (decreasing monotonic) for SLOW
    float fanSlow[] = {0, 40};          fanController.addFuzzySet(0, false, "SLOW", Z_SHAPED, fanSlow);
    
    // S-shaped (increasing monotonic) for MEDIUM  
    float fanMedium[] = {30, 70};       fanController.addFuzzySet(0, false, "MEDIUM", S_SHAPED, fanMedium);
    
    // S-shaped (increasing monotonic) for FAST
    float fanFast[] = {60, 100};        fanController.addFuzzySet(0, false, "FAST", S_SHAPED, fanFast);
}
```

**Step 4: Create Fuzzy Rules**
```cpp
void setupFuzzyRules() {
    // Rule 1: IF temp is COOL THEN fan_speed is SLOW
    int coolRule[] = {0};  // COOL temperature set
    fanController.addRule(coolRule, 1, 0, 0, true);  // Output: SLOW fan
    
    // Rule 2: IF temp is WARM THEN fan_speed is MEDIUM
    int warmRule[] = {1};  // WARM temperature set
    fanController.addRule(warmRule, 1, 0, 1, true);  // Output: MEDIUM fan
    
    // Rule 3: IF temp is HOT THEN fan_speed is FAST
    int hotRule[] = {2};   // HOT temperature set
    fanController.addRule(hotRule, 1, 0, 2, true);   // Output: FAST fan
}
```

**Step 5: Manual Calculation Example**

Input: Temperature = 28°C

**Fuzzification:**
```
Temperature 28°C:
- COOL: μ = 0 (28 > 25)
- WARM: μ = (34-28)/(34-27) = 6/7 = 0.857
- HOT: μ = (28-30)/(40-30) = 0 (28 < 30)
```

**Rule Evaluation:**
```
Rule 1: temp is COOL → μ = 0 → NOT FIRED
Rule 2: temp is WARM → μ = 0.857 → FIRED (MEDIUM fan)
Rule 3: temp is HOT → μ = 0 → NOT FIRED
```

**Height Defuzzification (Key Tsukamoto Feature):**
```
For MEDIUM fan with strength 0.857:
- Find x where μ_MEDIUM(x) = 0.857
- S-shaped function: μ(x) = S(x; 30, 70)
- Solve: 0.857 = S(x; 30, 70)
- Result: x ≈ 62.5% fan speed

Since only one rule fired:
Final output = 62.5%
```

**Complete Implementation:**
```cpp
#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

FuzzyTsukamoto fanController(1, 1, 3, 3);

#define TEMP_SENSOR_PIN A0
#define FAN_PWM_PIN 9

void setup() {
    Serial.begin(115200);
    setupFuzzyVariables();
    setupInputSets();
    setupOutputSets();
    setupFuzzyRules();
    
    fanController.setDefuzzificationMethod(HEIGHT);  // Tsukamoto uses HEIGHT method
    Serial.println("Fuzzy Tsukamoto Fan Controller Ready!");
}

void loop() {
    // Read temperature sensor
    float temperature = readTemperature();
    
    // Prepare input
    float inputs[] = {temperature};
    
    // Fuzzy inference using Tsukamoto method
    float *outputs = fanController.evaluate(inputs);
    
    if (outputs != nullptr) {
        float fanSpeed = outputs[0];
        
        // Apply control signal to fan
        int pwmValue = (int)(fanSpeed * 2.55);  // Convert to 0-255
        analogWrite(FAN_PWM_PIN, pwmValue);
        
        // Debug output
        Serial.print("Temp: " + String(temperature, 1) + "°C");
        Serial.println(" | Fan Speed: " + String(fanSpeed, 1) + "%");
        
        delete[] outputs;  // Free memory
    }
    
    delay(1000);
}

float readTemperature() {
    // LM35: 10mV/°C
    int rawValue = analogRead(TEMP_SENSOR_PIN);
    float voltage = (rawValue * 5.0) / 1023.0;
    return voltage * 100.0;  // Convert to Celsius
}
```

### 🎯 Key Insights: Mengapa Tsukamoto Unik?

**Keunggulan Tsukamoto vs Mamdani:**
- **Monotonic Output**: Semua fungsi output harus monoton (naik/turun konsisten)
- **Height Defuzzification**: Lebih cepat dan efisien daripada centroid
- **Unique Mapping**: Setiap rule strength menghasilkan nilai output yang unik
- **Smooth Response**: Transisi output sangat halus dan predictable
- **Real-time Friendly**: Komputasi lebih ringan untuk embedded systems

**Kapan Pakai Fuzzy Tsukamoto?**
- Motor speed control (smooth acceleration/deceleration)
- Temperature regulation (gradual heating/cooling)
- Pressure control systems (stable pressure transitions)
- Water level control (smooth valve operations)
- Audio volume control (natural volume transitions)
- Any system yang butuh smooth, monotonic response

### ⚡ 5-Minute Quick Start Template

```cpp
#define ENABLE_MODULE_FUZZY_TSUKAMOTO
#include "Kinematrix.h"

FuzzyTsukamoto fuzzy(1, 1, 3, 3);  // 1 input, 1 output, 3 rules, 3 sets

void setup() {
    // Step 1: Add variables
    fuzzy.addInputVariable("input", 0.0, 100.0);
    fuzzy.addOutputVariable("output", 0.0, 100.0);
    
    // Step 2: Add input fuzzy sets (can be any shape)
    float low[] = {0, 0, 40};     fuzzy.addFuzzySet(0, true, "LOW", TRIANGULAR, low);
    float med[] = {30, 50, 70};   fuzzy.addFuzzySet(0, true, "MED", TRIANGULAR, med);
    float high[] = {60, 100, 100}; fuzzy.addFuzzySet(0, true, "HIGH", TRIANGULAR, high);
    
    // Step 3: Add output fuzzy sets (MUST BE MONOTONIC)
    float small[] = {0, 50};      fuzzy.addFuzzySet(0, false, "SMALL", Z_SHAPED, small);
    float medium[] = {25, 75};    fuzzy.addFuzzySet(0, false, "MEDIUM", S_SHAPED, medium);
    float large[] = {50, 100};    fuzzy.addFuzzySet(0, false, "LARGE", S_SHAPED, large);
    
    // Step 4: Add rules
    int rule1[] = {0}; fuzzy.addRule(rule1, 1, 0, 0, true); // LOW→SMALL
    int rule2[] = {1}; fuzzy.addRule(rule2, 1, 0, 1, true); // MED→MEDIUM  
    int rule3[] = {2}; fuzzy.addRule(rule3, 1, 0, 2, true); // HIGH→LARGE
}

void loop() {
    float inputs[] = {analogRead(A0) * 100.0 / 1023.0};  // Read sensor (0-100)
    float *outputs = fuzzy.evaluate(inputs);
    
    if (outputs) {
        analogWrite(9, outputs[0] * 255.0 / 100.0);  // Apply output (0-255 PWM)
        delete[] outputs;
    }
    delay(100);
}
```

---

## 1. Introduction and Definitions

### 1.1 Fuzzy Tsukamoto Definition

**Fuzzy Tsukamoto Controller** adalah sistem fuzzy inference yang dikembangkan oleh Yoshikazu Tsukamoto. Sistem ini menggunakan **monotonic membership functions** untuk output dan **height defuzzification method** yang sangat efisien untuk aplikasi real-time [Tsukamoto, 1979].

### 1.2 Key Characteristics

**Monotonic Output Functions**: Semua fungsi keanggotaan output harus monoton (strictly increasing atau decreasing).

**Height Defuzzification**: Menggunakan metode "height" dimana output crisp diperoleh dari weighted average.

**Direct Mapping**: Setiap rule strength langsung dipetakan ke nilai output tanpa agregasi kompleks.

**Computational Efficiency**: Lebih cepat dari Mamdani karena tidak perlu agregasi dan centroid calculation.

### 1.3 Tsukamoto vs Mamdani vs Sugeno

| Aspect | Tsukamoto | Mamdani | Sugeno |
|--------|-----------|---------|---------|
| **Output Functions** | Monotonic fuzzy sets | Any fuzzy sets | Crisp functions |
| **Defuzzification** | Height method | Centroid/MOM | Weighted average |
| **Computational Speed** | Fast | Moderate | Fastest |
| **Output Smoothness** | Very smooth | Smooth | Less smooth |
| **Implementation** | Medium complexity | High complexity | Low complexity |
| **Applications** | Smooth control | General control | Mathematical modeling |

### 1.4 Historical Background

- **1979**: Tsukamoto memperkenalkan metode inference dengan monotonic functions
- **1985**: Aplikasi pertama dalam industrial process control
- **1990s**: Pengembangan algoritma optimized untuk embedded systems
- **2000s**: Adopsi luas dalam automotive dan consumer electronics
- **2010s**: Integration dengan IoT dan smart systems

### 1.5 Real-World Applications

**🏠 Smart Home Automation**
```
Temperature Control: IF room is COLD THEN heater_power is INCREASING_MONOTONIC
Light Dimming: IF ambient is DARK THEN brightness is INCREASING_MONOTONIC
Fan Speed: IF temperature is HIGH THEN fan_speed is INCREASING_MONOTONIC
```

**🚗 Automotive Systems**
```
Cruise Control: IF speed_error is POSITIVE THEN throttle is INCREASING_MONOTONIC
ABS Control: IF wheel_slip is HIGH THEN brake_pressure is DECREASING_MONOTONIC
Transmission: IF load is HEAVY THEN gear_ratio is DECREASING_MONOTONIC
```

**🏭 Industrial Process Control**
```
Flow Control: IF flow_error is POSITIVE THEN valve_opening is INCREASING_MONOTONIC
Pressure Control: IF pressure_low THEN compressor_speed is INCREASING_MONOTONIC
Temperature Regulation: IF temp_high THEN cooling_power is INCREASING_MONOTONIC
```

**🤖 Robotics Applications**
```
Motor Speed: IF load is HEAVY THEN torque is INCREASING_MONOTONIC
Gripper Control: IF object_slip THEN grip_force is INCREASING_MONOTONIC
Joint Control: IF position_error THEN actuator_power is INCREASING_MONOTONIC
```

### 1.6 Why Tsukamoto for Embedded Systems?

**🔋 Computational Advantages**
- Lower computational complexity than Mamdani
- Predictable execution time
- Memory efficient (no aggregation buffers needed)
- Suitable for real-time control applications

**⚡ Control Benefits**
```cpp
// Guaranteed smooth response due to monotonic functions
if (input_increases) {
    output_changes_monotonically();  // Predictable behavior
}
```

**🤖 Implementation Benefits**
- Easier tuning than Mamdani (fewer parameters)
- Natural integration with PID controllers
- Excellent stability characteristics
- Robust to parameter variations

---

## 2. Theoretical Foundation

### 2.1 Monotonic Membership Functions

**Definition**: A membership function μ(x) is monotonic if it is either:
- **Strictly Increasing**: μ(x₁) < μ(x₂) for all x₁ < x₂
- **Strictly Decreasing**: μ(x₁) > μ(x₂) for all x₁ < x₂

**Mathematical Properties**:
```
For increasing monotonic function f:
∀ x₁, x₂ ∈ [a,b]: x₁ < x₂ ⟹ f(x₁) ≤ f(x₂)

For decreasing monotonic function f:
∀ x₁, x₂ ∈ [a,b]: x₁ < x₂ ⟹ f(x₁) ≥ f(x₂)
```

**Invertibility**: Monotonic functions are invertible, which is crucial for Tsukamoto method:
```
If μ(x) is monotonic, then μ⁻¹(α) exists and is unique
```

### 2.2 Standard Monotonic Functions

**S-shaped Function (Increasing)**:
```
S(x; a, b) = {
    0,                    if x ≤ a
    2((x-a)/(b-a))²,     if a < x ≤ (a+b)/2
    1-2((x-b)/(b-a))²,   if (a+b)/2 < x < b
    1,                    if x ≥ b
}
```

**Z-shaped Function (Decreasing)**:
```
Z(x; a, b) = {
    1,                    if x ≤ a
    1-2((x-a)/(b-a))²,   if a < x ≤ (a+b)/2
    2((x-b)/(b-a))²,     if (a+b)/2 < x < b
    0,                    if x ≥ b
}
```

**Sigmoid Function**:
```
Increasing: σ(x; a, c) = 1/(1 + exp(-a(x-c)))
Decreasing: σ(x; a, c) = 1/(1 + exp(a(x-c)))
```

### 2.3 Inverse Function Calculation

For S-shaped function S(x; a, b), given membership value α:
```cpp
float inverseSShapedMembership(float alpha, float a, float b) {
    if (alpha <= 0) return a;
    if (alpha >= 1) return b;
    
    float mid = (a + b) / 2.0;
    
    if (alpha <= 0.5) {
        // Lower half: α = 2((x-a)/(b-a))²
        float ratio = sqrt(alpha / 2.0);
        return a + ratio * (b - a);
    } else {
        // Upper half: α = 1 - 2((x-b)/(b-a))²
        float ratio = sqrt((1 - alpha) / 2.0);
        return b - ratio * (b - a);
    }
}
```

### 2.4 Linguistic Variables in Tsukamoto

**Definition**: Same as Mamdani, but with monotonic constraint on output terms:

**Example**:
```
X = "motor_speed"
T(X) = {SLOW, MEDIUM, FAST}  // All must be monotonic
U = [0, 100] (%)
G = grammar rules for combining terms
M = monotonic membership functions only
```

**Input Variable Constraints**: None - can use any membership function type
**Output Variable Constraints**: MUST use monotonic membership functions only

---

## 3. Mathematical Framework

### 3.1 Tsukamoto Inference Process

The Tsukamoto inference system follows four main steps:

**Step 1: Fuzzification** (Same as Mamdani)
```
Input: x₀ ∈ X (crisp value)
Output: μAᵢ(x₀) ∀i (membership degrees)

For each input variable and each fuzzy set:
μAᵢ(x₀) = membership_function(x₀, parameters_of_Aᵢ)
```

**Step 2: Rule Evaluation** (Same as Mamdani)
```
For rule Rₖ: IF x₁ is A₁ AND x₂ is A₂ THEN y is B

Rule strength: αₖ = min(μA₁(x₁), μA₂(x₂))  // for AND operation
             αₖ = max(μA₁(x₁), μA₂(x₂))  // for OR operation
```

**Step 3: Individual Rule Output Calculation** (Unique to Tsukamoto)
```
For each rule Rₖ with strength αₖ and monotonic consequent Bₖ:
Find zₖ such that μBₖ(zₖ) = αₖ

Since Bₖ is monotonic: zₖ = μBₖ⁻¹(αₖ)  // Inverse function
```

**Step 4: Height Defuzzification** (Weighted Average)
```
Final output:
z* = Σ(αₖ × zₖ) / Σ(αₖ)

where:
- αₖ = firing strength of rule k
- zₖ = crisp output of rule k (from inverse function)
```

### 3.2 Implementation Mathematical Details

**S-shaped Membership Calculation**:
```cpp
float calculateSShapedMembership(float value, float a, float b) const {
    if (value <= a) return 0.0f;
    if (value >= b) return 1.0f;
    
    float mid = (a + b) / 2.0f;
    float range = b - a;
    
    if (value <= mid) {
        float ratio = (value - a) / range;
        return 2.0f * ratio * ratio;
    } else {
        float ratio = (value - b) / range;
        return 1.0f - 2.0f * ratio * ratio;
    }
}
```

**Z-shaped Membership Calculation**:
```cpp
float calculateZShapedMembership(float value, float a, float b) const {
    if (value <= a) return 1.0f;
    if (value >= b) return 0.0f;
    
    float mid = (a + b) / 2.0f;
    float range = b - a;
    
    if (value <= mid) {
        float ratio = (value - a) / range;
        return 1.0f - 2.0f * ratio * ratio;
    } else {
        float ratio = (value - b) / range;
        return 2.0f * ratio * ratio;
    }
}
```

**Height Defuzzification Implementation**:
```cpp
float defuzzifyHeight(int outputIndex, const float *ruleStrengths) const {
    float numerator = 0.0f;
    float denominator = 0.0f;
    
    for (int i = 0; i < numRules; i++) {
        if (rules[i].consequentVar == outputIndex && ruleStrengths[i] > 0.001f) {
            // Get consequent fuzzy set
            const FuzzyTsukamotoSet &consequentSet = 
                outputVars[outputIndex].sets[rules[i].consequentSet];
            
            // Calculate crisp output using inverse function
            float crispOutput = calculateInverseMembership(
                ruleStrengths[i], consequentSet);
            
            numerator += ruleStrengths[i] * crispOutput;
            denominator += ruleStrengths[i];
        }
    }
    
    return (denominator > 0.001f) ? (numerator / denominator) : 
           (outputVars[outputIndex].min + outputVars[outputIndex].max) / 2.0f;
}
```

**Inverse Membership Function**:
```cpp
float calculateInverseMembership(float alpha, const FuzzyTsukamotoSet &set) const {
    if (alpha <= 0.0f) return set.params[0];  // Lower bound
    if (alpha >= 1.0f) return set.params[1];  // Upper bound
    
    switch (set.type) {
        case S_SHAPED:
            return inverseSShapedMembership(alpha, set.params[0], set.params[1]);
            
        case Z_SHAPED:
            return inverseZShapedMembership(alpha, set.params[0], set.params[1]);
            
        case SIGMOID_INCREASING:
            return inverseSigmoidIncreasing(alpha, set.params[0], set.params[1]);
            
        case SIGMOID_DECREASING:
            return inverseSigmoidDecreasing(alpha, set.params[0], set.params[1]);
            
        default:
            return (set.params[0] + set.params[1]) / 2.0f;  // Fallback
    }
}
```

### 3.3 Rule Strength Calculation

**Multiple Antecedents with AND**:
```
Rule: IF x₁ is A₁ AND x₂ is A₂ AND ... AND xₙ is Aₙ THEN y is B

Strength: α = min(μA₁(x₁), μA₂(x₂), ..., μAₙ(xₙ))
```

**Multiple Antecedents with OR**:
```
Rule: IF x₁ is A₁ OR x₂ is A₂ OR ... OR xₙ is Aₙ THEN y is B  

Strength: α = max(μA₁(x₁), μA₂(x₂), ..., μAₙ(xₙ))
```

**Mixed AND/OR Operations**:
```cpp
// Example: IF (temp is HOT AND load is HEAVY) OR (emergency is TRUE)
float temp_load_strength = min(μHOT(temp), μHEAVY(load));  
float emergency_strength = μTRUE(emergency);
float rule_strength = max(temp_load_strength, emergency_strength);
```

### 3.4 Advantages of Height Defuzzification

**Computational Efficiency**:
```
Tsukamoto: O(r) where r = number of rules
Mamdani: O(r × d) where d = discretization resolution

Speed improvement: ~10-50x faster than Mamdani centroid
```

**Memory Efficiency**:
```
Tsukamoto: No aggregation buffer needed
Mamdani: Requires discretization array of size d

Memory savings: ~100-1000 bytes per output variable
```

**Accuracy**:
```
Tsukamoto: Exact calculation (no discretization error)
Mamdani: Approximation error due to discretization

Accuracy: Tsukamoto gives mathematically exact results
```

---

## 4. Implementation Architecture

### 4.1 Class Structure Overview

```cpp
class FuzzyTsukamoto {
private:
    // System constraints
    int maxInputs, maxOutputs, maxRules, maxSetsPerVar;
    int numInputs, numOutputs, numRules;
    
    // Core components
    FuzzyTsukamotoVariable *inputVars;     // Input variable array
    FuzzyTsukamotoVariable *outputVars;    // Output variable array  
    FuzzyTsukamotoRule *rules;             // Rule base array
    
    // Configuration
    bool debugMode;                        // Enable debug output
    
    // Error handling
    bool errorState;                       // Error flag
    char errorMessage[64];                // Error description
    
    // Core algorithms
    float calculateMembership(float value, const FuzzyTsukamotoSet &set) const;
    float calculateInverseMembership(float alpha, const FuzzyTsukamotoSet &set) const;
    float *evaluateRules(const float *inputs, int *activatedRules);
    float defuzzifyHeight(int outputIndex, const float *ruleStrengths) const;
    
    // Monotonic function implementations
    float inverseSShapedMembership(float alpha, float a, float b) const;
    float inverseZShapedMembership(float alpha, float a, float b) const;
    float inverseSigmoidIncreasing(float alpha, float a, float c) const;
    float inverseSigmoidDecreasing(float alpha, float a, float c) const;
};
```

### 4.2 Data Structures

**Fuzzy Variable Structure**:
```cpp
struct FuzzyTsukamotoVariable {
    char name[24];              // Variable name (e.g., "temperature")
    float min, max;             // Universe of discourse [min, max]
    int numSets;                // Number of fuzzy sets
    FuzzyTsukamotoSet *sets;    // Array of fuzzy sets
    bool isInput;               // true for input, false for output
};
```

**Fuzzy Set Structure (Enhanced for Monotonic)**:
```cpp
enum MonotonicType {
    NON_MONOTONIC,      // For input variables only
    S_SHAPED,           // Increasing monotonic
    Z_SHAPED,           // Decreasing monotonic
    SIGMOID_INCREASING, // Sigmoid increasing
    SIGMOID_DECREASING, // Sigmoid decreasing
    LINEAR_INCREASING,  // Linear increasing
    LINEAR_DECREASING   // Linear decreasing
};

struct FuzzyTsukamotoSet {
    char name[24];              // Set name (e.g., "FAST")
    MembershipType type;        // Base type (TRIANGULAR, etc.)
    MonotonicType monotonicType; // Monotonic constraint
    float params[4];            // Parameters for membership function
    bool isMonotonic;           // Flag for monotonic constraint check
};
```

**Fuzzy Rule Structure**:
```cpp
struct FuzzyTsukamotoRule {
    int *antecedentVars;        // Input variable indices
    int *antecedentSets;        // Input fuzzy set indices  
    int consequentVar;          // Output variable index
    int consequentSet;          // Output fuzzy set index
    int numAntecedents;         // Number of antecedents
    bool useAND;                // true=AND, false=OR logic
    float cachedOutput;         // Cached crisp output for this rule
    bool cacheValid;            // Cache validity flag
};
```

### 4.3 Memory Management Strategy

**Optimized Allocation Pattern**:
```cpp
// Constructor allocates major structures
inputVars = new FuzzyTsukamotoVariable[maxInputs];
outputVars = new FuzzyTsukamotoVariable[maxOutputs];  
rules = new FuzzyTsukamotoRule[maxRules];

// Sets allocated per variable with monotonic checking
for (int i = 0; i < maxOutputs; i++) {
    outputVars[i].sets = new FuzzyTsukamotoSet[maxSetsPerVar];
    // Validate monotonic constraint during allocation
}
```

**Memory Usage Calculation (Optimized)**:
```cpp
// Base memory per instance:
int baseMemory = sizeof(FuzzyTsukamoto);  // ~120 bytes

// Variable storage:
int variableMemory = (maxInputs + maxOutputs) * sizeof(FuzzyTsukamotoVariable);  // ~60 bytes each

// Fuzzy sets storage (with monotonic info):
int setsMemory = (maxInputs + maxOutputs) * maxSetsPerVar * sizeof(FuzzyTsukamotoSet);  // ~50 bytes each

// Rules storage (with caching):
int rulesMemory = maxRules * sizeof(FuzzyTsukamotoRule);  // ~40 bytes each

// No aggregation buffer needed (major memory saving)
// Total ≈ 120 + 60×vars + 50×sets + 40×rules (20-30% less than Mamdani)
```

### 4.4 Error Handling System

**Enhanced Error States**:
```cpp
enum TsukamotoErrorTypes {
    NO_ERROR,
    MEMORY_ALLOCATION_FAILED,
    INVALID_PARAMETERS,
    NON_MONOTONIC_OUTPUT_FUNCTION,  // Unique to Tsukamoto
    INVERSE_FUNCTION_FAILED,        // Unique to Tsukamoto
    MAX_CAPACITY_EXCEEDED,
    INVALID_VARIABLE_INDEX,
    INVALID_SET_INDEX,
    SYSTEM_NOT_DEFINED,
    NULL_INPUT_PROVIDED
};

// Monotonic constraint validation:
bool validateMonotonicConstraint(const FuzzyTsukamotoSet &set, bool isOutput) {
    if (isOutput && set.monotonicType == NON_MONOTONIC) {
        errorState = true;
        strncpy(errorMessage, "Output functions must be monotonic", 63);
        return false;
    }
    return true;
}
```

### 4.5 Performance Optimization Features

**Rule Output Caching**:
```cpp
// Cache crisp outputs to avoid repeated inverse calculations
if (!rules[i].cacheValid) {
    rules[i].cachedOutput = calculateInverseMembership(
        ruleStrength, consequentSet);
    rules[i].cacheValid = true;
}
float crispOutput = rules[i].cachedOutput;
```

**Fast Inverse Function Lookup**:
```cpp
// Pre-computed lookup table for common monotonic functions
static const float S_SHAPED_LOOKUP[101] = { /* pre-computed values */ };

float fastInverseSShapedMembership(float alpha, float a, float b) {
    int index = (int)(alpha * 100);
    float baseValue = S_SHAPED_LOOKUP[index];
    return a + baseValue * (b - a);  // Scale to range
}
```

**Early Termination Optimization**:
```cpp
// Skip rules with negligible strength
if (ruleStrengths[i] < 0.001f) continue;

// Skip defuzzification if no rules fired
if (totalStrength < 0.0001f) {
    return (outputVars[outputIndex].min + outputVars[outputIndex].max) / 2.0f;
}
```

### 4.6 Platform-Specific Features

**ESP32 Advanced Features**:
```cpp
#ifdef ESP32
// Hardware acceleration for inverse function calculation
#include "esp32/rom/libgcc.h"

float hardwareAcceleratedInverse(float alpha, float a, float b) {
    // Use ESP32 FPU for fast sqrt and division
    return hardwareOptimizedCalculation(alpha, a, b);
}

// Model persistence with monotonic validation
bool saveModelWithValidation(const char *filename) {
    File file = SPIFFS.open(filename, "w");
    
    // Save monotonic constraints
    for (int i = 0; i < numOutputs; i++) {
        for (int j = 0; j < outputVars[i].numSets; j++) {
            file.write((uint8_t*)&outputVars[i].sets[j].monotonicType, sizeof(MonotonicType));
        }
    }
    
    return true;
}
#endif
```

**Memory Optimization for AVR**:
```cpp
#ifdef AVR
// Use PROGMEM for monotonic function lookup tables
const PROGMEM float s_shaped_params[] = {0.0, 0.1, 0.2, /* ... */};

// Reduced precision for memory-constrained platforms
typedef int16_t membership_t;  // Use 16-bit instead of float for lookup
#define MEMBERSHIP_SCALE 1000  // Scale factor for fixed-point arithmetic

membership_t calculateMembershipFixed(int16_t value, const FuzzyTsukamotoSet &set) {
    // Fixed-point arithmetic implementation
    return (membership_t)(calculateMembership(value / 100.0f, set) * MEMBERSHIP_SCALE);
}
#endif
```

---

**Continued in FuzzyTsukamoto2.md for:**
- Usage Examples and Best Practices
- Performance Analysis  
- Troubleshooting and Optimization
- References and Advanced Applications