#include "../lib/CeriaModuleESP32RTOS.h"
using namespace CeriaDevOP;

CeriaModuleESP32RTOS rtos;

void testTask(void *parameter) {
    uint32_t counter = 0;
    
    while(1) {
        Serial.println("🧪 Test Task #" + String(counter++) + " - Core " + String(rtos.getCurrentCore()));
        
        if (counter % 10 == 0) {
            Serial.println("📊 Critical section test...");
            
            rtos.enterCritical();
            Serial.println("Inside critical section");
            delay(10);
            rtos.exitCritical();
            
            Serial.println("✅ Critical section test completed");
        }
        
        rtos.delay(2000);
    }
}

void compatibilityTask(void *parameter) {
    uint32_t counter = 0;
    
    while(1) {
        Serial.println("🔧 Compatibility Task #" + String(counter++));
        
        if (counter % 5 == 0) {
            Serial.println("📋 Task pinning test: Core " + String(rtos.getCurrentCore()));
            Serial.println("Stack free: " + String(rtos.getTaskFreeStack("CompatTask")) + " bytes");
            
            RTOSMemoryInfo memInfo = rtos.getMemoryInfo();
            Serial.println("Memory: " + String(memInfo.freeHeap) + " bytes free");
        }
        
        rtos.delay(3000);
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== CeriaModuleESP32RTOS Compatibility Test ===");
    Serial.println("Testing compatibility dengan ESP32 Arduino Core 2.0.17");
    Serial.println();
    
    rtos.begin(1000);
    
    Serial.println("🔍 System Information:");
    Serial.println("  Arduino Core: " + rtos.getCoreVersionString());
    Serial.println("  FreeRTOS: " + rtos.getVersionString());
    Serial.println("  Core Version Code: " + String(rtos.getCoreVersion()));
    Serial.println();
    
    #if CERIA_ESP32_CORE_VERSION >= CERIA_ESP32_CORE_3_0_X
        Serial.println("✅ Detected Arduino Core 3.0+ - Using modern API");
    #else
        Serial.println("✅ Detected Arduino Core 2.0.x - Using legacy API");
    #endif
    Serial.println();
    
    Serial.println("🧪 Testing enum compatibility:");
    Serial.println("  RTOS_CORE_0: " + String(RTOS_CORE_0));
    Serial.println("  RTOS_CORE_1: " + String(RTOS_CORE_1));
    Serial.println("  RTOS_NO_AFFINITY: " + String(RTOS_NO_AFFINITY));
    Serial.println();
    
    rtos.setMemoryCallback([](uint32_t freeHeap, uint8_t fragmentation) {
        Serial.println("⚠️ MEMORY ALERT: " + String(freeHeap) + " bytes, " + 
                      String(fragmentation) + "% fragmented");
    });
    
    rtos.setTaskCallback([](const String &taskName, RTOSTaskState state) {
        Serial.println("📋 Task '" + taskName + "' state: " + String(state));
    });
    
    TaskHandle_t test = rtos.createTask(
        testTask,
        "TestTask",
        2048,
        nullptr,
        RTOS_PRIORITY_HIGH,
        RTOS_CORE_0
    );
    
    TaskHandle_t compat = rtos.createTask(
        compatibilityTask,
        "CompatTask",
        2048,
        nullptr,
        RTOS_PRIORITY_NORMAL,
        RTOS_CORE_1
    );
    
    TaskHandle_t floating = rtos.createTask(
        [](void *param) {
            uint32_t counter = 0;
            while(1) {
                Serial.println("🌊 Floating Task #" + String(counter++) + 
                              " - Core " + String(CeriaModuleESP32RTOS::getCurrentCore()));
                CeriaModuleESP32RTOS::taskDelay(4000);
            }
        },
        "FloatingTask",
        1024,
        nullptr,
        RTOS_PRIORITY_LOW,
        RTOS_NO_AFFINITY
    );
    
    if (test && compat && floating) {
        Serial.println("✅ All tasks created successfully:");
        Serial.println("  - TestTask: Core 0, High Priority");
        Serial.println("  - CompatTask: Core 1, Normal Priority");
        Serial.println("  - FloatingTask: No Affinity, Low Priority");
    } else {
        Serial.println("❌ Task creation failed");
    }
    
    rtos.enableMemoryMonitoring(true);
    rtos.setMemoryThreshold(15000);
    
    if (!rtos.startMonitoring()) {
        Serial.println("❌ Failed to start monitoring");
    } else {
        Serial.println("✅ Monitoring started");
    }
    
    Serial.println();
    Serial.println("🚀 Compatibility test running...");
    Serial.println("==========================================");
}

void loop() {
    rtos.update();
    
    if (rtos.hasNewData()) {
        static uint32_t loopCounter = 0;
        loopCounter++;
        
        if (loopCounter % 20 == 0) {
            Serial.println();
            Serial.println("--- Compatibility Status (Loop #" + String(loopCounter) + ") ---");
            rtos.printSystemInfo();
            Serial.println();
            
            Serial.println("📋 Active Tasks:");
            rtos.printTaskList();
            Serial.println();
            
            Serial.println("💾 Memory Status:");
            rtos.printMemoryInfo();
            Serial.println("================================================");
        }
    }
    
    static unsigned long lastTest = 0;
    if (millis() - lastTest > 25000) {
        Serial.println();
        Serial.println("🔄 Testing task suspend/resume...");
        
        if (rtos.isTaskActive("CompatTask")) {
            rtos.suspendTask("CompatTask");
            Serial.println("🔴 CompatTask suspended");
        } else {
            rtos.resumeTask("CompatTask");
            Serial.println("🟢 CompatTask resumed");
        }
        
        lastTest = millis();
    }
    
    delay(100);
}