#include "nema-serial.h"

NemaSerial::NemaSerial() :
        _serial(NULL),
        _sequenceNumber(0),
        _receiveBuffer(NULL),
        _receiveBufferSize(0),
        _receiveBufferIndex(0),
        _transmitBuffer(NULL),
        _transmitBufferSize(0),
        _transmitBufferIndex(0),
        _transmitBufferCapacity(0),
        _escape(false),
        _inPacket(false),
        _timeout(1000),
        _maxRetries(3),
        _requireAck(true),
        _debugLevel(DEBUG_ERRORS),
        _debugStream(NULL),
        _packetsSent(0),
        _packetsReceived(0),
        _packetsRetried(0),
        _packetErrors(0),
        _lastError(ERR_NONE),
        _packetHandlerCallback(NULL),
        _errorHandlerCallback(NULL),
        _jsonHandlerCallback(NULL) {
}

NemaSerial::~NemaSerial() {
    end();
}

bool NemaSerial::begin(Stream *serialPort, uint16_t receiveBufferSize) {
    _serial = serialPort;

    if (_receiveBuffer != NULL) {
        free(_receiveBuffer);
        _receiveBuffer = NULL;
    }

    if (_transmitBuffer != NULL) {
        free(_transmitBuffer);
        _transmitBuffer = NULL;
    }

    _receiveBuffer = (uint8_t *) malloc(receiveBufferSize);

    if (_receiveBuffer == NULL) {
        _lastError = ERR_MEMORY;
        if (_errorHandlerCallback) {
            _errorHandlerCallback(ERR_MEMORY);
        }
        return false;
    }

    _receiveBufferSize = receiveBufferSize;
    resetReceiveBuffer();
    return true;
}

void NemaSerial::end() {
    if (_receiveBuffer != NULL) {
        free(_receiveBuffer);
        _receiveBuffer = NULL;
        _receiveBufferSize = 0;
    }

    if (_transmitBuffer != NULL) {
        free(_transmitBuffer);
        _transmitBuffer = NULL;
        _transmitBufferCapacity = 0;
        _transmitBufferSize = 0;
    }

    _serial = NULL;
}

bool NemaSerial::resizeReceiveBuffer(uint16_t newSize) {
    if (newSize == 0 || newSize == _receiveBufferSize) {
        return true;
    }

    uint8_t *newBuffer = (uint8_t *) realloc(_receiveBuffer, newSize);

    if (newBuffer == NULL) {
        _lastError = ERR_MEMORY;
        if (_errorHandlerCallback) {
            _errorHandlerCallback(ERR_MEMORY);
        }
        return false;
    }

    _receiveBuffer = newBuffer;
    _receiveBufferSize = newSize;

    if (_receiveBufferIndex >= _receiveBufferSize) {
        resetReceiveBuffer();
    }

    return true;
}

bool NemaSerial::resizeTransmitBuffer(uint16_t minRequiredSize) {
    if (_transmitBuffer == NULL) {
        uint16_t initialSize = max(minRequiredSize, (uint16_t) DEFAULT_CHUNK_SIZE);
        _transmitBuffer = (uint8_t *) malloc(initialSize);

        if (_transmitBuffer == NULL) {
            _lastError = ERR_MEMORY;
            if (_errorHandlerCallback) {
                _errorHandlerCallback(ERR_MEMORY);
            }
            return false;
        }

        _transmitBufferCapacity = initialSize;
        return true;
    } else if (minRequiredSize > _transmitBufferCapacity) {
        uint16_t newSize = max(uint16_t(minRequiredSize), uint16_t(_transmitBufferCapacity * 2));
        uint8_t *newBuffer = (uint8_t *) realloc(_transmitBuffer, newSize);

        if (newBuffer == NULL) {
            _lastError = ERR_MEMORY;
            if (_errorHandlerCallback) {
                _errorHandlerCallback(ERR_MEMORY);
            }
            return false;
        }

        _transmitBuffer = newBuffer;
        _transmitBufferCapacity = newSize;
    }

    return true;
}

bool NemaSerial::setReceiveBufferSize(uint16_t newSize) {
    return resizeReceiveBuffer(newSize);
}

uint16_t NemaSerial::getReceiveBufferSize() const {
    return _receiveBufferSize;
}

uint16_t NemaSerial::getTransmitBufferSize() const {
    return _transmitBufferSize;
}

uint16_t NemaSerial::calculateCRC(uint8_t *buffer, uint16_t length) {
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < length; i++) {
        crc ^= buffer[i] << 8;

        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }

    return crc;
}

void NemaSerial::resetReceiveBuffer() {
    _receiveBufferIndex = 0;
    _escape = false;
    _inPacket = false;
}

void NemaSerial::debugPrint(uint8_t level, const char *message) {
    if (_debugLevel >= level && _debugStream) {
        _debugStream->println(message);
    }
}

void NemaSerial::debugPrintHex(uint8_t level, uint8_t *buffer, uint16_t length) {
    if (_debugLevel >= level && _debugStream) {
        for (uint16_t i = 0; i < length; i++) {
            _debugStream->print(buffer[i], HEX);
            _debugStream->print(" ");

            if ((i + 1) % 16 == 0) {
                _debugStream->println();
            }
        }
        _debugStream->println();
    }
}

bool NemaSerial::processIncomingByte(uint8_t inByte) {
    if (_receiveBuffer == NULL || _receiveBufferSize == 0) {
        return false;
    }

    if (inByte == PKT_START_MARKER && !_inPacket) {
        resetReceiveBuffer();
        _inPacket = true;
        _receiveBuffer[_receiveBufferIndex++] = inByte;
        return false;
    }

    if (!_inPacket) {
        return false;
    }

    if (inByte == PKT_ESCAPE_CHAR && !_escape) {
        _escape = true;
        return false;
    }

    if (_escape) {
        inByte ^= PKT_XOR_CHAR;
        _escape = false;
    }

    if (inByte == PKT_END_MARKER && !_escape) {
        if (_receiveBufferIndex >= _receiveBufferSize) {
            if (!resizeReceiveBuffer(_receiveBufferIndex + 1)) {
                resetReceiveBuffer();
                return false;
            }
        }

        _receiveBuffer[_receiveBufferIndex++] = inByte;

        if (_receiveBufferIndex < 6) {
            debugPrint(DEBUG_ERRORS, "Packet too short");
            _lastError = ERR_LENGTH;
            _packetErrors++;
            if (_errorHandlerCallback) {
                _errorHandlerCallback(ERR_LENGTH);
            }
            return false;
        }

        uint8_t dataLength = _receiveBuffer[2];

        if (_receiveBufferIndex != dataLength + PKT_HEADER_SIZE + 3) {
            debugPrint(DEBUG_ERRORS, "Length mismatch");
            _lastError = ERR_LENGTH;
            _packetErrors++;
            if (_errorHandlerCallback) {
                _errorHandlerCallback(ERR_LENGTH);
            }
            return false;
        }

        uint16_t calculatedCRC = calculateCRC(_receiveBuffer + 1, dataLength + PKT_HEADER_SIZE - 1);
        uint16_t receivedCRC = (_receiveBuffer[_receiveBufferIndex - 3] << 8) | _receiveBuffer[_receiveBufferIndex - 2];

        if (calculatedCRC != receivedCRC) {
            debugPrint(DEBUG_ERRORS, "CRC error");
            _lastError = ERR_CRC;
            _packetErrors++;
            if (_errorHandlerCallback) {
                _errorHandlerCallback(ERR_CRC);
            }
            return false;
        }

        _packetsReceived++;
        _inPacket = false;
        return true;
    }

    if (_receiveBufferIndex >= _receiveBufferSize) {
        uint16_t newSize = _receiveBufferSize + DEFAULT_CHUNK_SIZE;
        if (!resizeReceiveBuffer(newSize)) {
            debugPrint(DEBUG_ERRORS, "Buffer overflow - resize failed");
            _lastError = ERR_BUFFER_OVERFLOW;
            _packetErrors++;
            if (_errorHandlerCallback) {
                _errorHandlerCallback(ERR_BUFFER_OVERFLOW);
            }
            resetReceiveBuffer();
            return false;
        }
    }

    _receiveBuffer[_receiveBufferIndex++] = inByte;
    return false;
}

bool NemaSerial::addToTransmitBuffer(uint8_t *data, uint16_t length) {
    if (data == NULL || length == 0) {
        return true;
    }

    uint16_t requiredSize = _transmitBufferSize + length;

    if (!resizeTransmitBuffer(requiredSize)) {
        return false;
    }

    memcpy(_transmitBuffer + _transmitBufferSize, data, length);
    _transmitBufferSize += length;

    return true;
}

void NemaSerial::sendAck(uint8_t sequenceNumber) {
    uint8_t ackData = CMD_ACK;
    sendPacket(CMD_PING, &ackData, 1);
}

void NemaSerial::sendNak(uint8_t sequenceNumber, uint8_t errorCode) {
    uint8_t nakData[2] = {CMD_NAK, errorCode};
    sendPacket(CMD_PING, nakData, 2);
}

void NemaSerial::setTimeout(unsigned long timeout) {
    _timeout = timeout;
}

void NemaSerial::setMaxRetries(uint8_t retries) {
    _maxRetries = retries;
}

void NemaSerial::setRequireAck(bool require) {
    _requireAck = require;
}

void NemaSerial::setDebugLevel(uint8_t level) {
    _debugLevel = level;
}

void NemaSerial::setDebugStream(Stream &stream) {
    _debugStream = &stream;
}

void NemaSerial::setPacketHandler(PacketHandlerFunction callback) {
    _packetHandlerCallback = callback;
}

void NemaSerial::setErrorHandler(ErrorHandlerFunction callback) {
    _errorHandlerCallback = callback;
}

void NemaSerial::setJsonHandler(JsonHandlerFunction callback) {
    _jsonHandlerCallback = callback;
}

void NemaSerial::dumpStats() {
    if (_debugStream) {
        _debugStream->println(F("=== Packet Statistics ==="));
        _debugStream->print(F("Receive Buffer Size: "));
        _debugStream->println(_receiveBufferSize);
        _debugStream->print(F("Transmit Buffer Capacity: "));
        _debugStream->println(_transmitBufferCapacity);
        _debugStream->print(F("Packets Sent: "));
        _debugStream->println(_packetsSent);
        _debugStream->print(F("Packets Received: "));
        _debugStream->println(_packetsReceived);
        _debugStream->print(F("Packets Retried: "));
        _debugStream->println(_packetsRetried);
        _debugStream->print(F("Packet Errors: "));
        _debugStream->println(_packetErrors);
        _debugStream->print(F("Last Error: "));
        _debugStream->println(getErrorString(_lastError));
        _debugStream->println(F("========================"));
    }
}

void NemaSerial::resetStats() {
    _packetsSent = 0;
    _packetsReceived = 0;
    _packetsRetried = 0;
    _packetErrors = 0;
    _lastError = ERR_NONE;
}

uint8_t NemaSerial::getLastError() const {
    return _lastError;
}

const char *NemaSerial::getErrorString(uint8_t errorCode) const {
    switch (errorCode) {
        case ERR_NONE:
            return "No Error";
        case ERR_TIMEOUT:
            return "Timeout";
        case ERR_CRC:
            return "CRC Error";
        case ERR_LENGTH:
            return "Length Error";
        case ERR_COMMAND:
            return "Invalid Command";
        case ERR_BUFFER_OVERFLOW:
            return "Buffer Overflow";
        case ERR_FORMAT:
            return "Format Error";
        case ERR_SEQUENCE:
            return "Sequence Error";
        case ERR_BUSY:
            return "Device Busy";
        case ERR_JSON:
            return "JSON Parse Error";
        case ERR_MEMORY:
            return "Memory Allocation Error";
        default:
            return "Unknown Error";
    }
}

bool NemaSerial::sendPacket(uint8_t command, uint8_t *data, uint16_t length) {
    if (!_serial) return false;

    uint16_t packetSize = 1 + 1 + 1 + length + 2 + 1;
    uint8_t *packet = (uint8_t *) malloc(packetSize);

    if (packet == NULL) {
        _lastError = ERR_MEMORY;
        if (_errorHandlerCallback) {
            _errorHandlerCallback(ERR_MEMORY);
        }
        return false;
    }

    uint16_t packetIndex = 0;

    packet[packetIndex++] = PKT_START_MARKER;
    packet[packetIndex++] = command;
    packet[packetIndex++] = length;

    for (uint16_t i = 0; i < length; i++) {
        packet[packetIndex++] = data[i];
    }

    uint16_t crc = calculateCRC(packet + 1, packetIndex - 1);
    packet[packetIndex++] = (crc >> 8) & 0xFF;
    packet[packetIndex++] = crc & 0xFF;

    packet[packetIndex++] = PKT_END_MARKER;

    if (_debugLevel >= DEBUG_INFO) {
        debugPrint(DEBUG_INFO, "Sending packet:");
        debugPrintHex(DEBUG_INFO, packet, packetIndex);
    }

    _serial->write(PKT_START_MARKER);

    for (uint16_t i = 1; i < packetIndex - 1; i++) {
        uint8_t b = packet[i];

        if (b == PKT_START_MARKER || b == PKT_END_MARKER || b == PKT_ESCAPE_CHAR) {
            _serial->write(PKT_ESCAPE_CHAR);
            _serial->write(b ^ PKT_XOR_CHAR);
        } else {
            _serial->write(b);
        }
    }

    _serial->write(PKT_END_MARKER);
    free(packet);

    _packetsSent++;
    _sequenceNumber = (_sequenceNumber + 1) & 0xFF;

    if (_requireAck) {
        unsigned long startTime = millis();
        uint8_t retries = 0;
        bool ackReceived = false;

        while (!ackReceived && retries < _maxRetries) {
            uint8_t respCmd;
            uint8_t respData[32];
            uint16_t respLen = 32;

            if (receivePacket(&respCmd, respData, &respLen, _timeout)) {
                if (respCmd == CMD_PING && respLen >= 1 && respData[0] == CMD_ACK) {
                    ackReceived = true;
                    debugPrint(DEBUG_INFO, "ACK received");
                } else if (respCmd == CMD_PING && respLen >= 2 && respData[0] == CMD_NAK) {
                    _lastError = respData[1];
                    debugPrint(DEBUG_ERRORS, "NAK received");
                    if (_errorHandlerCallback) {
                        _errorHandlerCallback(respData[1]);
                    }
                    retries++;
                    _packetsRetried++;
                    delay(50);
                }
            } else {
                _lastError = ERR_TIMEOUT;
                debugPrint(DEBUG_ERRORS, "ACK timeout");
                if (_errorHandlerCallback) {
                    _errorHandlerCallback(ERR_TIMEOUT);
                }
                retries++;
                _packetsRetried++;
                delay(50);
            }
        }

        return ackReceived;
    }

    return true;
}

bool NemaSerial::receivePacket(uint8_t *command, uint8_t *data, uint16_t *length, unsigned long timeout) {
    if (!_serial || _receiveBuffer == NULL || _receiveBufferSize == 0) return false;

    if (timeout == 0) {
        timeout = _timeout;
    }

    unsigned long startTime = millis();
    resetReceiveBuffer();

    while (millis() - startTime < timeout) {
        if (_serial->available()) {
            uint8_t inByte = _serial->read();

            if (_debugLevel >= DEBUG_VERBOSE) {
                if (_debugStream) {
                    _debugStream->print(inByte, HEX);
                    _debugStream->print(" ");
                }
            }

            if (processIncomingByte(inByte)) {
                *command = _receiveBuffer[1];

                uint8_t dataLength = _receiveBuffer[2];

                if (*length < dataLength) {
                    *length = dataLength;
                    _lastError = ERR_BUFFER_OVERFLOW;
                    if (_errorHandlerCallback) {
                        _errorHandlerCallback(ERR_BUFFER_OVERFLOW);
                    }
                    return false;
                }

                *length = dataLength;

                if (dataLength > 0 && data != NULL) {
                    memcpy(data, _receiveBuffer + 3, dataLength);
                }

                if (_requireAck) {
                    sendAck(_receiveBuffer[3]);
                }

                if (*command == CMD_JSON_DATA && _jsonHandlerCallback) {
                    JsonDocument doc;
                    if (parseJson(data, *length, doc)) {
                        _jsonHandlerCallback(doc);
                    }
                }

                if (_packetHandlerCallback) {
                    _packetHandlerCallback(*command, data, *length);
                }

                return true;
            }
        }

        yield();
    }

    _lastError = ERR_TIMEOUT;
    if (_errorHandlerCallback) {
        _errorHandlerCallback(ERR_TIMEOUT);
    }
    return false;
}

void NemaSerial::processIncoming() {
    if (!_serial || _receiveBuffer == NULL || _receiveBufferSize == 0) return;

    while (_serial->available()) {
        uint8_t inByte = _serial->read();

        if (_debugLevel >= DEBUG_VERBOSE) {
            if (_debugStream) {
                _debugStream->print(inByte, HEX);
                _debugStream->print(" ");
            }
        }

        if (processIncomingByte(inByte)) {
            uint8_t command = _receiveBuffer[1];
            uint8_t dataLength = _receiveBuffer[2];

            uint8_t *tempData = (uint8_t *) malloc(dataLength);

            if (tempData == NULL) {
                _lastError = ERR_MEMORY;
                if (_errorHandlerCallback) {
                    _errorHandlerCallback(ERR_MEMORY);
                }
                continue;
            }

            memcpy(tempData, _receiveBuffer + 3, dataLength);

            if (_requireAck) {
                sendAck(_receiveBuffer[3]);
            }

            if (command == CMD_JSON_DATA && _jsonHandlerCallback) {
                JsonDocument doc;
                if (parseJson(tempData, dataLength, doc)) {
                    _jsonHandlerCallback(doc);
                }
            }

            if (_packetHandlerCallback) {
                _packetHandlerCallback(command, tempData, dataLength);
            }

            free(tempData);
        }
    }
}

bool NemaSerial::sendCommand(uint8_t command) {
    return sendPacket(command, NULL, 0);
}

bool NemaSerial::ping() {
    uint8_t pingData = 0;
    return sendPacket(CMD_PING, &pingData, 1);
}

bool NemaSerial::readRegister(uint16_t reg, uint8_t *value) {
    uint8_t regData[2];
    regData[0] = (reg >> 8) & 0xFF;
    regData[1] = reg & 0xFF;

    if (!sendPacket(CMD_REGISTER_READ, regData, 2)) {
        return false;
    }

    uint8_t respCmd;
    uint8_t respData[4];
    uint16_t respLen = 4;

    if (receivePacket(&respCmd, respData, &respLen)) {
        if (respCmd == CMD_REGISTER_READ && respLen >= 3) {
            uint16_t respReg = (respData[0] << 8) | respData[1];

            if (respReg == reg) {
                *value = respData[2];
                return true;
            }
        }
    }

    return false;
}

bool NemaSerial::writeRegister(uint16_t reg, uint8_t value) {
    uint8_t regData[3];
    regData[0] = (reg >> 8) & 0xFF;
    regData[1] = reg & 0xFF;
    regData[2] = value;

    return sendPacket(CMD_REGISTER_WRITE, regData, 3);
}

NemaSerial &NemaSerial::beginPacket() {
    if (_transmitBuffer != NULL) {
        free(_transmitBuffer);
        _transmitBuffer = NULL;
        _transmitBufferCapacity = 0;
    }

    _transmitBufferSize = 0;
    _transmitBufferIndex = 0;

    return *this;
}

void NemaSerial::addInt8(int8_t value) {
    uint8_t data[2];
    data[0] = DATA_TYPE_INT8;
    data[1] = value;

    addToTransmitBuffer(data, 2);
}

NemaSerial &NemaSerial::write8(int8_t value) {
    addInt8(value);
    return *this;
}

void NemaSerial::addInt16(int16_t value) {
    uint8_t data[3];
    data[0] = DATA_TYPE_INT16;
    data[1] = value & 0xFF;
    data[2] = (value >> 8) & 0xFF;

    addToTransmitBuffer(data, 3);
}

NemaSerial &NemaSerial::write16(int16_t value) {
    addInt16(value);
    return *this;
}

void NemaSerial::addInt32(int32_t value) {
    uint8_t data[5];
    data[0] = DATA_TYPE_INT32;
    data[1] = value & 0xFF;
    data[2] = (value >> 8) & 0xFF;
    data[3] = (value >> 16) & 0xFF;
    data[4] = (value >> 24) & 0xFF;

    addToTransmitBuffer(data, 5);
}

NemaSerial &NemaSerial::write32(int32_t value) {
    addInt32(value);
    return *this;
}

void NemaSerial::addUInt8(uint8_t value) {
    uint8_t data[2];
    data[0] = DATA_TYPE_UINT8;
    data[1] = value;

    addToTransmitBuffer(data, 2);
}

NemaSerial &NemaSerial::writeU8(uint8_t value) {
    addUInt8(value);
    return *this;
}

void NemaSerial::addUInt16(uint16_t value) {
    uint8_t data[3];
    data[0] = DATA_TYPE_UINT16;
    data[1] = value & 0xFF;
    data[2] = (value >> 8) & 0xFF;

    addToTransmitBuffer(data, 3);
}

NemaSerial &NemaSerial::writeU16(uint16_t value) {
    addUInt16(value);
    return *this;
}

void NemaSerial::addUInt32(uint32_t value) {
    uint8_t data[5];
    data[0] = DATA_TYPE_UINT32;
    data[1] = value & 0xFF;
    data[2] = (value >> 8) & 0xFF;
    data[3] = (value >> 16) & 0xFF;
    data[4] = (value >> 24) & 0xFF;

    addToTransmitBuffer(data, 5);
}

NemaSerial &NemaSerial::writeU32(uint32_t value) {
    addUInt32(value);
    return *this;
}

void NemaSerial::addFloat(float value) {
    uint8_t data[5];
    data[0] = DATA_TYPE_FLOAT;

    uint8_t *valuePtr = (uint8_t *) &value;
    for (int i = 0; i < 4; i++) {
        data[i + 1] = valuePtr[i];
    }

    addToTransmitBuffer(data, 5);
}

NemaSerial &NemaSerial::writeFloat(float value) {
    addFloat(value);
    return *this;
}

void NemaSerial::addString(const char *str) {
    if (str == NULL) return;

    uint8_t len = strlen(str);
    if (len > PKT_MAX_STRING_LEN) len = PKT_MAX_STRING_LEN;

    uint8_t *data = (uint8_t *) malloc(2 + len);
    if (data == NULL) {
        _lastError = ERR_MEMORY;
        if (_errorHandlerCallback) {
            _errorHandlerCallback(ERR_MEMORY);
        }
        return;
    }

    data[0] = DATA_TYPE_STRING;
    data[1] = len;
    memcpy(&data[2], str, len);

    addToTransmitBuffer(data, 2 + len);
    free(data);
}

NemaSerial &NemaSerial::writeString(const char *str) {
    addString(str);
    return *this;
}

void NemaSerial::addBool(bool value) {
    uint8_t data[2];
    data[0] = DATA_TYPE_BOOL;
    data[1] = value ? 1 : 0;

    addToTransmitBuffer(data, 2);
}

NemaSerial &NemaSerial::writeBool(bool value) {
    addBool(value);
    return *this;
}

bool NemaSerial::endPacket(uint8_t command) {
    if (_transmitBuffer == NULL) {
        if (!resizeTransmitBuffer(1)) {
            return false;
        }
    }

    bool result = sendPacket(command, _transmitBuffer, _transmitBufferSize);

    if (_transmitBuffer != NULL) {
        free(_transmitBuffer);
        _transmitBuffer = NULL;
        _transmitBufferCapacity = 0;
        _transmitBufferSize = 0;
    }

    return result;
}

bool NemaSerial::sendJson(JsonDocument &doc, uint8_t command) {
    String jsonStr;
    serializeJson(doc, jsonStr);

    return sendPacket(command, (uint8_t *) jsonStr.c_str(), jsonStr.length());
}

bool NemaSerial::parseJson(uint8_t *data, uint16_t length, JsonDocument &doc) {
    if (data == NULL || length == 0) return false;

    String jsonStr;
    jsonStr.reserve(length + 1);

    for (uint16_t i = 0; i < length; i++) {
        jsonStr += (char) data[i];
    }

    DeserializationError error = deserializeJson(doc, jsonStr);
    if (error) {
        _lastError = ERR_JSON;
        return false;
    }

    return true;
}

int8_t NemaSerial::readInt8(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 1 >= _receiveBufferSize) return 0;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_INT8) {
        return (int8_t) data[(*index)++];
    }
    return 0;
}

int16_t NemaSerial::readInt16(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 2 >= _receiveBufferSize) return 0;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_INT16) {
        int16_t result = data[*index] | (data[*index + 1] << 8);
        *index += 2;
        return result;
    }
    return 0;
}

int32_t NemaSerial::readInt32(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 4 >= _receiveBufferSize) return 0;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_INT32) {
        int32_t result = data[*index] |
                         (data[*index + 1] << 8) |
                         (data[*index + 2] << 16) |
                         (data[*index + 3] << 24);
        *index += 4;
        return result;
    }
    return 0;
}

uint8_t NemaSerial::readUInt8(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 1 >= _receiveBufferSize) return 0;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_UINT8) {
        return data[(*index)++];
    }
    return 0;
}

uint16_t NemaSerial::readUInt16(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 2 >= _receiveBufferSize) return 0;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_UINT16) {
        uint16_t result = data[*index] | (data[*index + 1] << 8);
        *index += 2;
        return result;
    }
    return 0;
}

uint32_t NemaSerial::readUInt32(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 4 >= _receiveBufferSize) return 0;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_UINT32) {
        uint32_t result = data[*index] |
                          (data[*index + 1] << 8) |
                          (data[*index + 2] << 16) |
                          (data[*index + 3] << 24);
        *index += 4;
        return result;
    }
    return 0;
}

float NemaSerial::readFloat(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 4 >= _receiveBufferSize) return 0.0f;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_FLOAT) {
        float result;
        uint8_t *resultPtr = (uint8_t *) &result;
        for (int i = 0; i < 4; i++) {
            resultPtr[i] = data[*index + i];
        }
        *index += 4;
        return result;
    }
    return 0.0f;
}

String NemaSerial::readString(uint8_t *data, uint16_t *index) {
    String result = "";
    if (data == NULL || *index >= _receiveBufferSize) return result;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_STRING && *index < _receiveBufferSize) {
        uint8_t len = data[(*index)++];
        if (*index + len <= _receiveBufferSize) {
            for (uint8_t i = 0; i < len; i++) {
                result += (char) data[*index + i];
            }
            *index += len;
        }
    }
    return result;
}

bool NemaSerial::readBool(uint8_t *data, uint16_t *index) {
    if (data == NULL || *index + 1 >= _receiveBufferSize) return false;

    uint8_t type = data[(*index)++];
    if (type == DATA_TYPE_BOOL) {
        return data[(*index)++] != 0;
    }
    return false;
}