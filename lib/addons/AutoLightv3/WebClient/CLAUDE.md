# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the **AutoLight V3 WebClient**, a Next.js 15 web application that provides a modern interface for controlling AutoLight V3 LED systems. The application connects to AutoLight hardware devices via REST API and provides real-time control and monitoring capabilities with an advanced Pattern Builder for LED sequence development.

**Key Technologies**: Next.js 15, React 19, TypeScript, shadcn/ui, Tailwind CSS 4

## Essential Build and Development Commands

### Primary Development Workflow
```bash
# Start development server (port 3001)
npm run dev

# Build for production with static export
npm run build

# Start production server (port 3001)
npm run start

# Lint code
npm run lint

# Kill server on port 3001 (Linux/macOS)
npm run kill
```

### Mobile Development (Capacitor)
```bash
# Add Android platform
npm run cap:add:android

# Build and sync for Android development
npm run cap:build:android

# Sync project with native platforms
npm run cap:sync
```

### C++ Testing Framework
```bash
# Navigate to test directory and run sequence tests
cd test
npm install && npm run build

# Test individual sequences
npm run test -- --sequence 2
npm run test -- --all
npm run compare
npm run validate
```

## High-Level Architecture

### Client-Side Rendering Strategy
This application uses **full client-side rendering** with all major components using the `"use client"` directive:

- **Real-time Updates**: Immediate response to device state changes
- **Complex Interactions**: Pattern Builder requires client-side state management
- **Performance**: Avoids server-side rendering overhead for real-time UI

### Dual Context State Management
Separate contexts for different concerns:

1. **ALSContext** (`/lib/contexts/ALSContext.tsx`):
   - **Device State**: deviceName, deviceSerial, deviceCh, mode, delay
   - **API Configuration**: host, port, protocol, connectionType with localStorage persistence
   - **Settings Management**: themes, colors, loader state
   - **Optimized Updates**: Memoized callbacks with deep equality checks

2. **SimulationContext** (`/lib/contexts/SimulationContext.tsx`):
   - **Pattern Management**: LED frames, grid configuration, playback controls
   - **Multi-Mode Support**: Visual, C++, Expression, JavaScript simulation modes
   - **JavaScript Engine**: Integration with high-precision timing system
   - **Zustand Persistence**: Page state restoration across sessions

### Real-Time Polling System
**useALSPolling Hook** (`/lib/hooks/useALSPolling.ts`):
- **Concurrent API Calls**: Promise.allSettled() for 5 endpoints simultaneously
- **Grace Period Logic**: 1-second grace period after local changes to prevent UI reverts
- **Dynamic Configuration**: Updates API config without interrupting polling
- **Error Resilience**: Comprehensive error handling with fallbacks

### API Integration Pattern
The application communicates with AutoLight V3 hardware via REST endpoints:

```typescript
// Base URL construction
const baseURL = `${protocol}://${host}:${port}/api/v1`

// Key endpoints:
GET  /data/get/device/ch     // Get channel count
GET  /data/get/mode          // Get current LED mode
GET  /data/get/delay         // Get timing delay
GET  /data/get/device/name   // Get device name
GET  /data/get/device/serial // Get device serial

GET  /data/set/mode?value={mode}       // Set LED mode
GET  /data/set/delay?value={delay}     // Set timing delay
GET  /data/set/device/name?value={name} // Set device name
```

## Pattern Builder Architecture

### 4 Simulation Modes
The Pattern Builder provides desktop-only LED sequence development with 4 distinct modes:

1. **Visual Pattern Mode**: Interactive grid-based LED pattern creation
2. **Expression Builder Mode**: Mathematical expression-based patterns
3. **C++ Simulator Mode**: C++ code parsing and execution engine
4. **JavaScript Simulator Mode**: High-precision JavaScript engine with Arduino compatibility

### High-Precision Timing System
**PrecisionTimer Class** (`/lib/utils/precisionTimer.ts`):
- **±1-4% Timing Accuracy**: Revolutionary improvement over standard setTimeout
- **Hybrid Algorithm**: Combines setTimeout and requestAnimationFrame for optimal precision
- **Hardware Compatibility**: JavaScript patterns match ESP32 deployment timing exactly
- **Zero Double-Timing**: Eliminates setTimeout + playback context timing conflicts

### JavaScript Engine
**JavaScript Engine** (`/lib/simulation/javascriptEngine.ts`):
- **Arduino Compatibility Layer**: Complete C++ function compatibility with exact signatures
- **Frame Generation**: Real-time LED frame generation with microsecond accuracy
- **Error Handling**: Comprehensive error management for pattern execution

## Component Architecture

### Dual Theme System
- **ModernTheme**: Card-based interface with drawer components
- **ClassicTheme**: Section-based layout with alert components
- **Responsive Design**: Mobile-first approach with desktop enhancements
- **shadcn/ui Integration**: 21 Radix UI components with consistent styling

### Core Components
```
components/
├── MainPage.tsx              # Root component with theme routing
├── ModernTheme.tsx           # Card-based interface
├── ClassicTheme.tsx          # Section-based layout
├── content/                  # Shared UI components
│   ├── MainHeader.tsx        # Navigation header
│   ├── Sidebar.tsx           # Mobile sidebar navigation
│   ├── ConnectionStatus.tsx  # Device connection indicators
│   └── ThemeToggle.tsx       # Dark/light mode toggle
├── simulation/               # Pattern Builder components
│   ├── modes/                # 4 simulation modes
│   ├── grid/                 # Grid system components
│   ├── config/               # Configuration components
│   └── expression/           # Expression parsing UI
└── ui/                       # shadcn/ui components (21 components)
```

## Key Development Patterns

### State Management Flow
1. **ALSProvider** wraps the entire application providing global device state
2. **useALSPolling** hook automatically polls device every 2 seconds
3. **API functions** in `/lib/api/device.ts` handle REST communication
4. **Connection config** persisted in localStorage with migration support
5. **Theme preferences** managed through context with persistence

### Desktop-Only Pattern Builder
- **Responsive Restriction**: Available only on screens ≥1024px (lg breakpoint)
- **Mobile Warning**: Shows informative message on mobile devices explaining desktop requirement
- **Professional UI**: Monaco editor, frame timeline, grid controls

### Code Generation System
Dual C++ code generation with AutoLight V3 compatibility:
- **Direct API**: Uses BaseChannel direct control with optimized functions
- **Native API**: Direct hardware control with set(), sleep(), on(), off()
- **Grid Metadata**: Includes grid layout information in generated code comments
- **Export Ready**: Copy-paste ready code for BaseChannelSequence.cpp integration

## Configuration Files

### Next.js Configuration (`next.config.ts`)
```typescript
const nextConfig = {
  output: 'export',        // Static export mode
  trailingSlash: true,     // Required for static hosting
  basePath: '',            // No base path
  images: { unoptimized: true }  // Static image optimization
}
```

### shadcn/ui Configuration (`components.json`)
- **Style**: "new-york" variant with neutral base color
- **CSS Variables**: Enabled for dynamic theming
- **Icon Library**: lucide-react
- **Path Aliases**: Clean import paths with `@/` prefix

## Testing Framework

### TypeScript C++ Testing Framework (`/test`)
Standalone package for C++ sequence validation:
- **Multiple Test Modes**: Single sequence, all sequences, comparison, validation
- **Input/Output System**: C++ input files with JSON test results
- **CLI Interface**: Command-line testing with various options

## Hardware Communication

### AutoLight V3 Device Integration
- **Default Connection**: `192.168.4.1:8000` (AutoLight AP mode)
- **Protocol**: HTTP REST API over WiFi
- **Control Modes**: 16 total modes (OFF, ON, Modes 2-15)
- **Timing Control**: 30-300ms delay adjustment with slider interface
- **Device Management**: Name change, serial number display, channel count

### Connection Management
- **IP/DNS Support**: Automatic detection and validation
- **Connection Testing**: Built-in connectivity test with timeout
- **Config Persistence**: localStorage with migration support
- **Error Handling**: Comprehensive error states and recovery

## Common Development Tasks

### Adding New API Endpoints
1. Add function to `/lib/api/device.ts` with error handling
2. Update TypeScript interfaces in `/lib/contexts/ALSContext.tsx`
3. Add to polling hook if real-time updates needed
4. Update UI components to use new endpoints

### Theme Development
1. Modify `ModernTheme.tsx` for card-based interfaces
2. Modify `ClassicTheme.tsx` for section-based layouts
3. Update theme selection in `/app/settings/theme/page.tsx`
4. Maintain consistency in both themes for feature parity

### UI Component Integration
1. Use shadcn/ui components from `/components/ui/`
2. Follow existing patterns for drawer (Modern) vs alert (Classic) themes
3. Implement responsive design for mobile/desktop
4. Use Tailwind CSS 4 for styling with CSS variables

## Recent Bug Fixes

### Channel 0 Falsy Value Fix (Latest Update)
**Problem**: Channel 0 was not appearing in Pattern Grid due to JavaScript falsy value handling.

**Solution Applied**: Changed from `return channelMapping.get(key) || null` to `return channelMapping.has(key) ? channelMapping.get(key) : null`

**Files Fixed**:
- `components/simulation/grid/DynamicPatternGrid.tsx:32`
- `lib/utils/gridMapping.ts:15`

This WebClient provides a complete interface for AutoLight V3 LED control systems with professional-grade features including real-time monitoring, device configuration, multi-platform deployment support, and an advanced Pattern Builder for LED sequence development.