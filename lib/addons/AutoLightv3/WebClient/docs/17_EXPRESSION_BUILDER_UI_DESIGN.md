# 11_EXPRESSION_BUILDER_UI_DESIGN.md

# Expression Builder Mode - UI Design Specification
**Visual Interface Design Documentation**

---

## 📱 Overall Layout Structure

### Mode Selection Header
```
┌─────────────────────────────────────────────────────────────────┐
│                    Pattern Builder                              │
│  ┌─────────────────┬─────────────────┬─────────────────┐       │
│  │   Visual        │   C++ Simulator │  Expression     │       │
│  │   Builder       │                 │  Builder ✓      │       │
│  └─────────────────┴─────────────────┴─────────────────┘       │
│                                                                 │
│  [🏠 Back to Main] [💾 Save Project] [📤 Export All]           │
└─────────────────────────────────────────────────────────────────┘
```

### Main Content Area
```
┌─────────────────────────────────────────────────────────────────┐
│  Step 3: Create Pattern - Expression Builder Mode              │
│  ┌─────────────────────────────────────────────────────────────┐ │
│  │                 Expression Builder                          │ │
│  │                  (4 Sections)                              │ │
│  └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Section 1: Channel Configuration Panel

```
┌─────────────────────────────────────────────────────────────────┐
│  🔧 Channel Configuration                               [⚙️]    │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Total Channels: ┌──────────┐  ← Dropdown selection            │
│                  │ 24   [▼]│                                   │
│                  └──────────┘                                   │
│                  Options: 8,12,16,24,32,48,64                  │
│                                                                 │
│  Layout Suggestions:                                            │
│  ┌─────────────┬─────────────┬─────────────────────────┐       │
│  │ ⚪ 4×6 Matrix │ ○ 3×8 Matrix│ ○ Custom Layout         │       │
│  │ (Recommended)│ (Linear)    │ [Define Custom...]      │       │
│  └─────────────┴─────────────┴─────────────────────────┘       │
│                                                                 │
│  Preview Layout:                                                │
│  ┌─────────────────────────────────────────────────────┐       │
│  │  Channel Grid Preview (4×6)                         │       │
│  │                                                     │       │
│  │   0   1   2   3   4   5     ← Column numbers       │       │
│  │ 0 ■   ■   ■   ■   ■   ■     ← Row 0               │       │
│  │ 1 ■   ■   ■   ■   ■   ■     ← Row 1               │       │
│  │ 2 ■   ■   ■   ■   ■   ■     ← Row 2               │       │
│  │ 3 ■   ■   ■   ■   ■   ■     ← Row 3               │       │
│  │                                                     │       │
│  │         24 channels configured                      │       │
│  │    💡 Grid layout optimized for visualization       │       │
│  └─────────────────────────────────────────────────────┘       │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🎨 Section 2: Pattern Creation (Dual Panel Layout)

### Left Panel: Pattern Library
```
┌───────────────────────────────────────┐
│ 📚 Pattern Library                    │
├───────────────────────────────────────┤
│                                       │
│ Category Tabs:                        │
│ ┌─────┬─────┬─────────┬────────┐      │
│ │Basic│Math │Sequence │Custom  │      │
│ │ ✓   │     │         │        │      │
│ └─────┴─────┴─────────┴────────┘      │
│                                       │
│ Available Patterns:                   │
│                                       │
│ ┌─────────────────────────────────┐   │
│ │ 💡 All Channels                 │   │
│ │    Turn on all channels         │   │
│ │    Expression: true             │   │
│ │    Preview: ████████████        │   │
│ │                          [+]    │   │
│ └─────────────────────────────────┘   │
│                                       │
│ ┌─────────────────────────────────┐   │
│ │ 💡 First Half                   │   │
│ │    First 50% of channels        │   │
│ │    Expression: i < totalCh/2    │   │
│ │    Preview: ██████░░░░░░        │   │
│ │                          [+]    │   │
│ └─────────────────────────────────┘   │
│                                       │
│ ┌─────────────────────────────────┐   │
│ │ 💡 Even Channels                │   │
│ │    Even numbered channels       │   │
│ │    Expression: i % 2 === 0      │   │
│ │    Preview: █░█░█░█░█░█░        │   │
│ │                          [+]    │   │
│ └─────────────────────────────────┘   │
│                                       │
│ ┌─────────────────────────────────┐   │
│ │ 💡 Custom Modulo                │   │
│ │    Every Nth channel            │   │
│ │    Expression: i%${n}===${off}  │   │
│ │                                 │   │
│ │    Parameters:                  │   │
│ │    Divisor:  ┌───┐  Offset:┌───┐│   │
│ │             │ 3 │         │ 0 ││   │
│ │             └───┘         └───┘│   │
│ │    Preview: █░░█░░█░░█░░█░░█    │   │
│ │                          [+]    │   │
│ └─────────────────────────────────┘   │
│                                       │
│ ┌─────────────────────────────────┐   │
│ │         [+ Add to Timeline]     │   │
│ │         Duration: [100] ms      │   │
│ └─────────────────────────────────┘   │
└───────────────────────────────────────┘
```

### Right Panel: Custom Expression Editor
```
┌───────────────────────────────────────┐
│ ✏️ Custom Expression Editor           │
├───────────────────────────────────────┤
│                                       │
│ Expression Input:                     │
│ ┌─────────────────────────────────────┐│
│ │ i < totalChannels / 2             ✓││
│ └─────────────────────────────────────┘│
│ [✓ Valid Expression]                  │
│                                       │
│ Quick Insert Buttons:                 │
│ ┌────┬────┬────┬────┬────┬────┐      │
│ │ i  │ tC │ %  │===│ && │ || │      │
│ └────┴────┴────┴────┴────┴────┘      │
│ ┌────┬────┬────┬────┬────┬────┐      │
│ │ <  │ >  │ <= │ >= │!===│ () │      │
│ └────┴────┴────┴────┴────┴────┘      │
│                                       │
│ Variables Reference:                  │
│ • i = channel index (0 to 23)        │
│ • totalChannels = 24                  │
│                                       │
│ Live Expression Preview:              │
│ ┌─────────────────────────────────────┐│
│ │  Real-time Channel Preview         ││
│ │                                    ││
│ │  0  1  2  3  4  5  ← Columns      ││
│ │0 🟡 ⚫ 🟡 ⚫ 🟡 ⚫  ← Row 0        ││
│ │1 🟡 ⚫ 🟡 ⚫ 🟡 ⚫  ← Row 1        ││
│ │2 🟡 ⚫ 🟡 ⚫ 🟡 ⚫  ← Row 2        ││
│ │3 🟡 ⚫ 🟡 ⚫ 🟡 ⚫  ← Row 3        ││
│ │                                    ││
│ │ Active: 0,2,4,6,8,10,12,14,16,18   ││
│ │ 🟡 = ON (12 channels)              ││
│ │ ⚫ = OFF (12 channels)             ││
│ └─────────────────────────────────────┘│
│                                       │
│ Frame Settings:                       │
│ Duration: ┌─────┐ ms                  │
│          │ 100 │                     │
│          └─────┘                      │
│                                       │
│ Label (Optional):                     │
│ ┌─────────────────────────────────────┐│
│ │ First Half Pattern                 ││
│ └─────────────────────────────────────┘│
│                                       │
│ ┌─────────────────────────────────────┐│
│ │      [+ Add Custom Frame]          ││
│ └─────────────────────────────────────┘│
└───────────────────────────────────────┘
```

---

## 🎬 Section 3: Pattern Preview & Timeline

### Pattern Preview (Main Area)
```
┌─────────────────────────────────────────────────────────────────┐
│ 👁️ LED Pattern Preview                                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│ Current Frame: 2/4                                              │
│ ┌───────────────────────────────────────────────────────────────┐│
│ │ Frame 2: Even Channels (100ms)                              ││
│ └───────────────────────────────────────────────────────────────┘│
│                                                                 │
│ LED Grid Visualization:                                         │
│ ┌─────────────────────────────────────────────────────────────┐ │
│ │     Channel Numbers                                         │ │
│ │     0   1   2   3   4   5                                  │ │
│ │  0  🟡  ⚫  🟡  ⚫  🟡  ⚫   ← Row 0                       │ │
│ │  1  🟡  ⚫  🟡  ⚫  🟡  ⚫   ← Row 1                       │ │
│ │  2  🟡  ⚫  🟡  ⚫  🟡  ⚫   ← Row 2                       │ │
│ │  3  🟡  ⚫  🟡  ⚫  🟡  ⚫   ← Row 3                       │ │
│ │                                                             │ │
│ │  🟡 = Channel ON (12 channels active)                      │ │
│ │  ⚫ = Channel OFF (12 channels inactive)                    │ │
│ └─────────────────────────────────────────────────────────────┘ │
│                                                                 │
│ Active Channels: 0,2,4,6,8,10,12,14,16,18,20,22                │
│ Channel Pattern: Even numbered channels                         │
│ Expression: i % 2 === 0                                        │
│                                                                 │
│ Animation Controls:                                             │
│ ┌─────┬─────┬─────┬─────────┬──────────────────┐               │
│ │ ⏮️ │ ▶️ │ ⏭️ │ 🔄 Loop │ Speed: [1x ▼]   │               │
│ │Previous│Play │Next │   Toggle │             │               │
│ └─────┴─────┴─────┴─────────┴──────────────────┘               │
│                                                                 │
│ Timeline Progress:                                              │
│ Frame: ████████░░░░░░░░ 2/4 (50%)                             │
│ Time:  ████████░░░░░░░░ 200ms/350ms                           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Timeline Panel (Side)
```
┌─────────────────────────────────────┐
│ 🎞️ Frame Timeline                  │
├─────────────────────────────────────┤
│                                     │
│ Timeline Sequence:                  │
│                                     │
│ ┌─────────────────────────────────┐ │
│ │ 1. All Channels        100ms    │ │
│ │    ████████████████             │ │
│ │    Expression: true      [✏️][❌]│ │
│ └─────────────────────────────────┘ │
│                                     │
│ ┌─────────────────────────────────┐ │
│ │ ✓ 2. Even Channels     100ms    │ │ ← Current
│ │    ████░░░░████░░░░             │ │
│ │    Expression: i%2===0   [✏️][❌]│ │
│ └─────────────────────────────────┘ │
│                                     │
│ ┌─────────────────────────────────┐ │
│ │ 3. Odd Channels        100ms    │ │
│ │    ░░░░████░░░░████             │ │
│ │    Expression: i%2===1   [✏️][❌]│ │
│ └─────────────────────────────────┘ │
│                                     │
│ ┌─────────────────────────────────┐ │
│ │ 4. All OFF             50ms     │ │
│ │    ░░░░░░░░░░░░░░░░             │ │
│ │    Expression: false     [✏️][❌]│ │
│ └─────────────────────────────────┘ │
│                                     │
│ ┌─────────────────────────────────┐ │
│ │ Total Duration: 350ms           │ │
│ │ Total Frames: 4                 │ │
│ │                                 │ │
│ │ [🔄 Loop Preview]              │ │
│ │ [⏸️ Stop Preview]              │ │
│ └─────────────────────────────────┘ │
│                                     │
│ Frame Management:                   │
│ ┌─────────────────────────────────┐ │
│ │ [+ Add Frame]                   │ │
│ │ [📋 Duplicate Selected]         │ │
│ │ [🗑️ Clear All]                  │ │
│ │ [📤 Export Sequence]            │ │
│ └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

---

## 💻 Section 4: Code Generation Output

```
┌─────────────────────────────────────────────────────────────────┐
│ </> Generated C++ Code                                 [💾][📋] │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│ Code Format Selection:                                          │
│ ┌──────────────┬──────────────┐                                │
│ │ Modern API ✓ │ Native API   │                                │
│ └──────────────┴──────────────┘                                │
│                                                                 │
│ Generated Code Preview:                                         │
│ ┌─────────────────────────────────────────────────────────────┐ │
│ │ ```cpp                                                     │ │
│ │ void BaseChannel::taskSequenceCustomExpression() {         │ │
│ │     // Generated by Expression Builder - Modern API        │ │
│ │     // Total Frames: 4, Target Channels: 24 (dynamic)     │ │
│ │     // Generated on: 2024-01-15 10:30:00                  │ │
│ │                                                            │ │
│ │     // Frame 1: All Channels (100ms)                      │ │
│ │     // Expression: true                                    │ │
│ │     for (int i = 0; i < config_data_ptr_->header.pin_size_;│ │
│ │          i++) {                                            │ │
│ │         set(config_data_ptr_->header.pin_ptr_[i], HIGH);   │ │
│ │     }                                                      │ │
│ │     sleep(100);                                            │ │
│ │                                                            │ │
│ │     // Frame 2: Even Channels (100ms)                     │ │
│ │     // Expression: i % 2 === 0                            │ │
│ │     for (int i = 0; i < config_data_ptr_->header.pin_size_;│ │
│ │          i += 2) {                                         │ │
│ │         set(config_data_ptr_->header.pin_ptr_[i], HIGH);   │ │
│ │     }                                                      │ │
│ │     sleep(100);                                            │ │
│ │                                                            │ │
│ │     // Frame 3: Odd Channels (100ms)                      │ │
│ │     // Expression: i % 2 === 1                            │ │
│ │     for (int i = 1; i < config_data_ptr_->header.pin_size_;│ │
│ │          i += 2) {                                         │ │
│ │         set(config_data_ptr_->header.pin_ptr_[i], HIGH);   │ │
│ │     }                                                      │ │
│ │     sleep(100);                                            │ │
│ │                                                            │ │
│ │     // Frame 4: All OFF (50ms)                            │ │
│ │     // Expression: false                                   │ │
│ │     off();                                                 │ │
│ │     sleep(50);                                             │ │
│ │                                                            │ │
│ │     off(); // Turn off all channels at end                │ │
│ │ }                                                          │ │
│ │ ```                                                        │ │
│ └─────────────────────────────────────────────────────────────┘ │
│                                                                 │
│ Code Statistics:                                                │
│ • Function Lines: 28                                           │
│ • Total Duration: 350ms                                        │
│ • Memory Usage: ~45 bytes                                      │
│ • Compatible: ESP32/ESP8266/AVR                                │
│                                                                 │
│ Status Indicators:                                              │
│ ✅ BaseChannelSequence.cpp Compatible                          │
│ ✅ Dynamic Channel Support (8-64)                              │
│ ✅ Memory Optimized                                            │
│ ✅ Syntax Valid                                                │
│                                                                 │
│ Actions:                                                        │
│ ┌─────────────────┬─────────────────┬─────────────────┐       │
│ │ [📋 Copy Code]  │ [💾 Save File]  │ [📤 Export]     │       │
│ └─────────────────┴─────────────────┴─────────────────┘       │
│                                                                 │
│ ┌─────────────────────────────────────────────────────────────┐ │
│ │ 💡 Code ready for BaseChannelSequence.cpp integration!     │ │
│ │    1. Copy generated code                                   │ │
│ │    2. Paste into new taskSequence function                 │ │
│ │    3. Add function call to sequence switcher               │ │
│ │    4. Compile and upload to ESP32                          │ │
│ └─────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔄 Interactive Elements & Behaviors

### 1. Real-time Expression Validation
```
Expression Input: [i % 2 === 0          ] ✅ Valid
                 ↳ ✅ Syntax correct
                 ↳ ✅ 12 channels will be activated
                 ↳ ✅ Pattern preview updated

Expression Input: [i %% 2               ] ❌ Invalid
                 ↳ ❌ Syntax error: unexpected token
                 ↳ ❌ Pattern preview disabled
```

### 2. Smart Pattern Suggestions
```
┌─────────────────────────────────────────────────────────┐
│ 💡 Smart Suggestions                                    │
├─────────────────────────────────────────────────────────┤
│ Detected Pattern: "First half selection"               │
│                                                         │
│ Your expression: i < 12                                 │
│ Suggested: i < totalChannels / 2                        │
│                                                         │
│ ✨ Benefit: Works with any channel count               │
│                                                         │
│ [Apply Suggestion] [Ignore]                            │
└─────────────────────────────────────────────────────────┘
```

### 3. Channel Hover Information
```
Grid Cell Hover:
┌─────────────────────────┐
│ Channel 5               │
│ Position: Row 0, Col 5  │
│ Index: 5                │
│ Status: OFF             │
│ Expression Result: false│
└─────────────────────────┘
```

### 4. Timeline Frame Actions
```
Timeline Frame:
┌─────────────────────────────────┐
│ 2. Even Channels     100ms  [⋮] │ ← Menu trigger
│    ████░░░░████░░░░         │
│    Expression: i%2===0          │
│                                 │
│ Context Menu:                   │
│ ┌─────────────────────────────┐ │
│ │ ✏️ Edit Expression          │ │
│ │ 📋 Duplicate Frame          │ │
│ │ ⏱️ Change Duration          │ │
│ │ 🔼 Move Up                  │ │
│ │ 🔽 Move Down                │ │
│ │ ❌ Delete Frame             │ │
│ └─────────────────────────────┘ │
└─────────────────────────────────┘
```

---

## 🎯 Key UI Features Summary

### 1. Responsive Design Elements
- **Mobile Warning**: Expression Builder requires desktop (≥1024px)
- **Tablet Adaptation**: Stacked layout untuk medium screens
- **Desktop Optimized**: Full dual-panel layout

### 2. Accessibility Features
- **Keyboard Navigation**: Tab order untuk all interactive elements
- **Screen Reader Support**: ARIA labels untuk complex components
- **High Contrast**: Support untuk high contrast mode
- **Focus Indicators**: Clear focus states untuk all buttons

### 3. User Experience Enhancements
- **Progressive Disclosure**: Advanced features hidden by default
- **Contextual Help**: Tooltips dan inline guidance
- **Undo/Redo**: Action history untuk frame operations
- **Auto-save**: Automatic saving ke localStorage

### 4. Performance Optimizations
- **Lazy Rendering**: Grid cells rendered on-demand
- **Throttled Updates**: Expression evaluation throttled
- **Memoized Calculations**: Pattern calculations cached
- **Virtual Scrolling**: For large channel counts (64+)

---

## 🚀 User Journey Flow

### Step-by-Step Experience

1. **Entry Point**
   ```
   User clicks "Expression Builder" tab
   → Page loads dengan default 24 channel configuration
   → Smart suggestions appear based on common patterns
   ```

2. **Configuration Phase**
   ```
   User adjusts channel count → Grid layout updates automatically
   → Layout suggestions refresh → Preview updates in real-time
   ```

3. **Pattern Creation**
   ```
   User browses pattern library → Clicks [+] add button
   → Pattern appears di timeline → Preview shows result
   → OR User creates custom expression → Live preview updates
   ```

4. **Timeline Building**
   ```
   User adds multiple frames → Timeline shows sequence
   → User clicks play → Animation starts → Loop controls available
   ```

5. **Code Generation**
   ```
   Timeline complete → Code generates automatically
   → User switches Modern/Native API → Copy code to clipboard
   ```

6. **Deployment**
   ```
   User pastes code di BaseChannelSequence.cpp
   → Compiles project → Uploads to ESP32 → Pattern works!
   ```

### Visual Feedback System
- ✅ **Success States**: Green checkmarks, success animations
- ❌ **Error States**: Red highlights, clear error messages
- 🔄 **Loading States**: Spinners untuk code generation
- 💡 **Information States**: Blue highlights untuk suggestions

---

## 🎨 Design System Components

### Colors & Theme
```css
/* Primary Colors */
--primary: #2563eb      /* Blue for active elements */
--secondary: #64748b    /* Gray for secondary actions */
--success: #10b981      /* Green for valid states */
--warning: #f59e0b      /* Yellow for warnings */
--error: #ef4444        /* Red for errors */

/* LED Visualization */
--led-on: #fbbf24       /* Yellow/Gold for active LEDs */
--led-off: #374151      /* Dark gray for inactive LEDs */
--grid-border: #e5e7eb  /* Light gray for grid lines */
```

### Typography Scale
- **Headers**: text-2xl font-bold (Channel Configuration, etc.)
- **Subheaders**: text-lg font-semibold (Pattern names, etc.)
- **Body**: text-sm (Descriptions, labels)
- **Code**: text-xs font-mono (Expressions, generated code)
- **Captions**: text-xs text-muted (Helper text, statistics)

### Spacing System
- **Section Gap**: space-y-6 (24px between major sections)
- **Component Gap**: space-y-4 (16px between components)
- **Element Gap**: space-y-2 (8px between related elements)
- **Tight Gap**: space-y-1 (4px between very related elements)

### Interactive States
- **Hover**: Subtle scale-105 transform + shadow
- **Active**: Darker background dengan pressed effect
- **Focus**: 2px blue ring untuk keyboard navigation
- **Disabled**: Opacity-50 dengan cursor-not-allowed

This UI design provides a comprehensive, user-friendly interface untuk Expression Builder mode yang powerful namun accessible untuk users dengan varying technical backgrounds! 🎉