#define ENABLE_MODULE_IO_EXPANDER
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_TIMER_TASK
#define ENABLE_ADDONS_AUTOLIGHT_V3

#include "Kinematrix.h"

using namespace AutoLight;

const long BAUDRATE = 115200;
const int ON_INT_PIN = 34;
const int OFF_INT_PIN = 17;
const int NEXT_INT_PIN = 36;
const int PREV_INT_PIN = 39;
const int INIT_DELAY = 30;
const int INIT_SEQUENCE = 0;
const int TOTAL_SEQUENCE = 15;
const bool IS_REVERSE = true;
const bool ENABLE_DEBUG = true;

BaseConfig config(ENABLE_DEBUG);
BaseChannel led(ENABLE_DEBUG);
ButtonInterrupt button(ENABLE_DEBUG);
TaskHandle task;
SerialCommander cmd;

void setup() {
  Serial.begin(BAUDRATE);
  delay(1000);

  config.setConfigIndex(I2C_CONFIG);
  auto err_ch = config.readChannel();
  auto err_ver = config.readVersion();

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

  cmd.init(&led, &config, &button);

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
      cmd.initWiFi();
      led.enableWebServer(true);
      led.setInitSequence(INIT_SEQUENCE);
      for (;;) {
        task.delay(20);
      }
    });
  });
}

void loop() {
  if (led.isReady()) led.runAutoLight([](uint32_t sequence) {});
  for (int i = 0; i < 4; i++) {
    if (button.isPressed(i)) {
      button.resetButton(i);
    }
  }
  cmd.process();
}