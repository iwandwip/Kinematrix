#define ENABLE_MODULE_LORA_COM_V2
#include "Kinematrix.h"

#define LORA_CS_PIN    10
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN   2

#define LORA_FREQUENCY 915E6

#define NODE_ID 2

LoRaComV2 lora;

unsigned long lastMessageId = 0;
unsigned long messagesReceived = 0;
unsigned long lastReceivedTime = 0;

void onInit() {
    Serial.println("LoRa initialized successfully!");
}

void onAckSent(const String &data) {
    Serial.print("Acknowledgment sent: ");
    Serial.println(data);
}

void processMessage(LoRaMessage *msg) {
    Serial.println("\n--- Message Received ---");
    Serial.print("Type: ");
    Serial.println(msg->msgType);
    Serial.print("From Node: ");
    Serial.println(msg->sourceId);
    Serial.print("To Node: ");
    Serial.println(msg->destId);
    Serial.print("Message ID: ");
    Serial.println(msg->msgId);
    Serial.print("Payload: ");
    Serial.println(msg->payload);

    Serial.print("RSSI: ");
    Serial.print(lora.packetRssi());
    Serial.print(" dBm, SNR: ");
    Serial.print(lora.packetSnr());
    Serial.println(" dB");

    String messageType = lora.getStrData(msg->payload, 0);

    if (messageType == "IMPORTANT_DATA") {
        unsigned long sourceNodeId = (unsigned long) lora.getData(msg->payload, 1);
        unsigned long msgCount = (unsigned long) lora.getData(msg->payload, 2);
        float sensorValue = lora.getData(msg->payload, 3);
        unsigned long senderUptime = (unsigned long) lora.getData(msg->payload, 4);

        lastMessageId = msgCount;
        messagesReceived++;
        lastReceivedTime = millis();

        Serial.println("Important data received:");
        Serial.print("From Node: ");
        Serial.println(sourceNodeId);
        Serial.print("Message Count: ");
        Serial.println(msgCount);
        Serial.print("Sensor Value: ");
        Serial.println(sensorValue);
        Serial.print("Sender Uptime: ");
        Serial.print(senderUptime);
        Serial.println(" seconds");

        Serial.println("Sending acknowledgment...");
        lora.clearData();
        lora.addData("ACK");
        lora.addData(msgCount);

        delay(50);

        // FIXED: Use getDataString() instead of direct access to dataSend
        lora.sendMessageToNode(msg->sourceId, lora.getDataString().c_str(), MSG_ACK);
    } else {
        Serial.print("Unknown message type: ");
        Serial.println(messageType);
    }

    Serial.println("----------------------------");
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("LoRaComV2 - Acknowledged Receiver Example");

    if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
        Serial.println("LoRa initialization failed!");
        while (1);
    }

    lora.configure(9, 125E3, 17);
    lora.enableCRC();

    lora.setNodeId(NODE_ID);

    Serial.println("LoRa acknowledged receiver started.");
    Serial.print("Node ID: ");
    Serial.println(NODE_ID);
    Serial.println("Waiting for incoming messages...");
}

void loop() {
    LoRaMessage receivedMsg;

    if (lora.receiveMessage(&receivedMsg)) {
        processMessage(&receivedMsg);
    }

    static unsigned long lastStats = 0;
    if (millis() - lastStats > 30000) {
        lastStats = millis();

        Serial.println("\n--- Receiver Statistics ---");
        Serial.print("Total Messages Received: ");
        Serial.println(messagesReceived);

        if (lastReceivedTime > 0) {
            Serial.print("Time Since Last Message: ");
            Serial.print((millis() - lastReceivedTime) / 1000.0);
            Serial.println(" seconds");
        }

        Serial.println("----------------------------");
    }

    delay(10);
}