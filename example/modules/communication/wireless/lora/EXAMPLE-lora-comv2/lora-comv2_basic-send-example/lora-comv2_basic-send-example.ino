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
unsigned long lastSendTime = 0;
const int sendInterval = 5000;  // Send every 5 seconds

void onInit() {
  Serial.println("LoRa initialized successfully!");
}

void onDataSent(const String &data) {
  Serial.print("Data sent: ");
  Serial.println(data);
}

// Simulate reading from sensors
void readSensors() {
  // In a real application, you would read from actual sensors
  temperature = 22.5 + random(-20, 20) / 10.0;
  humidity = 45.0 + random(-50, 50) / 10.0;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  
  Serial.println("LoRaComV2 - Basic Sender Example");
  
  // Initialize LoRa module
  if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  
  // Configure LoRa parameters
  lora.configure(8, 125E3, 17);  // SF=8, BW=125kHz, TXPower=17dBm
  lora.enableCRC();
  
  Serial.println("LoRa sender started. Sending data every 5 seconds...");
}

void loop() {
  // Check if it's time to send data
  if (millis() - lastSendTime > sendInterval) {
    lastSendTime = millis();
    
    // Read sensors
    readSensors();
    
    // Prepare data to send
    lora.clearData();
    lora.addData("SENSOR");
    lora.addData(temperature);
    lora.addData(humidity);
    lora.addData(millis() / 1000);  // Uptime in seconds
    
    // Send data with callback
    lora.sendDataCb(onDataSent);
    
    // Print some debug info
    Serial.print("RSSI: ");
    Serial.print(lora.packetRssi());
    Serial.print(" dBm, SNR: ");
    Serial.print(lora.packetSnr());
    Serial.println(" dB");
  }
}
