#include "MQSensV2.h"

MQSensV2::MQSensV2(String board, float voltageResolution, int adcBitResolution, int pin, String mqType)
    : MQUnifiedsensor(board, voltageResolution, adcBitResolution, pin, mqType),
      _updateTimer(0),
      _updateInterval(2000),
      _lastUpdateStatus(false),
      _enableCalibration(true),
      _enableMultipleGases(false),
      _enableRatioMode(false),
      _correctionFactor(0.0),
      _gasProfiles(nullptr),
      _gasCount(0),
      _gasCapacity(0),
      _activeGasIndex(-1) {
    initGasProfiles();
    loadPresetGas(mqType.c_str());
}

MQSensV2::MQSensV2(String board, String mqType)
    : MQUnifiedsensor(board, mqType),
      _updateTimer(0),
      _updateInterval(2000),
      _lastUpdateStatus(false),
      _enableCalibration(true),
      _enableMultipleGases(false),
      _enableRatioMode(false),
      _correctionFactor(0.0),
      _gasProfiles(nullptr),
      _gasCount(0),
      _gasCapacity(0),
      _activeGasIndex(-1) {
    initGasProfiles();
    loadPresetGas(mqType.c_str());
}

MQSensV2::~MQSensV2() {
    if (_gasProfiles) {
        for (uint8_t i = 0; i < _gasCount; i++) {
            if (_gasProfiles[i].name) free(_gasProfiles[i].name);
            if (_gasProfiles[i].unit) free(_gasProfiles[i].unit);
        }
        free(_gasProfiles);
    }
}

bool MQSensV2::init() {
    MQUnifiedsensor::init();
    
    addValueInfo("ppm", "Gas Concentration", "ppm", 2, true);
    addValueInfo("ratio", "Rs/R0 Ratio", "", 3, false);
    addValueInfo("resistance", "Sensor Resistance", "kOhm", 2, false);
    addValueInfo("voltage", "Sensor Voltage", "V", 3, false);
    
    if (_enableRatioMode) {
        addValueInfo("ratioMode", "Ratio Mode", "", 3, false);
    }
    
    registerGasValues();
    
    updateValue("ppm", 0.0f);
    updateValue("ratio", 0.0f);
    updateValue("resistance", 0.0f);
    updateValue("voltage", 0.0f);
    
    return true;
}

bool MQSensV2::update() {
    unsigned long currentTime = millis();
    if ((currentTime - _updateTimer) >= _updateInterval) {
        MQUnifiedsensor::update();
        
        float voltage = getVoltage();
        float resistance = getRS();
        float ratio = readSensorR0Rs();
        
        updateValue("voltage", voltage);
        updateValue("resistance", resistance);
        updateValue("ratio", ratio);
        
        if (_enableRatioMode) {
            updateValue("ratioMode", ratio);
        }
        
        if (_activeGasIndex >= 0 && _activeGasIndex < _gasCount) {
            GasProfile& gas = _gasProfiles[_activeGasIndex];
            setA(gas.a);
            setB(gas.b);
            setRegressionMethod(gas.regressionMethod);
            
            float ppm = readSensor();
            updateValue("ppm", ppm);
            
            if (_enableMultipleGases) {
                String gasKey = String(gas.name) + "_ppm";
                updateValue(gasKey.c_str(), ppm);
            }
        }
        
        if (_enableMultipleGases && _gasCount > 1) {
            for (uint8_t i = 0; i < _gasCount; i++) {
                if (i != _activeGasIndex) {
                    GasProfile& gas = _gasProfiles[i];
                    setA(gas.a);
                    setB(gas.b);
                    setRegressionMethod(gas.regressionMethod);
                    
                    float ppm = readSensor();
                    String gasKey = String(gas.name) + "_ppm";
                    updateValue(gasKey.c_str(), ppm);
                }
            }
        }
        
        _updateTimer = currentTime;
        _lastUpdateStatus = true;
        return true;
    }
    _lastUpdateStatus = false;
    return false;
}

bool MQSensV2::isUpdated() const {
    return _lastUpdateStatus;
}

void MQSensV2::setUpdateInterval(uint32_t interval) {
    _updateInterval = interval;
}

void MQSensV2::setCorrectionFactor(float factor) {
    _correctionFactor = factor;
}

void MQSensV2::enableCalibration(bool enable) {
    _enableCalibration = enable;
}

void MQSensV2::enableMultipleGases(bool enable) {
    _enableMultipleGases = enable;
    if (enable) {
        registerGasValues();
    }
}

void MQSensV2::enableRatioMode(bool enable) {
    _enableRatioMode = enable;
    if (enable && !hasValue("ratioMode")) {
        addValueInfo("ratioMode", "Ratio Mode", "", 3, false);
    }
}

void MQSensV2::initGasProfiles() {
    _gasCapacity = 8;
    _gasProfiles = (GasProfile*)malloc(_gasCapacity * sizeof(GasProfile));
    for (uint8_t i = 0; i < _gasCapacity; i++) {
        _gasProfiles[i].name = nullptr;
        _gasProfiles[i].unit = nullptr;
    }
}

void MQSensV2::addGasProfile(const char* name, float a, float b, int regressionMethod, const char* unit, uint8_t precision) {
    if (_gasCount >= _gasCapacity) {
        uint8_t newCapacity = _gasCapacity + 4;
        GasProfile* newProfiles = (GasProfile*)realloc(_gasProfiles, newCapacity * sizeof(GasProfile));
        if (newProfiles) {
            _gasProfiles = newProfiles;
            for (uint8_t i = _gasCapacity; i < newCapacity; i++) {
                _gasProfiles[i].name = nullptr;
                _gasProfiles[i].unit = nullptr;
            }
            _gasCapacity = newCapacity;
        } else {
            return;
        }
    }
    
    _gasProfiles[_gasCount].name = strdup(name);
    _gasProfiles[_gasCount].a = a;
    _gasProfiles[_gasCount].b = b;
    _gasProfiles[_gasCount].regressionMethod = regressionMethod;
    _gasProfiles[_gasCount].unit = strdup(unit);
    _gasProfiles[_gasCount].precision = precision;
    
    if (_activeGasIndex < 0) {
        _activeGasIndex = 0;
    }
    
    _gasCount++;
}

void MQSensV2::addCustomGas(const char* name, float a, float b, int regressionMethod, const char* unit, uint8_t precision) {
    addGasProfile(name, a, b, regressionMethod, unit, precision);
    
    if (_enableMultipleGases) {
        String gasKey = String(name) + "_ppm";
        addValueInfo(gasKey.c_str(), name, unit, precision, true);
    }
}

void MQSensV2::setActiveGas(const char* gasName) {
    for (uint8_t i = 0; i < _gasCount; i++) {
        if (strcmp(_gasProfiles[i].name, gasName) == 0) {
            _activeGasIndex = i;
            return;
        }
    }
}

void MQSensV2::setActiveGas(int gasIndex) {
    if (gasIndex >= 0 && gasIndex < _gasCount) {
        _activeGasIndex = gasIndex;
    }
}

void MQSensV2::registerGasValues() {
    if (_enableMultipleGases) {
        for (uint8_t i = 0; i < _gasCount; i++) {
            String gasKey = String(_gasProfiles[i].name) + "_ppm";
            if (!hasValue(gasKey.c_str())) {
                addValueInfo(gasKey.c_str(), _gasProfiles[i].name, _gasProfiles[i].unit, _gasProfiles[i].precision, true);
            }
        }
    }
}

float MQSensV2::calibrateInCleanAir(float ratioInCleanAir) {
    if (_enableCalibration) {
        return calibrate(ratioInCleanAir);
    }
    return getR0();
}

float MQSensV2::readGasConcentration(const char* gasName) {
    if (gasName != nullptr) {
        int oldIndex = _activeGasIndex;
        setActiveGas(gasName);
        
        if (_activeGasIndex >= 0) {
            GasProfile& gas = _gasProfiles[_activeGasIndex];
            setA(gas.a);
            setB(gas.b);
            setRegressionMethod(gas.regressionMethod);
            
            float result = readSensor();
            _activeGasIndex = oldIndex;
            return result;
        }
        _activeGasIndex = oldIndex;
        return 0.0;
    }
    
    return readSensor();
}

float MQSensV2::readRatio() {
    return readSensorR0Rs();
}

float MQSensV2::getResistance() {
    return getRS();
}

void MQSensV2::loadPresetGas(const char* mqType) {
    String type = String(mqType);
    type.toUpperCase();
    
    if (type.indexOf("MQ-2") >= 0) {
        addGasProfile("LPG", 574.25, -2.222, 1, "ppm", 2);
        addGasProfile("CO", 36974, -3.109, 1, "ppm", 2);
        addGasProfile("Smoke", 3616.1, -2.675, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-3") >= 0) {
        addGasProfile("Alcohol", 4.8387, -2.68, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-4") >= 0) {
        addGasProfile("CH4", 3811.9, -3.113, 1, "ppm", 2);
        addGasProfile("Smoke", 30000000, -8.308, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-5") >= 0) {
        addGasProfile("LPG", 80.897, -2.431, 1, "ppm", 2);
        addGasProfile("CH4", 177.65, -2.56, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-6") >= 0) {
        addGasProfile("LPG", 88158, -3.597, 1, "ppm", 2);
        addGasProfile("CH4", 1009.2, -2.35, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-7") >= 0) {
        addGasProfile("CO", 23.943, -1.11, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-8") >= 0) {
        addGasProfile("H2", 976.97, -0.688, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-9") >= 0) {
        addGasProfile("LPG", 1000.5, -2.186, 1, "ppm", 2);
        addGasProfile("CH4", 4269.6, -2.648, 1, "ppm", 2);
        addGasProfile("CO", 599.65, -2.244, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-131") >= 0) {
        addGasProfile("O3", 23.943, -1.11, 1, "ppb", 1);
    }
    else if (type.indexOf("MQ-135") >= 0) {
        addGasProfile("CO2", 110.47, -2.862, 1, "ppm", 2);
        addGasProfile("CO", 605.18, -3.937, 1, "ppm", 2);
        addGasProfile("Alcohol", 77.255, -3.18, 1, "ppm", 2);
        addGasProfile("NH4", 102.2, -2.473, 1, "ppm", 2);
        addGasProfile("Toluene", 44.947, -3.445, 1, "ppm", 2);
        addGasProfile("Acetone", 34.668, -3.369, 1, "ppm", 2);
    }
    else if (type.indexOf("MQ-136") >= 0) {
        addGasProfile("H2S", 42.557, -3.594, 1, "ppm", 2);
        addGasProfile("NH4", 98.322, -0.947, 1, "ppm", 2);
    }
    else {
        addGasProfile("Unknown", 10, -1, 1, "ppm", 2);
    }
}

void MQSensV2::printSensorInfo() {
    Serial.println("=== MQ Sensor Info ===");
    Serial.print("Voltage Resolution: "); Serial.println(getVoltResolution());
    Serial.print("R0: "); Serial.println(getR0());
    Serial.print("RL: "); Serial.println(getRL());
    Serial.print("Gas Count: "); Serial.println(_gasCount);
    
    for (uint8_t i = 0; i < _gasCount; i++) {
        Serial.print("Gas "); Serial.print(i); Serial.print(": ");
        Serial.print(_gasProfiles[i].name);
        Serial.print(" (A="); Serial.print(_gasProfiles[i].a);
        Serial.print(", B="); Serial.print(_gasProfiles[i].b);
        Serial.print(", Method="); Serial.print(_gasProfiles[i].regressionMethod);
        Serial.println(")");
    }
    Serial.println("=====================");
}

int MQSensV2::getGasCount() const {
    return _gasCount;
}

const char* MQSensV2::getGasName(int index) const {
    if (index >= 0 && index < _gasCount) {
        return _gasProfiles[index].name;
    }
    return nullptr;
}

const char* MQSensV2::getActiveGasName() const {
    if (_activeGasIndex >= 0 && _activeGasIndex < _gasCount) {
        return _gasProfiles[_activeGasIndex].name;
    }
    return nullptr;
}

bool MQSensV2::isCalibrated() const {
    return getR0Internal() > 0;
}