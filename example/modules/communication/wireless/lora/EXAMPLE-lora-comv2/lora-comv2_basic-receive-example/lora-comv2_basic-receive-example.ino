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

// Variables to store received data
float receivedTemperature = 0.0;
float receivedHumidity = 0.0;
unsigned long receivedUptime = 0;
unsigned long lastReceivedTime = 0;

void onInit() {
  Serial.println("LoRa initialized successfully!");
}

void onDataReceived(const String &data) {
  Serial.print("Data received: ");
  Serial.println(data);
  
  // Get RSSI and SNR
  Serial.print("RSSI: ");
  Serial.print(lora.packetRssi());
  Serial.print(" dBm, SNR: ");
  Serial.print(lora.packetSnr());
  Serial.println(" dB");
  
  // Parse the received data
  String dataType = lora.getStrData(data, 0);
  
  if (dataType == "SENSOR") {
    receivedTemperature = lora.getData(data, 1);
    receivedHumidity = lora.getData(data, 2);
    receivedUptime = (unsigned long)lora.getData(data, 3);
    lastReceivedTime = millis();
    
    // Display the parsed data
    Serial.println("Parsed sensor data:");
    Serial.print("Temperature: ");
    Serial.print(receivedTemperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(receivedHumidity);
    Serial.println(" %");
    Serial.print("Sender uptime: ");
    Serial.print(receivedUptime);
    Serial.println(" seconds");
  }
  
  Serial.println("----------------------------");
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);
  
  Serial.println("LoRaComV2 - Basic Receiver Example");
  
  // Initialize LoRa module
  if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  
  // Configure LoRa parameters - must match sender
  lora.configure(8, 125E3, 17);  // SF=8, BW=125kHz, TXPower=17dBm
  lora.enableCRC();
  
  Serial.println("LoRa receiver started. Waiting for incoming data...");
}

void loop() {
  // Check for incoming data
  lora.receive(onDataReceived);
  
  // Display time since last data reception
  if (lastReceivedTime > 0 && millis() - lastReceivedTime > 10000) {
    Serial.println("No data received for 10 seconds...");
    lastReceivedTime = millis();
  }
  
  // Small delay to prevent hogging the CPU
  delay(10);
}
