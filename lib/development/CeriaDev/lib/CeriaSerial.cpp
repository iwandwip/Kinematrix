#include "CeriaSerial.h"

namespace CeriaDevOP {

    CeriaSerial::CeriaSerial()
            : _serial(nullptr), _baudRate(115200), _frameSize(FRAME_AUTO), _duplexMode(FULL_DUPLEX),
              _sequenceNumber(0), _lastHeartbeat(0), _timeout(CERIA_SERIAL_DEFAULT_TIMEOUT),
              _retryCount(0), _maxRetries(3), _txBuffer(nullptr), _rxBuffer(nullptr),
              _bufferSize(0), _rxIndex(0), _inFrame(false), _escaped(false),
              _lastActivity(0), _backoffTime(0), _channelBusy(false),
              _onReceive(nullptr), _onError(nullptr),
              _framesSent(0), _framesReceived(0), _crcErrors(0), _collisions(0) {
    }

    CeriaSerial::~CeriaSerial() {
        freeBuffers();
    }

    bool CeriaSerial::begin(Stream &serial, uint32_t baud, FrameSize size) {
        _serial = &serial;
        _baudRate = baud;
        _frameSize = (size == FRAME_AUTO) ? static_cast<FrameSize>(getOptimalFrameSize()) : size;

        if (!initializeBuffers()) {
            triggerError(CERIA_SERIAL_ERR_BUFFER_FULL);
            return false;
        }

        _sequenceNumber = 0;
        _rxIndex = 0;
        _inFrame = false;
        _escaped = false;
        _lastHeartbeat = millis();

        return true;
    }

    void CeriaSerial::setMode(DuplexMode mode) {
        _duplexMode = mode;
        _backoffTime = 0;
        _channelBusy = false;
    }

    void CeriaSerial::setRetries(uint8_t count) {
        _maxRetries = count;
    }

    void CeriaSerial::onReceive(CeriaSerialReceiveCallback callback) {
        _onReceive = callback;
    }

    void CeriaSerial::onError(CeriaSerialErrorCallback callback) {
        _onError = callback;
    }

    bool CeriaSerial::sendTyped(const String &key, int value, bool reliable) {
        return sendInternal(key, String(value), CERIA_SERIAL_TYPE_INT, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, long value, bool reliable) {
        return sendInternal(key, String(value), CERIA_SERIAL_TYPE_INT, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, uint8_t value, bool reliable) {
        return sendInternal(key, String(value), CERIA_SERIAL_TYPE_INT, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, uint16_t value, bool reliable) {
        return sendInternal(key, String(value), CERIA_SERIAL_TYPE_INT, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, uint32_t value, bool reliable) {
        return sendInternal(key, String(value), CERIA_SERIAL_TYPE_INT, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, float value, bool reliable) {
        return sendInternal(key, String(value, 6), CERIA_SERIAL_TYPE_FLOAT, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, double value, bool reliable) {
        return sendInternal(key, String(value, 6), CERIA_SERIAL_TYPE_FLOAT, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, const char *value, bool reliable) {
        String strValue = String(value);
        uint8_t dataType = detectStringType(strValue);
        return sendInternal(key, strValue, dataType, reliable);
    }

    bool CeriaSerial::sendTyped(const String &key, String value, bool reliable) {
        uint8_t dataType = detectStringType(value);
        return sendInternal(key, value, dataType, reliable);
    }

    uint8_t CeriaSerial::detectStringType(String &str) {
        str.trim();
        if ((str.startsWith("{") && str.endsWith("}")) ||
            (str.startsWith("[") && str.endsWith("]"))) {
            return CERIA_SERIAL_TYPE_JSON;
        }
        return CERIA_SERIAL_TYPE_STRING;
    }

    void CeriaSerial::tick() {
        if (!_serial) return;

        while (_serial->available()) {
            processIncomingByte(_serial->read());
            _lastActivity = millis();
        }

        uint32_t now = millis();
        if (now - _lastHeartbeat > (_timeout * 2)) {
            ping();
            _lastHeartbeat = now;
        }

        if (_duplexMode == HALF_DUPLEX) {
            _channelBusy = (now - _lastActivity) < 10;
        }
    }

    bool CeriaSerial::isConnected() {
        return (_serial != nullptr) && (millis() - _lastActivity < _timeout);
    }

    void CeriaSerial::setTimeout(uint32_t ms) {
        _timeout = ms;
    }

    void CeriaSerial::ping() {
        sendInternal("ping", String(millis()), CERIA_SERIAL_CMD_PING, false);
    }

    void CeriaSerial::getStats(uint32_t &sent, uint32_t &received, uint32_t &errors) {
        sent = _framesSent;
        received = _framesReceived;
        errors = _crcErrors + _collisions;
    }

    void CeriaSerial::reset() {
        _framesSent = 0;
        _framesReceived = 0;
        _crcErrors = 0;
        _collisions = 0;
        _rxIndex = 0;
        _inFrame = false;
        _escaped = false;
        _sequenceNumber = 0;
    }

    bool CeriaSerial::sendInternal(const String &key, const String &value, uint8_t dataType, bool reliable) {
        if (!_serial) return false;

        if (_duplexMode == HALF_DUPLEX) {
            if (!isChannelClear()) {
                handleCollision();
                return false;
            }
        }

        String payload = key + ":" + value;
        uint16_t payloadLen = payload.length();

        if (payloadLen + 6 > _bufferSize) {
            triggerError(CERIA_SERIAL_ERR_FRAME_TOO_LARGE);
            return false;
        }

        uint16_t frameIndex = 0;

        _txBuffer[frameIndex++] = CERIA_SERIAL_STX;
        _txBuffer[frameIndex++] = payloadLen & 0xFF;

        uint8_t flags = (CERIA_SERIAL_CMD_DATA << 5) | (dataType << 2) | (_sequenceNumber & 0x03);
        _txBuffer[frameIndex++] = flags;

        for (uint16_t i = 0; i < payloadLen; i++) {
            uint8_t byte = payload[i];
            if (byte == CERIA_SERIAL_STX || byte == CERIA_SERIAL_ETX || byte == CERIA_SERIAL_ESC) {
                _txBuffer[frameIndex++] = CERIA_SERIAL_ESC;
                _txBuffer[frameIndex++] = byte ^ CERIA_SERIAL_XOR;
            } else {
                _txBuffer[frameIndex++] = byte;
            }
        }

        uint16_t crc = calculateCRC16(_txBuffer + 1, frameIndex - 1);
        _txBuffer[frameIndex++] = crc & 0xFF;
        _txBuffer[frameIndex++] = (crc >> 8) & 0xFF;

        _txBuffer[frameIndex++] = CERIA_SERIAL_ETX;

        bool success = sendFrame(_txBuffer, frameIndex, reliable);
        if (success) {
            _framesSent++;
            _sequenceNumber = (_sequenceNumber + 1) & 0x03;
        }

        return success;
    }

    uint16_t CeriaSerial::getOptimalFrameSize() {
#if defined(ESP32)
        return FRAME_256B;
#elif defined(ESP8266)
        return FRAME_128B;
#else
        return FRAME_64B;
#endif
    }

    bool CeriaSerial::initializeBuffers() {
        freeBuffers();

        _bufferSize = static_cast<uint16_t>(_frameSize);

        if (_bufferSize <= 128) {
            static uint8_t staticTxBuffer[128];
            static uint8_t staticRxBuffer[128];
            _txBuffer = staticTxBuffer;
            _rxBuffer = staticRxBuffer;
        } else {
            _txBuffer = new uint8_t[_bufferSize];
            _rxBuffer = new uint8_t[_bufferSize];

            if (!_txBuffer || !_rxBuffer) {
                freeBuffers();
                return false;
            }
        }

        return true;
    }

    void CeriaSerial::freeBuffers() {
        if (_bufferSize > 128) {
            delete[] _txBuffer;
            delete[] _rxBuffer;
        }
        _txBuffer = nullptr;
        _rxBuffer = nullptr;
        _bufferSize = 0;
    }

    uint16_t CeriaSerial::calculateCRC16(const uint8_t *data, uint16_t length) {
        uint16_t crc = 0xFFFF;

        for (uint16_t i = 0; i < length; i++) {
            crc ^= data[i];
            for (uint8_t j = 0; j < 8; j++) {
                if (crc & 0x0001) {
                    crc = (crc >> 1) ^ 0xA001;
                } else {
                    crc >>= 1;
                }
            }
        }

        return crc;
    }

    bool CeriaSerial::sendFrame(const uint8_t *frame, uint16_t length, bool reliable) {
        if (!_serial) return false;

        uint8_t attempts = 0;

        do {
            _serial->write(frame, length);
            _serial->flush();

            if (!reliable) {
                return true;
            }

            uint32_t startTime = millis();
            while (millis() - startTime < _timeout) {
                tick();
            }

            attempts++;
        } while (attempts < _maxRetries);

        return attempts < _maxRetries;
    }

    void CeriaSerial::processIncomingByte(uint8_t byte) {
        if (!_inFrame) {
            if (byte == CERIA_SERIAL_STX) {
                _inFrame = true;
                _rxIndex = 0;
                _escaped = false;
            }
            return;
        }

        if (_escaped) {
            byte ^= CERIA_SERIAL_XOR;
            _escaped = false;
        } else if (byte == CERIA_SERIAL_ESC) {
            _escaped = true;
            return;
        } else if (byte == CERIA_SERIAL_ETX) {
            processCompleteFrame();
            _inFrame = false;
            return;
        }

        if (_rxIndex < _bufferSize - 1) {
            _rxBuffer[_rxIndex++] = byte;
        } else {
            _inFrame = false;
            triggerError(CERIA_SERIAL_ERR_BUFFER_FULL);
        }
    }

    void CeriaSerial::processCompleteFrame() {
        if (_rxIndex < 3) return;

        uint8_t length = _rxBuffer[0];
        uint8_t flags = _rxBuffer[1];

        uint8_t cmd = (flags >> 5) & 0x07;
        uint8_t dataType = (flags >> 2) & 0x07;
        uint8_t seq = flags & 0x03;

        uint16_t receivedCRC = _rxBuffer[_rxIndex - 2] | (_rxBuffer[_rxIndex - 1] << 8);
        uint16_t calculatedCRC = calculateCRC16(_rxBuffer, _rxIndex - 2);

        if (receivedCRC != calculatedCRC) {
            _crcErrors++;
            triggerError(CERIA_SERIAL_ERR_CRC_FAILED);
            return;
        }

        String payload = "";
        for (uint16_t i = 2; i < _rxIndex - 2; i++) {
            payload += (char) _rxBuffer[i];
        }

        int separatorIndex = payload.indexOf(':');
        if (separatorIndex == -1) return;

        String key = payload.substring(0, separatorIndex);
        String value = payload.substring(separatorIndex + 1);

        switch (cmd) {
            case CERIA_SERIAL_CMD_DATA:
                if (_onReceive) {
                    _onReceive(key, value);
                }
                _framesReceived++;
                break;

            case CERIA_SERIAL_CMD_PING:
                sendInternal("pong", value, CERIA_SERIAL_TYPE_STRING, false);
                break;

            case CERIA_SERIAL_CMD_ACK:
                break;

            default:
                break;
        }
    }

    bool CeriaSerial::isChannelClear() {
        return !_channelBusy && (millis() - _lastActivity > _backoffTime);
    }

    void CeriaSerial::handleCollision() {
        _collisions++;

        uint8_t collisionLimit = (_collisions < 4) ? _collisions : 4;
        _backoffTime = random(10, 50) * (1 << collisionLimit);

        triggerError(CERIA_SERIAL_ERR_COLLISION);
    }

    void CeriaSerial::triggerError(CeriaSerialError error) {
        if (_onError) {
            _onError(error);
        }
    }

}