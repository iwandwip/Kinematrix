# AutoLight V5 - Dynamic Matrix Lighting System

## Revolutionary Concept Overview

AutoLightV5 transforms LED control from **fixed hardcoded sequences** to **user-defined dynamic patterns** with **matrix spatial control**. Core philosophy: **Flexible Architecture, User-Defined Patterns, Universal Compatibility**.

Key Breakthrough: **No hardcoded sequences in library** - users create unlimited custom patterns both via **C++ code** and **JSON runtime**, with complete freedom to map PCF pins to matrix positions.

## Core Matrix Concept

### Matrix Size Support
- **4x4 Matrix**: 16 positions for compact applications
- **8x8 Matrix**: 64 positions for standard installations  
- **16x16 Matrix**: 256 positions for large displays
- **Custom Matrix**: Any NxM size up to 32x32

### Dynamic PCF-to-Matrix Mapping
Complete freedom mapping any PCF pin to any matrix position:

```cpp
// Example: 3 PCF Configuration  
PCF 0x27: [P0, P1, P2, P3, P4, P5, P6, P7] // 8 pins available
PCF 0x45: [P0, P1, P2, P3, P4, P5, P6, P7] // 8 pins available  
PCF 0x67: [P0, P1, P2, P3, P4, P5, P6, P7] // 8 pins available
Total: 24 physical pins available

// Map freely to 8x8 matrix (64 positions)
// Use only positions needed - rest stay XXXX (empty)
```

### Pin Naming Convention
```cpp
// Format: P{pin_number}{PCF_address_hex}
P027 = Pin 0 of PCF 0x27
P127 = Pin 1 of PCF 0x27  
P045 = Pin 0 of PCF 0x45
P767 = Pin 7 of PCF 0x67
```

## Matrix Examples

### Your Requested Layout
```cpp
// 8x8 Matrix - Complete freedom in pin placement
[P027, P127, XXXX, XXXX, XXXX, XXXX, P627, P727]  // Row 0
[XXXX, XXXX, P227, XXXX, XXXX, P527, XXXX, XXXX]  // Row 1  
[XXXX, XXXX, P327, XXXX, XXXX, P427, XXXX, XXXX]  // Row 2
[XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX]  // Row 3-7 (empty)
[XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX]
[XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX]
[XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX]
[XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX, XXXX]

// XXXX = Empty/unused position
// User can place remaining P045, P067 pins anywhere they want
```

## Architecture Design (Balanced Approach)

### 1. Core Data Structures (No STD Library)
```cpp
// Matrix position mapping
struct MatrixPosition {
    uint8_t row, col;        // Matrix coordinates
    uint8_t pcf_address;     // PCF I2C address
    uint8_t pcf_pin;         // PCF pin number (0-7)
    bool is_active;          // Position has assigned pin
    char pin_id[8];          // Pin identifier "P027", etc.
};

// Matrix configuration
struct MatrixConfig {
    uint8_t rows, cols;                // Matrix dimensions
    MatrixPosition positions[256];     // All positions (max 16x16)
    uint8_t active_count;              // Number of active positions
    uint8_t pcf_count;                 // Number of PCFs used
    uint8_t pcf_addresses[8];          // PCF I2C addresses array
};

// Pattern step structure (Enhanced for Simultaneous Control)
struct PatternStep {
    char targets[64];      // Multiple targets: "P027 P127 P227", "ALL", "ROW:0", etc.
    bool state;            // ON/OFF
    uint16_t duration;     // Step duration in ms
    uint16_t delay_after;  // Delay after step in ms
    bool is_simultaneous;  // Execute all targets at same time
};

// User pattern definition
struct UserPattern {
    char name[32];              // Pattern name
    PatternStep steps[100];     // Pattern steps (fixed array)
    uint8_t step_count;         // Actual step count
    bool loop;                  // Should pattern loop
    uint16_t loop_delay;        // Delay between loops
};
```

### 2. Core Classes Architecture

#### A. MatrixManager Class
```cpp
class MatrixManager {
public:
    MatrixManager();
    ~MatrixManager();
    
    // Matrix setup
    bool initMatrix(uint8_t rows, uint8_t cols);
    bool assignPin(uint8_t row, uint8_t col, uint8_t pcf_addr, uint8_t pin);
    bool removePin(uint8_t row, uint8_t col);
    void clearMatrix();
    
    // Matrix operations
    bool setPosition(uint8_t row, uint8_t col, bool state);
    bool getPosition(uint8_t row, uint8_t col);
    void setRow(uint8_t row, bool state);
    void setColumn(uint8_t col, bool state);  
    void setAll(bool state);
    
    // Pin operations
    bool setPin(const char* pin_id, bool state);  // "P027"
    bool setPins(const char* pin_list, bool state); // "P027 P127 P227"
    bool setPinsSimultaneous(const char* pin_list, bool state); // All pins at exact same time
    
    // Group operations
    bool setGroup(const char* group_name, bool state);  // Named groups
    bool defineGroup(const char* group_name, const char* pin_list); // Create groups
    
    // Configuration
    MatrixConfig* getConfig();
    bool validateConfig();
    void printMatrix();
    void printActivePins();
    
private:
    MatrixConfig config_;
    bool findPin(const char* pin_id, uint8_t* row, uint8_t* col);
    void updateHardware();
};
```

#### B. PatternBuilder Class (C++ Pattern Creation)
```cpp
class PatternBuilder {
public:
    PatternBuilder();
    PatternBuilder(const char* pattern_name);
    
    // Fluent API for C++ pattern building
    PatternBuilder& step(const char* target, bool state, uint16_t duration);
    PatternBuilder& stepPin(const char* pin_id, bool state, uint16_t duration);
    PatternBuilder& stepPins(const char* pin_list, bool state, uint16_t duration); // Multiple pins
    PatternBuilder& stepPinsSimultaneous(const char* pin_list, bool state, uint16_t duration); // Exact same time
    PatternBuilder& stepRow(uint8_t row, bool state, uint16_t duration);
    PatternBuilder& stepCol(uint8_t col, bool state, uint16_t duration);
    PatternBuilder& stepAll(bool state, uint16_t duration);
    PatternBuilder& stepGroup(const char* group_name, bool state, uint16_t duration); // Named groups
    PatternBuilder& delay(uint16_t ms);
    PatternBuilder& loop(bool enable, uint16_t loop_delay = 0);
    
    // Build and get pattern
    UserPattern build();
    bool saveTo(UserPattern* pattern);
    
    // Reset builder
    void reset();
    void reset(const char* new_name);
    
private:
    UserPattern current_pattern_;
    void addStep(const char* target, bool state, uint16_t duration, uint16_t delay_after);
};
```

#### C. PatternLoader Class (JSON Support)
```cpp
class PatternLoader {
public:
    PatternLoader();
    
    // JSON loading
    bool loadFromJSON(const char* json_string, UserPattern* pattern);
    bool loadFromFile(const char* filename, UserPattern* pattern);
    
    // JSON saving
    bool saveToJSON(const UserPattern* pattern, char* json_buffer, size_t buffer_size);
    bool saveToFile(const UserPattern* pattern, const char* filename);
    
    // Validation
    bool validateJSON(const char* json_string);
    const char* getLastError();
    
private:
    char error_buffer_[128];
    bool parseJSONStep(const char* step_json, PatternStep* step);
    void buildJSONStep(const PatternStep* step, char* json_buffer, size_t buffer_size);
};
```

#### D. PatternEngine Class (Execution)
```cpp
class PatternEngine {
public:
    PatternEngine();
    
    // Pattern management
    bool loadPattern(const UserPattern* pattern);
    bool addPattern(const UserPattern* pattern);
    void clearPatterns();
    
    // Playback control
    void play();
    void stop();
    void pause();
    void resume();
    void next();
    void previous();
    void goto(uint8_t pattern_index);
    
    // Pattern execution
    void update();  // Call in loop()
    bool isPlaying();
    bool isLooping();
    uint8_t getCurrentPatternIndex();
    uint8_t getCurrentStep();
    
    // Speed control
    void setSpeed(float multiplier);  // 0.1 to 10.0
    float getSpeed();
    
    // Attach matrix
    void attachMatrix(MatrixManager* matrix);
    
private:
    MatrixManager* matrix_;
    UserPattern patterns_[10];      // Support up to 10 patterns
    uint8_t pattern_count_;
    uint8_t current_pattern_;
    uint8_t current_step_;
    bool is_playing_;
    bool is_paused_;
    float speed_multiplier_;
    uint32_t last_update_;
    uint32_t step_start_time_;
    
    void executeStep(const PatternStep* step);
    void processStepTarget(const char* target, bool state);
};
```

#### E. ButtonManager Class (Multi-Modal)
```cpp
class ButtonManager {
public:
    ButtonManager();
    
    // Button configuration
    void setMode(uint8_t button_count);  // 1, 2, 3, or 4 buttons
    void attachButton(uint8_t index, uint8_t pin);
    void setDebounceTime(uint16_t ms);
    
    // Button handling
    void update();  // Call in loop()
    bool isPressed(uint8_t index);
    bool isLongPressed(uint8_t index);
    bool isDoublePressed(uint8_t index);
    
    // Pattern engine integration
    void attachPatternEngine(PatternEngine* engine);
    
    // Custom callbacks
    void setCallback(uint8_t index, void (*callback)());
    void setLongPressCallback(uint8_t index, void (*callback)());
    void setDoublePressCallback(uint8_t index, void (*callback)());
    
private:
    PatternEngine* engine_;
    uint8_t button_count_;
    uint8_t button_pins_[4];
    uint16_t debounce_time_;
    
    // Button state tracking
    bool button_states_[4];
    bool last_states_[4];
    uint32_t press_times_[4];
    uint32_t release_times_[4];
    bool long_press_triggered_[4];
    
    // Callbacks
    void (*callbacks_[4])();
    void (*long_callbacks_[4])();
    void (*double_callbacks_[4])();
    
    void handleButtonPress(uint8_t index);
    void handleDefaultBehavior(uint8_t index, bool is_long, bool is_double);
};
```

## Usage Examples

### 1. C++ Pattern Creation (Fluent API)
```cpp
#include "AutoLightV5.h"

MatrixManager matrix;
PatternBuilder builder;
PatternEngine engine;

void setup() {
    // Setup 8x8 matrix
    matrix.initMatrix(8, 8);
    
    // Map pins exactly as you want
    matrix.assignPin(0, 0, 0x27, 0);  // P027 at [0,0]
    matrix.assignPin(0, 1, 0x27, 1);  // P127 at [0,1]  
    matrix.assignPin(0, 6, 0x27, 6);  // P627 at [0,6]
    matrix.assignPin(0, 7, 0x27, 7);  // P727 at [0,7]
    matrix.assignPin(1, 2, 0x27, 2);  // P227 at [1,2]
    matrix.assignPin(1, 5, 0x27, 5);  // P527 at [1,5]
    matrix.assignPin(2, 2, 0x27, 3);  // P327 at [2,2]
    matrix.assignPin(2, 5, 0x27, 4);  // P427 at [2,5]
    
    // Define LED groups first
    matrix.defineGroup("OUTER_RING", "P027 P127 P627 P727");
    matrix.defineGroup("INNER_RING", "P227 P327 P427 P527");
    
    // Create pattern using fluent C++ API
    UserPattern my_pattern = builder
        .reset("MyAwesomePattern")
        
        // Simultaneous control examples
        .stepPinsSimultaneous("P027 P127", true, 200)    // 2 LEDs ON at exact same time
        .stepPinsSimultaneous("P627 P727", true, 200)    // 2 more LEDs ON simultaneously
        .stepGroup("INNER_RING", true, 300)              // Entire inner ring ON
        .delay(500)                                      // Wait 500ms
        .stepAll(false, 100)                             // All OFF simultaneously
        
        // Sequential then simultaneous
        .stepPin("P727", true, 100)                      // P727 ON first
        .stepPin("P027", true, 100)                      // P027 ON second  
        .stepPinsSimultaneous("P227 P327 P427 P527", true, 200) // Inner ring all at once
        .delay(300)
        .stepAll(false, 200)                             // All OFF together
        
        .loop(true, 1000)                                // Loop with 1s delay
        .build();
    
    // Load pattern to engine
    engine.attachMatrix(&matrix);
    engine.loadPattern(&my_pattern);
}

void loop() {
    engine.update();  // Handle pattern execution
}
```

### 2. JSON Pattern Loading
```cpp
PatternLoader loader;
PatternEngine engine;

void setup() {
    // ... matrix setup same as above ...
    
    // JSON pattern with simultaneous control
    const char* json_pattern = R"({
        "name": "SimultaneousDemo",
        "groups": {
            "OUTER": "P027 P127 P627 P727",
            "INNER": "P227 P327 P427 P527"
        },
        "steps": [
            {"pins": "P027 P127", "state": 1, "duration": 200, "simultaneous": true},
            {"pins": "P627 P727", "state": 1, "duration": 200, "simultaneous": true},
            {"group": "INNER", "state": 1, "duration": 300, "simultaneous": true},
            {"all": true, "state": 0, "duration": 100, "simultaneous": true, "delay": 500},
            {"pin": "P727", "state": 1, "duration": 100},
            {"pins": "P227 P327 P427 P527", "state": 1, "duration": 200, "simultaneous": true},
            {"all": true, "state": 0, "duration": 200, "simultaneous": true}
        ],
        "loop": true,
        "loop_delay": 1000
    })";
    
    // Load JSON pattern
    UserPattern json_pattern_obj;
    if (loader.loadFromJSON(json_pattern, &json_pattern_obj)) {
        engine.loadPattern(&json_pattern_obj);
        Serial.println("JSON pattern loaded successfully!");
    } else {
        Serial.println("JSON loading failed: ");
        Serial.println(loader.getLastError());
    }
}
```

### 3. Multi-Modal Button Control
```cpp
ButtonManager buttons;

void setup() {
    // ... previous setup ...
    
    // Configure for 2-button mode
    buttons.setMode(2);
    buttons.attachButton(0, 2);  // Button 1 on pin 2
    buttons.attachButton(1, 3);  // Button 2 on pin 3
    buttons.attachPatternEngine(&engine);
    
    // Button behavior:
    // Button 0: Play/Stop toggle
    // Button 1: Next pattern
    // Long press Button 0: Previous pattern
}

void loop() {
    buttons.update();  // Handle button presses
    engine.update();   // Handle pattern execution
}
```

### 4. Advanced Simultaneous Control Examples
```cpp
void createSimultaneousPatterns() {
    // Example 1: Blink Pattern - Multiple LEDs simultaneously
    UserPattern blink_pattern = builder
        .reset("BlinkPattern")
        .stepPinsSimultaneous("P027 P127 P227 P327", true, 300)  // 4 LEDs ON together
        .stepPinsSimultaneous("P427 P527 P627 P727", true, 300)  // Other 4 LEDs ON together  
        .delay(200)
        .stepAll(false, 200)                                     // All OFF together
        .loop(true, 1000)
        .build();
    
    // Example 2: Wave Pattern - Groups in sequence
    UserPattern wave_pattern = builder
        .reset("WavePattern")
        .stepPinsSimultaneous("P027 P127", true, 100)            // Left side
        .stepPinsSimultaneous("P227 P327", true, 100)            // Center
        .stepPinsSimultaneous("P427 P527", true, 100)            // Right-center  
        .stepPinsSimultaneous("P627 P727", true, 100)            // Right side
        .delay(100)
        .stepAll(false, 50)                                      // All OFF quickly
        .loop(true, 500)
        .build();
    
    // Example 3: Zone Control - Matrix areas
    UserPattern zone_pattern = builder
        .reset("ZonePattern")
        .stepRow(0, true, 200)                                   // Row 0 all ON
        .stepRow(1, true, 200)                                   // Row 1 all ON
        .stepRow(2, true, 200)                                   // Row 2 all ON
        .delay(300)
        .stepAll(false, 100)                                     // All OFF
        .stepCol(0, true, 150)                                   // Column 0 all ON
        .stepCol(2, true, 150)                                   // Column 2 all ON
        .stepCol(5, true, 150)                                   // Column 5 all ON
        .stepCol(7, true, 150)                                   // Column 7 all ON
        .delay(400)
        .stepAll(false, 100)                                     // All OFF
        .loop(true, 800)
        .build();
    
    // Add all patterns to engine
    engine.addPattern(&blink_pattern);
    engine.addPattern(&wave_pattern);  
    engine.addPattern(&zone_pattern);
}

void createCustomGroups() {
    // Define custom LED groups for specific patterns
    matrix.defineGroup("CORNERS", "P027 P727");        // Corner LEDs
    matrix.defineGroup("EDGES", "P127 P227 P327 P427 P527 P627"); // Edge LEDs
    matrix.defineGroup("LEFT_SIDE", "P027 P127 P227 P327");  // Left side
    matrix.defineGroup("RIGHT_SIDE", "P427 P527 P627 P727"); // Right side
    
    // Use groups in patterns
    UserPattern group_pattern = builder
        .reset("GroupPattern")
        .stepGroup("CORNERS", true, 200)                // Corners ON
        .delay(100)
        .stepGroup("EDGES", true, 200)                  // Edges ON
        .delay(100)
        .stepGroup("CORNERS", false, 100)              // Corners OFF
        .stepGroup("EDGES", false, 100)                // Edges OFF
        .delay(200)
        .stepGroup("LEFT_SIDE", true, 150)             // Left ON
        .stepGroup("RIGHT_SIDE", true, 150)            // Right ON (simultaneous)
        .delay(300)
        .stepAll(false, 100)                           // All OFF
        .loop(true, 600)
        .build();
        
    engine.addPattern(&group_pattern);
}
```

## JSON Pattern Format

### Enhanced Pattern Structure (With Simultaneous Control)
```json
{
  "name": "SimultaneousPattern",
  "groups": {
    "OUTER": "P027 P127 P627 P727",
    "INNER": "P227 P327 P427 P527",
    "LEFT": "P027 P127 P227 P327",
    "RIGHT": "P427 P527 P627 P727"
  },
  "steps": [
    {"pins": "P027 P127", "state": 1, "duration": 200, "simultaneous": true},
    {"group": "INNER", "state": 1, "duration": 300, "simultaneous": true},
    {"all": true, "state": 0, "duration": 100, "simultaneous": true, "delay": 500},
    {"row": 0, "state": 1, "duration": 150, "simultaneous": true},
    {"col": 2, "state": 0, "duration": 100, "simultaneous": true}
  ],
  "loop": true,
  "loop_delay": 1000
}
```

### All Pattern Step Options
```json
// Individual pin control
{"pin": "P027", "state": 1, "duration": 100, "delay": 0}

// Multiple pins (sequential by default)
{"pins": "P027 P127 P227", "state": 1, "duration": 100, "delay": 50}

// Multiple pins (simultaneous - exact same time)
{"pins": "P027 P127 P227", "state": 1, "duration": 100, "simultaneous": true}

// Named group control (simultaneous)
{"group": "OUTER", "state": 1, "duration": 200, "simultaneous": true}

// Matrix row control (all pins in row simultaneously)
{"row": 0, "state": 1, "duration": 100, "simultaneous": true}

// Matrix column control (all pins in column simultaneously)  
{"col": 2, "state": 1, "duration": 100, "simultaneous": true}

// All pins control (all at exact same time)
{"all": true, "state": 0, "duration": 200, "simultaneous": true}

// Range control (pins P027 through P427 simultaneously)
{"range": "P027-P427", "state": 1, "duration": 150, "simultaneous": true}
```

## Multi-Modal Button System

### Button Mode Configuration
```cpp
typedef enum {
    BTN_MODE_1 = 1,    // 1 button with gestures
    BTN_MODE_2 = 2,    // 2 button optimized  
    BTN_MODE_3 = 3,    // 3 button with dedicated functions
    BTN_MODE_4 = 4     // 4 button full control
} button_mode_t;
```

### Button Behavior Per Mode
```cpp
// 1 BUTTON MODE:
// Short press: Play/Stop toggle
// Long press: Next pattern  
// Double press: Previous pattern

// 2 BUTTON MODE:  
// Button 0: Play/Stop toggle + Long press for Previous
// Button 1: Next pattern

// 3 BUTTON MODE:
// Button 0: Play
// Button 1: Stop  
// Button 2: Next pattern + Long press for Previous

// 4 BUTTON MODE:
// Button 0: Play
// Button 1: Stop
// Button 2: Next pattern
// Button 3: Previous pattern
```

## Key Advantages

### 1. Flexible Architecture
- ✅ **Multiple classes** like AutoLightV3 but organized better
- ✅ **No STD library** - works on all microcontrollers
- ✅ **Modular design** - use only what you need
- ✅ **Clear separation** of concerns

### 2. Dual Pattern Creation Methods
- ✅ **C++ Fluent API** - powerful programmatic pattern creation
- ✅ **JSON Runtime Loading** - dynamic patterns from files/web/serial
- ✅ **Pattern Builder** - intuitive chaining syntax
- ✅ **Pattern Validation** - comprehensive error checking

### 3. Matrix Spatial Control
- ✅ **Free pin placement** - any PCF pin to any matrix position
- ✅ **Sparse matrices** - use only needed positions  
- ✅ **Spatial operations** - row/column/all controls
- ✅ **Visual intuition** - matches physical LED layout

### 4. Advanced Pattern Engine
- ✅ **Multiple pattern support** - up to 10 patterns loaded
- ✅ **Playback control** - play/stop/pause/next/previous
- ✅ **Speed control** - adjust pattern timing dynamically
- ✅ **Loop support** - configurable pattern looping

### 5. Simultaneous LED Control
- ✅ **Multiple LED simultaneous ON/OFF** - exact same timing
- ✅ **Group control** - named LED groups for complex patterns
- ✅ **Row/Column simultaneous** - entire matrix rows/columns at once
- ✅ **Mixed sequential/simultaneous** - combine both approaches in one pattern

## AutoLight V3 vs V5 Evolution

| Feature | AutoLight V3 | AutoLight V5 |
|---------|--------------|--------------|
| **Sequences** | 14 hardcoded in BaseChannelSequence.cpp | Unlimited user-defined |
| **Creation Method** | C++ code only | C++ Fluent API + JSON |
| **Pin Control** | Linear channels (0-63) | Matrix positions [row,col] |
| **Flexibility** | Fixed patterns only | Complete creative freedom |
| **Classes** | BaseChannel, BaseConfig, etc. | MatrixManager, PatternBuilder, etc. |
| **Dependencies** | Multiple Kinematrix modules | Single AutoLightV5 header |
| **Memory Usage** | ~4KB minimum | ~2KB minimum |
| **Runtime Loading** | No | Yes (JSON patterns) |

## Implementation Philosophy

AutoLightV5 revolutionizes LED control by:

1. **🎯 User-Defined Everything**: No hardcoded limitations, unlimited creativity
2. **🎨 Matrix Spatial Thinking**: 2D coordinates instead of linear channels  
3. **🔧 Flexible Architecture**: Multiple classes but well-organized and focused
4. **⚡ Dual Creation Methods**: C++ API for power users, JSON for flexibility
5. **🚀 Pattern Engine**: Advanced playback control with multiple pattern support
6. **📱 Universal Compatibility**: No STD library, works on all microcontrollers

**The Revolution**: Transform users from **pattern consumers** to **pattern creators** while maintaining the architectural flexibility that made AutoLight successful! 🌟

## Simultaneous Control Use Cases

### Real-World Application Examples

#### 1. **Automotive Turn Signals**
```cpp
// All turn signal LEDs blink together
UserPattern turn_signal = builder
    .reset("TurnSignal")
    .stepGroup("LEFT_TURN", true, 300)      // All left LEDs ON together
    .stepGroup("LEFT_TURN", false, 300)     // All left LEDs OFF together  
    .loop(true, 0)                          // Continuous blink
    .build();
```

#### 2. **Emergency Flashers**
```cpp
// All LEDs flash simultaneously for emergency
UserPattern emergency = builder
    .reset("EmergencyFlash")
    .stepAll(true, 200)                     // ALL LEDs ON at same time
    .stepAll(false, 200)                    // ALL LEDs OFF at same time
    .loop(true, 100)                        // Fast emergency flash
    .build();
```

#### 3. **Sequential Wave with Groups**
```cpp
// Groups of LEDs light in wave pattern
UserPattern wave = builder
    .reset("GroupWave")
    .stepPinsSimultaneous("P027 P127", true, 100)     // Group 1 ON
    .stepPinsSimultaneous("P227 P327", true, 100)     // Group 2 ON  
    .stepPinsSimultaneous("P427 P527", true, 100)     // Group 3 ON
    .stepPinsSimultaneous("P627 P727", true, 100)     // Group 4 ON
    .delay(200)
    .stepAll(false, 50)                               // All OFF together
    .loop(true, 300)
    .build();
```

#### 4. **Zone-Based Lighting**
```cpp
// Matrix zones light up simultaneously
UserPattern zones = builder
    .reset("ZoneLighting")
    .stepRow(0, true, 200)                  // Top row all ON
    .stepRow(2, true, 200)                  // Bottom row all ON
    .delay(300)
    .stepCol(0, true, 150)                  // Left column all ON
    .stepCol(7, true, 150)                  // Right column all ON
    .delay(300)
    .stepAll(false, 100)                    // Everything OFF together
    .loop(true, 500)
    .build();
```

### Performance Benefits

#### Hardware I2C Optimization
- **Batch Operations**: Multiple pins on same PCF updated in single I2C transaction
- **Reduced Latency**: True simultaneous control with minimal timing differences
- **Efficient Communication**: Group multiple LED changes into fewer I2C calls

#### Timing Precision
- **Microsecond Accuracy**: All simultaneous LEDs switch within microseconds of each other
- **Predictable Behavior**: No sequential delays between LEDs in same group
- **Synchronized Effects**: Perfect synchronization for complex lighting patterns

### Implementation Notes

#### C++ API Benefits
```cpp
// Clean, readable code for complex simultaneous patterns
builder
    .stepPinsSimultaneous("P027 P127 P227", true, 200)  // 3 LEDs exact same time
    .stepGroup("RING_OUTER", true, 300)                 // Named group simultaneous
    .stepAll(false, 100)                                // All OFF together
```

#### JSON Flexibility
```json
{
  "pins": "P027 P127 P227 P327",
  "state": 1,
  "duration": 200,
  "simultaneous": true
}
```

This comprehensive simultaneous control system solves the exact use case you mentioned - **multiple LEDs ON together, OFF together, ALL ON, etc.** - with both programmatic and JSON-based control methods! 🎯