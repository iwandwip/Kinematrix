#include "SerialInputHandler.h"

SerialInputHandler::SerialInputHandler()
    : _serial(nullptr), _timeout(10000), _inputMode(INPUT_MODE_NONE),
      _inputComplete(false),
      _inputDefaultFloat(0.0f), _inputDefaultInt(0), _inputDefaultBool(true),
      _inputDefaultChar('\0') {
}

SerialInputHandler::~SerialInputHandler() {
}

void SerialInputHandler::begin(Stream *serialPtr, unsigned long timeout) {
    _serial = serialPtr;
    _timeout = timeout;
}

float SerialInputHandler::readFloat(const char *prompt, float defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;
    
    _inputDefaultFloat = defaultValue;
    
    beginInputMode(INPUT_MODE_FLOAT, prompt);
    
    unsigned long startTime = millis();
    unsigned long timeoutMs = (timeout > 0) ? timeout : _timeout;
    
    while (!_inputComplete && ((millis() - startTime) < timeoutMs)) {
        if (_serial->available() > 0) {
            processSerialInput();
        }
        delay(10);
    }
    
    float result = _inputComplete ? _inputDefaultFloat : defaultValue;
    
    _inputMode = INPUT_MODE_NONE;
    _inputComplete = false;
    return result;
}

int SerialInputHandler::readInt(const char *prompt, int defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;
    
    _inputDefaultInt = defaultValue;
    
    beginInputMode(INPUT_MODE_INT, prompt);
    
    unsigned long startTime = millis();
    unsigned long timeoutMs = (timeout > 0) ? timeout : _timeout;
    
    while (!_inputComplete && ((millis() - startTime) < timeoutMs)) {
        if (_serial->available() > 0) {
            processSerialInput();
        }
        delay(10);
    }
    
    int result = _inputComplete ? _inputDefaultInt : defaultValue;
    
    _inputMode = INPUT_MODE_NONE;
    _inputComplete = false;
    return result;
}

bool SerialInputHandler::readYesNo(const char *prompt, bool defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;
    
    _inputDefaultBool = defaultValue;
    
    beginInputMode(INPUT_MODE_YES_NO, prompt);
    
    unsigned long startTime = millis();
    unsigned long timeoutMs = (timeout > 0) ? timeout : _timeout;
    
    while (!_inputComplete && ((millis() - startTime) < timeoutMs)) {
        if (_serial->available() > 0) {
            processSerialInput();
        }
        delay(10);
    }
    
    bool result = _inputComplete ? _inputDefaultBool : defaultValue;
    
    _inputMode = INPUT_MODE_NONE;
    _inputComplete = false;
    return result;
}

char SerialInputHandler::readChar(const char *prompt, char defaultValue, unsigned long timeout) {
    if (!_serial) return defaultValue;
    
    _inputDefaultChar = defaultValue;
    
    beginInputMode(INPUT_MODE_CHAR, prompt);
    
    unsigned long startTime = millis();
    unsigned long timeoutMs = (timeout > 0) ? timeout : _timeout;
    
    while (!_inputComplete && ((millis() - startTime) < timeoutMs)) {
        if (_serial->available() > 0) {
            processSerialInput();
        }
        delay(10);
    }
    
    char result = _inputComplete ? _inputDefaultChar : defaultValue;
    
    _inputMode = INPUT_MODE_NONE;
    _inputComplete = false;
    return result;
}

String SerialInputHandler::readString(const char *prompt, const char *defaultValue, unsigned long timeout) {
    if (!_serial) return String(defaultValue);
    
    _inputDefaultString = String(defaultValue);
    
    beginInputMode(INPUT_MODE_STRING, prompt);
    
    unsigned long startTime = millis();
    unsigned long timeoutMs = (timeout > 0) ? timeout : _timeout;
    
    while (!_inputComplete && ((millis() - startTime) < timeoutMs)) {
        if (_serial->available() > 0) {
            processSerialInput();
        }
        delay(10);
    }
    
    String result = _inputComplete ? _inputDefaultString : String(defaultValue);
    
    _inputMode = INPUT_MODE_NONE;
    _inputComplete = false;
    return result;
}

bool SerialInputHandler::isWaitingForInput() const {
    return _inputMode != INPUT_MODE_NONE && !_inputComplete;
}

bool SerialInputHandler::handleSerialInput() {
    if (_serial && _serial->available() > 0 && isWaitingForInput()) {
        processSerialInput();
        return _inputComplete;
    }
    return false;
}

void SerialInputHandler::clearSerialBuffer() {
    if (_serial) {
        while (_serial->available()) {
            _serial->read();
        }
    }
}

void SerialInputHandler::waitForSerialInput() {
    if (_serial) {
        while (!_serial->available()) {
            delay(10);
        }
    }
}

void SerialInputHandler::beginInputMode(InputModeType mode, const char *prompt) {
    _inputMode = mode;
    _inputBuffer = "";
    _inputComplete = false;
    
    if (_serial && prompt) {
        _serial->print(prompt);
        _serial->print(": ");
    }
}

void SerialInputHandler::processSerialInput() {
    char c = _serial->read();
    
    if (c == '\n' || c == '\r') {
        if (_inputBuffer.length() > 0 || _inputMode == INPUT_MODE_CHAR) {
            completeInputMode();
        }
    } else if (c == '\b' || c == 127) {
        if (_inputBuffer.length() > 0) {
            _inputBuffer.remove(_inputBuffer.length() - 1);
            _serial->print("\b \b");
        }
    } else if (c >= 32 && c <= 126) {
        _inputBuffer += c;
        _serial->print(c);
        
        if (_inputMode == INPUT_MODE_CHAR) {
            completeInputMode();
        }
    }
}

void SerialInputHandler::completeInputMode() {
    switch (_inputMode) {
        case INPUT_MODE_FLOAT:
            if (_inputBuffer.length() > 0) {
                _inputDefaultFloat = _inputBuffer.toFloat();
            }
            break;
        
        case INPUT_MODE_INT:
            if (_inputBuffer.length() > 0) {
                _inputDefaultInt = _inputBuffer.toInt();
            }
            break;
        
        case INPUT_MODE_CHAR:
            if (_inputBuffer.length() > 0) {
                _inputDefaultChar = _inputBuffer.charAt(0);
            }
            break;
        
        case INPUT_MODE_YES_NO:
            if (_inputBuffer.length() > 0) {
                char first = _inputBuffer.charAt(0);
                _inputDefaultBool = (first == 'y' || first == 'Y' || first == '1');
            }
            break;
        
        case INPUT_MODE_STRING:
            if (_inputBuffer.length() > 0) {
                _inputDefaultString = _inputBuffer;
            }
            break;
        
        default:
            break;
    }
    
    _inputComplete = true;
    if (_serial) {
        _serial->println();
    }
}

