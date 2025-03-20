#define ENABLE_MODULE_LORA_COM_V2
#include "Kinematrix.h"

// Pin definitions
#define LORA_CS_PIN    10
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN   2

// LoRa frequency - change according to your region
#define LORA_FREQUENCY 915E6

// Node configuration - CHANGE THIS FOR EACH NODE
#define NODE_ID 1  // Unique ID for this node

// Time intervals
#define STATUS_INTERVAL 20000      // Send status every 20 seconds
#define DISCOVERY_INTERVAL 60000   // Run discovery every 60 seconds
#define MAX_TTL 3                  // Maximum hops for messages

// Create LoRa instance
LoRaComV2 lora;

// Time tracking
unsigned long lastStatusTime = 0;
unsigned long lastDiscoveryTime = 0;
unsigned long messagesSent = 0;
unsigned long messagesReceived = 0;
unsigned long messagesRelayed = 0;

// Store last received messages to avoid duplicates
#define MAX_RECENT_MSGS 10
uint16_t recentMsgIds[MAX_RECENT_MSGS] = {0};
uint8_t recentMsgIndex = 0;

// Forward declarations of functions
void sendStatusMessage(uint8_t ttl);
void runDiscovery();

void onInit() {
    Serial.println("LoRa initialized successfully!");
}

// Check if message was recently received (to avoid duplicate processing)
bool isRecentMessage(uint16_t msgId) {
    for (int i = 0; i < MAX_RECENT_MSGS; i++) {
        if (recentMsgIds[i] == msgId) {
            return true;
        }
    }

    // Add to recent messages list
    recentMsgIds[recentMsgIndex] = msgId;
    recentMsgIndex = (recentMsgIndex + 1) % MAX_RECENT_MSGS;

    return false;
}

// Process incoming messages
void processMessage(LoRaMessage *msg) {
    // Check if this is a duplicate message
    if (isRecentMessage(msg->msgId)) {
        return;
    }

    messagesReceived++;

    Serial.println("\n--- Message Received ---");
    Serial.print("From: Node ");
    Serial.println(msg->sourceId);
    Serial.print("To: Node ");
    Serial.println(msg->destId);
    Serial.print("Message ID: ");
    Serial.println(msg->msgId);
    Serial.print("TTL: ");
    Serial.println(msg->ttl);
    Serial.print("Payload: ");
    Serial.println(msg->payload);

    // Print signal quality
    Serial.print("RSSI: ");
    Serial.print(lora.packetRssi());
    Serial.print(" dBm, SNR: ");
    Serial.print(lora.packetSnr());
    Serial.println(" dB");

    // Check if this message is for this node
    if (msg->destId == NODE_ID || msg->destId == 0xFFFF) {
        // Parse payload
        String messageType = lora.getStrData(msg->payload, 0);

        if (messageType == "STATUS") {
            // Process status message
            unsigned long sourceNodeId = (unsigned long) lora.getData(msg->payload, 1);
            float temp = lora.getData(msg->payload, 2);
            float humid = lora.getData(msg->payload, 3);

            Serial.print("Status from Node ");
            Serial.print(sourceNodeId);
            Serial.print(": Temp=");
            Serial.print(temp);
            Serial.print("°C, Humidity=");
            Serial.print(humid);
            Serial.println("%");
        } else if (messageType == "COMMAND") {
            // Process command message
            String command = lora.getStrData(msg->payload, 1);
            Serial.print("Command received: ");
            Serial.println(command);

            // Example command handling
            if (command == "REPORT") {
                // Send a status report
                sendStatusMessage(MAX_TTL);
            }
        } else if (messageType == "DISCOVER") {
            // Node discovery request
            Serial.println("Discovery request received, sending response...");

            // Send a discovery response
            char response[50];
            sprintf(response, "DISCOVER_RESP;%d", NODE_ID);
            lora.sendMessageToNode(msg->sourceId, response, MSG_BROADCAST);
        } else if (messageType == "DISCOVER_RESP") {
            // Node discovery response
            unsigned long respNodeId = (unsigned long) lora.getData(msg->payload, 1);
            Serial.print("Discovery response from Node ");
            Serial.println(respNodeId);
        }
    }

    // Check if we should relay this message
    if (msg->ttl > 1 && msg->destId != NODE_ID && msg->sourceId != NODE_ID) {
        Serial.print("Relaying message from Node ");
        Serial.print(msg->sourceId);
        Serial.print(" to Node ");
        Serial.println(msg->destId);

        // Relay the message
        if (lora.relayMessage(msg)) {
            messagesRelayed++;
            Serial.println("Message relayed successfully.");
        } else {
            Serial.println("Failed to relay message.");
        }
    }

    Serial.println("----------------------------");
}

// Send a status message
void sendStatusMessage(uint8_t ttl) {
    // Prepare status data with simulated sensor readings
    float temp = 22.0 + random(-20, 20) / 10.0;
    float humidity = 45.0 + random(-50, 50) / 10.0;

    // Create the status message
    char statusMsg[100];
    sprintf(statusMsg, "STATUS;%d;%.1f;%.1f", NODE_ID, temp, humidity);

    // Broadcast to all nodes
    if (lora.broadcastMessage(statusMsg, ttl)) {
        messagesSent++;
        Serial.println("Status message broadcast successfully.");
    } else {
        Serial.println("Failed to broadcast status message.");
    }
}

// Run network discovery
void runDiscovery() {
    Serial.println("Running network discovery...");
    lora.discoverNodes(MAX_TTL);
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("LoRaComV2 - Mesh Network Node Example");

    // Initialize LoRa module
    if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
        Serial.println("LoRa initialization failed!");
        while (1);
    }

    // Configure LoRa parameters - all nodes must use the same settings
    lora.configure(10, 125E3, 17);  // SF=10, BW=125kHz, TXPower=17dBm
    lora.enableCRC();

    // Enable mesh mode with this node's ID
    lora.enableMeshMode(NODE_ID);

    Serial.print("Mesh network node started. Node ID: ");
    Serial.println(NODE_ID);

    // Randomize first transmit times
    lastStatusTime = millis() - random(0, STATUS_INTERVAL);
    lastDiscoveryTime = millis() - random(0, DISCOVERY_INTERVAL);
}

void loop() {
    // Create a message structure to receive incoming messages
    LoRaMessage receivedMsg;

    // Check for incoming messages and process them
    if (lora.receiveMessage(&receivedMsg)) {
        processMessage(&receivedMsg);
    }

    // Periodically send status updates
    if (millis() - lastStatusTime > STATUS_INTERVAL) {
        lastStatusTime = millis();
        sendStatusMessage(MAX_TTL);
    }

    // Periodically run network discovery
    if (millis() - lastDiscoveryTime > DISCOVERY_INTERVAL) {
        lastDiscoveryTime = millis();
        runDiscovery();
    }

    // Display node statistics periodically
    static unsigned long lastStatsTime = 0;
    if (millis() - lastStatsTime > 30000) {  // Every 30 seconds
        lastStatsTime = millis();

        Serial.println("\n--- Node Statistics ---");
        Serial.print("Node ID: ");
        Serial.println(NODE_ID);
        Serial.print("Messages Sent: ");
        Serial.println(messagesSent);
        Serial.print("Messages Received: ");
        Serial.println(messagesReceived);
        Serial.print("Messages Relayed: ");
        Serial.println(messagesRelayed);

        // Get known nodes
        Serial.print("Known Nodes: ");
        uint8_t knownCount = lora.getKnownNodeCount();
        if (knownCount > 0) {
            for (uint8_t i = 0; i < knownCount; i++) {
                Serial.print(lora.getKnownNodeId(i));
                if (i < knownCount - 1) {
                    Serial.print(", ");
                }
            }
            Serial.println();
        } else {
            Serial.println("None");
        }

        Serial.println("----------------------------");
    }

    // Small delay to prevent hogging the CPU
    delay(10);
}