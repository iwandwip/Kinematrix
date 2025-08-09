#include "CeriaDisplayDWIN.h"

namespace CeriaDevOP {

    CeriaDisplayDWIN::CeriaDisplayDWIN()
            : _serial(nullptr), _baudRate(115200), _timeout(DWIN_DEFAULT_TIMEOUT),
              _rxIndex(0), _frameComplete(false), _lastFrameTime(0),
              _variableCount(0), _lastHeartbeat(0), _lastActivity(0), _connected(false),
              _onTouch(nullptr), _onPageChange(nullptr), _onError(nullptr), _onVariable(nullptr),
              _framesSent(0), _framesReceived(0), _errorCount(0),
              _currentBrightness(100), _currentVolume(50) {

        for (uint8_t i = 0; i < DWIN_MAX_VARIABLES; i++) {
            _variables[i].address = 0;
            _variables[i].dataType = DWIN_TYPE_UINT16;
            _variables[i].value16 = 0;
            _variables[i].value32 = 0;
            _variables[i].valueFloat = 0.0;
            _variables[i].valueString[0] = '\0';
            _variables[i].lastUpdate = 0;
            _variables[i].updated = false;
            _variables[i].autoUpload = false;
        }

        _pageData.currentPage = 0;
        _pageData.previousPage = 0;
        _pageData.pageChangeTime = 0;
        _pageData.pageChanged = false;

        _touchData.vp = 0;
        _touchData.x = 0;
        _touchData.y = 0;
        _touchData.event = DWIN_TOUCH_NONE;
        _touchData.timestamp = 0;
    }

    CeriaDisplayDWIN::~CeriaDisplayDWIN() {
    }

    bool CeriaDisplayDWIN::begin(Stream &serial, uint32_t baud) {
        _serial = &serial;
        _baudRate = baud;

        _rxIndex = 0;
        _frameComplete = false;
        _lastActivity = millis();
        _lastHeartbeat = millis();

        if (!initializeDisplay()) {
            triggerError(DWIN_ERR_NO_CONNECTION);
            return false;
        }

        _connected = true;
        return true;
    }

    bool CeriaDisplayDWIN::initializeDisplay() {
        if (!_serial) return false;

        delay(100);
        sendHeartbeat();
        delay(50);

        for (uint8_t attempt = 0; attempt < 3; attempt++) {
            if (sendRegisterRead(DWIN_REG_PAGE)) {
                uint32_t startTime = millis();
                while (millis() - startTime < 1000) {
                    update();
                    if (_frameComplete) {
                        return true;
                    }
                    delay(10);
                }
            }
            delay(100);
        }
        return false;
    }

    void CeriaDisplayDWIN::update() {
        if (!_serial) return;

        while (_serial->available()) {
            processIncomingByte(_serial->read());
            _lastActivity = millis();
        }

        if (_frameComplete) {
            processCompleteFrame();
            _frameComplete = false;
            _rxIndex = 0;
        }

        uint32_t now = millis();
        if (now - _lastFrameTime > DWIN_FRAME_TIMEOUT && _rxIndex > 0) {
            _rxIndex = 0;
            triggerError(DWIN_ERR_TIMEOUT);
        }

        if (now - _lastHeartbeat > (_timeout * 2)) {
            sendHeartbeat();
            _lastHeartbeat = now;
        }

        _connected = (now - _lastActivity) < _timeout;
    }

    void CeriaDisplayDWIN::processIncomingByte(uint8_t byte) {
        if (_rxIndex == 0) {
            if (byte == DWIN_HEADER_H) {
                _rxBuffer[_rxIndex++] = byte;
                _lastFrameTime = millis();
            }
            return;
        }

        if (_rxIndex == 1) {
            if (byte == DWIN_HEADER_L) {
                _rxBuffer[_rxIndex++] = byte;
            } else {
                _rxIndex = 0;
            }
            return;
        }

        _rxBuffer[_rxIndex++] = byte;

        if (_rxIndex >= 3) {
            uint8_t expectedLength = _rxBuffer[2] + 3;
            if (_rxIndex >= expectedLength) {
                if (validateFrame(_rxBuffer, expectedLength)) {
                    _frameComplete = true;
                } else {
                    _rxIndex = 0;
                    triggerError(DWIN_ERR_INVALID_FRAME);
                }
            }
        }

        if (_rxIndex >= 64) {
            _rxIndex = 0;
            triggerError(DWIN_ERR_BUFFER_FULL);
        }
    }

    void CeriaDisplayDWIN::processCompleteFrame() {
        if (_rxIndex < 4) return;

        uint8_t command = _rxBuffer[3];
        uint8_t *data = &_rxBuffer[4];
        uint8_t dataLength = _rxBuffer[2] - 1;

        _framesReceived++;

        switch (command) {
            case DWIN_CMD_REG_READ:
                if (dataLength >= 3) {
                    uint8_t reg = data[0];
                    uint16_t value = bytesToUint16(&data[1]);

                    if (reg == DWIN_REG_PAGE) {
                        uint8_t newPage = value & 0xFF;
                        if (newPage != _pageData.currentPage) {
                            _pageData.previousPage = _pageData.currentPage;
                            _pageData.currentPage = newPage;
                            _pageData.pageChangeTime = millis();
                            _pageData.pageChanged = true;

                            if (_onPageChange) {
                                _onPageChange(_pageData.currentPage, _pageData.previousPage);
                            }
                        }
                    } else if (reg == DWIN_REG_BRIGHTNESS) {
                        _currentBrightness = value & 0xFF;
                    } else if (reg == DWIN_REG_VOLUME) {
                        _currentVolume = value & 0xFF;
                    }
                }
                break;

            case DWIN_CMD_VAR_READ:
                if (dataLength >= 4) {
                    uint16_t vp = bytesToUint16(data);
                    uint8_t length = data[2];
                    uint8_t *vpData = &data[3];

                    int8_t index = findVariableIndex(vp);
                    if (index >= 0) {
                        updateVariableValue(index, vpData, length);
                        if (_onVariable) {
                            _onVariable(vp, _variables[index].dataType, _variables[index].value32);
                        }
                    }
                }
                break;

            case DWIN_CMD_TOUCH_EVENT:
                handleTouchEvent(data, dataLength);
                break;

            default:
                break;
        }
    }

    bool CeriaDisplayDWIN::validateFrame(uint8_t *buffer, uint8_t length) {
        if (length < 4) return false;
        if (buffer[0] != DWIN_HEADER_H || buffer[1] != DWIN_HEADER_L) return false;
        if (buffer[2] + 3 != length) return false;
        return true;
    }

    uint16_t CeriaDisplayDWIN::calculateCRC16(uint8_t *data, uint8_t length) {
        uint16_t crc = 0xFFFF;
        for (uint8_t i = 0; i < length; i++) {
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

    bool CeriaDisplayDWIN::sendCommand(uint8_t cmd, uint16_t address, uint8_t *data, uint8_t dataLength) {
        if (!_serial) return false;

        uint8_t frameLength = 4 + dataLength;
        if (frameLength > 60) return false;

        _txBuffer[0] = DWIN_HEADER_H;
        _txBuffer[1] = DWIN_HEADER_L;
        _txBuffer[2] = 1 + dataLength;
        _txBuffer[3] = cmd;

        if (cmd == DWIN_CMD_VAR_WRITE || cmd == DWIN_CMD_VAR_READ) {
            uint16ToBytes(address, &_txBuffer[4]);
            if (dataLength > 0 && data) {
                for (uint8_t i = 0; i < dataLength; i++) {
                    _txBuffer[6 + i] = data[i];
                }
            }
            frameLength = 6 + dataLength;
            _txBuffer[2] = 3 + dataLength;
        } else if (cmd == DWIN_CMD_REG_WRITE || cmd == DWIN_CMD_REG_READ) {
            _txBuffer[4] = address & 0xFF;
            if (dataLength > 0 && data) {
                for (uint8_t i = 0; i < dataLength; i++) {
                    _txBuffer[5 + i] = data[i];
                }
            }
            frameLength = 5 + dataLength;
            _txBuffer[2] = 2 + dataLength;
        }

        _serial->write((const uint8_t *) _txBuffer, frameLength);
        _serial->flush();
        _framesSent++;

        return true;
    }

    bool CeriaDisplayDWIN::sendRegisterWrite(uint8_t reg, uint16_t value) {
        uint8_t data[2];
        uint16ToBytes(value, data);
        return sendCommand(DWIN_CMD_REG_WRITE, reg, data, 2);
    }

    bool CeriaDisplayDWIN::sendRegisterRead(uint8_t reg) {
        return sendCommand(DWIN_CMD_REG_READ, reg, nullptr, 0);
    }

    bool CeriaDisplayDWIN::sendVariableWrite(uint16_t vp, uint8_t *data, uint8_t length) {
        return sendCommand(DWIN_CMD_VAR_WRITE, vp, data, length);
    }

    bool CeriaDisplayDWIN::sendVariableRead(uint16_t vp, uint8_t length) {
        uint8_t lengthData[1] = {length};
        return sendCommand(DWIN_CMD_VAR_READ, vp, lengthData, 1);
    }

    int8_t CeriaDisplayDWIN::findVariableIndex(uint16_t vp) {
        for (uint8_t i = 0; i < _variableCount; i++) {
            if (_variables[i].address == vp) {
                return i;
            }
        }
        return -1;
    }

    bool CeriaDisplayDWIN::addVariable(uint16_t vp, DWINDataType type, bool autoUpload) {
        if (_variableCount >= DWIN_MAX_VARIABLES) return false;

        int8_t existing = findVariableIndex(vp);
        if (existing >= 0) {
            _variables[existing].dataType = type;
            _variables[existing].autoUpload = autoUpload;
            return true;
        }

        _variables[_variableCount].address = vp;
        _variables[_variableCount].dataType = type;
        _variables[_variableCount].autoUpload = autoUpload;
        _variables[_variableCount].updated = false;
        _variables[_variableCount].lastUpdate = 0;
        _variableCount++;

        return true;
    }

    void CeriaDisplayDWIN::updateVariableValue(uint8_t index, uint8_t *data, uint8_t length) {
        if (index >= _variableCount) return;

        _variables[index].lastUpdate = millis();
        _variables[index].updated = true;

        switch (_variables[index].dataType) {
            case DWIN_TYPE_UINT8:
                _variables[index].value16 = data[0];
                break;

            case DWIN_TYPE_UINT16:
                if (length >= 2) {
                    _variables[index].value16 = bytesToUint16(data);
                }
                break;

            case DWIN_TYPE_UINT32:
                if (length >= 4) {
                    _variables[index].value32 = bytesToUint32(data);
                }
                break;

            case DWIN_TYPE_FLOAT:
                if (length >= 4) {
                    _variables[index].valueFloat = bytesToFloat(data);
                }
                break;

            case DWIN_TYPE_STRING:
                decodeString(data, length, _variables[index].valueString);
                break;

            default:
                break;
        }
    }

    void CeriaDisplayDWIN::handleTouchEvent(uint8_t *data, uint8_t length) {
        if (length >= 6) {
            _touchData.vp = bytesToUint16(data);
            _touchData.x = bytesToUint16(&data[2]);
            _touchData.y = bytesToUint16(&data[4]);
            _touchData.event = DWIN_TOUCH_PRESS;
            _touchData.timestamp = millis();

            if (_onTouch) {
                _onTouch(_touchData.vp, _touchData.x, _touchData.event);
            }
        }
    }

    void CeriaDisplayDWIN::triggerError(DWINError error) {
        _errorCount++;
        if (_onError) {
            _onError(error);
        }
    }

    uint32_t CeriaDisplayDWIN::bytesToUint32(uint8_t *data) {
        return ((uint32_t) data[0] << 24) | ((uint32_t) data[1] << 16) |
               ((uint32_t) data[2] << 8) | data[3];
    }

    void CeriaDisplayDWIN::uint32ToBytes(uint32_t value, uint8_t *data) {
        data[0] = (value >> 24) & 0xFF;
        data[1] = (value >> 16) & 0xFF;
        data[2] = (value >> 8) & 0xFF;
        data[3] = value & 0xFF;
    }

    uint16_t CeriaDisplayDWIN::bytesToUint16(uint8_t *data) {
        return ((uint16_t) data[0] << 8) | data[1];
    }

    void CeriaDisplayDWIN::uint16ToBytes(uint16_t value, uint8_t *data) {
        data[0] = (value >> 8) & 0xFF;
        data[1] = value & 0xFF;
    }

    float CeriaDisplayDWIN::bytesToFloat(uint8_t *data) {
        union {
            float f;
            uint8_t b[4];
        } converter;

        converter.b[3] = data[0];
        converter.b[2] = data[1];
        converter.b[1] = data[2];
        converter.b[0] = data[3];

        return converter.f;
    }

    void CeriaDisplayDWIN::floatToBytes(float value, uint8_t *data) {
        union {
            float f;
            uint8_t b[4];
        } converter;

        converter.f = value;
        data[0] = converter.b[3];
        data[1] = converter.b[2];
        data[2] = converter.b[1];
        data[3] = converter.b[0];
    }

    void CeriaDisplayDWIN::encodeString(const String &str, uint8_t *buffer, uint8_t maxLength) {
        uint8_t length = str.length();
        if (length > maxLength - 1) length = maxLength - 1;

        for (uint8_t i = 0; i < length; i++) {
            buffer[i] = str[i];
        }
        buffer[length] = 0;
    }

    void CeriaDisplayDWIN::decodeString(uint8_t *buffer, uint8_t length, char *output) {
        uint8_t copyLength = length;
        if (copyLength > 31) copyLength = 31;

        for (uint8_t i = 0; i < copyLength; i++) {
            output[i] = buffer[i];
        }
        output[copyLength] = '\0';
    }

    bool CeriaDisplayDWIN::hasNewData() {
        for (uint8_t i = 0; i < _variableCount; i++) {
            if (_variables[i].updated) return true;
        }
        return false;
    }

    bool CeriaDisplayDWIN::hasNewTouch() {
        return _touchData.timestamp > 0 &&
               (millis() - _touchData.timestamp) < 100;
    }

    bool CeriaDisplayDWIN::hasNewPage() {
        if (_pageData.pageChanged) {
            _pageData.pageChanged = false;
            return true;
        }
        return false;
    }

    bool CeriaDisplayDWIN::isConnected(uint32_t timeoutMs) {
        return _connected && (millis() - _lastActivity) < timeoutMs;
    }

    void CeriaDisplayDWIN::setTimeout(uint32_t ms) {
        _timeout = ms;
    }

    void CeriaDisplayDWIN::setTouchCallback(DWINTouchCallback callback) {
        _onTouch = callback;
    }

    void CeriaDisplayDWIN::setPageCallback(DWINPageCallback callback) {
        _onPageChange = callback;
    }

    void CeriaDisplayDWIN::setErrorCallback(DWINErrorCallback callback) {
        _onError = callback;
    }

    void CeriaDisplayDWIN::setVariableCallback(DWINVariableCallback callback) {
        _onVariable = callback;
    }

    bool CeriaDisplayDWIN::setPage(uint8_t page) {
        if (page > DWIN_MAX_PAGES) {
            triggerError(DWIN_ERR_INVALID_PAGE);
            return false;
        }
        return sendRegisterWrite(DWIN_REG_PAGE, page);
    }

    uint8_t CeriaDisplayDWIN::getCurrentPage() {
        return _pageData.currentPage;
    }

    uint8_t CeriaDisplayDWIN::getPreviousPage() {
        return _pageData.previousPage;
    }

    bool CeriaDisplayDWIN::waitForPageChange(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (hasNewPage()) {
                return true;
            }
            delay(10);
        }
        return false;
    }

    bool CeriaDisplayDWIN::setBrightness(uint8_t level) {
        if (level > 100) level = 100;
        return sendRegisterWrite(DWIN_REG_BRIGHTNESS, level);
    }

    uint8_t CeriaDisplayDWIN::getBrightness() {
        return _currentBrightness;
    }

    bool CeriaDisplayDWIN::setVolume(uint8_t level) {
        if (level > 100) level = 100;
        return sendRegisterWrite(DWIN_REG_VOLUME, level);
    }

    uint8_t CeriaDisplayDWIN::getVolume() {
        return _currentVolume;
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, uint8_t value) {
        uint8_t data[2];
        uint16ToBytes(value, data);
        return sendVariableWrite(vp, data, 2);
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, uint16_t value) {
        uint8_t data[2];
        uint16ToBytes(value, data);
        return sendVariableWrite(vp, data, 2);
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, uint32_t value) {
        uint8_t data[4];
        uint32ToBytes(value, data);
        return sendVariableWrite(vp, data, 4);
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, int value) {
        return writeVP(vp, (uint32_t) value);
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, long value) {
        return writeVP(vp, (uint32_t) value);
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, float value) {
        uint8_t data[4];
        floatToBytes(value, data);
        return sendVariableWrite(vp, data, 4);
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, const String &value) {
        uint8_t data[32];
        encodeString(value, data, 32);
        return sendVariableWrite(vp, data, value.length() + 1);
    }

    bool CeriaDisplayDWIN::writeVP(uint16_t vp, const char *value) {
        return writeVP(vp, String(value));
    }

    bool CeriaDisplayDWIN::readVP(uint16_t vp, DWINDataType type) {
        uint8_t length = 2;
        switch (type) {
            case DWIN_TYPE_UINT8:
                length = 1;
                break;
            case DWIN_TYPE_UINT16:
                length = 2;
                break;
            case DWIN_TYPE_UINT32:
            case DWIN_TYPE_FLOAT:
                length = 4;
                break;
            case DWIN_TYPE_STRING:
                length = 32;
                break;
            default:
                break;
        }

        addVariable(vp, type, false);
        return sendVariableRead(vp, length);
    }

    uint8_t CeriaDisplayDWIN::getVPUint8(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            return _variables[index].value16 & 0xFF;
        }
        return 0;
    }

    uint16_t CeriaDisplayDWIN::getVPUint16(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            return _variables[index].value16;
        }
        return 0;
    }

    uint32_t CeriaDisplayDWIN::getVPUint32(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            return _variables[index].value32;
        }
        return 0;
    }

    float CeriaDisplayDWIN::getVPFloat(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            return _variables[index].valueFloat;
        }
        return 0.0;
    }

    String CeriaDisplayDWIN::getVPString(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            return String(_variables[index].valueString);
        }
        return "";
    }

    bool CeriaDisplayDWIN::registerVP(uint16_t vp, DWINDataType type, bool autoUpload) {
        return addVariable(vp, type, autoUpload);
    }

    bool CeriaDisplayDWIN::unregisterVP(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index < 0) return false;

        for (uint8_t i = index; i < _variableCount - 1; i++) {
            _variables[i] = _variables[i + 1];
        }
        _variableCount--;
        return true;
    }

    bool CeriaDisplayDWIN::isVPRegistered(uint16_t vp) {
        return findVariableIndex(vp) >= 0;
    }

    bool CeriaDisplayDWIN::isVPUpdated(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            bool updated = _variables[index].updated;
            _variables[index].updated = false;
            return updated;
        }
        return false;
    }

    uint32_t CeriaDisplayDWIN::getVPLastUpdate(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            return _variables[index].lastUpdate;
        }
        return 0;
    }

    DWINTouchData CeriaDisplayDWIN::getLastTouch() {
        return _touchData;
    }

    uint16_t CeriaDisplayDWIN::getTouchVP() {
        return _touchData.vp;
    }

    uint16_t CeriaDisplayDWIN::getTouchX() {
        return _touchData.x;
    }

    uint16_t CeriaDisplayDWIN::getTouchY() {
        return _touchData.y;
    }

    DWINTouchEvent CeriaDisplayDWIN::getTouchEvent() {
        return _touchData.event;
    }

    bool CeriaDisplayDWIN::setText(uint16_t vp, const String &text) {
        return writeVP(vp, text);
    }

    bool CeriaDisplayDWIN::setNumber(uint16_t vp, int32_t number) {
        return writeVP(vp, (uint32_t) number);
    }

    bool CeriaDisplayDWIN::setFloat(uint16_t vp, float number, uint8_t decimals) {
        return writeVP(vp, number);
    }

    bool CeriaDisplayDWIN::setIcon(uint16_t vp, uint16_t iconId) {
        return writeVP(vp, iconId);
    }

    bool CeriaDisplayDWIN::setProgress(uint16_t vp, uint8_t percentage) {
        if (percentage > 100) percentage = 100;
        return writeVP(vp, (uint16_t) percentage);
    }

    bool CeriaDisplayDWIN::enableAutoUpload(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            _variables[index].autoUpload = true;
            return true;
        }
        return false;
    }

    bool CeriaDisplayDWIN::disableAutoUpload(uint16_t vp) {
        int8_t index = findVariableIndex(vp);
        if (index >= 0) {
            _variables[index].autoUpload = false;
            return true;
        }
        return false;
    }

    void CeriaDisplayDWIN::enableHeartbeat(bool enable) {
        if (enable) {
            _lastHeartbeat = millis();
        } else {
            _lastHeartbeat = 0;
        }
    }

    void CeriaDisplayDWIN::sendHeartbeat() {
        if (_serial) {
            _serial->write((uint8_t) DWIN_HEADER_H);
            _serial->write((uint8_t) DWIN_HEADER_L);
            _serial->write((uint8_t) 0x01);
            _serial->write((uint8_t) 0x00);
            _serial->flush();
        }
    }

    void CeriaDisplayDWIN::getStats(uint32_t &sent, uint32_t &received, uint32_t &errors) {
        sent = _framesSent;
        received = _framesReceived;
        errors = _errorCount;
    }

    void CeriaDisplayDWIN::resetStats() {
        _framesSent = 0;
        _framesReceived = 0;
        _errorCount = 0;
    }

    String CeriaDisplayDWIN::getDisplayString() {
        return "Page:" + String(_pageData.currentPage) +
               ", Bright:" + String(_currentBrightness) +
               ", Vol:" + String(_currentVolume);
    }

    String CeriaDisplayDWIN::getConnectionString() {
        return String(_connected ? "Connected" : "Disconnected") +
               " (" + String(_framesSent) + "/" + String(_framesReceived) +
               ", E:" + String(_errorCount) + ")";
    }

    String CeriaDisplayDWIN::getPageString() {
        return "Current:" + String(_pageData.currentPage) +
               ", Previous:" + String(_pageData.previousPage);
    }

    String CeriaDisplayDWIN::getTouchString() {
        return "VP:" + String(_touchData.vp, HEX) +
               ", XY:(" + String(_touchData.x) + "," + String(_touchData.y) + ")";
    }

    bool CeriaDisplayDWIN::playSound(uint8_t soundId) {
        return sendRegisterWrite(0x04, soundId);
    }

    bool CeriaDisplayDWIN::setRTC(uint16_t year, uint8_t month, uint8_t day,
                                  uint8_t hour, uint8_t minute, uint8_t second) {
        uint8_t data[7];
        data[0] = year >> 8;
        data[1] = year & 0xFF;
        data[2] = month;
        data[3] = day;
        data[4] = hour;
        data[5] = minute;
        data[6] = second;
        return sendCommand(DWIN_CMD_REG_WRITE, DWIN_REG_RTC, data, 7);
    }

    bool CeriaDisplayDWIN::getRTC() {
        return sendRegisterRead(DWIN_REG_RTC);
    }

    bool CeriaDisplayDWIN::waitForConnection(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (isConnected()) {
                return true;
            }
            delay(50);
        }
        return false;
    }

    bool CeriaDisplayDWIN::waitForTouch(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (hasNewTouch()) {
                return true;
            }
            delay(10);
        }
        return false;
    }

    bool CeriaDisplayDWIN::waitForVPUpdate(uint16_t vp, uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (isVPUpdated(vp)) {
                return true;
            }
            delay(10);
        }
        return false;
    }

    void CeriaDisplayDWIN::reset() {
        _rxIndex = 0;
        _frameComplete = false;
        _variableCount = 0;
        _pageData.pageChanged = false;
        _touchData.timestamp = 0;
        resetStats();
    }

    void CeriaDisplayDWIN::factoryReset() {
        sendRegisterWrite(0xFF, 0x55AA);
    }

    bool CeriaDisplayDWIN::uploadFile(const String &filename, uint8_t *data, uint32_t length) {
        return false;
    }

    bool CeriaDisplayDWIN::downloadFile(const String &filename, uint8_t *buffer, uint32_t &length) {
        return false;
    }

    String CeriaDisplayDWIN::getVersionString() {
        return "CeriaDisplayDWIN v1.0.0";
    }

    String CeriaDisplayDWIN::getDebugString() {
        String debug = "DWIN Debug:\n";
        debug += "Connected: " + String(_connected ? "Yes" : "No") + "\n";
        debug += "Page: " + String(_pageData.currentPage) + "\n";
        debug += "Variables: " + String(_variableCount) + "/" + String(DWIN_MAX_VARIABLES) + "\n";
        debug += "Frames S/R: " + String(_framesSent) + "/" + String(_framesReceived) + "\n";
        debug += "Errors: " + String(_errorCount) + "\n";
        debug += "Last Activity: " + String(millis() - _lastActivity) + "ms ago\n";
        return debug;
    }

}