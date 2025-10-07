#define ENABLE_MODULE_MOVING_AVERAGE_FILTER
#include "Kinematrix.h"

MovingAverageFilter maf(10);

void setup() {
    Serial.begin(115200);
}

void loop() {
    float noisySensorValue = analogRead(A0) + random(-50, 50);
    float filteredValue = maf.filter(noisySensorValue);

    Serial.print("Noisy: ");
    Serial.print(noisySensorValue);
    Serial.print("	Filtered: ");
    Serial.println(filteredValue);

    delay(100);
}