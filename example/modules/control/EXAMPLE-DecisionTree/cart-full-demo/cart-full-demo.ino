/*
 * CART (Classification and Regression Trees) Full Demo
 * 
 * Demonstrates the full CART implementation with:
 * - Classification trees with Gini impurity
 * - Regression trees with MSE
 * - Feature importance calculation
 * - Cost complexity pruning
 * - Class probability estimation
 * - Tree visualization
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

// Create two trees - one for classification, one for regression
DecisionTree classifier(3, 50, 8, 3, CLASSIFICATION);   // Classification tree
DecisionTree regressor(2, 30, 6, 3, REGRESSION);        // Regression tree

void setup() {
    Serial.begin(115200);
    Serial.println("CART (Classification and Regression Trees) Full Demo");
    Serial.println("====================================================");
    
    // Demo 1: Classification Tree for IoT Sensor Classification
    demonstrateClassification();
    
    delay(2000);
    
    // Demo 2: Regression Tree for Sensor Value Prediction
    demonstrateRegression();
    
    delay(2000);
    
    // Demo 3: Feature Importance Analysis  
    demonstrateFeatureImportance();
}

void loop() {
    // Nothing in loop for this demo
    delay(1000);
}

void demonstrateClassification() {
    Serial.println("\n=== CART CLASSIFICATION DEMO ===");
    Serial.println("Scenario: IoT Environmental Monitoring Classification");
    Serial.println("Input: [Temperature, Humidity, Light] -> Output: Environment Type");
    
    // Add training data for environmental classification
    // Features: [Temperature(°C), Humidity(%), Light(%)]
    // Classes: Indoor, Outdoor, Greenhouse
    
    // Indoor environment samples
    classifier.addTrainingData("Indoor", (float[]){22.0, 45.0, 30.0});    // Comfortable indoor
    classifier.addTrainingData("Indoor", (float[]){24.0, 40.0, 25.0});    // Living room
    classifier.addTrainingData("Indoor", (float[]){21.0, 50.0, 35.0});    // Office
    classifier.addTrainingData("Indoor", (float[]){23.0, 42.0, 28.0});    // Bedroom
    classifier.addTrainingData("Indoor", (float[]){25.0, 38.0, 32.0});    // Kitchen
    
    // Outdoor environment samples  
    classifier.addTrainingData("Outdoor", (float[]){28.0, 60.0, 85.0});   // Sunny day
    classifier.addTrainingData("Outdoor", (float[]){32.0, 55.0, 90.0});   // Hot sunny
    classifier.addTrainingData("Outdoor", (float[]){18.0, 80.0, 40.0});   // Cloudy day
    classifier.addTrainingData("Outdoor", (float[]){15.0, 85.0, 20.0});   // Rainy day
    classifier.addTrainingData("Outdoor", (float[]){35.0, 45.0, 95.0});   // Very hot day
    
    // Greenhouse environment samples
    classifier.addTrainingData("Greenhouse", (float[]){26.0, 70.0, 60.0}); // Controlled environment
    classifier.addTrainingData("Greenhouse", (float[]){27.0, 75.0, 65.0}); // Optimal growing
    classifier.addTrainingData("Greenhouse", (float[]){25.0, 68.0, 55.0}); // Moderate conditions
    classifier.addTrainingData("Greenhouse", (float[]){28.0, 72.0, 70.0}); // High humidity
    classifier.addTrainingData("Greenhouse", (float[]){24.0, 65.0, 50.0}); // Cool greenhouse
    
    // Set feature types
    classifier.setFeatureType(0, NUMERIC);  // Temperature
    classifier.setFeatureType(1, NUMERIC);  // Humidity  
    classifier.setFeatureType(2, NUMERIC);  // Light
    
    // Train with CART methods
    Serial.println("\nTraining CART Classification Tree...");
    if (classifier.trainClassification(GINI)) {
        Serial.println("Training successful!");
        
        // Display tree structure
        classifier.printTree();
        
        // Display CART-specific information
        classifier.printCARTInfo();
        
        // Test predictions with probability estimates
        Serial.println("=== CLASSIFICATION PREDICTIONS ===");
        
        float testCases[][3] = {
            {23.0, 45.0, 30.0},    // Typical indoor
            {30.0, 60.0, 85.0},    // Typical outdoor  
            {26.0, 70.0, 60.0},    // Typical greenhouse
            {20.0, 65.0, 45.0}     // Ambiguous case
        };
        
        const char* descriptions[] = {
            "Typical indoor conditions",
            "Typical outdoor conditions", 
            "Typical greenhouse conditions",
            "Ambiguous conditions"
        };
        
        for (int i = 0; i < 4; i++) {
            const char* prediction = classifier.predict(testCases[i]);
            float confidence = classifier.getPredictionConfidence(testCases[i]);
            float* probabilities = classifier.getPredictionProbabilities(testCases[i]);
            
            Serial.println("\nTest Case " + String(i+1) + ": " + String(descriptions[i]));
            Serial.println("  Input: Temp=" + String(testCases[i][0], 1) + 
                          "°C, Humidity=" + String(testCases[i][1], 1) + 
                          "%, Light=" + String(testCases[i][2], 1) + "%");
            Serial.println("  Prediction: " + String(prediction));
            Serial.println("  Confidence: " + String(confidence * 100, 1) + "%");
            
            if (probabilities != nullptr) {
                Serial.println("  Class Probabilities:");
                // Note: This assumes we know the order of classes
                Serial.println("    Indoor: " + String(probabilities[0] * 100, 1) + "%");
                Serial.println("    Outdoor: " + String(probabilities[1] * 100, 1) + "%"); 
                Serial.println("    Greenhouse: " + String(probabilities[2] * 100, 1) + "%");
                delete[] probabilities;
            }
        }
        
    } else {
        Serial.println("Classification training failed!");
        if (classifier.hasError()) {
            Serial.println("Error: " + String(classifier.getErrorMessage()));
        }
    }
}

void demonstrateRegression() {
    Serial.println("\n\n=== CART REGRESSION DEMO ===");
    Serial.println("Scenario: IoT Sensor Value Prediction");
    Serial.println("Input: [Temperature, Humidity] -> Output: Power Consumption (Watts)");
    
    // Add training data for power consumption prediction
    // Features: [Temperature(°C), Humidity(%)]
    // Target: Power consumption in Watts
    
    // Power consumption increases with temperature (AC usage)
    // and decreases with optimal humidity levels
    regressor.addTrainingData(150.0, (float[]){20.0, 50.0});  // Cool, optimal humidity - low power
    regressor.addTrainingData(180.0, (float[]){22.0, 45.0});  // Moderate temp - moderate power
    regressor.addTrainingData(220.0, (float[]){25.0, 40.0});  // Warm - higher power
    regressor.addTrainingData(280.0, (float[]){28.0, 35.0});  // Hot, dry - high power (AC)
    regressor.addTrainingData(350.0, (float[]){32.0, 30.0});  // Very hot - very high power
    regressor.addTrainingData(380.0, (float[]){35.0, 25.0});  // Extreme heat - maximum power
    
    // High humidity also increases power (dehumidifier)
    regressor.addTrainingData(200.0, (float[]){20.0, 75.0});  // Cool but humid
    regressor.addTrainingData(250.0, (float[]){22.0, 80.0});  // Moderate temp, high humidity
    regressor.addTrainingData(320.0, (float[]){25.0, 85.0});  // Warm and very humid
    regressor.addTrainingData(300.0, (float[]){30.0, 90.0});  // Hot and extremely humid
    
    // Optimal conditions - lowest power
    regressor.addTrainingData(120.0, (float[]){21.0, 55.0});  // Perfect conditions
    regressor.addTrainingData(130.0, (float[]){23.0, 52.0});  // Near perfect
    regressor.addTrainingData(140.0, (float[]){24.0, 48.0});  // Good conditions
    
    // Set feature types
    regressor.setFeatureType(0, NUMERIC);  // Temperature
    regressor.setFeatureType(1, NUMERIC);  // Humidity
    
    // Train regression tree
    Serial.println("\nTraining CART Regression Tree...");
    if (regressor.trainRegression()) {
        Serial.println("Training successful!");
        
        // Display tree structure
        regressor.printTree();
        
        // Display CART-specific information
        regressor.printCARTInfo();
        
        // Test regression predictions
        Serial.println("=== REGRESSION PREDICTIONS ===");
        
        float testCases[][2] = {
            {21.0, 50.0},    // Optimal conditions
            {26.0, 45.0},    // Moderate conditions
            {30.0, 70.0},    // Hot and humid
            {35.0, 30.0}     // Very hot and dry
        };
        
        const char* descriptions[] = {
            "Optimal conditions (low power expected)",
            "Moderate conditions",
            "Hot and humid (high power expected)",
            "Very hot and dry (maximum power expected)"
        };
        
        for (int i = 0; i < 4; i++) {
            float prediction = regressor.predictRegression(testCases[i]);
            
            Serial.println("\nTest Case " + String(i+1) + ": " + String(descriptions[i]));
            Serial.println("  Input: Temp=" + String(testCases[i][0], 1) + 
                          "°C, Humidity=" + String(testCases[i][1], 1) + "%");
            Serial.println("  Predicted Power: " + String(prediction, 1) + " Watts");
            
            // Provide interpretation
            if (prediction < 160) {
                Serial.println("  Interpretation: Low power consumption - efficient operation");
            } else if (prediction < 250) {
                Serial.println("  Interpretation: Moderate power consumption");
            } else if (prediction < 320) {
                Serial.println("  Interpretation: High power consumption - AC/dehumidifier active");
            } else {
                Serial.println("  Interpretation: Very high power consumption - extreme conditions");
            }
        }
        
        // Test model accuracy
        Serial.println("\n=== MODEL EVALUATION ===");
        
        // Create test dataset
        const float* testFeatures[] = {
            (float[]){22.0, 48.0},
            (float[]){27.0, 55.0}, 
            (float[]){33.0, 40.0}
        };
        float testTargets[] = {160.0, 240.0, 360.0};  // Expected values
        
        float mse = regressor.evaluateRegressionError(testFeatures, testTargets, 3);
        Serial.println("Test MSE: " + String(mse, 2));
        Serial.println("RMSE: " + String(sqrt(mse), 2) + " Watts");
        
    } else {
        Serial.println("Regression training failed!");
        if (regressor.hasError()) {
            Serial.println("Error: " + String(regressor.getErrorMessage()));
        }
    }
}

void demonstrateFeatureImportance() {
    Serial.println("\n\n=== FEATURE IMPORTANCE ANALYSIS ===");
    
    // Analyze classification tree feature importance
    Serial.println("\nClassification Tree Feature Importance:");
    float* classifierImportance = classifier.getFeatureImportance();
    if (classifierImportance != nullptr) {
        Serial.println("  Temperature: " + String(classifierImportance[0] * 100, 2) + "%");
        Serial.println("  Humidity: " + String(classifierImportance[1] * 100, 2) + "%");
        Serial.println("  Light: " + String(classifierImportance[2] * 100, 2) + "%");
        
        // Find most important feature
        int mostImportant = 0;
        for (int i = 1; i < 3; i++) {
            if (classifierImportance[i] > classifierImportance[mostImportant]) {
                mostImportant = i;
            }
        }
        
        const char* featureNames[] = {"Temperature", "Humidity", "Light"};
        Serial.println("  Most important feature: " + String(featureNames[mostImportant]));
        
        delete[] classifierImportance;
    }
    
    // Analyze regression tree feature importance  
    Serial.println("\nRegression Tree Feature Importance:");
    float* regressorImportance = regressor.getFeatureImportance();
    if (regressorImportance != nullptr) {
        Serial.println("  Temperature: " + String(regressorImportance[0] * 100, 2) + "%");
        Serial.println("  Humidity: " + String(regressorImportance[1] * 100, 2) + "%");
        
        if (regressorImportance[0] > regressorImportance[1]) {
            Serial.println("  Temperature is more important for power prediction");
        } else {
            Serial.println("  Humidity is more important for power prediction");
        }
        
        delete[] regressorImportance;
    }
    
    // Demonstrate pruning
    Serial.println("\n=== PRUNING DEMONSTRATION ===");
    classifier.printPruningPath();
    
    Serial.println("\nApplying cost complexity pruning...");
    classifier.performCostComplexityPruning(0.02f);  // Apply moderate pruning
    
    Serial.println("Tree after pruning:");
    classifier.printTreeStats();
}

/*
Expected Output Summary:
========================

The demo will show:

1. CLASSIFICATION TREE:
   - Environmental monitoring with 3 classes (Indoor/Outdoor/Greenhouse)
   - Tree structure with decision nodes showing feature splits
   - Prediction probabilities for each class
   - Feature importance analysis

2. REGRESSION TREE:
   - Power consumption prediction based on temperature/humidity
   - Continuous value predictions
   - Mean Squared Error evaluation
   - Different tree structure optimized for regression

3. CART-SPECIFIC FEATURES:
   - Feature importance scores showing which variables matter most
   - Cost complexity pruning to prevent overfitting
   - Gini impurity calculations for classification
   - MSE calculations for regression

4. PRACTICAL INSIGHTS:
   - How temperature/humidity affect environment classification
   - How weather conditions impact power consumption
   - Which sensors are most important for each prediction task
   - Tree complexity analysis and optimization

This demonstrates why CART is so popular in IoT:
- Handles both classification and regression in one framework
- Provides interpretable decision rules
- Shows feature importance for sensor selection
- Fast inference suitable for embedded systems
- Built-in pruning prevents overfitting with noisy sensor data
*/