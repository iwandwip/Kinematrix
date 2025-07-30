/*
 * Decision Tree Visualization Demo
 * 
 * Demonstrates the new tree visualization features added to DecisionTree class.
 * Creates a simple decision tree and displays its structure in the serial monitor.
 * 
 * Features demonstrated:
 * - printTree(): Complete tree visualization with statistics
 * - printTreeStructure(): Visual tree structure display  
 * - printTreeStats(): Tree statistics and information
 * 
 * Expected output shows a visual representation of the decision tree
 * with branches, nodes, and leaf classifications.
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

// Create decision tree: 2 features, 20 data points, max depth 5
DecisionTree classifier(2, 20, 5, 2);

void setup() {
    Serial.begin(115200);
    Serial.println("Decision Tree Visualization Demo");
    Serial.println("================================");
    
    // Enable debug mode for additional information
    classifier.setDebugMode(true);
    
    // Create sample dataset: Weather prediction
    // Features: [Temperature, Humidity] -> Class: [Play, NoPlay]
    Serial.println("\nAdding training data...");
    
    // Sunny days with different temperature and humidity
    classifier.addTrainingData("Play", (float[]){25.0, 45.0});      // Sunny, moderate temp, low humidity
    classifier.addTrainingData("Play", (float[]){28.0, 40.0});      // Sunny, warm, low humidity
    classifier.addTrainingData("NoPlay", (float[]){35.0, 85.0});    // Sunny, hot, high humidity
    classifier.addTrainingData("NoPlay", (float[]){32.0, 90.0});    // Sunny, hot, very high humidity
    
    // Overcast days (generally good for playing)
    classifier.addTrainingData("Play", (float[]){22.0, 70.0});      // Overcast, cool, moderate humidity
    classifier.addTrainingData("Play", (float[]){26.0, 65.0});      // Overcast, warm, moderate humidity
    classifier.addTrainingData("Play", (float[]){20.0, 80.0});      // Overcast, cool, high humidity
    
    // Rainy days with different conditions
    classifier.addTrainingData("NoPlay", (float[]){18.0, 95.0});    // Rainy, cold, very high humidity  
    classifier.addTrainingData("Play", (float[]){24.0, 75.0});      // Light rain, moderate temp
    classifier.addTrainingData("NoPlay", (float[]){16.0, 85.0});    // Rainy, cold, high humidity
    
    // Additional diverse samples
    classifier.addTrainingData("Play", (float[]){30.0, 35.0});      // Perfect conditions
    classifier.addTrainingData("Play", (float[]){27.0, 50.0});      // Good conditions
    classifier.addTrainingData("NoPlay", (float[]){38.0, 60.0});    // Too hot
    classifier.addTrainingData("NoPlay", (float[]){15.0, 95.0});    // Too cold and humid
    classifier.addTrainingData("Play", (float[]){23.0, 55.0});      // Ideal conditions
    
    Serial.println("Added " + String(classifier.getDataCount()) + " training samples");
    
    // Set feature types (both are numeric)
    classifier.setFeatureType(0, NUMERIC);  // Temperature
    classifier.setFeatureType(1, NUMERIC);  // Humidity
    
    // Train the decision tree
    Serial.println("\nTraining decision tree...");
    if (classifier.train(INFORMATION_GAIN)) {
        Serial.println("Training completed successfully!");
        
        // Display the complete tree visualization
        classifier.printTree();
        
        // Demonstrate individual components
        Serial.println("\n=== INDIVIDUAL COMPONENTS DEMO ===");
        
        Serial.println("\n1. Tree Statistics Only:");
        classifier.printTreeStats();
        
        Serial.println("\n2. Tree Structure Only:");
        classifier.printTreeStructure();
        
        // Test predictions with tree explanation
        Serial.println("\n=== PREDICTION EXAMPLES ===");
        testPredictions();
        
        // Show confidence levels
        Serial.println("\n=== PREDICTION CONFIDENCE ===");
        showPredictionConfidence();
        
    } else {
        Serial.println("Training failed!");
        if (classifier.hasError()) {
            Serial.println("Error: " + String(classifier.getErrorMessage()));
        }
    }
}

void loop() {
    // Interactive mode - uncomment to enable continuous predictions
    /*
    if (Serial.available()) {
        String input = Serial.readString();
        input.trim();
        
        if (input.startsWith("predict")) {
            // Format: "predict 25.5 60.2"
            int space1 = input.indexOf(' ', 8);
            int space2 = input.indexOf(' ', space1 + 1);
            
            if (space1 > 0 && space2 > 0) {
                float temp = input.substring(8, space1).toFloat();
                float humidity = input.substring(space1 + 1, space2).toFloat();
                
                float features[] = {temp, humidity};
                const char* prediction = classifier.predict(features);
                float confidence = classifier.getPredictionConfidence(features);
                
                Serial.println("\nPrediction for Temperature=" + String(temp, 1) + 
                             "°C, Humidity=" + String(humidity, 1) + "%:");
                Serial.println("Result: " + String(prediction));
                Serial.println("Confidence: " + String(confidence * 100, 1) + "%");
            }
        } else if (input == "tree") {
            classifier.printTree();
        } else if (input == "stats") {
            classifier.printTreeStats();
        } else if (input == "help") {
            Serial.println("\nCommands:");
            Serial.println("predict <temp> <humidity> - Make prediction");
            Serial.println("tree - Show full tree");
            Serial.println("stats - Show tree statistics");
            Serial.println("help - Show this help");
        }
    }
    */
    
    delay(5000);
}

void testPredictions() {
    // Test various weather conditions
    struct TestCase {
        float temperature;
        float humidity;
        const char* description;
    };
    
    TestCase testCases[] = {
        {25.0, 45.0, "Perfect weather (moderate temp, low humidity)"},
        {35.0, 85.0, "Hot and humid (uncomfortable)"},
        {20.0, 70.0, "Cool with moderate humidity"},
        {15.0, 95.0, "Cold and very humid"},
        {30.0, 35.0, "Warm and dry (ideal)"},
        {40.0, 50.0, "Very hot but manageable humidity"}
    };
    
    int numTests = sizeof(testCases) / sizeof(TestCase);
    
    for (int i = 0; i < numTests; i++) {
        float features[] = {testCases[i].temperature, testCases[i].humidity};
        const char* prediction = classifier.predict(features);
        
        Serial.println("\nTest " + String(i + 1) + ": " + String(testCases[i].description));
        Serial.println("  Input: Temp=" + String(testCases[i].temperature, 1) + 
                      "°C, Humidity=" + String(testCases[i].humidity, 1) + "%");
        Serial.println("  Prediction: " + String(prediction));
        Serial.println("  Decision path: Follow tree from root to leaf");
    }
}

void showPredictionConfidence() {
    // Show confidence levels for borderline cases
    float borderlineCases[][2] = {
        {25.0, 65.0},  // Moderate temp, moderate-high humidity
        {30.0, 55.0},  // Warm temp, moderate humidity
        {20.0, 75.0},  // Cool temp, high humidity
        {32.0, 70.0}   // Hot temp, high humidity
    };
    
    int numCases = sizeof(borderlineCases) / sizeof(borderlineCases[0]);
    
    for (int i = 0; i < numCases; i++) {
        float features[] = {borderlineCases[i][0], borderlineCases[i][1]};
        const char* prediction = classifier.predict(features);
        float confidence = classifier.getPredictionConfidence(features);
        
        Serial.println("\nBorderline Case " + String(i + 1) + ":");
        Serial.println("  Temp=" + String(borderlineCases[i][0], 1) + 
                      "°C, Humidity=" + String(borderlineCases[i][1], 1) + "%");
        Serial.println("  Prediction: " + String(prediction));
        Serial.println("  Confidence: " + String(confidence * 100, 1) + "%");
        
        if (confidence < 0.7) {
            Serial.println("  Note: Low confidence - close to decision boundary");
        } else if (confidence > 0.9) {
            Serial.println("  Note: High confidence - clear classification");
        }
    }
}

/*
Expected Serial Monitor Output:
================================

Decision Tree Visualization Demo
================================

Adding training data...
Added 15 training samples

Training decision tree...
Training completed successfully!

=== DECISION TREE VISUALIZATION ===
Tree Statistics:
  Total Nodes: 7
  Leaf Nodes: 4
  Internal Nodes: 3
  Tree Depth: 3
  Training Samples: 15
  Unique Classes: 2
  Classes: [Play, NoPlay]

Tree Structure:
Root
└── 🌳 Feature[1] <= 82.50
    ├── [≤ 82.50]
    │   └── 🌳 Feature[0] <= 31.00
    │       ├── [≤ 31.00]
    │       │   └── 🍃 Leaf: Play
    │       └── [> 31.00]
    │           └── 🍃 Leaf: NoPlay
    └── [> 82.50]
        └── 🌳 Feature[0] <= 17.00
            ├── [≤ 17.00]
            │   └── 🍃 Leaf: NoPlay
            └── [> 17.00]
                └── 🍃 Leaf: Play

==================================

The tree shows:
1. First split on humidity (Feature[1]) at 82.5%
2. For low humidity (≤82.5%): split on temperature at 31°C
3. For high humidity (>82.5%): split on temperature at 17°C
4. Final classifications at leaf nodes

This creates intuitive rules:
- Low humidity + moderate temp → Play
- Low humidity + high temp → NoPlay  
- High humidity + very cold → NoPlay
- High humidity + not very cold → Play
*/