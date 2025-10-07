#include "CeriaModuleIO.h"

namespace CeriaDevOP {

    CeriaDigitalInput::CeriaDigitalInput(uint8_t pin) {
        _pin = pin;
        _ioModule = nullptr;
        _debounceTime = 50;
        _count = 0;
        _countMode = COUNT_FALLING;
        _lastDebounceTime = 0;
        _previousSteadyState = HIGH;
        _lastSteadyState = HIGH;
        _lastFlickerableState = HIGH;
        _pressedFlag = false;
        _releasedFlag = false;
        
        _pressStartTime = 0;
        _releaseTime = 0;
        _clickCount = 0;
        _firstClickTime = 0;
        _waitingForMultiClick = false;
        _longPressTriggered = false;
        _veryLongPressTriggered = false;
        _extremelyLongPressTriggered = false;
        _rapidFireActive = false;
        _lastRapidFireTime = 0;
        _lastEvent = BUTTON_NONE;
        
        _timings.debounceTime = 50;
        _timings.doubleClickWindow = 300;
        _timings.tripleClickWindow = 500;
        _timings.longPressThreshold = 1000;
        _timings.veryLongPressThreshold = 2000;
        _timings.extremelyLongPressThreshold = 5000;
        _timings.rapidFireInterval = 100;
    }

    CeriaDigitalInput::CeriaDigitalInput(uint8_t pin, CeriaModuleIO *module) {
        _pin = pin;
        _ioModule = module;
        _debounceTime = 50;
        _count = 0;
        _countMode = COUNT_FALLING;
        _lastDebounceTime = 0;
        _previousSteadyState = HIGH;
        _lastSteadyState = HIGH;
        _lastFlickerableState = HIGH;
        _pressedFlag = false;
        _releasedFlag = false;
        
        _pressStartTime = 0;
        _releaseTime = 0;
        _clickCount = 0;
        _firstClickTime = 0;
        _waitingForMultiClick = false;
        _longPressTriggered = false;
        _veryLongPressTriggered = false;
        _extremelyLongPressTriggered = false;
        _rapidFireActive = false;
        _lastRapidFireTime = 0;
        _lastEvent = BUTTON_NONE;
        
        _timings.debounceTime = 50;
        _timings.doubleClickWindow = 300;
        _timings.tripleClickWindow = 500;
        _timings.longPressThreshold = 1000;
        _timings.veryLongPressThreshold = 2000;
        _timings.extremelyLongPressThreshold = 5000;
        _timings.rapidFireInterval = 100;
    }

    void CeriaDigitalInput::init(CeriaModuleIO *module) {
        _ioModule = module;
        if (_ioModule) {
            _previousSteadyState = getStateRaw();
            _lastSteadyState = _previousSteadyState;
            _lastFlickerableState = _previousSteadyState;
        }
    }

    void CeriaDigitalInput::setDebounceTime(unsigned long time) {
        _debounceTime = time;
    }

    int CeriaDigitalInput::getState() const {
        return _lastSteadyState;
    }

    int CeriaDigitalInput::getStateRaw() const {
        if (_ioModule) {
            return _ioModule->digitalRead(_pin);
        }
        return HIGH;
    }

    bool CeriaDigitalInput::isPressed(unsigned long time) {
        if (millis() - _debounceTime >= time) {
            _debounceTime = millis();
            return true;
        }
        return false;
    }

    bool CeriaDigitalInput::isPressed() {
        if (_pressedFlag) {
            _pressedFlag = false;
            return true;
        }
        return false;
    }

    bool CeriaDigitalInput::isLongPressed(unsigned long time) const {
        if (_lastSteadyState == LOW && (millis() - _lastDebounceTime) >= time)
            return true;
        else
            return false;
    }

    bool CeriaDigitalInput::isReleased() {
        if (_releasedFlag) {
            _releasedFlag = false;
            return true;
        }
        return false;
    }

    void CeriaDigitalInput::setCountMode(int mode) {
        _countMode = mode;
    }

    unsigned long CeriaDigitalInput::getCount() const {
        return _count;
    }

    void CeriaDigitalInput::resetCount() {
        _count = 0;
    }

    void CeriaDigitalInput::resetState() {
        _previousSteadyState = HIGH;
        _lastSteadyState = HIGH;
        _lastDebounceTime = millis();
    }

    void CeriaDigitalInput::update() {
        int currentState = getStateRaw();
        unsigned long currentTime = millis();

        if (currentState != _lastFlickerableState) {
            _lastDebounceTime = currentTime;
            _lastFlickerableState = currentState;
        }

        if ((currentTime - _lastDebounceTime) >= _timings.debounceTime) {
            _previousSteadyState = _lastSteadyState;
            _lastSteadyState = currentState;
        }

        // Smart button logic processing
        processSmartButtonLogic(currentTime);

        if (_previousSteadyState != _lastSteadyState) {
            // Set edge detection flags
            if (_previousSteadyState == HIGH && _lastSteadyState == LOW) {
                _pressedFlag = true;
                handleButtonPressed(currentTime);
            }
            if (_previousSteadyState == LOW && _lastSteadyState == HIGH) {
                _releasedFlag = true;
                handleButtonReleased(currentTime);
            }
            
            // Count events
            if (_countMode == COUNT_BOTH)
                _count++;
            else if (_countMode == COUNT_FALLING) {
                if (_previousSteadyState == HIGH && _lastSteadyState == LOW)
                    _count++;
            } else if (_countMode == COUNT_RISING) {
                if (_previousSteadyState == LOW && _lastSteadyState == HIGH)
                    _count++;
            }
        }
    }

    CeriaDigitalOutput::CeriaDigitalOutput() {
        _pin = 0;
        _ioModule = nullptr;
        _state = false;
        _onDelayTime = 0;
        _onDelayStart = 0;
        _onDelayState = false;
        _offDelayTime = 0;
        _offDelayStart = 0;
        _offDelayState = false;
        _reverse = false;
        _toggleTime = 0;
    }

    CeriaDigitalOutput::CeriaDigitalOutput(uint8_t pin, CeriaModuleIO *module, bool isReverse) {
        _pin = pin;
        _ioModule = module;
        _reverse = isReverse;
        _state = false;
        _onDelayTime = 0;
        _onDelayStart = 0;
        _onDelayState = false;
        _offDelayTime = 0;
        _offDelayStart = 0;
        _offDelayState = false;
        _toggleTime = 0;
        off();
    }

    void CeriaDigitalOutput::init(CeriaModuleIO *module) {
        _ioModule = module;
        off();
    }

    void CeriaDigitalOutput::on() {
        if (_ioModule) {
            if (_reverse)
                _ioModule->digitalWrite(_pin, LOW);
            else
                _ioModule->digitalWrite(_pin, HIGH);
            _state = true;
        }
    }

    void CeriaDigitalOutput::off() {
        if (_ioModule) {
            if (_reverse)
                _ioModule->digitalWrite(_pin, HIGH);
            else
                _ioModule->digitalWrite(_pin, LOW);
            _state = false;
        }
    }

    void CeriaDigitalOutput::onToOffDelay(unsigned long delay_time) {
        on();
        delay(delay_time);
        off();
    }

    void CeriaDigitalOutput::offToOnDelay(unsigned long delay_time) {
        off();
        delay(delay_time);
        on();
    }

    void CeriaDigitalOutput::set(bool state) {
        if (state) {
            on();
        } else {
            off();
        }
    }

    void CeriaDigitalOutput::toggle(void (*toggleCallback)()) {
        if (toggleCallback != nullptr) toggleCallback();
        set(!_state);
    }

    void CeriaDigitalOutput::toggleDelay(unsigned long delay_time, void (*toggleCallback)()) {
        if (toggleCallback != nullptr) toggleCallback();
        set(!_state);
        delay(delay_time);
    }

    void CeriaDigitalOutput::toggleInit(unsigned long delay_time, int num, void (*toggleCallback)()) {
        for (int i = 0; i < num; i++) {
            if (toggleCallback != nullptr) toggleCallback();
            on();
            delay(delay_time);
            off();
            delay(delay_time);
        }
    }

    void CeriaDigitalOutput::toggleAsync(unsigned long delay_time, void (*toggleCallback)(bool state)) {
        if (millis() - _toggleTime >= delay_time) {
            set(!_state);
            if (toggleCallback != nullptr) toggleCallback(_state);
            _toggleTime = millis();
        }
    }

    void CeriaDigitalOutput::onDelay(unsigned long delay_time) {
        _onDelayTime = delay_time;
        _onDelayStart = millis();
        _onDelayState = true;
    }

    void CeriaDigitalOutput::offDelay(unsigned long delay_time) {
        _offDelayTime = delay_time;
        _offDelayStart = millis();
        _offDelayState = true;
    }

    void CeriaDigitalOutput::update() {
        if (_onDelayState) {
            unsigned long currentMillisOn = millis();
            if (currentMillisOn - _onDelayStart >= _onDelayTime) {
                on();
                _onDelayState = false;
            }
        }
        if (_offDelayState) {
            unsigned long currentMillisOff = millis();
            if (currentMillisOff - _offDelayStart >= _offDelayTime) {
                off();
                _offDelayState = false;
            }
        }
    }

    void CeriaDigitalOutput::reverse() {
        _reverse = true;
    }

    void CeriaDigitalOutput::setPin(uint8_t pin, bool isReverse) {
        _reverse = isReverse;
        _pin = pin;
        off();
    }

    bool CeriaDigitalOutput::getState() const {
        return _state;
    }

    CeriaModuleIO::CeriaModuleIO(uint8_t address) : PCF8574(address) {
        initializeArrays();
    }

    CeriaModuleIO::CeriaModuleIO(uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(address, interruptPin, interruptFunction) {
        initializeArrays();
    }

#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(TEENSYDUINO)

    CeriaModuleIO::CeriaModuleIO(uint8_t address, int sda, int scl) : PCF8574(address, sda, scl) {
        initializeArrays();
    }

    CeriaModuleIO::CeriaModuleIO(uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(address, sda, scl, interruptPin, interruptFunction) {
        initializeArrays();
    }

#endif

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)

    CeriaModuleIO::CeriaModuleIO(TwoWire *pWire, uint8_t address) : PCF8574(pWire, address) {
        initializeArrays();
    }

    CeriaModuleIO::CeriaModuleIO(TwoWire *pWire, uint8_t address, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(pWire, address, interruptPin, interruptFunction) {
        initializeArrays();
    }

#endif

#if defined(ESP32)

    CeriaModuleIO::CeriaModuleIO(TwoWire *pWire, uint8_t address, int sda, int scl)
            : PCF8574(pWire, address, sda, scl) {
        initializeArrays();
    }

    CeriaModuleIO::CeriaModuleIO(TwoWire *pWire, uint8_t address, int sda, int scl, uint8_t interruptPin, void (*interruptFunction)())
            : PCF8574(pWire, address, sda, scl, interruptPin, interruptFunction) {
        initializeArrays();
    }

#endif

    void CeriaModuleIO::initializeArrays() {
        for (int i = 0; i < 8; i++) {
            _inputs[i] = nullptr;
            _outputs[i] = nullptr;
            _prePinModes[i] = INPUT_PULLUP;
            _prePinSetup[i] = false;
        }
        _inputCount = 0;
        _outputCount = 0;
        _lastBatchUpdate = 0;
        _batchUpdateInterval = 10;
        _autoUpdate = false;
        _lastUpdate = 0;
        _hasNewData = false;
    }

    bool CeriaModuleIO::begin() {
        setupPins();
        bool success = PCF8574::begin();
        
        if (success) {
            // Initialize all created objects after PCF8574 is ready
            for (int i = 0; i < 8; i++) {
                if (_inputs[i] != nullptr) {
                    _inputs[i]->init(this);
                }
                if (_outputs[i] != nullptr) {
                    _outputs[i]->init(this);
                }
            }
        }
        
        return success;
    }

    bool CeriaModuleIO::begin(bool setupPins) {
        if (setupPins) {
            this->setupPins();
        }
        bool success = PCF8574::begin();
        
        if (success) {
            // Initialize all created objects after PCF8574 is ready
            for (int i = 0; i < 8; i++) {
                if (_inputs[i] != nullptr) {
                    _inputs[i]->init(this);
                }
                if (_outputs[i] != nullptr) {
                    _outputs[i]->init(this);
                }
            }
        }
        
        return success;
    }

    void CeriaModuleIO::setupPins() {
        applyPreSetupPins();
    }

    void CeriaModuleIO::applyPreSetupPins() {
        for (int i = 0; i < 8; i++) {
            if (_prePinSetup[i]) {
                PCF8574::pinMode(i, _prePinModes[i]);
            }
        }
    }

    CeriaDigitalInput *CeriaModuleIO::createInput(uint8_t pin, unsigned long debounceTime, IOPinMode mode) {
        if (pin >= 8) return nullptr;

        if (_inputs[pin] == nullptr) {
            if (mode == IO_INPUT) {
                _prePinModes[pin] = INPUT;
            } else {
                _prePinModes[pin] = INPUT_PULLUP;
            }
            _prePinSetup[pin] = true;
            
            _inputs[pin] = new CeriaDigitalInput(pin, this);
            _inputs[pin]->setDebounceTime(debounceTime);
            _inputCount++;
        }
        return _inputs[pin];
    }

    CeriaDigitalOutput *CeriaModuleIO::createOutput(uint8_t pin, bool isReverse) {
        if (pin >= 8) return nullptr;

        if (_outputs[pin] == nullptr) {
            _prePinModes[pin] = OUTPUT;
            _prePinSetup[pin] = true;
            
            _outputs[pin] = new CeriaDigitalOutput(pin, this, isReverse);
            _outputCount++;
        }
        return _outputs[pin];
    }

    void CeriaModuleIO::setupButton(uint8_t pin, unsigned long debounceTime, IOPinMode mode) {
        createInput(pin, debounceTime, mode);
    }

    void CeriaModuleIO::setupLED(uint8_t pin, bool initialState, bool isReverse) {
        CeriaDigitalOutput *output = createOutput(pin, isReverse);
        if (output) {
            output->set(initialState);
        }
    }

    bool CeriaModuleIO::buttonPressed(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isPressed() : false;
    }

    bool CeriaModuleIO::buttonReleased(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isReleased() : false;
    }

    bool CeriaModuleIO::buttonLongPressed(uint8_t pin, unsigned long time) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isLongPressed(time) : false;
    }

    unsigned long CeriaModuleIO::buttonCount(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->getCount() : 0;
    }

    void CeriaModuleIO::resetButtonCount(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        if (input) input->resetCount();
    }

    void CeriaModuleIO::ledOn(uint8_t pin) {
        CeriaDigitalOutput *output = findOutputByPin(pin);
        if (output) output->on();
    }

    void CeriaModuleIO::ledOff(uint8_t pin) {
        CeriaDigitalOutput *output = findOutputByPin(pin);
        if (output) output->off();
    }

    void CeriaModuleIO::ledToggle(uint8_t pin) {
        CeriaDigitalOutput *output = findOutputByPin(pin);
        if (output) output->toggle();
    }

    void CeriaModuleIO::ledSet(uint8_t pin, bool state) {
        CeriaDigitalOutput *output = findOutputByPin(pin);
        if (output) output->set(state);
    }

    void CeriaModuleIO::ledBlink(uint8_t pin, unsigned long interval) {
        CeriaDigitalOutput *output = findOutputByPin(pin);
        if (output) output->toggleAsync(interval);
    }

    void CeriaModuleIO::ledBlinkPattern(uint8_t pin, unsigned long onTime, int count) {
        CeriaDigitalOutput *output = findOutputByPin(pin);
        if (output) output->toggleInit(onTime, count);
    }

    bool CeriaModuleIO::ledState(uint8_t pin) {
        CeriaDigitalOutput *output = findOutputByPin(pin);
        return output ? output->getState() : false;
    }

    void CeriaModuleIO::updateAll() {
        _hasNewData = false;
        unsigned long currentTime = millis();
        
        for (int i = 0; i < 8; i++) {
            if (_inputs[i] != nullptr) {
                _inputs[i]->update();
                _hasNewData = true;
            }
            if (_outputs[i] != nullptr) {
                _outputs[i]->update();
            }
        }

        _lastBatchUpdate = currentTime;
        _lastUpdate = currentTime;
    }

    void CeriaModuleIO::setBatchUpdateInterval(uint8_t intervalMs) {
        _batchUpdateInterval = intervalMs;
    }

    String CeriaModuleIO::getInputStatusString() {
        String result = "Inputs: ";
        for (int i = 0; i < 8; i++) {
            if (_inputs[i] != nullptr) {
                result += "P" + String(i) + ":" + (_inputs[i]->getState() == HIGH ? "H" : "L") + " ";
            }
        }
        return result;
    }

    String CeriaModuleIO::getOutputStatusString() {
        String result = "Outputs: ";
        for (int i = 0; i < 8; i++) {
            if (_outputs[i] != nullptr) {
                result += "P" + String(i) + ":" + (_outputs[i]->getState() ? "ON" : "OFF") + " ";
            }
        }
        return result;
    }

    String CeriaModuleIO::getIOStatusString() {
        if (getTransmissionStatusCode() != 0) return "CeriaModuleIO Error";

        String result = "IO Status - ";
        result += "Inputs(" + String(_inputCount) + "): ";
        for (int i = 0; i < 8; i++) {
            if (_inputs[i] != nullptr) {
                result += String(i) + ":" + (_inputs[i]->getState() == HIGH ? "H" : "L") + " ";
            }
        }
        result += ", Outputs(" + String(_outputCount) + "): ";
        for (int i = 0; i < 8; i++) {
            if (_outputs[i] != nullptr) {
                result += String(i) + ":" + (_outputs[i]->getState() ? "1" : "0") + " ";
            }
        }
        return result;
    }

    bool CeriaModuleIO::waitForButtonPress(uint8_t pin, uint32_t timeoutMs) {
        uint32_t startTime = millis();
        CeriaDigitalInput *input = findInputByPin(pin);
        if (!input) return false;

        while (millis() - startTime < timeoutMs) {
            updateAll();
            if (input->isPressed()) {
                return true;
            }
            delay(1);
        }
        return false;
    }

    bool CeriaModuleIO::waitForAnyButtonPress(uint32_t timeoutMs) {
        uint32_t startTime = millis();

        while (millis() - startTime < timeoutMs) {
            updateAll();
            for (int i = 0; i < 8; i++) {
                if (_inputs[i] != nullptr && _inputs[i]->isPressed()) {
                    return true;
                }
            }
            delay(1);
        }
        return false;
    }

    void CeriaModuleIO::enableAutoUpdate(bool enable) {
        _autoUpdate = enable;
    }

    void CeriaModuleIO::setInputCountMode(uint8_t pin, int mode) {
        CeriaDigitalInput *input = findInputByPin(pin);
        if (input) input->setCountMode(mode);
    }

    CeriaDigitalInput *CeriaModuleIO::findInputByPin(uint8_t pin) {
        if (pin >= 8) return nullptr;
        return _inputs[pin];
    }

    CeriaDigitalOutput *CeriaModuleIO::findOutputByPin(uint8_t pin) {
        if (pin >= 8) return nullptr;
        return _outputs[pin];
    }

    bool CeriaModuleIO::isConnected(uint32_t timeoutMs) {
        return PCF8574::isLastTransmissionSuccess();
    }

    void CeriaModuleIO::setupEncoder(uint8_t pinA, uint8_t pinB) {
        PCF8574::encoder(pinA, pinB);
    }

    bool CeriaModuleIO::hasNewData() {
        return _hasNewData;
    }

    // Smart Button Logic Implementation for CeriaDigitalInput
    void CeriaDigitalInput::processSmartButtonLogic(unsigned long currentTime) {
        processMultiClickDetection(currentTime);
        processLongPressDetection(currentTime);
        processRapidFireDetection(currentTime);
    }

    void CeriaDigitalInput::handleButtonPressed(unsigned long currentTime) {
        _pressStartTime = currentTime;
        _longPressTriggered = false;
        _veryLongPressTriggered = false;
        _extremelyLongPressTriggered = false;
        _lastEvent = BUTTON_PRESSED;

        if (_clickCount == 0) {
            _firstClickTime = currentTime;
        }
        _clickCount++;
        _waitingForMultiClick = true;
    }

    void CeriaDigitalInput::handleButtonReleased(unsigned long currentTime) {
        _releaseTime = currentTime;
        _rapidFireActive = false;
        _lastEvent = BUTTON_RELEASED;

        unsigned long pressDuration = currentTime - _pressStartTime;
        
        // Handle click and hold
        if (pressDuration >= _timings.veryLongPressThreshold) {
            _lastEvent = BUTTON_CLICK_AND_HOLD;
        }
    }

    void CeriaDigitalInput::processMultiClickDetection(unsigned long currentTime) {
        if (!_waitingForMultiClick || _clickCount == 0) return;

        unsigned long timeSinceFirstClick = currentTime - _firstClickTime;

        if (_lastSteadyState == HIGH) { // Button released
            if (_clickCount == 1) {
                if (timeSinceFirstClick > _timings.doubleClickWindow) {
                    _lastEvent = BUTTON_SINGLE_CLICK;
                    _waitingForMultiClick = false;
                    _clickCount = 0;
                }
            } else if (_clickCount == 2) {
                if (timeSinceFirstClick <= _timings.doubleClickWindow) {
                    _lastEvent = BUTTON_DOUBLE_CLICK;
                    _waitingForMultiClick = false;
                    _clickCount = 0;
                } else if (timeSinceFirstClick > _timings.tripleClickWindow) {
                    _lastEvent = BUTTON_SINGLE_CLICK;
                    _waitingForMultiClick = false;
                    _clickCount = 0;
                }
            } else if (_clickCount == 3) {
                if (timeSinceFirstClick <= _timings.tripleClickWindow) {
                    _lastEvent = BUTTON_TRIPLE_CLICK;
                    _waitingForMultiClick = false;
                    _clickCount = 0;
                } else {
                    _lastEvent = BUTTON_SINGLE_CLICK;
                    _waitingForMultiClick = false;
                    _clickCount = 0;
                }
            } else if (timeSinceFirstClick > _timings.tripleClickWindow) {
                _lastEvent = BUTTON_SINGLE_CLICK;
                _waitingForMultiClick = false;
                _clickCount = 0;
            }
        }
    }

    void CeriaDigitalInput::processLongPressDetection(unsigned long currentTime) {
        if (_lastSteadyState != LOW || _pressStartTime == 0) return;

        unsigned long pressDuration = currentTime - _pressStartTime;

        if (!_longPressTriggered && pressDuration >= _timings.longPressThreshold) {
            _lastEvent = BUTTON_LONG_PRESS;
            _longPressTriggered = true;
            _waitingForMultiClick = false;
            _clickCount = 0;
        } else if (!_veryLongPressTriggered && pressDuration >= _timings.veryLongPressThreshold) {
            _lastEvent = BUTTON_VERY_LONG_PRESS;
            _veryLongPressTriggered = true;
        } else if (!_extremelyLongPressTriggered && pressDuration >= _timings.extremelyLongPressThreshold) {
            _lastEvent = BUTTON_EXTREMELY_LONG_PRESS;
            _extremelyLongPressTriggered = true;
        }
    }

    void CeriaDigitalInput::processRapidFireDetection(unsigned long currentTime) {
        if (_lastSteadyState != LOW || !_longPressTriggered) return;

        if (!_rapidFireActive) {
            _rapidFireActive = true;
            _lastRapidFireTime = currentTime;
            _lastEvent = BUTTON_RAPID_FIRE;
        } else if (currentTime - _lastRapidFireTime >= _timings.rapidFireInterval) {
            _lastEvent = BUTTON_RAPID_FIRE;
            _lastRapidFireTime = currentTime;
        }
    }

    // Smart Button Detection Methods
    ButtonEvent CeriaDigitalInput::getLastEvent() {
        ButtonEvent event = _lastEvent;
        if (event != BUTTON_NONE && event != BUTTON_PRESSED && event != BUTTON_RELEASED && event != BUTTON_RAPID_FIRE) {
            _lastEvent = BUTTON_NONE; // Clear one-time events
        }
        return event;
    }

    bool CeriaDigitalInput::isSingleClick() {
        return (getLastEvent() == BUTTON_SINGLE_CLICK);
    }

    bool CeriaDigitalInput::isDoubleClick() {
        return (getLastEvent() == BUTTON_DOUBLE_CLICK);
    }

    bool CeriaDigitalInput::isTripleClick() {
        return (getLastEvent() == BUTTON_TRIPLE_CLICK);
    }

    bool CeriaDigitalInput::isLongPress() {
        return (getLastEvent() == BUTTON_LONG_PRESS);
    }

    bool CeriaDigitalInput::isVeryLongPress() {
        return (getLastEvent() == BUTTON_VERY_LONG_PRESS);
    }

    bool CeriaDigitalInput::isExtremelyLongPress() {
        return (getLastEvent() == BUTTON_EXTREMELY_LONG_PRESS);
    }

    bool CeriaDigitalInput::isClickAndHold() {
        return (getLastEvent() == BUTTON_CLICK_AND_HOLD);
    }

    bool CeriaDigitalInput::isRapidFire() {
        return (_lastEvent == BUTTON_RAPID_FIRE); // Don't clear this as it repeats
    }

    void CeriaDigitalInput::setButtonTimings(const ButtonTimings& timings) {
        _timings = timings;
        _debounceTime = timings.debounceTime; // Keep legacy support
    }

    ButtonTimings CeriaDigitalInput::getButtonTimings() const {
        return _timings;
    }

    void CeriaDigitalInput::resetClickPattern() {
        _clickCount = 0;
        _firstClickTime = 0;
        _waitingForMultiClick = false;
        _longPressTriggered = false;
        _veryLongPressTriggered = false;
        _extremelyLongPressTriggered = false;
        _rapidFireActive = false;
        _lastEvent = BUTTON_NONE;
    }

    // CeriaModuleIO Smart Button Wrapper Methods
    ButtonEvent CeriaModuleIO::getButtonEvent(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->getLastEvent() : BUTTON_NONE;
    }

    bool CeriaModuleIO::buttonSingleClick(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isSingleClick() : false;
    }

    bool CeriaModuleIO::buttonDoubleClick(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isDoubleClick() : false;
    }

    bool CeriaModuleIO::buttonTripleClick(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isTripleClick() : false;
    }

    bool CeriaModuleIO::buttonLongPress(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isLongPress() : false;
    }

    bool CeriaModuleIO::buttonVeryLongPress(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isVeryLongPress() : false;
    }

    bool CeriaModuleIO::buttonExtremelyLongPress(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isExtremelyLongPress() : false;
    }

    bool CeriaModuleIO::buttonClickAndHold(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isClickAndHold() : false;
    }

    bool CeriaModuleIO::buttonRapidFire(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->isRapidFire() : false;
    }

    void CeriaModuleIO::setButtonTimings(uint8_t pin, const ButtonTimings& timings) {
        CeriaDigitalInput *input = findInputByPin(pin);
        if (input) input->setButtonTimings(timings);
    }

    ButtonTimings CeriaModuleIO::getButtonTimings(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        return input ? input->getButtonTimings() : ButtonTimings();
    }

    void CeriaModuleIO::resetButtonPattern(uint8_t pin) {
        CeriaDigitalInput *input = findInputByPin(pin);
        if (input) input->resetClickPattern();
    }

    CeriaModuleIO::~CeriaModuleIO() {
        for (int i = 0; i < 8; i++) {
            delete _inputs[i];
            delete _outputs[i];
        }
    }

}