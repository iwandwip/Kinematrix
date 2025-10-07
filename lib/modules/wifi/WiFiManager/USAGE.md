# WiFiManager Usage Examples

This document provides comprehensive usage examples for the WiFiManager library, from simple implementations to complex IoT projects.

## Table of Contents

1. [Basic Usage](#basic-usage)
2. [Configuration Examples](#configuration-examples) 
3. [Callback Usage](#callback-usage)
4. [Display Integration](#display-integration)
5. [Advanced Features](#advanced-features)
6. [IoT Project Examples](#iot-project-examples)
7. [Platform-Specific Examples](#platform-specific-examples)

---

## Basic Usage

### 1. Minimal Setup (Auto-Connect)
The simplest possible implementation - just connect to WiFi automatically.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    wm.begin();
    wm.autoConnect();
}

void loop() {
    wm.loop();
}
```

### 2. Basic with Serial Output
Add basic status reporting to Serial monitor.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    Serial.println("WiFiManager Basic Example");
    
    if (!wm.begin()) {
        Serial.println("Failed to initialize WiFiManager");
        return;
    }
    
    Serial.println("Starting auto-connect...");
    if (wm.autoConnect()) {
        Serial.println("Connected with saved credentials");
    } else {
        Serial.println("Starting config mode");
    }
}

void loop() {
    wm.loop();
    
    // Show status every 30 seconds
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 30000) {
        if (wm.isConnected()) {
            Serial.print("Connected to: ");
            Serial.print(wm.getSSID());
            Serial.print(" | IP: ");
            Serial.println(wm.getIP());
        } else {
            Serial.println("Not connected");
        }
        lastCheck = millis();
    }
}
```

### 3. Custom Access Point Name
Set a custom name for the configuration access point.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    
    // Set custom AP name
    wm.setAPCredentials("MyDevice-Setup");
    
    wm.begin();
    wm.autoConnect();
    
    Serial.println("Connect to 'MyDevice-Setup' AP if in config mode");
    Serial.println("Then go to http://192.168.4.1");
}

void loop() {
    wm.loop();
}
```

---

## Configuration Examples

### 4. Custom Timeouts
Configure connection and retry timeouts for different scenarios.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    
    // Fast timeout for quick fallback to config mode
    wm.setTimeout(10000);      // 10 seconds connection timeout
    wm.setRetryDelay(2000);    // 2 seconds retry delay
    wm.setConfigTimeout(180000); // 3 minutes config timeout
    
    wm.begin();
    wm.autoConnect();
    
    Serial.print("Connection timeout: ");
    Serial.print(wm.getConnectionTimeoutS());
    Serial.println(" seconds");
}

void loop() {
    wm.loop();
}
```

### 5. Signal Quality Filtering
Only show networks with strong signal strength.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    
    // Only show networks with signal > -70 dBm (strong signal)
    wm.setMinSignalQuality(-70);
    
    // Set custom device identity
    wm.setAPCredentials("StrongSignal-WiFi", "password123");
    wm.setHostname("MyStrongDevice");
    
    wm.begin();
    wm.autoConnect();
    
    Serial.print("Minimum signal quality: ");
    Serial.print(wm.getMinSignalQuality());
    Serial.println(" dBm");
}

void loop() {
    wm.loop();
}
```

### 6. mDNS Configuration
Access the web interface via friendly hostname even when connected.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    
    // Configure mDNS for easy access
    wm.setMDNSName("mydevice");        // Access via mydevice.local
    wm.enableMDNS(true);
    
    wm.begin();
    wm.autoConnect();
    
    Serial.println("Device accessible via:");
    Serial.println("- AP Mode: http://192.168.4.1");
    Serial.print("- mDNS: http://");
    Serial.print(wm.getMDNSName());
    Serial.println(".local");
}

void loop() {
    wm.loop();
}
```

---

## Callback Usage

### 7. Connection Event Callbacks
Handle WiFi connection events with custom functions.

```cpp
#include "WiFiManager.h"

WiFiManager wm;
bool ledState = false;

void onWiFiConnected() {
    Serial.println("🎉 WiFi Connected!");
    Serial.print("Network: ");
    Serial.println(wm.getSSID());
    Serial.print("IP Address: ");
    Serial.println(wm.getIP());
    Serial.print("Signal Quality: ");
    Serial.print(wm.getSignalQuality());
    Serial.println("%");
    
    // Turn on LED to indicate connection
    digitalWrite(LED_BUILTIN, HIGH);
}

void onWiFiDisconnected() {
    Serial.println("❌ WiFi Disconnected!");
    digitalWrite(LED_BUILTIN, LOW);
}

void onConfigMode() {
    Serial.println("⚙️  Config Mode Started");
    Serial.println("Connect to AP and configure WiFi");
    
    // Blink LED in config mode
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    
    // Set callbacks
    wm.onConnected(onWiFiConnected);
    wm.onDisconnected(onWiFiDisconnected);
    wm.onConfigMode(onConfigMode);
    
    wm.begin();
    wm.autoConnect();
}

void loop() {
    wm.loop();
    
    // Blink LED in config mode
    if (wm.isConfigMode()) {
        static unsigned long lastBlink = 0;
        if (millis() - lastBlink > 500) {
            ledState = !ledState;
            digitalWrite(LED_BUILTIN, ledState);
            lastBlink = millis();
        }
    }
}
```

### 8. Status Monitoring with Callbacks
Advanced status monitoring and automatic actions.

```cpp
#include "WiFiManager.h"

WiFiManager wm;
unsigned long connectionStartTime = 0;
unsigned long totalUptime = 0;
unsigned long lastConnectionTime = 0;

void onWiFiConnected() {
    lastConnectionTime = millis();
    unsigned long connectionDuration = lastConnectionTime - connectionStartTime;
    
    Serial.println("=== WiFi Connected ===");
    Serial.print("Connection took: ");
    Serial.print(connectionDuration);
    Serial.println(" ms");
    
    // Log connection success
    totalUptime = 0; // Reset uptime counter
}

void onWiFiDisconnected() {
    Serial.println("=== WiFi Disconnected ===");
    Serial.print("Was connected for: ");
    Serial.print((millis() - lastConnectionTime) / 1000);
    Serial.println(" seconds");
    
    connectionStartTime = millis(); // Start timing reconnection
}

void setup() {
    Serial.begin(115200);
    
    wm.onConnected(onWiFiConnected);
    wm.onDisconnected(onWiFiDisconnected);
    
    connectionStartTime = millis();
    wm.begin();
    wm.autoConnect();
}

void loop() {
    wm.loop();
    
    // Update uptime counter
    if (wm.isConnected()) {
        totalUptime = millis() - lastConnectionTime;
        
        // Show periodic status
        static unsigned long lastReport = 0;
        if (millis() - lastReport > 60000) { // Every minute
            Serial.print("Uptime: ");
            Serial.print(totalUptime / 1000);
            Serial.print("s | Signal: ");
            Serial.print(wm.getSignalQuality());
            Serial.println("%");
            lastReport = millis();
        }
    }
}
```

---

## Display Integration

### 9. LCD 16x2 Integration
Show WiFi status and connection info on LCD display.

```cpp
#include "WiFiManager.h"
#include <LiquidCrystal.h>

WiFiManager wm;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void updateLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    switch (wm.getState()) {
        case WM_IDLE:
            lcd.print("WiFi: Idle");
            break;
            
        case WM_CONNECTING:
            lcd.print("Connecting...");
            lcd.setCursor(0, 1);
            lcd.print("Timeout: ");
            lcd.print(wm.getRemainingTimeoutS());
            lcd.print("s");
            break;
            
        case WM_CONNECTED:
            lcd.print("Connected");
            lcd.setCursor(0, 1);
            lcd.print(wm.getIP());
            break;
            
        case WM_CONFIG_MODE:
            lcd.print("Config Mode");
            lcd.setCursor(0, 1);
            lcd.print("192.168.4.1");
            break;
            
        case WM_RETRY:
            lcd.print("Retrying...");
            lcd.setCursor(0, 1);
            lcd.print("Wait: ");
            lcd.print(wm.getRemainingTimeoutS());
            lcd.print("s");
            break;
    }
}

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);
    
    lcd.print("WiFi Starting...");
    
    wm.begin();
    wm.autoConnect();
}

void loop() {
    wm.loop();
    
    // Update LCD every second
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {
        updateLCD();
        lastUpdate = millis();
    }
}
```

### 10. OLED Display with Graphics
Advanced OLED display showing connection progress and network info.

```cpp
#include "WiFiManager.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

WiFiManager wm;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void drawWiFiIcon(int x, int y, int signalQuality) {
    // Draw WiFi signal bars based on quality
    int bars = map(signalQuality, 0, 100, 1, 4);
    
    for (int i = 0; i < 4; i++) {
        if (i < bars) {
            display.fillRect(x + i * 3, y - i * 2, 2, i * 2 + 2, WHITE);
        } else {
            display.drawRect(x + i * 3, y - i * 2, 2, i * 2 + 2, WHITE);
        }
    }
}

void updateOLED() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    
    // Title
    display.setCursor(0, 0);
    display.println("WiFi Manager");
    display.drawLine(0, 10, 128, 10, WHITE);
    
    // Status
    display.setCursor(0, 15);
    WiFiManagerState state = wm.getState();
    
    switch (state) {
        case WM_CONNECTED:
            display.print("Connected to:");
            display.setCursor(0, 25);
            display.println(wm.getSSID());
            display.setCursor(0, 35);
            display.print("IP: ");
            display.println(wm.getIP());
            display.setCursor(0, 45);
            display.print("Signal: ");
            display.print(wm.getSignalQuality());
            display.println("%");
            
            // Draw WiFi icon
            drawWiFiIcon(100, 50, wm.getSignalQuality());
            break;
            
        case WM_CONNECTING:
            display.print("Connecting...");
            display.setCursor(0, 25);
            display.print("Timeout: ");
            display.print(wm.getConnectionTimeoutS());
            display.println("s");
            display.setCursor(0, 35);
            display.print("Remaining: ");
            display.print(wm.getRemainingTimeoutS());
            display.println("s");
            
            // Progress bar
            int progress = map(wm.getStateTimerS(), 0, wm.getConnectionTimeoutS(), 0, 100);
            display.drawRect(0, 50, 100, 8, WHITE);
            display.fillRect(0, 50, progress, 8, WHITE);
            break;
            
        case WM_CONFIG_MODE:
            display.print("Config Mode");
            display.setCursor(0, 25);
            display.println("Connect to:");
            display.setCursor(0, 35);
            display.println("ESP32-WiFiManager");
            display.setCursor(0, 45);
            display.println("192.168.4.1");
            break;
            
        case WM_RETRY:
            display.print("Retry in:");
            display.setCursor(0, 25);
            display.print(wm.getRemainingTimeoutS());
            display.println(" seconds");
            break;
            
        default:
            display.print("Initializing...");
            break;
    }
    
    display.display();
}

void setup() {
    Serial.begin(115200);
    
    // Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED init failed");
        return;
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Starting WiFi...");
    display.display();
    
    wm.begin();
    wm.autoConnect();
}

void loop() {
    wm.loop();
    
    // Update OLED every 500ms
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 500) {
        updateOLED();
        lastUpdate = millis();
    }
}
```

## Advanced Features

### 11. Manual Control Interface
Full manual control via Serial commands for development and testing.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void printCommands() {
    Serial.println("\n=== WiFiManager Manual Control ===");
    Serial.println("Commands:");
    Serial.println("  c - Start config mode");
    Serial.println("  s - Stop config mode");
    Serial.println("  r - Reset WiFi config");
    Serial.println("  R - Reset all + restart");
    Serial.println("  i - Show device info");
    Serial.println("  t - Show timeouts");
    Serial.println("  m - Toggle mDNS");
    Serial.println("  h - Show this help");
    Serial.println("================================\n");
}

void showDeviceInfo() {
    Serial.println("\n=== Device Information ===");
    Serial.print("State: ");
    Serial.println(wm.getState());
    Serial.print("Connected: ");
    Serial.println(wm.isConnected() ? "Yes" : "No");
    
    if (wm.isConnected()) {
        Serial.print("SSID: ");
        Serial.println(wm.getSSID());
        Serial.print("IP: ");
        Serial.println(wm.getIP());
        Serial.print("Signal: ");
        Serial.print(wm.getSignalQuality());
        Serial.println("% (");
        Serial.print(wm.getRSSI());
        Serial.println(" dBm)");
    }
    
    Serial.print("Config Mode: ");
    Serial.println(wm.isConfigMode() ? "Active" : "Inactive");
    Serial.print("Auto Reconnect: ");
    Serial.println(wm.isAutoReconnectEnabled() ? "Enabled" : "Disabled");
    Serial.print("mDNS: ");
    Serial.println(wm.isMDNSStarted() ? "Running" : "Stopped");
    
    if (wm.isMDNSEnabled()) {
        Serial.print("  Hostname: ");
        Serial.print(wm.getMDNSName());
        Serial.println(".local");
    }
    Serial.println("==========================\n");
}

void showTimeouts() {
    Serial.println("\n=== Timeout Configuration ===");
    Serial.print("Connection Timeout: ");
    Serial.print(wm.getConnectionTimeoutS());
    Serial.println(" seconds");
    Serial.print("Config Timeout: ");
    Serial.print(wm.getConfigTimeoutS());
    Serial.println(" seconds");
    Serial.print("Retry Delay: ");
    Serial.print(wm.getRetryDelayS());
    Serial.println(" seconds");
    Serial.print("Reconnect Interval: ");
    Serial.print(wm.getReconnectIntervalS());
    Serial.println(" seconds");
    Serial.print("Min Signal Quality: ");
    Serial.print(wm.getMinSignalQuality());
    Serial.println(" dBm");
    
    // Show current state timing
    Serial.print("Current State Timer: ");
    Serial.print(wm.getStateTimerS());
    Serial.println(" seconds");
    
    unsigned long remaining = wm.getRemainingTimeoutS();
    if (remaining > 0) {
        Serial.print("Remaining Timeout: ");
        Serial.print(remaining);
        Serial.println(" seconds");
    }
    Serial.println("=============================\n");
}

void setup() {
    Serial.begin(115200);
    
    wm.setAPCredentials("ManualControl-WiFi");
    wm.begin();
    wm.autoConnect();
    
    printCommands();
}

void loop() {
    wm.loop();
    
    if (Serial.available()) {
        char cmd = Serial.read();
        Serial.read(); // consume newline
        
        switch (cmd) {
            case 'c':
                Serial.println("Starting config mode...");
                wm.startConfig();
                break;
                
            case 's':
                Serial.println("Stopping config mode...");
                wm.stopConfig();
                break;
                
            case 'r':
                Serial.println("Resetting WiFi configuration...");
                wm.reset();
                break;
                
            case 'R':
                Serial.println("Complete system reset in 3 seconds...");
                delay(3000);
                wm.resetAll();
                break;
                
            case 'i':
                showDeviceInfo();
                break;
                
            case 't':
                showTimeouts();
                break;
                
            case 'm':
                if (wm.isMDNSEnabled()) {
                    Serial.println("Disabling mDNS...");
                    wm.enableMDNS(false);
                } else {
                    Serial.println("Enabling mDNS...");
                    wm.enableMDNS(true);
                }
                break;
                
            case 'h':
                printCommands();
                break;
                
            default:
                Serial.println("Unknown command. Type 'h' for help.");
                break;
        }
    }
}
```

### 12. Button Reset with Different Actions
Implement different reset actions based on button press duration.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

#define RESET_BUTTON_PIN 0  // Boot button
#define STATUS_LED_PIN 2    // Built-in LED

unsigned long buttonPressTime = 0;
bool buttonPressed = false;
bool actionExecuted = false;

void setup() {
    Serial.begin(115200);
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    pinMode(STATUS_LED_PIN, OUTPUT);
    
    wm.setAPCredentials("ButtonControl-WiFi");
    wm.begin();
    wm.autoConnect();
    
    Serial.println("Button Control:");
    Serial.println("- Short press (1-3s): Show status");
    Serial.println("- Medium press (3-7s): Reset WiFi config");
    Serial.println("- Long press (7s+): Complete reset");
}

void blinkLED(int times, int delayMs) {
    for (int i = 0; i < times; i++) {
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(delayMs);
        digitalWrite(STATUS_LED_PIN, LOW);
        delay(delayMs);
    }
}

void showStatus() {
    Serial.println("\n=== Quick Status ===");
    if (wm.isConnected()) {
        Serial.print("✓ Connected to: ");
        Serial.println(wm.getSSID());
    } else if (wm.isConfigMode()) {
        Serial.println("⚙️ Config mode active");
    } else {
        Serial.println("❌ Not connected");
    }
    Serial.println("===================\n");
    blinkLED(2, 200); // 2 quick blinks
}

void resetWiFiConfig() {
    Serial.println("🔄 Resetting WiFi configuration...");
    blinkLED(3, 300); // 3 medium blinks
    wm.reset();
    delay(1000);
    ESP.restart();
}

void completeReset() {
    Serial.println("💣 Complete system reset...");
    blinkLED(5, 100); // 5 fast blinks
    wm.resetAll();
}

void loop() {
    wm.loop();
    
    bool currentButtonState = (digitalRead(RESET_BUTTON_PIN) == LOW);
    
    if (currentButtonState && !buttonPressed) {
        // Button just pressed
        buttonPressed = true;
        buttonPressTime = millis();
        actionExecuted = false;
        Serial.println("Button pressed...");
        digitalWrite(STATUS_LED_PIN, HIGH);
        
    } else if (!currentButtonState && buttonPressed) {
        // Button just released
        buttonPressed = false;
        digitalWrite(STATUS_LED_PIN, LOW);
        
        if (!actionExecuted) {
            unsigned long pressDuration = millis() - buttonPressTime;
            
            if (pressDuration < 3000) {
                showStatus();
            } else if (pressDuration < 7000) {
                resetWiFiConfig();
            } else {
                completeReset();
            }
        }
        
    } else if (buttonPressed && !actionExecuted) {
        // Button currently held - check duration
        unsigned long currentDuration = millis() - buttonPressTime;
        
        if (currentDuration >= 7000) {
            Serial.println("Long press - Complete reset!");
            completeReset();
            actionExecuted = true;
        } else if (currentDuration >= 3000) {
            Serial.println("Medium press - WiFi reset!");
            resetWiFiConfig();
            actionExecuted = true;
        } else if (currentDuration >= 1000) {
            Serial.print("Hold for: ");
            if (currentDuration < 3000) {
                Serial.println("WiFi reset (3s) or Complete reset (7s)");
            } else if (currentDuration < 7000) {
                Serial.println("Complete reset (7s)");
            }
        }
    }
}
```

### 13. Configuration Backup and Restore
Save and restore WiFi configurations to/from SPIFFS/LittleFS.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void saveBackupConfig() {
    if (!LittleFS.exists("/backup")) {
        LittleFS.mkdir("/backup");
    }
    
    // Copy current config to backup
    if (LittleFS.exists("/config.json")) {
        File src = LittleFS.open("/config.json", "r");
        File dst = LittleFS.open("/backup/config_backup.json", "w");
        
        if (src && dst) {
            while (src.available()) {
                dst.write(src.read());
            }
            Serial.println("✓ Configuration backed up");
        }
        
        src.close();
        dst.close();
    }
    
    // Backup settings too
    if (LittleFS.exists("/settings.json")) {
        File src = LittleFS.open("/settings.json", "r");
        File dst = LittleFS.open("/backup/settings_backup.json", "w");
        
        if (src && dst) {
            while (src.available()) {
                dst.write(src.read());
            }
            Serial.println("✓ Settings backed up");
        }
        
        src.close();
        dst.close();
    }
}

void restoreBackupConfig() {
    Serial.println("Restoring configuration from backup...");
    
    // Restore config
    if (LittleFS.exists("/backup/config_backup.json")) {
        File src = LittleFS.open("/backup/config_backup.json", "r");
        File dst = LittleFS.open("/config.json", "w");
        
        if (src && dst) {
            while (src.available()) {
                dst.write(src.read());
            }
            Serial.println("✓ Configuration restored");
        }
        
        src.close();
        dst.close();
    }
    
    // Restore settings
    if (LittleFS.exists("/backup/settings_backup.json")) {
        File src = LittleFS.open("/backup/settings_backup.json", "r");
        File dst = LittleFS.open("/settings.json", "w");
        
        if (src && dst) {
            while (src.available()) {
                dst.write(src.read());
            }
            Serial.println("✓ Settings restored");
        }
        
        src.close();
        dst.close();
    }
    
    Serial.println("Restarting to apply restored configuration...");
    delay(2000);
    ESP.restart();
}

void listBackups() {
    Serial.println("\n=== Available Backups ===");
    
    if (LittleFS.exists("/backup/config_backup.json")) {
        File f = LittleFS.open("/backup/config_backup.json", "r");
        Serial.print("✓ Configuration backup (");
        Serial.print(f.size());
        Serial.println(" bytes)");
        f.close();
    } else {
        Serial.println("❌ No configuration backup");
    }
    
    if (LittleFS.exists("/backup/settings_backup.json")) {
        File f = LittleFS.open("/backup/settings_backup.json", "r");
        Serial.print("✓ Settings backup (");
        Serial.print(f.size());
        Serial.println(" bytes)");
        f.close();
    } else {
        Serial.println("❌ No settings backup");
    }
    
    Serial.println("========================\n");
}

void setup() {
    Serial.begin(115200);
    
    wm.begin();
    
    Serial.println("Configuration Management Example");
    Serial.println("Commands:");
    Serial.println("  b - Backup current configuration");
    Serial.println("  r - Restore from backup");
    Serial.println("  l - List available backups");
    Serial.println("  d - Delete backups");
    
    wm.autoConnect();
}

void loop() {
    wm.loop();
    
    if (Serial.available()) {
        char cmd = Serial.read();
        Serial.read(); // consume newline
        
        switch (cmd) {
            case 'b':
                saveBackupConfig();
                break;
                
            case 'r':
                restoreBackupConfig();
                break;
                
            case 'l':
                listBackups();
                break;
                
            case 'd':
                Serial.println("Deleting backups...");
                LittleFS.remove("/backup/config_backup.json");
                LittleFS.remove("/backup/settings_backup.json");
                LittleFS.rmdir("/backup");
                Serial.println("✓ Backups deleted");
                break;
        }
    }
}
```

---

## IoT Project Examples

### 14. Environmental Monitoring Station
Complete IoT project with sensors, WiFi management, and cloud connectivity.

```cpp
#include "WiFiManager.h"
#include <DHT.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

WiFiManager wm;
DHT dht(4, DHT22);
HTTPClient http;

struct SensorData {
    float temperature;
    float humidity;
    int rssi;
    unsigned long timestamp;
};

SensorData currentReading;

void onWiFiConnected() {
    Serial.println("📡 WiFi Connected - Starting monitoring");
    Serial.print("Access dashboard: http://");
    Serial.print(wm.getMDNSName());
    Serial.println(".local");
    
    // Initialize sensors
    dht.begin();
}

void onWiFiDisconnected() {
    Serial.println("📡 WiFi Lost - Monitoring offline");
}

void readSensors() {
    currentReading.temperature = dht.readTemperature();
    currentReading.humidity = dht.readHumidity();
    currentReading.rssi = wm.getRSSI();
    currentReading.timestamp = millis();
    
    // Validate readings
    if (isnan(currentReading.temperature) || isnan(currentReading.humidity)) {
        Serial.println("❌ Sensor reading failed");
        return;
    }
    
    Serial.println("📊 Sensor Reading:");
    Serial.print("  Temperature: ");
    Serial.print(currentReading.temperature, 1);
    Serial.println("°C");
    Serial.print("  Humidity: ");
    Serial.print(currentReading.humidity, 1);
    Serial.println("%");
    Serial.print("  WiFi Signal: ");
    Serial.print(wm.getSignalQuality());
    Serial.println("%");
}

void sendToCloud() {
    if (!wm.isConnected()) {
        Serial.println("⚠️ No WiFi - skipping cloud upload");
        return;
    }
    
    // Create JSON payload
    JsonDocument doc;
    doc["device_id"] = "env_monitor_01";
    doc["temperature"] = currentReading.temperature;
    doc["humidity"] = currentReading.humidity;
    doc["wifi_signal"] = wm.getSignalQuality();
    doc["timestamp"] = currentReading.timestamp;
    
    String payload;
    serializeJson(doc, payload);
    
    // Send to cloud (replace with your endpoint)
    http.begin("https://api.thingspeak.com/update");
    http.addHeader("Content-Type", "application/json");
    
    int responseCode = http.POST(payload);
    
    if (responseCode > 0) {
        Serial.print("☁️ Cloud upload: ");
        Serial.println(responseCode == 200 ? "Success" : "Failed");
    } else {
        Serial.println("❌ Cloud connection failed");
    }
    
    http.end();
}

void setup() {
    Serial.begin(115200);
    
    // Configure WiFi Manager
    wm.setAPCredentials("EnvMonitor-Setup");
    wm.setMDNSName("envmonitor");
    wm.setTimeout(20000);        // 20 second timeout
    wm.setRetryDelay(5000);      // 5 second retry
    
    wm.onConnected(onWiFiConnected);
    wm.onDisconnected(onWiFiDisconnected);
    
    wm.begin();
    wm.autoConnect();
    
    Serial.println("🌡️ Environmental Monitor Started");
    Serial.println("Features:");
    Serial.println("- Temperature & Humidity monitoring");
    Serial.println("- Cloud data upload");
    Serial.println("- Web dashboard via mDNS");
}

void loop() {
    wm.loop();
    
    static unsigned long lastReading = 0;
    static unsigned long lastUpload = 0;
    
    // Read sensors every 10 seconds
    if (millis() - lastReading > 10000) {
        readSensors();
        lastReading = millis();
    }
    
    // Upload to cloud every 60 seconds
    if (millis() - lastUpload > 60000) {
        sendToCloud();
        lastUpload = millis();
    }
    
    // Blink LED based on connection status
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    
    int blinkInterval = wm.isConnected() ? 2000 : 500;
    
    if (millis() - lastBlink > blinkInterval) {
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
        lastBlink = millis();
    }
}
```

### 15. Smart Home Device with MQTT
Smart device that connects to Home Assistant via MQTT with automatic discovery.

```cpp
#include "WiFiManager.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiManager wm;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

const char* MQTT_SERVER = "homeassistant.local";
const int MQTT_PORT = 1883;
const char* DEVICE_NAME = "smart_switch_01";

bool relayState = false;
const int RELAY_PIN = 2;
const int BUTTON_PIN = 0;

unsigned long lastButtonPress = 0;
bool lastButtonState = HIGH;

void onWiFiConnected() {
    Serial.println("WiFi Connected - Connecting to MQTT...");
    connectToMQTT();
}

void onWiFiDisconnected() {
    Serial.println("WiFi Disconnected - MQTT offline");
}

void connectToMQTT() {
    mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    mqtt.setCallback(mqttCallback);
    
    while (!mqtt.connected() && wm.isConnected()) {
        Serial.print("Connecting to MQTT...");
        
        if (mqtt.connect(DEVICE_NAME)) {
            Serial.println(" Connected!");
            
            // Subscribe to command topic
            String commandTopic = String("homeassistant/switch/") + DEVICE_NAME + "/set";
            mqtt.subscribe(commandTopic.c_str());
            
            // Send discovery message to Home Assistant
            sendMQTTDiscovery();
            
            // Send initial state
            publishState();
            
        } else {
            Serial.print(" Failed, rc=");
            Serial.println(mqtt.state());
            delay(5000);
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    
    Serial.print("MQTT command received: ");
    Serial.println(message);
    
    if (message == "ON") {
        relayState = true;
        digitalWrite(RELAY_PIN, HIGH);
    } else if (message == "OFF") {
        relayState = false;
        digitalWrite(RELAY_PIN, LOW);
    }
    
    publishState();
}

void sendMQTTDiscovery() {
    // Home Assistant MQTT Discovery
    String discoveryTopic = String("homeassistant/switch/") + DEVICE_NAME + "/config";
    
    JsonDocument doc;
    doc["name"] = "Smart Switch 01";
    doc["unique_id"] = DEVICE_NAME;
    doc["command_topic"] = String("homeassistant/switch/") + DEVICE_NAME + "/set";
    doc["state_topic"] = String("homeassistant/switch/") + DEVICE_NAME + "/state";
    doc["payload_on"] = "ON";
    doc["payload_off"] = "OFF";
    doc["state_on"] = "ON";
    doc["state_off"] = "OFF";
    
    // Device info
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = DEVICE_NAME;
    device["name"] = "Smart Switch";
    device["model"] = "ESP32 Switch v1.0";
    device["manufacturer"] = "DIY";
    device["sw_version"] = "1.0.0";
    
    String payload;
    serializeJson(doc, payload);
    
    mqtt.publish(discoveryTopic.c_str(), payload.c_str(), true);
    Serial.println("MQTT Discovery sent");
}

void publishState() {
    String stateTopic = String("homeassistant/switch/") + DEVICE_NAME + "/state";
    String state = relayState ? "ON" : "OFF";
    
    mqtt.publish(stateTopic.c_str(), state.c_str(), true);
    Serial.print("State published: ");
    Serial.println(state);
}

void handleButton() {
    bool currentButtonState = digitalRead(BUTTON_PIN);
    
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        if (millis() - lastButtonPress > 50) { // Debounce
            relayState = !relayState;
            digitalWrite(RELAY_PIN, relayState);
            publishState();
            
            Serial.print("Button pressed - Relay: ");
            Serial.println(relayState ? "ON" : "OFF");
        }
        lastButtonPress = millis();
    }
    
    lastButtonState = currentButtonState;
}

void setup() {
    Serial.begin(115200);
    
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    digitalWrite(RELAY_PIN, LOW);
    
    // Configure WiFi Manager
    wm.setAPCredentials("SmartSwitch-Setup");
    wm.setMDNSName("smartswitch");
    wm.setTimeout(30000);
    
    wm.onConnected(onWiFiConnected);
    wm.onDisconnected(onWiFiDisconnected);
    
    wm.begin();
    wm.autoConnect();
    
    Serial.println("🏠 Smart Home Switch Started");
    Serial.println("Features:");
    Serial.println("- MQTT integration with Home Assistant");
    Serial.println("- Physical button control");
    Serial.println("- Automatic device discovery");
}

void loop() {
    wm.loop();
    
    if (wm.isConnected()) {
        if (!mqtt.connected()) {
            connectToMQTT();
        }
        mqtt.loop();
    }
    
    handleButton();
    
    // Heartbeat every 5 minutes
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 300000) {
        if (mqtt.connected()) {
            publishState();
        }
        lastHeartbeat = millis();
    }
}
```

---

## Platform-Specific Examples

### 16. ESP32-Specific Features
Utilize ESP32-specific features like dual-core processing and deep sleep.

```cpp
#include "WiFiManager.h"
#include <esp_sleep.h>
#include <esp_wifi.h>

WiFiManager wm;
TaskHandle_t wifiTaskHandle;

// Deep sleep configuration
const int SLEEP_TIME_SECONDS = 300; // 5 minutes
RTC_DATA_ATTR int bootCount = 0;

void wifiTask(void * parameter) {
    // WiFi management runs on Core 0
    while (true) {
        wm.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void onWiFiConnected() {
    Serial.println("WiFi connected on Core 0");
    Serial.print("Running on core: ");
    Serial.println(xPortGetCoreID());
}

void enterDeepSleep() {
    Serial.println("Entering deep sleep for 5 minutes...");
    
    // Configure wake up timer
    esp_sleep_enable_timer_wakeup(SLEEP_TIME_SECONDS * 1000000ULL);
    
    // Optionally save current state
    if (wm.isConnected()) {
        // Force save current WiFi config before sleep
        Serial.println("Connected - WiFi config will persist");
    }
    
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
}

void setup() {
    Serial.begin(115200);
    
    bootCount++;
    Serial.print("Boot count: ");
    Serial.println(bootCount);
    
    // Check wake up reason
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wake up from deep sleep timer");
            break;
        default:
            Serial.println("Wake up from reset or power on");
            break;
    }
    
    // Configure WiFi Manager
    wm.setAPCredentials("ESP32-DeepSleep");
    wm.setTimeout(20000);
    wm.onConnected(onWiFiConnected);
    
    wm.begin();
    
    // Create WiFi task on Core 0
    xTaskCreatePinnedToCore(
        wifiTask,           // Function
        "WiFi Task",        // Name
        4096,              // Stack size
        NULL,              // Parameter
        1,                 // Priority
        &wifiTaskHandle,   // Handle
        0                  // Core 0
    );
    
    wm.autoConnect();
    
    Serial.print("Main setup running on core: ");
    Serial.println(xPortGetCoreID());
}

void loop() {
    // Main application runs on Core 1
    static unsigned long lastReport = 0;
    
    if (millis() - lastReport > 10000) {
        Serial.print("Main loop on core: ");
        Serial.println(xPortGetCoreID());
        
        if (wm.isConnected()) {
            Serial.print("WiFi: Connected to ");
            Serial.println(wm.getSSID());
        } else {
            Serial.println("WiFi: Not connected");
        }
        
        lastReport = millis();
    }
    
    // Enter deep sleep after 60 seconds of running
    if (millis() > 60000) {
        enterDeepSleep();
    }
    
    delay(1000);
}
```

### 17. ESP8266-Specific Optimization
ESP8266 optimized example with memory management and power saving.

```cpp
#include "WiFiManager.h"

WiFiManager wm;

void onWiFiConnected() {
    Serial.println("ESP8266 WiFi Connected");
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("Heap fragmentation: ");
    Serial.print(ESP.getHeapFragmentation());
    Serial.println("%");
}

void printMemoryInfo() {
    Serial.println("\n=== ESP8266 Memory Info ===");
    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
    Serial.print("Heap fragmentation: ");
    Serial.print(ESP.getHeapFragmentation());
    Serial.println("%");
    Serial.print("Max free block: ");
    Serial.print(ESP.getMaxFreeBlockSize());
    Serial.println(" bytes");
    Serial.print("Chip ID: ");
    Serial.println(ESP.getChipId(), HEX);
    Serial.print("Flash size: ");
    Serial.print(ESP.getFlashChipSize());
    Serial.println(" bytes");
    Serial.print("Sketch size: ");
    Serial.print(ESP.getSketchSize());
    Serial.println(" bytes");
    Serial.print("Free sketch space: ");
    Serial.print(ESP.getFreeSketchSpace());
    Serial.println(" bytes");
    Serial.println("==========================\n");
}

void setup() {
    Serial.begin(115200);
    
    // ESP8266 specific optimizations
    wm.setTimeout(15000);        // Shorter timeout for limited memory
    wm.setRetryDelay(3000);      // Quick retry
    wm.setMinSignalQuality(-75); // Only strong signals
    
    // Use shorter AP name to save memory
    wm.setAPCredentials("ESP8266-WiFi");
    wm.setMDNSName("esp8266");
    
    wm.onConnected(onWiFiConnected);
    
    printMemoryInfo();
    
    wm.begin();
    wm.autoConnect();
    
    Serial.println("ESP8266 WiFiManager Example");
    Serial.println("Optimized for limited memory");
}

void loop() {
    wm.loop();
    
    static unsigned long lastMemCheck = 0;
    
    // Check memory every 30 seconds
    if (millis() - lastMemCheck > 30000) {
        if (ESP.getFreeHeap() < 5000) {
            Serial.println("⚠️ Low memory warning!");
        }
        
        if (ESP.getHeapFragmentation() > 50) {
            Serial.println("⚠️ High fragmentation warning!");
        }
        
        lastMemCheck = millis();
    }
    
    // Yield to system for stability
    yield();
}
```

---

## Conclusion

This USAGE.md provides comprehensive examples covering:

- **Basic Usage**: From minimal setup to custom configurations
- **Advanced Features**: Manual control, button handling, backup/restore  
- **Display Integration**: LCD and OLED implementations with real-time updates
- **IoT Projects**: Environmental monitoring and smart home integration
- **Platform-Specific**: ESP32 dual-core and ESP8266 memory optimization

Choose the example that best fits your project requirements and customize as needed. Each example is self-contained and can be used as a starting point for your own implementation.

For more details, see the main [README.md](README.md) for API reference and troubleshooting information.