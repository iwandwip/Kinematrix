#include "lora-ebyte-e220.h"

LoRaModuleEByteE220::LoRaModuleEByteE220(HardwareSerial *serial, UART_BPS_RATE bpsRate) {
    _lora = new LoRa_E220(serial, bpsRate);
    _rssiEnabled = false;
    _lastErrorMessage = "";
}

LoRaModuleEByteE220::LoRaModuleEByteE220(HardwareSerial *serial, byte auxPin, UART_BPS_RATE bpsRate) {
    _lora = new LoRa_E220(serial, auxPin, bpsRate);
    _rssiEnabled = false;
    _lastErrorMessage = "";
}

LoRaModuleEByteE220::LoRaModuleEByteE220(HardwareSerial *serial, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate) {
    _lora = new LoRa_E220(serial, auxPin, m0Pin, m1Pin, bpsRate);
    _rssiEnabled = false;
    _lastErrorMessage = "";
}

#ifdef ACTIVATE_SOFTWARE_SERIAL
LoRaModuleEByteE220::LoRaModuleEByteE220(SoftwareSerial* serial, UART_BPS_RATE bpsRate) {
    _lora = new LoRa_E220(serial, bpsRate);
    _rssiEnabled = false;
    _lastErrorMessage = "";
}
#endif

LoRaModuleEByteE220::~LoRaModuleEByteE220() {
    if (_lora != nullptr) {
        delete _lora;
        _lora = nullptr;
    }
}

bool LoRaModuleEByteE220::begin() {
    if (_lora == nullptr) return false;
    return _lora->begin();
}

bool LoRaModuleEByteE220::getConfiguration(LoRaConfig *config) {
    if (!config || _lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *loraConfig = (Configuration *) rsc.data;
    *config = _convertFromConfiguration(*loraConfig);
    rsc.close();
    return true;
}

bool LoRaModuleEByteE220::setConfiguration(LoRaConfig config, bool permanent) {
    if (_lora == nullptr) return false;

    Configuration loraConfig = _convertToConfiguration(config);
    PROGRAM_COMMAND saveType = permanent ? WRITE_CFG_PWR_DWN_SAVE : WRITE_CFG_PWR_DWN_LOSE;

    ResponseStatus rs = _lora->setConfiguration(loraConfig, saveType);
    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::resetModule() {
    if (_lora == nullptr) return false;

    ResponseStatus rs = _lora->resetModule();
    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::sendMessage(const String &message) {
    if (_lora == nullptr) return false;

    ResponseStatus rs = _lora->sendMessage(message);
    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::receiveMessage(String *message, int *rssi) {
    if (!message || _lora == nullptr) return false;

    if (_rssiEnabled && rssi != NULL) {
        ResponseContainer rc = _lora->receiveMessageRSSI();
        if (rc.status.code != E220_SUCCESS) {
            _lastErrorMessage = rc.status.getResponseDescription();
            return false;
        }

        *message = rc.data;
        *rssi = rc.rssi;
    } else {
        ResponseContainer rc = _lora->receiveMessage();
        if (rc.status.code != E220_SUCCESS) {
            _lastErrorMessage = rc.status.getResponseDescription();
            return false;
        }

        *message = rc.data;
    }

    return true;
}

bool LoRaModuleEByteE220::receiveMessageUntil(String *message, char delimiter, int *rssi) {
    if (!message || _lora == nullptr) return false;

    if (_rssiEnabled && rssi != NULL) {
        ResponseContainer rc = _lora->receiveMessageUntil(delimiter);
        if (rc.status.code != E220_SUCCESS) {
            _lastErrorMessage = rc.status.getResponseDescription();
            return false;
        }

        *message = rc.data;
        *rssi = 0;
    } else {
        ResponseContainer rc = _lora->receiveMessageUntil(delimiter);
        if (rc.status.code != E220_SUCCESS) {
            _lastErrorMessage = rc.status.getResponseDescription();
            return false;
        }

        *message = rc.data;
    }

    return true;
}

bool LoRaModuleEByteE220::sendFixedMessage(uint8_t addh, uint8_t addl, uint8_t channel, const String &message) {
    if (_lora == nullptr) return false;

    ResponseStatus rs = _lora->sendFixedMessage(addh, addl, channel, message);
    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::sendBroadcastMessage(uint8_t channel, const String &message) {
    if (_lora == nullptr) return false;

    ResponseStatus rs = _lora->sendBroadcastFixedMessage(channel, message);
    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::setAddress(uint8_t high, uint8_t low) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->ADDH = high;
    config->ADDL = low;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::setChannel(uint8_t channel) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->CHAN = channel;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::setAirDataRate(uint8_t rate) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->SPED.airDataRate = rate;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::setUartBaudRate(uint8_t rate) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->SPED.uartBaudRate = rate;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::setTransmissionPower(uint8_t power) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->OPTION.transmissionPower = power;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::setTransmissionMode(bool fixed) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->TRANSMISSION_MODE.fixedTransmission = fixed ? FT_FIXED_TRANSMISSION : FT_TRANSPARENT_TRANSMISSION;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    return _checkResponseStatus(rs);
}

bool LoRaModuleEByteE220::configureWOR(uint8_t period) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->TRANSMISSION_MODE.WORPeriod = period;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    return _checkResponseStatus(rs);
}

int LoRaModuleEByteE220::available() {
    if (_lora == nullptr) return 0;
    return _lora->available();
}

bool LoRaModuleEByteE220::hasError() const {
    return _lastErrorMessage.length() > 0;
}

void LoRaModuleEByteE220::clearError() {
    _lastErrorMessage = "";
}

String LoRaModuleEByteE220::getLastError() const {
    return _lastErrorMessage;
}

bool LoRaModuleEByteE220::isRSSIEnabled() const {
    return _rssiEnabled;
}

bool LoRaModuleEByteE220::enableRSSI(bool enable) {
    if (_lora == nullptr) return false;

    ResponseStructContainer rsc = _lora->getConfiguration();
    if (rsc.status.code != E220_SUCCESS) {
        _lastErrorMessage = rsc.status.getResponseDescription();
        rsc.close();
        return false;
    }

    Configuration *config = (Configuration *) rsc.data;
    config->TRANSMISSION_MODE.enableRSSI = enable ? RSSI_ENABLED : RSSI_DISABLED;

    ResponseStatus rs = _lora->setConfiguration(*config, WRITE_CFG_PWR_DWN_LOSE);
    rsc.close();

    if (rs.code != E220_SUCCESS) {
        _lastErrorMessage = rs.getResponseDescription();
        return false;
    }

    _rssiEnabled = enable;
    return true;
}

bool LoRaModuleEByteE220::waitAvailableTimeout(uint32_t timeout) {
    if (_lora == nullptr) return false;

    uint32_t startTime = millis();
    while (millis() - startTime < timeout) {
        if (available() > 0) {
            return true;
        }
        delay(10);
    }

    _lastErrorMessage = "Timeout waiting for data";
    return false;
}

bool LoRaModuleEByteE220::_checkResponseStatus(ResponseStatus rs) {
    if (rs.code != E220_SUCCESS) {
        _lastErrorMessage = rs.getResponseDescription();
        return false;
    }
    return true;
}

LoRaConfig LoRaModuleEByteE220::_convertFromConfiguration(Configuration config) {
    LoRaConfig result;

    result.addressHigh = config.ADDH;
    result.addressLow = config.ADDL;
    result.channel = config.CHAN;

    result.uartBaudRate = config.SPED.uartBaudRate;
    result.airDataRate = config.SPED.airDataRate;
    result.uartParity = config.SPED.uartParity;

    result.subPacketSetting = config.OPTION.subPacketSetting;
    result.transmissionPower = config.OPTION.transmissionPower;
    result.rssiAmbientNoise = config.OPTION.RSSIAmbientNoise;

    result.enableRSSI = config.TRANSMISSION_MODE.enableRSSI;
    result.fixedTransmission = config.TRANSMISSION_MODE.fixedTransmission;
    result.enableLBT = config.TRANSMISSION_MODE.enableLBT;
    result.worPeriod = config.TRANSMISSION_MODE.WORPeriod;

    return result;
}

Configuration LoRaModuleEByteE220::_convertToConfiguration(LoRaConfig config) {
    Configuration result;

    result.COMMAND = WRITE_CFG_PWR_DWN_LOSE;
    result.STARTING_ADDRESS = REG_ADDRESS_CFG;
    result.LENGHT = PL_CONFIGURATION;

    result.ADDH = config.addressHigh;
    result.ADDL = config.addressLow;
    result.CHAN = config.channel;

    result.SPED.uartBaudRate = config.uartBaudRate;
    result.SPED.airDataRate = config.airDataRate;
    result.SPED.uartParity = config.uartParity;

    result.OPTION.subPacketSetting = config.subPacketSetting;
    result.OPTION.transmissionPower = config.transmissionPower;
    result.OPTION.RSSIAmbientNoise = config.rssiAmbientNoise;

    result.TRANSMISSION_MODE.enableRSSI = config.enableRSSI;
    result.TRANSMISSION_MODE.fixedTransmission = config.fixedTransmission;
    result.TRANSMISSION_MODE.enableLBT = config.enableLBT;
    result.TRANSMISSION_MODE.WORPeriod = config.worPeriod;

    result.CRYPT.CRYPT_H = 0;
    result.CRYPT.CRYPT_L = 0;

    return result;
}