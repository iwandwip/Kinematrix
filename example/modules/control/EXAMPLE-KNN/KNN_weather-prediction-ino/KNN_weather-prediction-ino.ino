/*
 * Weather Prediction using KNN
 * 
 * Menggunakan data suhu dan kelembapan untuk memprediksi cuaca
 */

#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Parameter KNN
const int K = 3;
const int MAX_FEATURES = 2; // Suhu dan kelembapan
const int MAX_SAMPLES = 30;

// Create KNN instance
KNN weatherKnn(K, MAX_FEATURES, MAX_SAMPLES);

// Setup pins for sensors
const int TEMP_PIN = A0;
const int HUMIDITY_PIN = A1;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Weather Prediction using KNN");
  Serial.println("---------------------------");
  
  // Train the model with sample data
  trainWeatherModel();
  
  Serial.println("Model trained. Ready for prediction.");
}

void loop() {
  // Read sensor values
  float temperature = readTemperature();
  float humidity = readHumidity();
  
  // Create feature array
  float features[2] = {temperature, humidity};
  
  // Make prediction
  const char* prediction = weatherKnn.predict(features);
  float confidence = weatherKnn.getPredictionConfidence(features);
  
  // Display results
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Prediction: ");
  Serial.print(prediction);
  Serial.print(" (Confidence: ");
  Serial.print(confidence * 100);
  Serial.println("%)");
  
  delay(5000); // Wait 5 seconds before next reading
}

void trainWeatherModel() {
  Serial.println("Training weather prediction model...");
  
  // Data for sunny weather
  float sunny[][2] = {
    {30.0, 40.0}, {29.5, 42.0}, {31.0, 38.0}, {28.5, 45.0},
    {30.5, 36.0}, {32.0, 35.0}, {29.0, 43.0}, {31.5, 37.0}
  };
  
  // Data for cloudy weather
  float cloudy[][2] = {
    {25.0, 60.0}, {26.0, 58.0}, {24.0, 65.0}, {23.5, 62.0},
    {27.0, 55.0}, {24.5, 63.0}, {26.5, 59.0}, {22.0, 61.0}
  };
  
  // Data for rainy weather
  float rainy[][2] = {
    {20.0, 85.0}, {19.5, 88.0}, {21.0, 82.0}, {18.0, 90.0},
    {22.0, 80.0}, {19.0, 87.0}, {21.5, 84.0}, {17.5, 92.0}
  };
  
  // Add sunny data
  for (int i = 0; i < 8; i++) {
    weatherKnn.addTrainingData("sunny", sunny[i]);
  }
  
  // Add cloudy data
  for (int i = 0; i < 8; i++) {
    weatherKnn.addTrainingData("cloudy", cloudy[i]);
  }
  
  // Add rainy data
  for (int i = 0; i < 8; i++) {
    weatherKnn.addTrainingData("rainy", rainy[i]);
  }
  
  // Enable normalization for better accuracy
  weatherKnn.enableNormalization(true);
  
  Serial.print("Total training samples: ");
  Serial.println(weatherKnn.getDataCount());
}

float readTemperature() {
  // In a real application, read from temperature sensor
  // For this example, simulate with random values
  return random(170, 320) / 10.0; // 17.0 to 32.0 °C
}

float readHumidity() {
  // In a real application, read from humidity sensor
  // For this example, simulate with random values
  return random(350, 950) / 10.0; // 35.0 to 95.0 %
}