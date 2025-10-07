#define ENABLE_MODULE_IO_EXPANDER
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_TIMER_TASK

#define ENABLE_ADDONS_AUTOLIGHT_V2

#include "Kinematrix.h"

using namespace AutoLight;

/* SETTING PARAMETER START */    // [DEFAULT VALUE]  | [DESCRIPTION]
const long BAUDRATE = 115200;    // [DEFAULT] 115200 | Baud rate of serial communication.
const int ON_INT_PIN = 34;       // [DEFAULT] 36     | Pin number to handle an interrupt.
const int OFF_INT_PIN = 17;      // [DEFAULT] 39     | Pin number to handle an interrupt.
const int NEXT_INT_PIN = 36;     // [DEFAULT] 34     | Pin number to handle an interrupt.
const int PREV_INT_PIN = 39;     // [DEFAULT] 35     | Pin number to handle an interrupt.
const int INIT_DELAY = 30;       // [DEFAULT] 30     | Initial delay for each LED.
const int INIT_SEQUENCE = 0;     // [DEFAULT] 0      | Initial sequence modes.
const int TOTAL_SEQUENCE = 15;   // [DEFAULT] 10     | Total sequence modes.
const bool IS_REVERSE = true;    // [DEFAULT] true   | Reverse LED logic.
const bool ENABLE_DEBUG = true;  // [DEFAULT] false  | Debug all to the serial monitor. set to false first
/* SETTING PARAMETER END */

BaseConfig config(ENABLE_DEBUG);
BaseChannel led(ENABLE_DEBUG);
ButtonInterrupt button(ENABLE_DEBUG);

TaskHandle task;
Credentials credentials;

void setup() {
  Serial.begin(BAUDRATE);
  initCredentials();
  config.setConfigIndex(I2C_CONFIG);
  auto err_ch = config.readChannel();
  auto err_ver = config.readVersion();

  /* CUSTOM CONFIGURATION START */
  // config.setChannel(18);                                    // [DEFAULT] 24                        | Total channel
  // config.setAddress(config.getIoSize(), 0x00038, 0x00021);  // [DEFAULT] 0x00020, 0x00024, 0x00022 | Config Address
  // config.setSequence(
  //   config.getChannel(),              // [SPECIFIED]
  //   1, 2, 4, 5, 6, 7, 8, 9,           // [ROW 1]   1|  2|  3|  4|  5|  6|  7|  8|
  //   10, 11, 12, 3, 22, 13, 14, 15,    // [ROW 2]   9| 10| 11| 12| 13| 14| 15| 16|
  //   16, 17, 18, 19, 20, 21, 23, 24);  // [ROW 3]  17| 18| 19| 20| 21| 22| 23| 24|
  /* CUSTOM CONFIGURATION END */

  config.initialize();
  config.debugConfig();

  button.attach(0, ON_INT_PIN, INPUT_PULLUP, FALLING, []() [[IRAM_ATTR]] -> void {
    button.readInterrupt(0, []() -> void {
      led.onMode();
    });
  });
  button.attach(1, OFF_INT_PIN, INPUT_PULLUP, FALLING, []() [[IRAM_ATTR]] -> void {
    button.readInterrupt(1, []() -> void {
      led.offMode();
    });
  });
  button.attach(2, NEXT_INT_PIN, INPUT_PULLUP, FALLING, []() [[IRAM_ATTR]] -> void {
    button.readInterrupt(2, []() -> void {
      led.nextMode();
    });
  });
  button.attach(3, PREV_INT_PIN, INPUT_PULLUP, FALLING, []() [[IRAM_ATTR]] -> void {
    button.readInterrupt(3, []() -> void {
      led.previousMode();
    });
  });
  button.debug();

  led.setParamCallback([]() -> void {
    led.setTotalSequence(TOTAL_SEQUENCE);
    led.setInitDelay(INIT_DELAY);
    led.setInitSequence(INIT_SEQUENCE);
    led.reverse(IS_REVERSE);
    led.enableI2CConfig();
    led.attachConfig(config.getConfigData());
    led.attachInterrupt(button());
  });

  auto err_init = led.initialize();
  if (!err_init) return;
  led.debug();

  task.initialize([]() -> void {
    task.createTask(4000, [](void *pvParameter) [[TASK_FUNCTION]] -> void {
      Indicator indicator;
      for (;;) {
        auto sequence = led.getSequenceIndex();
        indicator.show(sequence);
        indicator.update();
        task.delay(20);
      }
    });
    task.createTask(10000, [](void *pvParameter) [[TASK_FUNCTION]] -> void {
      initServer();
      led.setInitSequence(INIT_SEQUENCE);
      for (;;) {
        task.delay(20);
      }
    });
  });
}

void loop() {
  if (led.isReady()) led.runAutoLight(runAutoLightCallback);
  for (int i = 0; i < 4; i++) {
    if (button.isPressed(i)) {
      button.resetButton(i);
    }
  }
}