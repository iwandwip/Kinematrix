/*
 * Sensor Network - Hub (Data Collector)
 * 
 * Hardware Setup:
 * Hub (ESP32):             Node (Arduino):
 * ├─ TX2 (GPIO17) ────────→ RX (Pin 10)
 * └─ RX2 (GPIO16) ←────── TX (Pin 11)
 * ├─ GND ─────────────── GND
 * 
 * Hub Role: Central data collector and coordinator
 * - Collects data from multiple sensor nodes
 * - Aggregates and processes sensor data
 * - Provides web interface or data logging
 * - Manages node health and status
 */

#include "ceria-serial.h"

using namespace CeriaDevOP;

CeriaSerial link;

struct NodeData {
    String nodeId = "";
    float temperature = 0.0;
    float humidity = 0.0;
    int soilMoisture = 0;
    int lightLevel = 0;
    int batteryLevel = 100;
    String status = "unknown";
    unsigned long lastSeen = 0;
    bool isOnline = false;
};

const int MAX_NODES = 5;
NodeData nodes[MAX_NODES];
int nodeCount = 0;

unsigned long lastStatusCheck = 0;
const unsigned long STATUS_CHECK_INTERVAL = 30000;

unsigned long lastDataRequest = 0;
const unsigned long DATA_REQUEST_INTERVAL = 10000;

void setup() {
    Serial.begin(115200);
    Serial.println("=== Sensor Network Hub ===");
    
    if (link.begin(Serial2, 115200, FRAME_256B)) {
        Serial.println("✅ Hub initialized - ready to collect sensor data");
    } else {
        Serial.println("❌ Failed to initialize hub");
        return;
    }
    
    link.setMode(FULL_DUPLEX);
    link.setRetries(3);
    link.setTimeout(3000);
    
    link.onReceive([](const String& key, const String& value) {
        Serial.println("📨 Node data: " + key + " = " + value);
        
        if (key == "node_id") {
            registerOrUpdateNode(value);
        }
        else if (key.startsWith("node_")) {
            String nodeId = key.substring(5, key.indexOf('_', 5));
            String dataType = key.substring(key.lastIndexOf('_') + 1);
            updateNodeData(nodeId, dataType, value);
        }
        else if (key == "heartbeat") {
            handleHeartbeat(value);
        }
        else if (key == "alert") {
            handleAlert(value);
        }
    });
    
    link.onError([](CeriaSerialError error) {
        Serial.println("⚠️ Hub communication error: " + String(error));
    });
    
    Serial.println("🎯 Hub ready - waiting for sensor nodes to connect");
    Serial.println("💬 Commands: 'nodes', 'data', 'request <nodeId>', 'ping <nodeId>'");
}

void loop() {
    link.tick();
    
    if (millis() - lastDataRequest > DATA_REQUEST_INTERVAL) {
        requestDataFromAllNodes();
        lastDataRequest = millis();
    }
    
    if (millis() - lastStatusCheck > STATUS_CHECK_INTERVAL) {
        checkNodeStatus();
        lastStatusCheck = millis();
    }
    
    processUserCommands();
    displayNetworkStatus();
    
    delay(100);
}

void registerOrUpdateNode(const String& nodeId) {
    int nodeIndex = findNodeIndex(nodeId);
    
    if (nodeIndex == -1 && nodeCount < MAX_NODES) {
        nodeIndex = nodeCount++;
        nodes[nodeIndex].nodeId = nodeId;
        Serial.println("📡 New node registered: " + nodeId);
    }
    
    if (nodeIndex != -1) {
        nodes[nodeIndex].lastSeen = millis();
        nodes[nodeIndex].isOnline = true;
        link.send("hub_ack", "node_registered");
    }
}

void updateNodeData(const String& nodeId, const String& dataType, const String& value) {
    int nodeIndex = findNodeIndex(nodeId);
    if (nodeIndex == -1) return;
    
    NodeData& node = nodes[nodeIndex];
    node.lastSeen = millis();
    node.isOnline = true;
    
    if (dataType == "temp") {
        node.temperature = value.toFloat();
    }
    else if (dataType == "humid") {
        node.humidity = value.toFloat();
    }
    else if (dataType == "soil") {
        node.soilMoisture = value.toInt();
    }
    else if (dataType == "light") {
        node.lightLevel = value.toInt();
    }
    else if (dataType == "battery") {
        node.batteryLevel = value.toInt();
    }
    else if (dataType == "status") {
        node.status = value;
    }
}

void handleHeartbeat(const String& nodeId) {
    int nodeIndex = findNodeIndex(nodeId);
    if (nodeIndex != -1) {
        nodes[nodeIndex].lastSeen = millis();
        nodes[nodeIndex].isOnline = true;
        Serial.println("💓 Heartbeat from " + nodeId);
    }
}

void handleAlert(const String& alertData) {
    Serial.println("🚨 ALERT: " + alertData);
    
    // Process alert (could be low battery, sensor failure, etc.)
    if (alertData.indexOf("battery") != -1) {
        Serial.println("🔋 Battery alert received");
    }
    else if (alertData.indexOf("sensor") != -1) {
        Serial.println("⚠️  Sensor malfunction alert");
    }
    
    // Acknowledge alert
    link.send("alert_ack", "received");
}

void requestDataFromAllNodes() {
    Serial.println("📡 Requesting data from all nodes...");
    
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].isOnline) {
            link.send("request_data", nodes[i].nodeId);
            delay(100);
        }
    }
}

void checkNodeStatus() {
    Serial.println("🔍 Checking node status...");
    
    unsigned long now = millis();
    const unsigned long OFFLINE_THRESHOLD = 60000;
    
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].isOnline && (now - nodes[i].lastSeen > OFFLINE_THRESHOLD)) {
            nodes[i].isOnline = false;
            Serial.println("⚠️  Node " + nodes[i].nodeId + " went offline");
        }
    }
}

int findNodeIndex(const String& nodeId) {
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].nodeId == nodeId) {
            return i;
        }
    }
    return -1;
}

void processUserCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command == "nodes") {
            displayNodeList();
        }
        else if (command == "data") {
            displayAllNodeData();
        }
        else if (command.startsWith("request ")) {
            String nodeId = command.substring(8);
            link.send("request_data", nodeId);
            Serial.println("👤 Data request sent to " + nodeId);
        }
        else if (command.startsWith("ping ")) {
            String nodeId = command.substring(5);
            link.send("ping", nodeId);
            Serial.println("👤 Ping sent to " + nodeId);
        }
        else if (command == "stats") {
            displayNetworkStats();
        }
        else if (command == "help") {
            Serial.println("📖 Available commands:");
            Serial.println("  nodes           - List all registered nodes");
            Serial.println("  data            - Show all node data");
            Serial.println("  request <nodeId> - Request data from specific node");
            Serial.println("  ping <nodeId>   - Ping specific node");
            Serial.println("  stats           - Show network statistics");
            Serial.println("  help            - Show this help");
        }
        else if (command.length() > 0) {
            Serial.println("❓ Unknown command: " + command + " (type 'help')");
        }
    }
}

void displayNodeList() {
    Serial.println("\n📋 === REGISTERED NODES ===");
    for (int i = 0; i < nodeCount; i++) {
        unsigned long lastSeenAge = (millis() - nodes[i].lastSeen) / 1000;
        Serial.println("Node " + String(i+1) + ": " + nodes[i].nodeId + 
                      " (" + (nodes[i].isOnline ? "ONLINE" : "OFFLINE") + 
                      ") - Last seen: " + String(lastSeenAge) + "s ago");
    }
    Serial.println("==========================\n");
}

void displayAllNodeData() {
    Serial.println("\n📊 === ALL SENSOR DATA ===");
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].isOnline) {
            NodeData& node = nodes[i];
            Serial.println("🏠 Node: " + node.nodeId);
            Serial.println("  🌡️  Temp: " + String(node.temperature, 1) + "°C");
            Serial.println("  💧 Humid: " + String(node.humidity, 1) + "%");
            Serial.println("  🌱 Soil: " + String(node.soilMoisture) + "%");
            Serial.println("  💡 Light: " + String(node.lightLevel));
            Serial.println("  🔋 Battery: " + String(node.batteryLevel) + "%");
            Serial.println("  📟 Status: " + node.status);
            Serial.println();
        }
    }
    Serial.println("===========================\n");
}

void displayNetworkStats() {
    uint32_t sent, received, errors;
    link.getStats(sent, received, errors);
    
    Serial.println("\n📈 === NETWORK STATISTICS ===");
    Serial.println("Total nodes: " + String(nodeCount));
    
    int onlineCount = 0;
    for (int i = 0; i < nodeCount; i++) {
        if (nodes[i].isOnline) onlineCount++;
    }
    Serial.println("Online nodes: " + String(onlineCount));
    Serial.println("Offline nodes: " + String(nodeCount - onlineCount));
    
    Serial.println("Messages sent: " + String(sent));
    Serial.println("Messages received: " + String(received));
    Serial.println("Communication errors: " + String(errors));
    
    if (sent > 0) {
        float successRate = ((float)(sent - errors) / sent) * 100.0;
        Serial.println("Success rate: " + String(successRate, 1) + "%");
    }
    Serial.println("==============================\n");
}

void displayNetworkStatus() {
    static unsigned long lastDisplay = 0;
    
    if (millis() - lastDisplay > 20000) {
        Serial.println("\n🌐 === HUB STATUS ===");
        Serial.println("Active nodes: " + String(nodeCount));
        
        int onlineCount = 0;
        for (int i = 0; i < nodeCount; i++) {
            if (nodes[i].isOnline) onlineCount++;
        }
        Serial.println("Online: " + String(onlineCount) + " | Offline: " + String(nodeCount - onlineCount));
        
        if (nodeCount > 0) {
            Serial.println("Network health: " + String((float)onlineCount / nodeCount * 100, 1) + "%");
        }
        
        Serial.println("💬 Type 'help' for commands");
        Serial.println("====================\n");
        
        lastDisplay = millis();
    }
}