# WiFi Manager - Cross-Platform

A modern, feature-rich WiFi Manager library for ESP32 and ESP8266 microcontrollers.

## Platform Support

### ✅ ESP32
- Full feature set
- AsyncWebServer support
- LittleFS file system
- Advanced scanning with event system
- 2MB JSON buffer for complex operations

### ✅ ESP8266  
- Full feature set
- AsyncWebServer support
- LittleFS file system (requires Arduino Core 2.7.0+)
- Basic scanning functionality
- 1MB JSON buffer optimized for limited memory

## Features

- **Modern Web Interface** with Shadcn/ui inspired design
- **mDNS Support** - Access via `wifimanager.local` even when connected to WiFi
- **Captive Portal** for easy WiFi configuration
- **Persistent Settings** stored in LittleFS
- **WiFi Scanning** with caching and refresh capabilities
- **Static IP Configuration** support
- **Smart Timeout System** - Auto-fallback to config mode when WiFi unavailable
- **Reset Functions** - WiFi only or complete system reset
- **Cross-Platform** - Single codebase for ESP32 and ESP8266
- **Responsive Design** - Works on mobile and desktop
- **Auto-Reconnection** - Intelligent reconnection handling

## Quick Start

### ESP32
```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    
    if (!wm.begin()) {
        Serial.println("WiFi Manager failed to start");
        return;
    }
    
    Serial.println("WiFi Manager started");
    Serial.println("Connect to AP and go to 192.168.4.1");
}

void loop() {
    wm.loop();
}
```

### ESP8266
```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    
    if (!wm.begin()) {
        Serial.println("WiFi Manager failed to start");
        return;
    }
    
    Serial.println("WiFi Manager started");
    Serial.println("Connect to AP and go to 192.168.4.1");
}

void loop() {
    wm.loop();
}
```

## Complete API Reference

### Core Methods
```cpp
bool begin()                          // Initialize WiFi Manager and LittleFS
bool autoConnect()                    // Try to connect with saved credentials
void loop()                          // Process WiFi Manager (call in main loop)
bool isConnected()                   // Check WiFi connection status
bool isConfigMode()                  // Check if in configuration mode
WiFiManagerState getState()          // Get current state (WM_IDLE, WM_CONNECTING, etc.)
```

### Network Information
```cpp
String getSSID()                     // Get connected network name
String getPassword()                 // Get saved password
IPAddress getIP()                    // Get device IP address
int getRSSI()                       // Get signal strength (dBm)
int getSignalQuality()              // Get signal quality (0-100%)
```

### Configuration Methods
```cpp
void setAPCredentials(ssid)                    // Open Access Point
void setAPCredentials(ssid, password)          // Password-protected AP
void setHostname(hostname)                     // Set device hostname
void setTimeout(timeout)                       // Connection timeout (ms)
void setRetryDelay(delay)                     // Retry delay before config mode (ms)  
void setConfigTimeout(timeout)                 // Config mode timeout (ms, 0=no timeout)
void setReconnectInterval(interval)            // Reconnection check interval (ms)
void setMinSignalQuality(quality)             // Minimum signal strength (dBm)
void enableAutoReconnect(enable)               // Enable/disable auto-reconnection
void setScanCacheTime(seconds)                 // WiFi scan cache duration
```

### mDNS Methods
```cpp
void setMDNSName(hostname)                    // Set mDNS hostname (.local)
void enableMDNS(enable)                       // Enable/disable mDNS
String getMDNSName()                          // Get mDNS hostname
bool isMDNSEnabled()                          // Check if mDNS is enabled
bool isMDNSStarted()                          // Check if mDNS is running
```

### WiFi Management
```cpp
void startConfig()                            // Start configuration mode manually
void stopConfig()                             // Stop configuration mode  
void disconnect()                             // Disconnect from WiFi
void reset()                                  // Reset WiFi configuration only
void resetAll()                               // Complete system reset + restart
void restart()                                // Restart device
```

### Callback Methods
```cpp
void onConnected(callback)                    // Called when WiFi connects
void onDisconnected(callback)                 // Called when WiFi disconnects
void onConfigMode(callback)                   // Called when entering config mode

// Callback function signature:
typedef void (*WiFiManagerCallback)(void);
```

### Advanced Scanning Methods
```cpp
bool WiFi_scanNetworks()                      // Basic network scan
bool WiFi_scanNetworks(cachetime)             // Scan with cache time
bool WiFi_scanNetworks(cachetime, async)      // Async scan with cache
bool getScanCompleteCached()                  // Check if cached scan available
int getScanCacheTime()                        // Get current cache time
```

### State Constants
```cpp
enum WiFiManagerState {
    WM_IDLE,           // Initial state
    WM_CONNECTING,     // Attempting to connect
    WM_CONNECTED,      // Successfully connected
    WM_CONFIG_MODE,    // Configuration portal active
    WM_RETRY           // Waiting before config mode
};
```

## Web Interface

### Access Methods:
- **AP Mode**: `http://192.168.4.1` when device is in configuration mode
- **mDNS**: `http://wifimanager.local` - Works even when connected to WiFi
- **Direct IP**: `http://[device-ip]` when connected to WiFi

### Features:
- **Network Scanning** - View available WiFi networks with signal strength
- **Connection Setup** - Enter SSID and password
- **Static IP Configuration** - Advanced IP settings (expandable section)
- **Real-time Status** - Live connection status and network information
- **Reset Options** - WiFi reset or complete system reset with confirmation
- **Password Visibility** - Toggle password visibility for easier input

## mDNS Configuration

mDNS (Multicast DNS) allows you to access the web interface using a hostname instead of IP address, even when the device is connected to WiFi.

### Default Setup
```cpp
WiFiManager wm;
// mDNS is enabled by default with hostname "wifimanager.local"
```

### Custom Hostname
```cpp
WiFiManager wm;
wm.setMDNSName("mydevice");  // Access via "mydevice.local"
wm.enableMDNS(true);         // Enable/disable mDNS
```

### mDNS Methods
```cpp
wm.setMDNSName("hostname");     // Set custom hostname
wm.enableMDNS(true/false);      // Enable/disable mDNS
String name = wm.getMDNSName(); // Get current hostname
bool enabled = wm.isMDNSEnabled(); // Check if mDNS is enabled
bool started = wm.isMDNSStarted(); // Check if mDNS is running
```

### Benefits:
- **Persistent Access** - Access web interface even after connecting to WiFi
- **Easy Management** - Change WiFi settings without device reset
- **User Friendly** - Remember `mydevice.local` instead of IP addresses
- **Cross-Platform** - Works on ESP32 and ESP8266

## Advanced Configuration

### Timeout Settings
Control how long the device waits before entering configuration mode:

```cpp
WiFiManager wm;

// Connection timeout - how long to try connecting to saved WiFi (default: 30 seconds)
wm.setTimeout(15000);  // 15 seconds

// Retry delay - delay before starting config mode after failed connection (default: 5 seconds)  
wm.setRetryDelay(3000);  // 3 seconds

// Config timeout - how long config mode stays active (default: 5 minutes, 0 = no timeout)
wm.setConfigTimeout(300000);  // 5 minutes
```

### Auto-Reconnection
```cpp
// Enable/disable automatic reconnection when WiFi is lost
wm.enableAutoReconnect(true);   // Default: true

// Set reconnection interval for monitoring connection
wm.setReconnectInterval(30000); // Check every 30 seconds
```

### Signal Quality Filtering
```cpp
// Set minimum signal quality for networks to appear in scan (default: -80 dBm)
wm.setMinSignalQuality(-70);  // Only show networks with signal > -70 dBm
```

### Access Point Configuration
```cpp
// Customize the configuration AP
wm.setAPCredentials("MyDevice-Config");           // Open AP
wm.setAPCredentials("MyDevice-Config", "12345");  // Password protected AP
wm.setHostname("MyESP32Device");                  // Device hostname
```

### Scan Cache Settings
```cpp
// Control WiFi network scanning behavior
wm.setScanCacheTime(30);  // Cache scan results for 30 seconds (default)
```

## Connection Flow & Behavior

### Boot Sequence
Understanding what happens when your device starts:

```
1. Device Boot
2. Load saved WiFi credentials from LittleFS
3. If credentials exist:
   ├─ Try to connect (30 seconds timeout)
   ├─ If successful: Normal operation + start web server + mDNS
   └─ If failed: Wait 5 seconds → Start config mode
4. If no credentials: Immediately start config mode
```

### Timeline Example
When moving to a location without saved WiFi:
```
Boot → Try connect (30s) → Retry delay (5s) → Config AP appears
Total time: ~35 seconds
```

### Scenarios

**Scenario 1: First Time Setup**
- No saved credentials
- Config AP appears immediately
- User connects and configures WiFi

**Scenario 2: Normal Operation**  
- Saved credentials exist and WiFi is available
- Connects within a few seconds
- Web interface accessible via mDNS

**Scenario 3: WiFi Not Available**
- Saved credentials exist but WiFi not found
- Waits 30 seconds trying to connect
- Enters config mode after 5 second delay
- User can configure new WiFi

**Scenario 4: WiFi Password Changed**
- Device tries to connect with old password
- Fails after 30 seconds
- Enters config mode for reconfiguration

### State Management
The device operates in different states:
- **WM_IDLE**: Initial state
- **WM_CONNECTING**: Attempting to connect to WiFi
- **WM_CONNECTED**: Successfully connected to WiFi
- **WM_RETRY**: Waiting before entering config mode
- **WM_CONFIG_MODE**: Configuration portal active

## File System Structure

```
/config.json     - WiFi credentials and network settings
/settings.json   - WiFi Manager configuration
/index.html      - Main web interface
/style.css       - Shadcn/ui inspired styling
```

## Platform Differences

| Feature | ESP32 | ESP8266 |
|---------|-------|---------|
| JSON Buffer | 2048 bytes | 1024 bytes |
| Max Networks | 20 | 10 |
| Event System | Full WiFi events | Basic polling |
| Memory Usage | Higher | Optimized |
| Async Scanning | Yes | Yes |
| Reset Method | `WiFi.disconnect(true,true)` | `WiFi.persistent(true)` + `WiFi.disconnect(true)` |

## Troubleshooting

### Common Issues

**Problem: "LittleFS Mount Failed"**
```
Solution: Upload the data folder to LittleFS
1. Install ESP32/ESP8266 LittleFS Data Upload plugin
2. Place index.html and style.css in /data/ folder
3. Tools → ESP32/ESP8266 Sketch Data Upload
```

**Problem: Cannot access 192.168.4.1**
```
Solution: Check AP connection
1. Look for "ESP32-WiFiManager" or your custom AP name
2. Connect to the AP (may have password if configured)
3. Wait for captive portal or manually go to 192.168.4.1
```

**Problem: mDNS not working (.local address)**
```
Solutions:
1. Ensure mDNS is enabled: wm.enableMDNS(true);
2. Check platform support (Windows may need Bonjour)
3. Try different browsers (Chrome/Safari work best)
4. Verify network allows mDNS multicast traffic
```

**Problem: WiFi not connecting after config**
```
Solutions:
1. Verify correct SSID/password entered
2. Check signal strength (minimum -80 dBm by default)
3. Ensure WiFi network is 2.4GHz (ESP32/ESP8266 limitation)
4. Try reset: wm.reset() or use web interface reset button
```

**Problem: Device keeps entering config mode**
```
Solutions:
1. Check if saved WiFi network is available
2. Verify WiFi password hasn't changed
3. Check connectionTimeout setting (may be too short)
4. Monitor Serial output for connection details
```

### Debugging Tips

**Enable Serial Debug Output**
```cpp
void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);  // Enable WiFi debug
    
    WiFiManager wm;
    wm.begin();
}
```

**Check Saved Configuration**
```cpp
void setup() {
    // Check if config files exist
    if (LittleFS.exists("/config.json")) {
        Serial.println("WiFi config found");
    } else {
        Serial.println("No WiFi config - will start AP mode");
    }
}
```

**Manual Reset via Code**
```cpp
// In setup() or triggered by button/condition
wm.resetAll();  // Complete reset + restart
// or
wm.reset();     // WiFi config reset only
```

### Platform-Specific Notes

**ESP32**
- Uses hardware WiFi events for faster scanning
- Higher memory buffer (2048 bytes) for JSON operations
- Supports up to 20 networks in scan results
- `WiFi.disconnect(true, true)` for complete credential clearing

**ESP8266**
- Uses polling-based scan completion detection  
- Lower memory buffer (1024 bytes) optimized for limited RAM
- Supports up to 10 networks in scan results
- Requires `WiFi.persistent(true)` + `WiFi.disconnect(true)` for credential clearing
- Minimum Arduino Core 2.7.0 required for LittleFS

## Installation & Setup

### Step 1: Install Dependencies
```bash
# In Arduino IDE Library Manager, install:
- ESPAsyncWebServer
- ArduinoJson (version 7.x)
- AsyncTCP (for ESP32) or ESPAsyncTCP (for ESP8266)
```

### Step 2: Copy Library Files
```bash
# Copy these files to your project:
- WiFiManager.h
- WiFiManager.cpp
- data/index.html
- data/style.css
```

### Step 3: Upload Web Files
```bash
# Install LittleFS Upload Plugin
1. Download ESP32/ESP8266 LittleFS Data Upload plugin
2. Install in Arduino IDE tools folder
3. Restart Arduino IDE

# Upload Data Folder
1. Create /data/ folder in sketch directory
2. Copy index.html and style.css to /data/
3. Tools → ESP32 Sketch Data Upload (or ESP8266 equivalent)
```

### Step 4: Basic Implementation
```cpp
#include "WiFiManager.h"

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    
    if (!wm.begin()) {
        Serial.println("Failed to start WiFiManager");
        return;
    }
    
    wm.autoConnect();
}

void loop() {
    wm.loop();
}
```

## Requirements

### ESP32
- ESP32 Arduino Core 1.0.6+
- ESPAsyncWebServer library
- ArduinoJson library 7.x
- LittleFS support
- ESPmDNS library (included in core)

### ESP8266
- ESP8266 Arduino Core 2.7.0+ (for LittleFS)  
- ESPAsyncWebServer library
- ArduinoJson library 7.x
- LittleFS support
- ESP8266mDNS library (included in core)

## Examples

### Basic Usage
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

### Advanced Configuration with Callbacks
```cpp
#include "WiFiManager.h"

WiFiManager wm;

void onWiFiConnected() {
    Serial.println("Connected to WiFi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Access via: http://");
    Serial.print(wm.getMDNSName());
    Serial.println(".local");
}

void onWiFiDisconnected() {
    Serial.println("WiFi connection lost!");
}

void onConfigMode() {
    Serial.println("Entered config mode");
    Serial.print("Connect to AP: ");
    Serial.println("MyDevice-Setup");
    Serial.println("Go to: http://192.168.4.1");
}

void setup() {
    Serial.begin(115200);
    
    // Advanced configuration
    wm.setAPCredentials("MyDevice-Setup", "password123");
    wm.setHostname("MyESP32Device");
    wm.setMDNSName("mydevice");
    wm.setTimeout(20000);        // 20 seconds connection timeout
    wm.setRetryDelay(3000);      // 3 seconds retry delay
    wm.setMinSignalQuality(-75); // Only show strong networks
    wm.enableAutoReconnect(true);
    
    // Set callbacks
    wm.onConnected(onWiFiConnected);
    wm.onDisconnected(onWiFiDisconnected);
    wm.onConfigMode(onConfigMode);
    
    if (!wm.begin()) {
        Serial.println("WiFiManager failed to start!");
        return;
    }
    
    Serial.println("Starting WiFiManager...");
    wm.autoConnect();
}

void loop() {
    wm.loop();
    
    // Your application code here
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 30000) {  // Every 30 seconds
        if (wm.isConnected()) {
            Serial.print("WiFi Status: Connected to ");
            Serial.print(wm.getSSID());
            Serial.print(" (Signal: ");
            Serial.print(wm.getSignalQuality());
            Serial.println("%)");
        } else {
            Serial.println("WiFi Status: Disconnected");
        }
        lastCheck = millis();
    }
}
```

### Manual Control Example  
```cpp
#include "WiFiManager.h"

WiFiManager wm;
bool manualConfigMode = false;

void setup() {
    Serial.begin(115200);
    
    wm.setAPCredentials("ManualConfig-AP");
    wm.begin();
    
    // Try to auto-connect first
    if (!wm.autoConnect()) {
        Serial.println("Auto-connect failed, but that's OK");
    }
    
    Serial.println("Commands:");
    Serial.println("'c' - Start config mode");
    Serial.println("'s' - Stop config mode"); 
    Serial.println("'r' - Reset WiFi config");
    Serial.println("'R' - Reset all + restart");
    Serial.println("'i' - Show info");
}

void loop() {
    wm.loop();
    
    // Handle serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        
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
                Serial.println("Resetting WiFi config...");
                wm.reset();
                break;
                
            case 'R':
                Serial.println("Complete reset...");
                wm.resetAll();
                break;
                
            case 'i':
                Serial.println("=== Device Info ===");
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
                    Serial.println("%");
                }
                Serial.print("Config Mode: ");
                Serial.println(wm.isConfigMode() ? "Active" : "Inactive");
                Serial.print("mDNS: ");
                Serial.print(wm.isMDNSStarted() ? "Running" : "Stopped");
                if (wm.isMDNSStarted()) {
                    Serial.print(" (");
                    Serial.print(wm.getMDNSName());
                    Serial.print(".local)");
                }
                Serial.println();
                break;
        }
    }
}
```

### Button Reset Example
```cpp
#include "WiFiManager.h"

#define RESET_BUTTON_PIN 0  // Boot button on most ESP32 boards

WiFiManager wm;
unsigned long buttonPressTime = 0;
bool buttonPressed = false;

void setup() {
    Serial.begin(115200);
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    
    wm.setAPCredentials("ESP32-Config");
    wm.begin();
    wm.autoConnect();
    
    Serial.println("Hold BOOT button for 5 seconds to reset WiFi config");
}

void loop() {
    wm.loop();
    
    // Check reset button
    bool currentButtonState = (digitalRead(RESET_BUTTON_PIN) == LOW);
    
    if (currentButtonState && !buttonPressed) {
        // Button just pressed
        buttonPressed = true;
        buttonPressTime = millis();
        Serial.println("Reset button pressed...");
    } else if (!currentButtonState && buttonPressed) {
        // Button just released
        buttonPressed = false;
        unsigned long pressDuration = millis() - buttonPressTime;
        
        if (pressDuration >= 5000) {
            Serial.println("Long press detected - Resetting WiFi config!");
            wm.reset();
            delay(1000);
            ESP.restart();
        } else {
            Serial.println("Short press - ignoring");
        }
    } else if (buttonPressed) {
        // Button currently held
        unsigned long currentDuration = millis() - buttonPressTime;
        if (currentDuration >= 5000) {
            Serial.println("5 seconds reached - Release to reset!");
        }
    }
}
```

## License

MIT License - see LICENSE file for details.