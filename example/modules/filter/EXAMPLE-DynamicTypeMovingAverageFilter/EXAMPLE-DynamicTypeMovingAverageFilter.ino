#define ENABLE_MODULE_DYNAMIC_TYPE_MOVING_AVERAGE_FILTER
#include "Kinematrix.h"

DynamicTypeMovingAverageFilter<int> dtmaf(10);

void setup() {
    Serial.begin(115200);
}

void loop() {
    int noisySensorValue = analogRead(A0) + random(-50, 50);
    int filteredValue = dtmaf.filter(noisySensorValue);

    Serial.print("Noisy: ");
    Serial.print(noisySensorValue);
    Serial.print("\tFiltered: ");
    Serial.println(filteredValue);

    delay(100);
}