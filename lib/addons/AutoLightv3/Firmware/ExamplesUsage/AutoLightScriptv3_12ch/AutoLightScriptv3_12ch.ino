#define ENABLE_MODULE_IO_EXPANDER
#define ENABLE_MODULE_DIGITAL_OUTPUT
#define ENABLE_MODULE_TIMER_TASK
#define ENABLE_ADDONS_AUTOLIGHT_V3

#include "Kinematrix.h"

using namespace AutoLight;

const long BAUDRATE = 115200;
const int NEXT_INT_PIN = 36;
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

  config.setDynamicDistribution(12, 2, 6, 6);
  config.initialize();

  button.attach(2, NEXT_INT_PIN, INPUT_PULLUP, FALLING, []() [[IRAM_ATTR]] -> void {
    button.readInterrupt(2, []() -> void {
      led.smartButtonPress(2);
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
    led.setButtonMode(BUTTON_MODE_1BUTTON);
    led.enableSequenceMapping(true);
    led.setActiveSequences(8, 0, 1, 5, 7, 9, 10, 16, 17);
  });

  auto err_init = led.initialize();
  if (!err_init) {
    return;
  }

  cmd.init(&led, &config, &button);

  cmd.initWiFi();
  led.enableWebServer(true);

  task.initialize([]() -> void {
    task.createTask(4000, [](void *pvParamester) [[TASK_FUNCTION]] -> void {
      Indicator indicator;
      for (;;) {
        auto sequence = led.getSequenceIndex();
        indicator.show(sequence);
        indicator.update();
        task.delay(20);
      }
    });
  });

  Serial.println("\n=== AutoLight V3 with SerialCommander ===");
  Serial.println("Type 'help' for available commands");
  Serial.print("AutoLight> ");
}

void loop() {
  if (led.isReady()) {
    led.runAutoLight([](uint32_t sequence) {
    });
  }

  for (int i = 0; i < 4; i++) {
    if (button.isPressed(i)) {
      button.resetButton(i);
    }
  }

  cmd.process();
}