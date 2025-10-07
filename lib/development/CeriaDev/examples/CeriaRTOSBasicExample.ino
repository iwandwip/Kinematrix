#include "../lib/CeriaModuleESP32RTOS.h"
using namespace CeriaDevOP;

CeriaModuleESP32RTOS rtos;

void sensorTask(void *parameter) {
    uint32_t counter = 0;
    
    while(1) {
        Serial.println("📊 Sensor Task #" + String(counter++) + " running on Core " + String(rtos.getCurrentCore()));
        
        if (counter % 10 == 0) {
            Serial.println("Stack free: " + String(rtos.getTaskFreeStack("SensorTask")) + " bytes");
        }
        
        rtos.delay(1000);
    }
}

void displayTask(void *parameter) {
    uint32_t counter = 0;
    
    while(1) {
        Serial.println("🖥️ Display Task #" + String(counter++) + " running on Core " + String(rtos.getCurrentCore()));
        
        RTOSMemoryInfo memInfo = rtos.getMemoryInfo();
        if (counter % 5 == 0) {
            Serial.println("Memory: " + String(memInfo.freeHeap) + " free, " + String(memInfo.fragmentation) + "% fragmented");
        }
        
        rtos.delay(1500);
    }
}

void communicationTask(void *parameter) {
    uint32_t counter = 0;
    
    while(1) {
        Serial.println("📡 Communication Task #" + String(counter++) + " running on Core " + String(rtos.getCurrentCore()));
        
        if (counter % 20 == 0) {
            rtos.printSystemInfo();
            Serial.println("=== Task List ===");
            rtos.printTaskList();
            Serial.println("=================");
        }
        
        rtos.delay(2000);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== CeriaModuleESP32RTOS Basic Example ===");
    Serial.println("ESP32 Dual-Core RTOS Task Management Demo");
    Serial.println();
    
    rtos.begin(1000);
    
    rtos.enableMemoryMonitoring(true);
    rtos.setMemoryThreshold(20000);
    rtos.setFragmentationThreshold(75);
    
    rtos.setMemoryCallback([](uint32_t freeHeap, uint8_t fragmentation) {
        Serial.println("⚠️ MEMORY ALERT: " + String(freeHeap) + " bytes free, " + String(fragmentation) + "% fragmented");
    });
    
    rtos.setTaskCallback([](const String &taskName, RTOSTaskState state) {
        Serial.println("📋 Task '" + taskName + "' changed to state: " + String(state));
    });
    
    TaskHandle_t sensor = rtos.createTask(
        sensorTask,
        "SensorTask",
        2048,
        nullptr,
        RTOS_PRIORITY_HIGH,
        RTOS_CORE_0
    );
    
    TaskHandle_t display = rtos.createTask(
        displayTask,
        "DisplayTask",
        2048,
        nullptr,
        RTOS_PRIORITY_NORMAL,
        RTOS_CORE_1
    );
    
    TaskHandle_t comm = rtos.createTask(
        communicationTask,
        "CommTask",
        4096,
        nullptr,
        RTOS_PRIORITY_LOW,
        RTOS_NO_AFFINITY
    );
    
    Serial.println("✅ Tasks created:");
    Serial.println("  - SensorTask: Core 0, High Priority");
    Serial.println("  - DisplayTask: Core 1, Normal Priority");
    Serial.println("  - CommTask: No Affinity, Low Priority");
    Serial.println();
    
    if (!rtos.startMonitoring()) {
        Serial.println("❌ Failed to start monitoring task");
    } else {
        Serial.println("✅ RTOS monitoring started");
    }
    
    Serial.println("System ready. Tasks are running...");
    Serial.println("=========================================");
}

void loop() {
    rtos.update();
    
    if (rtos.hasNewData()) {
        static uint32_t debugCounter = 0;
        debugCounter++;
        
        if (debugCounter % 30 == 0) {
            Serial.println("\n--- System Status (Loop #" + String(debugCounter) + ") ---");
            Serial.println("Uptime: " + String(rtos.getUptime() / 1000) + " seconds");
            Serial.println("Total Tasks: " + String(rtos.getTaskCount()));
            Serial.println("Free Heap: " + String(rtos.getMemoryInfo().freeHeap) + " bytes");
            Serial.println("Current Core: " + String(rtos.getCurrentCore()));
            Serial.println("-------------------------------\n");
        }
    }
    
    static unsigned long lastTaskManagement = 0;
    if (millis() - lastTaskManagement > 15000) {
        static bool suspended = false;
        
        if (!suspended) {
            Serial.println("🔴 Suspending DisplayTask for 5 seconds...");
            rtos.suspendTask("DisplayTask");
            suspended = true;
        } else {
            Serial.println("🟢 Resuming DisplayTask...");
            rtos.resumeTask("DisplayTask");
            suspended = false;
        }
        
        lastTaskManagement = millis();
    }
    
    delay(100);
}