
# Kinematrix

**Arduino Library for Educational Purposes**.
Utilize this library to fulfill your project needs. Within it, there are modules for inputs such as button sensors, and the like, as well as modules for outputs such as LEDs, buzzers, and motor drivers.


## Basic Usage

```cpp
#include "Kinematrix.h"
#include "index/ultrasonic.h"
#include "index/soil_moisture.h"

SensorModule sensor;

void setup() {
  Serial.begin(115200);
  sensor.addModule("sonar", new UltrasonicSens(13, 12));
  sensor.addModule("moisture", new SoilMoistureSens(A0));
  sensor.init();
}

void loop() {
  sensor.update([]() -> void {
    sensor.debug(300, true);
  });
}

```

```cpp
#include "Kinematrix.h"
#include "index/digital_input.h"
#include "index/digital_output.h"

DigitalIn button(2);
DigitalOut led(3);

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (button.isPressed()) {
    led.on();
  }
  button.update();
}

```


## Authors

- [@iwandwip](https://github.com/iwandwip)


## Feedback

If you have any feedback, please reach out to me at mcclawsher369@gmail.com


## License

[MIT](https://choosealicense.com/licenses/mit/)

