#include "flowmeterv3-sens.h"
#include "Arduino.h"

FlowSensor::FlowSensor(uint16_t type, uint8_t pin) {
    this->_pin = pin;
    this->_pulse1liter = type;
    this->_interval = 1000;
    this->_pulse = 0;
    this->_totalpulse = 0;
    this->_volume = 0.0;
}

void FlowSensor::setType(uint16_t type) {
    this->_pulse1liter = type;
}

void FlowSensor::setPin(uint8_t pin) {
    this->_pin = pin;
}

uint8_t FlowSensor::getPin() {
    return this->_pin;
}

void FlowSensor::count() {
    this->_pulse++;
}

bool FlowSensor::_isTimeToRead() {
    return (millis() - this->_timebefore >= this->_interval);
}

void FlowSensor::setInterval(unsigned long interval) {
    this->_interval = interval;
}

bool FlowSensor::read(long calibration) {
    if (!_isTimeToRead()) return false;

    float timeInSeconds = this->_interval / 1000.0;
    float adjustedPulses = this->_pulse1liter + calibration;
    if (adjustedPulses <= 0) adjustedPulses = 1.0;

    this->_flowratesecound = (this->_pulse / adjustedPulses) / timeInSeconds;
    this->_volume += (this->_pulse / adjustedPulses);
    this->_totalpulse += this->_pulse;
    this->_pulse = 0;
    this->_timebefore = millis();
    return true;
}

unsigned long FlowSensor::getPulse() {
    return this->_totalpulse;
}

float FlowSensor::getFlowRate_h() {
    this->_flowratehour = this->_flowratesecound * 3600;
    return this->_flowratehour;
}

float FlowSensor::getFlowRate_m() {
    this->_flowrateminute = this->_flowratesecound * 60;
    return this->_flowrateminute;
}

float FlowSensor::getFlowRate_s() {
    return this->_flowratesecound;
}

float FlowSensor::getVolume() {
    return this->_volume;
}

void FlowSensor::resetPulse() {
    this->_pulse = 0;
    this->_totalpulse = 0;
}

void FlowSensor::resetVolume() {
    this->_volume = 0;
}

FlowSensorNode *FlowMeterV3Sens::sensorList = nullptr;

#if defined(PLATFORM_ARDUINO)
void IRAM_ATTR handleFlowSensorISR0() { FlowMeterV3Sens::handleInterrupt(0); }
void IRAM_ATTR handleFlowSensorISR1() { FlowMeterV3Sens::handleInterrupt(1); }
void IRAM_ATTR handleFlowSensorISR2() { FlowMeterV3Sens::handleInterrupt(2); }
void IRAM_ATTR handleFlowSensorISR3() { FlowMeterV3Sens::handleInterrupt(3); }
typedef void (*FlowISRFunction)();
const FlowISRFunction flowIsrFunctions[] = {
    handleFlowSensorISR0,
    handleFlowSensorISR1,
    handleFlowSensorISR2,
    handleFlowSensorISR3,
};

#elif defined(PLATFORM_ESP32)

void IRAM_ATTR FlowMeterV3Sens::isr(void *arg) {
    uint8_t pin = *((uint8_t *) arg);
    handleInterrupt(pin);
}

#elif defined(PLATFORM_ESP8266)
void ICACHE_RAM_ATTR handleFlowSensorESP8266_ISR0() { FlowMeterV3Sens::handleInterrupt(0); }
void ICACHE_RAM_ATTR handleFlowSensorESP8266_ISR1() { FlowMeterV3Sens::handleInterrupt(1); }
void ICACHE_RAM_ATTR handleFlowSensorESP8266_ISR2() { FlowMeterV3Sens::handleInterrupt(2); }
void ICACHE_RAM_ATTR handleFlowSensorESP8266_ISR3() { FlowMeterV3Sens::handleInterrupt(3); }
const FlowISRFunction flowEsp8266IsrFunctions[] = {
    handleFlowSensorESP8266_ISR0,
    handleFlowSensorESP8266_ISR1,
    handleFlowSensorESP8266_ISR2,
    handleFlowSensorESP8266_ISR3,
};
#endif

void FlowMeterV3Sens::handleInterrupt(uint8_t pin) {
    FlowSensorNode *current = sensorList;
    while (current != nullptr) {
        if (current->pin == pin) {
            current->instance->readInterrupt();
            return;
        }
        current = current->next;
    }
}

bool FlowMeterV3Sens::registerInstance() {
    FlowSensorNode *current = sensorList;
    while (current != nullptr) {
        if (current->pin == sensorPin) {
            current->instance = this;
            return true;
        }
        current = current->next;
    }

    FlowSensorNode *newNode = new FlowSensorNode(sensorPin, this);

    if (sensorList == nullptr) {
        sensorList = newNode;
    } else {
        newNode->next = sensorList;
        sensorList = newNode;
    }

    return true;
}

FlowMeterV3Sens::FlowMeterV3Sens(uint16_t type, uint8_t pin, uint32_t interval, bool pullup, InterruptCallback callback)
        : FlowSensor(type, pin) {

    sensorPin = pin;
    usePullup = pullup;
    userCallback = callback;

    if (usePullup) {
        pinMode(pin, INPUT_PULLUP);
    } else {
        pinMode(pin, INPUT);
    }

    FlowSensor::setInterval(interval);
    FlowSensor::resetVolume();

    if (userCallback == nullptr) {
        registerInstance();

#if defined(PLATFORM_ARDUINO)
        if (pin < sizeof(flowIsrFunctions)/sizeof(flowIsrFunctions[0])) {
                attachInterrupt(digitalPinToInterrupt(pin), flowIsrFunctions[pin], RISING);
            }
#elif defined(PLATFORM_ESP32)
        attachInterruptArg(digitalPinToInterrupt(pin), isr, &sensorPin, RISING);
#elif defined(PLATFORM_ESP8266)
        if (pin < sizeof(flowEsp8266IsrFunctions)/sizeof(flowEsp8266IsrFunctions[0])) {
                attachInterrupt(digitalPinToInterrupt(pin), flowEsp8266IsrFunctions[pin], RISING);
            }
#endif
    } else {
        attachInterrupt(digitalPinToInterrupt(pin), userCallback, RISING);
    }
}

FlowMeterV3Sens::~FlowMeterV3Sens() {
#if defined(PLATFORM_ARDUINO) || defined(PLATFORM_ESP8266) || defined(PLATFORM_ESP32)
    detachInterrupt(digitalPinToInterrupt(sensorPin));
#endif

    if (userCallback == nullptr) {
        FlowSensorNode *current = sensorList;
        FlowSensorNode *prev = nullptr;

        while (current != nullptr) {
            if (current->pin == sensorPin && current->instance == this) {
                if (prev == nullptr) {
                    sensorList = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                break;
            }
            prev = current;
            current = current->next;
        }
    }
}

bool FlowMeterV3Sens::init() {
    if (doc == nullptr) {
        doc = new JsonDocument;
    }

    if (name == nullptr || strcmp(name, "") == 0) {
        name = "FlowMeterV3Sens";
    }

    (*doc)[name]["rate"] = 0.0;
    (*doc)[name]["volume"] = 0.0;
    return true;
}

bool FlowMeterV3Sens::update() {
    FlowSensor::read();
    (*doc)[name]["rate"] = FlowSensor::getFlowRate_m();
    (*doc)[name]["volume"] = FlowSensor::getVolume();
    return true;
}

void FlowMeterV3Sens::setDocument(const char *objName) {
    name = objName;
}

void FlowMeterV3Sens::setDocumentValue(JsonDocument *docBase) {
    doc = docBase;
}

JsonDocument FlowMeterV3Sens::getDocument() {
    return (*doc);
}

JsonVariant FlowMeterV3Sens::getVariant(const char *searchName) {
    return (*doc)[searchName];
}

float FlowMeterV3Sens::getValueFlowMeterV3Sens() const {
    return (*doc)[name]["rate"].as<float>();
}

void FlowMeterV3Sens::setPins(uint8_t _pin) {
#if defined(PLATFORM_ARDUINO) || defined(PLATFORM_ESP8266) || defined(PLATFORM_ESP32)
    detachInterrupt(digitalPinToInterrupt(sensorPin));
#endif

    if (userCallback == nullptr) {
        FlowSensorNode *current = sensorList;
        FlowSensorNode *prev = nullptr;

        while (current != nullptr) {
            if (current->pin == sensorPin && current->instance == this) {
                if (prev == nullptr) {
                    sensorList = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                break;
            }
            prev = current;
            current = current->next;
        }
    }

    sensorPin = _pin;
    FlowSensor::setPin(_pin);

    if (usePullup) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }

    if (userCallback == nullptr) {
        registerInstance();

#if defined(PLATFORM_ARDUINO)
        if (_pin < sizeof(flowIsrFunctions)/sizeof(flowIsrFunctions[0])) {
                attachInterrupt(digitalPinToInterrupt(_pin), flowIsrFunctions[_pin], RISING);
            }
#elif defined(PLATFORM_ESP32)
        attachInterruptArg(digitalPinToInterrupt(_pin), isr, &sensorPin, RISING);
#elif defined(PLATFORM_ESP8266)
        if (_pin < sizeof(flowEsp8266IsrFunctions)/sizeof(flowEsp8266IsrFunctions[0])) {
                attachInterrupt(digitalPinToInterrupt(_pin), flowEsp8266IsrFunctions[_pin], RISING);
            }
#endif
    } else {
        attachInterrupt(digitalPinToInterrupt(_pin), userCallback, RISING);
    }
}

void FlowMeterV3Sens::setPullup(bool pullup) {
    usePullup = pullup;

    if (usePullup) {
        pinMode(sensorPin, INPUT_PULLUP);
    } else {
        pinMode(sensorPin, INPUT);
    }
}

void FlowMeterV3Sens::setCallback(InterruptCallback callback) {
    detachInterrupt(digitalPinToInterrupt(sensorPin));

    if (userCallback == nullptr && callback != nullptr) {
        FlowSensorNode *current = sensorList;
        FlowSensorNode *prev = nullptr;

        while (current != nullptr) {
            if (current->pin == sensorPin && current->instance == this) {
                if (prev == nullptr) {
                    sensorList = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                break;
            }
            prev = current;
            current = current->next;
        }
    } else if (userCallback != nullptr && callback == nullptr) {
        registerInstance();
    }

    userCallback = callback;

    if (userCallback == nullptr) {
#if defined(PLATFORM_ARDUINO)
        if (sensorPin < sizeof(flowIsrFunctions)/sizeof(flowIsrFunctions[0])) {
                attachInterrupt(digitalPinToInterrupt(sensorPin), flowIsrFunctions[sensorPin], RISING);
            }
#elif defined(PLATFORM_ESP32)
        attachInterruptArg(digitalPinToInterrupt(sensorPin), isr, &sensorPin, RISING);
#elif defined(PLATFORM_ESP8266)
        if (sensorPin < sizeof(flowEsp8266IsrFunctions)/sizeof(flowEsp8266IsrFunctions[0])) {
                attachInterrupt(digitalPinToInterrupt(sensorPin), flowEsp8266IsrFunctions[sensorPin], RISING);
            }
#endif
    } else {
        attachInterrupt(digitalPinToInterrupt(sensorPin), userCallback, RISING);
    }
}

void FlowMeterV3Sens::readInterrupt() {
    FlowSensor::count();
}