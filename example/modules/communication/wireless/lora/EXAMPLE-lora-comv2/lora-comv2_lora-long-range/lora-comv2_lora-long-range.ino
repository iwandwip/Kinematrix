#define ENABLE_MODULE_LORA_COM_V2
#include "Kinematrix.h"

// Pin definitions
#define LORA_CS_PIN    10
#define LORA_RESET_PIN 9
#define LORA_IRQ_PIN   2

// LoRa frequency - change according to your region
#define LORA_FREQUENCY 915E6

// Create LoRa instance
LoRaComV2 lora;

// Variables for sensor simulation
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
unsigned long lastSendTime = 0;
const int sendInterval = 15000;  // Send every 15 seconds for long range (lower data rate)

void onInit() {
  Serial.println("LoRa initialized successfully!");
  
  // LONG RANGE Configuration:
  // - Maximum range (SF=12, highest value)
  // - Narrow bandwidth (31.25kHz) 
  // - Maximum power (20dBm)
  // - Optimized for maximum distance at cost of data rate
  // - Range: ~5-15km+ in optimal conditions
  lora.configure(12, 31.25E3, 20);
  lora.setPreambleLength(16); // Longer preamble for better reception
  lora.enableCRC();
  
  Serial.println("Applied LONG RANGE configuration:");
  Serial.println("SF=12, BW=31.25kHz, Power=20dBm, Preamble=16");
  Serial.println("Expected range: 5-15km+ with lower data rate (~0.18 kbps)");
}

void onDataSent(const String &data) {
  Serial.print("Data sent: ");
  Serial.println(data);
}

void onDataReceived(const String &data) {
  Serial.print("Data received: ");
  Serial.println(data);
  
  // Get RSSI and SNR (signal strength metrics)
  Serial.print("RSSI: ");
  Serial.print(lora.packetRssi());
  Serial.print(" dBm, SNR: ");
  Serial.print(lora.packetSnr());
  Serial.println(" dB");
  
  // Parse the received data
  String command = lora.getStrData(data, 0);
  
  if (command == "DATA_REQUEST") {
    Serial.println("Received data request, sending sensor data...");
    
    // Create response data
    lora.clearData();
    lora.addData("SENSOR_DATA");
    lora.addData(temperature);
    lora.addData(humidity);
    lora.addData(pressure);
    
    // Allow more time for processing in long range mode
    delay(200);
    lora.sendDataCb(onDataSent);
  }
  else if (command == "PING") {
    Serial.println("Received ping request, sending pong...");
    
    // Create pong response
    lora.clearData();
    lora.addData("PONG");
    lora.addData(millis() / 1000);  // Uptime in seconds
    
    // Wait a bit longer before sending response in long range mode
    delay(100);
    lora.sendData();
  }
  
  Serial.println("----------------------------");
}

// Simulate reading from sensors
void readSensors() {
  // In a real application, you would read from actual sensors
  temperature = 21.5 + random(-20, 20) / 10.0;
  humidity = 48.0 + random(-50, 50) / 10.0;
  pressure = 1013.0 + random(-20, 20) / 10.0;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  
  Serial.println("LoRaComV2 - Long Range Configuration Example");
  
  // Initialize LoRa module
  if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  
  // Read sensor values initially
  readSensors();
  
  Serial.println("LoRa node started with LONG RANGE configuration");
  Serial.println("Maximum range settings for rural/remote deployments (5-15km+)");
  Serial.println("Note: Transmissions will be slower due to higher spreading factor");
}

void loop() {
  // First, check for any incoming data
  lora.receive(onDataReceived);
  
  // Check if it's time to send a status update
  if (millis() - lastSendTime > sendInterval) {
    lastSendTime = millis();
    
    // Read updated sensor values
    readSensors();
    
    // Long range sends less data to conserve airtime
    lora.clearData();
    lora.addData("STATUS");
    lora.addData(temperature);
    lora.addData(humidity);
    lora.addData(pressure);
    
    // Send the status update
    lora.sendDataCb(onDataSent);
    
    // Print additional debug info for long range mode
    Serial.print("Current SF: ");
    Serial.print(lora.getSpreadingFactor());
    Serial.print(", BW: ");
    Serial.print(lora.getSignalBandwidth() / 1000.0);
    Serial.println(" kHz");
  }
  
  // Small delay to prevent hogging the CPU
  delay(10);
}
