#define ENABLE_MODULE_EXPONENTIAL_MOVING_AVERAGE_FILTER
#include "Kinematrix.h"

ExponentialMovingAverageFilter emaf(0.1);

void setup() {
    Serial.begin(115200);
}

void loop() {
    float noisySensorValue = analogRead(A0) + random(-50, 50);
    float filteredValue = emaf.filter(noisySensorValue);

    Serial.print("Noisy: ");
    Serial.print(noisySensorValue);
    Serial.print("\tFiltered: ");
    Serial.println(filteredValue);

    delay(100);
}