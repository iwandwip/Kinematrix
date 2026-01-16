# CLAUDE.md - Source Headers

## Directory Overview
Main library headers providing unified API access to all Kinematrix modules, sensors, and add-ons.

## Core Headers

### `Kinematrix.h` - Master Header
Primary include file that orchestrates all library components:
```cpp
#include "KinematrixDevelopment.h"
#include "KinematrixModules.h"
#include "KinematrixModulesHelper.h"
#include "KinematrixModulesNoDef.h"
#include "KinematrixSensor.h"
#include "KinematrixSensorHelper.h"
#include "KinematrixSensorNoDef.h"
#include "KinematrixAddons.h"
```

### Development Integration
- **`KinematrixDevelopment.h`** - Development and experimental modules

### Module Integration Headers
- **`KinematrixModules.h`** - Main module orchestration
- **`KinematrixModulesHelper.h`** - Module helper functions and utilities
- **`KinematrixModulesNoDef.h`** - Module inclusion without enable definitions

### Sensor Integration Headers
- **`KinematrixSensor.h`** - Sensor framework orchestration
- **`KinematrixSensorHelper.h`** - Sensor helper functions and utilities
- **`KinematrixSensorNoDef.h`** - Sensor inclusion without enable definitions

### Add-on Integration
- **`KinematrixAddons.h`** - Complete application systems integration

## Usage Patterns

### Standard Usage
```cpp
// Define required modules
#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_MODULE_WIFI_HANDLER_V2
#define ENABLE_MODULE_PID_CONTROLLER

// Include main header
#include "Kinematrix.h"

void setup() {
    // All enabled modules now available
}
```

### Development Usage
```cpp
// Define development modules
#define ENABLE_DEVELOPMENT_CERIA_SERIAL

// Include main header
#include "Kinematrix.h"

void setup() {
    // Development modules available
}
```

### Selective Inclusion
```cpp
// Include only modules without definitions
#include "KinematrixModulesNoDef.h"

// Manually include specific headers
#include "lib/modules/control/PID.h"
```

### Helper Functions
```cpp
#include "KinematrixModulesHelper.h"
// Access to utility functions for modules

#include "KinematrixSensorHelper.h" 
// Access to sensor framework utilities
```

## Compilation Messages
Each header provides compilation feedback:
- `[INFO]: compiling Kinematrix started`
- `[INFO]: compiling Kinematrix done`
- Module-specific compilation messages

## Architecture Benefits

### Unified API
- Single include point for entire library
- Consistent naming conventions
- Platform abstraction layer

### Conditional Compilation
- Include only required components
- Optimize memory usage
- Reduce compilation time

### Hierarchical Organization
- Logical grouping of functionality
- Clear dependency management
- Scalable architecture

## Fixed Architecture Summary (Latest Update)

### Triple Include System Architecture ✅ COMPLETED
The project now implements a **perfectly consistent** 3-tier include system:

#### **Production Files** (.h + .cpp includes)
- **KinematrixModules.h**: 88 modules, 97 .cpp includes ✅
- **KinematrixModulesHelper.h**: 88 modules, 97 .cpp includes ✅
- **KinematrixSensor.h**: 78 .cpp includes ✅  
- **KinematrixSensorHelper.h**: 78 .cpp includes ✅

#### **Header-Only Files** (.h includes only)
- **KinematrixModulesNoDef.h**: 88 modules, 0 .cpp includes ✅
- **KinematrixSensorNoDef.h**: 72 .h includes, 0 .cpp includes ✅

### Recent Fixes Applied
1. **✅ Added Missing SERIAL_CERIA Module** - All 3 module files updated
2. **✅ Added Missing STANDARD_SCALER & TRAIN_TEST_SPLIT** - Helper & NoDef files updated
3. **✅ Fixed DynamicType Filter Includes** - Added all 6 missing .cpp includes:
   - DynamicTypeMovingAverageFilter.cpp
   - DynamicTypeExponentialMovingAverageFilter.cpp
   - DynamicTypeHighPassFilter.cpp
   - DynamicTypeMedianFilter.cpp
   - DynamicTypeComplementaryFilter.cpp
   - DynamicTypeBandStopFilter.cpp
4. **✅ Achieved Perfect Consistency** - All files now follow correct patterns

### Include Pattern Examples
```cpp
// PRODUCTION PATTERN (.h + .cpp) - KinematrixModules.h
#ifdef ENABLE_MODULE_I2C_SCANNER
#include "../lib/modules/communication/wired/i2c/i2c-scanner.h"
#include "../lib/modules/communication/wired/i2c/i2c-scanner.cpp"
#endif

// HELPER PATTERN (.h + .cpp) - KinematrixModulesHelper.h
#ifdef ENABLE_MODULE_HELPER_I2C_SCANNER
#include "../lib/modules/communication/wired/i2c/i2c-scanner.h"
#include "../lib/modules/communication/wired/i2c/i2c-scanner.cpp"
#endif

// HEADER-ONLY PATTERN (.h only) - KinematrixModulesNoDef.h
#ifdef ENABLE_MODULE_NODEF_I2C_SCANNER
#include "../lib/modules/communication/wired/i2c/i2c-scanner.h"
#endif
```

## Platform Compatibility
Headers automatically detect platform capabilities:
- ESP32: Full feature set
- ESP8266: WiFi features with optimization
- AVR: Basic modules with memory constraints

## Memory Optimization
- Conditional compilation reduces binary size
- Selective feature inclusion
- Platform-appropriate optimizations

## Development Workflow
1. Define required modules in `lib/enable.h` or before include
2. Include `Kinematrix.h`
3. Access all enabled functionality through unified API
4. Use helper headers for advanced utilities

## Integration Points
- Works with PlatformIO library system
- Compatible with Arduino IDE
- Supports external library dependencies
- Integrates with Arduino Library Manager