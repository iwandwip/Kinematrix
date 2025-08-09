# Pattern Engine - Usage Guide

Simple yet powerful pattern creation system for AutoLight V3 sequences.

## Overview

The Pattern Engine consists of three main classes:
- **PatternHelper**: Direct pattern utilities
- **ChainHelper**: Fluent/chaining API
- **SimplePatterns**: Pre-built pattern templates

## Quick Start

### Basic Setup

```cpp
void BaseChannel::taskSequenceExample() {
    PatternHelper h(this);  // Initialize helper
    
    // Your patterns here...
}
```

## PatternHelper Class

Core utilities for LED pattern creation.

### Basic Channel Control

```cpp
PatternHelper h(this);

// Set individual channels
h.set(0, HIGH);           // Turn on channel 0
h.set(5, LOW);            // Turn off channel 5

// Set ranges
h.setRange(0, 5, HIGH);   // Turn on channels 0-5
h.setRange(2, 8, LOW);    // Turn off channels 2-8

// All channels
h.allOn();                // Turn on all channels
h.allOff();               // Turn off all channels

// Timing
h.delay();                // Default delay
h.delay(500);             // Custom delay (500ms)
```

### Symmetric Patterns

```cpp
// Symmetric control (mirrors from edges)
h.setSymmetric(0, HIGH);  // Sets channel 0 and last channel
h.setSymmetric(1, HIGH);  // Sets channel 1 and second-to-last

// Fill from edges to center
h.fillSymmetric();        // Fill inward
h.unfillSymmetric();      // Empty outward
```

### Movement Patterns

```cpp
// Wave patterns
h.wave(0, 7);             // Wave from channel 0 to 7
h.wave(7, 0);             // Wave from channel 7 to 0 (reverse)
h.wave(0, 7, 100);        // Wave with custom speed

// Ripple from center
h.ripple(4);              // Ripple from channel 4
h.ripple(4, 80);          // Ripple with custom speed

// Chase effect
h.chase(3);               // 3-LED chase across all channels
h.chase(2, 150);          // 2-LED chase with custom speed

// Bounce effect
h.bounce(1, 2);           // 1-LED bouncing 2 cycles
h.bounce(3, 1);           // 3-LED ball bouncing once
```

### Utility Functions

```cpp
// Get channel information
int total = h.size();           // Total number of channels
int current_delay = h.getDelay(); // Current delay setting

// Change default delay
h.setDelay(200);               // Set new default delay

// Blink patterns
h.blinkRange(0, 3, 5);         // Blink channels 0-3, 5 times

// Alternating patterns
h.alternate(2, 3);             // Alternate groups of 2, repeat 3 times
```

## ChainHelper Class

Fluent API for chaining pattern commands.

### Basic Chaining

```cpp
PatternHelper h(this);
ChainHelper chain(h);

chain.wave().wait(500).ripple().off();
```

### Available Chain Commands

```cpp
// Movement patterns
chain.wave()                    // Full wave left to right
     .waveReverse()            // Wave right to left
     .wave(2, 6, 100)          // Custom wave from channel 2-6, speed 100
     .ripple()                 // Ripple from center
     .ripple(3, 80);           // Ripple from channel 3, speed 80

// Effects
chain.chase(3)                  // 3-LED chase
     .bounce(2, 1)             // 2-LED bounce, 1 cycle
     .fillSymmetric()          // Fill from edges
     .unfillSymmetric();       // Empty to edges

// Control
chain.blink(3)                  // Blink all channels 3 times
     .blinkRange(0, 5, 2)      // Blink channels 0-5, 2 times
     .alternate(2, 3)          // Alternate groups
     .wait(1000)               // Wait 1 second
     .off()                    // Turn all off
     .on();                    // Turn all on

// Speed control
chain.setSpeed(100)             // Change default delay to 100ms
     .wave()                   // Wave with new speed
     .setSpeed(50)             // Change to 50ms
     .ripple();                // Ripple with new speed
```

### Advanced Chaining

```cpp
// Custom functions in chain
void myCustomPattern(PatternHelper& h) {
    for (int i = 0; i < h.size(); i += 2) {
        h.set(i, HIGH);
    }
    h.delay(500);
    h.allOff();
}

void myRepeatedPattern(ChainHelper& c) {
    c.wave().wait(200).waveReverse().wait(200);
}

chain.wave()
     .custom(myCustomPattern)
     .ripple();

// Repeat patterns
chain.repeat(3, myRepeatedPattern);
```

## SimplePatterns Class

Pre-built pattern templates that auto-scale to any channel count.

### Classic Effects

```cpp
PatternHelper h(this);

// Knight Rider effect
SimplePatterns::knightRider(h);           // Default: 1 cycle
SimplePatterns::knightRider(h, 100, 3);   // Speed 100ms, 3 cycles

// Scanner effect
SimplePatterns::scanner(h);               // Default: 3-LED width, 2 cycles
SimplePatterns::scanner(h, 5, 1);         // 5-LED width, 1 cycle

// Breathing effect
SimplePatterns::breathe(h);               // Default: 3 cycles
SimplePatterns::breathe(h, 5, 30);        // 5 cycles, 30ms fade speed
```

### Dynamic Effects

```cpp
// Random patterns
SimplePatterns::randomBlink(h);           // Default: 5 seconds
SimplePatterns::randomBlink(h, 10000);    // 10 seconds

SimplePatterns::twinkle(h);               // Default: 3 seconds, 3 intensity
SimplePatterns::twinkle(h, 5000, 5);      // 5 seconds, 5 intensity

// Strobe effect
SimplePatterns::strobe(h);                // Default: 10 flashes, 50ms
SimplePatterns::strobe(h, 20, 25);        // 20 flashes, 25ms intervals
```

### Advanced Patterns

```cpp
// Meteor effect
SimplePatterns::meteor(h);                // Default: 5-LED tail, 3 cycles  
SimplePatterns::meteor(h, 8, 2, 120);     // 8-LED tail, 2 cycles, 120ms speed

// Explosion from center
SimplePatterns::explosion(h);             // From center
SimplePatterns::explosion(h, 2, 80);      // From channel 2, 80ms speed

// Rainbow effect
SimplePatterns::rainbow(h);               // Default: 2 cycles, 100ms
SimplePatterns::rainbow(h, 5, 150);       // 5 cycles, 150ms

// Heartbeat
SimplePatterns::heartbeat(h);             // Default: 3 beats
SimplePatterns::heartbeat(h, 5);          // 5 beats

// Police lights
SimplePatterns::police(h);                // Default: 5 cycles
SimplePatterns::police(h, 8);             // 8 cycles

// Spiral effect
SimplePatterns::spiral(h);                // Default: 2 turns
SimplePatterns::spiral(h, 4, 200);        // 4 turns, 200ms speed
```

## Usage Examples

### Simple Sequence

```cpp
void BaseChannel::taskSequence16Simple() {
    PatternHelper h(this);
    
    h.wave(0, h.size() - 1);        // Wave across all channels
    h.delay(500);                   // Wait 500ms
    h.ripple(h.size() / 2);         // Ripple from center
    h.allOff();                     // Turn off all
}
```

### Chain Sequence

```cpp
void BaseChannel::taskSequence17Chain() {
    PatternHelper h(this);
    ChainHelper chain(h);
    
    chain.wave()
         .wait(200)
         .waveReverse()
         .wait(200)
         .ripple()
         .wait(500)
         .blink(3)
         .off();
}
```

### Template Sequence

```cpp
void BaseChannel::taskSequence18Templates() {
    PatternHelper h(this);
    
    SimplePatterns::knightRider(h, 80, 2);
    h.delay(1000);
    SimplePatterns::breathe(h, 3);
    h.delay(500);
    SimplePatterns::explosion(h);
}
```

### Mixed Sequence

```cpp
// Custom pattern function (define outside class)
void customLogic(PatternHelper& h) {
    for (int i = 0; i < h.size(); i += 3) {
        h.setSymmetric(i, HIGH);
        h.delay(100);
    }
    h.delay(1000);
    h.allOff();
}

void BaseChannel::taskSequence19Mixed() {
    PatternHelper h(this);
    ChainHelper chain(h);
    
    // Start with template
    SimplePatterns::scanner(h, 4, 1);
    
    // Chain patterns
    chain.fillSymmetric()
         .wait(500)
         .unfillSymmetric()
         .custom(customLogic);
    
    // End with template
    SimplePatterns::heartbeat(h, 2);
}
```

### Auto-scaling Example

```cpp
void BaseChannel::taskSequenceAutoScale() {
    PatternHelper h(this);
    
    // These patterns automatically work with any channel count
    // (8, 12, 16, 24, 32, 64 channels)
    
    h.fillSymmetric();              // Always fills from edges to center
    h.delay(500);
    
    SimplePatterns::knightRider(h); // Always bounces across full width
    
    h.ripple(h.size() / 2);         // Always ripples from center
    
    SimplePatterns::police(h);      // Always splits into two halves
}
```

## Integration with Existing Code

### Option 1: Helper Only

```cpp
void BaseChannel::taskSequenceHelper() {
    PatternHelper h(this);
    
    // 90% cleaner code
    h.wave(0, h.size() - 1);
    h.ripple(h.size() / 2);
    
    // Mix with custom logic when needed
    for (int i = 0; i < h.size(); i++) {
        if (complexCondition(i)) {
            h.setSymmetric(i, HIGH);
            h.delay(calculateCustomDelay(i));
        }
    }
}
```

### Option 2: Chain Style

```cpp
void myComplexAlgorithm(PatternHelper& h) {
    // Your complex algorithm here
    for (int i = 0; i < h.size(); i++) {
        if (complexCondition(i)) {
            h.setSymmetric(i, HIGH);
            h.delay(50);
        }
    }
}

void BaseChannel::taskSequenceChain() {
    PatternHelper h(this);
    ChainHelper(h)
        .wave().wait(200)
        .custom(myComplexAlgorithm)
        .ripple().off();
}
```

### Option 3: Template Style

```cpp
void BaseChannel::taskSequenceTemplate() {
    PatternHelper h(this);
    
    SimplePatterns::breathe(h, 2);
    SimplePatterns::knightRider(h, 100, 1);
    SimplePatterns::explosion(h);
    
    // Custom ending
    for (int i = 0; i < 5; i++) {
        h.allOn();
        h.delay(100);
        h.allOff();
        h.delay(100);
    }
}
```

## Notes

- All patterns automatically scale to your channel count (8-64)
- PatternHelper provides direct access to existing BaseChannel functionality
- ChainHelper enables fluent/readable pattern composition
- SimplePatterns offers ready-to-use effects
- Mix and match approaches as needed
- Existing sequences remain unchanged
- Zero performance overhead - compiles to same efficiency as manual code