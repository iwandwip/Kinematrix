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

// Range presets
enum RangePreset {
  SHORT_RANGE,  // High data rate, lower power, shorter range (~100-500m)
  MEDIUM_RANGE, // Balanced performance (~1-3km)
  LONG_RANGE    // Maximum range, lower data rate (~5-15km+)
};

// Current preset - change this to use different settings
RangePreset currentPreset = MEDIUM_RANGE;

// Variables for sensor simulation
float temperature = 0.0;
float humidity = 0.0;
float pressure = 0.0;
unsigned long lastSendTime = 0;
const int sendInterval = 8000;  // Send every 8 seconds

void applyRangePreset(RangePreset preset);

void onInit() {
  Serial.println("LoRa initialized successfully!");
  applyRangePreset(currentPreset);
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
    
    // Wait a short moment before sending a response
    delay(100);
    
    // Send the response
    lora.sendDataCb(onDataSent);
  }
  else if (command == "PING") {
    Serial.println("Received ping request, sending pong...");
    
    // Create pong response
    lora.clearData();
    lora.addData("PONG");
    lora.addData(millis() / 1000);  // Uptime in seconds
    
    // Wait a short moment before sending a response
    delay(50);
    
    // Send the response
    lora.sendData();
  }
  
  Serial.println("----------------------------");
}

// Apply LoRa configuration based on selected range preset
void applyRangePreset(RangePreset preset) {
  switch (preset) {
    case SHORT_RANGE:
      // Short Range Configuration:
      // - High data rate (SF=7, the lowest practical value)
      // - Wide bandwidth (500kHz)
      // - Lower power (13dBm to save battery)
      // - Good for indoor/urban use with minimal obstacles
      // - Range: ~100-500m depending on environment
      lora.configure(7, 500E3, 13);
      Serial.println("Applied SHORT RANGE preset - Optimized for close proximity");
      Serial.println("SF=7, BW=500kHz, Power=13dBm");
      Serial.println("Expected range: 100-500m with higher data rate");
      break;
      
    case MEDIUM_RANGE:
      // Medium Range Configuration:
      // - Balanced settings (SF=9)
      // - Standard bandwidth (125kHz)
      // - Standard power (17dBm)
      // - Good balance between range and data rate
      // - Range: ~1-3km depending on environment
      lora.configure(9, 125E3, 17);
      Serial.println("Applied MEDIUM RANGE preset - Balanced performance");
      Serial.println("SF=9, BW=125kHz, Power=17dBm");
      Serial.println("Expected range: 1-3km with moderate data rate");
      break;
      
    case LONG_RANGE:
      // Long Range Configuration:
      // - Maximum range (SF=12, highest value)
      // - Narrow bandwidth (31.25kHz) 
      // - Maximum power (20dBm)
      // - Optimized for maximum distance at cost of data rate
      // - Range: ~5-15km+ in optimal conditions
      lora.configure(12, 31.25E3, 20);
      lora.setPreambleLength(16); // Longer preamble for better reception
      Serial.println("Applied LONG RANGE preset - Maximum distance");
      Serial.println("SF=12, BW=31.25kHz, Power=20dBm, Preamble=16");
      Serial.println("Expected range: 5-15km+ with lower data rate");
      break;
  }
  
  // Enable CRC error checking for all configurations
  lora.enableCRC();
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
  
  Serial.println("LoRaComV2 - Range Presets Example");
  
  // Initialize LoRa module
  if (!lora.init(LORA_CS_PIN, LORA_RESET_PIN, LORA_IRQ_PIN, LORA_FREQUENCY, onInit)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }
  
  // Read sensor values initially
  readSensors();
  
  Serial.println("LoRa node started. Sending status updates and responding to requests...");
  Serial.println("Current preset: " + String(currentPreset == SHORT_RANGE ? "SHORT RANGE" : 
                                      currentPreset == MEDIUM_RANGE ? "MEDIUM RANGE" : "LONG RANGE"));
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
