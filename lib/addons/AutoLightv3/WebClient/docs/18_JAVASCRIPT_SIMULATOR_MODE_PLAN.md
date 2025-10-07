# 12_JAVASCRIPT_SIMULATOR_MODE_PLAN.md

## JavaScript Simulator Mode - Complete Implementation Plan

### **Project Overview**

Mode ke-4 untuk Pattern Builder AutoLight V3 WebClient yang mengimplementasikan JavaScript Simulator. Mode ini menyediakan JavaScript interface yang exact 1:1 mapping dengan BaseChannelSequence.cpp, memungkinkan developer menulis LED patterns menggunakan JavaScript syntax yang familiar.

### **Target Architecture: 4 Mode Pattern Builder**

1. **Visual Builder** - Grid interaktif untuk pattern creation
2. **C++ Simulator** - C++ code editor dengan BaseChannelSequence.cpp compatibility
3. **Expression Builder** - Mathematical expressions untuk pattern generation
4. **JavaScript Simulator (NEW)** - JavaScript code editor dengan exact mapping dari C++ methods

---

## **Requirements & Specifications**

### **Core Requirements** (Based on User Input)

#### **1. JavaScript Execution Context**
- **Single Function Editor**: 1 editor = 1 function scope (sama seperti C++ Simulator)
- **No Multiple Functions**: Fokus pada satu pattern function per session
- **Function Wrapper**: Code editor already dalam context function

#### **2. API Access Pattern**
- **Global Functions**: Methods diakses sebagai global functions (bukan class methods)
  ```javascript
  set(channelIndex, state)  // instead of this.set()
  sleep(milliseconds)       // instead of this.sleep()
  ```
- **Dynamic Channel Count**: `totalChannels` sesuai jumlah channel yang dipilih user
- **Configurable Delay**: `delayTime` dapat di-customize

#### **3. Preset Templates**
- **Minimal Implementation**: Hanya 2 method sebagai starting point
  - `taskSequence2BlinkAll()` - Basic blinking pattern
  - `taskSequence3FillTwoPoint()` - Fill pattern dari dua titik
- **No Complex Preset Selector**: Simple implementation

#### **4. Error Handling & Safety**
- **Execution Control**: User dapat stop execution kapan saja
- **Timeout Mechanism**: Prevent infinite loops dengan automatic timeout
- **Performance Monitoring**: Track dan warn untuk slow execution

#### **5. Compatibility & Output**
- **Preview Only**: Results hanya untuk preview dan testing
- **No C++ Export**: Tidak perlu convert ke C++ code
- **Focus on Development**: Tool untuk experimenting dengan JavaScript patterns

#### **6. UI/UX Design Philosophy**
- **Consistent Interface**: Interface consistency dengan existing modes
- **Best Practice Implementation**: Follow established patterns dari C++ Simulator

---

## **JavaScript API Design**

### **Core Global Functions**

```javascript
// LED Control Functions - Exact mapping dari BaseChannelSequence.cpp
set(channelIndex, state)              // set(config_data_ptr_->header.pin_ptr_[k], HIGH/LOW)
sleep(milliseconds)                   // sleep(channel_data_.delay_time_)
off()                                // Turn all channels OFF
on()                                 // Turn all channels ON

// Arduino-compatible Utility Functions
map(value, fromLow, fromHigh, toLow, toHigh)  // Arduino map() function
random(min, max)                     // Random number generation
constrain(value, min, max)           // Constrain value within bounds

// Advanced Control Functions
setChannels(indices, state)          // Bulk set multiple channels
getChannelState(index)               // Get current state of channel
getAllChannelStates()                // Get array of all channel states
```

### **Global Variables**

```javascript
// Configuration Variables
const totalChannels = 24;           // Dynamic based on user selection (8-64)
const delayTime = 50;               // Default delay time (customizable)

// State Variables
const channels = Array(totalChannels).fill(false);  // Current channel states
const halfSize = Math.floor(totalChannels / 2);     // Half size calculation

// Hardware Mapping Variables (for exact C++ compatibility)
const pin_size_ = totalChannels;                     // config_data_ptr_->header.pin_size_
const delay_time_ = delayTime;                       // channel_data_.delay_time_
const pin_ptr_ = Array.from({length: totalChannels}, (_, i) => i);  // pin mapping array
```

---

## **Exact 1:1 Mapping Examples**

### **Example 1: taskSequence2BlinkAll**

#### **C++ Original (BaseChannelSequence.cpp)**
```cpp
void BaseChannel::taskSequence2BlinkAll() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 15; ++j) {
            for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
                set(config_data_ptr_->header.pin_ptr_[k], HIGH);
            }
            sleep(channel_data_.delay_time_);
            for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
                set(config_data_ptr_->header.pin_ptr_[k], LOW);
            }
            sleep(channel_data_.delay_time_);
        }
        sleep(500);
    }
    off();
}
```

#### **JavaScript Exact Mapping**
```javascript
// User code dalam JavaScript Simulator editor:
for (let i = 0; i < 2; i++) {
    for (let j = 0; j < 15; j++) {
        for (let k = 0; k < pin_size_; k++) {
            set(pin_ptr_[k], true);  // HIGH = true
        }
        sleep(delay_time_);
        for (let k = 0; k < pin_size_; k++) {
            set(pin_ptr_[k], false); // LOW = false
        }
        sleep(delay_time_);
    }
    sleep(500);
}
off();
```

### **Example 2: taskSequence3FillTwoPoint**

#### **C++ Pattern Analysis**
- Fill dari tengah ke ujung
- Menggunakan half-size calculations
- Complex indexing patterns

#### **JavaScript Implementation**
```javascript
// Fill two point pattern - exact logic mapping
const halfSize = Math.floor(pin_size_ / 2);

for (let i = 0; i < halfSize; i++) {
    // Fill from center outward
    set(pin_ptr_[halfSize + i], true);
    set(pin_ptr_[(halfSize - 1) - i], true);
    sleep(delay_time_ * 2);
}

sleep(500);

for (let i = 0; i < halfSize; i++) {
    // Clear from center outward
    set(pin_ptr_[halfSize + i], false);
    set(pin_ptr_[(halfSize - 1) - i], false);
    sleep(delay_time_ * 2);
}
off();
```

---

## **Implementation Architecture**

### **File Structure**

```
WebClient/
├── components/simulation/modes/
│   └── JavaScriptSimulatorMode.tsx           # Main JavaScript mode component
├── lib/simulation/
│   ├── javascriptEngine.ts                   # JavaScript execution engine
│   ├── javascriptAPI.ts                      # Global API functions implementation
│   └── javascriptPresets.ts                  # Preset templates (2 basic patterns)
├── lib/contexts/
│   └── SimulationContext.tsx                 # Update untuk JavaScript mode support
└── lib/utils/
    └── javascriptSafety.ts                   # Safety utilities dan timeout handling
```

### **Core Components Design**

#### **1. JavaScriptSimulatorMode.tsx**
```typescript
interface JavaScriptSimulatorProps {
    // Same interface pattern sebagai CppSimulatorMode
    onPatternGenerated: (frames: LEDFrame[]) => void;
    totalChannels: number;
    delayTime: number;
}

export default function JavaScriptSimulatorMode() {
    // Features:
    // - JavaScript code editor dengan syntax highlighting
    // - Simple preset selector (2 patterns)
    // - Live execution controls (Play/Pause/Stop/Reset)
    // - Real-time LED preview
    // - Error display dan handling
    // - Execution statistics
}
```

#### **2. JavaScriptEngine.ts**
```typescript
class JavaScriptEngine {
    private frames: LEDFrame[] = [];
    private currentChannels: boolean[] = [];
    private isExecuting: boolean = false;
    private executionTimeout: NodeJS.Timeout | null = null;

    // Core execution methods
    executeCode(code: string, totalChannels: number, delayTime: number): Promise<LEDFrame[]>
    stopExecution(): void

    // API function implementations
    private createGlobalAPI(totalChannels: number, delayTime: number): GlobalAPI
    private captureFrame(delay: number): void
    private validateExecution(): boolean
}
```

#### **3. JavaScriptAPI.ts**
```typescript
interface GlobalAPI {
    // Core LED functions
    set: (index: number, state: boolean) => void;
    sleep: (ms: number) => void;
    off: () => void;
    on: () => void;

    // Utility functions
    map: (value: number, fromLow: number, fromHigh: number, toLow: number, toHigh: number) => number;
    random: (min: number, max: number) => number;
    constrain: (value: number, min: number, max: number) => number;

    // Global variables
    totalChannels: number;
    delayTime: number;
    channels: boolean[];
    pin_size_: number;
    delay_time_: number;
    pin_ptr_: number[];
}
```

---

## **Safety & Error Handling**

### **Execution Safety Measures**

#### **1. Timeout Protection**
```typescript
const EXECUTION_TIMEOUT = 30000; // 30 second maximum execution time

class ExecutionSafety {
    private timeout: NodeJS.Timeout;

    startExecution(callback: () => void) {
        this.timeout = setTimeout(() => {
            throw new Error('Execution timeout: Pattern took too long to execute');
        }, EXECUTION_TIMEOUT);

        try {
            callback();
        } finally {
            clearTimeout(this.timeout);
        }
    }
}
```

#### **2. Performance Monitoring**
```typescript
interface ExecutionStats {
    executionTime: number;
    frameCount: number;
    memoryUsage: number;
    warningMessages: string[];
}

class PerformanceMonitor {
    trackExecution(code: string): ExecutionStats
    validatePerformance(stats: ExecutionStats): boolean
    generateWarnings(stats: ExecutionStats): string[]
}
```

#### **3. Error Handling Strategy**
- **Syntax Errors**: Detect dan display sebelum execution
- **Runtime Errors**: Try-catch wrapper dengan user-friendly messages
- **Infinite Loops**: Timeout mechanism dengan clear error messages
- **Memory Issues**: Monitor memory usage dan warn user

---

## **User Experience Flow**

### **Step-by-Step User Journey**

#### **1. Mode Selection**
- User selects "JavaScript Simulator" dari mode dropdown
- Interface loads dengan JavaScript code editor
- Default template atau blank editor ready

#### **2. Code Writing**
```javascript
// User writes JavaScript code:
// Simple blink pattern
for (let i = 0; i < 5; i++) {
    on();           // Turn all channels ON
    sleep(200);     // Wait 200ms
    off();          // Turn all channels OFF
    sleep(200);     // Wait 200ms
}
```

#### **3. Execution & Preview**
- User clicks "Run" button
- JavaScript engine executes code dengan safety wrapper
- Real-time LED pattern preview di grid
- Frame timeline shows generated sequence

#### **4. Control & Debug**
- Play/Pause/Stop controls untuk pattern review
- Error messages jika ada issues
- Execution stats (time, frames, performance)
- Reset button untuk clear results

---

## **Integration Points**

### **SimulationContext Updates**

#### **State Management Extension**
```typescript
interface SimulationData {
    // Existing properties...
    patternMode: 'visual' | 'cpp' | 'expression' | 'javascript'; // Add javascript
    javascriptCode: string;                    // JavaScript code state
    javascriptFrames: LEDFrame[];              // Generated frames
    javascriptExecuting: boolean;              // Execution state
    javascriptError: string | null;           // Error state
}

interface SimulationActions {
    // Existing actions...
    setJavaScriptCode: (code: string) => void;
    executeJavaScript: () => Promise<void>;
    stopJavaScriptExecution: () => void;
    clearJavaScriptFrames: () => void;
}
```

### **Mode Selector Updates**

#### **SimulationContent.tsx Changes**
```typescript
// Add JavaScript option to mode selector
<SelectItem value="javascript">
    <div className="flex items-center gap-2">
        <Code className="h-3 w-3" />  {/* atau icon khusus JavaScript */}
        JavaScript Simulator
    </div>
</SelectItem>

// Add conditional rendering
{simulationData.patternMode === 'visual' ? (
    <VisualPatternMode />
) : simulationData.patternMode === 'cpp' ? (
    <CppSimulatorMode />
) : simulationData.patternMode === 'expression' ? (
    <ExpressionBuilderMode />
) : (
    <JavaScriptSimulatorMode />  // New JavaScript mode
)}
```

---

## **Technical Specifications**

### **Performance Requirements**

#### **Execution Performance**
- **Max Execution Time**: 30 seconds timeout
- **Frame Generation**: Support untuk 1000+ frames
- **Memory Usage**: Efficient array management
- **Real-time Preview**: Smooth 60fps preview capability

#### **Code Editor Features**
- **Syntax Highlighting**: JavaScript/TypeScript syntax
- **Error Detection**: Real-time syntax validation
- **Auto-completion**: Basic JavaScript API completion
- **Code Formatting**: Optional code beautification

### **Browser Compatibility**
- **Modern JavaScript**: ES6+ features support
- **Browser Support**: Chrome 90+, Firefox 88+, Safari 14+
- **Performance**: Optimized untuk smooth real-time execution
- **Memory Management**: Efficient garbage collection

---

## **Development Phases**

### **Phase 1: Core Infrastructure**
- [ ] Create JavaScriptEngine class
- [ ] Implement basic Global API functions
- [ ] Create JavaScriptSimulatorMode component
- [ ] Update SimulationContext untuk JavaScript support

### **Phase 2: Safety & Error Handling**
- [ ] Implement execution timeout mechanism
- [ ] Add performance monitoring
- [ ] Create error handling system
- [ ] Add execution controls (Stop/Reset)

### **Phase 3: User Interface**
- [ ] JavaScript code editor dengan syntax highlighting
- [ ] Integration dengan existing Pattern Builder UI
- [ ] Add preset templates (2 basic patterns)
- [ ] Real-time preview dan frame generation

### **Phase 4: Testing & Optimization**
- [ ] Performance testing dengan complex patterns
- [ ] Error handling testing (infinite loops, syntax errors)
- [ ] User experience testing
- [ ] Code optimization dan refactoring

### **Phase 5: Documentation & Polish**
- [ ] User documentation untuk JavaScript API
- [ ] Code comments dan technical documentation
- [ ] UI polish dan accessibility improvements
- [ ] Final integration testing

---

## **Success Metrics**

### **Technical Metrics**
- **Execution Speed**: JavaScript patterns execute dalam <1s untuk simple patterns
- **Error Rate**: <5% runtime errors dengan proper error messages
- **Performance**: Smooth real-time preview tanpa lag
- **Compatibility**: Work across all supported browsers

### **User Experience Metrics**
- **Learning Curve**: Users dapat create basic patterns dalam <10 minutes
- **Feature Adoption**: JavaScript mode usage rate vs other modes
- **Error Recovery**: Users dapat recover dari errors dengan clear guidance
- **Pattern Complexity**: Support untuk patterns dengan 100+ frames

---

## **Future Enhancements (Post-MVP)**

### **Advanced Features**
- **TypeScript Support**: Full TypeScript syntax support dengan type checking
- **Code Templates**: More preset templates beyond basic 2 patterns
- **Pattern Library**: Save/load custom JavaScript patterns
- **Advanced Debugging**: Breakpoints dan step-by-step execution
- **Performance Profiler**: Detailed performance analysis tools

### **Integration Enhancements**
- **C++ Export**: Convert JavaScript patterns ke C++ code
- **Pattern Sharing**: Share JavaScript patterns between users
- **Version Control**: Pattern history dan versioning
- **Collaboration**: Multi-user pattern development

---

## **Conclusion**

JavaScript Simulator Mode akan menjadi valuable addition untuk Pattern Builder, providing familiar development environment untuk web developers sambil maintaining exact compatibility dengan BaseChannelSequence.cpp logic. Implementation plan ini provides comprehensive roadmap untuk successful development dan deployment.

Key benefits:
- **Developer Friendly**: JavaScript syntax familiar untuk web developers
- **Exact Mapping**: 1:1 compatibility dengan BaseChannelSequence.cpp
- **Safe Execution**: Comprehensive error handling dan performance monitoring
- **Efficient Development**: Real-time execution tanpa complex parsing
- **Seamless Integration**: Consistent dengan existing Pattern Builder modes

Implementation dapat dimulai dengan Phase 1 dan incrementally developed untuk complete solution.