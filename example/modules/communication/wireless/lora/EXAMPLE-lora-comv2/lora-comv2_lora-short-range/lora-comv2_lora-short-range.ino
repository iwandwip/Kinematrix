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
const int sendInterval = 5000;  // Send every 5 seconds for short range

void onInit() {
  Serial.println("LoRa initialized successfully!");
  
  // SHORT RANGE Configuration:
  // - High data rate (SF=7, the lowest practical value)
  // - Wide bandwidth (500kHz)
  // - Lower power (13dBm to save battery)
  // - Good for indoor/urban use with minimal obstacles
  // - Range: ~100-500m depending on environment
  lora.configure(7, 500E3, 13);
  lora.enableCRC();
  
  Serial.println("Applied SHORT RANGE configuration:");
  Serial.println("SF=7, BW=500kHz, Power=13dBm");
  Serial.println("Expected range: 100-500m with higher data rate (~5.5 kbps)");
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
    
    // Send the response - short delay for fast response in short range
    delay(50);
    lora.sendDataCb(onDataSent);
  }
  else if (command == "PING") {
    Serial.println("Received ping request, sending pong...");
    
    // Create pong response
    lora.clearData();
    lora.addData("PONG");
    lora.addData(millis() / 1000);  // Uptime in seconds
    
    // Send the response immediately
    delay(20);
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
  
  Serial.println("LoRaComV2 - Short Range Configuration Example");
  
  // Initialize LoRa module
  if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  
  // Read sensor values initially
  readSensors();
  
  Serial.println("LoRa node started with SHORT RANGE configuration");
  Serial.println("Optimized for high data rate in close proximity (100-500m)");
}

void loop() {
  // First, check for any incoming data
  lora.receive(onDataReceived);
  
  // Check if it's time to send a status update
  if (millis() - lastSendTime > sendInterval) {
    lastSendTime = millis();
    
    // Read updated sensor values
    readSensors();
    
    // Prepare status update
    lora.clearData();
    lora.addData("STATUS");
    lora.addData(temperature);
    lora.addData(humidity);
    lora.addData(pressure);
    lora.addData(millis() / 1000);  // Uptime in seconds
    
    // Send the status update
    lora.sendDataCb(onDataSent);
  }
  
  // Small delay to prevent hogging the CPU
  delay(10);
}
