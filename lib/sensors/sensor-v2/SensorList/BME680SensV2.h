#pragma once

#ifndef BME680_SENS_V2
#define BME680_SENS_V2

#pragma message("[COMPILED]: BME680SensV2.h")

#include "Arduino.h"
#include "../SensorModuleV2.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BME680.h"

class BME680SensV2 : public BaseSensV2, public Adafruit_BME680 {
private:
    uint32_t _updateTimer;
    uint32_t _updateInterval;

    void initCustomValue();

    using Adafruit_BME680::Adafruit_BME680;

public:
    BME680SensV2(TwoWire *theWire = &Wire);
    BME680SensV2(int8_t cspin, SPIClass *theSPI = &SPI);
    BME680SensV2(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin);
    virtual ~BME680SensV2();

    bool init() override;
    bool update() override;

    void setUpdateInterval(uint32_t interval);
    float getDefaultSeaLevelPressureHPA() const;
};

#endif  // BME680_SENS_V2