# Universal LED Pattern Generator untuk AutoLight V3 - Best Practices

## **Research Findings dari Internet**

Berdasarkan analysis dari berbagai sumber (FastLED, Arduino LED chasers, ESP32 pattern generators, QuickPatterns library), saya menemukan pattern-pattern yang paling umum dan efektif:

### **Top LED Pattern Categories:**

#### **1. Sequential Patterns**
- **Left-to-Right Scan**: LEDs nyala satu per satu dari kiri ke kanan
- **Right-to-Left Scan**: Kebalikan dari left-to-right
- **Dual Chase**: Dua "runner" bergerak berlawanan arah
- **Theater Chase**: Pattern seperti lampu bioskop (3 LED bergerak bersama)

#### **2. Fill Patterns**
- **Fill In**: LEDs nyala dari ujung menuju tengah
- **Fill Out**: LEDs nyala dari tengah menuju ujung
- **Wave Fill**: Gelombang mengisi dan mengosongkan
- **Random Fill**: Pengisian acak dengan algoritma

#### **3. Blink Patterns**
- **All Blink**: Semua LED blink bersamaan
- **Alternate Blink**: Bergantian (ganjil/genap)
- **Half Split**: Setengah pertama vs setengah kedua
- **Random Blink**: Blink pattern acak

#### **4. Wave/Bounce Patterns**
- **Snake**: Pattern bergerak seperti ular
- **Bounce**: Bola memantul dari ujung ke ujung
- **Wave**: Gelombang sine/triangle across channels
- **Ripple**: Efek air mengalir

## **Architecture Berdasarkan Best Practices**

### **1. Pattern Definition System**
```typescript
interface UniversalPattern {
  id: string
  name: string
  category: 'sequential' | 'fill' | 'blink' | 'wave' | 'custom'
  algorithm: PatternAlgorithm
  parameters: PatternParameters
  preview: string  // ASCII art preview
}

interface PatternAlgorithm {
  type: 'sequential_scan' | 'fill_center_out' | 'wave_sine' | 'theater_chase' | etc.
  direction: 'forward' | 'reverse' | 'bidirectional' | 'random'
  channelLogic: ChannelOperation[]
}

interface PatternParameters {
  speed: number          // 1-10 (slow to fast)
  cycles: number         // How many times to repeat
  width: number          // How many LEDs active at once (for chase patterns)
  intensity: number      // Brightness level (for future PWM support)
  spacing: number        // Gap between active LEDs
}
```

### **2. Universal Algorithm Implementation**

**Berdasarkan FastLED dan Arduino patterns, ada 3 approach utama:**

#### **A. Index-Based Patterns**
```typescript
// Sequential scan (like Arduino LED chaser)
function generateSequentialScan(totalChannels: number, parameters: PatternParameters): LEDFrame[] {
  const frames: LEDFrame[] = []
  const speed = 11 - parameters.speed  // Convert 1-10 to 10-1 for delay

  for (let cycle = 0; cycle < parameters.cycles; cycle++) {
    // Forward scan
    for (let i = 0; i < totalChannels; i++) {
      const channels = new Array(totalChannels).fill(false)
      for (let w = 0; w < parameters.width; w++) {
        if (i + w < totalChannels) channels[i + w] = true
      }
      frames.push({
        id: crypto.randomUUID(),
        channels,
        delay: speed * 10,
        label: `Scan Forward ${i + 1}`
      })
    }

    // Reverse scan (if bidirectional)
    if (parameters.direction === 'bidirectional') {
      for (let i = totalChannels - 1; i >= 0; i--) {
        const channels = new Array(totalChannels).fill(false)
        for (let w = 0; w < parameters.width; w++) {
          if (i - w >= 0) channels[i - w] = true
        }
        frames.push({
          id: crypto.randomUUID(),
          channels,
          delay: speed * 10,
          label: `Scan Reverse ${i + 1}`
        })
      }
    }
  }

  return frames
}
```

#### **B. Mathematical Wave Patterns**
```typescript
// Wave pattern (like WLED effects)
function generateWavePattern(totalChannels: number, parameters: PatternParameters): LEDFrame[] {
  const frames: LEDFrame[] = []
  const steps = 30  // Animation steps for smooth wave

  for (let step = 0; step < steps; step++) {
    const channels = new Array(totalChannels).fill(false)

    for (let i = 0; i < totalChannels; i++) {
      // Sine wave calculation
      const phase = (step / steps) * 2 * Math.PI
      const waveValue = Math.sin(phase + (i / totalChannels) * 2 * Math.PI * parameters.width)
      channels[i] = waveValue > 0.5  // Threshold for ON/OFF
    }

    frames.push({
      id: crypto.randomUUID(),
      channels,
      delay: (11 - parameters.speed) * 5,
      label: `Wave Step ${step + 1}`
    })
  }

  return frames
}
```

#### **C. Fill/Center-Based Patterns**
```typescript
// Center-out fill (like automotive turn signals)
function generateCenterOutFill(totalChannels: number, parameters: PatternParameters): LEDFrame[] {
  const frames: LEDFrame[] = []
  const center = Math.floor(totalChannels / 2)
  const maxDistance = Math.ceil(totalChannels / 2)

  for (let distance = 0; distance <= maxDistance; distance++) {
    const channels = new Array(totalChannels).fill(false)

    // Fill symmetrically from center
    for (let d = 0; d <= distance; d++) {
      if (center - d >= 0) channels[center - d] = true
      if (center + d < totalChannels) channels[center + d] = true
    }

    frames.push({
      id: crypto.randomUUID(),
      channels,
      delay: (11 - parameters.speed) * 15,
      label: `Fill Out ${distance + 1}`
    })
  }

  return frames
}
```

### **3. Pattern Library Implementation**

#### **Pre-defined Pattern Templates**
```typescript
const UNIVERSAL_PATTERNS = [
  {
    id: 'sequential_lr',
    name: 'Sequential Left→Right',
    category: 'sequential',
    description: 'Classic LED chaser from left to right',
    algorithm: 'sequential_scan',
    defaultParams: { speed: 5, cycles: 3, width: 1, spacing: 1 },
    preview: '●○○○ → ○●○○ → ○○●○ → ○○○●'
  },
  {
    id: 'theater_chase',
    name: 'Theater Chase',
    category: 'sequential',
    description: 'Three LEDs chase like old theater marquees',
    algorithm: 'theater_chase',
    defaultParams: { speed: 6, cycles: 5, width: 3, spacing: 3 },
    preview: '●●●○○○ → ○●●●○○ → ○○●●●○'
  },
  {
    id: 'center_out_fill',
    name: 'Center Out Fill',
    category: 'fill',
    description: 'Fill from center outward like turn signal',
    algorithm: 'center_out_fill',
    defaultParams: { speed: 4, cycles: 2, width: 1, spacing: 1 },
    preview: '○○●○○ → ○●●●○ → ●●●●●'
  },
  {
    id: 'wave_sine',
    name: 'Sine Wave',
    category: 'wave',
    description: 'Smooth sine wave flowing across channels',
    algorithm: 'wave_sine',
    defaultParams: { speed: 7, cycles: 3, width: 2, spacing: 1 },
    preview: '●○○○○ → ○●○○○ → ○○●○○ → ○○○●○'
  },
  {
    id: 'dual_bounce',
    name: 'Dual Bounce',
    category: 'wave',
    description: 'Two dots bouncing in opposite directions',
    algorithm: 'dual_bounce',
    defaultParams: { speed: 6, cycles: 4, width: 1, spacing: 1 },
    preview: '●○○○● → ○●○●○ → ○○●○○ → ○●○●○'
  },
  {
    id: 'split_blink',
    name: 'Split Half Blink',
    category: 'blink',
    description: 'First half and second half alternate blinking',
    algorithm: 'split_blink',
    defaultParams: { speed: 5, cycles: 6, width: 1, spacing: 1 },
    preview: '●●●○○○ → ○○○●●● → ●●●○○○'
  }
]
```

### **4. UI/UX Implementation**

#### **Pattern Selection Interface**
- **Category Tabs**: Sequential | Fill | Blink | Wave | Custom
- **Pattern Gallery**: Grid view dengan preview animations
- **Live Preview**: Real-time simulation dengan multiple channel counts (8, 12, 24, 48)
- **Parameter Sliders**: Speed, Cycles, Width, Spacing dengan live update

#### **Multi-Channel Preview**
```typescript
// Show how pattern adapts to different channel counts
const previewChannelCounts = [8, 12, 24, 48]
const selectedPattern = 'sequential_lr'

previewChannelCounts.map(count =>
  <PatternPreview
    channelCount={count}
    pattern={generatePattern(selectedPattern, count, parameters)}
    label={`${count} Channels`}
  />
)
```

## **Implementation Strategy**

### **Phase 1: Core Pattern Engine**
1. Buat pattern algorithm functions (6 basic patterns)
2. Implement pattern parameter system
3. Add pattern preview generator

### **Phase 2: UI Integration**
1. Add "Universal Pattern" mode di Pattern Builder
2. Create pattern selection gallery
3. Implement live parameter controls

### **Phase 3: Code Generation**
1. Generate C++ code yang compatible dengan BaseChannelSequence.cpp
2. Support untuk dynamic channel count (`config_data_ptr_->header.pin_size_`)
3. Export functionality

### **Phase 4: Advanced Features**
1. Custom pattern builder (drag-drop blocks)
2. Pattern morphing/blending
3. Import/export pattern library

## **Expected Benefits**

1. **Industry Standard**: Mengikuti pattern yang sudah proven di FastLED, WLED, dll
2. **Universal Compatibility**: Works dengan 8 channel sampai 64+ channel
3. **Professional Output**: Generate production-ready C++ code
4. **User Friendly**: Visual pattern selection dengan live preview
5. **Educational**: Users belajar pattern algorithm fundamentals

## **Example Generated Code**
```cpp
void BaseChannel::taskSequence16SequentialScan() {
    const int total_channels = config_data_ptr_->header.pin_size_;
    const int speed_delay = 50;  // 50ms for speed level 5
    const int cycles = 3;
    const int chase_width = 1;

    for (int cycle = 0; cycle < cycles; cycle++) {
        // Forward scan
        for (int i = 0; i < total_channels; i++) {
            // Turn on chase_width LEDs starting at position i
            for (int w = 0; w < chase_width && (i + w) < total_channels; w++) {
                set(config_data_ptr_->header.pin_ptr_[i + w], HIGH);
            }
            sleep(speed_delay);

            // Turn off previous LEDs
            for (int w = 0; w < chase_width && (i + w) < total_channels; w++) {
                set(config_data_ptr_->header.pin_ptr_[i + w], LOW);
            }
        }

        // Optional: Add reverse scan for bidirectional
        // ... reverse scan code
    }

    off();  // Ensure all LEDs are off at end
}
```

## **Research Sources**

Pattern ini berdasarkan research dari:
- **FastLED Library**: Industry standard untuk addressable LEDs
- **Arduino LED Chasers**: Classic sequential pattern implementations
- **ESP32 Pattern Generators**: Modern microcontroller pattern approaches
- **QuickPatterns Library**: Advanced layered pattern system
- **WLED Project**: Professional LED control system dengan 100+ effects
- **Theater Chase Patterns**: Classic marquee lighting effects
- **Automotive LED Patterns**: Turn signal dan hazard light algorithms

Pattern ini akan mengikuti best practices dari industry dan memberikan user experience yang professional untuk membuat universal LED patterns!