# CLAUDE.md - Control Algorithms

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Directory Overview
Advanced control algorithms and machine learning implementations for embedded systems automation.

## Available Controllers

### PID Controllers
- **`PID.h/cpp`** - Basic PID controller implementation
- **`PIDController.h/cpp`** - Enhanced PID with auto-tuning and EEPROM storage

#### Features
- Ziegler-Nichols tuning methods (Type 1 & 2)
- Cohen-Coon tuning algorithm
- Integral windup protection
- Parameter persistence in EEPROM
- Real-time parameter adjustment

### Fuzzy Logic Systems
Three distinct fuzzy logic implementations for different control scenarios:

#### **`FuzzyMamdani.h/cpp`** - Mamdani Fuzzy Controller
- Traditional fuzzy logic with max-min inference
- Centroid defuzzification
- Ideal for: HVAC systems, fan control, irrigation

#### **`FuzzySugeno.h/cpp`** - Sugeno Fuzzy System  
- Linear consequent functions
- Weighted average defuzzification
- Ideal for: Robot navigation, adaptive control systems

#### **`FuzzyTsukamoto.h/cpp`** - Tsukamoto Fuzzy Inference
- Monotonic membership functions
- Height defuzzification method
- Ideal for: Temperature control, water level management

### Machine Learning
#### **`DecisionTree.h/cpp`** - Decision Tree Classifier
- Information gain-based splitting
- Pruning capabilities to prevent overfitting
- Cross-validation support
- Model save/load functionality
- Applications: Soil classification, weather prediction

#### **`KNN.h/cpp`** - K-Nearest Neighbors
- Multiple distance metrics (Euclidean, Manhattan, Chebyshev)
- Weighted KNN support
- Data normalization
- Cross-validation with confusion matrix
- Applications: Gesture recognition, currency detection, pattern classification

## Shared Header
- **`FuzzyHeader.h`** - Common definitions and structures for fuzzy logic systems

## Usage Examples

### PID Controller
```cpp
#define ENABLE_MODULE_PID_CONTROLLER
#include "Kinematrix.h"

PIDController pid(2.0, 1.0, 0.5); // Kp, Ki, Kd
pid.setSetpoint(25.0);
double output = pid.compute(currentTemperature);
```

### Fuzzy Logic (Mamdani)
```cpp
#define ENABLE_MODULE_FUZZY_MAMDANI
#include "Kinematrix.h"

FuzzyMamdani fuzzy;
fuzzy.addInputVariable("temperature", 0, 40);
fuzzy.addOutputVariable("fanSpeed", 0, 100);
double output = fuzzy.processInput(temperature);
```

### Decision Tree
```cpp
#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

DecisionTree tree;
tree.trainFromData(features, labels, numSamples);
int prediction = tree.predict(newFeatures);
```

## Platform Compatibility
- **ESP32**: Full feature set including complex algorithms
- **ESP8266**: All algorithms with memory optimization
- **AVR**: PID and basic fuzzy logic (memory constraints)

## Performance Characteristics
- **PID**: ~100μs execution time
- **Fuzzy Logic**: ~500μs - 2ms depending on rules
- **Decision Tree**: ~200μs for prediction
- **KNN**: Variable based on training set size

## Applications
- Industrial process control
- Smart home automation
- Robotics and autonomous systems
- Environmental monitoring
- Agricultural automation
- Educational demonstrations

## Architecture and Implementation Details

### Module Enable System
All control algorithms use the conditional compilation system defined in `lib/enable.h`:
```cpp
#define ENABLE_MODULE_PID_CONTROLLER      // Enhanced PID with auto-tuning
#define ENABLE_MODULE_PID                 // Basic PID implementation
#define ENABLE_MODULE_FUZZY_MAMDANI       // Mamdani fuzzy logic
#define ENABLE_MODULE_FUZZY_SUGENO        // Sugeno fuzzy system
#define ENABLE_MODULE_FUZZY_TSUKAMOTO     // Tsukamoto fuzzy inference
#define ENABLE_MODULE_DECISION_TREE       // Decision tree classifier
#define ENABLE_MODULE_KNN                 // K-Nearest Neighbors
```

### Common Architecture Patterns

#### Platform-Specific Implementations
All modules use platform detection for optimal memory usage:
- **ESP32/ESP8266**: Full feature set with EEPROM/SPIFFS persistence
- **AVR platforms**: Memory-optimized versions with reduced features
- Automatic platform detection via `#if defined(ESP32) || defined(ESP8266)`

#### Memory Management Strategy
- **Dynamic allocation**: Used for training data in ML algorithms (DecisionTree, KNN)
- **Static buffers**: Used for real-time control (PID, Fuzzy Logic)
- **EEPROM persistence**: Parameter storage for PIDController with platform-specific handling
- **Low memory modes**: Available in KNN and DecisionTree for resource-constrained platforms

#### Error Handling and Validation
- Input validation for all parameter ranges
- Graceful degradation on memory allocation failures
- Bounds checking for array operations
- Timeout mechanisms for auto-tuning processes

### Development Workflow

#### Testing Control Algorithms
```bash
# Build and test on Arduino platform (memory-constrained)
pio run -e uno

# Build and test on ESP32 (full features)
pio run -e esp32doit-devkit-v1

# Run specific control algorithm examples
pio run --target upload -e esp32doit-devkit-v1
```

#### Examples Organization
Control examples are located in `example/modules/control/`:
- **EXAMPLE-PIDController/**: 6 comprehensive PID examples including auto-tuning
- **EXAMPLE-FuzzyMamdani/**: 2 real-world fuzzy control examples
- **EXAMPLE-FuzzySugeno/**: 2 Sugeno fuzzy system examples
- **EXAMPLE-FuzzyTsukamoto/**: 4 Tsukamoto inference examples

#### Key Implementation Files
- **`FuzzyHeader.h`**: Shared enums and structures for all fuzzy logic systems
- **`PIDController.cpp`**: Contains auto-tuning algorithms (Ziegler-Nichols, Cohen-Coon)
- **`DecisionTree.cpp`**: Information gain calculation and tree pruning logic
- **`KNN.cpp`**: Multiple distance metrics and cross-validation implementation

### Algorithm-Specific Notes

#### PID Controller Advanced Features
- **Auto-tuning**: Implements oscillation-based parameter identification
- **Derivative filtering**: Reduces noise in derivative term with configurable alpha
- **Setpoint ramping**: Prevents sudden setpoint changes
- **Output rate limiting**: Controls maximum change rate
- **Integral windup protection**: Configurable integral limits

#### Fuzzy Logic Systems
- **Rule definition**: Text-based rule parsing with AND/OR operators
- **Membership functions**: Triangular, trapezoidal, Gaussian, singleton support
- **Defuzzification**: Multiple methods (centroid, bisector, MOM, SOM, LOM)
- **Variable scaling**: Automatic input/output range handling

#### Machine Learning Algorithms
- **Feature handling**: Mixed numeric/categorical data support
- **Cross-validation**: K-fold validation with confusion matrix generation
- **Model persistence**: Save/load functionality using SPIFFS on ESP32
- **Performance optimization**: Distance calculations optimized for embedded systems