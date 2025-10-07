#include "lora-comv2.h"

LoRaComV2::LoRaComV2() {
    isLoRaReady = false;
    dataSend = "";
    sendTime = 0;
    receiveTime = 0;
    lastActivity = 0;

    operationMode = MODE_NORMAL;
    lastMsgId = 0;
    expectingAck = false;
    expectedAckId = 0;
    ackTimeout = 1000;

    inTransmitState = true;
    switchTime = 0;
    txRxInterval = 1000;

    txBufferLen = 0;
    rxBufferLen = 0;

    knownNodeCount = 0;
}

LoRaComV2::~LoRaComV2() {
    end();
}

int LoRaComV2::init(long frequency, void (*onInit)()) {
    config.frequency = frequency;
    config.spreadFactor = 7;
    config.bandwidth = 125E3;
    config.txPower = 17;
    config.syncWord = 0x12;
    config.preambleLen = 8;
    config.nodeId = 0;

    int success = LoRa.begin(frequency);
    isLoRaReady = success;

    if (success) {
        LoRa.setSpreadingFactor(config.spreadFactor);
        LoRa.setSignalBandwidth(config.bandwidth);
        LoRa.setTxPower(config.txPower);
        LoRa.setSyncWord(config.syncWord);
        LoRa.setPreambleLength(config.preambleLen);
        LoRa.enableCrc();

        if (onInit != nullptr) onInit();
    }

    return success;
}

int LoRaComV2::init(uint8_t ss, uint8_t reset, uint8_t dio0, long frequency, void (*onInit)()) {
    config.cs = ss;
    config.rst = reset;
    config.irq = dio0;
    config.frequency = frequency;
    config.spreadFactor = 7;
    config.bandwidth = 125E3;
    config.txPower = 17;
    config.syncWord = 0x12;
    config.preambleLen = 8;

    LoRa.setPins(ss, reset, dio0);
    int success = LoRa.begin(frequency);
    isLoRaReady = success;

    if (success) {
        LoRa.setSpreadingFactor(config.spreadFactor);
        LoRa.setSignalBandwidth(config.bandwidth);
        LoRa.setTxPower(config.txPower);
        LoRa.setSyncWord(config.syncWord);
        LoRa.setPreambleLength(config.preambleLen);
        LoRa.enableCrc();

        if (onInit != nullptr) onInit();
    }

    return success;
}

int LoRaComV2::init(LoRaConfig cfg, void (*onInit)()) {
    config = cfg;

    LoRa.setPins(config.cs, config.rst, config.irq);
    int success = LoRa.begin(config.frequency);
    isLoRaReady = success;

    if (success) {
        LoRa.setSpreadingFactor(config.spreadFactor);
        LoRa.setSignalBandwidth(config.bandwidth);
        LoRa.setTxPower(config.txPower);
        LoRa.setSyncWord(config.syncWord);
        LoRa.setPreambleLength(config.preambleLen);
        LoRa.enableCrc();

        if (onInit != nullptr) onInit();
    }

    return success;
}

bool LoRaComV2::isReady(void (*onReady)()) {
    if (onReady != nullptr && isLoRaReady) onReady();
    return isLoRaReady;
}

void LoRaComV2::end() {
    LoRa.end();
    isLoRaReady = false;
}

void LoRaComV2::setMode(LoRaMode mode, uint32_t switchInterval) {
    operationMode = mode;
    txRxInterval = switchInterval;

    if (mode == MODE_HALF_DUPLEX) {
        inTransmitState = true;
        switchTime = millis() + txRxInterval;
    }
}

void LoRaComV2::setNodeId(uint16_t id) {
    config.nodeId = id;
}

void LoRaComV2::configure(uint8_t spreadingFactor, long bandwidth, uint8_t txPower, uint8_t syncWord) {
    if (!isLoRaReady) return;

    config.spreadFactor = spreadingFactor;
    config.bandwidth = bandwidth;
    config.txPower = txPower;
    config.syncWord = syncWord;

    LoRa.setSpreadingFactor(spreadingFactor);
    LoRa.setSignalBandwidth(bandwidth);
    LoRa.setTxPower(txPower);
    LoRa.setSyncWord(syncWord);
}

void LoRaComV2::setPreambleLength(uint16_t length) {
    if (!isLoRaReady) return;

    config.preambleLen = length;
    LoRa.setPreambleLength(length);
}

void LoRaComV2::enableCRC() {
    if (!isLoRaReady) return;
    LoRa.enableCrc();
}

void LoRaComV2::disableCRC() {
    if (!isLoRaReady) return;
    LoRa.disableCrc();
}

int LoRaComV2::packetRssi() {
    return LoRa.packetRssi();
}

float LoRaComV2::packetSnr() {
    return LoRa.packetSnr();
}

long LoRaComV2::packetFrequencyError() {
    return LoRa.packetFrequencyError();
}

int LoRaComV2::getSpreadingFactor() {
    return LoRa.getSpreadingFactor();
}

long LoRaComV2::getSignalBandwidth() {
    return LoRa.getSignalBandwidth();
}

uint16_t LoRaComV2::getLastMessageId() {
    return lastMsgId;
}

bool LoRaComV2::isChannelActive() {
    return LoRa.parsePacket(0) > 0;
}

void LoRaComV2::sleep() {
    LoRa.sleep();
}

void LoRaComV2::idle() {
    LoRa.idle();
}

void LoRaComV2::clearData() {
    dataSend = "";
}

bool LoRaComV2::sendData() {
    if (!isLoRaReady) return false;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        return false;
    }

    LoRa.beginPacket();
    LoRa.print(dataSend);
    int result = LoRa.endPacket();
    lastActivity = millis();

    return result == 1;
}

bool LoRaComV2::sendDataCb(void (*onSend)(const String &)) {
    if (!isLoRaReady) return false;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        return false;
    }

    LoRa.beginPacket();
    LoRa.print(dataSend);
    int result = LoRa.endPacket();
    lastActivity = millis();

    if (onSend != nullptr) {
        onSend(dataSend);
    }

    return result == 1;
}

bool LoRaComV2::sendDataWithAck(unsigned long timeout, int maxRetries) {
    if (!isLoRaReady) return false;

    int retries = 0;
    bool ackReceived = false;

    while (retries < maxRetries && !ackReceived) {
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
        lastActivity = millis();

        unsigned long startTime = millis();
        while (millis() - startTime < timeout && !ackReceived) {
            int packetSize = LoRa.parsePacket();
            if (packetSize > 0) {
                String response = "";
                while (LoRa.available()) {
                    response += (char) LoRa.read();
                }

                if (response.indexOf("ACK") >= 0) {
                    ackReceived = true;
                }
            }
            yield();
        }

        retries++;
    }

    return ackReceived;
}

String LoRaComV2::sendDataAndWaitResponse(unsigned long timeout, int maxRetries) {
    if (!isLoRaReady) return "";

    int retries = 0;
    String response = "";
    bool responseReceived = false;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        switchToTransmit();
    }

    while (retries < maxRetries && !responseReceived) {
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
        lastActivity = millis();

        if (operationMode == MODE_HALF_DUPLEX) {
            switchToReceive();
        }

        unsigned long startTime = millis();
        while (millis() - startTime < timeout && !responseReceived) {
            int packetSize = LoRa.parsePacket();
            if (packetSize > 0) {
                response = "";
                while (LoRa.available()) {
                    response += (char) LoRa.read();
                }
                responseReceived = true;
            }
            yield();
        }

        retries++;

        if (!responseReceived && operationMode == MODE_HALF_DUPLEX) {
            switchToTransmit();
        }
    }

    return response;
}

void LoRaComV2::sendDataAsync(uint32_t interval) {
    if (!isLoRaReady) return;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        return;
    }

    if (millis() - sendTime >= interval) {
        sendTime = millis();
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
        lastActivity = millis();
    }
}

void LoRaComV2::sendDataAsyncCb(uint32_t interval, void (*onSend)(const String &)) {
    if (!isLoRaReady) return;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        return;
    }

    if (millis() - sendTime >= interval) {
        sendTime = millis();
        LoRa.beginPacket();
        LoRa.print(dataSend);
        LoRa.endPacket();
        lastActivity = millis();

        if (onSend != nullptr) {
            onSend(dataSend);
        }
    }
}

void LoRaComV2::sendBytes(const uint8_t *buffer, size_t size) {
    if (!isLoRaReady) return;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        return;
    }

    LoRa.beginPacket();
    for (size_t i = 0; i < size; i++) {
        LoRa.write(buffer[i]);
    }
    LoRa.endPacket();
    lastActivity = millis();
}

void LoRaComV2::sendByte(uint8_t byte) {
    if (!isLoRaReady) return;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        return;
    }

    LoRa.beginPacket();
    LoRa.write(byte);
    LoRa.endPacket();
    lastActivity = millis();
}

bool LoRaComV2::sendMessage(LoRaMessage *msg) {
    if (!isLoRaReady || msg == nullptr) return false;

    if (operationMode == MODE_HALF_DUPLEX && !inTransmitState) {
        return false;
    }

    if (msg->msgId == 0) {
        msg->msgId = generateMessageId();
    }

    if (msg->sourceId == 0) {
        msg->sourceId = config.nodeId;
    }

    LoRa.beginPacket();

    LoRa.write(msg->msgType);
    LoRa.write((uint8_t) (msg->sourceId >> 8));
    LoRa.write((uint8_t) (msg->sourceId & 0xFF));
    LoRa.write((uint8_t) (msg->destId >> 8));
    LoRa.write((uint8_t) (msg->destId & 0xFF));
    LoRa.write((uint8_t) (msg->msgId >> 8));
    LoRa.write((uint8_t) (msg->msgId & 0xFF));
    LoRa.write(msg->ttl);
    LoRa.write((uint8_t) (msg->payloadLen >> 8));
    LoRa.write((uint8_t) (msg->payloadLen & 0xFF));

    for (uint16_t i = 0; i < msg->payloadLen; i++) {
        LoRa.write(msg->payload[i]);
    }

    int result = LoRa.endPacket();
    lastActivity = millis();
    lastMsgId = msg->msgId;

    return result == 1;
}

bool LoRaComV2::sendMessageToNode(uint16_t nodeId, const char *data, uint8_t msgType) {
    if (!isLoRaReady || data == nullptr) return false;

    LoRaMessage msg;
    msg.msgType = msgType;
    msg.sourceId = config.nodeId;
    msg.destId = nodeId;
    msg.msgId = generateMessageId();
    msg.ttl = 1;

    size_t dataLen = strlen(data);
    if (dataLen > sizeof(msg.payload) - 1) {
        dataLen = sizeof(msg.payload) - 1;
    }
    memcpy(msg.payload, data, dataLen);
    msg.payload[dataLen] = '\0';
    msg.payloadLen = dataLen;

    return sendMessage(&msg);
}

bool LoRaComV2::broadcastMessage(const char *data, uint8_t ttl) {
    if (!isLoRaReady || data == nullptr) return false;

    return sendMessageToNode(0xFFFF, data, MSG_BROADCAST);
}

bool LoRaComV2::relayMessage(LoRaMessage *msg) {
    if (!isLoRaReady || msg == nullptr) return false;

    if (msg->ttl <= 1 || msg->destId == config.nodeId) {
        return false;
    }

    msg->ttl--;

    return sendMessage(msg);
}

bool LoRaComV2::receive(void (*onReceive)(const String &)) {
    if (!isLoRaReady || onReceive == nullptr) return false;

    if (operationMode == MODE_HALF_DUPLEX && inTransmitState) {
        return false;
    }

    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String data = "";
        while (LoRa.available()) {
            data += (char) LoRa.read();
        }
        lastActivity = millis();
        onReceive(data);
        return true;
    }

    return false;
}

void LoRaComV2::receiveAsync(uint32_t interval, void (*onReceive)(const String &)) {
    if (!isLoRaReady || onReceive == nullptr) return;

    if (operationMode == MODE_HALF_DUPLEX && inTransmitState) {
        return;
    }

    if (millis() - receiveTime >= interval) {
        receiveTime = millis();
        int packetSize = LoRa.parsePacket();
        if (packetSize) {
            String data = "";
            while (LoRa.available()) {
                data += (char) LoRa.read();
            }
            lastActivity = millis();
            onReceive(data);
        }
    }
}

void LoRaComV2::receiveString(void (*onReceive)(const String &)) {
    if (!isLoRaReady || onReceive == nullptr) return;

    if (operationMode == MODE_HALF_DUPLEX && inTransmitState) {
        return;
    }

    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String data = LoRa.readStringUntil('\n');
        lastActivity = millis();
        onReceive(data);
    }
}

bool LoRaComV2::receiveMessage(LoRaMessage *msg) {
    if (!isLoRaReady || msg == nullptr) return false;

    if (operationMode == MODE_HALF_DUPLEX && inTransmitState) {
        return false;
    }

    int packetSize = LoRa.parsePacket();
    if (packetSize < 10) {
        return false;
    }

    msg->msgType = LoRa.read();
    msg->sourceId = (LoRa.read() << 8) | LoRa.read();
    msg->destId = (LoRa.read() << 8) | LoRa.read();
    msg->msgId = (LoRa.read() << 8) | LoRa.read();
    msg->ttl = LoRa.read();
    msg->payloadLen = (LoRa.read() << 8) | LoRa.read();

    if (msg->payloadLen > sizeof(msg->payload) - 1) {
        msg->payloadLen = sizeof(msg->payload) - 1;
    }

    for (uint16_t i = 0; i < msg->payloadLen && LoRa.available(); i++) {
        msg->payload[i] = LoRa.read();
    }
    msg->payload[msg->payloadLen] = '\0';

    lastActivity = millis();

    refreshNodeList(msg->sourceId);

    return true;
}

void LoRaComV2::onReceiveCallback(void (*callback)(LoRaMessage *)) {
}

void LoRaComV2::runHalfDuplexCycle(void (*onReceive)(const String &), void (*onSend)(const String &)) {
    if (!isLoRaReady || operationMode != MODE_HALF_DUPLEX) return;

    if (shouldSwitchState()) {
        if (inTransmitState) {
            switchToReceive();
        } else {
            switchToTransmit();

            if (dataSend.length() > 0 && onSend != nullptr) {
                sendDataCb(onSend);
            }
        }
    }

    if (!inTransmitState && onReceive != nullptr) {
        receive(onReceive);
    }
}

bool LoRaComV2::isInTransmitState() {
    return inTransmitState;
}

void LoRaComV2::setTxRxInterval(uint32_t interval) {
    txRxInterval = interval;
}

float LoRaComV2::getData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index).toFloat();
}

String LoRaComV2::getStrData(String data, uint8_t index, char separator[]) {
    return parseStr(data, separator, index);
}

void LoRaComV2::enableMeshMode(uint16_t nodeId) {
    setNodeId(nodeId);
    setMode(MODE_MESH);
    knownNodeCount = 0;
}

bool LoRaComV2::sendToMesh(uint16_t destNodeId, const char *data, uint8_t ttl) {
    if (!isLoRaReady || operationMode != MODE_MESH) return false;

    return sendMessageToNode(destNodeId, data, MSG_DATA);
}

void LoRaComV2::discoverNodes(uint8_t ttl) {
    if (!isLoRaReady || operationMode != MODE_MESH) return;

    char discoveryMsg[] = "DISCOVER";
    broadcastMessage(discoveryMsg, ttl);
}

uint8_t LoRaComV2::getKnownNodeCount() {
    return knownNodeCount;
}

uint16_t LoRaComV2::getKnownNodeId(uint8_t index) {
    if (index < knownNodeCount) {
        return knownNodes[index];
    }
    return 0;
}

void LoRaComV2::dumpRegisters() {
    LoRa.dumpRegisters(Serial);
}

String LoRaComV2::getStatusString() {
    String status = "LoRa Status:\n";
    status += "Ready: " + String(isLoRaReady ? "Yes" : "No") + "\n";
    status += "Mode: " + String(operationMode) + "\n";
    status += "Node ID: " + String(config.nodeId) + "\n";
    status += "Frequency: " + String(config.frequency / 1E6) + " MHz\n";
    status += "SF: " + String(config.spreadFactor) + "\n";
    status += "BW: " + String(config.bandwidth / 1E3) + " kHz\n";
    status += "Last Msg ID: " + String(lastMsgId) + "\n";
    status += "Known Nodes: " + String(knownNodeCount) + "\n";

    return status;
}

void LoRaComV2::printPacketInfo() {
    Serial.println("Packet Info:");
    Serial.print("RSSI: ");
    Serial.println(packetRssi());
    Serial.print("SNR: ");
    Serial.println(packetSnr());
    Serial.print("Frequency Error: ");
    Serial.println(packetFrequencyError());
}

void LoRaComV2::setDebugMode(bool enabled) {
}

String LoRaComV2::parseStr(String data, char separator[], int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator[0] || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void LoRaComV2::switchToReceive() {
    inTransmitState = false;
    switchTime = millis() + txRxInterval;
}

void LoRaComV2::switchToTransmit() {
    inTransmitState = true;
    switchTime = millis() + txRxInterval;
}

bool LoRaComV2::shouldSwitchState() {
    return (millis() >= switchTime);
}

void LoRaComV2::refreshNodeList(uint16_t nodeId) {
    if (nodeId == config.nodeId || nodeId == 0xFFFF) {
        return;
    }

    if (!isNodeKnown(nodeId)) {
        if (knownNodeCount < sizeof(knownNodes) / sizeof(knownNodes[0])) {
            knownNodes[knownNodeCount++] = nodeId;
        }
    }
}

bool LoRaComV2::isNodeKnown(uint16_t nodeId) {
    for (uint8_t i = 0; i < knownNodeCount; i++) {
        if (knownNodes[i] == nodeId) {
            return true;
        }
    }
    return false;
}

uint16_t LoRaComV2::generateMessageId() {
    lastMsgId++;
    return lastMsgId;
}

bool LoRaComV2::validatePacket(const uint8_t *buffer, size_t size) {
    return size > 0;
}

String LoRaComV2::getDataString() {
    return dataSend;
}