# CeriaModuleESP32RTOS - Simple & Powerful RTOS Wrapper

## Overview
CeriaModuleESP32RTOS adalah wrapper class minimal namun powerful untuk ESP32 FreeRTOS yang menyederhanakan task management, memory monitoring, dan system information dengan API yang clean dan mudah digunakan.

## Key Features
- **Simple Task Creation**: Easy task creation dengan pinning ke specific core
- **Memory Monitoring**: Real-time heap monitoring dengan fragmentation detection
- **Task Management**: Complete task lifecycle management (create, delete, suspend, resume)
- **System Information**: Comprehensive system stats dan runtime information
- **Callback System**: Event-driven notifications untuk memory alerts dan task state changes
- **Static Utilities**: Helper functions untuk timing dan critical sections

## Basic Usage

```cpp
#include "CeriaModuleESP32RTOS.h"
using namespace CeriaDevOP;

CeriaModuleESP32RTOS rtos;

void sensorTask(void *parameter) {
    while(1) {
        Serial.println("Sensor reading...");
        rtos.delay(1000);  // Clean delay wrapper
    }
}

void setup() {
    Serial.begin(115200);
    
    // Initialize RTOS wrapper
    rtos.begin(1000);  // Update interval 1000ms
    
    // Create task pinned to Core 0
    TaskHandle_t sensor = rtos.createTask(
        sensorTask,           // Task function
        "SensorTask",         // Task name
        2048,                 // Stack size (bytes)
        nullptr,              // Parameters
        RTOS_PRIORITY_NORMAL, // Priority
        RTOS_CORE_0           // Pin to Core 0
    );
    
    if (sensor != nullptr) {
        Serial.println("✅ Sensor task created successfully");
    }
}

void loop() {
    rtos.update();
    
    if (rtos.hasNewData()) {
        rtos.printSystemInfo();
        rtos.printMemoryInfo();
    }
    
    delay(100);
}
```

## Advanced Features

### Memory Monitoring dengan Callbacks
```cpp
void setup() {
    rtos.begin();
    
    // Enable memory monitoring
    rtos.enableMemoryMonitoring(true);
    rtos.setMemoryThreshold(10240);  // Alert ketika free heap < 10KB
    rtos.setFragmentationThreshold(80);  // Alert ketika fragmentation > 80%
    
    // Set callback untuk memory alerts
    rtos.setMemoryCallback([](uint32_t freeHeap, uint8_t fragmentation) {
        Serial.println("⚠️ Memory Alert: " + String(freeHeap) + " bytes free, " + 
                      String(fragmentation) + "% fragmented");
    });
    
    // Set callback untuk task state changes
    rtos.setTaskCallback([](const String &taskName, RTOSTaskState state) {
        Serial.println("📋 Task '" + taskName + "' state changed to: " + String(state));
    });
}
```

### Multiple Tasks dengan Different Priorities
```cpp
void criticalTask(void *parameter) {
    while(1) {
        Serial.println("🔴 Critical task running on Core " + String(rtos.getCurrentCore()));
        rtos.delay(500);
    }
}

void backgroundTask(void *parameter) {
    while(1) {
        Serial.println("🟢 Background task running");
        rtos.delay(2000);
    }
}

void setup() {
    rtos.begin();
    
    // Critical task - High priority, pinned to Core 1
    rtos.createTask(criticalTask, "Critical", 2048, nullptr, RTOS_PRIORITY_HIGH, RTOS_CORE_1);
    
    // Background task - Low priority, can float between cores
    rtos.createTask(backgroundTask, "Background", 1024, nullptr, RTOS_PRIORITY_LOW, RTOS_NO_AFFINITY);
    
    // Start automatic monitoring
    rtos.startMonitoring();
}
```

### Task Management dan Monitoring
```cpp
void loop() {
    rtos.update();
    
    // Get specific task information
    RTOSTaskInfo sensorInfo = rtos.getTaskInfo("SensorTask");
    if (sensorInfo.isActive) {
        Serial.println("Sensor task free stack: " + String(sensorInfo.freeStack) + " bytes");
    }
    
    // Check system health
    RTOSMemoryInfo memInfo = rtos.getMemoryInfo();
    if (memInfo.freeHeap < 5000) {
        Serial.println("⚠️ Low memory warning!");
    }
    
    // Suspend/Resume tasks dynamically
    static bool suspended = false;
    static unsigned long lastToggle = 0;
    if (millis() - lastToggle > 10000) {
        if (!suspended) {
            rtos.suspendTask("Background");
            Serial.println("Background task suspended");
            suspended = true;
        } else {
            rtos.resumeTask("Background");
            Serial.println("Background task resumed");
            suspended = false;
        }
        lastToggle = millis();
    }
}
```

## API Reference

### Core Methods
- `void begin(uint32_t updateInterval = 1000)` - Initialize wrapper
- `void update()` - Update task dan memory information
- `bool hasNewData()` - Check untuk new data availability

### Task Management
- `TaskHandle_t createTask(...)` - Create new task dengan options
- `bool deleteTask(TaskHandle_t/String)` - Delete task by handle atau name
- `bool suspendTask(TaskHandle_t/String)` - Suspend task execution
- `bool resumeTask(TaskHandle_t/String)` - Resume suspended task
- `RTOSTaskInfo getTaskInfo(TaskHandle_t/String)` - Get detailed task information

### Memory & System Information
- `RTOSMemoryInfo getMemoryInfo()` - Get current memory status
- `RTOSSystemInfo getSystemInfo()` - Get comprehensive system info
- `uint32_t getUptime()` - System uptime dalam milliseconds
- `String getVersionString()` - FreeRTOS version information

### Utility Functions
- `void delay(uint32_t ms)` - Task-safe delay
- `uint32_t getTickCount()` - Current system tick count
- `static BaseType_t getCurrentCore()` - Get current executing core
- `static void enterCritical() / exitCritical()` - Critical section management

### Monitoring & Callbacks
- `void enableMemoryMonitoring(bool enable)` - Enable/disable memory monitoring
- `void setMemoryCallback(RTOSMemoryCallback)` - Set memory alert callback
- `void setTaskCallback(RTOSTaskCallback)` - Set task state change callback
- `bool startMonitoring()` - Start automatic monitoring task
- `void stopMonitoring()` - Stop monitoring task

### Debug & Information
- `void printTaskList()` - Print all tasks information
- `void printMemoryInfo()` - Print memory usage
- `void printSystemInfo()` - Print system information
- `String getTaskListString()` - Get formatted task list
- `String getMemoryString()` - Get formatted memory info

## Configuration Constants

### Task Priorities
```cpp
RTOS_PRIORITY_IDLE = 0      // Idle priority
RTOS_PRIORITY_LOW = 1       // Low priority tasks
RTOS_PRIORITY_NORMAL = 5    // Normal priority (default)
RTOS_PRIORITY_HIGH = 10     // High priority tasks
RTOS_PRIORITY_CRITICAL = 20 // Critical tasks
RTOS_PRIORITY_MAXIMUM = 24  // Maximum priority
```

### Core Assignment
```cpp
RTOS_CORE_0 = 0           // Pin to Core 0 (Protocol CPU)
RTOS_CORE_1 = 1           // Pin to Core 1 (Application CPU)
RTOS_NO_AFFINITY          // Allow floating between cores
```

### Default Values
```cpp
RTOS_DEFAULT_STACK_SIZE = 2048    // Default stack size
RTOS_MEMORY_THRESHOLD = 10240     // Memory alert threshold
RTOS_FRAGMENTATION_THRESHOLD = 80 // Fragmentation alert threshold
```

## Memory Usage Guidelines
- **Sensor Tasks**: 1024-2048 bytes stack size
- **Communication Tasks**: 4096-8192 bytes stack size
- **Display Tasks**: 2048-4096 bytes stack size
- **Critical Tasks**: 1024-2048 bytes (minimal, focused functionality)

## Best Practices
1. **Always use delays dalam tasks** untuk prevent watchdog timeouts
2. **Monitor stack usage** dengan `getTaskFreeStack()` during development
3. **Pin critical tasks** ke specific cores untuk consistent timing
4. **Use callbacks** untuk proactive system health monitoring
5. **Enable memory monitoring** dalam production untuk early problem detection
6. **Batch similar operations** dalam single task untuk reduce overhead

## Integration dengan CeriaDev
Wrapper ini mengikuti design patterns CeriaDev:
- Inheritance-based design (extends functionality)
- Simple API surface dengan powerful capabilities
- Comprehensive error handling dan monitoring
- String output methods untuk human-readable information
- Event-driven architecture dengan callback support

## Platform Requirements
- ESP32 platform (ESP32, ESP32-S2, ESP32-S3, ESP32-C3)
- ESP-IDF atau Arduino Core untuk ESP32
- FreeRTOS support (included dengan ESP32 platforms)

## Version Compatibility

### ESP32 Arduino Core Support
CeriaModuleESP32RTOS mendukung multiple versi Arduino Core:
- **Arduino Core 2.0.x** - Full compatibility dengan legacy API
- **Arduino Core 3.0.x** - Full compatibility dengan modern API
- Automatic version detection dan conditional compilation

### Version Detection Example
```cpp
// Check Arduino Core version at runtime
Serial.println("Core Version: " + rtos.getCoreVersionString());
Serial.println("Version Code: " + String(rtos.getCoreVersion()));

#if CERIA_ESP32_CORE_VERSION >= CERIA_ESP32_CORE_3_0_X
  // Code untuk Core 3.0+
#else
  // Code untuk Core 2.0.x
#endif
```

### Compatibility Fixes Implemented
- **Enum Type Fix**: `RTOSTaskCore` menggunakan `int32_t` untuk support `tskNO_AFFINITY` value
- **Critical Section Fix**: Custom `portMUX_TYPE` variable untuk compatibility dengan semua versi
- **Version Detection**: Automatic detection dengan fallback untuk older cores

## Troubleshooting

### Common Issues dan Solutions

#### Error: "enumerator value is outside the range of underlying type"
**Solution**: Fixed dengan mengubah `RTOSTaskCore` enum dari `uint8_t` ke `int32_t`

#### Error: "'portMUX_TMP' was not declared in this scope"
**Solution**: Menggunakan custom static `_criticalMux` variable untuk critical sections

#### Error: "'ESP_ARDUINO_VERSION_STR' was not declared in this scope"
**Solution**: Manual string construction menggunakan individual version macros dengan fallback

### Quick Compatibility Test
```cpp
// Simple test untuk verify compatibility
#include "CeriaModuleESP32RTOS.h"
CeriaModuleESP32RTOS rtos;

void setup() {
    Serial.begin(115200);
    Serial.println("Core: " + rtos.getCoreVersionString());
    Serial.print(rtos.getCompatibilityString());
    
    // Test task creation
    rtos.createTask(testTask, "Test", 2048, nullptr, RTOS_PRIORITY_NORMAL, RTOS_CORE_0);
}
```

#### Compatibility Tests
- **`CeriaRTOSQuickTest.ino`** - Simple compatibility verification
- **`CeriaRTOSCompatibilityTest.ino`** - Comprehensive compatibility testing

CeriaModuleESP32RTOS menyediakan foundation yang solid untuk real-time embedded applications dengan focus pada simplicity, reliability, cross-version compatibility, dan comprehensive monitoring capabilities.