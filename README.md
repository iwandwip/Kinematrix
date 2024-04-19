
# Kinematrix Beta

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

## Package List
```cpp
// modules
#include "modules/communication/wired/i2c/i2c-expander.h"
#include "modules/communication/wired/i2c/i2c-scanner.h"
#include "modules/communication/wired/i2c/io-expander.h"
#include "modules/communication/wired/serial/hard-serial.h"
#include "modules/communication/wired/serial/soft-serial.h"
#include "modules/communication/wired/serial/swap-serial.h"
#include "modules/communication/wired/spi/dot-matrix.h"
#include "modules/communication/wireless/lora/lora-com.h"

#include "modules/db/firebase-handler.h"
#include "modules/display/lcd-menu.h"
#include "modules/driver/motor/motor-driver.h"
#include "modules/driver/servo/HCPCA9685.h"
#include "modules/file/SDArduino.h"

#include "modules/io/input-module.h"
#include "modules/io/output-module.h"
#include "modules/io/rotary-module.h"

#include "modules/task/Task.h"
#include "modules/time/timer-duration.h"
#include "modules/time/timer-task.h"

// sensor
#include "Arduino.h"
#include "sensors/abstract-sens.h"
#include "sensors/aht-sens.h"
#include "sensors/bme280-sens.h"
#include "sensors/dht-sens.h"
#include "sensors/ds18b20-sens.h"
#include "sensors/flowmeter-sens.h"
#include "sensors/gm67-sens.h"
#include "sensors/hx711-sens.h"
#include "sensors/ina219-sens.h"
#include "sensors/ldr-sens.h"
#include "sensors/max6675-sens.h"
#include "sensors/max31865-sens.h"
#include "sensors/rfid-sens.h"
#include "sensors/rtc-sens.h"
#include "sensors/soil-sens.h"
#include "sensors/soilph-sens.h"
#include "sensors/tds-sens.h"
#include "sensors/turbidity-sens.h"
#include "sensors/ultrasonic-sens.h"
#include "sensors/voltage-sens.h"

```


## Authors

- [@iwandwip](https://github.com/iwandwip)


## Feedback

If you have any feedback, please reach out to me at mcclawsher369@gmail.com


## License

[MIT](https://choosealicense.com/licenses/mit/)

