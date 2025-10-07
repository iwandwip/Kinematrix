#include "InteractiveCalibrator.h"

InteractiveCalibrator::InteractiveCalibrator()
    : _dataInterface(new SensorDataInterface()),
      _engine(new CalibrationEngine()),
      _storage(new CalibrationStorage()),
      _inputHandler(new SerialInputHandler()),
      _ui(new CalibrationUI()),
      _completedCallback(nullptr),
      _completedCallbackContext(nullptr) {
}

InteractiveCalibrator::~InteractiveCalibrator() {
    if (_dataInterface) delete _dataInterface;
    if (_engine) delete _engine;
    if (_storage) delete _storage;
    if (_inputHandler) delete _inputHandler;
    if (_ui) delete _ui;
}

void InteractiveCalibrator::begin(Stream *serialPtr, unsigned long timeout, int eepromAddress) {
    if (_inputHandler) _inputHandler->begin(serialPtr, timeout);
    if (_ui) _ui->begin(serialPtr);
    if (_storage) _storage->setEEPROMAddress(eepromAddress);
    
    initializeEEPROM();
    
    if (_ui) {
        _ui->setState(STATE_MENU);
        _ui->printMenu();
    }
}

bool InteractiveCalibrator::isActive() const {
    return _ui && _ui->getState() != STATE_IDLE;
}

bool InteractiveCalibrator::isFinished() const {
    return _ui && _ui->isFinished();
}

void InteractiveCalibrator::setSensorReadCallback(SensorDataInterface::SensorReadCallback callback) {
    if (_dataInterface) _dataInterface->setSensorReadCallback(callback);
}

void InteractiveCalibrator::setSensorReadCallbackWithContext(SensorDataInterface::SensorReadCallbackWithContext callback, void *context) {
    if (_dataInterface) _dataInterface->setSensorReadCallbackWithContext(callback, context);
}

void InteractiveCalibrator::setSensorValueContext(BaseSensV2 *sensor, const char *valueKey) {
    if (_dataInterface) _dataInterface->setSensorValueContext(sensor, valueKey);
}

void InteractiveCalibrator::setSensorValuePointer(float *valuePtr) {
    if (_dataInterface) _dataInterface->setSensorValuePointer(valuePtr);
}

void InteractiveCalibrator::setSensorInfo(const char *sensorName, const char *valueLabel) {
    if (_dataInterface) _dataInterface->setSensorInfo(sensorName, valueLabel);
}

void InteractiveCalibrator::setCompletedCallback(CalibrationCompletedCallback callback, void *callbackContext) {
    _completedCallback = callback;
    _completedCallbackContext = callbackContext;
}

void InteractiveCalibrator::setEEPROMAddress(int address) {
    if (_storage) _storage->setEEPROMAddress(address);
}

int InteractiveCalibrator::getEEPROMAddress() const {
    return _storage ? _storage->getEEPROMAddress() : 0;
}

float InteractiveCalibrator::readRawValue() {
    return _dataInterface ? _dataInterface->readRawValue() : 0.0f;
}

float InteractiveCalibrator::readCalibratedValue() {
    if (!_dataInterface || !_engine) return 0.0f;
    
    float rawValue = _dataInterface->readRawValue();
    return _engine->readCalibratedValue(rawValue);
}

void InteractiveCalibrator::setReadInterval(unsigned long interval) {
    if (_ui) _ui->setReadInterval(interval);
}

void InteractiveCalibrator::enableContinuousReading(bool enable) {
    if (_ui) _ui->enableContinuousReading(enable);
}

void InteractiveCalibrator::update() {
    if (_ui) _ui->update(_dataInterface);
    
    if (_inputHandler && _inputHandler->isWaitingForInput()) {
        _inputHandler->handleSerialInput();
    }
}

bool InteractiveCalibrator::calibrateOnePoint(float knownValue, float rawValue) {
    if (!_engine || !_dataInterface) return false;
    
    if (rawValue == -1) {
        rawValue = _dataInterface->readRawValue();
    }
    
    return _engine->calibrateOnePoint(knownValue, rawValue);
}

bool InteractiveCalibrator::calibrateTwoPoint(float knownValue1, float rawValue1, float knownValue2, float rawValue2) {
    return _engine && _engine->calibrateTwoPoint(knownValue1, rawValue1, knownValue2, rawValue2);
}

void InteractiveCalibrator::setMaxCalibrationPoints(uint8_t maxPoints) {
    if (_engine) _engine->setMaxCalibrationPoints(maxPoints);
}

void InteractiveCalibrator::startCalibration(uint8_t calibrationType) {
    if (_engine) _engine->startCalibration(calibrationType);
}

bool InteractiveCalibrator::addCalibrationPoint(float knownValue, float rawValue) {
    if (!_engine || !_dataInterface) return false;
    
    if (rawValue == -1) {
        rawValue = _dataInterface->readRawValue();
    }
    
    return _engine->addCalibrationPoint(knownValue, rawValue);
}

bool InteractiveCalibrator::calculateCalibration() {
    return _engine && _engine->calculateCalibration();
}

void InteractiveCalibrator::cancelCalibration() {
    if (_engine) _engine->cancelCalibration();
}

bool InteractiveCalibrator::clearCalibrationPoints() {
    return _engine && _engine->clearCalibrationPoints();
}

bool InteractiveCalibrator::isCalibrated() {
    return _engine && _engine->isCalibrated();
}

uint8_t InteractiveCalibrator::getCalibrationMethod() {
    return _engine ? _engine->getCalibrationMethod() : CALIBRATION_NONE;
}

uint8_t InteractiveCalibrator::getCalibrationPointCount() {
    return _engine ? _engine->getCalibrationPointCount() : 0;
}

void InteractiveCalibrator::setInterpolationMethod(uint8_t method) {
    if (_engine) _engine->setInterpolationMethod(method);
}

void InteractiveCalibrator::setPolynomialDegree(uint8_t degree) {
    if (_engine) _engine->setPolynomialDegree(degree);
}

bool InteractiveCalibrator::processCommand(char cmd) {
    return _ui && _ui->processCommand(cmd, _dataInterface, _engine, _storage, _inputHandler);
}

void InteractiveCalibrator::printMenu() {
    if (_ui) _ui->printMenu();
}

void InteractiveCalibrator::setMenuPrompt(const char *prompt) {
    if (_ui) _ui->setMenuPrompt(prompt);
}

void InteractiveCalibrator::setMenuOptions(const char *options[], uint8_t numOptions) {
    if (_ui) _ui->setMenuOptions(options, numOptions);
}

void InteractiveCalibrator::startReading() {
    if (_ui) _ui->startReading(_dataInterface);
}

void InteractiveCalibrator::stopReading() {
    if (_ui) _ui->stopReading();
}

void InteractiveCalibrator::readSensor() {
    if (_ui) _ui->readSensor(_dataInterface);
}

void InteractiveCalibrator::runOnePointCalibration() {
    if (_ui) _ui->runOnePointCalibration(_dataInterface, _engine, _inputHandler);
}

void InteractiveCalibrator::runTwoPointCalibration() {
    if (_ui) _ui->runTwoPointCalibration(_dataInterface, _engine, _inputHandler);
}

void InteractiveCalibrator::runMultiPointCalibration() {
    if (_ui) _ui->runMultiPointCalibration(_dataInterface, _engine, _inputHandler);
}

void InteractiveCalibrator::showCalibrationDetails() {
    if (_ui) _ui->showCalibrationDetails(_engine, _storage);
}

void InteractiveCalibrator::runClearCalibration() {
    if (_ui) _ui->runClearCalibration(_engine);
}

float InteractiveCalibrator::readFloat(const char *prompt, float defaultValue, unsigned long timeout) {
    return _inputHandler ? _inputHandler->readFloat(prompt, defaultValue, timeout) : defaultValue;
}

int InteractiveCalibrator::readInt(const char *prompt, int defaultValue, unsigned long timeout) {
    return _inputHandler ? _inputHandler->readInt(prompt, defaultValue, timeout) : defaultValue;
}

bool InteractiveCalibrator::readYesNo(const char *prompt, bool defaultValue, unsigned long timeout) {
    return _inputHandler ? _inputHandler->readYesNo(prompt, defaultValue, timeout) : defaultValue;
}

char InteractiveCalibrator::readChar(const char *prompt, char defaultValue, unsigned long timeout) {
    return _inputHandler ? _inputHandler->readChar(prompt, defaultValue, timeout) : defaultValue;
}

String InteractiveCalibrator::readString(const char *prompt, const char *defaultValue, unsigned long timeout) {
    return _inputHandler ? _inputHandler->readString(prompt, defaultValue, timeout) : String(defaultValue);
}

bool InteractiveCalibrator::saveCalibration(int eepromAddress) {
    return _storage && _storage->saveCalibration(_engine, eepromAddress);
}

bool InteractiveCalibrator::loadCalibration(int eepromAddress) {
    return _storage && _storage->loadCalibration(_engine, eepromAddress);
}

bool InteractiveCalibrator::saveCalibrationProfile(uint8_t profileNumber) {
    return _storage && _storage->saveCalibrationProfile(_engine, profileNumber);
}

bool InteractiveCalibrator::loadCalibrationProfile(uint8_t profileNumber) {
    return _storage && _storage->loadCalibrationProfile(_engine, profileNumber);
}

void InteractiveCalibrator::listCalibrationProfiles() {
    if (_storage && _ui) {
        _storage->listCalibrationProfiles(nullptr);
    }
}

void InteractiveCalibrator::showStatistics(uint16_t numSamples) {
    if (_ui) _ui->showStatistics(_dataInterface, _storage, numSamples);
}

void InteractiveCalibrator::drawCalibrationCurve(uint8_t width, uint8_t height) {
    if (_ui) _ui->drawCalibrationCurve(_engine, width, height);
}

void InteractiveCalibrator::setDisplayUnits(const char *units) {
    if (_storage) _storage->setDisplayUnits(units);
}

void InteractiveCalibrator::setDisplayPrecision(uint8_t precision) {
    if (_storage) _storage->setDisplayPrecision(precision);
}

const char *InteractiveCalibrator::getDisplayUnits() const {
    return _storage ? _storage->getDisplayUnits() : "units";
}

uint8_t InteractiveCalibrator::getDisplayPrecision() const {
    return _storage ? _storage->getDisplayPrecision() : 2;
}

bool InteractiveCalibrator::loadPreferences(int eepromAddress) {
    return _storage && _storage->loadPreferences(eepromAddress);
}

bool InteractiveCalibrator::savePreferences(int eepromAddress) {
    return _storage && _storage->savePreferences(eepromAddress);
}

void InteractiveCalibrator::resetToDefaults() {
    if (_storage) _storage->resetToDefaults();
}

void InteractiveCalibrator::clearSerialBuffer() {
    if (_inputHandler) _inputHandler->clearSerialBuffer();
}

void InteractiveCalibrator::waitForSerialInput() {
    if (_inputHandler) _inputHandler->waitForSerialInput();
}

bool InteractiveCalibrator::isWaitingForInput() const {
    return _inputHandler && _inputHandler->isWaitingForInput();
}

bool InteractiveCalibrator::handleSerialInput() {
    return _inputHandler && _inputHandler->handleSerialInput();
}

void InteractiveCalibrator::end() {
    if (_ui) {
        _ui->setState(STATE_IDLE);
        _ui->setFinished(true);
    }
    notifyCalibrationCompleted();
}

void InteractiveCalibrator::notifyCalibrationCompleted() {
    if (_completedCallback && _completedCallbackContext) {
        _completedCallback(_completedCallbackContext);
    }
}

void InteractiveCalibrator::initializeEEPROM() {
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.begin(512);
#endif
}