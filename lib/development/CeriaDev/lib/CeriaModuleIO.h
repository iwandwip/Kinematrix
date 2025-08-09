#ifndef CERIA_MODULE_IO_H
#define CERIA_MODULE_IO_H

#include "Arduino.h"
#include "PCF8574.h"

#ifndef P0
#define P0  0
#define P1  1
#define P2  2
#define P3  3
#define P4  4
#define P5  5
#define P6  6
#define P7  7
#endif

namespace CeriaDevOP {

#define COUNT_FALLING 0
#define COUNT_RISING 1
#define COUNT_BOTH 2

    enum IOPinMode {
        IO_INPUT = INPUT,
        IO_INPUT_PULLUP = INPUT_PULLUP,
        IO_OUTPUT = OUTPUT
    };

    enum ButtonEvent {
        BUTTON_NONE = 0,
        BUTTON_PRESSED,
        BUTTON_RELEASED,
        BUTTON_SINGLE_CLICK,
        BUTTON_DOUBLE_CLICK,
        BUTTON_TRIPLE_CLICK,
        BUTTON_LONG_PRESS,
        BUTTON_VERY_LONG_PRESS,
        BUTTON_EXTREMELY_LONG_PRESS,
        BUTTON_CLICK_AND_HOLD,
        BUTTON_RAPID_FIRE
    };

    struct ButtonTimings {
        unsigned long debounceTime = 50;
        unsigned long doubleClickWindow = 300;
        unsigned long tripleClickWindow = 400;
        unsigned long longPressThreshold = 1000;
        unsigned long veryLongPressThreshold = 2000;
        unsigned long extremelyLongPressThreshold = 5000;
        unsigned long rapidFireInterval = 100;
    };

    class CeriaModuleIO;

    class CeriaDigitalInput {
    private:
        uint8_t _pin;
        CeriaModuleIO *_ioModule;
        unsigned long _debounceTime;
        unsigned long _count;
        int _countMode;
        int _previousSteadyState;
        int _lastSteadyState;
        int _lastFlickerableState;
        unsigned long _lastDebounceTime;
        bool _pressedFlag;
        bool _releasedFlag;
        
        ButtonTimings _timings;
        unsigned long _pressStartTime;
        unsigned long _releaseTime;
        uint8_t _clickCount;
        unsigned long _firstClickTime;
        bool _waitingForMultiClick;
        bool _longPressTriggered;
        bool _veryLongPressTriggered;
        bool _extremelyLongPressTriggered;
        bool _rapidFireActive;
        unsigned long _lastRapidFireTime;
        ButtonEvent _lastEvent;

    public:
        CeriaDigitalInput(uint8_t pin);
        CeriaDigitalInput(uint8_t pin, CeriaModuleIO *module);

        void init(CeriaModuleIO *module);
        void setDebounceTime(unsigned long time);
        int getState() const;
        int getStateRaw() const;
        bool isPressed(unsigned long time);
        bool isPressed();
        bool isLongPressed(unsigned long time) const;
        bool isReleased();
        void setCountMode(int mode);
        unsigned long getCount() const;
        void resetCount();
        void resetState();
        void update();
        
        ButtonEvent getLastEvent();
        bool isSingleClick();
        bool isDoubleClick();
        bool isTripleClick();
        bool isLongPress();
        bool isVeryLongPress();
        bool isExtremelyLongPress();
        bool isClickAndHold();
        bool isRapidFire();
        
        void setButtonTimings(const ButtonTimings& timings);
        ButtonTimings getButtonTimings() const;
        void resetClickPattern();

    private:
        void processSmartButtonLogic(unsigned long currentTime);
        void handleButtonPressed(unsigned long currentTime);
        void handleButtonReleased(unsigned long currentTime);
        void processMultiClickDetection(unsigned long currentTime);
        void processLongPressDetection(unsigned long currentTime);
        void processRapidFireDetection(unsigned long currentTime);
    };

    class CeriaDigitalOutput {
    private:
        uint8_t _pin;
        CeriaModuleIO *_ioModule;
        unsigned long _onDelayTime;
        unsigned long _onDelayStart;
        bool _onDelayState;
        unsigned long _offDelayTime;
        unsigned long _offDelayStart;
        bool _offDelayState;
        bool _state;
        bool _reverse;
        unsigned long _toggleTime;

    public:
        CeriaDigitalOutput();
        CeriaDigitalOutput(uint8_t pin, CeriaModuleIO *module, bool isReverse = false);

        void init(CeriaModuleIO *module);
        void on();
        void off();
        void onToOffDelay(unsigned long delay_time);
        void offToOnDelay(unsigned long delay_time);
        void set(bool state);
        void toggle(void (*toggleCallback)() = nullptr);
        void toggleDelay(unsigned long delay_time, void (*toggleCallback)() = nullptr);
        void toggleInit(unsigned long delay_time = 100, int num = 1, void (*toggleCallback)() = nullptr);
        void toggleAsync(unsigned long delay_time = 100, void (*toggleCallback)(bool state) = nullptr);
        void onDelay(unsigned long delay_time);
        void offDelay(unsigned long delay_time);
        void update();
        void reverse();
        void setPin(uint8_t pin, bool isReverse = false);
        bool getState() const;
    };

    class CeriaModuleIO : public PCF8574 {
    private:
        CeriaDigitalInput *_inputs[8];
        CeriaDigitalOutput *_outputs[8];
        uint8_t _inputCount;
        uint8_t _outputCount;
        unsigned long _lastBatchUpdate;
        uint8_t _batchUpdateInterval;

    public:
        CeriaModuleIO(uint8_t address = 0x38);
        CeriaModuleIO(uint8_t address, uint8_t interruptPin, void (*interruptFunction)());

#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(TEENSYDUINO)
        CeriaModuleIO(uint8_t address, int sda, int scl);
        CeriaModuleIO(uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)());
#endif

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
        CeriaModuleIO(TwoWire *pWire, uint8_t address);
        CeriaModuleIO(TwoWire *pWire, uint8_t address, uint8_t interruptPin, void (*interruptFunction)());
#endif
#if defined(ESP32)
        CeriaModuleIO(TwoWire *pWire, uint8_t address, int sda, int scl);
        CeriaModuleIO(TwoWire *pWire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)());
#endif

        bool begin();
        bool begin(bool setupPins);
        void setupPins();

        CeriaDigitalInput *createInput(uint8_t pin, unsigned long debounceTime = 50, IOPinMode mode = IO_INPUT_PULLUP);
        CeriaDigitalOutput *createOutput(uint8_t pin, bool isReverse = false);

        void setupButton(uint8_t pin, unsigned long debounceTime = 50, IOPinMode mode = IO_INPUT_PULLUP);
        void setupLED(uint8_t pin, bool initialState = false, bool isReverse = false);

        bool isConnected(uint32_t timeoutMs = 5000);

        bool buttonPressed(uint8_t pin);
        bool buttonReleased(uint8_t pin);
        bool buttonLongPressed(uint8_t pin, unsigned long time = 1000);
        unsigned long buttonCount(uint8_t pin);
        void resetButtonCount(uint8_t pin);

        void ledOn(uint8_t pin);
        void ledOff(uint8_t pin);
        void ledToggle(uint8_t pin);
        void ledSet(uint8_t pin, bool state);
        void ledBlink(uint8_t pin, unsigned long interval = 500);
        void ledBlinkPattern(uint8_t pin, unsigned long onTime = 100, int count = 3);
        bool ledState(uint8_t pin);

        void updateAll();
        void setBatchUpdateInterval(uint8_t intervalMs);

        String getInputStatusString();
        String getOutputStatusString();
        String getIOStatusString();

        bool waitForButtonPress(uint8_t pin, uint32_t timeoutMs = 5000);
        bool waitForAnyButtonPress(uint32_t timeoutMs = 5000);

        void enableAutoUpdate(bool enable = true);
        void setInputCountMode(uint8_t pin, int mode);

        void setupEncoder(uint8_t pinA, uint8_t pinB);
        bool hasNewData();
        
        ButtonEvent getButtonEvent(uint8_t pin);
        bool buttonSingleClick(uint8_t pin);
        bool buttonDoubleClick(uint8_t pin);
        bool buttonTripleClick(uint8_t pin);
        bool buttonLongPress(uint8_t pin);
        bool buttonVeryLongPress(uint8_t pin);
        bool buttonExtremelyLongPress(uint8_t pin);
        bool buttonClickAndHold(uint8_t pin);
        bool buttonRapidFire(uint8_t pin);
        
        void setButtonTimings(uint8_t pin, const ButtonTimings& timings);
        ButtonTimings getButtonTimings(uint8_t pin);
        void resetButtonPattern(uint8_t pin);

        ~CeriaModuleIO();

    private:
        void initializeArrays();
        CeriaDigitalInput *findInputByPin(uint8_t pin);
        CeriaDigitalOutput *findOutputByPin(uint8_t pin);
        bool _autoUpdate;
        uint8_t _prePinModes[8];
        bool _prePinSetup[8];
        void applyPreSetupPins();
        unsigned long _lastUpdate;
        bool _hasNewData;
    };

}

#endif