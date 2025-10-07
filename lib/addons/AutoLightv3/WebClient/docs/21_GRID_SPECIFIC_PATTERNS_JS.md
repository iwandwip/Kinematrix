# Grid-Specific LED Patterns Implementation Plan - JavaScript Version

## Overview

Dokumen ini berisi rencana implementasi 10 mode tambahan untuk AutoLight V3 JavaScript Simulator yang dioptimalkan secara spesifik untuk grid layout 4×4 dalam area 8×6. Layout ini berdasarkan screenshot yang menunjukkan 16 channel terdistribusi dalam 4 baris × 4 kolom.

## Grid Layout Configuration

### Channel Mapping (Berdasarkan Screenshot)
```
Grid 8×6 dengan 16 channel aktif:
┌──────────────────────────────────────┐
│ Channel:  0   1   2   3   .   .   . │ Row 0
│ Channel:  4   5   6   7   .   .   . │ Row 1
│ Channel:  8   9  10  11   .   .   . │ Row 2
│ Channel: 12  13  14  15   .   .   . │ Row 3
│          .   .   .   .   .   .   . │ Row 4
│          .   .   .   .   .   .   . │ Row 5
└──────────────────────────────────────┘
    Col 0   1   2   3   4   5   6
```

### Grid Helper Functions
```javascript
// Helper functions untuk grid layout 4x4
function getChannelFromGridPosition(row, col) {
  if (row >= 0 && row < 4 && col >= 0 && col < 4) {
    return row * 4 + col;  // Mapping: [0,0]=0, [0,1]=1, [0,2]=2, [0,3]=3
  }
  return -1;  // Invalid position
}

function activateRow(row, state = true) {
  for (let col = 0; col < 4; col++) {
    const channel = row * 4 + col;
    if (channel < pin_size_) {
      set(pin_ptr_[channel], state);
    }
  }
}

function activateColumn(col, state = true) {
  for (let row = 0; row < 4; row++) {
    const channel = row * 4 + col;
    if (channel < pin_size_) {
      set(pin_ptr_[channel], state);
    }
  }
}

function activateDiagonal(mainDiagonal = true, state = true) {
  if (mainDiagonal) {
    // Diagonal utama: [0,0], [1,1], [2,2], [3,3]
    for (let i = 0; i < 4; i++) {
      const channel = i * 4 + i;
      if (channel < pin_size_) {
        set(pin_ptr_[channel], state);
      }
    }
  } else {
    // Diagonal sekunder: [0,3], [1,2], [2,1], [3,0]
    for (let i = 0; i < 4; i++) {
      const channel = i * 4 + (3 - i);
      if (channel < pin_size_) {
        set(pin_ptr_[channel], state);
      }
    }
  }
}
```

## Pattern Implementations

### 1. Row Wave Pattern
```javascript
// Grid Row Wave Pattern - Gelombang horizontal melintasi 4 baris
{
  id: 'gridRowWave',
  name: 'Grid Row Wave Pattern',
  description: 'Horizontal wave across 4x4 grid layout',
  category: 'intermediate',
  code: `// Row wave for 4x4 grid layout
for (let pass = 0; pass < 3; pass++) {
  for (let row = 0; row < 4; row++) {
    // Activate entire row
    for (let col = 0; col < 4; col++) {
      const channel = row * 4 + col;
      if (channel < pin_size_) {
        set(pin_ptr_[channel], true);
      }
    }
    await sleep(delay_time_ * 2);

    // Deactivate row
    for (let col = 0; col < 4; col++) {
      const channel = row * 4 + col;
      if (channel < pin_size_) {
        set(pin_ptr_[channel], false);
      }
    }
    await sleep(delay_time_);
  }
  await sleep(delay_time_ * 2);
}`
}
```

### 2. Column Fill Pattern
```javascript
// Column Fill Pattern - Pengisian vertikal melintasi 4 kolom
{
  id: 'gridColumnFill',
  name: 'Grid Column Fill Pattern',
  description: 'Vertical fill pattern across 4x4 grid columns',
  category: 'intermediate',
  code: `// Column fill for 4x4 grid layout
for (let col = 0; col < 4; col++) {
  // Fill dari atas ke bawah
  for (let row = 0; row < 4; row++) {
    const channel = row * 4 + col;
    if (channel < pin_size_) {
      set(pin_ptr_[channel], true);
      await sleep(delay_time_);
    }
  }

  await sleep(delay_time_ * 3);

  // Clear dari bawah ke atas
  for (let row = 3; row >= 0; row--) {
    const channel = row * 4 + col;
    if (channel < pin_size_) {
      set(pin_ptr_[channel], false);
      await sleep(delay_time_);
    }
  }
}`
}
```

### 3. Diagonal Cross Pattern
```javascript
// Diagonal Cross Pattern - Pola diagonal silang
{
  id: 'gridDiagonalCross',
  name: 'Grid Diagonal Cross Pattern',
  description: 'Diagonal cross pattern on 4x4 grid',
  category: 'advanced',
  code: `// Diagonal cross for 4x4 grid layout
// Diagonal utama (kiri atas ke kanan bawah)
for (let i = 0; i < 4; i++) {
  const channel = i * 4 + i;  // [0,0], [1,1], [2,2], [3,3]
  if (channel < pin_size_) {
    set(pin_ptr_[channel], true);
    await sleep(delay_time_);
  }
}

await sleep(delay_time_ * 2);

// Diagonal sekunder (kanan atas ke kiri bawah)
for (let i = 0; i < 4; i++) {
  const channel = i * 4 + (3 - i);  // [0,3], [1,2], [2,1], [3,0]
  if (channel < pin_size_) {
    set(pin_ptr_[channel], true);
    await sleep(delay_time_);
  }
}

await sleep(delay_time_ * 3);
off();`
}
```

### 4. Spiral Square Pattern
```javascript
// Spiral Square Pattern - Pola spiral melingkar
{
  id: 'gridSpiralSquare',
  name: 'Grid Spiral Square Pattern',
  description: 'Spiral pattern on 4x4 grid layout',
  category: 'advanced',
  code: `// Spiral square for 4x4 grid layout
const spiralOrder = [
  0, 1, 2, 3,      // Top row
  7, 11, 15, 14,    // Right column + bottom
  13, 12, 9, 8,     // Bottom row + left column
  4, 5, 6, 10       // Inner square
];

for (let i = 0; i < 16; i++) {
  const channel = spiralOrder[i];
  if (channel < pin_size_) {
    set(pin_ptr_[channel], true);
    await sleep(delay_time_);
  }
}

await sleep(delay_time_ * 2);

// Reverse spiral
for (let i = 15; i >= 0; i--) {
  const channel = spiralOrder[i];
  if (channel < pin_size_) {
    set(pin_ptr_[channel], false);
    await sleep(delay_time_);
  }
}`
}
```

### 5. Checkerboard Pattern
```javascript
// Checkerboard Pattern - Pola kotak-kotak classic
{
  id: 'gridCheckerboard',
  name: 'Grid Checkerboard Pattern',
  description: 'Classic checkerboard pattern on 4x4 grid',
  category: 'intermediate',
  code: `// Checkerboard for 4x4 grid layout
for (let cycle = 0; cycle < 4; cycle++) {
  // Black squares (even positions)
  for (let row = 0; row < 4; row++) {
    for (let col = 0; col < 4; col++) {
      if ((row + col) % 2 === 0) {
        const channel = row * 4 + col;
        if (channel < pin_size_) {
          set(pin_ptr_[channel], true);
        }
      }
    }
  }
  await sleep(delay_time_ * 2);
  off();
  await sleep(delay_time_);

  // White squares (odd positions)
  for (let row = 0; row < 4; row++) {
    for (let col = 0; col < 4; col++) {
      if ((row + col) % 2 === 1) {
        const channel = row * 4 + col;
        if (channel < pin_size_) {
          set(pin_ptr_[channel], true);
        }
      }
    }
  }
  await sleep(delay_time_ * 2);
  off();
  await sleep(delay_time_);
}`
}
```

### 6. Frame Pattern
```javascript
// Frame Pattern - Pola bingkai/frame
{
  id: 'gridFrame',
  name: 'Grid Frame Pattern',
  description: 'Frame/border pattern on 4x4 grid',
  category: 'intermediate',
  code: `// Frame pattern for 4x4 grid layout
const frameChannels = [
  0, 1, 2, 3,      // Top edge
  7, 11, 15, 14,    // Right column + bottom
  13, 12, 9, 8,     // Bottom edge + left column
  4                  // Complete the frame
];

for (let cycle = 0; cycle < 3; cycle++) {
  // Draw frame
  for (let i = 0; i < frameChannels.length; i++) {
    const channel = frameChannels[i];
    if (channel < pin_size_) {
      set(pin_ptr_[channel], true);
      await sleep(delay_time_ / 2);
    }
  }

  await sleep(delay_time_ * 2);

  // Fill center
  for (let center = 5; center <= 10; center++) {
    if (center !== 7 && center !== 11) {  // Skip frame positions
      if (center < pin_size_) {
        set(pin_ptr_[center], true);
        await sleep(delay_time_);
      }
    }
  }

  await sleep(delay_time_ * 2);
  off();
}`
}
```

### 7. Snake Matrix Pattern
```javascript
// Snake Matrix Pattern - Pola ular ularan
{
  id: 'gridSnakeMatrix',
  name: 'Grid Snake Matrix Pattern',
  description: 'Snake pattern traversing 4x4 grid matrix',
  category: 'advanced',
  code: `// Snake matrix for 4x4 grid layout
for (let cycle = 0; cycle < 2; cycle++) {
  // Forward snake
  for (let row = 0; row < 4; row++) {
    if (row % 2 === 0) {
      // Kiri ke kanan
      for (let col = 0; col < 4; col++) {
        const channel = row * 4 + col;
        if (channel < pin_size_) {
          set(pin_ptr_[channel], true);
          await sleep(delay_time_);
        }
      }
    } else {
      // Kanan ke kiri
      for (let col = 3; col >= 0; col--) {
        const channel = row * 4 + col;
        if (channel < pin_size_) {
          set(pin_ptr_[channel], true);
          await sleep(delay_time_);
        }
      }
    }
  }

  await sleep(delay_time_ * 2);
  off();

  // Reverse snake
  for (let row = 3; row >= 0; row--) {
    if (row % 2 === 0) {
      // Kiri ke kanan
      for (let col = 0; col < 4; col++) {
        const channel = row * 4 + col;
        if (channel < pin_size_) {
          set(pin_ptr_[channel], true);
          await sleep(delay_time_);
        }
      }
    } else {
      // Kanan ke kiri
      for (let col = 3; col >= 0; col--) {
        const channel = row * 4 + col;
        if (channel < pin_size_) {
          set(pin_ptr_[channel], true);
          await sleep(delay_time_);
        }
      }
    }
  }

  await sleep(delay_time_ * 2);
  off();
}`
}
```

### 8. Cross Pattern
```javascript
// Cross Pattern - Pola cross horizontal dan vertikal
{
  id: 'gridCross',
  name: 'Grid Cross Pattern',
  description: 'Cross pattern (horizontal + vertical) on 4x4 grid',
  category: 'intermediate',
  code: `// Cross pattern for 4x4 grid layout
for (let cycle = 0; cycle < 3; cycle++) {
  // Horizontal cross (baris tengah)
  for (let col = 0; col < 4; col++) {
    const channel1 = 1 * 4 + col;  // Row 1
    const channel2 = 2 * 4 + col;  // Row 2

    if (channel1 < pin_size_) set(pin_ptr_[channel1], true);
    if (channel2 < pin_size_) set(pin_ptr_[channel2], true);
    await sleep(delay_time_);
  }

  await sleep(delay_time_ * 2);

  // Vertical cross (kolom tengah)
  for (let row = 0; row < 4; row++) {
    const channel1 = row * 4 + 1;  // Col 1
    const channel2 = row * 4 + 2;  // Col 2

    if (channel1 < pin_size_) set(pin_ptr_[channel1], true);
    if (channel2 < pin_size_) set(pin_ptr_[channel2], true);
    await sleep(delay_time_);
  }

  await sleep(delay_time_ * 2);
  off();
}`
}
```

### 9. Corner Expansion Pattern
```javascript
// Corner Expansion Pattern - Ekspansi dari corner ke tengah
{
  id: 'gridCornerExpansion',
  name: 'Grid Corner Expansion Pattern',
  description: 'Expansion from corners to center on 4x4 grid',
  category: 'advanced',
  code: `// Corner expansion for 4x4 grid layout
const corners = [0, 3, 12, 15];  // 4 corners
const center = [5, 6, 9, 10];   // Center positions

// Activate corners
for (let i = 0; i < corners.length; i++) {
  if (corners[i] < pin_size_) {
    set(pin_ptr_[corners[i]], true);
  }
}
await sleep(delay_time_ * 2);

// Expand edges
const edges = [1, 2, 4, 7, 8, 11, 13, 14];
for (let i = 0; i < edges.length; i++) {
  if (edges[i] < pin_size_) {
    set(pin_ptr_[edges[i]], true);
    await sleep(delay_time_);
  }
}

await sleep(delay_time_ * 2);

// Fill center
for (let i = 0; i < center.length; i++) {
  if (center[i] < pin_size_) {
    set(pin_ptr_[center[i]], true);
    await sleep(delay_time_);
  }
}

await sleep(delay_time_ * 3);
off();`
}
```

### 10. Random Cluster Pattern
```javascript
// Random Cluster Pattern - Pola cluster acak
{
  id: 'gridRandomCluster',
  name: 'Grid Random Cluster Pattern',
  description: 'Random cluster activation with grid coordination',
  category: 'intermediate',
  code: `// Random cluster for 4x4 grid layout
for (let cycle = 0; cycle < 8; cycle++) {
  // Pilih baris acak
  const randomRow = Math.floor(Math.random() * 4);

  // Aktifkan cluster di baris tersebut
  for (let col = 0; col < 4; col++) {
    const channel = randomRow * 4 + col;
    if (channel < pin_size_) {
      set(pin_ptr_[channel], true);
    }
  }

  await sleep(delay_time_ * 2);

  // Matikan beberapa channel acak
  for (let i = 0; i < 3; i++) {
    const randomChannel = Math.floor(Math.random() * pin_size_);
    set(pin_ptr_[randomChannel], false);
    await sleep(delay_time_ / 2);
  }
}
off();`
}
```

## Integration Steps

### 1. Update javascriptPresets.ts
```typescript
// Add to javascriptPresets array
export const javascriptPresets: JavaScriptPreset[] = [
  // ... existing presets
  {
    id: 'gridRowWave',
    name: 'Grid Row Wave Pattern',
    description: 'Horizontal wave across 4x4 grid layout',
    category: 'intermediate',
    code: `// Row wave implementation here...`
  },
  // Add all 10 new patterns
];
```

### 2. Update Template Help
```typescript
export const getTemplateHelp = (): string => {
  return `// JavaScript Simulator API Reference - Enhanced with Grid Patterns

// === Grid Helper Functions for 4x4 Layout ===
function getChannelFromGridPosition(row, col) {
  return row >= 0 && row < 4 && col >= 0 && col < 4 ? row * 4 + col : -1;
}

function activateRow(row, state = true) {
  for (let col = 0; col < 4; col++) {
    const channel = row * 4 + col;
    if (channel < pin_size_) set(pin_ptr_[channel], state);
  }
}

// ... other helper functions

// === Grid Pattern Examples ===
// Row wave pattern
for (let row = 0; row < 4; row++) {
  activateRow(row, true);
  await sleep(delay_time_);
  activateRow(row, false);
}

// Diagonal cross pattern
for (let i = 0; i < 4; i++) {
  set(pin_ptr_[i * 4 + i], true);  // Main diagonal
  await sleep(delay_time_);
}
`
}
```

### 3. Update Category Filtering
```typescript
// Add grid-specific category filtering
export const getGridPatterns = (): JavaScriptPreset[] => {
  return javascriptPresets.filter(preset =>
    preset.id.startsWith('grid') ||
    preset.category === 'grid-specific'
  );
}
```

## Pattern Categories

### Basic Grid Patterns
- **gridRowWave**: Horizontal wave traversal
- **gridColumnFill**: Vertical fill animation
- **gridCheckerboard**: Classic alternating pattern

### Advanced Grid Patterns
- **gridDiagonalCross**: Diagonal intersection
- **gridSpiralSquare**: Spiral traversal
- **gridSnakeMatrix**: Snake path traversal

### Geometric Patterns
- **gridFrame**: Border/frame effect
- **gridCross**: Plus sign formation
- **gridCornerExpansion**: Corner-to-center expansion

### Dynamic Patterns
- **gridRandomCluster**: Randomized cluster activation

## Testing Strategy

### 1. Pattern Validation
```javascript
// Test each pattern with different grid sizes
const testGridPattern = (patternId, gridSize = 16) => {
  // Mock the pin_ptr_ array for testing
  const mockPinPtr = Array.from({length: gridSize}, (_, i) => i);

  // Test pattern execution
  // Verify channel bounds
  // Check timing accuracy
};
```

### 2. Visual Verification
- Test each pattern in JavaScript Simulator Mode
- Verify visual effects match expectations
- Check for smooth transitions and timing

### 3. Performance Testing
- Monitor memory usage during pattern execution
- Verify timing accuracy with different delay settings
- Test edge cases (minimum/maximum channel counts)

### 4. Compatibility Testing
- Ensure patterns work with existing grid configurations
- Test backward compatibility with older patterns
- Verify integration with mode selection system

## Benefits

1. **Grid Optimization**: Patterns specifically designed for 4×4 grid layout
2. **Visual Coherence**: Structured, predictable visual effects
3. **Educational Value**: Examples of grid-based pattern algorithms
4. **Performance**: Optimized for JavaScript execution in browser
5. **Maintainability**: Clear, well-documented code structure

## Usage Examples

### Quick Integration
```typescript
// Add to your JavaScript Simulator
const gridPattern = getPresetByID('gridRowWave');
if (gridPattern) {
  // Execute the pattern
  await executePattern(gridPattern.code);
}
```

### Custom Pattern Creation
```javascript
// Create custom grid pattern
function customGridPattern() {
  for (let row = 0; row < 4; row++) {
    for (let col = 0; col < 4; col++) {
      const channel = row * 4 + col;
      if (channel < pin_size_) {
        // Custom logic here
        set(pin_ptr_[channel], true);
        await sleep(delay_time_);
      }
    }
  }
}
```

## Implementation Timeline

- **Phase 1**: Basic grid patterns implementation (taskSequence16-18)
- **Phase 2**: Advanced geometric patterns (taskSequence19-22)
- **Phase 3**: Dynamic and mathematical patterns (taskSequence23-25)
- **Phase 4**: Integration testing and optimization
- **Phase 5**: Documentation and deployment

---

**Note**: JavaScript patterns designed to maintain 100% compatibility with C++ implementations. All patterns use the same channel mapping and timing logic as their C++ counterparts for seamless deployment to ESP32 hardware.