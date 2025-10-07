# K-Nearest Neighbors (KNN): Comprehensive Technical Documentation

## Table of Contents
1. [Quick Start for Beginners](#quick-start-for-beginners)
2. [Introduction and Definitions](#introduction-and-definitions)
3. [Theoretical Foundation](#theoretical-foundation)
4. [Mathematical Framework](#mathematical-framework)
5. [Algorithm Design and Implementation](#algorithm-design-and-implementation)
6. [Implementation Architecture](#implementation-architecture)
7. [Usage Examples and Best Practices](#usage-examples-and-best-practices)
8. [Performance Analysis](#performance-analysis)
9. [Troubleshooting and Optimization](#troubleshooting-and-optimization)
10. [References](#references)

---

## 🚀 Quick Start for Beginners

### What is K-Nearest Neighbors? (In Simple Terms)

Bayangkan Anda **pindah ke kota baru** dan ingin tahu apakah tetangga baru Anda ramah atau tidak. Apa yang Anda lakukan?
- Lihat 5 tetangga terdekat dari rumah Anda
- Jika 4 dari 5 tetangga ramah → kemungkinan besar lingkungan ini ramah
- Jika 4 dari 5 tetangga tidak ramah → kemungkinan lingkungan ini tidak ramah

**KNN bekerja persis seperti itu!** Algoritma ini melihat K data terdekat (neighbors) untuk menentukan klasifikasi data baru.

### Visual Example: Sensor Classification

Misalkan kita punya data sensor suhu dan kelembaban untuk klasifikasi "Nyaman" vs "Tidak Nyaman":

```
           Humidity (%)
               ^
           100 |                    
               |      x (Hot)        
            80 |   x        x      
               | (Hot)   (Hot)      
            60 |    o        x     
               |  (Cool)  (Hot)    
            40 |    o    o          
               |  (Cool)(Cool)      
            20 |    o               
               |  (Cool)           
             0 +--+--+--+--+--+---> Temperature (°C)
               0 10 20 30 40 50

        ? = New data point (28°C, 65%)
        
        Find K=3 nearest neighbors:
        1. (30°C, 70%) = Hot     - distance = 7.07
        2. (25°C, 60%) = Cool    - distance = 5.83  
        3. (32°C, 68%) = Hot     - distance = 4.47
        
        Vote: Hot=2, Cool=1 → Prediction: HOT
```

**Dalam kode, ini menjadi:**
```cpp
KNN classifier(3, 2, 50);  // K=3, 2 features, max 50 data

// Add training data
float hot1[] = {30, 70}; classifier.addTrainingData("HOT", hot1);
float cool1[] = {25, 60}; classifier.addTrainingData("COOL", cool1);
// ... add more data

// Predict new point
float newPoint[] = {28, 65};
const char* result = classifier.predict(newPoint); // "HOT"
```

**KNN LEBIH PINTAR** karena:
- Otomatis menghitung distance ke semua training data
- Memilih K neighbors terdekat secara optimal
- Handle multiple distance metrics (Euclidean, Manhattan, Cosine)
- Weighted voting berdasarkan distance

### 🔍 Step-by-Step Example: Building KNN Manually

Mari kita buat KNN classifier untuk **gesture recognition** berdasarkan accelerometer:

**Training Data (X, Y, Z acceleration):**
```
Gesture    |  X   |  Y   |  Z
-----------|------|------|------
Shake      | 2.1  | 0.3  | 9.8
Shake      | 2.5  | 0.1  | 9.9  
Tilt       | 0.2  | 3.1  | 9.1
Tilt       | 0.1  | 3.5  | 9.0
Tap        | 0.0  | 0.2  | 12.1
Tap        | 0.1  | 0.0  | 11.9
```

**Step 1: Calculate Distances (K=3)**

Test point: `X=2.0, Y=0.5, Z=9.7`

**Euclidean Distance Formula**: `d = √[(x₁-x₂)² + (y₁-y₂)² + (z₁-z₂)²]`

```
To Shake(2.1,0.3,9.8): d = √[(2.0-2.1)² + (0.5-0.3)² + (9.7-9.8)²] = √[0.01+0.04+0.01] = 0.245
To Shake(2.5,0.1,9.9): d = √[(2.0-2.5)² + (0.5-0.1)² + (9.7-9.9)²] = √[0.25+0.16+0.04] = 0.671
To Tilt(0.2,3.1,9.1):  d = √[(2.0-0.2)² + (0.5-3.1)² + (9.7-9.1)²] = √[3.24+6.76+0.36] = 3.192
To Tilt(0.1,3.5,9.0):  d = √[(2.0-0.1)² + (0.5-3.5)² + (9.7-9.0)²] = √[3.61+9.00+0.49] = 3.619
To Tap(0.0,0.2,12.1):  d = √[(2.0-0.0)² + (0.5-0.2)² + (9.7-12.1)²] = √[4.00+0.09+5.76] = 3.138
To Tap(0.1,0.0,11.9):  d = √[(2.0-0.1)² + (0.5-0.0)² + (9.7-11.9)²] = √[3.61+0.25+4.84] = 2.947
```

**Step 2: Sort by Distance and Pick K=3 Nearest**

```
1. Shake(2.1,0.3,9.8) - distance: 0.245  ← Nearest
2. Shake(2.5,0.1,9.9) - distance: 0.671  ← 2nd nearest  
3. Tap(0.1,0.0,11.9)  - distance: 2.947  ← 3rd nearest
```

**Step 3: Vote**

```
K=3 neighbors:
- Shake: 2 votes
- Tap: 1 vote

Result: SHAKE (majority vote)
```

**Step 4: Implementation**

```cpp
KNN gestureClassifier(3, 3, 20);  // K=3, 3 features (X,Y,Z), max 20 data

void setup() {
    // Add training data
    float shake1[] = {2.1, 0.3, 9.8}; gestureClassifier.addTrainingData("SHAKE", shake1);
    float shake2[] = {2.5, 0.1, 9.9}; gestureClassifier.addTrainingData("SHAKE", shake2);
    float tilt1[] = {0.2, 3.1, 9.1};  gestureClassifier.addTrainingData("TILT", tilt1);
    float tilt2[] = {0.1, 3.5, 9.0};  gestureClassifier.addTrainingData("TILT", tilt2);
    float tap1[] = {0.0, 0.2, 12.1};  gestureClassifier.addTrainingData("TAP", tap1);
    float tap2[] = {0.1, 0.0, 11.9};  gestureClassifier.addTrainingData("TAP", tap2);
}

void loop() {
    // Read accelerometer
    float accel[] = {readAccelX(), readAccelY(), readAccelZ()};
    
    // Classify gesture
    const char* gesture = gestureClassifier.predict(accel);
    Serial.println("Detected gesture: " + String(gesture));
    
    delay(100);
}
```

### 🎯 Key Insights

**Mengapa KNN efektif?**
- **Lazy learning**: Tidak perlu training phase yang kompleks
- **Robust**: Tahan terhadap outliers jika K cukup besar
- **Intuitive**: Konsep "similarity" mudah dipahami
- **Non-parametric**: Tidak assume distribusi data tertentu

**Kapan pakai KNN?**
- Pattern recognition (gesture, voice, image)
- Recommendation systems
- Anomaly detection
- Real-time classification dengan data yang terus bertambah

---

## 1. Introduction and Definitions

### 1.1 K-Nearest Neighbors Definition

**K-Nearest Neighbors (KNN)** adalah algoritma machine learning **lazy learning** yang melakukan klasifikasi atau regresi berdasarkan mayoritas kelas dari K data training terdekat dalam feature space [Cover & Hart, 1967]. Algoritma ini tidak membangun model eksplisit selama training, tetapi menyimpan semua training data dan melakukan komputasi saat prediction.

### 1.2 Key Characteristics

**Lazy Learning Algorithm**: Tidak ada phase training eksplisit. Semua komputasi dilakukan saat prediction time.

**Instance-Based Learning**: Keputusan didasarkan pada similarity dengan instance training data yang sudah ada.

**Non-Parametric**: Tidak membuat asumsi tentang distribusi data yang mendasari.

**Memory-Based**: Menyimpan semua training data dalam memori untuk referensi saat prediction.

### 1.3 Historical Background

KNN pertama kali diperkenalkan oleh **Fix dan Hodges (1951)** dalam konteks discriminant analysis. Perkembangan teoritis penting dilakukan oleh **Cover dan Hart (1967)** yang membuktikan bahwa KNN memiliki error rate maksimum 2 kali Bayes error rate ketika jumlah data mendekati infinite.

### 1.4 Types of KNN

**Classification KNN**: Prediksi berupa kelas diskrit berdasarkan majority voting dari K neighbors terdekat.

**Regression KNN**: Prediksi berupa nilai numerik berdasarkan rata-rata (atau weighted average) dari K neighbors terdekat.

**Weighted KNN**: Neighbors yang lebih dekat diberi bobot lebih besar dalam voting.

**Distance-Weighted KNN**: Bobot berbanding terbalik dengan distance (w = 1/distance).

### 1.5 Advantages and Disadvantages

**Advantages**:
- **Simple to understand**: Konsep intuitive dan mudah dijelaskan
- **No training period**: Dapat langsung digunakan setelah data ditambahkan
- **Adaptive**: Performance meningkat seiring bertambahnya data
- **Multi-class**: Naturally handle multi-class classification
- **Robust to outliers**: Jika K dipilih dengan tepat

**Disadvantages**:
- **Computationally expensive**: O(n) untuk setiap prediction
- **Memory intensive**: Harus menyimpan semua training data
- **Sensitive to irrelevant features**: Curse of dimensionality
- **Sensitive to K value**: Performance sangat tergantung pilihan K
- **Imbalanced data**: Bias terhadap kelas majority

### 1.6 Real-World Analogies for Better Understanding

**🛒 E-Commerce Recommendation (Collaborative Filtering)**
```
User A suka: [Laptop, Mouse, Keyboard]
User B suka: [Laptop, Mouse, Headset]  ← Similar to A
User C suka: [Phone, Case, Charger]
User D suka: [Laptop, Mouse, Monitor]  ← Similar to A

Recommend to A: Headset, Monitor (from similar users B & D)
```

**🏥 Medical Diagnosis (Symptom-Based)**
```
Patient symptoms: [Fever=38°C, Headache=7/10, Cough=Yes]

Find K=3 most similar cases:
1. [Fever=37.8°C, Headache=8/10, Cough=Yes] → Flu
2. [Fever=38.2°C, Headache=6/10, Cough=Yes] → Flu  
3. [Fever=38.1°C, Headache=7/10, Cough=No] → Cold

Diagnosis: Flu (2/3 votes)
```

**🎵 Music Recommendation (Feature-Based)**
```
Song features: [Tempo=120bpm, Energy=0.8, Danceability=0.7]

Find similar songs:
1. [Tempo=118bpm, Energy=0.85, Danceability=0.72] → Rock
2. [Tempo=125bpm, Energy=0.78, Danceability=0.68] → Pop
3. [Tempo=122bpm, Energy=0.82, Danceability=0.71] → Rock

Recommendation: Rock songs (similar musical characteristics)
```

**🏠 Real Estate Valuation (Location-Based)**
```
House features: [Size=100m², Bedrooms=3, Distance_to_city=5km]

Find K=5 nearest houses:
1. [Size=98m², Bedrooms=3, Distance=4.8km] → $250k
2. [Size=105m², Bedrooms=3, Distance=5.2km] → $260k
3. [Size=95m², Bedrooms=2, Distance=4.9km] → $230k
4. [Size=102m², Bedrooms=3, Distance=5.1km] → $255k
5. [Size=100m², Bedrooms=3, Distance=5.3km] → $248k

Estimated price: $248.6k (average of similar houses)
```

### 1.7 Why KNN is Perfect for Embedded Systems

**🔋 Resource Considerations**
- **No complex training**: Langsung ready to use
- **Incremental learning**: Mudah menambah data baru
- **Predictable memory**: Memory usage = data_size × feature_size
- **Simple computation**: Hanya distance calculation dan sorting

**🤖 Real-time Applications**
```cpp
// Real-time gesture recognition
if (gestureKNN.predict(accelData) == "SHAKE") {
    Serial.println("Shake detected in " + String(millis()-startTime) + "ms");
}

// Sensor fault detection
if (sensorKNN.predict(sensorReadings) == "FAULTY") {
    activateBackupSensor();
}
```

**📊 Handles Sensor Characteristics**
- **Noisy data**: Averaging effect dari multiple neighbors
- **Sensor drift**: Mudah retrain dengan data baru
- **Multi-modal**: Combine berbagai jenis sensor
- **Non-linear patterns**: Tidak assume linear relationships

### ⚡ 5-Minute Quick Start Guide

**Step 1: Include Library**
```cpp
#define ENABLE_MODULE_KNN
#include "Kinematrix.h"
```

**Step 2: Create KNN Object**
```cpp
// KNN(k, maxFeatures, maxData)
KNN classifier(3, 2, 50);  // K=3, 2 features, max 50 training data
```

**Step 3: Add Training Data**
```cpp
float hot[] = {35, 80};     classifier.addTrainingData("HOT", hot);
float warm[] = {28, 60};    classifier.addTrainingData("WARM", warm);
float cool[] = {20, 40};    classifier.addTrainingData("COOL", cool);
// Add more data...
```

**Step 4: Predict**
```cpp
float sensorData[] = {32, 70};
const char* result = classifier.predict(sensorData);
Serial.println("Classification: " + String(result));
```

**Complete 5-Minute Example:**
```cpp
#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

KNN tempClassifier(3, 2, 20);

void setup() {
    Serial.begin(115200);
    
    // Training data (Temperature, Humidity -> Comfort Level)
    float data1[] = {18, 45}; tempClassifier.addTrainingData("COLD", data1);
    float data2[] = {22, 50}; tempClassifier.addTrainingData("COOL", data2);
    float data3[] = {26, 55}; tempClassifier.addTrainingData("WARM", data3);
    float data4[] = {30, 70}; tempClassifier.addTrainingData("HOT", data4);
    float data5[] = {35, 80}; tempClassifier.addTrainingData("HOT", data5);
    float data6[] = {20, 60}; tempClassifier.addTrainingData("COOL", data6);
    
    // Test prediction
    float test[] = {24, 58};
    Serial.println("24C, 58% humidity -> " + String(tempClassifier.predict(test)));
}

void loop() {
    // Your sensor reading and classification code here
}
```

### ⚠️ Common Misconceptions & Gotchas

**❌ SALAH: "KNN tidak perlu training"**
- KNN tetap perlu fase "learning" dengan menambah training data
- Bedanya: tidak ada model building, tapi data tetap perlu dikumpulkan

**✅ BENAR: "KNN adalah lazy learning"**
```cpp
// No explicit training phase, but still need data
classifier.addTrainingData("CLASS_A", dataA);  // This IS learning
classifier.addTrainingData("CLASS_B", dataB);
```

**❌ SALAH: "K yang besar selalu lebih baik"**
- K terlalu besar → over-smoothing, hilang local patterns
- K terlalu kecil → sensitive to noise
- K optimal biasanya √n atau menggunakan cross validation

**✅ BENAR: "K optimal tergantung data"**
```cpp
// Find optimal K through cross validation
float bestAccuracy = 0;
int bestK = 1;
for (int k = 1; k <= 10; k++) {
    KNN testKNN(k, features, maxData);
    float acc = testKNN.crossValidate(5);
    if (acc > bestAccuracy) {
        bestAccuracy = acc;
        bestK = k;
    }
}
```

**❌ SALAH: "Semua features sama penting"**
- Features dengan range besar mendominasi distance calculation
- Perlu normalization atau feature scaling

**✅ BENAR: "Feature scaling penting untuk KNN"**
```cpp
classifier.enableNormalization(true);  // Auto-scale features to [0,1]
```

**❌ SALAH: "Distance metric tidak penting"**
- Euclidean bagus untuk continuous data
- Manhattan bagus untuk high-dimensional sparse data
- Cosine bagus untuk text/document similarity

**✅ BENAR: "Pilih distance metric sesuai data"**
```cpp
classifier.setDistanceMetric(EUCLIDEAN);  // For sensor data
// classifier.setDistanceMetric(MANHATTAN);  // For categorical-like data
// classifier.setDistanceMetric(COSINE);     // For normalized vectors
```

**❌ SALAH: "KNN cepat karena no training"**
- Training memang instant, tapi prediction lambat O(n)
- Harus hitung distance ke semua training data
- Perlu sorting untuk find K nearest

**✅ BENAR: "KNN trade-off: fast training, slow prediction"**
```cpp
// For real-time applications, consider:
classifier.setLowMemoryMode(true);  // Reduce memory usage
// Or use smaller training set for speed
```

### 🔧 Troubleshooting Quick Fixes

**Problem: "Prediction always returns same class"**
```cpp
// Check if training data is balanced
Serial.println("Class A count: " + String(knn.getDataCountByLabel("CLASS_A")));
Serial.println("Class B count: " + String(knn.getDataCountByLabel("CLASS_B")));

// Solution: Add more data for minority class or use weighted voting
knn.setWeightedVoting(true);
```

**Problem: "Low accuracy on test data"**
```cpp
// Try different K values
float acc = knn.crossValidate(5);
Serial.println("Accuracy with current K: " + String(acc));

// Try normalization
knn.enableNormalization(true);
```

**Problem: "Memory allocation failed"**
```cpp
// Reduce parameters or enable low memory mode
KNN smallerKNN(3, 4, 100);  // Instead of (5, 10, 1000)
smallerKNN.setLowMemoryMode(true);
```

**Problem: "Prediction takes too long"**
```cpp
// Reduce training data size or use low memory mode
knn.setLowMemoryMode(true);  // Trade accuracy for speed
```

---

## 2. Theoretical Foundation

### 2.1 Statistical Learning Theory

KNN didasarkan pada **non-parametric density estimation** dan **Bayes decision theory**. Algoritma ini mengasumsikan bahwa data points yang dekat dalam feature space memiliki kemungkinan besar untuk memiliki label yang sama.

### 2.2 Curse of Dimensionality

Dalam high-dimensional space, semua points cenderung memiliki distance yang sama (**distance concentration**). Hal ini membuat concept "nearest neighbor" menjadi less meaningful. Untuk embedded systems, biasanya dibatasi pada ≤10 features.

### 2.3 Bias-Variance Tradeoff

- **Small K**: Low bias, high variance (overfitting to local noise)
- **Large K**: High bias, low variance (over-smoothing decision boundary)
- **Optimal K**: Balance antara bias dan variance

---

## 3. Mathematical Framework

### 3.1 Distance Metrics

**Euclidean Distance (L2 norm)**:
```
d_euclidean(x, y) = √[Σ(i=1 to n) (xᵢ - yᵢ)²]
```

**Properties**:
- Paling umum digunakan
- Bagus untuk continuous features dengan distribusi normal
- Sensitive terhadap outliers

**Implementation**:
```cpp
float calculateEuclideanDistance(const float dataPoint[], const float trainDataPoint[]) {
    float sum = 0;
    for (int i = 0; i < maxFeatures; i++) {
        float diff = dataPoint[i] - trainDataPoint[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}
```

**Manhattan Distance (L1 norm)**:
```
d_manhattan(x, y) = Σ(i=1 to n) |xᵢ - yᵢ|
```

**Properties**:
- Less sensitive terhadap outliers
- Bagus untuk high-dimensional sparse data
- Computational lebih efisien (no sqrt)

**Implementation**:
```cpp
float calculateManhattanDistance(const float dataPoint[], const float trainDataPoint[]) {
    float sum = 0;
    for (int i = 0; i < maxFeatures; i++) {
        sum += abs(dataPoint[i] - trainDataPoint[i]);
    }
    return sum;
}
```

**Cosine Distance**:
```
d_cosine(x, y) = 1 - (x·y) / (||x|| × ||y||)

where: x·y = Σ(i=1 to n) xᵢyᵢ
       ||x|| = √[Σ(i=1 to n) xᵢ²]
```

**Properties**:
- Measure angle between vectors, bukan magnitude
- Bagus untuk text classification atau normalized data
- Range: [0, 2]

### 3.2 Voting Mechanisms

**Simple Majority Vote**:
```
ŷ = argmax(c ∈ C) Σ(i=1 to k) I(yᵢ = c)

where I(condition) = 1 if true, 0 if false
```

**Distance-Weighted Vote**:
```
ŷ = argmax(c ∈ C) Σ(i=1 to k) wᵢ × I(yᵢ = c)

where wᵢ = 1 / (dᵢ + ε), ε = small constant to avoid division by zero
```

**Confidence Score**:
```
confidence = max_votes / k

For weighted voting:
confidence = max_weighted_votes / total_weights
```

### 3.3 Feature Normalization

**Min-Max Normalization**:
```
x_normalized = (x - x_min) / (x_max - x_min)
```

**Implementation**:
```cpp
float normalizeFeature(float value, int featureIndex) {
    if (featureMax[featureIndex] == featureMin[featureIndex]) {
        return 0.5;  // If all values same, return middle value
    }
    return (value - featureMin[featureIndex]) / 
           (featureMax[featureIndex] - featureMin[featureIndex]);
}
```

### 3.4 Optimal K Selection

**Rule of Thumb**:
```
K_optimal ≈ √n

where n = number of training samples
```

**Cross-Validation Method**:
```
For k = 1 to K_max:
    accuracy[k] = cross_validate(k, folds=5)
    
K_optimal = argmax(accuracy)
```

### 3.5 Error Analysis

**Bayes Error Rate Relationship** [Cover & Hart, 1967]:
```
P_error(KNN) ≤ 2 × P_error(Bayes)  as n → ∞
```

**Finite Sample Error**:
```
P_error(KNN) ≈ P_error(Bayes) × [1 + 2√(P_error(Bayes) × ln(2)/k)]
```

---

## 4. Algorithm Design and Implementation

### 4.1 Core Algorithm Pseudocode

**KNN Prediction Algorithm**:
```
ALGORITHM: KNN_Predict(query_point, training_data, k)
INPUT: query_point, training_data, k
OUTPUT: predicted_class

1. distances = []
2. FOR each training_sample in training_data DO
3.    distance = calculate_distance(query_point, training_sample.features)
4.    distances.append((distance, training_sample.label))
5. END FOR
6. 
7. sorted_distances = sort(distances, key=distance)
8. k_nearest = sorted_distances[:k]
9. 
10. IF weighted_voting THEN
11.    votes = calculate_weighted_votes(k_nearest)
12. ELSE
13.    votes = calculate_simple_votes(k_nearest)
14. END IF
15. 
16. RETURN most_frequent_class(votes)
```

**Distance Calculation with Normalization**:
```
ALGORITHM: Calculate_Distance(point1, point2, metric)
INPUT: point1, point2, distance_metric
OUTPUT: distance

1. IF normalization_enabled THEN
2.    point1 = normalize_features(point1)
3.    point2 = normalize_features(point2)
4. END IF
5. 
6. SWITCH metric:
7.    CASE EUCLIDEAN:
8.       RETURN sqrt(sum((point1[i] - point2[i])² for i in features))
9.    CASE MANHATTAN:
10.      RETURN sum(|point1[i] - point2[i]| for i in features)
11.   CASE COSINE:
12.      RETURN 1 - dot_product(point1, point2) / (norm(point1) * norm(point2))
13. END SWITCH
```

### 4.2 Embedded Systems Optimizations

**Low Memory Mode**:
```cpp
// Instead of storing all distances, use fixed-size buffer
struct DistanceIndex {
    float distance;
    int index;
};
DistanceIndex distanceBuffer[k];  // Only store k distances

// Update buffer incrementally
void updateKNearest(float distance, int index) {
    if (distance < distanceBuffer[k-1].distance) {
        distanceBuffer[k-1] = {distance, index};
        // Re-sort buffer (insertion sort for small k)
        insertionSort(distanceBuffer, k);
    }
}
```

---

## 5. Implementation Architecture

### 5.1 Class Structure

```cpp
class KNN {
private:
    int k;                      // Number of neighbors
    int maxFeatures;            // Number of features
    int maxData;               // Maximum training data
    int currentDataSize;       // Current data count
    float **trainingData;      // Training features
    char **trainingLabels;     // Training labels
    
    // Configuration options
    DistanceMetric metric;     // EUCLIDEAN, MANHATTAN, COSINE
    bool useWeightedVoting;    // Weight by inverse distance
    bool normalizationEnabled; // Auto-normalize features
    bool lowMemoryMode;        // Memory-optimized prediction
    bool debugMode;            // Debug output
    
    // Normalization parameters
    float *featureMin;         // Min values for each feature
    float *featureMax;         // Max values for each feature
    
    // Temporary buffers
    DistanceIndex *distanceBuffer;  // For sorting distances
    int *voteBuffer;               // For counting votes
};
```

### 5.2 Key Implementation Methods

**Adding Training Data**:
```cpp
bool addTrainingData(const char *label, const float features[]);
```

**Prediction**:
```cpp
const char *predict(const float dataPoint[]);
```

**Configuration**:
```cpp
void setDistanceMetric(DistanceMetric newMetric);
void setWeightedVoting(bool weighted);
void enableNormalization(bool enable);
void setLowMemoryMode(bool enable);
```

**Evaluation**:
```cpp
float evaluateAccuracy(const float **testFeatures, const char **testLabels, int testCount);
float crossValidate(int folds);
float getPredictionConfidence(const float dataPoint[]);
```

---

## 6. Usage Examples and Best Practices

### 6.1 Basic Sensor Classification

```cpp
#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

KNN sensorClassifier(5, 3, 100);

void setup() {
    Serial.begin(115200);
    
    // Configure
    sensorClassifier.setDistanceMetric(EUCLIDEAN);
    sensorClassifier.enableNormalization(true);
    sensorClassifier.setWeightedVoting(true);
    
    // Add training data (Temperature, Humidity, Light -> Activity)
    float office1[] = {22, 45, 800}; sensorClassifier.addTrainingData("WORK", office1);
    float office2[] = {23, 50, 750}; sensorClassifier.addTrainingData("WORK", office2);
    float sleep1[] = {19, 60, 10};   sensorClassifier.addTrainingData("SLEEP", sleep1);
    float sleep2[] = {18, 65, 5};    sensorClassifier.addTrainingData("SLEEP", sleep2);
    float active1[] = {25, 40, 600}; sensorClassifier.addTrainingData("ACTIVE", active1);
    // ... add more training data
    
    Serial.println("KNN Classifier ready!");
}

void loop() {
    // Read sensors
    float temp = readTemperature();
    float humidity = readHumidity();
    float light = readLightLevel();
    
    // Classify activity
    float sensors[] = {temp, humidity, light};
    const char* activity = sensorClassifier.predict(sensors);
    float confidence = sensorClassifier.getPredictionConfidence(sensors);
    
    Serial.println("Activity: " + String(activity) + 
                   " (Confidence: " + String(confidence*100) + "%)");
    
    delay(5000);
}
```

### 6.2 Real IoT Example: Smart Gesture Controller

**Multi-axis accelerometer gesture recognition**

```cpp
#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

KNN gestureKNN(7, 6, 200);  // K=7, 6 features (accel + gyro), max 200 samples

struct MotionData {
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
};

void setup() {
    Serial.begin(115200);
    
    // Configure for gesture recognition
    gestureKNN.setDistanceMetric(EUCLIDEAN);
    gestureKNN.enableNormalization(true);
    gestureKNN.setWeightedVoting(true);
    
    // Load pre-recorded gesture data
    loadGestureTrainingData();
    
    Serial.println("Gesture Recognition System Ready!");
    Serial.println("Supported gestures: SHAKE, TILT, TAP, SWIPE, ROTATE");
}

void loadGestureTrainingData() {
    // Shake gestures (high X acceleration, low Y/Z)
    float shake1[] = {2.5, 0.2, 9.8, 0.1, 0.0, 0.1}; gestureKNN.addTrainingData("SHAKE", shake1);
    float shake2[] = {2.8, 0.1, 9.9, 0.2, 0.1, 0.0}; gestureKNN.addTrainingData("SHAKE", shake2);
    float shake3[] = {2.3, 0.3, 9.7, 0.0, 0.1, 0.2}; gestureKNN.addTrainingData("SHAKE", shake3);
    
    // Tilt gestures (high Y acceleration)
    float tilt1[] = {0.1, 3.2, 9.1, 0.0, 0.5, 0.1}; gestureKNN.addTrainingData("TILT", tilt1);
    float tilt2[] = {0.2, 3.5, 9.0, 0.1, 0.6, 0.0}; gestureKNN.addTrainingData("TILT", tilt2);
    
    // Tap gestures (high Z acceleration)
    float tap1[] = {0.0, 0.1, 12.5, 0.0, 0.0, 0.0}; gestureKNN.addTrainingData("TAP", tap1);
    float tap2[] = {0.1, 0.0, 12.8, 0.1, 0.0, 0.1}; gestureKNN.addTrainingData("TAP", tap2);
    
    // Swipe gestures (high gyro values)
    float swipe1[] = {0.5, 0.2, 9.8, 2.1, 0.3, 0.1}; gestureKNN.addTrainingData("SWIPE", swipe1);
    float swipe2[] = {0.3, 0.4, 9.9, 2.3, 0.2, 0.2}; gestureKNN.addTrainingData("SWIPE", swipe2);
    
    // Rotate gestures (high Z gyro)
    float rotate1[] = {0.1, 0.1, 9.8, 0.2, 0.1, 1.8}; gestureKNN.addTrainingData("ROTATE", rotate1);
    float rotate2[] = {0.2, 0.0, 9.9, 0.1, 0.2, 2.1}; gestureKNN.addTrainingData("ROTATE", rotate2);
    
    // Add more training data for better accuracy...
}

void loop() {
    MotionData motion = readMotionSensors();
    
    // Create feature vector
    float features[] = {
        motion.accelX, motion.accelY, motion.accelZ,
        motion.gyroX, motion.gyroY, motion.gyroZ
    };
    
    // Classify gesture
    const char* gesture = gestureKNN.predict(features);
    float confidence = gestureKNN.getPredictionConfidence(features);
    
    // Only act on high-confidence predictions
    if (confidence > 0.6) {
        handleGesture(gesture, confidence);
    }
    
    delay(50);  // 20Hz sampling rate
}

void handleGesture(const char* gesture, float confidence) {
    Serial.println("Gesture: " + String(gesture) + 
                   " (Confidence: " + String(confidence*100, 1) + "%)");
    
    if (strcmp(gesture, "SHAKE") == 0) {
        // Turn on/off lights
        toggleLights();
        Serial.println("Action: Lights toggled");
    }
    else if (strcmp(gesture, "TILT") == 0) {
        // Adjust temperature
        adjustTemperature();
        Serial.println("Action: Temperature adjusted");
    }
    else if (strcmp(gesture, "TAP") == 0) {
        // Play/pause music
        toggleMusic();
        Serial.println("Action: Music toggled");
    }
    else if (strcmp(gesture, "SWIPE") == 0) {
        // Change channel/track
        nextTrack();
        Serial.println("Action: Next track");
    }
    else if (strcmp(gesture, "ROTATE") == 0) {
        // Volume control
        adjustVolume();
        Serial.println("Action: Volume adjusted");
    }
}

MotionData readMotionSensors() {
    // Read from MPU6050 or similar IMU
    MotionData data;
    data.accelX = readAccelX();
    data.accelY = readAccelY();
    data.accelZ = readAccelZ();
    data.gyroX = readGyroX();
    data.gyroY = readGyroY();
    data.gyroZ = readGyroZ();
    return data;
}
```

### 6.3 Advanced IoT Example: Multi-Sensor Anomaly Detection

**Industrial equipment monitoring with multiple sensors**

```cpp
#define ENABLE_MODULE_KNN
#include "Kinematrix.h"

// Multiple KNN classifiers for different aspects
KNN temperatureKNN(5, 3, 150);    // Temperature anomaly detection
KNN vibrationKNN(7, 4, 200);      // Vibration pattern analysis
KNN overallKNN(5, 7, 300);        // Overall system health

struct SensorReading {
    float temp1, temp2, temp3;           // Multiple temperature sensors
    float vibX, vibY, vibZ, vibMag;      // Vibration in 3 axes + magnitude
    unsigned long timestamp;
};

void setup() {
    Serial.begin(115200);
    
    setupTemperatureKNN();
    setupVibrationKNN();
    setupOverallKNN();
    
    Serial.println("Multi-Sensor Anomaly Detection System Online");
}

void setupTemperatureKNN() {
    temperatureKNN.setDistanceMetric(EUCLIDEAN);
    temperatureKNN.enableNormalization(true);
    temperatureKNN.setWeightedVoting(true);
    
    // Normal temperature patterns
    float normal1[] = {65, 68, 62}; temperatureKNN.addTrainingData("NORMAL", normal1);
    float normal2[] = {67, 70, 64}; temperatureKNN.addTrainingData("NORMAL", normal2);
    float normal3[] = {66, 69, 63}; temperatureKNN.addTrainingData("NORMAL", normal3);
    
    // Warning patterns (elevated but not critical)
    float warn1[] = {75, 78, 72}; temperatureKNN.addTrainingData("WARNING", warn1);
    float warn2[] = {77, 80, 74}; temperatureKNN.addTrainingData("WARNING", warn2);
    
    // Critical patterns (overheating)
    float crit1[] = {85, 88, 82}; temperatureKNN.addTrainingData("CRITICAL", crit1);
    float crit2[] = {87, 90, 84}; temperatureKNN.addTrainingData("CRITICAL", crit2);
}

void setupVibrationKNN() {
    vibrationKNN.setDistanceMetric(EUCLIDEAN);
    vibrationKNN.enableNormalization(true);
    
    // Normal vibration (smooth operation)
    float smooth1[] = {0.1, 0.1, 0.2, 0.24}; vibrationKNN.addTrainingData("SMOOTH", smooth1);
    float smooth2[] = {0.12, 0.11, 0.18, 0.26}; vibrationKNN.addTrainingData("SMOOTH", smooth2);
    
    // Rough operation (needs maintenance)
    float rough1[] = {0.3, 0.4, 0.5, 0.71}; vibrationKNN.addTrainingData("ROUGH", rough1);
    float rough2[] = {0.35, 0.42, 0.48, 0.73}; vibrationKNN.addTrainingData("ROUGH", rough2);
    
    // Severe vibration (immediate attention)
    float severe1[] = {0.8, 0.9, 1.1, 1.56}; vibrationKNN.addTrainingData("SEVERE", severe1);
    float severe2[] = {0.85, 0.95, 1.05, 1.61}; vibrationKNN.addTrainingData("SEVERE", severe2);
}

void setupOverallKNN() {
    overallKNN.setDistanceMetric(EUCLIDEAN);
    overallKNN.enableNormalization(true);
    overallKNN.setWeightedVoting(true);
    
    // Combined sensor patterns for overall health assessment
    // [temp1, temp2, temp3, vibX, vibY, vibZ, vibMag]
    float healthy1[] = {66, 69, 63, 0.1, 0.1, 0.2, 0.24}; 
    overallKNN.addTrainingData("HEALTHY", healthy1);
    
    float degraded1[] = {75, 78, 72, 0.3, 0.4, 0.5, 0.71}; 
    overallKNN.addTrainingData("DEGRADED", degraded1);
    
    float failing1[] = {85, 88, 82, 0.8, 0.9, 1.1, 1.56}; 
    overallKNN.addTrainingData("FAILING", failing1);
}

void loop() {
    SensorReading reading = readAllSensors();
    
    // Analyze different aspects
    analyzeSensorData(reading);
    
    delay(1000);  // Check every second
}

void analyzeSensorData(SensorReading reading) {
    // Temperature analysis
    float tempData[] = {reading.temp1, reading.temp2, reading.temp3};
    const char* tempStatus = temperatureKNN.predict(tempData);
    float tempConfidence = temperatureKNN.getPredictionConfidence(tempData);
    
    // Vibration analysis
    float vibData[] = {reading.vibX, reading.vibY, reading.vibZ, reading.vibMag};
    const char* vibStatus = vibrationKNN.predict(vibData);
    float vibConfidence = vibrationKNN.getPredictionConfidence(vibData);
    
    // Overall system health
    float overallData[] = {reading.temp1, reading.temp2, reading.temp3, 
                          reading.vibX, reading.vibY, reading.vibZ, reading.vibMag};
    const char* overallStatus = overallKNN.predict(overallData);
    float overallConfidence = overallKNN.getPredictionConfidence(overallData);
    
    // Log results
    logSensorAnalysis(reading, tempStatus, tempConfidence, 
                     vibStatus, vibConfidence, overallStatus, overallConfidence);
    
    // Take action based on results
    if (strcmp(overallStatus, "FAILING") == 0 && overallConfidence > 0.8) {
        triggerEmergencyShutdown();
        sendAlert("CRITICAL: Equipment failure detected!");
    }
    else if (strcmp(tempStatus, "CRITICAL") == 0 || strcmp(vibStatus, "SEVERE") == 0) {
        triggerWarningSignal();
        sendAlert("WARNING: Abnormal sensor readings detected");
    }
}

void logSensorAnalysis(SensorReading reading, 
                      const char* tempStatus, float tempConf,
                      const char* vibStatus, float vibConf,
                      const char* overallStatus, float overallConf) {
    
    Serial.println("=== Sensor Analysis ===");
    Serial.println("Timestamp: " + String(reading.timestamp));
    Serial.println("Temperatures: " + String(reading.temp1) + "°C, " + 
                   String(reading.temp2) + "°C, " + String(reading.temp3) + "°C");
    Serial.println("Temperature Status: " + String(tempStatus) + 
                   " (Confidence: " + String(tempConf*100, 1) + "%)");
    
    Serial.println("Vibration: X=" + String(reading.vibX) + ", Y=" + String(reading.vibY) + 
                   ", Z=" + String(reading.vibZ) + ", Mag=" + String(reading.vibMag));
    Serial.println("Vibration Status: " + String(vibStatus) + 
                   " (Confidence: " + String(vibConf*100, 1) + "%)");
    
    Serial.println("Overall Health: " + String(overallStatus) + 
                   " (Confidence: " + String(overallConf*100, 1) + "%)");
    Serial.println("========================");
}

SensorReading readAllSensors() {
    SensorReading reading;
    reading.timestamp = millis();
    
    // Read temperature sensors (thermocouples, DS18B20, etc.)
    reading.temp1 = readTemperatureSensor1();
    reading.temp2 = readTemperatureSensor2();
    reading.temp3 = readTemperatureSensor3();
    
    // Read vibration sensor (accelerometer)
    reading.vibX = readVibrationX();
    reading.vibY = readVibrationY();
    reading.vibZ = readVibrationZ();
    reading.vibMag = sqrt(reading.vibX*reading.vibX + 
                         reading.vibY*reading.vibY + 
                         reading.vibZ*reading.vibZ);
    
    return reading;
}
```

### 6.4 Cross Validation and Model Evaluation

```cpp
void evaluateModel() {
    Serial.println("=== Model Evaluation ===");
    
    // Cross validation
    float cvAccuracy = gestureKNN.crossValidate(5);
    Serial.println("5-Fold CV Accuracy: " + String(cvAccuracy*100, 2) + "%");
    
    // Test different K values
    Serial.println("Testing different K values:");
    for (int k = 1; k <= 15; k += 2) {
        KNN testKNN(k, 6, 200);
        // Add same training data...
        
        float accuracy = testKNN.crossValidate(3);
        Serial.println("K=" + String(k) + ": " + String(accuracy*100, 1) + "%");
    }
    
    // Test different distance metrics
    Serial.println("Testing different distance metrics:");
    const char* metrics[] = {"EUCLIDEAN", "MANHATTAN", "COSINE"};
    DistanceMetric metricEnums[] = {EUCLIDEAN, MANHATTAN, COSINE};
    
    for (int i = 0; i < 3; i++) {
        KNN testKNN(7, 6, 200);
        testKNN.setDistanceMetric(metricEnums[i]);
        // Add training data...
        
        float accuracy = testKNN.crossValidate(3);
        Serial.println(String(metrics[i]) + ": " + String(accuracy*100, 1) + "%");
    }
}
```

**Key Features of these IoT Implementations:**
- **Multiple KNN classifiers**: Specialized for different data types
- **Confidence-based decisions**: Only act on high-confidence predictions
- **Real-time monitoring**: Continuous sensor analysis
- **Adaptive thresholds**: Different confidence levels for different actions
- **Comprehensive logging**: Track all predictions and decisions
- **Fail-safe mechanisms**: Emergency shutdowns and alerts

---

## 7. Performance Analysis

### 7.1 Time Complexity

- **Training**: O(1) per sample (just store data)
- **Prediction**: O(n × m) where n = training samples, m = features
- **Memory**: O(n × m) for storing training data

### 7.2 Space Complexity

- **Training Data**: O(n × m)
- **Distance Buffer**: O(k) in low memory mode, O(n) in full mode
- **Total Memory**: Depends on maxData dan maxFeatures

### 7.3 Platform Performance

| Platform | Max Data | Max Features | Typical Prediction Time |
|----------|----------|--------------|------------------------|
| ESP32    | 1000+    | 10+         | 1-10ms                 |
| ESP8266  | 500      | 8           | 2-20ms                 |
| Arduino  | 100      | 4           | 5-50ms                 |

---

## 8. Troubleshooting and Optimization

### 8.1 Common Issues

**Low Accuracy**:
- Check data quality and balance
- Try different K values
- Enable normalization
- Consider different distance metrics

**Slow Prediction**:
- Enable low memory mode
- Reduce training data size
- Use Manhattan distance (faster than Euclidean)

**Memory Issues**:
- Reduce maxData or maxFeatures
- Enable low memory mode
- Remove old training data periodically

### 8.2 Optimization Strategies

**Memory Optimization**:
```cpp
knn.setLowMemoryMode(true);      // Use fixed-size buffers
knn.enableNormalization(false);  // Skip normalization if not needed
```

**Speed Optimization**:
```cpp
knn.setDistanceMetric(MANHATTAN); // Faster than Euclidean
// Or use smaller K value
```

---

## 9. References

### Academic References

1. **Fix, E., & Hodges, J. L. (1951)**. "Discriminatory Analysis. Nonparametric Discrimination: Consistency Properties". *Technical Report 4*, USAF School of Aviation Medicine.

2. **Cover, T., & Hart, P. (1967)**. "Nearest neighbor pattern classification". *IEEE Transactions on Information Theory*, 13(1), 21-27.

3. **Hastie, T., Tibshirani, R., & Friedman, J. (2009)**. "The Elements of Statistical Learning: Data Mining, Inference, and Prediction". *Springer-Verlag*, 2nd Edition.

4. **Duda, R. O., Hart, P. E., & Stork, D. G. (2000)**. "Pattern Classification". *John Wiley & Sons*, 2nd Edition.

### Technical References

5. **Aha, D. W., Kibler, D., & Albert, M. K. (1991)**. "Instance-based learning algorithms". *Machine Learning*, 6(1), 37-66.

6. **Wilson, D. L. (1972)**. "Asymptotic properties of nearest neighbor rules using edited data". *IEEE Transactions on Systems, Man, and Cybernetics*, 2(3), 408-421.

### Implementation References

7. **Pedregosa, F., et al. (2011)**. "Scikit-learn: Machine Learning in Python". *Journal of Machine Learning Research*, 12, 2825-2830.

8. **Hand, D. J., Mannila, H., & Smyth, P. (2001)**. "Principles of Data Mining". *MIT Press*.

### Embedded Systems References

9. **Kumar, A., & Gopal, M. (2009)**. "Least squares SVM for embedded systems". *Expert Systems with Applications*, 36(4), 8129-8136.

10. **Zhang, C., & Ma, Y. (2012)**. "Ensemble Machine Learning: Methods and Applications". *Springer*.

---

## Appendix: Terminology Glossary

### KNN-Specific Terms

- **K**: Number of nearest neighbors to consider for classification
- **Query Point**: New data point to be classified
- **Training Instance**: Stored data point with known label
- **Distance Metric**: Method to measure similarity between data points
- **Lazy Learning**: No explicit training phase, computation at prediction time
- **Instance-Based**: Decisions based on stored instances rather than abstract model

### Distance Metrics

- **Euclidean Distance**: Straight-line distance in n-dimensional space
- **Manhattan Distance**: Sum of absolute differences along each dimension
- **Cosine Distance**: Angle-based similarity measure
- **Minkowski Distance**: Generalized distance metric (includes Euclidean and Manhattan)

### Voting Methods

- **Simple Majority**: Each neighbor gets equal vote
- **Weighted Voting**: Neighbors weighted by inverse distance
- **Distance-Weighted**: Closer neighbors have more influence
- **Confidence Score**: Measure of prediction certainty

### Performance Terms

- **Curse of Dimensionality**: Performance degrades in high-dimensional spaces
- **Bias-Variance Tradeoff**: Balance between model complexity and generalization
- **Cross Validation**: Method to evaluate model performance
- **Overfitting**: Model too complex, poor generalization
- **Underfitting**: Model too simple, misses patterns

---

*Documentation Version: 1.0*  
*Last Updated: 2025*  
*Implementation: Kinematrix KNN Library*