#define ENABLE_MODULE_LORA_COM_V2
#include "Kinematrix.h"

// Pin definitions
#define LORA_CS_PIN    10
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN   2

// LoRa frequency - change according to your region
#define LORA_FREQUENCY 915E6

// Node configuration
#define NODE_ID 1           // Unique ID for this node
#define TARGET_NODE_ID 2    // ID of the target node
#define MAX_RETRIES 3       // Maximum number of retries
#define ACK_TIMEOUT 1000    // Timeout for acknowledgment (ms)

// Create LoRa instance
LoRaComV2 lora;

// Variables for data tracking
unsigned long lastSendTime = 0;
const int sendInterval = 10000;  // Send every 10 seconds
unsigned long messageCount = 0;
unsigned long successCount = 0;
unsigned long failCount = 0;

void onInit() {
    Serial.println("LoRa initialized successfully!");
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    Serial.println("LoRaComV2 - Acknowledged Sender Example");

    // Initialize LoRa module
    if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
        Serial.println("LoRa initialization failed!");
        while (1);
    }

    // Configure LoRa parameters
    lora.configure(9, 125E3, 17);  // SF=9, BW=125kHz, TXPower=17dBm
    lora.enableCRC();

    // Set node ID
    lora.setNodeId(NODE_ID);

    Serial.println("LoRa acknowledged sender started.");
    Serial.print("Node ID: ");
    Serial.println(NODE_ID);
    Serial.print("Target Node ID: ");
    Serial.println(TARGET_NODE_ID);
}

void loop() {
    // Check if it's time to send a message
    if (millis() - lastSendTime > sendInterval) {
        lastSendTime = millis();
        messageCount++;

        // Prepare data to send
        lora.clearData();
        lora.addData("IMPORTANT_DATA");
        lora.addData(NODE_ID);
        lora.addData(messageCount);
        lora.addData(random(0, 100));  // Random value to simulate sensor data
        lora.addData(millis() / 1000);  // Uptime in seconds

        Serial.print("Sending message #");
        Serial.print(messageCount);
        Serial.println(" with acknowledgment...");

        // Try to send the message using structured messaging
        bool success = lora.sendMessageToNode(TARGET_NODE_ID, lora.getDataString().c_str());

        if (success) {
            Serial.println("Message sent, waiting for acknowledgment...");

            // Wait for acknowledgment
            String response = lora.sendDataAndWaitResponse(ACK_TIMEOUT, MAX_RETRIES);

            if (response.length() > 0) {
                // Parse the response
                String responseType = lora.getStrData(response, 0);

                if (responseType == "ACK") {
                    unsigned long ackMsgId = (unsigned long) lora.getData(response, 1);

                    if (ackMsgId == messageCount) {
                        Serial.println("Acknowledgment received! Transmission successful.");
                        successCount++;
                    } else {
                        Serial.println("Received acknowledgment for wrong message ID.");
                        failCount++;
                    }
                } else {
                    Serial.print("Received unexpected response: ");
                    Serial.println(response);
                    failCount++;
                }
            } else {
                Serial.println("No acknowledgment received after retries. Transmission failed.");
                failCount++;
            }
        } else {
            Serial.println("Failed to send message.");
            failCount++;
        }

        // Print statistics
        Serial.println("Transmission Statistics:");
        Serial.print("Total Messages: ");
        Serial.println(messageCount);
        Serial.print("Successful: ");
        Serial.print(successCount);
        Serial.print(" (");
        Serial.print(successCount * 100.0 / messageCount, 1);
        Serial.println("%)");
        Serial.print("Failed: ");
        Serial.print(failCount);
        Serial.print(" (");
        Serial.print(failCount * 100.0 / messageCount, 1);
        Serial.println("%)");
        Serial.println("----------------------------");
    }
}
