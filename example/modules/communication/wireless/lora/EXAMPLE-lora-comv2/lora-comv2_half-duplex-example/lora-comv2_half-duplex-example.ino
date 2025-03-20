#define ENABLE_MODULE_LORA_COM_V2
#include "Kinematrix.h"

// Pin definitions
#define LORA_CS_PIN    10
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN   2

// LoRa frequency - change according to your region
#define LORA_FREQUENCY 915E6

// Node configuration
#define NODE_ID 1  // Unique ID for this node
#define HALF_DUPLEX_INTERVAL 2000  // 2 seconds in each state

// Create LoRa instance
LoRaComV2 lora;

// Variables for sensor simulation
float temperature = 0.0;
float humidity = 0.0;
bool updatedSensorData = false;

void onInit() {
  Serial.println("LoRa initialized successfully!");
}

void onDataSent(const String &data) {
  Serial.print("Data sent: ");
  Serial.println(data);
}

void onDataReceived(const String &data) {
  Serial.print("Data received: ");
  Serial.println(data);
  
  // Print signal quality information
  Serial.print("RSSI: ");
  Serial.print(lora.packetRssi());
  Serial.print(" dBm, SNR: ");
  Serial.print(lora.packetSnr());
  Serial.println(" dB");
  
  // Parse the received data
  String messageType = lora.getStrData(data, 0);
  
  if (messageType == "STATUS") {
    // Extract values from status message
    float remoteTemp = lora.getData(data, 1);
    float remoteHumidity = lora.getData(data, 2);
    unsigned long remoteUptime = (unsigned long)lora.getData(data, 3);
    
    Serial.println("Received status update:");
    Serial.print("Remote Temperature: ");
    Serial.print(remoteTemp);
    Serial.println(" °C");
    Serial.print("Remote Humidity: ");
    Serial.print(remoteHumidity);
    Serial.println(" %");
    Serial.print("Remote Uptime: ");
    Serial.print(remoteUptime);
    Serial.println(" seconds");
  }
  else if (messageType == "COMMAND") {
    String command = lora.getStrData(data, 1);
    Serial.print("Received command: ");
    Serial.println(command);
    
    // Handle command (example)
    if (command == "READ_SENSORS") {
      updatedSensorData = true;
    }
  }
  
  Serial.println("----------------------------");
}

// Simulate reading from sensors
void readSensors() {
  // In a real application, you would read from actual sensors
  temperature = 23.0 + random(-20, 20) / 10.0;
  humidity = 50.0 + random(-50, 50) / 10.0;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  
  Serial.println("LoRaComV2 - Half-Duplex Example");
  
  // Read initial sensor values
  readSensors();
  
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
  
  // Set to half-duplex mode with 2-second switching interval
  lora.setMode(MODE_HALF_DUPLEX, HALF_DUPLEX_INTERVAL);
  
  Serial.println("LoRa half-duplex node started.");
  Serial.print("Node ID: ");
  Serial.println(NODE_ID);
  Serial.print("Half-duplex switching interval: ");
  Serial.print(HALF_DUPLEX_INTERVAL / 1000.0);
  Serial.println(" seconds");
}

void loop() {
  // Every 30 seconds, update sensor readings
  if (millis() % 30000 < 10) {
    readSensors();
  }
  
  // Prepare data to send when in transmit state
  if (lora.isInTransmitState()) {
    if (updatedSensorData || millis() % 10000 < 10) {
      // Prepare data packet
      lora.clearData();
      lora.addData("STATUS");
      lora.addData(NODE_ID);
      lora.addData(temperature);
      lora.addData(humidity);
      lora.addData(millis() / 1000);  // Uptime in seconds
      
      // The Flag will be reset after the transmitted cycle
      updatedSensorData = false;
    }
  }
  
  // Run the half-duplex cycle
  // This automatically switches between TX and RX states and
  // calls the provided callbacks when sending or receiving data
  lora.runHalfDuplexCycle(onDataReceived, onDataSent);
  
  // Display current state
  static bool lastState = false;
  if (lastState != lora.isInTransmitState()) {
    lastState = lora.isInTransmitState();
    Serial.print("Switched to ");
    Serial.println(lastState ? "TRANSMIT" : "RECEIVE");
  }
  
  // Small delay to prevent hogging the CPU
  delay(10);
}
