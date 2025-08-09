#include "../lib/CeriaModuleESP32RTOS.h"
using namespace CeriaDevOP;

CeriaModuleESP32RTOS rtos;

void quickTask(void *parameter) {
    while(1) {
        Serial.println("✅ Quick Task Running - Core " + String(rtos.getCurrentCore()));
        rtos.delay(2000);
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("=== CeriaModuleESP32RTOS Quick Test ===");
    Serial.println("Testing compatibility dengan Arduino Core 2.0.17");
    Serial.println();
    
    Serial.println("🔍 Compatibility Check:");
    Serial.print(rtos.getCompatibilityString());
    Serial.println();
    
    Serial.println("📊 Version Information:");
    Serial.println("  Core: " + rtos.getCoreVersionString());
    Serial.println("  FreeRTOS: " + rtos.getVersionString());
    Serial.println("  Version Code: " + String(rtos.getCoreVersion()));
    Serial.println();
    
    Serial.println("🧪 Testing Enum Values:");
    Serial.println("  RTOS_CORE_0: " + String(RTOS_CORE_0));
    Serial.println("  RTOS_CORE_1: " + String(RTOS_CORE_1));
    Serial.println("  RTOS_NO_AFFINITY: " + String(RTOS_NO_AFFINITY));
    Serial.println();
    
    rtos.begin();
    
    Serial.println("🚀 Creating Test Task...");
    TaskHandle_t task = rtos.createTask(
        quickTask,
        "QuickTask", 
        2048,
        nullptr,
        RTOS_PRIORITY_NORMAL,
        RTOS_CORE_1
    );
    
    if (task) {
        Serial.println("✅ Task created successfully!");
    } else {
        Serial.println("❌ Task creation failed!");
    }
    
    Serial.println();
    Serial.println("🧪 Testing Critical Sections...");
    rtos.enterCritical();
    Serial.println("✅ Entered critical section");
    delay(100);
    rtos.exitCritical();
    Serial.println("✅ Exited critical section");
    Serial.println();
    
    Serial.println("📋 System Ready:");
    Serial.println("  Tasks: " + String(rtos.getTaskCount()));
    Serial.println("  Uptime: " + String(rtos.getUptime()) + "ms");
    Serial.println("  Free Heap: " + String(rtos.getMemoryInfo().freeHeap) + " bytes");
    Serial.println();
    
    Serial.println("🎉 All tests passed! Running main loop...");
    Serial.println("=====================================");
}

void loop() {
    rtos.update();
    
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        Serial.println("📊 Status - Uptime: " + String(rtos.getUptime()/1000) + "s, " +
                      "Free: " + String(rtos.getMemoryInfo().freeHeap) + " bytes, " +
                      "Tasks: " + String(rtos.getTaskCount()));
        lastPrint = millis();
    }
    
    delay(100);
}