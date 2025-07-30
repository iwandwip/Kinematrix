#include "BME680SensV2.h"

BME680SensV2::BME680SensV2(TwoWire *theWire)
        : Adafruit_BME680(theWire),
          _updateTimer(0),
          _updateInterval(2000),
          _lastUpdateStatus(false) {
    initCustomValue();
}

BME680SensV2::BME680SensV2(int8_t cspin, SPIClass *theSPI)
        : Adafruit_BME680(cspin, theSPI),
          _updateTimer(0),
          _updateInterval(2000),
          _lastUpdateStatus(false) {
    initCustomValue();
}

BME680SensV2::BME680SensV2(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin)
        : Adafruit_BME680(cspin, mosipin, misopin, sckpin),
          _updateTimer(0),
          _updateInterval(2000),
          _lastUpdateStatus(false) {
    initCustomValue();
}

BME680SensV2::~BME680SensV2() = default;

bool BME680SensV2::init() {
    if (!Adafruit_BME680::begin()) return false;
    Adafruit_BME680::setTemperatureOversampling(BME680_OS_8X);
    Adafruit_BME680::setHumidityOversampling(BME680_OS_2X);
    Adafruit_BME680::setPressureOversampling(BME680_OS_4X);
    Adafruit_BME680::setIIRFilterSize(BME680_FILTER_SIZE_3);
    Adafruit_BME680::setGasHeater(320, 150); // 320*C for 150 ms
    return true;
}

bool BME680SensV2::update() {
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        if (!Adafruit_BME680::performReading()) {
            _lastUpdateStatus = false;
            return false;
        }
        updateValue("temperature", Adafruit_BME680::readTemperature());
        updateValue("humidity", Adafruit_BME680::readHumidity());
        updateValue("pressure", Adafruit_BME680::readPressure() / 100.0f);
        updateValue("gas", Adafruit_BME680::readGas() / 1000.0f);
        updateValue("altitude", Adafruit_BME680::readAltitude(getDefaultSeaLevelPressureHPA()));
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

void BME680SensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

float BME680SensV2::getDefaultSeaLevelPressureHPA() const {
    return 1013.25f;
}

bool BME680SensV2::isUpdated() const {
    return _lastUpdateStatus;
}

void BME680SensV2::initCustomValue() {
    addValueInfo("temperature", "Temperature", "C", 3, true);
    addValueInfo("humidity", "Humidity", "%", 3, true);
    addValueInfo("pressure", "Pressure", "hPa", 3, true);
    addValueInfo("gas", "Gas", "KOhms", 3, true);
    addValueInfo("altitude", "Altitude", "m", 3, true);
}