# AutoLight V3 Mobile App

React Native Expo application untuk mengontrol sistem AutoLight V3 menggunakan WebView yang mengintegrasikan WebClient Next.js.

## 🚀 Quick Start

### Development
```bash
# Install dependencies
npm install

# Build WebClient dan copy ke assets
npm run build:webclient

# Start Expo dev server
npm start
```

### Production Build

#### EAS Build (Recommended)
```bash
# Install EAS CLI
npm install -g @expo/eas-cli

# Configure EAS
eas build:configure

# Build APK for Android
eas build --platform android --profile preview
```

#### Local Build
```bash
# Build APK menggunakan Expo local build
expo build:android
```

## 📱 Features

### ✅ Implemented
- ✅ **WebView Integration**: WebClient Next.js terintegrasi dalam React Native WebView
- ✅ **Native Bridge**: Komunikasi bi-directional antara WebView dan native code
- ✅ **Device API**: Proxy API calls ke AutoLight hardware melalui native bridge
- ✅ **Auto Mock Mode**: Fallback ke mock mode jika device tidak terhubung
- ✅ **AsyncStorage**: Persistent storage untuk settings dan preferences
- ✅ **Build Automation**: Script otomatis untuk build dan copy WebClient assets
- ✅ **Device Settings**: Configuration UI untuk device URL dan mock mode
- ✅ **Loading States**: Loading indicators dan error handling

### 🔧 Architecture

```
AppBuilder/
├── app/
│   ├── index.jsx              # Main entry point → WebViewContainer
│   └── _layout.jsx            # Root layout
├── components/
│   ├── WebViewContainer.jsx   # Main WebView dengan bridge communication
│   ├── NativeBridge.jsx       # JavaScript bridge script generator
│   ├── DeviceSettings.jsx     # Device configuration UI
│   └── ui/
│       └── LoadingSpinner.jsx # Loading component
├── services/
│   ├── WebViewBridge.js       # Native bridge service untuk handle komunikasi
│   └── DeviceAPI.js           # Device API service dengan auto-fallback
├── assets/
│   └── web/                   # WebClient static export files (auto-generated)
└── scripts/
    └── build-webclient.js     # Build automation script
```

### 🔗 Communication Flow

1. **WebClient (Next.js)** → Static export di `/assets/web/`
2. **WebView** → Load static files dengan injected JavaScript bridge
3. **Native Bridge** → Handle komunikasi WebView ↔ React Native
4. **Device API** → Proxy HTTP calls ke AutoLight hardware
5. **Auto Fallback** → Switch ke mock mode jika device tidak terhubung

### 📡 API Integration

#### Device Control Endpoints
- `GET /get/device/ch` - Get device channels
- `GET /get/delay` - Get current delay  
- `GET /get/device/name` - Get device name
- `GET /get/device/serial` - Get serial number
- `GET /get/mode` - Get current mode
- `GET /set/mode?value={mode}` - Set device mode
- `GET /set/delay?value={delay}` - Set delay value
- `GET /set/device/name?value={name}` - Set device name

#### Bridge Communication
```javascript
// From WebView to Native
window.NativeBridge.setDeviceMode(1);
window.NativeBridge.setDeviceDelay(50);
window.NativeBridge.setDeviceName("My AutoLight");

// Native to WebView (automatic)
window.addEventListener('deviceDataUpdate', (event) => {
  console.log('Device data updated:', event.detail);
});
```

## 🛠️ Development

### WebClient Development
```bash
# Edit WebClient (Next.js)
cd ../WebClient
npm run dev  # Development pada http://localhost:3001

# Build dan integrate ke mobile app  
cd ../AppBuilder
npm run build:webclient  # Auto-build dan copy assets
```

### Mobile App Development
```bash
# Start Expo dev server
npm start

# Test di device/simulator
# Scan QR code atau gunakan Android/iOS simulator
```

### Device Connection Settings
- **Default URL**: `http://192.168.4.1` (AutoLight AP mode)  
- **Mock Mode**: Automatic fallback saat device tidak terhubung
- **Settings UI**: Long press pada device info untuk configurasi

## 📦 Build & Deployment

### APK Build dengan EAS
```bash
# Setup EAS (first time)
eas build:configure

# Build preview APK
eas build --platform android --profile preview

# Build production release
eas build --platform android --profile production
```

### Dependencies
- **Core**: React Native 0.79 + Expo 53
- **WebView**: react-native-webview 13.12+
- **Storage**: @react-native-async-storage/async-storage
- **UI**: @gluestack-ui untuk native components
- **Icons**: lucide-react-native

## 🔧 Configuration

### app.json
- **Package**: com.mcclawsher.autolightapp
- **Version**: 1.0.0
- **Expo Router**: Enabled untuk navigation
- **Edge-to-Edge**: Enabled untuk Android

### WebView Settings
- **File Access**: Enabled untuk local assets
- **JavaScript**: Enabled dengan bridge injection
- **Storage**: DOM storage enabled
- **Mixed Content**: Compatibility mode

## 🚨 Troubleshooting

### Build Issues
```bash
# Clear cache dan reinstall
npm run clean
npm run reinstall

# Rebuild WebClient assets
npm run build:webclient
```

### WebView Loading Issues  
- Check assets di `/assets/web/index.html`
- Verify bridge script injection
- Test dengan mock mode enabled

### Device Connection
- Verify AutoLight device di 192.168.4.1
- Check WiFi connection ke AutoLight AP
- Enable mock mode untuk testing tanpa hardware

## 📈 Performance

- **WebClient Build**: ~7.4s dengan Turbopack
- **Bundle Size**: ~152KB total JavaScript
- **WebView Loading**: <2s dengan cached assets
- **API Response**: <500ms dengan device terhubung
- **Memory Usage**: ~50MB typical, ~100MB peak

## 🔮 Next Steps

- [ ] **Push Notifications**: Real-time device alerts
- [ ] **Offline Mode**: Cache device state untuk offline usage  
- [ ] **Multiple Devices**: Support multiple AutoLight devices
- [ ] **Advanced Settings**: Custom API endpoints, polling intervals
- [ ] **Export Features**: Device configuration export/import

---

**AutoLight V3 Mobile App** - Complete mobile interface untuk AutoLight V3 LED control system.