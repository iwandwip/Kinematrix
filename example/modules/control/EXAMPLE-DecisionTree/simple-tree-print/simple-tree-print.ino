/*
 * Simple Decision Tree Visualization
 * 
 * Basic example showing how to use the new printTree() functionality.
 * Creates a small decision tree and displays its structure.
 */

#define ENABLE_MODULE_DECISION_TREE
#include "Kinematrix.h"

DecisionTree tree(2, 10, 3, 2);  // 2 features, 10 samples, depth 3, min 2 samples

void setup() {
    Serial.begin(115200);
    Serial.println("Simple Decision Tree Visualization");
    Serial.println("==================================");
    
    // Simple binary classification: Size and Weight -> Type
    // Small, Light -> Type A
    // Large, Heavy -> Type B
    
    // Add training data: [Size, Weight] -> Class
    tree.addTrainingData("TypeA", (float[]){10.0, 5.0});   // Small, Light
    tree.addTrainingData("TypeA", (float[]){12.0, 6.0});   // Small, Light
    tree.addTrainingData("TypeA", (float[]){8.0, 4.0});    // Small, Light
    tree.addTrainingData("TypeB", (float[]){25.0, 20.0});  // Large, Heavy
    tree.addTrainingData("TypeB", (float[]){30.0, 25.0});  // Large, Heavy
    tree.addTrainingData("TypeB", (float[]){28.0, 22.0});  // Large, Heavy
    tree.addTrainingData("TypeA", (float[]){15.0, 8.0});   // Medium-Small
    tree.addTrainingData("TypeB", (float[]){35.0, 30.0});  // Very Large, Very Heavy
    
    // Set feature types
    tree.setFeatureType(0, NUMERIC);  // Size
    tree.setFeatureType(1, NUMERIC);  // Weight
    
    // Train the tree
    Serial.println("Training decision tree...");
    if (tree.train()) {
        Serial.println("Training successful!\n");
        
        // Print the complete tree visualization
        tree.printTree();
        
        // Test some predictions
        Serial.println("=== TESTING PREDICTIONS ===");
        
        float testCases[][2] = {
            {11.0, 5.5},   // Small, Light
            {32.0, 27.0},  // Large, Heavy
            {18.0, 12.0}   // Medium
        };
        
        for (int i = 0; i < 3; i++) {
            const char* prediction = tree.predict(testCases[i]);
            Serial.println("Size=" + String(testCases[i][0], 1) + 
                          ", Weight=" + String(testCases[i][1], 1) + 
                          " -> " + String(prediction));
        }
        
    } else {
        Serial.println("Training failed!");
        if (tree.hasError()) {
            Serial.println("Error: " + String(tree.getErrorMessage()));
        }
    }
}

void loop() {
    // Nothing to do in loop
    delay(1000);
}

/*
Expected Output:
================

Simple Decision Tree Visualization
==================================
Training decision tree...
Training successful!

=== DECISION TREE VISUALIZATION ===
Tree Statistics:
  Total Nodes: 5
  Leaf Nodes: 3
  Internal Nodes: 2
  Tree Depth: 2
  Training Samples: 8
  Unique Classes: 2
  Classes: [TypeA, TypeB]

Tree Structure:
Root
└── 🌳 Feature[0] <= 16.50
    ├── [≤ 16.50]
    │   └── 🍃 Leaf: TypeA
    └── [> 16.50]
        └── 🌳 Feature[1] <= 19.00
            ├── [≤ 19.00]
            │   └── 🍃 Leaf: TypeA
            └── [> 19.00]
                └── 🍃 Leaf: TypeB

==================================

=== TESTING PREDICTIONS ===
Size=11.0, Weight=5.5 -> TypeA
Size=32.0, Weight=27.0 -> TypeB
Size=18.0, Weight=12.0 -> TypeA

The tree shows clear decision rules:
1. If Size ≤ 16.5 → TypeA (small items)
2. If Size > 16.5 and Weight ≤ 19.0 → TypeA (medium size, light weight)
3. If Size > 16.5 and Weight > 19.0 → TypeB (large and heavy)
*/