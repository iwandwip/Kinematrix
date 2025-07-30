# AutoLight V3 - Revolutionary Dynamic PCF Distribution & Multi-Modal Button System Documentation

## Overview

AutoLight V3 merupakan breakthrough dalam LED control system yang memperkenalkan **Dynamic PCF Distribution**, **Multi-Modal Button System**, dan **Intelligent Hardware Adaptation**. Dikembangkan pada 2023, V3 revolutionizes cara PCF8574 I/O expanders didistribusikan dan dikelola, sambil memperkenalkan 5 mode button interaction yang berbeda.

## Complete Directory Structure

```
AutoLightv3/ (Revolutionary Dynamic PCF Distribution)
├── AutoLightv3.h                   # Main library entry point with dynamic features
├── index.h                         # Component orchestration and includes
├── App.h/.cpp                      # Application framework (enhanced placeholder)
├── ButtonInterrupt.h/.cpp          # Multi-modal interrupt-driven button system
├── Channel/                        # Revolutionary LED Channel Management
│   ├── BaseChannel.h/.cpp          # Core channel with 4-button support & smart control
│   ├── BaseChannelSequence.cpp     # 14 predefined LED sequence patterns (modes 2-15)
│   └── Indicator.h/.cpp            # Visual/audio feedback system (RGB LED + buzzer)
├── Config/                         # Dynamic Configuration Management
│   ├── ConfigData.h                # Revolutionary data structures with PCF distribution
│   └── BaseConfig.h/.cpp           # Dynamic PCF distribution and configuration logic
├── Cores/                          # Core System Services
│   ├── FileSystem.h/.cpp           # LittleFS file system management
│   └── Task.h/.cpp                 # Enhanced FreeRTOS task management
├── Log/                           # Logging System
│   └── LogSerial.h/.cpp            # Serial debug logging (minimal implementation)
└── CLAUDE.md                       # V3 comprehensive documentation
```

## Revolutionary Architecture Features

### 1. Dynamic PCF Distribution System

V3 의 breakthrough feature는 **intelligent PCF8574 I/O expander distribution system**:

```cpp
namespace AutoLight {
    typedef enum {
        DISTRIBUTE_BALANCED = 0,    // Even distribution across PCFs
        DISTRIBUTE_SEQUENTIAL = 1,  // Fill PCFs sequentially (8 channels each)
        DISTRIBUTE_OPTIMIZED = 2,   // Minimize PCF usage (fill completely first)
        DISTRIBUTE_CUSTOM = 3       // Manual configuration with custom addresses
    } distribution_strategy_t;

    struct PCFDistribution {
        uint8_t address;         // I2C address (0x20-0x27 typical)
        uint8_t used_pins;       // Number of pins allocated (1-8)
        uint8_t start_channel;   // Starting channel number for this PCF
        uint8_t pin_offset;      // Pin offset within PCF (0-7)
        bool is_active;          // PCF activation status
    };

    struct DynamicConfigData {
        PCFDistribution pcf_list[MAXNUM_IO_EXPANDER];    // Up to 8 PCFs
        uint8_t pcf_count;                               // Active PCF count
        uint8_t total_channels;                          // Total managed channels
        distribution_strategy_t strategy;                // Distribution strategy
    };
}
```

#### Distribution Strategy Examples:

**1. DISTRIBUTE_BALANCED** - Even distribution:
```
24 channels, 3 PCFs:
PCF 0x20: Channels 0-7   (8 channels)
PCF 0x21: Channels 8-15  (8 channels) 
PCF 0x22: Channels 16-23 (8 channels)
```

**2. DISTRIBUTE_SEQUENTIAL** - Fill sequentially:
```
20 channels, 3 PCFs:
PCF 0x20: Channels 0-7   (8 channels, full)
PCF 0x21: Channels 8-15  (8 channels, full)
PCF 0x22: Channels 16-19 (4 channels, partial)
```

**3. DISTRIBUTE_OPTIMIZED** - Minimize PCF usage:
```
20 channels, 3 PCFs available:
PCF 0x20: Channels 0-7   (8 channels, full)
PCF 0x21: Channels 8-15  (8 channels, full)
PCF 0x22: Channels 16-19 (4 channels, partial)
PCF 0x23: Not used       (0 channels, saves power)
```

**4. DISTRIBUTE_CUSTOM** - Manual configuration:
```cpp
// Custom addressing for specific hardware
PCFDistribution custom_config[] = {
    {0x27, 8, 0,  0, true},   // Primary controller
    {0x45, 6, 8,  0, true},   // Secondary controller (6 pins only)
    {0x67, 4, 14, 2, true},   // Tertiary controller (pins 2-5 only)
};
```

### 2. Multi-Modal Button System

V3 memperkenalkan **5 button interaction paradigms**:

```cpp
namespace AutoLight {
    typedef enum {
        BUTTON_MODE_4BUTTON = 0,  // ON/OFF/NEXT/PREV (traditional full control)
        BUTTON_MODE_1BUTTON = 1,  // Single button: OFF → ON → Mode2→15 → OFF
        BUTTON_MODE_2BUTTON = 2,  // Toggle ON/OFF + Next Mode
        BUTTON_MODE_3BUTTON = 3,  // Separate ON/OFF/Next buttons
        BUTTON_MODE_CUSTOM = 99   // User-defined custom handlers
    } button_mode_t;

    struct ButtonConfig {
        uint8_t button_count;                // Physical button count (1-4)
        button_mode_t mode;                  // Current button interaction mode
        bool auto_cycle_enabled;             // Auto-cycling capability
        uint32_t auto_cycle_timeout;         // Timeout for auto-cycling (ms)
        void (*custom_handlers[4])();        // Custom button handler functions
    };
}
```

#### Button Mode Behaviors:

**1. BUTTON_MODE_4BUTTON** (Traditional):
```
Button 0: nextMode()     - Next sequence (1→2→3→...→15→1)
Button 1: previousMode() - Previous sequence (15→14→13→...→1→15)  
Button 2: onMode()       - Force ON (last active sequence)
Button 3: offMode()      - Force OFF (mode 0)
```

**2. BUTTON_MODE_1BUTTON** (Single Button Cycle):
```
Press 1: OFF → ON (last active sequence)
Press 2: ON → Mode 2
Press 3: Mode 2 → Mode 3
...
Press N: Mode 15 → OFF
```

**3. BUTTON_MODE_2BUTTON** (Toggle + Next):
```
Button 0: toggleOnOff()  - Toggle between OFF and last active mode
Button 1: nextMode()     - Next sequence (only when ON)
```

**4. BUTTON_MODE_3BUTTON** (Separate Controls):
```
Button 0: onMode()       - Turn ON with last active sequence
Button 1: offMode()      - Turn OFF
Button 2: nextMode()     - Next sequence (only when ON)
```

**5. BUTTON_MODE_CUSTOM** (User-Defined):
```cpp
ButtonConfig config;
config.mode = BUTTON_MODE_CUSTOM;
config.custom_handlers[0] = myCustomHandler1;
config.custom_handlers[1] = myCustomHandler2;
// ... user-defined behavior
```

### 3. Enhanced Channel Management

V3 memperkenalkan **advanced channel state management**:

```cpp
namespace AutoLight {
    class ChannelData {
    public:
        volatile uint32_t delay_time_;
        volatile uint32_t sequence_index_;
        volatile uint32_t sequence_index_apps_;
        volatile uint32_t last_active_sequence_;     // NEW: Store last active mode
        volatile bool is_reverse_;
        volatile bool is_mode_changed_;
        volatile bool is_mode_changed_apps_;
        volatile bool is_on_;                        // NEW: Track system on/off state
    };
}
```

#### Smart Button Implementation:

```cpp
class BaseChannel {
public:
    // Multi-modal button functions
    void nextMode();                              // Next sequence
    void previousMode();                          // Previous sequence  
    void onMode();                               // Turn ON with last active
    void offMode();                              // Turn OFF
    
    void singleButtonCycle();                    // Single button cycling
    void toggleOnOff();                          // Toggle ON/OFF
    void smartButtonPress(uint8_t button_index); // Intelligent button handling
    void setButtonMode(button_mode_t mode);      // Runtime mode switching
    
    volatile bool isOn();                        // Check if system is ON
    
private:
    ButtonConfig button_config_;                 // Button configuration
    button_mode_t current_button_mode_;          // Current button mode
};
```

### 4. Advanced Sequence System

V3 maintains **16 total lighting modes** dengan enhanced state management:

- **Mode 0**: Complete OFF state
- **Mode 1**: Static ON (all channels active)
- **Modes 2-15**: 14 customizable LED sequence patterns

#### Enhanced State Management:
```cpp
void BaseChannel::smartButtonPress(uint8_t button_index) {
    switch (current_button_mode_) {
        case BUTTON_MODE_1BUTTON:
            singleButtonCycle();
            break;
            
        case BUTTON_MODE_2BUTTON:
            if (button_index == 0) toggleOnOff();
            else if (button_index == 1 && isOn()) nextMode();
            break;
            
        case BUTTON_MODE_3BUTTON:
            switch (button_index) {
                case 0: onMode(); break;
                case 1: offMode(); break;
                case 2: if (isOn()) nextMode(); break;
            }
            break;
            
        case BUTTON_MODE_4BUTTON:
            switch (button_index) {
                case 0: nextMode(); break;
                case 1: previousMode(); break;
                case 2: onMode(); break;
                case 3: offMode(); break;
            }
            break;
            
        case BUTTON_MODE_CUSTOM:
            if (button_config_.custom_handlers[button_index]) {
                button_config_.custom_handlers[button_index]();
            }
            break;
    }
}
```

### 5. Intelligent State Preservation

```cpp
void BaseChannel::onMode() {
    if (channel_data_.last_active_sequence_ == 0) {
        channel_data_.last_active_sequence_ = 1;  // Default to static ON
    }
    
    channel_data_.sequence_index_ = channel_data_.last_active_sequence_;
    channel_data_.is_on_ = true;
    channel_data_.is_mode_changed_ = true;
    
    temp_mode_ = total_mode_[channel_data_.sequence_index_];
}

void BaseChannel::offMode() {
    if (channel_data_.sequence_index_ > 0) {
        channel_data_.last_active_sequence_ = channel_data_.sequence_index_;
    }
    
    channel_data_.sequence_index_ = 0;  // OFF mode
    channel_data_.is_on_ = false;
    channel_data_.is_mode_changed_ = true;
    
    temp_mode_ = total_mode_[0];  // OFF sequence
    forceOff();  // Immediately turn off all LEDs
}
```

## Advanced Configuration Management

### Dynamic PCF Configuration

```cpp
class BaseConfig {
public:
    // Dynamic PCF management
    bool setDistributionStrategy(distribution_strategy_t strategy);
    bool setPCFCount(uint8_t count);
    bool setPCFAddresses(std::vector<uint8_t> addresses);
    bool distributeChannels(uint8_t total_channels);
    
    // Advanced configuration
    bool validatePCFConfiguration();
    bool optimizePCFDistribution();
    bool testPCFConnections();
    
    // Configuration queries
    PCFDistribution* getPCFDistribution(uint8_t pcf_index);
    uint8_t getChannelForPCF(uint8_t pcf_address, uint8_t pin);
    uint8_t getPCFForChannel(uint8_t channel);
    
private:
    DynamicConfigData dynamic_config_;
    bool validateDistribution();
    bool applyDistribution();
};
```

### Usage Examples:

#### 1. Balanced Distribution Setup
```cpp
BaseConfig config;
BaseChannel channel;

void setupBalancedDistribution() {
    // Setup for 24 channels across 3 PCFs
    config.setDistributionStrategy(DISTRIBUTE_BALANCED);
    config.setPCFCount(3);
    config.setPCFAddresses({0x20, 0x21, 0x22});
    config.distributeChannels(24);
    
    if (config.validatePCFConfiguration()) {
        channel.attachConfig(&config);
        channel.initialize();
        
        Serial.println("Balanced distribution configured:");
        Serial.println("PCF 0x20: Channels 0-7");
        Serial.println("PCF 0x21: Channels 8-15"); 
        Serial.println("PCF 0x22: Channels 16-23");
    }
}
```

#### 2. Optimized Distribution Setup
```cpp
void setupOptimizedDistribution() {
    // Setup for 20 channels, minimize PCF usage
    config.setDistributionStrategy(DISTRIBUTE_OPTIMIZED);
    config.setPCFCount(4);  // 4 available, but only use what's needed
    config.setPCFAddresses({0x20, 0x21, 0x22, 0x23});
    config.distributeChannels(20);
    
    if (config.validatePCFConfiguration()) {
        Serial.println("Optimized distribution configured:");
        Serial.println("PCF 0x20: Channels 0-7 (full)");
        Serial.println("PCF 0x21: Channels 8-15 (full)");
        Serial.println("PCF 0x22: Channels 16-19 (partial)");
        Serial.println("PCF 0x23: Not used (power saving)");
    }
}
```

#### 3. Custom Distribution Setup
```cpp
void setupCustomDistribution() {
    // Custom setup for specific hardware layout
    config.setDistributionStrategy(DISTRIBUTE_CUSTOM);
    
    DynamicConfigData* dynamic = config.getDynamicConfig();
    
    // PCF 1: Main controller (all pins)
    dynamic->pcf_list[0] = {0x27, 8, 0, 0, true};
    
    // PCF 2: Secondary controller (6 pins only, hardware limitation)
    dynamic->pcf_list[1] = {0x45, 6, 8, 0, true};
    
    // PCF 3: Tertiary controller (4 pins, offset by 2)
    dynamic->pcf_list[2] = {0x67, 4, 14, 2, true};
    
    dynamic->pcf_count = 3;
    dynamic->total_channels = 18;
    dynamic->strategy = DISTRIBUTE_CUSTOM;
    
    if (config.validatePCFConfiguration()) {
        Serial.println("Custom distribution configured:");
        Serial.println("PCF 0x27: Channels 0-7 (pins 0-7)");
        Serial.println("PCF 0x45: Channels 8-13 (pins 0-5)");
        Serial.println("PCF 0x67: Channels 14-17 (pins 2-5)");
    }
}
```

## Multi-Modal Button Implementation

### Button Mode Setup Examples:

#### 1. Single Button Mode (Most Popular)
```cpp
BaseChannel channel;

void setupSingleButtonMode() {
    channel.setButtonMode(BUTTON_MODE_1BUTTON);
    
    // Single button attached to pin 2
    ButtonInterrupt button;
    button.attachPin(2);
    channel.attachInterrupt(&button);
    
    Serial.println("Single button mode: OFF → ON → Mode2-15 → OFF");
}

// ISR for single button
void IRAM_ATTR singleButtonISR() {
    channel.smartButtonPress(0);  // Always button index 0 in single mode
}
```

#### 2. Dual Button Mode (Toggle + Next)
```cpp
void setupDualButtonMode() {
    channel.setButtonMode(BUTTON_MODE_2BUTTON);
    
    // Two buttons: Toggle (pin 2) and Next (pin 3)
    ButtonInterrupt toggleButton, nextButton;
    toggleButton.attachPin(2);
    nextButton.attachPin(3);
    
    attachInterrupt(digitalPinToInterrupt(2), toggleButtonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(3), nextButtonISR, FALLING);
    
    Serial.println("Dual button mode: Button0=Toggle, Button1=Next");
}

void IRAM_ATTR toggleButtonISR() {
    channel.smartButtonPress(0);  // Toggle ON/OFF
}

void IRAM_ATTR nextButtonISR() {
    channel.smartButtonPress(1);  // Next mode (if ON)
}
```

#### 3. Four Button Mode (Full Control)
```cpp
void setupFourButtonMode() {
    channel.setButtonMode(BUTTON_MODE_4BUTTON);
    
    // Four buttons for full control
    attachInterrupt(digitalPinToInterrupt(2), nextButtonISR, FALLING);     // Next
    attachInterrupt(digitalPinToInterrupt(3), prevButtonISR, FALLING);     // Previous  
    attachInterrupt(digitalPinToInterrupt(4), onButtonISR, FALLING);       // ON
    attachInterrupt(digitalPinToInterrupt(5), offButtonISR, FALLING);      // OFF
    
    Serial.println("Four button mode: Next/Prev/On/Off");
}

void IRAM_ATTR nextButtonISR() { channel.smartButtonPress(0); }
void IRAM_ATTR prevButtonISR() { channel.smartButtonPress(1); }
void IRAM_ATTR onButtonISR() { channel.smartButtonPress(2); }
void IRAM_ATTR offButtonISR() { channel.smartButtonPress(3); }
```

#### 4. Custom Button Mode
```cpp
void customHandler1() {
    Serial.println("Custom action 1: Blink mode");
    channel.changeModeApp(2);  // Force to blink mode
}

void customHandler2() {
    Serial.println("Custom action 2: Chase mode");
    channel.changeModeApp(5);  // Force to chase mode
}

void setupCustomButtonMode() {
    ButtonConfig config;
    config.mode = BUTTON_MODE_CUSTOM;
    config.button_count = 2;
    config.custom_handlers[0] = customHandler1;
    config.custom_handlers[1] = customHandler2;
    
    channel.setButtonConfig(&config);
    
    Serial.println("Custom button mode with user-defined handlers");
}
```

## Advanced Usage Patterns

### Complete Setup Example
```cpp
#define ENABLE_ADDONS_AUTOLIGHT_V3
#include "Kinematrix.h"

using namespace AutoLight;

BaseConfig config;
BaseChannel channel;
ButtonInterrupt button;

void setup() {
    Serial.begin(115200);
    
    // 1. Configure dynamic PCF distribution
    setupDynamicPCF();
    
    // 2. Setup button system
    setupButtonSystem();
    
    // 3. Initialize system
    if (channel.initialize()) {
        Serial.println("AutoLight V3 initialized successfully");
        channel.debug();  // Show configuration
    }
}

void setupDynamicPCF() {
    // Optimized distribution for 20 channels
    config.setDistributionStrategy(DISTRIBUTE_OPTIMIZED);
    config.setPCFCount(3);
    config.setPCFAddresses({0x27, 0x45, 0x67});
    config.distributeChannels(20);
    
    if (config.testPCFConnections()) {
        Serial.println("All PCFs responding");
        channel.attachConfig(&config);
    } else {
        Serial.println("PCF connection error - check wiring");
    }
}

void setupButtonSystem() {
    // Single button mode for simplicity
    channel.setButtonMode(BUTTON_MODE_1BUTTON);
    
    button.attachPin(2);
    button.setDebounceTime(250);
    channel.attachInterrupt(&button);
    
    attachInterrupt(digitalPinToInterrupt(2), buttonISR, FALLING);
}

void IRAM_ATTR buttonISR() {
    channel.smartButtonPress(0);
}

void loop() {
    // Main control loop
    channel.runAutoLight([](uint32_t sequence) {
        Serial.printf("Sequence changed to: %d, System %s\n", 
                     sequence, channel.isOn() ? "ON" : "OFF");
    });
    
    delay(20);
}
```

### Runtime Mode Switching
```cpp
void demonstrateRuntimeSwitching() {
    Serial.println("Demonstrating runtime button mode switching:");
    
    // Start with single button
    channel.setButtonMode(BUTTON_MODE_1BUTTON);
    delay(5000);
    
    // Switch to dual button
    Serial.println("Switching to dual button mode");
    channel.setButtonMode(BUTTON_MODE_2BUTTON);
    delay(5000);
    
    // Switch to four button
    Serial.println("Switching to four button mode");
    channel.setButtonMode(BUTTON_MODE_4BUTTON);
    delay(5000);
    
    // Back to single button
    Serial.println("Back to single button mode");
    channel.setButtonMode(BUTTON_MODE_1BUTTON);
}
```

## Performance Characteristics

### Memory Requirements
- **Base System**: ~55KB Flash, ~4KB RAM
- **With Dynamic PCF**: +~8KB Flash, +~2KB RAM
- **With Multi-Modal Buttons**: +~3KB Flash, +~1KB RAM
- **Full V3 Configuration**: ~70KB Flash, ~8KB RAM total

### Hardware Capabilities
- **Maximum Channels**: 64 (8 PCF × 8 pins)
- **Maximum PCF Expanders**: 8 with dynamic distribution
- **Button Modes**: 5 different interaction paradigms
- **Sequences**: 16 total modes (0-15)
- **Distribution Strategies**: 4 intelligent algorithms

### Platform Support
- **ESP32**: Full support dengan semua features
- **ESP8266**: Limited support (reduced features)
- **AVR**: Basic support (limited dynamic features)

## Evolution dari V2

### Revolutionary Improvements
1. **Dynamic PCF Distribution**: Intelligent allocation vs fixed assignment
2. **Multi-Modal Button System**: 5 interaction modes vs single mode
3. **State Preservation**: Smart ON/OFF state management
4. **Hardware Adaptation**: Runtime PCF configuration vs compile-time
5. **Advanced Configuration**: Validation and optimization algorithms

### Architectural Breakthroughs
- **Smart Distribution**: Algorithms untuk optimal PCF usage
- **Button Abstraction**: Universal button interface untuk berbagai modes
- **State Management**: Intelligent state preservation dan recovery
- **Runtime Flexibility**: Dynamic configuration changes without recompilation

### Backward Compatibility
- **V2 API**: Maintains compatibility dengan V2 methods
- **Hardware**: Supports all V1/V2 I2C expansion setups
- **Configuration**: Can import V2 configurations dan upgrade

## Kesimpulan

AutoLight V3 merupakan revolutionary advancement yang mengubah paradigm LED control dari fixed configuration ke dynamic, intelligent system. Dengan PCF distribution algorithms dan multi-modal button system, V3 provides unprecedented flexibility dan user experience.

### Revolutionary Features:
- **Dynamic PCF Distribution**: 4 intelligent distribution strategies
- **Multi-Modal Buttons**: 5 different interaction paradigms
- **Smart State Management**: Intelligent ON/OFF state preservation
- **Runtime Configuration**: Dynamic hardware adaptation
- **Hardware Intelligence**: Automatic PCF testing dan validation

### Perfect For:
- **Advanced Projects**: Complex LED installations dengan multiple PCFs
- **User-Friendly Applications**: Projects requiring intuitive button control
- **Production Systems**: Systems needing runtime configuration flexibility
- **Educational**: Understanding advanced embedded system architecture
- **Professional**: Commercial applications requiring reliability

V3 establishes the foundation untuk V4's fully automated features while providing sophisticated manual control capabilities yang suitable untuk both beginners dan professionals.