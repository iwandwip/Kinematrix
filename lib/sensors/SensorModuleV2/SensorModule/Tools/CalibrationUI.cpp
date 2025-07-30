#include "CalibrationUI.h"

CalibrationUI::CalibrationUI()
    : _serial(nullptr), _readInterval(1000), _lastReadTime(0),
      _continuousReading(false), _state(STATE_IDLE), _isFinished(false),
      _menuPrompt(nullptr) {
}

CalibrationUI::~CalibrationUI() {
}

void CalibrationUI::begin(Stream *serialPtr) {
    _serial = serialPtr;
}

void CalibrationUI::setReadInterval(unsigned long interval) {
    _readInterval = interval;
}

void CalibrationUI::enableContinuousReading(bool enable) {
    _continuousReading = enable;
}

void CalibrationUI::setMenuPrompt(const char *prompt) {
    _menuPrompt = prompt;
}

void CalibrationUI::setMenuOptions(const char *options[], uint8_t numOptions) {
    (void)options;
    (void)numOptions;
}

bool CalibrationUI::processCommand(char cmd, SensorDataInterface* dataInterface, 
                                  CalibrationEngine* engine, CalibrationStorage* storage,
                                  SerialInputHandler* inputHandler) {
    switch (cmd) {
        case 'r':
        case 'R':
            startReading(dataInterface);
            return true;
            
        case 's':
        case 'S':
            stopReading();
            return true;
            
        case '1':
            runOnePointCalibration(dataInterface, engine, inputHandler);
            return true;
            
        case '2':
            runTwoPointCalibration(dataInterface, engine, inputHandler);
            return true;
            
        case 'm':
        case 'M':
            runMultiPointCalibration(dataInterface, engine, inputHandler);
            return true;
            
        case 'd':
        case 'D':
            showCalibrationDetails(engine, storage);
            return true;
            
        case 'c':
        case 'C':
            runClearCalibration(engine);
            return true;
            
        case 'q':
        case 'Q':
            _isFinished = true;
            return true;
            
        default:
            if (_serial) {
                _serial->println("Invalid command. Press 'h' for help.");
            }
            return false;
    }
}

void CalibrationUI::printMenu() {
    if (!_serial) return;
    
    _serial->println("\n==== Sensor Calibration Menu ====");
    _serial->println("r/R - Start reading sensor");
    _serial->println("s/S - Stop reading");
    _serial->println("1   - One-point calibration");
    _serial->println("2   - Two-point calibration");
    _serial->println("m/M - Multi-point calibration");
    _serial->println("d/D - Show calibration details");
    _serial->println("c/C - Clear calibration");
    _serial->println("q/Q - Quit");
    _serial->println("=================================");
    
    if (_menuPrompt) {
        _serial->print(_menuPrompt);
    } else {
        _serial->print("Enter command");
    }
    _serial->print(": ");
}

void CalibrationUI::startReading(SensorDataInterface* dataInterface) {
    if (!_serial || !dataInterface) return;
    
    _state = STATE_READING;
    _continuousReading = true;
    _serial->println("\nStarting continuous reading (press 's' to stop)...");
    printReadingHeader();
}

void CalibrationUI::stopReading() {
    if (!_serial) return;
    
    _continuousReading = false;
    _state = STATE_MENU;
    _serial->println("\nReading stopped.");
    printMenu();
}

void CalibrationUI::readSensor(SensorDataInterface* dataInterface) {
    if (!_serial || !dataInterface) return;
    
    float rawValue = dataInterface->readRawValue();
    _serial->print("Raw: ");
    _serial->print(rawValue, 3);
    _serial->println();
}

void CalibrationUI::runOnePointCalibration(SensorDataInterface* dataInterface, 
                                          CalibrationEngine* engine, SerialInputHandler* inputHandler) {
    if (!_serial || !dataInterface || !engine || !inputHandler) return;
    
    _serial->println("\n=== One-Point Calibration ===");
    _serial->println("Place sensor in known environment and press Enter...");
    inputHandler->waitForSerialInput();
    
    float rawValue = collectStableMeasurement(dataInterface);
    float knownValue = inputHandler->readFloat("Enter known value", 0.0f);
    
    if (engine->calibrateOnePoint(knownValue, rawValue)) {
        _serial->println("One-point calibration completed!");
    } else {
        _serial->println("Calibration failed!");
    }
    
    _state = STATE_MENU;
    printMenu();
}

void CalibrationUI::runTwoPointCalibration(SensorDataInterface* dataInterface, 
                                          CalibrationEngine* engine, SerialInputHandler* inputHandler) {
    if (!_serial || !dataInterface || !engine || !inputHandler) return;
    
    _serial->println("\n=== Two-Point Calibration ===");
    
    _serial->println("Point 1: Place sensor in first environment and press Enter...");
    inputHandler->waitForSerialInput();
    float rawValue1 = collectStableMeasurement(dataInterface);
    float knownValue1 = inputHandler->readFloat("Enter known value for point 1", 0.0f);
    
    _serial->println("Point 2: Place sensor in second environment and press Enter...");
    inputHandler->waitForSerialInput();
    float rawValue2 = collectStableMeasurement(dataInterface);
    float knownValue2 = inputHandler->readFloat("Enter known value for point 2", 0.0f);
    
    if (engine->calibrateTwoPoint(knownValue1, rawValue1, knownValue2, rawValue2)) {
        _serial->println("Two-point calibration completed!");
    } else {
        _serial->println("Calibration failed!");
    }
    
    _state = STATE_MENU;
    printMenu();
}

void CalibrationUI::runMultiPointCalibration(SensorDataInterface* dataInterface, 
                                            CalibrationEngine* engine, SerialInputHandler* inputHandler) {
    if (!_serial || !dataInterface || !engine || !inputHandler) return;
    
    _serial->println("\n=== Multi-Point Calibration ===");
    
    uint8_t maxPoints = inputHandler->readInt("Enter number of calibration points (2-10)", 3);
    if (maxPoints < 2) maxPoints = 2;
    if (maxPoints > 10) maxPoints = 10;
    
    engine->setMaxCalibrationPoints(maxPoints);
    engine->startCalibration(CALIBRATION_MULTI_POINT);
    
    for (uint8_t i = 0; i < maxPoints; i++) {
        _serial->print("Point ");
        _serial->print(i + 1);
        _serial->println(": Place sensor and press Enter...");
        inputHandler->waitForSerialInput();
        
        float rawValue = collectStableMeasurement(dataInterface);
        float knownValue = inputHandler->readFloat("Enter known value", 0.0f);
        
        engine->addCalibrationPoint(knownValue, rawValue);
    }
    
    if (engine->calculateCalibration()) {
        _serial->println("Multi-point calibration completed!");
    } else {
        _serial->println("Calibration failed!");
    }
    
    _state = STATE_MENU;
    printMenu();
}

void CalibrationUI::showCalibrationDetails(const CalibrationEngine* engine, const CalibrationStorage* storage) {
    if (!_serial || !engine || !storage) return;
    
    _serial->println("\n=== Calibration Details ===");
    
    if (engine->isCalibrated()) {
        _serial->print("Method: ");
        _serial->println(engine->getCalibrationMethod());
        
        _serial->print("Points: ");
        _serial->println(engine->getCalibrationPointCount());
        
        printCalibrationDetails(engine);
        
        _serial->print("Units: ");
        _serial->println(storage->getDisplayUnits());
        
        _serial->print("Precision: ");
        _serial->println(storage->getDisplayPrecision());
    } else {
        _serial->println("No calibration active.");
    }
    
    _state = STATE_MENU;
    printMenu();
}

void CalibrationUI::runClearCalibration(CalibrationEngine* engine) {
    if (!_serial || !engine) return;
    
    if (engine->clearCalibrationPoints()) {
        _serial->println("\nCalibration cleared.");
    } else {
        _serial->println("\nFailed to clear calibration.");
    }
    
    _state = STATE_MENU;
    printMenu();
}

void CalibrationUI::showStatistics(SensorDataInterface* dataInterface, const CalibrationStorage* storage, uint16_t numSamples) {
    if (!_serial || !dataInterface || !storage) return;
    
    _serial->println("\n=== Statistics ===");
    printStatsHeader();
    
    float sum = 0.0f;
    float min = 999999.0f;
    float max = -999999.0f;
    
    for (uint16_t i = 0; i < numSamples; i++) {
        float value = dataInterface->readRawValue();
        sum += value;
        if (value < min) min = value;
        if (value > max) max = value;
        
        _serial->print(i + 1);
        _serial->print("\t");
        _serial->print(value, storage->getDisplayPrecision());
        _serial->println();
        
        delay(100);
    }
    
    float avg = sum / numSamples;
    
    _serial->println("\n=== Summary ===");
    _serial->print("Average: ");
    _serial->println(avg, storage->getDisplayPrecision());
    _serial->print("Min: ");
    _serial->println(min, storage->getDisplayPrecision());
    _serial->print("Max: ");
    _serial->println(max, storage->getDisplayPrecision());
    _serial->print("Range: ");
    _serial->println(max - min, storage->getDisplayPrecision());
}

void CalibrationUI::drawCalibrationCurve(const CalibrationEngine* engine, uint8_t /*width*/, uint8_t /*height*/) {
    if (!_serial || !engine || !engine->isCalibrated()) return;
    
    _serial->println("\n=== Calibration Curve ===");
    
    const CalibrationEngine::CalibrationPoint* points = engine->getCalibrationPoints();
    uint8_t numPoints = engine->getCalibrationPointCount();
    
    if (numPoints < 2) {
        _serial->println("Insufficient points for curve display.");
        return;
    }
    
    for (uint8_t i = 0; i < numPoints; i++) {
        _serial->print("Point ");
        _serial->print(i + 1);
        _serial->print(": (");
        _serial->print(points[i].rawValue, 2);
        _serial->print(", ");
        _serial->print(points[i].knownValue, 2);
        _serial->println(")");
    }
}

CalibrationStateV2 CalibrationUI::getState() const {
    return _state;
}

void CalibrationUI::setState(CalibrationStateV2 state) {
    _state = state;
}

bool CalibrationUI::isFinished() const {
    return _isFinished;
}

void CalibrationUI::setFinished(bool finished) {
    _isFinished = finished;
}

void CalibrationUI::update(SensorDataInterface* dataInterface) {
    if (_continuousReading && _state == STATE_READING && dataInterface) {
        if (millis() - _lastReadTime >= _readInterval) {
            readSensor(dataInterface);
            _lastReadTime = millis();
        }
    }
}

float CalibrationUI::collectStableMeasurement(SensorDataInterface* dataInterface, uint8_t numSamples, unsigned long delayBetweenSamples) {
    float sum = 0.0f;
    for (uint8_t i = 0; i < numSamples; i++) {
        sum += dataInterface->readRawValue();
        delay(delayBetweenSamples);
    }
    return sum / numSamples;
}

void CalibrationUI::printReadingHeader() {
    if (_serial) {
        _serial->println("Time\t\tRaw Value\tCalibrated");
        _serial->println("====\t\t=========\t==========");
    }
}

void CalibrationUI::printStatsHeader() {
    if (_serial) {
        _serial->println("Sample\tValue");
        _serial->println("======\t=====");
    }
}

void CalibrationUI::printCalibrationDetails(const CalibrationEngine* engine) {
    if (!_serial) return;
    
    switch (engine->getCalibrationMethod()) {
        case CALIBRATION_ONE_POINT:
            _serial->print("Scale factor: ");
            _serial->println(engine->getScale(), 6);
            break;
            
        case CALIBRATION_TWO_POINT:
            _serial->print("Slope: ");
            _serial->println(engine->getSlope(), 6);
            _serial->print("Offset: ");
            _serial->println(engine->getOffset(), 6);
            break;
            
        case CALIBRATION_MULTI_POINT:
            _serial->println("Multi-point interpolation active");
            break;
    }
}