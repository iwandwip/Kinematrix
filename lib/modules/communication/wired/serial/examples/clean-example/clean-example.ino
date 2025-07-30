#include "ceria-serial.h"

using namespace CeriaDevOP;

CeriaSerial link;

void setup() {
    Serial.begin(115200);
    
    link.begin(Serial1, 115200);
    
    link.onReceive([](const String& key, const String& value) {
        Serial.println(key + "=" + value);
    });
}

void loop() {
    link.send("temp", 25.6f);
    link.send("status", "ok");
    link.tick();
    delay(1000);
}