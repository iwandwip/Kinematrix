void runAutoLightCallback(uint32_t sequence) {
  // Serial.println(sequence);
}

// void BaseChannel::taskSequence12() {
//   // Serial.println("BaseChannel::taskSequence10() start");
//   if (config_data_ptr_->header.pin_size_ < 24) return;

//   static int size = config_data_ptr_->header.pin_size_;
//   static uint8_t *pin = config_data_ptr_->header.pin_ptr_;
//   static uint32_t time = 30;

//   static TaskHandle taskCh10;

//   auto taskLed1Callback = [](void *pvParameter) -> void {
//     static int ledIndexStart = (size - 8) / 2;
//     static int ledIndexEnd = ledIndexStart + 4;
//     for (;;) {
//       for (int i = ledIndexEnd - 1; i >= ledIndexStart; --i) {
//         if (led.getSequenceIndex() == 10) led.set(pin[i], HIGH);
//         taskCh10.delay(time / 2);
//         if (led.getSequenceIndex() == 10) led.set(pin[i], LOW);
//         taskCh10.delay(time / 2);
//       }
//       for (int i = ledIndexStart; i < ledIndexEnd; ++i) {
//         if (led.getSequenceIndex() == 10) led.set(pin[i], HIGH);
//         taskCh10.delay(time / 2);
//         if (led.getSequenceIndex() == 10) led.set(pin[i], LOW);
//         taskCh10.delay(time / 2);
//       }
//     }
//   };

//   auto taskLed2Callback = [](void *pvParameter) -> void {
//     static int ledIndexStart = ((size - 8) / 2) + 4;
//     static int ledIndexEnd = ledIndexStart + 4;
//     for (;;) {
//       for (int i = ledIndexStart; i < ledIndexEnd; ++i) {
//         if (led.getSequenceIndex() == 10) led.set(pin[i], HIGH);
//         taskCh10.delay(time / 2);
//         if (led.getSequenceIndex() == 10) led.set(pin[i], LOW);
//         taskCh10.delay(time / 2);
//       }
//       for (int i = ledIndexEnd - 1; i >= ledIndexStart; --i) {
//         if (led.getSequenceIndex() == 10) led.set(pin[i], HIGH);
//         taskCh10.delay(time / 2);
//         if (led.getSequenceIndex() == 10) led.set(pin[i], LOW);
//         taskCh10.delay(time / 2);
//       }
//     }
//   };

//   auto taskLed3Callback = [](void *pvParameter) -> void {  // ledBlue
//     static int ledIndexStart = size - 1;
//     static int ledIndexEnd = size - size;
//     for (;;) {
//       for (int i = 0; i < 4; i++) {
//         if (led.getSequenceIndex() == 10) {
//           led.set(pin[ledIndexStart], HIGH);
//           led.set(pin[ledIndexStart - 1], HIGH);
//           led.set(pin[ledIndexEnd], HIGH);
//           led.set(pin[ledIndexEnd + 1], HIGH);
//         }
//         taskCh10.delay(time * 4);
//         if (led.getSequenceIndex() == 10) {
//           led.set(pin[ledIndexStart], LOW);
//           led.set(pin[ledIndexStart - 1], LOW);
//           led.set(pin[ledIndexEnd], LOW);
//           led.set(pin[ledIndexEnd + 1], LOW);
//         }
//         taskCh10.delay(time * 4);
//       }

//       for (int i = 0; i < 3; i++) {
//         for (int j = 0; j < 3; j++) {
//           if (led.getSequenceIndex() == 10) {
//             led.set(pin[ledIndexStart], HIGH);
//             led.set(pin[ledIndexStart - 1], HIGH);
//             led.set(pin[ledIndexEnd], HIGH);
//             led.set(pin[ledIndexEnd + 1], HIGH);
//           }
//           taskCh10.delay(time);
//           if (led.getSequenceIndex() == 10) {
//             led.set(pin[ledIndexStart], LOW);
//             led.set(pin[ledIndexStart - 1], LOW);
//             led.set(pin[ledIndexEnd], LOW);
//             led.set(pin[ledIndexEnd + 1], LOW);
//           }
//           taskCh10.delay(time);
//         }
//         taskCh10.delay(time * 7);
//       }
//       taskCh10.delay(time * 3);
//     }
//   };

//   auto taskLed4Callback = [](void *pvParameter) -> void {  // ledWhite2
//     static int ledIndexStart = size - 6;
//     static int ledIndexEnd = size - size + 6;
//     static uint32_t timerLedTest;
//     for (;;) {
//       if (millis() - timerLedTest > time * 7) {
//         uint8_t pinLed1 = (pinLed1 == ledIndexStart) ? ledIndexEnd - 1 : ledIndexStart;
//         uint8_t pinLed2 = (pinLed2 == ledIndexStart - 1) ? (ledIndexEnd - 1) + 1 : ledIndexStart - 1;
//         for (int i = 0; i < 4; i++) {
//           if (led.getSequenceIndex() == 10) led.set(pin[pinLed1], HIGH);
//           if (led.getSequenceIndex() == 10) led.set(pin[pinLed2], HIGH);
//           taskCh10.delay(time);
//           if (led.getSequenceIndex() == 10) led.set(pin[pinLed1], LOW);
//           if (led.getSequenceIndex() == 10) led.set(pin[pinLed2], LOW);
//           taskCh10.delay(time);
//         }
//         timerLedTest = millis();
//       }
//     }
//   };

//   auto taskLed5Callback = [](void *pvParameter) -> void {  // ledWhite
//     static int ledIndexStart = size - 4;
//     static int ledIndexEnd = size - size + 4;
//     for (;;) {
//       if (led.getSequenceIndex() == 10) {
//         led.set(pin[ledIndexStart], HIGH);
//         led.set(pin[ledIndexStart - 1], HIGH);
//         led.set(pin[ledIndexEnd], LOW);
//         led.set(pin[ledIndexEnd - 1], LOW);
//       }
//       taskCh10.delay(time * 6);
//       if (led.getSequenceIndex() == 10) {
//         led.set(pin[ledIndexStart], LOW);
//         led.set(pin[ledIndexStart - 1], LOW);
//         led.set(pin[ledIndexEnd], HIGH);
//         led.set(pin[ledIndexEnd - 1], HIGH);
//       }
//       taskCh10.delay(time * 6);
//     }
//   };

//   auto taskLed6Callback = [](void *pvParameter) -> void {  // ledSein
//     static int ledIndexStart = size - 3;
//     static int ledIndexEnd = size - size + 3;
//     static uint32_t timerLedTest;
//     for (;;) {
//       if (led.getSequenceIndex() == 10) led.set(pin[ledIndexStart], HIGH);
//       if (led.getSequenceIndex() == 10) led.set(pin[ledIndexEnd - 1], HIGH);
//       taskCh10.delay(time * 10);
//       if (led.getSequenceIndex() == 10) led.set(pin[ledIndexStart], LOW);
//       if (led.getSequenceIndex() == 10) led.set(pin[ledIndexEnd - 1], LOW);
//       taskCh10.delay(time * 10);
//     }
//   };

//   auto taskLed7Callback = [](void *pvParameter) -> void {
//     static int ledIndexStart = size - 8;
//     static int ledIndexEnd = size - size + 8;
//     static uint32_t timerLedTest;
//     for (;;) {
//       for (int i = 0; i < 4; i++) {
//         if (led.getSequenceIndex() == 10) led.set(pin[ledIndexStart], HIGH);
//         if (led.getSequenceIndex() == 10) led.set(pin[ledIndexEnd - 1], LOW);
//         taskCh10.delay(time * 4);
//         if (led.getSequenceIndex() == 10) led.set(pin[ledIndexStart], LOW);
//         if (led.getSequenceIndex() == 10) led.set(pin[ledIndexEnd - 1], HIGH);
//         taskCh10.delay(time * 4);
//       }

//       for (int i = 0; i < 3; i++) {
//         for (int j = 0; j < 3; j++) {
//           if (led.getSequenceIndex() == 10) {
//             if (led.getSequenceIndex() == 10) led.set(pin[ledIndexStart], HIGH);
//             if (led.getSequenceIndex() == 10) led.set(pin[ledIndexEnd - 1], HIGH);
//           }
//           taskCh10.delay(time);
//           if (led.getSequenceIndex() == 10) {
//             if (led.getSequenceIndex() == 10) led.set(pin[ledIndexStart], LOW);
//             if (led.getSequenceIndex() == 10) led.set(pin[ledIndexEnd - 1], LOW);
//           }
//           taskCh10.delay(time);
//         }
//         taskCh10.delay(time * 7);
//       }
//       taskCh10.delay(time * 3);
//     }
//   };

//   static auto taskLed1 = taskCh10.createTask(1200, taskLed1Callback);
//   static auto taskLed2 = taskCh10.createTask(1200, taskLed2Callback);
//   static auto taskLed3 = taskCh10.createTask(1200, taskLed3Callback);  // ledBlue
//   static auto taskLed4 = taskCh10.createTask(1100, taskLed4Callback);  // ledWhite2
//   static auto taskLed5 = taskCh10.createTask(1100, taskLed5Callback);  // ledWhite
//   static auto taskLed6 = taskCh10.createTask(1100, taskLed6Callback);  // ledSein
//   static auto taskLed7 = taskCh10.createTask(1100, taskLed7Callback);  // ledSein
//   Serial.println("BaseChannel::taskSequence10() end");
// }