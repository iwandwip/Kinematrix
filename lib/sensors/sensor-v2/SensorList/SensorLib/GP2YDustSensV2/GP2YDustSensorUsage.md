# GP2Y Dust Sensor Library Documentation

## Table of Contents

1. [Overview](#overview)
2. [Sensor Specifications](#sensor-specifications)
3. [Hardware Setup](#hardware-setup)
4. [Library Installation](#library-installation)
5. [Basic Usage](#basic-usage)
6. [Blocking vs Non-Blocking Modes](#blocking-vs-non-blocking-modes)
7. [Platform-Specific Configuration](#platform-specific-configuration)
8. [Calculation Formulas](#calculation-formulas)
9. [API Reference](#api-reference)
10. [Calibration Guide](#calibration-guide)
11. [Troubleshooting](#troubleshooting)
12. [Example Projects](#example-projects)

## Overview

The GP2Y Dust Sensor Library provides a flexible interface for Sharp GP2Y1010AU0F and GP2Y1014AU0F optical dust sensors.
It supports both blocking and non-blocking operation modes, making it suitable for various applications from simple
Arduino projects to complex IoT systems.

### Key Features

- **Dual operation modes**: Blocking (legacy) and Non-blocking (recommended)
- **Multi-platform support**: Arduino, ESP32, ESP8266, STM32, and more
- **Automatic baseline calibration**: Compensates for sensor drift
- **Configurable parameters**: vRef, ADC resolution, voltage divider ratio
- **Built-in averaging**: Running average buffer for stable readings
- **Zero dependencies**: Only requires Arduino framework

### Supported Sensors

| Model        | Baseline Voltage | Max Output | Sensitivity   |
|--------------|------------------|------------|---------------|
| GP2Y1010AU0F | 0.9V (0-1.5V)    | 3.4V       | 0.5V/100μg/m³ |
| GP2Y1014AU0F | 0.6V (0.1-1.1V)  | 3.4V       | 0.5V/100μg/m³ |

## Sensor Specifications

### Operating Conditions

- **Supply Voltage**: 5V ± 0.5V (4.5V - 5.5V)
- **Operating Temperature**: -10°C to +65°C
- **Current Consumption**: Max 20mA
- **Detection Range**: 0 - 600 μg/m³

### Timing Requirements

```
LED ON → Wait 280μs → Read ADC → LED OFF → Wait 9.62ms
|←-------------------- 10ms cycle -------------------→|
```

### Pin Configuration

| Pin | Name    | Function                 | Connection     |
|-----|---------|--------------------------|----------------|
| 1   | V-LED   | LED Supply Voltage       | 5V             |
| 2   | LED-GND | LED Ground               | GND            |
| 3   | LED     | LED Control (Active Low) | Digital Output |
| 4   | S-GND   | Sensor Ground            | GND            |
| 5   | Vo      | Analog Output            | Analog Input   |
| 6   | VCC     | Sensor Supply            | 5V             |

## Hardware Setup

### Direct Connection (5V Microcontrollers)

For Arduino Uno, Nano, Mega, and other 5V boards:

```
Arduino 5V  ────────┬──→ Sensor Pin 1 (V-LED)
                    └──→ Sensor Pin 6 (VCC)

Arduino GND ────────┬──→ Sensor Pin 2 (LED-GND)
                    └──→ Sensor Pin 4 (S-GND)

Arduino D7  ────────────→ Sensor Pin 3 (LED)
Arduino A0  ────────────→ Sensor Pin 5 (Vo)
```

### Voltage Divider Connection (3.3V Microcontrollers)

For ESP32, ESP8266, STM32, and other 3.3V boards:

```
ESP32 5V    ────────┬──→ Sensor Pin 1 (V-LED)
                    └──→ Sensor Pin 6 (VCC)

ESP32 GND   ────────┬──→ Sensor Pin 2 (LED-GND)
                    └──→ Sensor Pin 4 (S-GND)

ESP32 GPIO14 ───────────→ Sensor Pin 3 (LED)

Sensor Pin 5 (Vo) ──[R1]──┬──→ ESP32 GPIO34
                          │
                         [R2]
                          │
                         GND
```

### Voltage Divider Calculations

The sensor can output up to 3.4V, which exceeds the 3.3V ADC limit of ESP32/ESP8266. A voltage divider is required:

**For 1:1 ratio (R1 = R2 = 10kΩ):**

```
Vout = Vin × R2/(R1+R2) = Vin × 0.5
Max output: 3.4V × 0.5 = 1.7V (Safe for 3.3V ADC)
```

**Common Configurations:**

| Platform | R1    | R2    | Ratio | Max Input | Safe Range |
|----------|-------|-------|-------|-----------|------------|
| ESP32    | 10kΩ  | 10kΩ  | 0.5   | 6.6V      | ✓          |
| ESP32    | 10kΩ  | 15kΩ  | 0.6   | 5.5V      | ✓          |
| ESP8266  | 220kΩ | 100kΩ | 0.31  | 3.2V*     | ✓          |

*ESP8266 ADC max input is 1V

## Library Installation

### Arduino IDE

1. Download the library ZIP file
2. Open Arduino IDE
3. Go to Sketch → Include Library → Add .ZIP Library
4. Select the downloaded file

### PlatformIO

Add to `platformio.ini`:

```ini
lib_deps = 
    GP2YDustSensor
```

### Manual Installation

Copy the library folder to your Arduino libraries directory:

- Windows: `Documents\Arduino\libraries\`
- macOS: `~/Documents/Arduino/libraries/`
- Linux: `~/Arduino/libraries/`

## Basic Usage

### Minimal Example

```cpp
#include <GP2YDustSensor.h>

GP2YDustSensor dustSensor(GP2Y1010AU0F, 7, A0);

void setup() {
    Serial.begin(9600);
    dustSensor.begin();
}

void loop() {
    uint16_t dust = dustSensor.getDustDensity();
    Serial.print("Dust Density: ");
    Serial.print(dust);
    Serial.println(" μg/m³");
    delay(1000);
}
```

## Blocking vs Non-Blocking Modes

### Blocking Mode (Default)

In blocking mode, `getDustDensity()` blocks the CPU for the entire sampling duration:

```cpp
// This will block for 200ms (20 samples × 10ms)
uint16_t dust = dustSensor.getDustDensity();
```

**Timing Analysis:**

- 1 sample: 10ms blocking
- 20 samples (default): 200ms blocking
- 50 samples: 500ms blocking

**Pros:**

- Simple to use
- Accurate timing
- Backward compatible

**Cons:**

- Blocks all other code execution
- Can cause WiFi disconnections
- May trigger watchdog resets
- Not suitable for real-time applications

### Non-Blocking Mode (Recommended)

Non-blocking mode uses a state machine to process readings without blocking:

```cpp
void setup() {
    dustSensor.begin();
    dustSensor.setBlockingMode(false);
    dustSensor.setTargetSamples(20);
}

void loop() {
    dustSensor.update();  // Must be called frequently
    
    if (dustSensor.available()) {
        uint16_t dust = dustSensor.getDustDensity();
        Serial.println(dust);
    }
    
    // Other code runs normally
    handleWiFi();
    updateDisplay();
}
```

**State Machine Operation:**

```
IDLE → LED_ON → WAIT_280μs → READ_ADC → LED_OFF → WAIT_9.62ms → IDLE
```

**Pros:**

- Zero CPU blocking
- Maintains WiFi/network connections
- Allows multitasking
- Prevents watchdog resets

**Cons:**

- Slightly more complex setup
- Requires frequent `update()` calls

## Platform-Specific Configuration

### Arduino Uno/Nano (ATmega328P)

**Specifications:**

- vRef: 5V
- ADC Resolution: 10-bit (0-1023)
- Direct connection (no voltage divider needed)

```cpp
GP2YDustSensor dustSensor(
    GP2Y1010AU0F,  // Sensor type
    7,             // LED pin
    A0             // Analog pin
    // Default values: 60, 5.0, 1024.0, 1.0
);
```

### ESP32

**Specifications:**

- vRef: 3.3V
- ADC Resolution: 12-bit (0-4095)
- Requires voltage divider

```cpp
GP2YDustSensor dustSensor(
    GP2Y1010AU0F,  // Sensor type
    14,            // LED pin
    34,            // Analog pin (use ADC1 channels)
    60,            // Running average samples
    3.3,           // vRef
    4096.0,        // ADC resolution
    0.5            // Voltage divider ratio
);

void setup() {
    analogReadResolution(12);      // Set ADC to 12-bit
    analogSetAttenuation(ADC_11db); // 0-3.3V range
    dustSensor.begin();
}
```

### ESP8266

**Specifications:**

- vRef: 1.0V (internal)
- ADC Resolution: 10-bit (0-1023)
- Requires voltage divider for 0-3.3V range

```cpp
GP2YDustSensor dustSensor(
    GP2Y1010AU0F,  // Sensor type
    D1,            // LED pin
    A0,            // Analog pin (only one available)
    60,            // Running average samples
    3.3,           // vRef (with external divider)
    1024.0,        // ADC resolution
    0.31           // Voltage divider ratio (100k:220k)
);
```

### STM32

**Specifications:**

- vRef: 3.3V (most variants)
- ADC Resolution: 12-bit (0-4095)
- Requires voltage divider

```cpp
GP2YDustSensor dustSensor(
    GP2Y1010AU0F,  // Sensor type
    PA0,           // LED pin
    PA1,           // Analog pin
    60,            // Running average samples
    3.3,           // vRef
    4096.0,        // ADC resolution
    0.5            // Voltage divider ratio
);
```

## Calculation Formulas

### 1. ADC to Voltage Conversion

```
Measured Voltage = ADC_Value × (vRef / ADC_Resolution)
Actual Sensor Voltage = Measured Voltage / Voltage_Divider_Ratio
Final Voltage = Actual Sensor Voltage × Calibration_Factor
```

**Example (ESP32 with 1:1 divider):**

```
ADC Value: 2048
Measured: 2048 × (3.3 / 4096) = 1.65V
Actual: 1.65V / 0.5 = 3.3V
Final: 3.3V × 1.0 = 3.3V
```

### 2. Voltage to Dust Density Conversion

```
If (Voltage < Baseline):
    Dust Density = 0
Else:
    Dust Density = (Voltage - Baseline) / Sensitivity × 100

Where:
- Baseline = Zero dust voltage (typ. 0.9V)
- Sensitivity = 0.5V per 100μg/m³
```

**Example:**

```
Voltage: 1.4V
Baseline: 0.9V
Density = (1.4 - 0.9) / 0.5 × 100 = 100 μg/m³
```

### 3. Running Average

```
Buffer: [D₁, D₂, D₃, ..., Dₙ]
Average = Σ(Dᵢ) / n
```

## API Reference

### Constructor

```cpp
GP2YDustSensor(
    GP2YDustSensorType type,        // GP2Y1010AU0F or GP2Y1014AU0F
    uint8_t ledOutputPin,           // Digital pin for LED control
    uint8_t analogReadPin,          // Analog input pin
    uint16_t runningAverageCount,   // Size of averaging buffer (default: 60)
    float vRef,                     // ADC reference voltage (default: 5.0)
    float adcResolution,            // ADC maximum value (default: 1024.0)
    float voltageDividerRatio       // Voltage divider ratio (default: 1.0)
);
```

### Methods

#### Core Functions

| Method                    | Description                       | Returns          |
|---------------------------|-----------------------------------|------------------|
| `begin()`                 | Initialize the sensor             | void             |
| `getDustDensity()`        | Get current dust density          | uint16_t (μg/m³) |
| `getDustDensity(samples)` | Get density with specific samples | uint16_t (μg/m³) |
| `getRunningAverage()`     | Get averaged dust density         | uint16_t (μg/m³) |

#### Non-Blocking Mode

| Method                  | Description                  | Returns |
|-------------------------|------------------------------|---------|
| `setBlockingMode(bool)` | Enable/disable blocking mode | void    |
| `update()`              | Update state machine         | void    |
| `available()`           | Check if new data ready      | bool    |
| `setTargetSamples(n)`   | Set samples for non-blocking | void    |

#### Calibration

| Method                    | Description                | Returns            |
|---------------------------|----------------------------|--------------------|
| `setBaseline(voltage)`    | Set zero dust voltage      | void               |
| `getBaseline()`           | Get current baseline       | float (V)          |
| `getBaselineCandidate()`  | Get auto-detected baseline | float (V)          |
| `setSensitivity(v)`       | Set sensor sensitivity     | void               |
| `getSensitivity()`        | Get current sensitivity    | float (V/100μg/m³) |
| `setCalibrationFactor(f)` | Set calibration multiplier | void               |

## Calibration Guide

### Initial Calibration

1. **Clean Air Environment**: Ensure sensor is in clean air
2. **Warm-up Period**: Wait 30-60 seconds for stabilization
3. **Baseline Detection**: Run calibration routine

```cpp
void calibrateSensor() {
    Serial.println("Calibrating... Ensure clean air!");
    delay(30000);  // 30 second warm-up
    
    float minVoltage = 5.0;
    for (int i = 0; i < 100; i++) {
        dustSensor.getDustDensity(1);
        float baseline = dustSensor.getBaseline();
        if (baseline < minVoltage) {
            minVoltage = baseline;
        }
        delay(100);
    }
    
    dustSensor.setBaseline(minVoltage);
    Serial.print("Calibration complete. Baseline: ");
    Serial.println(minVoltage);
}
```

### Sensitivity Adjustment

Compare with reference instrument:

```cpp
float referenceDensity = 100.0;  // From calibrated device
float sensorDensity = dustSensor.getDustDensity();
float newSensitivity = sensitivity * (sensorDensity / referenceDensity);
dustSensor.setSensitivity(newSensitivity);
```

### Automatic Drift Compensation

```cpp
void loop() {
    static unsigned long lastCalibration = 0;
    
    // Auto-calibrate every hour
    if (millis() - lastCalibration > 3600000) {
        float candidate = dustSensor.getBaselineCandidate();
        if (candidate < dustSensor.getBaseline()) {
            dustSensor.setBaseline(candidate);
        }
        lastCalibration = millis();
    }
}
```

## Troubleshooting

### Common Issues and Solutions

| Problem                | Possible Causes                                                       | Solutions                                                                               |
|------------------------|-----------------------------------------------------------------------|-----------------------------------------------------------------------------------------|
| Always reads 0         | - Incorrect wiring<br>- Wrong voltage divider<br>- Sensor not powered | - Check 5V supply to sensor<br>- Verify voltage divider ratio<br>- Test LED control pin |
| Very high readings     | - Baseline too low<br>- Sensor contaminated<br>- Light interference   | - Recalibrate baseline<br>- Clean sensor lens<br>- Shield from external light           |
| Unstable readings      | - Insufficient samples<br>- Vibration<br>- Power supply noise         | - Increase sample count<br>- Mount securely<br>- Add capacitor to power                 |
| WiFi disconnects (ESP) | - Using blocking mode<br>- Sample count too high                      | - Switch to non-blocking mode<br>- Reduce samples to <10                                |

### Diagnostic Code

```cpp
void runDiagnostics() {
    Serial.println("=== Sensor Diagnostics ===");
    
    // Test LED control
    digitalWrite(LED_PIN, LOW);
    delay(10);
    uint16_t ledOn = analogRead(ANALOG_PIN);
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    uint16_t ledOff = analogRead(ANALOG_PIN);
    
    Serial.print("LED ON: ");
    Serial.print(ledOn);
    Serial.print(" | LED OFF: ");
    Serial.println(ledOff);
    
    if (ledOn > ledOff + 50) {
        Serial.println("✓ LED control working");
    } else {
        Serial.println("✗ LED control failed");
    }
    
    // Check baseline
    float baseline = dustSensor.getBaseline();
    Serial.print("Baseline: ");
    Serial.print(baseline);
    Serial.println("V");
    
    if (baseline >= 0.5 && baseline <= 1.5) {
        Serial.println("✓ Baseline in range");
    } else {
        Serial.println("✗ Baseline out of range");
    }
}
```

## Example Projects

### 1. Simple Air Quality Monitor

```cpp
#include <GP2YDustSensor.h>

GP2YDustSensor dustSensor(GP2Y1010AU0F, 7, A0);

void setup() {
    Serial.begin(9600);
    dustSensor.begin();
}

void loop() {
    uint16_t dust = dustSensor.getDustDensity();
    String quality;
    
    if (dust <= 50) quality = "Good";
    else if (dust <= 100) quality = "Moderate";
    else if (dust <= 150) quality = "Unhealthy for Sensitive";
    else if (dust <= 200) quality = "Unhealthy";
    else if (dust <= 300) quality = "Very Unhealthy";
    else quality = "Hazardous";
    
    Serial.print("Dust: ");
    Serial.print(dust);
    Serial.print(" μg/m³ - ");
    Serial.println(quality);
    
    delay(1000);
}
```

### 2. IoT Air Quality Station (ESP32)

```cpp
#include <WiFi.h>
#include <GP2YDustSensor.h>

const char* ssid = "your-ssid";
const char* password = "your-password";

GP2YDustSensor dustSensor(
    GP2Y1010AU0F, 14, 34, 60,
    3.3, 4096.0, 0.5
);

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    dustSensor.begin();
    dustSensor.setBlockingMode(false);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
}

void loop() {
    dustSensor.update();
    
    if (dustSensor.available()) {
        uint16_t dust = dustSensor.getDustDensity();
        sendToCloud(dust);
    }
    
    // WiFi stays connected!
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
    }
}

void sendToCloud(uint16_t value) {
    // HTTP POST or MQTT publish
}
```

### 3. Multi-Sensor Environmental Monitor

```cpp
#include <GP2YDustSensor.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

GP2YDustSensor dustSensor(GP2Y1010AU0F, 7, A0);
DHT dht(2, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

struct {
    uint16_t dust;
    float temperature;
    float humidity;
} data;

void setup() {
    dustSensor.begin();
    dustSensor.setBlockingMode(false);
    dht.begin();
    lcd.init();
    lcd.backlight();
}

void loop() {
    static unsigned long lastDHT = 0;
    static unsigned long lastLCD = 0;
    
    dustSensor.update();
    
    if (dustSensor.available()) {
        data.dust = dustSensor.getDustDensity();
    }
    
    if (millis() - lastDHT >= 2000) {
        lastDHT = millis();
        data.temperature = dht.readTemperature();
        data.humidity = dht.readHumidity();
    }
    
    if (millis() - lastLCD >= 500) {
        lastLCD = millis();
        updateDisplay();
    }
}

void updateDisplay() {
    lcd.setCursor(0, 0);
    lcd.print("Dust: ");
    lcd.print(data.dust);
    lcd.print(" ug/m3  ");
    
    lcd.setCursor(0, 1);
    lcd.print(data.temperature, 1);
    lcd.print("C ");
    lcd.print(data.humidity, 0);
    lcd.print("%  ");
}
```

## Performance Metrics

### Memory Usage

- **Flash**: ~4KB (varies by platform)
- **RAM**: 250 bytes + (2 × runningAverageCount)
- **Stack**: <100 bytes

### Timing Performance

| Operation      | Blocking Mode | Non-Blocking Mode |
|----------------|---------------|-------------------|
| Single reading | 10-200ms      | <100μs per update |
| CPU usage      | 100%          | <1%               |
| Loop frequency | 5-100 Hz      | >1000 Hz          |

### Accuracy

| Sample Count | Typical Error | Response Time |
|--------------|---------------|---------------|
| 1            | ±30%          | 10ms          |
| 5            | ±15%          | 50ms          |
| 20           | ±5%           | 200ms         |
| 50           | ±2%           | 500ms         |

## References

- [Sharp GP2Y1010AU0F Datasheet](https://www.sharp-world.com/)
- [Arduino ADC Reference](https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/)
- [ESP32 ADC Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)

## License

This library is released under the MIT License. See LICENSE file for details.

## Contributing

Contributions are welcome! Please submit pull requests or open issues on GitHub.

---

*Last updated: 2024*