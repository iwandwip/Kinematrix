#pragma once

#ifndef LORA_COMV2_H
#define LORA_COMV2_H

#include "Arduino.h"
#include "SPI.h"
#include "lora-base.h"

#pragma message("[COMPILED]: lora-comv2.h")

#define NONE ""
#define SEPARATOR ";"

enum LoRaMode {
    MODE_NORMAL,
    MODE_HALF_DUPLEX,
    MODE_MESH
};

enum MessageType {
    MSG_DATA,
    MSG_ACK,
    MSG_COMMAND,
    MSG_BROADCAST,
    MSG_PING,
    MSG_PONG
};

typedef struct {
    uint8_t msgType;
    uint16_t sourceId;
    uint16_t destId;
    uint16_t msgId;
    uint8_t ttl;
    uint16_t payloadLen;
    char payload[240];
} LoRaMessage;

typedef struct {
    uint8_t cs;
    uint8_t rst;
    uint8_t irq;
    uint16_t nodeId;
    long frequency;
    uint8_t spreadFactor;
    long bandwidth;
    uint8_t txPower;
    uint8_t syncWord;
    uint16_t preambleLen;
} LoRaConfig;

class LoRaComV2 {
private:
    bool isLoRaReady;
    String dataSend;
    uint32_t sendTime;
    uint32_t receiveTime;
    uint32_t lastActivity;

    LoRaConfig config;
    LoRaMode operationMode;

    uint16_t lastMsgId;
    bool expectingAck;
    uint16_t expectedAckId;
    uint32_t ackTimeout;

    bool inTransmitState;
    uint32_t switchTime;
    uint32_t txRxInterval;

    uint16_t knownNodes[32];
    uint8_t knownNodeCount;

    uint8_t txBuffer[256];
    uint8_t rxBuffer[256];
    uint16_t txBufferLen;
    uint16_t rxBufferLen;

    String parseStr(String data, char separator[], int index);
    void switchToReceive();
    void switchToTransmit();
    bool shouldSwitchState();
    void refreshNodeList(uint16_t nodeId);
    bool isNodeKnown(uint16_t nodeId);
    uint16_t generateMessageId();
    bool validatePacket(const uint8_t *buffer, size_t size);

public:
    LoRaComV2();
    ~LoRaComV2();

    int init(long frequency = 915E6, void (*onInit)() = nullptr);
    int init(uint8_t ss, uint8_t reset, uint8_t dio0, long frequency = 915E6, void (*onInit)() = nullptr);
    int init(LoRaConfig config, void (*onInit)() = nullptr);
    bool isReady(void (*onReady)() = nullptr);
    void end();

    void setMode(LoRaMode mode, uint32_t switchInterval = 1000);
    void setNodeId(uint16_t id);
    void configure(uint8_t spreadingFactor, long bandwidth, uint8_t txPower, uint8_t syncWord = 0x12);
    void setPreambleLength(uint16_t length);
    void enableCRC();
    void disableCRC();

    int packetRssi();
    float packetSnr();
    long packetFrequencyError();
    int getSpreadingFactor();
    long getSignalBandwidth();
    uint16_t getLastMessageId();
    bool isChannelActive();
    void sleep();
    void idle();

    template<typename T>
    void addData(T newData, const char *separator = ";") {
        dataSend += String(newData);
        dataSend += separator;
    }

    void clearData();

    bool sendData();
    bool sendDataCb(void (*onSend)(const String &));
    bool sendDataWithAck(unsigned long timeout = 1000, int maxRetries = 3);
    String sendDataAndWaitResponse(unsigned long timeout = 1000, int maxRetries = 3);
    void sendDataAsync(uint32_t interval = 500);
    void sendDataAsyncCb(uint32_t interval = 500, void (*onSend)(const String &) = nullptr);
    void sendBytes(const uint8_t *buffer, size_t size);
    void sendByte(uint8_t byte);

    bool sendMessage(LoRaMessage *msg);
    bool sendMessageToNode(uint16_t nodeId, const char *data, uint8_t msgType = MSG_DATA);
    bool broadcastMessage(const char *data, uint8_t ttl = 3);
    bool relayMessage(LoRaMessage *msg);

    bool receive(void (*onReceive)(const String &));
    void receiveAsync(uint32_t interval = 500, void (*onReceive)(const String &) = nullptr);
    void receiveString(void (*onReceive)(const String &));
    bool receiveMessage(LoRaMessage *msg);
    void onReceiveCallback(void (*callback)(LoRaMessage *));

    void runHalfDuplexCycle(void (*onReceive)(const String &) = nullptr, void (*onSend)(const String &) = nullptr);
    bool isInTransmitState();
    void setTxRxInterval(uint32_t interval);

    float getData(String data, uint8_t index, char separator[] = (char *) ";");
    String getStrData(String data, uint8_t index, char separator[] = (char *) ";");

    void enableMeshMode(uint16_t nodeId);
    bool sendToMesh(uint16_t destNodeId, const char *data, uint8_t ttl = 3);
    void discoverNodes(uint8_t ttl = 2);
    uint8_t getKnownNodeCount();
    uint16_t getKnownNodeId(uint8_t index);

    void dumpRegisters();
    String getStatusString();
    void printPacketInfo();
    void setDebugMode(bool enabled);

    String getDataString();
};

#endif // LORA_COMV2_H