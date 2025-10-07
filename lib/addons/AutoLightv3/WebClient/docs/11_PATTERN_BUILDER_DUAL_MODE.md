# Pattern Builder Dual Mode Implementation

## Overview

AutoLight V3 WebClient now features a dual-mode Pattern Builder that provides two distinct methods for creating LED patterns:

1. **Visual Builder Mode**: Interactive grid-based pattern creation (existing functionality)
2. **C++ Simulator Mode**: Direct C++ code editing with real-time LED simulation (new feature)

## Architecture

### Shared Foundation (Step 1-2)
Both modes share the same initial configuration steps:
- **Step 1**: Configure Grid Layout - Set dimensions, channel count, layout type
- **Step 2**: Map Channels - Assign channel numbers to grid positions

### Mode Selection (Step 3)
Users can switch between modes using a dropdown selector:
- **Visual Builder**: Traditional grid-based pattern creation
- **C++ Simulator**: Direct C++ code editing with live simulation

## Visual Builder Mode (Existing)

### Features
- Interactive LED grid with channel toggle functionality
- Frame-by-frame pattern creation
- Real-time pattern preview and playback
- Code generation (Modern/Native C++ styles)
- Pattern library management

### Workflow
1. Toggle LED channels in the visual grid
2. Create multiple frames with custom delays
3. Preview pattern with frame timeline
4. Generate C++ code for AutoLight integration

## C++ Simulator Mode (New)

### Features
- Direct C++ code editor with syntax highlighting
- Real-time parsing and simulation
- Live LED visualization
- Support for complex AutoLight patterns
- Automatic frame generation from C++ code

### Supported C++ Syntax

#### LED Control Commands
```cpp
set(config_data_ptr_->header.pin_ptr_[index], HIGH);  // Turn ON channel
set(config_data_ptr_->header.pin_ptr_[index], LOW);   // Turn OFF channel
sleep(channel_data_.delay_time_);                     // Delay
off();                                                // All channels OFF
```

#### Control Flow
```cpp
// Forward loops
for (int i = 0; i < config_data_ptr_->header.pin_size_; i++)

// Custom step loops
for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i += 2)

// Reverse loops
for (int i = config_data_ptr_->header.pin_size_; i > 0; --i)

// Nested loops
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 5; j++) {
        // Pattern logic
    }
}
```

#### Complex Channel Expressions
```cpp
// Simple index
set(config_data_ptr_->header.pin_ptr_[i], HIGH);

// Arithmetic expressions
set(config_data_ptr_->header.pin_ptr_[i + 1], HIGH);
set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], HIGH);
set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
```

#### Variables Support
- `config_data_ptr_->header.pin_size_`: Total channel count (from grid configuration)
- `channel_data_.delay_time_`: Default delay (from pattern settings)

### Parser Implementation

#### Core Components
- **CppParser**: Parses C++ code into command objects
- **SimulationEngine**: Executes commands and generates LED frames
- **ExpressionEvaluator**: Handles complex channel expressions
- **LoopStack**: Manages nested loop execution

#### Advanced Features
- **Complex Expression Evaluation**: Arithmetic operations in channel indices
- **Nested Loop Support**: Multiple levels of for loops
- **Direction Handling**: Forward and reverse loop iterations
- **Step Support**: Custom increment values (+=2, +=3, etc.)
- **Variable Substitution**: Replace loop variables and constants

## Implementation Details

### File Structure
```
components/simulation/
├── modes/
│   ├── VisualPatternMode.tsx      # Extracted visual mode
│   └── CppSimulatorMode.tsx       # New C++ simulator mode
├── SimulationContent.tsx          # Updated with mode switcher
└── grid/DynamicPatternGrid.tsx    # Shared LED visualization

lib/simulation/
└── cppParser.ts                   # C++ parsing and simulation engine

lib/contexts/
└── SimulationContext.tsx          # Extended with C++ mode support
```

### Key Components

#### SimulationContext Extensions
```typescript
interface SimulationData {
  // ... existing properties
  patternMode: 'visual' | 'cpp'      // Mode selection
  cppCode: string                    // C++ code storage
}

// New context functions
setPatternMode: (mode: 'visual' | 'cpp') => void
setCppCode: (code: string) => void
generateFramesFromCpp: (code: string) => void
```

#### C++ Parser Interface
```typescript
interface ParsedCommand {
  type: 'set' | 'sleep' | 'off' | 'for_start' | 'for_end'
  channel?: number
  channelExpression?: string         // Complex expressions
  state?: boolean
  delay?: number
  loopCount?: number
  loopVariable?: string
  loopStep?: number                  // Custom increments
  loopDirection?: 'forward' | 'reverse'
}
```

## Usage Examples

### Simple All Blink Pattern
```cpp
void BaseChannel::taskSequenceSimple() {
    for (int i = 0; i < 3; i++) {
        for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
            set(config_data_ptr_->header.pin_ptr_[k], HIGH);
        }
        sleep(channel_data_.delay_time_);

        for (int k = 0; k < config_data_ptr_->header.pin_size_; k++) {
            set(config_data_ptr_->header.pin_ptr_[k], LOW);
        }
        sleep(channel_data_.delay_time_);
    }
    off();
}
```

### Complex Two-Point Fill Pattern
```cpp
void BaseChannel::taskSequenceComplex() {
    for (int i = 0; i < config_data_ptr_->header.pin_size_ / 2; i += 2) {
        for (int j = 0; j < 5; j++) {
            set(config_data_ptr_->header.pin_ptr_[i], HIGH);
            set(config_data_ptr_->header.pin_ptr_[i + 1], HIGH);
            sleep(channel_data_.delay_time_);
            set(config_data_ptr_->header.pin_ptr_[i], LOW);
            set(config_data_ptr_->header.pin_ptr_[i + 1], LOW);
            sleep(channel_data_.delay_time_);
        }

        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], HIGH);
        sleep(300);
        set(config_data_ptr_->header.pin_ptr_[(config_data_ptr_->header.pin_size_ - 1) - i], LOW);
    }

    for (int i = config_data_ptr_->header.pin_size_; i > 0; --i) {
        set(config_data_ptr_->header.pin_ptr_[i - 1], HIGH);
        sleep(channel_data_.delay_time_ * 2);
        set(config_data_ptr_->header.pin_ptr_[i - 1], LOW);
    }
    off();
}
```

## User Experience

### Mode Switching
- Seamless switching between Visual and C++ modes
- Shared grid configuration and channel mapping
- Real-time updates in both directions

### Visual Mode Benefits
- Beginner-friendly interface
- Precise frame-by-frame control
- Easy pattern library management
- No coding required

### C++ Mode Benefits
- Advanced pattern creation capabilities
- Direct AutoLight V3 syntax compatibility
- Complex algorithmic patterns
- Professional development workflow

## Technical Features

### Real-time Simulation
- Live parsing on code changes (debounced)
- Instant LED grid updates
- Frame timeline generation
- Error handling and validation

### Performance Optimizations
- Efficient parser with regex-based matching
- Optimized simulation engine
- Debounced real-time updates
- Memory-efficient frame generation

### Error Handling
- Parse error detection and reporting
- Infinite loop protection
- Graceful fallbacks for invalid expressions
- User-friendly error messages

## Integration with AutoLight V3

### Code Generation Compatibility
- Both modes generate compatible C++ code
- Standard AutoLight V3 function signatures
- BaseChannelSequence.cpp integration ready
- Modern and Native API styles supported

### Hardware Abstraction
- Dynamic channel count support
- PCF8574 distribution compatibility
- Platform-agnostic LED control
- Real hardware simulation accuracy

## Development Notes

### Current Status
- ✅ Dual mode system implemented
- ✅ C++ parser with complex expression support
- ✅ Real-time simulation and visualization
- ✅ Mode switcher in Step 3 integration
- ✅ Shared component reuse architecture

### Future Enhancements
- Monaco Editor integration for advanced syntax highlighting
- IntelliSense and autocomplete support
- Pattern template library
- Export/import functionality between modes
- Advanced debugging features

## Conclusion

The Pattern Builder dual mode system provides the best of both worlds:
- **Accessibility**: Visual mode for beginners and quick prototyping
- **Power**: C++ mode for advanced users and complex patterns
- **Compatibility**: Both modes generate production-ready AutoLight V3 code
- **Flexibility**: Seamless switching based on user preference and task complexity

This implementation significantly enhances the AutoLight V3 development experience while maintaining backward compatibility and ease of use.