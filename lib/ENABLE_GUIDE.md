# Enable.h Module Addition Guide

## Overview

This guide provides comprehensive instructions for adding new modules to the `enable.h` file in the Kinematrix framework. The `enable.h` file serves as the central configuration system for conditional compilation, allowing users to selectively include only the required components.

## File Structure

The `enable.h` file is organized hierarchically based on the physical directory structure:

```
// === ADDONS ===
// === DEVELOPMENT ===
// === MODULES ===
// === SENSORS ===
```

## Adding New Modules

### Step 1: Determine Module Category

First, identify which category your new module belongs to:

- **ADDONS**: Complete application systems (e.g., AutoLight variants)
- **DEVELOPMENT**: Experimental and project-specific modules under active development
- **MODULES**: Core functionality modules organized by purpose
- **SENSORS**: Sensor framework components (V1 or V2)

### Step 2: Locate Directory Path

Identify the exact directory path where your module files are located:

```
lib/
├── addons/
├── development/
│   └── CeriaDev/
├── modules/
│   ├── communication/
│   │   ├── wired/
│   │   │   ├── i2c/
│   │   │   ├── modbus/
│   │   │   ├── serial/
│   │   │   └── spi/
│   │   └── wireless/
│   │       ├── lora/
│   │       └── now/
│   ├── control/
│   ├── debug/
│   ├── display/
│   ├── driver/
│   │   ├── motor/
│   │   └── servo/
│   ├── file/
│   ├── filter/
│   ├── io/
│   ├── task/
│   ├── time/
│   ├── utils/
│   └── wifi/
└── sensors/
    ├── SensorModuleV1/
    └── SensorModuleV2/
```

### Step 3: Follow Naming Convention

Use the appropriate naming pattern:

#### For MODULES:
```cpp
#define ENABLE_MODULE_[NAME]
```

#### For ADDONS:
```cpp
#define ENABLE_ADDONS_[NAME]
```

#### For DEVELOPMENT:
```cpp
#define ENABLE_DEVELOPMENT_[PROJECT]_[NAME]
```

#### For SENSORS:
```cpp
#define ENABLE_SENSOR_[NAME]           // For V1 sensors
#define ENABLE_SENSOR_[NAME]_V2        // For V2 sensors
```

### Step 4: Insert in Correct Location

#### 4.1 Find the Appropriate Section

Locate the comment section that matches your module's directory path:

```cpp
// modules/communication/wired/serial
#define ENABLE_MODULE_SERIAL_CERIA
#define ENABLE_MODULE_SERIAL_ENHANCED
// ... other serial modules
```

#### 4.2 Insert Alphabetically

Within each section, modules must be ordered alphabetically by their DEFINE name:

**❌ Incorrect:**
```cpp
// modules/filter
#define ENABLE_MODULE_KALMAN_FILTER
#define ENABLE_MODULE_HIGH_PASS_FILTER  // Wrong position
#define ENABLE_MODULE_LOW_PASS_FILTER
```

**✅ Correct:**
```cpp
// modules/filter
#define ENABLE_MODULE_HIGH_PASS_FILTER
#define ENABLE_MODULE_KALMAN_FILTER
#define ENABLE_MODULE_LOW_PASS_FILTER
```

### Step 5: Add Section Comment (if needed)

If you're adding a module to a new directory that doesn't exist in `enable.h`, add a new section comment:

```cpp
// modules/[category]/[subcategory]
#define ENABLE_MODULE_YOUR_NEW_MODULE
```

## Detailed Examples

### Example 1: Adding a New Development Module

**Scenario**: Adding a new protocol to `development/CeriaDev/`

**Steps**:

1. **Identify Category**: DEVELOPMENT
2. **Directory Path**: `development/CeriaDev/`
3. **Module Name**: `ceria-mqtt`
4. **Define Name**: `ENABLE_DEVELOPMENT_CERIA_MQTT`

**Implementation**:
```cpp
// development/CeriaDev
#define ENABLE_DEVELOPMENT_CERIA_MQTT
#define ENABLE_DEVELOPMENT_CERIA_SERIAL
```

### Example 2: Adding a New Communication Module

**Scenario**: Adding a new Bluetooth module to `modules/communication/wireless/bluetooth/`

**Steps**:

1. **Identify Category**: MODULES
2. **Directory Path**: `modules/communication/wireless/bluetooth/`
3. **Module Name**: `bluetooth-handler`
4. **Define Name**: `ENABLE_MODULE_BLUETOOTH_HANDLER`

**Implementation**:
```cpp
// modules/communication/wireless/bluetooth
#define ENABLE_MODULE_BLUETOOTH_HANDLER

// modules/communication/wireless/lora
#define ENABLE_MODULE_LORA_COM
// ... existing lora modules
```

### Example 3: Adding a New Sensor V2

**Scenario**: Adding a new BME680 sensor to `sensors/SensorModuleV2/SensorList/BME680SensV2/`

**Steps**:

1. **Identify Category**: SENSORS
2. **Directory Path**: `sensors/SensorModuleV2/SensorList/`
3. **Module Name**: `BME680SensV2`
4. **Define Name**: `ENABLE_SENSOR_BME680_V2`

**Implementation**:
```cpp
// sensors/SensorModuleV2/SensorList (individual sensors)
#define ENABLE_SENSOR_ABSTRACT_V2
#define ENABLE_SENSOR_ANALOG_V2
#define ENABLE_SENSOR_BME680_V2        // Insert alphabetically
#define ENABLE_SENSOR_CUSTOM_TEMPLATE_V2
// ... other V2 sensors
```

### Example 4: Adding a New Filter Module

**Scenario**: Adding a Butterworth filter to `modules/filter/`

**Steps**:

1. **Identify Category**: MODULES
2. **Directory Path**: `modules/filter/`
3. **Module Name**: `butterworth-filter`
4. **Define Name**: `ENABLE_MODULE_BUTTERWORTH_FILTER`

**Implementation**:
```cpp
// modules/filter
#define ENABLE_MODULE_BAND_STOP_FILTER
#define ENABLE_MODULE_BUTTERWORTH_FILTER    // Insert alphabetically
#define ENABLE_MODULE_COMPLEMENTARY_FILTER
// ... other filter modules
```

## Special Cases

### Multiple File Modules

For modules with multiple related files, use descriptive suffixes:

```cpp
// modules/communication/wired/modbus
#define ENABLE_MODULE_MODBUS_MASTER
#define ENABLE_MODULE_MODBUS_SLAVE
#define ENABLE_MODULE_MODBUS_TCP
```

### Version Variants

For modules with multiple versions, include version suffix:

```cpp
// modules/wifi
#define ENABLE_MODULE_FIREBASE_RTDB_V2
#define ENABLE_MODULE_FIREBASE_RTDB_V3
#define ENABLE_MODULE_WIFI_HANDLER
#define ENABLE_MODULE_WIFI_HANDLER_V2
```

### Development Modules

For modules in development directories, use the DEVELOPMENT category:

```cpp
// development/CeriaDev
#define ENABLE_DEVELOPMENT_CERIA_SERIAL
#define ENABLE_DEVELOPMENT_CERIA_MQTT
```

### Moved Modules

For modules that have been moved from one location to another, remove them from the old location:

```cpp
// OLD - modules/communication/wired/serial
// #define ENABLE_MODULE_SERIAL_CERIA    // Moved to development

// NEW - development/CeriaDev  
#define ENABLE_DEVELOPMENT_CERIA_SERIAL
```

## Validation Checklist

Before committing your changes, verify:

- [ ] **Category**: Module is in the correct main category (ADDONS/DEVELOPMENT/MODULES/SENSORS)
- [ ] **Section**: Module is under the correct directory comment
- [ ] **Alphabetical**: Module is inserted in alphabetical order within its section
- [ ] **Naming**: Define name follows the established convention
- [ ] **Consistency**: Formatting matches existing entries
- [ ] **Comments**: Any special notes are included (if applicable)

## Common Mistakes to Avoid

### ❌ Wrong Category Placement
```cpp
// Don't put sensor modules in MODULES section
#define ENABLE_MODULE_BME680_SENSOR  // Wrong - should be in SENSORS
```

### ❌ Incorrect Alphabetical Order
```cpp
// modules/filter
#define ENABLE_MODULE_MOVING_AVERAGE_FILTER
#define ENABLE_MODULE_KALMAN_FILTER          // Wrong - should come before MOVING_AVERAGE
```

### ❌ Missing Directory Comment
```cpp
// Don't add modules without proper section header
#define ENABLE_MODULE_NEW_MODULE  // Wrong - needs directory comment above
```

### ❌ Inconsistent Naming
```cpp
// Don't use inconsistent naming patterns
#define ENABLE_NEW_MODULE          // Wrong - missing category prefix
#define ENABLE_MODULE_newmodule    // Wrong - should be UPPER_CASE
```

## Best Practices

1. **Follow Existing Patterns**: Study the current structure before adding new modules
2. **Use Descriptive Names**: Module names should clearly indicate functionality
3. **Group Related Modules**: Keep related functionality together in the same section
4. **Document Special Cases**: Add comments for moved, deprecated, or special modules
5. **Test Thoroughly**: Ensure your additions don't break existing functionality
6. **Maintain Consistency**: Follow the established formatting and naming conventions

## Integration with Build System

After adding modules to `enable.h`, ensure they are properly integrated:

1. **Include Files**: Update orchestration headers (`KinematrixModules.h`, etc.)
2. **Examples**: Create appropriate usage examples
3. **Documentation**: Update relevant documentation
4. **Testing**: Verify compilation across target platforms

## Tools for Verification

### Count Total Defines
```bash
grep -c "#define ENABLE_" lib/enable.h
```

### Verify Alphabetical Order (per section)
```bash
grep "#define ENABLE_MODULE_" lib/enable.h | sort
```

### Check for Duplicates
```bash
grep "#define ENABLE_" lib/enable.h | sort | uniq -d
```

## Conclusion

Following this guide ensures that new modules are properly integrated into the Kinematrix framework's conditional compilation system. Proper organization in `enable.h` maintains code readability, makes it easier for users to find and enable specific modules, and ensures the framework remains maintainable as it grows.

Remember: **Organization today prevents confusion tomorrow.**