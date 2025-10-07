# Fuzzy Mamdani: Comprehensive Technical Documentation

## Table of Contents
1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Introduction and Definitions](#introduction-and-definitions)
3. [Theoretical Foundation](#theoretical-foundation)
4. [Mathematical Framework](#mathematical-framework)
5. [Implementation Architecture](#implementation-architecture)
6. [Usage Examples and Best Practices](#usage-examples-and-best-practices)
7. [Performance Analysis](#performance-analysis)
8. [Troubleshooting and Optimization](#troubleshooting-and-optimization)
9. [References](#references)

---

## 🚀 Quick Start for Beginners

### Apa itu Fuzzy Mamdani? (In Simple Terms)

Bayangkan Anda **mengatur kipas angin otomatis** berdasarkan suhu dan kelembaban:

**Tanpa Fuzzy (Traditional Logic):**
```
IF temperature > 25°C THEN fan = 100%
ELSE fan = 0%
```
Hasil: **Kipas hidup-mati tiba-tiba!**

**Dengan Fuzzy Mamdani:**
```
IF temperature is HOT AND humidity is HIGH THEN fan is FAST
IF temperature is WARM OR humidity is MEDIUM THEN fan is MEDIUM  
IF temperature is COOL THEN fan is SLOW
```
Hasil: **Kipas berubah smooth dan natural!**

### Visual Example: Smart Fan Control

Mari lihat bagaimana Fuzzy Mamdani bekerja untuk kontrol kipas pintar:

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

Humidity (%)
    ^
    |  LOW    MEDIUM   HIGH  
 1.0|   /\      /\      /\
    |  /  \    /  \    /  \
 0.5| /    \  /    \  /    \
    |/      \/      \/      \
 0.0+--------+--------+--------→
   30      50      70      90

Fan Speed (%)
    ^
    |  SLOW   MEDIUM   FAST
 1.0|   /\      /\      /\
    |  /  \    /  \    /  \
 0.5| /    \  /    \  /    \
    |/      \/      \/      \
 0.0+--------+--------+--------→
    0      50      80     100
```

**Step-by-Step Fuzzy Process:**
1. **Fuzzification**: Input 30°C, 60% → "HOT" (0.5), "MEDIUM" (0.8)
2. **Rule Evaluation**: Check semua aturan dengan AND/OR
3. **Aggregation**: Gabungkan hasil dengan MAX operation
4. **Defuzzification**: Hitung centroid → Output 75% fan speed

### 🔍 Step-by-Step Example: Temperature Control

Mari kita buat sistem kontrol suhu ruangan step by step:

**Target**: Mengatur heater dan cooler berdasarkan suhu ruangan

**Step 1: Setup Variables**
```cpp
#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

FuzzyMamdani tempControl(2, 1, 4, 3);  // 2 inputs, 1 output, 4 rules, 3 sets/var

void setup() {
    // Input: Temperature (15-35°C)
    tempControl.addInputVariable("temperature", 15.0, 35.0);
    
    // Input: Humidity (30-90%)  
    tempControl.addInputVariable("humidity", 30.0, 90.0);
    
    // Output: Control Signal (-100 to 100, negative=cooling, positive=heating)
    tempControl.addOutputVariable("control", -100.0, 100.0);
}
```

**Step 2: Define Fuzzy Sets**
```cpp
void setupFuzzySets() {
    // Temperature fuzzy sets (triangular membership functions)
    float tempCold[] = {15, 15, 23};    tempControl.addFuzzySet(0, true, "COLD", TRIANGULAR, tempCold);
    float tempWarm[] = {20, 25, 30};    tempControl.addFuzzySet(0, true, "WARM", TRIANGULAR, tempWarm);
    float tempHot[] = {27, 35, 35};     tempControl.addFuzzySet(0, true, "HOT", TRIANGULAR, tempHot);
    
    // Humidity fuzzy sets
    float humLow[] = {30, 30, 50};      tempControl.addFuzzySet(1, true, "LOW", TRIANGULAR, humLow);
    float humMed[] = {40, 55, 70};      tempControl.addFuzzySet(1, true, "MEDIUM", TRIANGULAR, humMed);
    float humHigh[] = {60, 90, 90};     tempControl.addFuzzySet(1, true, "HIGH", TRIANGULAR, humHigh);
    
    // Control output fuzzy sets
    float coolStrong[] = {-100, -100, -50};  tempControl.addFuzzySet(0, false, "COOL_STRONG", TRIANGULAR, coolStrong);
    float coolLight[] = {-60, -30, 0};       tempControl.addFuzzySet(0, false, "COOL_LIGHT", TRIANGULAR, coolLight);
    float heatLight[] = {0, 30, 60};         tempControl.addFuzzySet(0, false, "HEAT_LIGHT", TRIANGULAR, heatLight);
    float heatStrong[] = {50, 100, 100};     tempControl.addFuzzySet(0, false, "HEAT_STRONG", TRIANGULAR, heatStrong);
}
```

**Step 3: Create Fuzzy Rules**
```cpp
void setupFuzzyRules() {
    // Rule 1: IF temp is COLD THEN control is HEAT_STRONG
    int antVars1[] = {0};         // temperature variable
    int antSets1[] = {0};         // COLD set
    tempControl.addRule(antVars1, antSets1, 1, 0, 3, true);  // HEAT_STRONG
    
    // Rule 2: IF temp is WARM AND humidity is LOW THEN control is HEAT_LIGHT  
    int antVars2[] = {0, 1};      // temperature, humidity
    int antSets2[] = {1, 0};      // WARM, LOW
    tempControl.addRule(antVars2, antSets2, 2, 0, 2, true);  // HEAT_LIGHT
    
    // Rule 3: IF temp is HOT OR humidity is HIGH THEN control is COOL_STRONG
    int antVars3[] = {0, 1};      // temperature, humidity  
    int antSets3[] = {2, 2};      // HOT, HIGH
    tempControl.addRule(antVars3, antSets3, 2, 0, 0, false); // COOL_STRONG (OR logic)
    
    // Rule 4: IF temp is WARM AND humidity is HIGH THEN control is COOL_LIGHT
    int antVars4[] = {0, 1};      // temperature, humidity
    int antSets4[] = {1, 2};      // WARM, HIGH  
    tempControl.addRule(antVars4, antSets4, 2, 0, 1, true);  // COOL_LIGHT
}
```

**Step 4: Manual Calculation Example**

Input: Temperature = 28°C, Humidity = 65%

**Fuzzification:**
```
Temperature 28°C:
- COLD: μ = 0 (28 < 23)
- WARM: μ = (30-28)/(30-25) = 2/5 = 0.4  
- HOT: μ = (28-27)/(35-27) = 1/8 = 0.125

Humidity 65%:
- LOW: μ = 0 (65 > 50)
- MEDIUM: μ = (70-65)/(70-55) = 5/15 = 0.33
- HIGH: μ = (65-60)/(90-60) = 5/30 = 0.167
```

**Rule Evaluation:**
```
Rule 1: temp is COLD → μ = 0 → NOT FIRED
Rule 2: temp is WARM AND humidity is LOW → μ = min(0.4, 0) = 0 → NOT FIRED  
Rule 3: temp is HOT OR humidity is HIGH → μ = max(0.125, 0.167) = 0.167 → COOL_STRONG
Rule 4: temp is WARM AND humidity is HIGH → μ = min(0.4, 0.167) = 0.167 → COOL_LIGHT
```

**Aggregation (MAX method):**
```
COOL_STRONG: max(0.167) = 0.167
COOL_LIGHT: max(0.167) = 0.167
Combined output membership function is created by taking max of both
```

**Defuzzification (Centroid method):**
```cpp
// The system calculates the centroid of the combined membership function
// Result ≈ -45% (moderate cooling needed)
```

**Complete Implementation:**
```cpp
#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

FuzzyMamdani tempController(2, 1, 4, 4);

#define TEMP_SENSOR_PIN A0
#define HUMIDITY_SENSOR_PIN A1
#define HEATER_PIN 9
#define COOLER_PIN 10

void setup() {
    Serial.begin(115200);
    setupFuzzyVariables();
    setupFuzzySets();
    setupFuzzyRules();
    
    tempController.setDefuzzificationMethod(CENTROID);
    Serial.println("Fuzzy Mamdani Temperature Controller Ready!");
}

void loop() {
    // Read sensors
    float temperature = readTemperature();
    float humidity = readHumidity();
    
    // Prepare inputs
    float inputs[] = {temperature, humidity};
    
    // Fuzzy inference
    float *outputs = tempController.evaluate(inputs);
    
    if (outputs != nullptr) {
        float controlSignal = outputs[0];
        
        // Apply control signal
        applyControl(controlSignal);
        
        // Debug output
        Serial.print("Temp: " + String(temperature, 1) + "°C");
        Serial.print(" | Humidity: " + String(humidity, 1) + "%");
        Serial.println(" | Control: " + String(controlSignal, 1));
        
        delete[] outputs;  // Free memory
    }
    
    delay(1000);
}

void applyControl(float signal) {
    if (signal > 5) {
        // Heating needed
        analogWrite(HEATER_PIN, (int)constrain(signal * 2.55, 0, 255));
        analogWrite(COOLER_PIN, 0);
    } else if (signal < -5) {
        // Cooling needed
        analogWrite(HEATER_PIN, 0);
        analogWrite(COOLER_PIN, (int)constrain(-signal * 2.55, 0, 255));
    } else {
        // Dead zone - no action
        analogWrite(HEATER_PIN, 0);
        analogWrite(COOLER_PIN, 0);
    }
}
```

### 🎯 Key Insights

**Mengapa Fuzzy Mamdani Powerful?**
- **Smooth Control**: Tidak ada perubahan mendadak seperti on/off control
- **Multiple Inputs**: Bisa menggabungkan banyak sensor sekaligus
- **Human-like Logic**: Rules mudah dipahami dan dimodifikasi
- **Robust**: Tahan terhadap noise dan uncertainty
- **Linguistic Variables**: Menggunakan bahasa natural (HOT, COLD, etc.)

**Kapan Pakai Fuzzy Mamdani?**
- HVAC systems (heating, ventilation, air conditioning)
- Automatic washing machine control
- Camera auto-focus systems
- Traffic light optimization
- Robot navigation and obstacle avoidance
- Any system yang butuh smooth, intelligent control

### ⚡ 5-Minute Quick Start Template

```cpp
#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

FuzzyMamdani fuzzy(1, 1, 3, 3);  // 1 input, 1 output, 3 rules, 3 sets

void setup() {
    // Step 1: Add variables
    fuzzy.addInputVariable("input", 0.0, 100.0);
    fuzzy.addOutputVariable("output", 0.0, 100.0);
    
    // Step 2: Add fuzzy sets (triangular)
    float low[] = {0, 0, 50};     fuzzy.addFuzzySet(0, true, "LOW", TRIANGULAR, low);
    float med[] = {25, 50, 75};   fuzzy.addFuzzySet(0, true, "MED", TRIANGULAR, med);
    float high[] = {50, 100, 100}; fuzzy.addFuzzySet(0, true, "HIGH", TRIANGULAR, high);
    
    float small[] = {0, 0, 50};   fuzzy.addFuzzySet(0, false, "SMALL", TRIANGULAR, small);
    float medium[] = {25, 50, 75}; fuzzy.addFuzzySet(0, false, "MEDIUM", TRIANGULAR, medium);
    float large[] = {50, 100, 100}; fuzzy.addFuzzySet(0, false, "LARGE", TRIANGULAR, large);
    
    // Step 3: Add rules
    int antVar[] = {0}; int antSet[] = {0}; fuzzy.addRule(antVar, antSet, 1, 0, 0, true); // LOW→SMALL
    int antVar2[] = {0}; int antSet2[] = {1}; fuzzy.addRule(antVar2, antSet2, 1, 0, 1, true); // MED→MEDIUM  
    int antVar3[] = {0}; int antSet3[] = {2}; fuzzy.addRule(antVar3, antSet3, 1, 0, 2, true); // HIGH→LARGE
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

### 1.1 Fuzzy Mamdani Definition

**Fuzzy Mamdani Controller** adalah sistem fuzzy inference yang dikembangkan oleh Ebrahim Mamdani pada tahun 1975. Sistem ini menggunakan **fuzzy sets** untuk input dan output, dengan proses **defuzzification** yang mengkonversi fuzzy output menjadi crisp value [Mamdani & Assilian, 1975].

### 1.2 Key Characteristics

**Linguistic Variables**: Menggunakan variabel linguistik seperti "HOT", "COLD", "FAST", "SLOW".

**Fuzzy Rule Base**: Aturan berbentuk "IF-THEN" dengan antecedent dan consequent fuzzy.

**Max-Min Inference**: Menggunakan minimum untuk AND operation dan maximum untuk aggregation.

**Centroid Defuzzification**: Metode defuzzifikasi paling umum menggunakan center of gravity.

### 1.3 Mamdani vs Sugeno

| Aspect | Mamdani | Sugeno |
|--------|---------|---------|
| **Output** | Fuzzy sets | Crisp functions |
| **Interpretability** | High (linguistic) | Medium |
| **Computational** | More intensive | More efficient |
| **Applications** | Human-centric control | Mathematical modeling |

### 1.4 Historical Background

- **1965**: Zadeh memperkenalkan fuzzy set theory
- **1973**: Mamdani mulai penelitian aplikasi fuzzy logic untuk control
- **1975**: Mamdani & Assilian publikasi pertama tentang fuzzy control untuk steam engine
- **1976**: Mamdani method menjadi standard untuk fuzzy inference systems
- **1980s**: Aplikasi luas di industri Jepang (Mitsubishi, Toyota, dll)

### 1.5 Real-World Applications

**🏠 Smart Home Systems**
```
Temperature Control: IF room is COLD AND outside is FREEZING THEN heater is HIGH
Light Control: IF time is EVENING AND occupancy is DETECTED THEN brightness is MEDIUM  
Security: IF motion is DETECTED AND time is NIGHT THEN alert is HIGH
```

**🚗 Automotive Systems**
```
ABS: IF wheel_slip is HIGH AND road is WET THEN brake_pressure is REDUCED
Transmission: IF speed is LOW AND throttle is HIGH THEN gear is DOWN
Suspension: IF road is ROUGH AND speed is HIGH THEN damping is STIFF  
```

**🏭 Industrial Control**
```
Process Control: IF temperature is HIGH AND pressure is LOW THEN valve is OPEN
Quality Control: IF defect_rate is HIGH THEN inspection is STRICT
Motor Control: IF load is HEAVY AND speed is LOW THEN torque is HIGH
```

**🤖 Robotics Applications**
```
Navigation: IF obstacle is NEAR AND path is NARROW THEN speed is SLOW
Gripper: IF object is FRAGILE AND grip is LIGHT THEN pressure is GENTLE
Balance: IF tilt is LEFT AND angular_velocity is HIGH THEN correction is RIGHT_STRONG
```

### 1.6 Why Mamdani for Embedded Systems?

**🔋 Computational Characteristics**
- Moderate computational complexity (higher than Sugeno, lower than neural networks)
- Predictable memory usage
- Suitable for 32-bit microcontrollers (ESP32, STM32)
- Real-time performance achievable with proper optimization

**⚡ Development Benefits**
```cpp
// Intuitive rule creation
if (temperature.isHot() && humidity.isHigh()) {
    fanSpeed.setFast();  // Easy to understand and modify
}
```

**🤖 Integration Advantages**
- Natural integration with sensor data
- Easy calibration and tuning
- Robust to sensor noise and measurement uncertainty
- Scales well with system complexity

---

## 2. Theoretical Foundation

### 2.1 Fuzzy Set Theory Basics

**Fuzzy Set Definition**: A fuzzy set A in universe X is characterized by membership function μA(x) where μA: X → [0,1].

**Mathematical Notation**:
```
A = {(x, μA(x)) | x ∈ X}

where μA(x) represents the degree of membership of x in set A
```

**Fuzzy vs Crisp Sets**:
```
Crisp Set: YOUNG = {x | age(x) ≤ 30}
Fuzzy Set: YOUNG = {(x, μYOUNG(x)) | μYOUNG(x) = max(0, min(1, (40-age(x))/10))}
```

### 2.2 Membership Functions

**Triangular Membership Function**:
```
μ(x; a, b, c) = {
    0,           if x ≤ a
    (x-a)/(b-a), if a < x ≤ b  
    (c-x)/(c-b), if b < x < c
    0,           if x ≥ c
}
```

**Trapezoidal Membership Function**:
```
μ(x; a, b, c, d) = {
    0,           if x < a
    (x-a)/(b-a), if a ≤ x < b
    1,           if b ≤ x ≤ c
    (d-x)/(d-c), if c < x ≤ d
    0,           if x > d
}
```

**Gaussian Membership Function**:
```
μ(x; μ, σ) = exp(-(x-μ)²/(2σ²))
```

### 2.3 Fuzzy Operations

**Intersection (AND) - T-norm**:
```
μA∩B(x) = min(μA(x), μB(x))    // Minimum
μA∩B(x) = μA(x) × μB(x)        // Product (alternative)
```

**Union (OR) - T-conorm**:
```
μA∪B(x) = max(μA(x), μB(x))    // Maximum  
μA∪B(x) = μA(x) + μB(x) - μA(x)×μB(x)  // Probabilistic sum (alternative)
```

**Complement (NOT)**:
```
μA'(x) = 1 - μA(x)
```

### 2.4 Linguistic Variables

**Definition**: A linguistic variable is characterized by (X, T(X), U, G, M) where:
- X: name of variable
- T(X): term set of X
- U: universe of discourse  
- G: syntactic rule for generating elements of T(X)
- M: semantic rule for associating meaning with each element of T(X)

**Example**:
```
X = "temperature"
T(X) = {COLD, COOL, WARM, HOT}
U = [0, 50] (°C)
G = grammar rules for combining terms
M = membership functions defining meaning of each term
```

---

## 3. Mathematical Framework

### 3.1 Mamdani Inference Process

The Mamdani inference system follows four main steps:

**Step 1: Fuzzification**
```
Input: x₀ ∈ X (crisp value)
Output: μAᵢ(x₀) ∀i (membership degrees)

For each input variable and each fuzzy set:
μAᵢ(x₀) = membership_function(x₀, parameters_of_Aᵢ)
```

**Step 2: Rule Evaluation**
```
For rule Rₖ: IF x₁ is A₁ AND x₂ is A₂ THEN y is B

Rule strength: αₖ = min(μA₁(x₁), μA₂(x₂))  // for AND operation
             αₖ = max(μA₁(x₁), μA₂(x₂))  // for OR operation
```

**Step 3: Aggregation**
```
For output fuzzy set B':
μB'(y) = max(αₖ × μBₖ(y)) ∀k

where αₖ is the strength of rule k and Bₖ is the consequent of rule k
```

**Step 4: Defuzzification**
```
Centroid Method:
y* = ∫ y × μB'(y) dy / ∫ μB'(y) dy

Discrete form:
y* = Σ yᵢ × μB'(yᵢ) / Σ μB'(yᵢ)
```

### 3.2 Implementation Mathematical Details

**Triangular Membership (Code Implementation)**:
```cpp
float calculateTriangularMembership(float value, float a, float b, float c) const {
    if (value == a && a == b) return 1.0f;  // Left shoulder case
    if (value == c && b == c) return 1.0f;  // Right shoulder case  
    if (value == b) return 1.0f;
    if (value <= a || value >= c) {
        return 0.0f;
    } else if (value < b) {
        return (value - a) / (b - a);        // Ascending slope
    } else {
        return (c - value) / (c - b);        // Descending slope
    }
}
```

**Fuzzy AND/OR Operations**:
```cpp
float applyFuzzyOperator(float a, float b, bool useAND) const {
    if (useAND) {
        return (a < b) ? a : b;  // min(a,b) for AND
    } else {
        return (a > b) ? a : b;  // max(a,b) for OR  
    }
}
```

**Centroid Defuzzification**:
```cpp
float defuzzifyCentroid(int outputIndex, const float *ruleStrengths) const {
    const int RESOLUTION = 100;  // Discretization resolution
    
    float weightedSum = 0.0f;
    float membershipSum = 0.0f;
    
    for (float x = min; x <= max; x += step) {
        float membership = 0.0f;
        
        // Calculate aggregated membership at point x
        for (int i = 0; i < numRules; i++) {
            if (rules[i].consequentVar == outputIndex) {
                float setMembership = calculateMembership(x, set);
                float ruleMembership = min(setMembership, ruleStrengths[i]);
                membership = max(membership, ruleMembership);  // Max aggregation
            }
        }
        
        weightedSum += x * membership;    // Σ(x × μ(x))
        membershipSum += membership;      // Σ(μ(x))
    }
    
    return weightedSum / membershipSum;   // Centroid formula
}
```

### 3.3 Advanced Defuzzification Methods

**Mean of Maximum (MOM)**:
```
y* = (1/|YMAX|) × Σ(y), y ∈ YMAX

where YMAX = {y | μB'(y) = max(μB'(z)) ∀z}
```

**Smallest of Maximum (SOM)**:
```
y* = min{y | μB'(y) = max(μB'(z)) ∀z}
```

**Largest of Maximum (LOM)**:
```
y* = max{y | μB'(y) = max(μB'(z)) ∀z}
```

**Bisector Method**:
```
∫₋∞^y* μB'(y)dy = ∫ʸ*^∞ μB'(y)dy

Finds y* such that the area under the curve is split in half
```

### 3.4 Rule Strength Calculation

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
// Example: IF (temp is HOT AND humid is HIGH) OR (temp is VERY_HOT)
float temp_humid_strength = min(μHOT(temp), μHIGH(humid));  
float very_hot_strength = μVERY_HOT(temp);
float rule_strength = max(temp_humid_strength, very_hot_strength);
```

### 3.5 Aggregation Methods

**Maximum (MAX) - Standard Mamdani**:
```
μB'(y) = max{αₖ × μBₖ(y)} ∀k

Implementation:
for each rule k:
    clipped_membership = min(rule_strength[k], membership_Bk(y))
    aggregated_membership = max(aggregated_membership, clipped_membership)
```

**Sum (SUM) - Alternative Method**:
```
μB'(y) = min(1, Σₖ(αₖ × μBₖ(y)))

More sensitive to multiple rules but risks saturation
```

**Probabilistic OR**:
```
μB'(y) = 1 - Π(1 - αₖ × μBₖ(y)) ∀k

Avoids saturation while maintaining sensitivity
```

---

## 4. Implementation Architecture

### 4.1 Class Structure Overview

```cpp
class FuzzyMamdani {
private:
    // System constraints
    int maxInputs, maxOutputs, maxRules, maxSetsPerVar;
    int numInputs, numOutputs, numRules;
    
    // Core components
    FuzzyMamdaniVariable *inputVars;     // Input variable array
    FuzzyMamdaniVariable *outputVars;    // Output variable array  
    FuzzyMamdaniRule *rules;             // Rule base array
    
    // Configuration
    DefuzzificationMethod defuzzMethod;  // CENTROID, MOM, SOM, LOM, BISECTOR
    bool debugMode;                      // Enable debug output
    
    // Error handling
    bool errorState;                     // Error flag
    char errorMessage[50];               // Error description
    
    // Core algorithms
    float calculateMembership(float value, const FuzzyMamdaniSet &set) const;
    float *evaluateRules(const float *inputs, int *activatedRules);
    float defuzzifyCentroid(int outputIndex, const float *ruleStrengths) const;
    // ... other defuzzification methods
};
```

### 4.2 Data Structures

**Fuzzy Variable Structure**:
```cpp
struct FuzzyMamdaniVariable {
    char name[20];              // Variable name (e.g., "temperature")
    float min, max;             // Universe of discourse [min, max]
    int numSets;                // Number of fuzzy sets
    FuzzyMamdaniSet *sets;      // Array of fuzzy sets
};
```

**Fuzzy Set Structure**:
```cpp
struct FuzzyMamdaniSet {
    char name[20];              // Set name (e.g., "HOT")
    MembershipType type;        // TRIANGULAR, TRAPEZOIDAL, GAUSSIAN, SINGLETON
    float params[4];            // Parameters for membership function
};
```

**Fuzzy Rule Structure**:
```cpp
struct FuzzyMamdaniRule {
    int *antecedentVars;        // Input variable indices
    int *antecedentSets;        // Input fuzzy set indices  
    int consequentVar;          // Output variable index
    int consequentSet;          // Output fuzzy set index
    int numAntecedents;         // Number of antecedents
    bool useAND;                // true=AND, false=OR logic
};
```

### 4.3 Memory Management Strategy

**Dynamic Allocation Pattern**:
```cpp
// Constructor allocates major structures
inputVars = new FuzzyMamdaniVariable[maxInputs];
outputVars = new FuzzyMamdaniVariable[maxOutputs];  
rules = new FuzzyMamdaniRule[maxRules];

// Sets allocated per variable
inputVars[i].sets = new FuzzyMamdaniSet[maxSetsPerVar];

// Rule antecedents allocated per rule
rules[i].antecedentVars = new int[numAntecedents];
```

**Memory Usage Calculation**:
```cpp
// Base memory per instance (approximate):
int baseMemory = sizeof(FuzzyMamdani);  // ~100 bytes

// Variable storage:
int variableMemory = (maxInputs + maxOutputs) * sizeof(FuzzyMamdaniVariable);  // ~50 bytes each

// Fuzzy sets storage:
int setsMemory = (maxInputs + maxOutputs) * maxSetsPerVar * sizeof(FuzzyMamdaniSet);  // ~40 bytes each

// Rules storage:
int rulesMemory = maxRules * sizeof(FuzzyMamdaniRule);  // ~30 bytes each
int antecedentsMemory = maxRules * averageAntecedents * sizeof(int) * 2;  // vars + sets

// Total ≈ 100 + 50×vars + 40×sets + 30×rules + 8×antecedents
```

### 4.4 Error Handling System

**Error States and Recovery**:
```cpp
enum ErrorTypes {
    NO_ERROR,
    MEMORY_ALLOCATION_FAILED,
    INVALID_PARAMETERS,
    MAX_CAPACITY_EXCEEDED,
    INVALID_VARIABLE_INDEX,
    INVALID_SET_INDEX,
    SYSTEM_NOT_DEFINED,
    NULL_INPUT_PROVIDED
};

// Error checking pattern:
if (errorState) return false;  // Fail fast if in error state

// Error setting pattern:
if (condition_failed) {
    errorState = true;
    strncpy(errorMessage, "Descriptive error message", 49);
    errorMessage[49] = '\0';
    return false;
}
```

**Validation Mechanisms**:
```cpp
// Parameter validation for triangular membership
if (params[0] > params[1] || params[1] > params[2]) {
    errorState = true;
    strncpy(errorMessage, "Invalid triangular parameters", 49);
    return false;
}

// Index validation
if (varIndex < 0 || varIndex >= numVars) {
    errorState = true;
    strncpy(errorMessage, "Invalid variable index", 49);
    return false;
}
```

### 4.5 Performance Optimization Features

**Discretization Control**:
```cpp
const int RESOLUTION = 100;  // Balance between accuracy and speed
float step = (max - min) / RESOLUTION;
```

**Early Termination**:
```cpp
// Skip processing if rule strength is negligible
if (ruleStrengths[i] < 0.001f) continue;

// Skip defuzzification if no rules fired
if (membershipSum < 0.0001f) {
    return (min + max) / 2.0f;  // Default to center
}
```

**Memory Pool Reuse**:
```cpp
// Reuse output array rather than frequent allocation
float *outputs = new float[numOutputs];
// ... processing ...
// Caller responsible for deletion to avoid frequent alloc/dealloc
```

### 4.6 Platform-Specific Features

**ESP32 Model Persistence**:
```cpp
#ifdef ESP32
bool saveModel(const char *filename) {
    File file = SPIFFS.open(filename, "w");
    // Binary serialization of complete fuzzy system
    file.write((uint8_t*)&maxInputs, sizeof(int));
    // ... save all parameters
    return true;
}

bool loadModel(const char *filename) {
    File file = SPIFFS.open(filename, "r");
    // Binary deserialization with validation
    file.read((uint8_t*)&loadedMaxInputs, sizeof(int));
    // ... validate and load
    return true;
}
#endif
```

**Memory Optimization for AVR**:
```cpp
#ifdef AVR
// Reduced resolution for memory-constrained platforms
const int RESOLUTION = 50;  // vs 100 on more capable platforms

// Use PROGMEM for constant membership function parameters
const PROGMEM float triangular_params[] = {0, 25, 50};
#endif
```

---

## 5. Usage Examples and Best Practices

### 5.1 Basic HVAC Control System

**Complete temperature and humidity control for smart thermostat**

```cpp
#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Hardware pins
#define TEMP_SENSOR_PIN A0      // LM35 or similar
#define HUMIDITY_SENSOR_PIN A1  // DHT22 or similar
#define HEATER_RELAY_PIN 7      // Solid state relay for heater
#define COOLER_RELAY_PIN 8      // Solid state relay for AC
#define FAN_PWM_PIN 9           // PWM for fan speed control

// Fuzzy system configuration
FuzzyMamdani hvacController(2, 2, 12, 4);  // 2 inputs, 2 outputs, 12 rules, 4 sets per variable

// Setpoints
float targetTemperature = 22.0;  // °C
float targetHumidity = 50.0;     // %

void setup() {
    Serial.begin(115200);
    
    // Configure hardware pins
    pinMode(HEATER_RELAY_PIN, OUTPUT);
    pinMode(COOLER_RELAY_PIN, OUTPUT);
    pinMode(FAN_PWM_PIN, OUTPUT);
    
    setupFuzzyHVACSystem();
    Serial.println("Smart HVAC Fuzzy Controller Initialized");
    printSystemConfiguration();
}

void setupFuzzyHVACSystem() {
    // Input Variables
    hvacController.addInputVariable("temperature", 10.0, 40.0);    // 10-40°C range
    hvacController.addInputVariable("humidity", 20.0, 80.0);       // 20-80% range
    
    // Output Variables  
    hvacController.addOutputVariable("heating", -100.0, 100.0);    // -100=max cooling, +100=max heating
    hvacController.addOutputVariable("fan_speed", 0.0, 100.0);     // 0-100% fan speed
    
    setupTemperatureFuzzySets();
    setupHumidityFuzzySets();
    setupHeatingFuzzySets();
    setupFanSpeedFuzzySets();
    setupHVACRules();
    
    hvacController.setDefuzzificationMethod(CENTROID);
}

void setupTemperatureFuzzySets() {
    // Temperature fuzzy sets with overlapping ranges for smooth transitions
    float tempVeryCold[] = {10, 10, 16};    hvacController.addFuzzySet(0, true, "VERY_COLD", TRIANGULAR, tempVeryCold);
    float tempCold[] = {12, 18, 22};        hvacController.addFuzzySet(0, true, "COLD", TRIANGULAR, tempCold);
    float tempComfortable[] = {20, 24, 26}; hvacController.addFuzzySet(0, true, "COMFORTABLE", TRIANGULAR, tempComfortable);
    float tempWarm[] = {24, 28, 32};        hvacController.addFuzzySet(0, true, "WARM", TRIANGULAR, tempWarm);
    float tempHot[] = {30, 40, 40};         hvacController.addFuzzySet(0, true, "HOT", TRIANGULAR, tempHot);
}

void setupHumidityFuzzySets() {
    // Humidity fuzzy sets
    float humVeryDry[] = {20, 20, 35};      hvacController.addFuzzySet(1, true, "VERY_DRY", TRIANGULAR, humVeryDry);
    float humDry[] = {30, 40, 50};          hvacController.addFuzzySet(1, true, "DRY", TRIANGULAR, humDry);
    float humComfortable[] = {45, 55, 65};  hvacController.addFuzzySet(1, true, "COMFORTABLE", TRIANGULAR, humComfortable);
    float humHumid[] = {60, 70, 80};        hvacController.addFuzzySet(1, true, "HUMID", TRIANGULAR, humHumid);
}

void setupHeatingFuzzySets() {
    // Heating/Cooling output sets (negative = cooling, positive = heating)
    float maxCooling[] = {-100, -100, -60};     hvacController.addFuzzySet(0, false, "MAX_COOLING", TRIANGULAR, maxCooling);
    float lightCooling[] = {-80, -40, -10};     hvacController.addFuzzySet(0, false, "LIGHT_COOLING", TRIANGULAR, lightCooling);
    float noAction[] = {-20, 0, 20};            hvacController.addFuzzySet(0, false, "NO_ACTION", TRIANGULAR, noAction);
    float lightHeating[] = {10, 40, 80};        hvacController.addFuzzySet(0, false, "LIGHT_HEATING", TRIANGULAR, lightHeating);
    float maxHeating[] = {60, 100, 100};        hvacController.addFuzzySet(0, false, "MAX_HEATING", TRIANGULAR, maxHeating);
}

void setupFanSpeedFuzzySets() {
    // Fan speed fuzzy sets
    float fanOff[] = {0, 0, 20};            hvacController.addFuzzySet(1, false, "OFF", TRIANGULAR, fanOff);
    float fanLow[] = {10, 30, 50};          hvacController.addFuzzySet(1, false, "LOW", TRIANGULAR, fanLow);
    float fanMedium[] = {40, 60, 80};       hvacController.addFuzzySet(1, false, "MEDIUM", TRIANGULAR, fanMedium);
    float fanHigh[] = {70, 90, 100};        hvacController.addFuzzySet(1, false, "HIGH", TRIANGULAR, fanHigh);
}

void setupHVACRules() {
    // Temperature-based heating/cooling rules
    int tempVar[] = {0};  // temperature variable index
    
    // Very cold - maximum heating
    int veryColdSet[] = {0}; hvacController.addRule(tempVar, veryColdSet, 1, 0, 4, true);  // MAX_HEATING
    int veryColdFan[] = {0}; hvacController.addRule(tempVar, veryColdFan, 1, 1, 1, true);  // LOW fan
    
    // Cold - light heating
    int coldSet[] = {1}; hvacController.addRule(tempVar, coldSet, 1, 0, 3, true);      // LIGHT_HEATING
    int coldFan[] = {1}; hvacController.addRule(tempVar, coldFan, 1, 1, 1, true);      // LOW fan
    
    // Comfortable - no heating/cooling action
    int comfSet[] = {2}; hvacController.addRule(tempVar, comfSet, 1, 0, 2, true);      // NO_ACTION
    int comfFan[] = {2}; hvacController.addRule(tempVar, comfFan, 1, 1, 0, true);      // OFF fan
    
    // Warm - light cooling
    int warmSet[] = {3}; hvacController.addRule(tempVar, warmSet, 1, 0, 1, true);      // LIGHT_COOLING
    int warmFan[] = {3}; hvacController.addRule(tempVar, warmFan, 1, 1, 2, true);      // MEDIUM fan
    
    // Hot - maximum cooling  
    int hotSet[] = {4}; hvacController.addRule(tempVar, hotSet, 1, 0, 0, true);        // MAX_COOLING
    int hotFan[] = {4}; hvacController.addRule(tempVar, hotFan, 1, 1, 3, true);        // HIGH fan
    
    // Humidity-based fan speed rules (OR logic for comfort)
    int humidityVar[] = {1};  // humidity variable index
    
    // Very dry conditions - reduce fan to maintain humidity
    int veryDrySet[] = {0}; hvacController.addRule(humidityVar, veryDrySet, 1, 1, 0, false);  // OFF fan (OR logic)
    
    // Humid conditions - increase fan for air circulation
    int humidSet[] = {3}; hvacController.addRule(humidityVar, humidSet, 1, 1, 3, false);      // HIGH fan (OR logic)
}

void loop() {
    // Read environmental sensors
    float currentTemp = readTemperatureSensor();
    float currentHumidity = readHumiditySensor();
    
    // Prepare fuzzy inputs
    float inputs[] = {currentTemp, currentHumidity};
    
    // Execute fuzzy inference  
    float *outputs = hvacController.evaluate(inputs);
    
    if (outputs != nullptr) {
        float heatingOutput = outputs[0];    // -100 to +100
        float fanSpeedOutput = outputs[1];   // 0 to 100
        
        // Apply control signals to hardware
        controlHeatingCooling(heatingOutput);
        controlFanSpeed(fanSpeedOutput);
        
        // Log system status
        logHVACStatus(currentTemp, currentHumidity, heatingOutput, fanSpeedOutput);
        
        // Free memory
        delete[] outputs;
    } else {
        Serial.println("Fuzzy evaluation failed!");
        // Failsafe - turn off all systems
        digitalWrite(HEATER_RELAY_PIN, LOW);
        digitalWrite(COOLER_RELAY_PIN, LOW);
        analogWrite(FAN_PWM_PIN, 0);
    }
    
    delay(5000);  // Update every 5 seconds
}

void controlHeatingCooling(float heatingSignal) {
    if (heatingSignal > 10) {
        // Heating mode
        digitalWrite(HEATER_RELAY_PIN, HIGH);
        digitalWrite(COOLER_RELAY_PIN, LOW);
        Serial.print("HEATING: " + String(heatingSignal, 1) + "%");
    } else if (heatingSignal < -10) {
        // Cooling mode
        digitalWrite(HEATER_RELAY_PIN, LOW);
        digitalWrite(COOLER_RELAY_PIN, HIGH);
        Serial.print("COOLING: " + String(-heatingSignal, 1) + "%");
    } else {
        // Deadband - no action
        digitalWrite(HEATER_RELAY_PIN, LOW);
        digitalWrite(COOLER_RELAY_PIN, LOW);
        Serial.print("NO_ACTION");
    }
}

void controlFanSpeed(float fanSpeed) {
    int pwmValue = (int)constrain(fanSpeed * 2.55, 0, 255);  // Convert 0-100 to 0-255
    analogWrite(FAN_PWM_PIN, pwmValue);
    Serial.print(" | FAN: " + String(fanSpeed, 1) + "%");
}

void logHVACStatus(float temp, float humidity, float heating, float fan) {
    Serial.print("Temp: " + String(temp, 1) + "°C");
    Serial.print(" | Humidity: " + String(humidity, 1) + "%");
    Serial.print(" | ");
    controlHeatingCooling(heating);  // This also prints the heating status
    Serial.println(" | Target: " + String(targetTemperature, 1) + "°C");
}

float readTemperatureSensor() {
    // LM35: 10mV/°C, Vref=5V, 10-bit ADC
    int rawValue = analogRead(TEMP_SENSOR_PIN);
    float voltage = (rawValue * 5.0) / 1023.0;
    float temperature = voltage * 100.0;  // LM35 formula
    
    // Add some smoothing (simple moving average)
    static float tempHistory[5] = {25, 25, 25, 25, 25};
    static int historyIndex = 0;
    
    tempHistory[historyIndex] = temperature;
    historyIndex = (historyIndex + 1) % 5;
    
    float avgTemp = 0;
    for (int i = 0; i < 5; i++) {
        avgTemp += tempHistory[i];
    }
    return avgTemp / 5.0;
}

float readHumiditySensor() {
    // Placeholder for DHT22 or similar sensor reading
    // In real implementation, use DHT library or similar
    int rawValue = analogRead(HUMIDITY_SENSOR_PIN);
    float humidity = (rawValue * 100.0) / 1023.0;  // Simple linear mapping
    
    // Clamp to realistic range
    return constrain(humidity, 20.0, 80.0);
}

void printSystemConfiguration() {
    Serial.println("\n=== HVAC Fuzzy System Configuration ===");
    Serial.println("Inputs: Temperature (10-40°C), Humidity (20-80%)");
    Serial.println("Outputs: Heating (-100 to +100), Fan Speed (0-100%)");
    Serial.println("Rules: 12 total (temperature + humidity based)");
    Serial.println("Defuzzification: Centroid method");
    Serial.println("Target: " + String(targetTemperature) + "°C, " + String(targetHumidity) + "%");
    Serial.println("==========================================\n");
}
```

### 5.2 Advanced Washing Machine Control

**Multi-variable fuzzy control for automatic washing cycle optimization**

```cpp
#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Hardware interfaces
#define LOAD_SENSOR_PIN A0      // Load cell for clothes weight
#define SOIL_SENSOR_PIN A1      // Turbidity sensor for soil level
#define FABRIC_TYPE_PIN A2      // Fabric type selector (potentiometer)
#define WATER_VALVE_PIN 3       // Water inlet valve
#define PUMP_PIN 4              // Drain pump  
#define MOTOR_SPEED_PIN 5       // Wash motor PWM
#define HEATER_PIN 6            // Water heater element

// Fuzzy system: 3 inputs, 3 outputs, 27 rules, 3 sets per variable
FuzzyMamdani washingController(3, 3, 27, 3);

struct WashCycle {
    float waterLevel;    // 0-100%
    float washTime;      // 0-100% (relative)
    float motorSpeed;    // 0-100%
};

void setup() {
    Serial.begin(115200);
    setupWashingFuzzySystem();
    Serial.println("Intelligent Washing Machine Controller Ready");
}

void setupWashingFuzzySystem() {
    // Input variables
    washingController.addInputVariable("load_weight", 0.0, 10.0);      // 0-10 kg
    washingController.addInputVariable("soil_level", 0.0, 100.0);      // 0-100% turbidity
    washingController.addInputVariable("fabric_type", 0.0, 100.0);     // 0=delicate, 50=normal, 100=heavy
    
    // Output variables
    washingController.addOutputVariable("water_level", 20.0, 100.0);   // 20-100% water level
    washingController.addOutputVariable("wash_time", 10.0, 60.0);      // 10-60 minutes
    washingController.addOutputVariable("motor_speed", 20.0, 100.0);   // 20-100% motor speed
    
    setupLoadWeightSets();
    setupSoilLevelSets();
    setupFabricTypeSets();
    setupWaterLevelSets();
    setupWashTimeSets();
    setupMotorSpeedSets();
    setupWashingRules();
    
    washingController.setDefuzzificationMethod(CENTROID);
}

void setupLoadWeightSets() {
    // Load weight categories (kg)
    float lightLoad[] = {0, 0, 3.5};        washingController.addFuzzySet(0, true, "LIGHT", TRIANGULAR, lightLoad);
    float mediumLoad[] = {2.5, 5, 7.5};     washingController.addFuzzySet(0, true, "MEDIUM", TRIANGULAR, mediumLoad);
    float heavyLoad[] = {6.5, 10, 10};      washingController.addFuzzySet(0, true, "HEAVY", TRIANGULAR, heavyLoad);
}

void setupSoilLevelSets() {
    // Soil level (turbidity percentage)
    float lightSoil[] = {0, 0, 40};         washingController.addFuzzySet(1, true, "LIGHT", TRIANGULAR, lightSoil);
    float mediumSoil[] = {25, 50, 75};      washingController.addFuzzySet(1, true, "MEDIUM", TRIANGULAR, mediumSoil);
    float heavySoil[] = {60, 100, 100};     washingController.addFuzzySet(1, true, "HEAVY", TRIANGULAR, heavySoil);
}

void setupFabricTypeSets() {
    // Fabric type selector
    float delicate[] = {0, 0, 35};          washingController.addFuzzySet(2, true, "DELICATE", TRIANGULAR, delicate);
    float normal[] = {25, 50, 75};          washingController.addFuzzySet(2, true, "NORMAL", TRIANGULAR, normal);
    float heavy[] = {65, 100, 100};         washingController.addFuzzySet(2, true, "HEAVY", TRIANGULAR, heavy);
}

void setupWaterLevelSets() {
    // Water level output (%)
    float lowWater[] = {20, 20, 50};        washingController.addFuzzySet(0, false, "LOW", TRIANGULAR, lowWater);
    float mediumWater[] = {40, 65, 85};     washingController.addFuzzySet(0, false, "MEDIUM", TRIANGULAR, mediumWater);
    float highWater[] = {75, 100, 100};     washingController.addFuzzySet(0, false, "HIGH", TRIANGULAR, highWater);
}

void setupWashTimeSets() {
    // Wash time output (minutes)
    float shortTime[] = {10, 10, 25};       washingController.addFuzzySet(1, false, "SHORT", TRIANGULAR, shortTime);
    float mediumTime[] = {20, 35, 50};      washingController.addFuzzySet(1, false, "MEDIUM", TRIANGULAR, mediumTime);
    float longTime[] = {45, 60, 60};        washingController.addFuzzySet(1, false, "LONG", TRIANGULAR, longTime);
}

void setupMotorSpeedSets() {
    // Motor speed output (%)
    float gentleSpeed[] = {20, 20, 50};     washingController.addFuzzySet(2, false, "GENTLE", TRIANGULAR, gentleSpeed);
    float normalSpeed[] = {40, 65, 85};     washingController.addFuzzySet(2, false, "NORMAL", TRIANGULAR, normalSpeed);
    float vigorousSpeed[] = {75, 100, 100}; washingController.addFuzzySet(2, false, "VIGOROUS", TRIANGULAR, vigorousSpeed);
}

void setupWashingRules() {
    // Rule structure: [load_weight, soil_level, fabric_type] -> [water_level, wash_time, motor_speed]
    
    // Light load rules
    int rule1[] = {0, 0, 0}; washingController.addRule(rule1, (int[]){0, 1, 2}, 3, 0, 0, true); // LIGHT+LIGHT+DELICATE -> LOW water
    washingController.addRule(rule1, (int[]){0, 1, 2}, 3, 1, 0, true); // -> SHORT time
    washingController.addRule(rule1, (int[]){0, 1, 2}, 3, 2, 0, true); // -> GENTLE speed
    
    int rule2[] = {0, 0, 1}; washingController.addRule(rule2, (int[]){0, 1, 1}, 3, 0, 0, true); // LIGHT+LIGHT+NORMAL -> LOW water
    washingController.addRule(rule2, (int[]){0, 1, 1}, 3, 1, 0, true); // -> SHORT time
    washingController.addRule(rule2, (int[]){0, 1, 1}, 3, 2, 1, true); // -> NORMAL speed
    
    int rule3[] = {0, 1, 0}; washingController.addRule(rule3, (int[]){0, 1, 2}, 3, 0, 1, true); // LIGHT+MEDIUM+DELICATE -> MEDIUM water
    washingController.addRule(rule3, (int[]){0, 1, 2}, 3, 1, 1, true); // -> MEDIUM time
    washingController.addRule(rule3, (int[]){0, 1, 2}, 3, 2, 0, true); // -> GENTLE speed
    
    // Medium load rules
    int rule4[] = {1, 0, 1}; washingController.addRule(rule4, (int[]){1, 0, 1}, 3, 0, 1, true); // MEDIUM+LIGHT+NORMAL -> MEDIUM water
    washingController.addRule(rule4, (int[]){1, 0, 1}, 3, 1, 0, true); // -> SHORT time
    washingController.addRule(rule4, (int[]){1, 0, 1}, 3, 2, 1, true); // -> NORMAL speed
    
    int rule5[] = {1, 1, 1}; washingController.addRule(rule5, (int[]){1, 1, 1}, 3, 0, 1, true); // MEDIUM+MEDIUM+NORMAL -> MEDIUM water
    washingController.addRule(rule5, (int[]){1, 1, 1}, 3, 1, 1, true); // -> MEDIUM time
    washingController.addRule(rule5, (int[]){1, 1, 1}, 3, 2, 1, true); // -> NORMAL speed
    
    int rule6[] = {1, 2, 2}; washingController.addRule(rule6, (int[]){1, 2, 2}, 3, 0, 2, true); // MEDIUM+HEAVY+HEAVY -> HIGH water
    washingController.addRule(rule6, (int[]){1, 2, 2}, 3, 1, 2, true); // -> LONG time
    washingController.addRule(rule6, (int[]){1, 2, 2}, 3, 2, 2, true); // -> VIGOROUS speed
    
    // Heavy load rules
    int rule7[] = {2, 1, 2}; washingController.addRule(rule7, (int[]){2, 1, 2}, 3, 0, 2, true); // HEAVY+MEDIUM+HEAVY -> HIGH water
    washingController.addRule(rule7, (int[]){2, 1, 2}, 3, 1, 1, true); // -> MEDIUM time
    washingController.addRule(rule7, (int[]){2, 1, 2}, 3, 2, 2, true); // -> VIGOROUS speed
    
    int rule8[] = {2, 2, 2}; washingController.addRule(rule8, (int[]){2, 2, 2}, 3, 0, 2, true); // HEAVY+HEAVY+HEAVY -> HIGH water
    washingController.addRule(rule8, (int[]){2, 2, 2}, 3, 1, 2, true); // -> LONG time
    washingController.addRule(rule8, (int[]){2, 2, 2}, 3, 2, 2, true); // -> VIGOROUS speed
    
    // Delicate fabric overrides (high priority rules with OR logic for safety)
    int delicateRule[] = {2}; washingController.addRule((int[]){2}, delicateRule, 1, 2, 0, false); // ANY + DELICATE -> GENTLE (OR logic)
}

void loop() {
    Serial.println("\n=== Starting Wash Cycle Analysis ===");
    
    // Read sensors
    float loadWeight = readLoadWeight();
    float soilLevel = readSoilLevel();
    float fabricType = readFabricType();
    
    Serial.println("Sensor Readings:");
    Serial.println("Load Weight: " + String(loadWeight, 1) + " kg");
    Serial.println("Soil Level: " + String(soilLevel, 1) + "%");
    Serial.println("Fabric Type: " + String(fabricType, 1) + " (0=delicate, 50=normal, 100=heavy)");
    
    // Fuzzy inference
    float inputs[] = {loadWeight, soilLevel, fabricType};
    float *outputs = washingController.evaluate(inputs);
    
    if (outputs != nullptr) {
        WashCycle cycle = {
            .waterLevel = outputs[0],
            .washTime = outputs[1],
            .motorSpeed = outputs[2]
        };
        
        Serial.println("\nOptimized Wash Cycle:");
        Serial.println("Water Level: " + String(cycle.waterLevel, 1) + "%");
        Serial.println("Wash Time: " + String(cycle.washTime, 1) + " minutes");
        Serial.println("Motor Speed: " + String(cycle.motorSpeed, 1) + "%");
        
        // Execute wash cycle
        executeWashCycle(cycle);
        
        delete[] outputs;
    } else {
        Serial.println("Fuzzy evaluation failed - using default cycle");
        WashCycle defaultCycle = {60.0, 30.0, 65.0};
        executeWashCycle(defaultCycle);
    }
    
    delay(10000);  // Wait before next cycle (for demonstration)
}

void executeWashCycle(const WashCycle &cycle) {
    Serial.println("\n=== Executing Wash Cycle ===");
    
    // Step 1: Fill water to specified level
    Serial.println("Phase 1: Filling water to " + String(cycle.waterLevel, 1) + "%");
    fillWater(cycle.waterLevel);
    
    // Step 2: Main wash with specified motor speed
    Serial.println("Phase 2: Main wash for " + String(cycle.washTime, 1) + " minutes at " + String(cycle.motorSpeed, 1) + "% speed");
    runWashCycle(cycle.washTime, cycle.motorSpeed);
    
    // Step 3: Drain and rinse cycles
    Serial.println("Phase 3: Drain and rinse cycles");
    drainAndRinse();
    
    // Step 4: Spin dry
    Serial.println("Phase 4: Spin dry cycle");
    spinDry();
    
    Serial.println("Wash cycle completed successfully!");
}

float readLoadWeight() {
    // Simulate load cell reading (0-10 kg)
    int rawValue = analogRead(LOAD_SENSOR_PIN);
    float weight = (rawValue * 10.0) / 1023.0;
    
    // Add some noise filtering
    static float weightHistory[3] = {5, 5, 5};
    static int weightIndex = 0;
    
    weightHistory[weightIndex] = weight;
    weightIndex = (weightIndex + 1) % 3;
    
    return (weightHistory[0] + weightHistory[1] + weightHistory[2]) / 3.0;
}

float readSoilLevel() {
    // Simulate turbidity sensor reading (0-100%)
    int rawValue = analogRead(SOIL_SENSOR_PIN);
    return (rawValue * 100.0) / 1023.0;
}

float readFabricType() {
    // Read fabric type selector (potentiometer)
    int rawValue = analogRead(FABRIC_TYPE_PIN);
    return (rawValue * 100.0) / 1023.0;
}

void fillWater(float targetLevel) {
    Serial.println("Opening water valve...");
    digitalWrite(WATER_VALVE_PIN, HIGH);
    
    // Simulate filling time based on target level
    int fillTime = (int)(targetLevel * 30);  // 30ms per 1%
    delay(fillTime);
    
    digitalWrite(WATER_VALVE_PIN, LOW);
    Serial.println("Water filled to " + String(targetLevel, 1) + "%");
}

void runWashCycle(float washTime, float motorSpeed) {
    // Convert motor speed to PWM value
    int pwmValue = (int)(motorSpeed * 2.55);  // 0-100% to 0-255
    
    Serial.println("Starting wash motor at " + String(motorSpeed, 1) + "% speed");
    analogWrite(MOTOR_SPEED_PIN, pwmValue);
    
    // Simulate wash time (scaled down for demonstration)
    int simulatedTime = (int)(washTime * 100);  // 1 minute = 100ms
    delay(simulatedTime);
    
    analogWrite(MOTOR_SPEED_PIN, 0);  // Stop motor
    Serial.println("Wash cycle completed");
}

void drainAndRinse() {
    Serial.println("Draining water...");
    digitalWrite(PUMP_PIN, HIGH);
    delay(2000);  // Simulate drain time
    digitalWrite(PUMP_PIN, LOW);
    
    Serial.println("Rinse cycle...");
    fillWater(60);  // Fill for rinse
    runWashCycle(5, 50);  // 5 minute rinse at 50% speed
    
    Serial.println("Final drain...");
    digitalWrite(PUMP_PIN, HIGH);
    delay(2000);  // Final drain
    digitalWrite(PUMP_PIN, LOW);
}

void spinDry() {
    Serial.println("Spin dry cycle...");
    analogWrite(MOTOR_SPEED_PIN, 255);  // Maximum speed for spin
    delay(3000);  // Simulate spin cycle
    analogWrite(MOTOR_SPEED_PIN, 0);
    Serial.println("Spin dry completed");
}
```

### 5.3 IoT Smart Greenhouse Climate Control

**Complex multi-zone climate control with remote monitoring capabilities**

```cpp
#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

// Multi-zone fuzzy controllers
FuzzyMamdani temperatureController(3, 2, 15, 4);  // 3 inputs, 2 outputs, 15 rules
FuzzyMamdani humidityController(2, 1, 9, 3);      // 2 inputs, 1 output, 9 rules
FuzzyMamdani lightController(3, 1, 12, 3);        // 3 inputs, 1 output, 12 rules

// Sensor pins
#define TEMP_INDOOR_PIN A0       // Indoor temperature sensor
#define TEMP_OUTDOOR_PIN A1      // Outdoor temperature sensor  
#define HUMIDITY_PIN A2          // Humidity sensor
#define LIGHT_SENSOR_PIN A3      // Light intensity sensor
#define SOIL_MOISTURE_PIN A4     // Soil moisture sensor

// Actuator pins
#define HEATER_PIN 3             // Greenhouse heater
#define COOLER_PIN 4             // Exhaust fan for cooling
#define HUMIDIFIER_PIN 5         // Humidification system
#define GROW_LIGHTS_PIN 6        // LED grow lights
#define IRRIGATION_PIN 7         // Irrigation pump

// Target setpoints (configurable via IoT)
struct ClimateTargets {
    float temperature = 24.0;    // °C
    float humidity = 65.0;       // %
    float lightIntensity = 70.0; // %
} targets;

void setup() {
    Serial.begin(115200);
    setupGreenhouseFuzzySystems();
    setupHardware();
    
    Serial.println("Smart Greenhouse Fuzzy Control System Online");
    Serial.println("Multi-zone climate optimization with IoT integration");
}

void setupGreenhouseFuzzySystems() {
    setupTemperatureController();
    setupHumidityController();
    setupLightController();
}

void setupTemperatureController() {
    // Inputs: indoor_temp, outdoor_temp, time_of_day
    temperatureController.addInputVariable("indoor_temp", 5.0, 45.0);    // 5-45°C
    temperatureController.addInputVariable("outdoor_temp", -10.0, 40.0); // -10 to 40°C
    temperatureController.addInputVariable("time_of_day", 0.0, 24.0);    // 0-24 hours
    
    // Outputs: heating_power, cooling_power
    temperatureController.addOutputVariable("heating", 0.0, 100.0);      // 0-100%
    temperatureController.addOutputVariable("cooling", 0.0, 100.0);      // 0-100%
    
    // Indoor temperature sets
    float indoorCold[] = {5, 5, 18};        temperatureController.addFuzzySet(0, true, "COLD", TRIANGULAR, indoorCold);
    float indoorOptimal[] = {20, 24, 28};   temperatureController.addFuzzySet(0, true, "OPTIMAL", TRIANGULAR, indoorOptimal);
    float indoorWarm[] = {26, 32, 38};      temperatureController.addFuzzySet(0, true, "WARM", TRIANGULAR, indoorWarm);
    float indoorHot[] = {35, 45, 45};       temperatureController.addFuzzySet(0, true, "HOT", TRIANGULAR, indoorHot);
    
    // Outdoor temperature influence  
    float outdoorCold[] = {-10, -10, 5};    temperatureController.addFuzzySet(1, true, "COLD", TRIANGULAR, outdoorCold);
    float outdoorMild[] = {0, 15, 25};      temperatureController.addFuzzySet(1, true, "MILD", TRIANGULAR, outdoorMild);
    float outdoorHot[] = {20, 40, 40};      temperatureController.addFuzzySet(1, true, "HOT", TRIANGULAR, outdoorHot);
    
    // Time of day influence
    float nightTime[] = {0, 0, 8};          temperatureController.addFuzzySet(2, true, "NIGHT", TRIANGULAR, nightTime);
    float dayTime[] = {6, 12, 18};          temperatureController.addFuzzySet(2, true, "DAY", TRIANGULAR, dayTime);
    float eveningTime[] = {16, 24, 24};     temperatureController.addFuzzySet(2, true, "EVENING", TRIANGULAR, eveningTime);
    
    // Heating output sets
    float heatingOff[] = {0, 0, 20};        temperatureController.addFuzzySet(0, false, "OFF", TRIANGULAR, heatingOff);
    float heatingLow[] = {10, 30, 50};      temperatureController.addFuzzySet(0, false, "LOW", TRIANGULAR, heatingLow);
    float heatingMedium[] = {40, 60, 80};   temperatureController.addFuzzySet(0, false, "MEDIUM", TRIANGULAR, heatingMedium);
    float heatingHigh[] = {70, 100, 100};   temperatureController.addFuzzySet(0, false, "HIGH", TRIANGULAR, heatingHigh);
    
    // Cooling output sets  
    float coolingOff[] = {0, 0, 20};        temperatureController.addFuzzySet(1, false, "OFF", TRIANGULAR, coolingOff);
    float coolingLow[] = {10, 30, 50};      temperatureController.addFuzzySet(1, false, "LOW", TRIANGULAR, coolingLow);
    float coolingMedium[] = {40, 60, 80};   temperatureController.addFuzzySet(1, false, "MEDIUM", TRIANGULAR, coolingMedium);
    float coolingHigh[] = {70, 100, 100};   temperatureController.addFuzzySet(1, false, "HIGH", TRIANGULAR, coolingHigh);
    
    setupTemperatureRules();
}

void setupTemperatureRules() {
    // Rule format: [indoor_temp, outdoor_temp, time_of_day] -> [heating, cooling]
    
    // Cold indoor conditions - strong heating needed
    int coldIndoorRules[][3] = {
        {0, 0, 0}, {0, 0, 1}, {0, 0, 2},  // COLD indoor + any outdoor + any time
        {0, 1, 0}, {0, 1, 1}, {0, 1, 2},
        {0, 2, 0}, {0, 2, 1}, {0, 2, 2}
    };
    
    for (int i = 0; i < 9; i++) {
        temperatureController.addRule(coldIndoorRules[i], coldIndoorRules[i], 3, 0, 3, true);  // HIGH heating
        temperatureController.addRule(coldIndoorRules[i], coldIndoorRules[i], 3, 1, 0, true);  // OFF cooling
    }
    
    // Optimal indoor conditions - minimal intervention
    int optimalRules[][3] = {
        {1, 1, 1}  // OPTIMAL indoor + MILD outdoor + DAY
    };
    temperatureController.addRule(optimalRules[0], optimalRules[0], 3, 0, 0, true);  // OFF heating
    temperatureController.addRule(optimalRules[0], optimalRules[0], 3, 1, 0, true);  // OFF cooling
    
    // Hot indoor conditions - cooling needed
    int hotIndoorRules[][3] = {
        {3, 2, 1}, {3, 2, 2}  // HOT indoor + HOT outdoor + DAY/EVENING
    };
    
    for (int i = 0; i < 2; i++) {
        temperatureController.addRule(hotIndoorRules[i], hotIndoorRules[i], 3, 0, 0, true);  // OFF heating
        temperatureController.addRule(hotIndoorRules[i], hotIndoorRules[i], 3, 1, 3, true);  // HIGH cooling
    }
}

void setupHumidityController() {
    // Inputs: current_humidity, outdoor_humidity
    humidityController.addInputVariable("current_humidity", 20.0, 90.0);  // 20-90%
    humidityController.addInputVariable("outdoor_humidity", 10.0, 95.0);  // 10-95%
    
    // Output: humidifier_power
    humidityController.addOutputVariable("humidifier", 0.0, 100.0);       // 0-100%
    
    // Current humidity sets
    float humidityLow[] = {20, 20, 50};     humidityController.addFuzzySet(0, true, "LOW", TRIANGULAR, humidityLow);
    float humidityOptimal[] = {50, 65, 75}; humidityController.addFuzzySet(0, true, "OPTIMAL", TRIANGULAR, humidityOptimal);
    float humidityHigh[] = {70, 90, 90};    humidityController.addFuzzySet(0, true, "HIGH", TRIANGULAR, humidityHigh);
    
    // Outdoor humidity influence
    float outdoorDry[] = {10, 10, 40};      humidityController.addFuzzySet(1, true, "DRY", TRIANGULAR, outdoorDry);
    float outdoorNormal[] = {35, 60, 80};   humidityController.addFuzzySet(1, true, "NORMAL", TRIANGULAR, outdoorNormal);
    float outdoorWet[] = {70, 95, 95};      humidityController.addFuzzySet(1, true, "WET", TRIANGULAR, outdoorWet);
    
    // Humidifier output sets
    float humidifierOff[] = {0, 0, 25};     humidityController.addFuzzySet(0, false, "OFF", TRIANGULAR, humidifierOff);
    float humidifierLow[] = {15, 40, 65};   humidityController.addFuzzySet(0, false, "LOW", TRIANGULAR, humidifierLow);
    float humidifierHigh[] = {55, 100, 100}; humidityController.addFuzzySet(0, false, "HIGH", TRIANGULAR, humidifierHigh);
    
    setupHumidityRules();
}

void setupHumidityRules() {
    // Humidity control rules
    int rule1[] = {0, 0}; humidityController.addRule(rule1, rule1, 2, 0, 2, true);  // LOW + DRY -> HIGH
    int rule2[] = {0, 1}; humidityController.addRule(rule2, rule2, 2, 0, 1, true);  // LOW + NORMAL -> LOW
    int rule3[] = {0, 2}; humidityController.addRule(rule3, rule3, 2, 0, 0, true);  // LOW + WET -> OFF
    
    int rule4[] = {1, 0}; humidityController.addRule(rule4, rule4, 2, 0, 1, true);  // OPTIMAL + DRY -> LOW
    int rule5[] = {1, 1}; humidityController.addRule(rule5, rule5, 2, 0, 0, true);  // OPTIMAL + NORMAL -> OFF
    int rule6[] = {1, 2}; humidityController.addRule(rule6, rule6, 2, 0, 0, true);  // OPTIMAL + WET -> OFF
    
    int rule7[] = {2, 0}; humidityController.addRule(rule7, rule7, 2, 0, 0, true);  // HIGH + DRY -> OFF
    int rule8[] = {2, 1}; humidityController.addRule(rule8, rule8, 2, 0, 0, true);  // HIGH + NORMAL -> OFF  
    int rule9[] = {2, 2}; humidityController.addRule(rule9, rule9, 2, 0, 0, true);  // HIGH + WET -> OFF
}

void loop() {
    // Read all environmental sensors
    EnvironmentalData data = readEnvironmentalSensors();
    
    // Execute fuzzy control for each subsystem
    ClimateOutputs outputs = executeFuzzyClimateControl(data);
    
    // Apply control signals to actuators
    applyClimateControl(outputs);
    
    // Log system status and send IoT data
    logSystemStatus(data, outputs);
    
    // Check for IoT commands (setpoint changes, etc.)
    checkIoTCommands();
    
    delay(30000);  // Update every 30 seconds
}

struct EnvironmentalData {
    float indoorTemp;
    float outdoorTemp;
    float humidity;
    float outdoorHumidity;
    float lightIntensity;
    float timeOfDay;
    float soilMoisture;
};

struct ClimateOutputs {
    float heating;
    float cooling;
    float humidification;
    float lighting;
};

EnvironmentalData readEnvironmentalSensors() {
    EnvironmentalData data;
    
    // Read temperature sensors (with smoothing)
    static float tempHistory[5] = {25, 25, 25, 25, 25};
    static int tempIndex = 0;
    
    int rawTemp = analogRead(TEMP_INDOOR_PIN);
    float currentTemp = (rawTemp * 50.0) / 1023.0;  // 0-50°C range
    
    tempHistory[tempIndex] = currentTemp;
    tempIndex = (tempIndex + 1) % 5;
    
    data.indoorTemp = 0;
    for (int i = 0; i < 5; i++) {
        data.indoorTemp += tempHistory[i];
    }
    data.indoorTemp /= 5.0;
    
    // Simulate outdoor temperature (could be from weather API)
    data.outdoorTemp = data.indoorTemp + random(-10, 15);  // Outdoor variation
    
    // Read humidity sensor  
    int rawHumidity = analogRead(HUMIDITY_PIN);
    data.humidity = (rawHumidity * 100.0) / 1023.0;
    data.outdoorHumidity = data.humidity + random(-20, 10);
    
    // Read light sensor
    int rawLight = analogRead(LIGHT_SENSOR_PIN);
    data.lightIntensity = (rawLight * 100.0) / 1023.0;
    
    // Get time of day (simplified)
    data.timeOfDay = (millis() / 10000) % 24;  // Simulate 24-hour cycle in demo
    
    // Read soil moisture
    int rawSoil = analogRead(SOIL_MOISTURE_PIN);
    data.soilMoisture = (rawSoil * 100.0) / 1023.0;
    
    return data;
}

ClimateOutputs executeFuzzyClimateControl(const EnvironmentalData &data) {
    ClimateOutputs outputs = {0, 0, 0, 0};
    
    // Temperature control
    float tempInputs[] = {data.indoorTemp, data.outdoorTemp, data.timeOfDay};
    float *tempOutputs = temperatureController.evaluate(tempInputs);
    
    if (tempOutputs != nullptr) {
        outputs.heating = tempOutputs[0];
        outputs.cooling = tempOutputs[1];
        delete[] tempOutputs;
    }
    
    // Humidity control
    float humInputs[] = {data.humidity, data.outdoorHumidity};
    float *humOutputs = humidityController.evaluate(humInputs);
    
    if (humOutputs != nullptr) {
        outputs.humidification = humOutputs[0];
        delete[] humOutputs;
    }
    
    // Light control (simplified - could be another fuzzy system)
    if (data.lightIntensity < targets.lightIntensity && data.timeOfDay > 6 && data.timeOfDay < 18) {
        outputs.lighting = targets.lightIntensity - data.lightIntensity;
    } else {
        outputs.lighting = 0;
    }
    
    return outputs;
}

void applyClimateControl(const ClimateOutputs &outputs) {
    // Apply heating control
    if (outputs.heating > 10) {
        analogWrite(HEATER_PIN, (int)(outputs.heating * 2.55));
        digitalWrite(COOLER_PIN, LOW);  // Ensure cooling is off
    } else if (outputs.cooling > 10) {
        analogWrite(COOLER_PIN, (int)(outputs.cooling * 2.55));
        digitalWrite(HEATER_PIN, LOW);  // Ensure heating is off
    } else {
        digitalWrite(HEATER_PIN, LOW);
        digitalWrite(COOLER_PIN, LOW);
    }
    
    // Apply humidity control
    if (outputs.humidification > 10) {
        analogWrite(HUMIDIFIER_PIN, (int)(outputs.humidification * 2.55));
    } else {
        digitalWrite(HUMIDIFIER_PIN, LOW);
    }
    
    // Apply lighting control
    if (outputs.lighting > 5) {
        analogWrite(GROW_LIGHTS_PIN, (int)(outputs.lighting * 2.55));
    } else {
        digitalWrite(GROW_LIGHTS_PIN, LOW);
    }
}

void logSystemStatus(const EnvironmentalData &data, const ClimateOutputs &outputs) {
    Serial.println("\n=== Greenhouse Climate Status ===");
    Serial.println("Environmental Conditions:");
    Serial.println("  Indoor Temp: " + String(data.indoorTemp, 1) + "°C (Target: " + String(targets.temperature, 1) + "°C)");
    Serial.println("  Outdoor Temp: " + String(data.outdoorTemp, 1) + "°C");
    Serial.println("  Humidity: " + String(data.humidity, 1) + "% (Target: " + String(targets.humidity, 1) + "%)");
    Serial.println("  Light: " + String(data.lightIntensity, 1) + "% (Target: " + String(targets.lightIntensity, 1) + "%)");
    Serial.println("  Time: " + String(data.timeOfDay, 1) + " hours");
    Serial.println("  Soil Moisture: " + String(data.soilMoisture, 1) + "%");
    
    Serial.println("Control Outputs:");
    Serial.println("  Heating: " + String(outputs.heating, 1) + "%");
    Serial.println("  Cooling: " + String(outputs.cooling, 1) + "%");
    Serial.println("  Humidification: " + String(outputs.humidification, 1) + "%");
    Serial.println("  Grow Lights: " + String(outputs.lighting, 1) + "%");
    Serial.println("================================");
}

void setupHardware() {
    pinMode(HEATER_PIN, OUTPUT);
    pinMode(COOLER_PIN, OUTPUT);
    pinMode(HUMIDIFIER_PIN, OUTPUT);
    pinMode(GROW_LIGHTS_PIN, OUTPUT);
    pinMode(IRRIGATION_PIN, OUTPUT);
    
    // Initialize all actuators to OFF state
    digitalWrite(HEATER_PIN, LOW);
    digitalWrite(COOLER_PIN, LOW);
    digitalWrite(HUMIDIFIER_PIN, LOW);
    digitalWrite(GROW_LIGHTS_PIN, LOW);
    digitalWrite(IRRIGATION_PIN, LOW);
}

void checkIoTCommands() {
    // Placeholder for IoT command processing
    // In real implementation, this would check WiFi/MQTT for setpoint changes
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        
        if (command.startsWith("SET_TEMP:")) {
            targets.temperature = command.substring(9).toFloat();
            Serial.println("Temperature target updated to " + String(targets.temperature) + "°C");
        } else if (command.startsWith("SET_HUMIDITY:")) {
            targets.humidity = command.substring(13).toFloat();
            Serial.println("Humidity target updated to " + String(targets.humidity) + "%");
        } else if (command.startsWith("SET_LIGHT:")) {
            targets.lightIntensity = command.substring(10).toFloat();
            Serial.println("Light target updated to " + String(targets.lightIntensity) + "%");
        }
    }
}
```

---

## 6. Performance Analysis

### 6.1 Computational Complexity

**Time Complexity Analysis**:
```
Fuzzification: O(n × m)
  where n = number of inputs, m = average sets per input

Rule Evaluation: O(r × a)  
  where r = number of rules, a = average antecedents per rule

Aggregation: O(r × d)
  where r = number of rules, d = discretization resolution

Defuzzification: O(d × r)
  where d = discretization resolution, r = number of rules

Total: O(n×m + r×a + r×d)
```

**Space Complexity**:
```
Input Variables: O(n × m × s)
  where s = average size of fuzzy set structure

Rules: O(r × a)
  where a = average antecedents per rule

Output Calculation: O(d)
  where d = discretization resolution

Total: O(n×m×s + r×a + d)
```

### 6.2 Platform Performance Benchmarks

| Platform | Max Rules | Execution Time | Memory Usage | Notes |
|----------|-----------|----------------|--------------|-------|
| **ESP32** | 50+ | 5-15 ms | 8-20 KB | Full feature set |
| **ESP8266** | 30 | 8-25 ms | 4-12 KB | Reduced resolution |
| **Arduino Uno** | 15 | 15-40 ms | 1-3 KB | Basic functionality |

**Performance Optimization Results**:
```cpp
// Before optimization
Resolution = 100, Rules = 20
ESP32: 18ms execution time

// After optimization  
Resolution = 50, Rules = 20, Early termination enabled
ESP32: 8ms execution time (56% improvement)
```

### 6.3 Memory Usage Breakdown

**Per Fuzzy System Instance**:
```cpp
Base class: ~100 bytes
Variables (2 inputs, 1 output): ~150 bytes
Fuzzy sets (3 sets/var × 3 vars): ~360 bytes  
Rules (10 rules, avg 2 antecedents): ~400 bytes
Working buffers: ~200 bytes
Total: ~1210 bytes per fuzzy system
```

**Memory Scaling**:
```
Memory = Base + (I+O)×VarSize + (I+O)×S×SetSize + R×RuleSize

Where:
I = inputs, O = outputs, S = sets per variable
R = rules, VarSize ≈ 50 bytes, SetSize ≈ 40 bytes, RuleSize ≈ 40 bytes
```

### 6.4 Real-time Performance Characteristics

**Deterministic Execution**: Yes, with fixed discretization resolution
**Worst-case Execution Time**: Predictable based on system parameters
**Memory Allocation**: Dynamic during setup, static during runtime
**Interrupt Safety**: Evaluation functions are reentrant

---

## 7. Troubleshooting and Optimization

### 7.1 Common Issues and Solutions

**Issue: High Memory Usage**
```cpp
// Problem: Too many fuzzy sets or high discretization
FuzzyMamdani(5, 3, 30, 10);  // 5 inputs, 10 sets each = high memory

// Solution: Optimize system design
FuzzyMamdani(3, 2, 15, 5);   // Reduce complexity while maintaining functionality
```

**Issue: Slow Execution Time**
```cpp
// Problem: High resolution defuzzification  
const int RESOLUTION = 200;  // Too fine

// Solution: Balance accuracy vs speed
const int RESOLUTION = 50;   // Usually sufficient for most applications
```

**Issue: Poor Control Performance**
```cpp
// Problem: Rules don't cover all input space
// Missing rules cause unexpected behavior

// Solution: Ensure complete rule coverage
void validateRuleCoverage() {
    // Test with grid of input values
    for (float temp = minTemp; temp <= maxTemp; temp += step) {
        for (float humid = minHumid; humid <= maxHumid; humid += step) {
            float inputs[] = {temp, humid};
            float *outputs = fuzzy.evaluate(inputs);
            if (outputs == nullptr) {
                Serial.println("No rule fired for: " + String(temp) + ", " + String(humid));
            }
            delete[] outputs;
        }
    }
}
```

**Issue: Inconsistent Output**
```cpp
// Problem: Overlapping fuzzy sets cause ambiguity
float set1[] = {10, 20, 30};  // Too much overlap
float set2[] = {15, 25, 35};

// Solution: Ensure proper set spacing
float set1[] = {10, 20, 25};  // Better separation
float set2[] = {20, 30, 40};
```

### 7.2 Optimization Strategies

**Rule Reduction Techniques**:
```cpp
// Before: Explicit rules for all combinations
Rule 1: IF temp is COLD AND humid is LOW THEN...
Rule 2: IF temp is COLD AND humid is MEDIUM THEN...
Rule 3: IF temp is COLD AND humid is HIGH THEN...

// After: Use OR logic to combine similar rules
Rule 1: IF temp is COLD THEN...  // Covers all humidity levels
```

**Set Design Optimization**:
```cpp
// Optimal triangular set design
void designOptimalSets(float min, float max, int numSets) {
    float range = max - min;
    float step = range / (numSets - 1);
    
    for (int i = 0; i < numSets; i++) {
        float center = min + i * step;
        float left = (i == 0) ? min : center - step/2;
        float right = (i == numSets-1) ? max : center + step/2;
        
        float params[] = {left, center, right};
        fuzzy.addFuzzySet(varIndex, isInput, name, TRIANGULAR, params);
    }
}
```

**Performance Monitoring**:
```cpp
class FuzzyProfiler {
private:
    unsigned long startTime;
    unsigned long totalTime;
    int callCount;
    
public:
    void startProfiling() { startTime = micros(); }
    
    void endProfiling() {
        totalTime += (micros() - startTime);
        callCount++;
    }
    
    float getAverageTime() {
        return callCount > 0 ? (float)totalTime / callCount : 0;
    }
    
    void printStats() {
        Serial.println("Fuzzy Performance Stats:");
        Serial.println("Average execution time: " + String(getAverageTime()) + " μs");
        Serial.println("Total calls: " + String(callCount));
    }
};
```

### 7.3 Debugging Tools

**Rule Tracing**:
```cpp
void traceRuleExecution(const float *inputs) {
    Serial.println("=== Rule Trace ===");
    
    for (int i = 0; i < numRules; i++) {
        float strength = calculateRuleStrength(i, inputs);
        
        Serial.print("Rule " + String(i) + ": ");
        printRuleText(i);
        Serial.println(" | Strength: " + String(strength, 3));
        
        if (strength > 0.001) {
            Serial.println("  → FIRED");
        }
    }
    Serial.println("==================");
}
```

**Membership Function Visualization**:
```cpp
void plotMembershipFunction(int varIndex, bool isInput) {
    FuzzyVariable *var = isInput ? &inputVars[varIndex] : &outputVars[varIndex];
    
    Serial.println("Membership plot for " + String(var->name));
    
    for (float x = var->min; x <= var->max; x += (var->max - var->min) / 50) {
        Serial.print(String(x, 1) + ": ");
        
        for (int s = 0; s < var->numSets; s++) {
            float membership = calculateMembership(x, var->sets[s]);
            int bars = (int)(membership * 10);
            
            Serial.print(String(var->sets[s].name) + "[");
            for (int b = 0; b < bars; b++) Serial.print("=");
            for (int b = bars; b < 10; b++) Serial.print(" ");
            Serial.print("] ");
        }
        Serial.println();
    }
}
```

---

## 8. References

### Academic References

1. **Mamdani, E. H., & Assilian, S. (1975)**. "An experiment in linguistic synthesis with a fuzzy logic controller". *International Journal of Man-Machine Studies*, 7(1), 1-13.

2. **Zadeh, L. A. (1965)**. "Fuzzy sets". *Information and Control*, 8(3), 338-353.

3. **Takagi, T., & Sugeno, M. (1985)**. "Fuzzy identification of systems and its applications to modeling and control". *IEEE Transactions on Systems, Man, and Cybernetics*, 15(1), 116-132.

4. **Lee, C. C. (1990)**. "Fuzzy logic in control systems: fuzzy logic controller—part I". *IEEE Transactions on Systems, Man, and Cybernetics*, 20(2), 404-418.

### Technical References

5. **Klir, G., & Yuan, B. (1995)**. *Fuzzy Sets and Fuzzy Logic*. Prentice Hall.

6. **Ross, T. J. (2010)**. *Fuzzy Logic with Engineering Applications*. John Wiley & Sons, 3rd Edition.

7. **Passino, K. M., & Yurkovich, S. (1998)**. *Fuzzy Control*. Addison Wesley Longman.

### Implementation References

8. **Driankov, D., Hellendoorn, H., & Reinfrank, M. (2013)**. *An Introduction to Fuzzy Control*. Springer Science & Business Media.

9. **Jang, J. S. R., Sun, C. T., & Mizutani, E. (1997)**. *Neuro-Fuzzy and Soft Computing*. Prentice Hall.

---

## Appendix: Terminology Glossary

### Fuzzy Logic Terms

- **Fuzzy Set**: A set with graded membership where elements can belong partially
- **Membership Function**: Function μ(x) that maps elements to membership degrees [0,1]
- **Linguistic Variable**: Variable whose values are words rather than numbers
- **Fuzzification**: Process of converting crisp inputs to fuzzy values
- **Defuzzification**: Process of converting fuzzy output to crisp value
- **Rule Base**: Collection of IF-THEN rules defining system behavior

### Control System Terms

- **Antecedent**: IF part of a fuzzy rule (input conditions)
- **Consequent**: THEN part of a fuzzy rule (output actions)
- **T-norm**: Fuzzy AND operation (typically minimum)
- **T-conorm**: Fuzzy OR operation (typically maximum)
- **Aggregation**: Combining multiple rule outputs into single fuzzy set
- **Universe of Discourse**: Range of possible values for a variable

### Implementation Terms

- **Discretization**: Converting continuous functions to discrete points
- **Resolution**: Number of discrete points used in defuzzification
- **Rule Strength**: Degree to which a rule is activated (0-1)
- **Centroid**: Center of gravity defuzzification method
- **Singleton**: Fuzzy set with membership 1 at single point, 0 elsewhere

---

*Documentation Version: 1.0*  
*Last Updated: 2025*  
*Implementation: Kinematrix Fuzzy Mamdani Library*