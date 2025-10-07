# AutoLight V3 WebClient

A modern Next.js 15 web application providing a comprehensive interface for controlling AutoLight V3 LED systems. This application connects to AutoLight hardware devices via REST API and provides real-time control, monitoring, and advanced Pattern Builder capabilities.

**Key Technologies**: Next.js 15, React 19, TypeScript, shadcn/ui, Tailwind CSS 4

## Getting Started

### Development Server

```bash
# Start development server (port 3001)
npm run dev
# or
pnpm dev
```

Open [http://localhost:3001](http://localhost:3001) with your browser to access the AutoLight control interface.

### Essential Commands

```bash
# Build for production
npm run build

# Start production server (port 3001)
npm run start

# Lint code
npm run lint

# Kill server on port 3001 (Linux/macOS)
npm run kill

# Export static files
npm run export

# Build for mobile deployment
npm run build:mobile
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

## Architecture Overview

### Client-Side Architecture
This application uses **full client-side rendering** with all major components using the `"use client"` directive:

- **App Router**: Next.js 15 App Router with `/app` directory structure
- **Global State**: React Context (`ALSContext`) with custom hooks for device communication
- **Real-time Polling**: 2-second interval polling hook (`useALSPolling`) for device data
- **Theme System**: Dynamic theme switching (Classic/Modern) with dark/light mode support

### Core State Management

```typescript
// Global state structure (ALSContext)
interface ALSData {
  deviceName: string
  deviceSerial: string
  deviceCh: number
  mode: string           // LED mode (0-15)
  delay: number         // Timing delay (30-300ms)
  apiConfig: {          // Device connection config
    host: string        // IP address or hostname
    port: number        // Default: 8000
    protocol: string    // http/https
    connectionType: 'ip' | 'dns'
  }
}

interface SettingsData {
  themes: string        // 'classic' | 'modern'
  colors: string        // 'dark' | 'light'
  loader: boolean
  page: string
  failedLoad: number
}
```

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

## Pattern Builder

### Desktop-Only Feature
The Pattern Builder is restricted to desktop-only access for optimal user experience:

- **Responsive Restriction**: Available only on screens ≥1024px (lg breakpoint)
- **Mobile Warning**: Shows informative message on mobile devices explaining desktop requirement
- **Sidebar Integration**: Pattern Builder menu item hidden on mobile navigation

### Dynamic Grid System
Provides flexible LED pattern creation with custom grid configurations:

- **Grid Configuration**: Support for any dimensions (rows × cols) with custom channel count
- **Channel Mapping**: Interactive assignment of channel numbers to grid positions
- **Layout Types**: Matrix, Line, Circle, and Custom layout options
- **Visual Builder**: Real-time pattern creation with toggle functionality

### 3-Step Workflow
1. **Step 1: Configure Grid Layout** - Set grid dimensions and layout type
2. **Step 2: Map Channels** - Assign channel numbers to specific grid positions
3. **Step 3: Create Pattern** - Design LED patterns and generate C++ code

### Code Generation
Dual C++ code generation with AutoLight V3 compatibility:

- **Direct API**: Uses BaseChannel direct LED control with optimized functions
- **Native API**: Direct hardware control with set(), sleep(), on(), off()
- **Grid Metadata**: Includes grid layout information in generated code comments
- **Export Ready**: Copy-paste ready code for BaseChannelSequence.cpp integration

## Recent Bug Fixes

### Channel 0 Falsy Value Fix (Latest Update)
**Problem**: Channel 0 was not appearing in Pattern Grid (Step 3) due to JavaScript falsy value handling.

**Root Cause**:
```typescript
// Problematic code - Channel 0 becomes null
return channelMapping.get(key) || null  // 0 || null = null
```

**Solution Applied**:
```typescript
// Fixed code - Explicit null checking
return channelMapping.has(key) ? channelMapping.get(key) : null
```

**Files Fixed**:
- `components/simulation/grid/DynamicPatternGrid.tsx:32` ✅
- `lib/utils/gridMapping.ts:15` ✅

**Impact**: Channel 0 now displays correctly throughout the entire Pattern Builder workflow (Steps 1-3).

### Responsive Design Implementation
**Desktop-Only Pattern Builder**: Implemented responsive restrictions ensuring Pattern Builder is only accessible on desktop devices while maintaining full mobile compatibility for all other features.

**Files Modified**:
- `components/content/Sidebar.tsx` - Hidden Pattern Builder menu on mobile
- `app/simulation/page.tsx` - Added desktop/mobile conditional rendering with warning

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

## Development Configuration

### Next.js Configuration
```typescript
// next.config.ts - Configured for static export
const nextConfig = {
  output: 'export',        // Static export mode
  trailingSlash: true,     // Required for static hosting
  basePath: '',            // No base path
  images: { unoptimized: true }  // Static image optimization
}
```

### shadcn/ui Configuration
- **Style**: "new-york" variant
- **Base Color**: neutral
- **CSS Variables**: enabled
- **Icon Library**: lucide-react
- **21 UI components** installed and configured

### TypeScript Configuration
- **Strict mode**: enabled
- **Path aliases**: configured for clean imports (`@/components`, `@/lib`)
- **Target**: ES2017 with modern JS features

## Component Architecture

```
components/
├── MainPage.tsx              # Root component with theme routing
├── ModernTheme.tsx           # Card-based interface with drawers
├── ClassicTheme.tsx          # Section-based layout with alerts
├── content/
│   ├── MainHeader.tsx        # Navigation header
│   ├── Sidebar.tsx           # Mobile sidebar navigation
│   ├── TopbarTitle.tsx       # Title component
│   ├── ThemeToggle.tsx       # Dark/light mode toggle
│   └── ConnectionStatus.tsx  # Device connection indicators
└── ui/                       # shadcn/ui components (21 components)
```

### State Management Flow
1. **ALSProvider** wraps the entire application providing global state
2. **useALSPolling** hook automatically polls device every 2 seconds
3. **API functions** in `/lib/api/device.ts` handle REST communication
4. **Connection config** persisted in localStorage with migration support
5. **Theme preferences** managed through context with persistence

### Real-time Polling System
```typescript
// Polling implementation (useALSPolling)
- Fetches 5 endpoints concurrently using Promise.allSettled()
- 2-second polling interval with timeout management
- Automatic retry and error handling
- Deep equality checks to prevent unnecessary re-renders
- Dynamic API config updates without polling interruption
```

## Production Deployment

### Static Export
1. Run `npm run build` to create static export
2. Deploy `out/` directory to static web server
3. Configure base URL if deploying to subdirectory
4. Ensure target network has access to AutoLight devices

### Mobile Development
1. Install Android Studio and Capacitor CLI
2. Run `npm run cap:build:android` to prepare Android build
3. Use Android Studio to build and deploy APK
4. Configure network access for device communication in mobile environment

This WebClient provides a complete interface for AutoLight V3 LED control systems with professional-grade features including real-time monitoring, device configuration, multi-platform deployment support, and an advanced Pattern Builder for LED sequence development.
