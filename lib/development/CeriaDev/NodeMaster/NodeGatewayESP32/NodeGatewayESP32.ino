#include "lib/CeriaSerial.h"
#include "lib/CeriaSensorGPS.h"
#include "lib/CeriaSensorINA219.h"
#include "lib/CeriaDisplayDWIN.h"
using namespace CeriaDevOP;

CeriaSerial comm;
CeriaSensorGPS gps;
CeriaSensorINA219 power(0x40);
CeriaDisplayDWIN display;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== CeriaDev Complete Test with DWIN Display ===");

    Serial.print("Initializing CeriaSerial...");
    Serial1.begin(115200);
    comm.begin(Serial1, 115200, FRAME_AUTO);
    comm.setMode(FULL_DUPLEX);
    comm.setTimeout(2000);
    Serial.println("OK");

    Serial.print("Initializing CeriaSensorGPS...");
    #if defined(ESP32)
        Serial2.begin(9600);
        gps.begin(Serial2, 9600);
    #else
        Serial.println("GPS requires Serial2 (ESP32 only)");
    #endif
    Serial.println("OK");

    Serial.print("Initializing CeriaSensorINA219...");
    Wire.begin();
    if (power.begin(&Wire, INA219_CAL_32V_2A)) {
        Serial.println("OK");
    } else {
        Serial.println("FAILED");
    }

    Serial.print("Initializing CeriaDisplayDWIN...");
    #if defined(ESP32)
        Serial2.begin(115200, SERIAL_8N1, 16, 17);
        if (display.begin(Serial2, 115200)) {
            Serial.println("OK");
            Serial.println(display.getConnectionString());
        } else {
            Serial.println("FAILED");
        }
    #else
        Serial.println("DWIN requires Serial2 (ESP32 only)");
    #endif

    setupDisplayCallbacks();
    initializeDisplay();

    Serial.println("Setup complete:");
    Serial.println("- CeriaSerial: Serial1 communication");
    Serial.println("- CeriaSensorGPS: Serial2 GPS module (ESP32)");
    Serial.println("- CeriaSensorINA219: I2C power monitoring");
    Serial.println("- CeriaDisplayDWIN: Serial2 DWIN T5L display (ESP32)");
    Serial.println();
}

void setupDisplayCallbacks() {
    display.setTouchCallback([](uint16_t vp, uint16_t value, DWINTouchEvent event) {
        Serial.println("Touch Event - VP: 0x" + String(vp, HEX) + 
                      ", Value: " + String(value) + 
                      ", Event: " + String(event));
        
        switch (vp) {
            case 0x1000: // Home button
                display.setPage(0);
                comm.send("display_page", "0");
                break;
            case 0x1001: // Sensor page button
                display.setPage(1);
                comm.send("display_page", "1");
                break;
            case 0x1002: // Settings button
                display.setPage(2);
                comm.send("display_page", "2");
                break;
        }
    });

    display.setPageCallback([](uint8_t newPage, uint8_t oldPage) {
        Serial.println("Page changed from " + String(oldPage) + " to " + String(newPage));
        comm.send("page_change", String(newPage));
        
        switch (newPage) {
            case 0:
                initializeHomePage();
                break;
            case 1:
                initializeSensorPage();
                break;
            case 2:
                initializeSettingsPage();
                break;
        }
    });

    display.setErrorCallback([](DWINError error) {
        Serial.println("Display Error: " + String(error));
        comm.send("display_error", String(error));
    });

    display.setVariableCallback([](uint16_t vp, DWINDataType type, uint32_t value) {
        Serial.println("VP Update - 0x" + String(vp, HEX) + ": " + String(value));
    });
}

void initializeDisplay() {
    display.setPage(0);
    display.setBrightness(80);
    display.setVolume(50);
    
    display.registerVP(0x2000, DWIN_TYPE_FLOAT, true);   // Temperature
    display.registerVP(0x2001, DWIN_TYPE_UINT16, true);  // Humidity  
    display.registerVP(0x2002, DWIN_TYPE_FLOAT, true);   // Voltage
    display.registerVP(0x2003, DWIN_TYPE_FLOAT, true);   // Current
    display.registerVP(0x2004, DWIN_TYPE_STRING, false); // GPS Location
    display.registerVP(0x2005, DWIN_TYPE_UINT8, false);  // Satellites
}

void initializeHomePage() {
    display.setText(0x0100, "Smart Home Controller");
    display.setText(0x0101, "CeriaDev System");
    display.setText(0x0102, "Status: Online");
    display.setIcon(0x0103, 1);
}

void initializeSensorPage() {
    display.setText(0x1100, "Sensor Monitoring");
    display.setText(0x1101, "Temperature:");
    display.setText(0x1102, "Humidity:");
    display.setText(0x1103, "Power:");
    display.setText(0x1104, "GPS Location:");
}

void initializeSettingsPage() {
    display.setText(0x2100, "System Settings");
    display.setText(0x2101, "Brightness:");
    display.setText(0x2102, "Volume:");
    display.setText(0x2103, "Network:");
}

void loop() {
    comm.tick();
    gps.update();
    power.update();
    display.update();

    static unsigned long lastComm = 0;
    if (millis() - lastComm > 3000) {
        comm.send("temperature", 25.6f);
        comm.send("humidity", 60);
        comm.send("status", "online");
        lastComm = millis();
    }

    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > 1000) {
        updateDisplayData();
        lastDisplayUpdate = millis();
    }

    if (gps.hasNewData() && gps.hasFix()) {
        Serial.println("GPS: " + gps.getLocation() + " (" + String(gps.getSatellites()) + " sats)");
        comm.send("gps_location", gps.getLocation());
        comm.send("gps_satellites", gps.getSatellites());
        
        display.setText(0x2004, gps.getLocation());
        display.setNumber(0x2005, gps.getSatellites());
    }

    if (power.hasNewData()) {
        Serial.println("Power: " + power.getPowerString());
        comm.send("voltage", power.getVoltage());
        comm.send("current", power.getCurrent());
        comm.send("power", power.getPower());
        
        display.setFloat(0x2002, power.getVoltage(), 2);
        display.setFloat(0x2003, power.getCurrent(), 1);
    }

    if (display.hasNewTouch()) {
        DWINTouchData touch = display.getLastTouch();
        Serial.println("Display touched at VP: 0x" + String(touch.vp, HEX) + 
                      " (" + String(touch.x) + "," + String(touch.y) + ")");
        
        comm.send("touch_vp", String(touch.vp, HEX));
        comm.send("touch_xy", String(touch.x) + "," + String(touch.y));
    }

    if (display.hasNewPage()) {
        Serial.println("Display page changed to: " + String(display.getCurrentPage()));
    }

    comm.onReceive([](const String& key, const String& value) {
        if (key == "display_brightness") {
            display.setBrightness(value.toInt());
        } else if (key == "display_volume") {
            display.setVolume(value.toInt());
        } else if (key == "display_page") {
            display.setPage(value.toInt());
        } else if (key == "display_text") {
            int separatorIndex = value.indexOf(':');
            if (separatorIndex > 0) {
                String vpStr = value.substring(0, separatorIndex);
                String text = value.substring(separatorIndex + 1);
                uint16_t vp = strtol(vpStr.c_str(), NULL, 16);
                display.setText(vp, text);
            }
        }
    });

    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000) {
        printSystemStatus();
        lastStatus = millis();
    }

    delay(10);
}

void updateDisplayData() {
    float temp = 25.6 + (random(-50, 50) / 10.0);
    uint16_t humidity = 60 + random(-10, 10);
    
    display.setFloat(0x2000, temp, 1);
    display.setNumber(0x2001, humidity);
    
    String status = display.isConnected() ? "Connected" : "Disconnected";
    display.setText(0x0102, "Status: " + status);
}

void printSystemStatus() {
    Serial.println("=== System Status ===");
    Serial.println("CeriaSerial: " + String(comm.isConnected() ? "Connected" : "Disconnected"));
    Serial.println("GPS: " + String(gps.isConnected() ? "Connected" : "Disconnected") + 
                   " | Fix: " + String(gps.hasFix() ? "Yes" : "No"));
    Serial.println("Power: " + String(power.isConnected() ? "Connected" : "Disconnected"));
    Serial.println("Display: " + display.getConnectionString());
    Serial.println("Current Page: " + String(display.getCurrentPage()));
    
    uint32_t commSent, commReceived, commErrors;
    comm.getStats(commSent, commReceived, commErrors);
    Serial.println("Comm Stats - S:" + String(commSent) + " R:" + String(commReceived) + " E:" + String(commErrors));
    
    uint32_t dispSent, dispReceived, dispErrors;
    display.getStats(dispSent, dispReceived, dispErrors);
    Serial.println("Display Stats - S:" + String(dispSent) + " R:" + String(dispReceived) + " E:" + String(dispErrors));
    
    Serial.println();
}