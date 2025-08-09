#include "CeriaSerialI2C.h"

namespace CeriaDevOP {

    // Static members initialization
    CeriaSerialI2C* CeriaSerialI2C::_instance = nullptr;
    uint8_t CeriaSerialI2C::_responseBuffer[20];
    uint8_t CeriaSerialI2C::_responseLength = 0;
    I2CDataType CeriaSerialI2C::_responseType = I2C_TYPE_UINT16;
    
    uint8_t CeriaSerialI2C::_receivedBuffer[20];
    uint8_t CeriaSerialI2C::_receivedLength = 0;
    I2CDataType CeriaSerialI2C::_receivedType = I2C_TYPE_UINT16;
    bool CeriaSerialI2C::_hasNewData = false;

    CeriaSerialI2C::CeriaSerialI2C() 
        : _mode(I2C_MASTER), _slaveAddress(0), _framesSent(0), _framesReceived(0), _errorCount(0) {
        _instance = this;
    }

    CeriaSerialI2C::~CeriaSerialI2C() {
        _instance = nullptr;
    }

    bool CeriaSerialI2C::begin(I2CMode mode, uint8_t slaveAddress) {
        _mode = mode;
        _slaveAddress = slaveAddress;
        
        if (_mode == I2C_MASTER) {
            Wire.begin();
            // Set conservative I2C speed for cross-platform compatibility
            Wire.setClock(100000); // 100kHz - works on all platforms
        } else {
            Wire.begin(slaveAddress);
            Wire.onReceive(onReceiveHandler);
            Wire.onRequest(onRequestHandler);
        }
        
        return true;
    }

    void CeriaSerialI2C::update() {
        // Simple update - no complex processing needed
        if (_mode == I2C_SLAVE && _hasNewData) {
            // Data is ready to be processed
        }
    }

    // MASTER SEND FUNCTIONS - Simple packet format: [TYPE][DATA]
    bool CeriaSerialI2C::send(uint8_t slaveAddr, uint16_t data) {
        if (_mode != I2C_MASTER) return false;
        
        Wire.beginTransmission(slaveAddr);
        Wire.write(I2C_TYPE_UINT16);  // Type identifier
        Wire.write((uint8_t*)&data, 2); // Data
        uint8_t result = Wire.endTransmission();
        
        if (result == 0) {
            _framesSent++;
            return true;
        } else {
            _errorCount++;
            return false;
        }
    }

    bool CeriaSerialI2C::send(uint8_t slaveAddr, float data) {
        if (_mode != I2C_MASTER) return false;
        
        Wire.beginTransmission(slaveAddr);
        Wire.write(I2C_TYPE_FLOAT);   // Type identifier
        Wire.write((uint8_t*)&data, 4); // Data
        uint8_t result = Wire.endTransmission();
        
        if (result == 0) {
            _framesSent++;
            return true;
        } else {
            _errorCount++;
            return false;
        }
    }

    bool CeriaSerialI2C::send(uint8_t slaveAddr, const String& data) {
        if (_mode != I2C_MASTER) return false;
        if (data.length() > 18) return false; // Max 18 chars (20 - type - length)
        
        Wire.beginTransmission(slaveAddr);
        Wire.write(I2C_TYPE_STRING); // Type identifier
        Wire.write(data.length());   // String length
        Wire.print(data);           // String data
        uint8_t result = Wire.endTransmission();
        
        if (result == 0) {
            _framesSent++;
            return true;
        } else {
            _errorCount++;
            return false;
        }
    }

    // MASTER REQUEST FUNCTIONS
    bool CeriaSerialI2C::request(uint8_t slaveAddr, uint16_t& data) {
        if (_mode != I2C_MASTER) return false;
        
        uint8_t received = Wire.requestFrom(slaveAddr, (uint8_t)3); // type + 2 bytes data
        if (received >= 3 && Wire.available()) {
            uint8_t type = Wire.read();
            if (type == I2C_TYPE_UINT16) {
                data = Wire.read();
                data |= (Wire.read() << 8);
                _framesReceived++;
                return true;
            }
        }
        _errorCount++;
        return false;
    }

    bool CeriaSerialI2C::request(uint8_t slaveAddr, float& data) {
        if (_mode != I2C_MASTER) return false;
        
        uint8_t received = Wire.requestFrom(slaveAddr, (uint8_t)5); // type + 4 bytes data
        if (received >= 5 && Wire.available()) {
            uint8_t type = Wire.read();
            if (type == I2C_TYPE_FLOAT) {
                uint8_t* dataPtr = (uint8_t*)&data;
                for (int i = 0; i < 4 && Wire.available(); i++) {
                    dataPtr[i] = Wire.read();
                }
                _framesReceived++;
                return true;
            }
        }
        _errorCount++;
        return false;
    }

    bool CeriaSerialI2C::request(uint8_t slaveAddr, String& data) {
        if (_mode != I2C_MASTER) return false;
        
        uint8_t received = Wire.requestFrom(slaveAddr, (uint8_t)20); // Max response size
        if (received >= 2 && Wire.available()) {
            uint8_t type = Wire.read();
            if (type == I2C_TYPE_STRING) {
                uint8_t length = Wire.read();
                if (length > 0 && length <= 18) {
                    data = "";
                    for (int i = 0; i < length && Wire.available(); i++) {
                        data += (char)Wire.read();
                    }
                    _framesReceived++;
                    return true;
                }
            }
        }
        _errorCount++;
        return false;
    }

    // SLAVE RESPONSE FUNCTIONS
    void CeriaSerialI2C::setResponseUint16(uint16_t data) {
        _responseBuffer[0] = I2C_TYPE_UINT16;
        memcpy(&_responseBuffer[1], &data, 2);
        _responseLength = 3;
        _responseType = I2C_TYPE_UINT16;
    }

    void CeriaSerialI2C::setResponseFloat(float data) {
        _responseBuffer[0] = I2C_TYPE_FLOAT;
        memcpy(&_responseBuffer[1], &data, 4);
        _responseLength = 5;
        _responseType = I2C_TYPE_FLOAT;
    }

    void CeriaSerialI2C::setResponseString(const String& data) {
        uint8_t len = (data.length() > 18) ? 18 : data.length();
        _responseBuffer[0] = I2C_TYPE_STRING;
        _responseBuffer[1] = len;
        memcpy(&_responseBuffer[2], data.c_str(), len);
        _responseLength = len + 2;
        _responseType = I2C_TYPE_STRING;
    }

    // SLAVE DATA ACCESS FUNCTIONS
    bool CeriaSerialI2C::hasNewData() {
        return _hasNewData;
    }

    bool CeriaSerialI2C::getReceivedData(uint16_t& data) {
        if (!_hasNewData || _receivedType != I2C_TYPE_UINT16) return false;
        if (_receivedLength >= 2) {
            memcpy(&data, _receivedBuffer, 2);
            _hasNewData = false;
            return true;
        }
        return false;
    }

    bool CeriaSerialI2C::getReceivedData(float& data) {
        if (!_hasNewData || _receivedType != I2C_TYPE_FLOAT) return false;
        if (_receivedLength >= 4) {
            memcpy(&data, _receivedBuffer, 4);
            _hasNewData = false;
            return true;
        }
        return false;
    }

    bool CeriaSerialI2C::getReceivedData(String& data) {
        if (!_hasNewData || _receivedType != I2C_TYPE_STRING) return false;
        if (_receivedLength > 0) {
            // Cross-platform string construction
            data = "";
            for (uint8_t i = 0; i < _receivedLength; i++) {
                data += (char)_receivedBuffer[i];
            }
            _hasNewData = false;
            return true;
        }
        return false;
    }

    I2CDataType CeriaSerialI2C::getReceivedType() {
        return _receivedType;
    }

    // UTILITY FUNCTIONS
    bool CeriaSerialI2C::isConnected(uint8_t slaveAddr) {
        if (_mode != I2C_MASTER) return false;
        
        Wire.beginTransmission(slaveAddr);
        return (Wire.endTransmission() == 0);
    }

    void CeriaSerialI2C::getStats(uint32_t& sent, uint32_t& received, uint32_t& errors) {
        sent = _framesSent;
        received = _framesReceived;
        errors = _errorCount;
    }

    void CeriaSerialI2C::reset() {
        _framesSent = 0;
        _framesReceived = 0;
        _errorCount = 0;
        _hasNewData = false;
        _receivedLength = 0;
    }

    // INTERRUPT HANDLERS - Simple and cross-platform
    void CeriaSerialI2C::onReceiveHandler(int numBytes) {
        if (_instance && Wire.available()) {
            // Read type identifier
            uint8_t type = Wire.read();
            _receivedType = (I2CDataType)type;
            
            // Read data based on type
            _receivedLength = 0;
            if (type == I2C_TYPE_UINT16 && Wire.available() >= 2) {
                _receivedBuffer[0] = Wire.read();
                _receivedBuffer[1] = Wire.read();
                _receivedLength = 2;
                _hasNewData = true;
                _instance->_framesReceived++;
            }
            else if (type == I2C_TYPE_FLOAT && Wire.available() >= 4) {
                for (int i = 0; i < 4 && Wire.available(); i++) {
                    _receivedBuffer[i] = Wire.read();
                }
                _receivedLength = 4;
                _hasNewData = true;
                _instance->_framesReceived++;
            }
            else if (type == I2C_TYPE_STRING && Wire.available()) {
                uint8_t length = Wire.read();
                if (length <= 18) {
                    for (int i = 0; i < length && Wire.available(); i++) {
                        _receivedBuffer[i] = Wire.read();
                    }
                    _receivedLength = length;
                    _hasNewData = true;
                    _instance->_framesReceived++;
                }
            }
            
            // Clear any remaining bytes
            while (Wire.available()) {
                Wire.read();
            }
        }
    }

    void CeriaSerialI2C::onRequestHandler() {
        if (_instance) {
            // Simple response cycling for testing
            static uint8_t responseCounter = 0;
            responseCounter++;
            
            switch (responseCounter % 3) {
                case 1: {
                    // Send uint16 response
                    uint8_t resp[] = {I2C_TYPE_UINT16, 0x2E, 0x16}; // 5678 in little endian
                    Wire.write(resp, 3);
                    break;
                }
                case 2: {
                    // Send float response (99.42)
                    uint8_t resp[] = {I2C_TYPE_FLOAT, 0x14, 0xAE, 0xC7, 0x42}; // 99.42 in IEEE 754
                    Wire.write(resp, 5);
                    break;
                }
                case 0: {
                    // Send string response
                    uint8_t resp[] = {I2C_TYPE_STRING, 5, 'W', 'o', 'r', 'l', 'd'};
                    Wire.write(resp, 7);
                    break;
                }
            }
        }
    }

}