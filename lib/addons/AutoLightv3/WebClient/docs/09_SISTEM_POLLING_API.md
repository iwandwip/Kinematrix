# Sistem Polling API - AutoLight V3

## Overview

AutoLight V3 WebClient menggunakan sistem HTTP API dengan polling untuk komunikasi real-time dengan ESP32 device. Sistem ini terdiri dari beberapa komponen polling dengan interval dan fungsi yang berbeda.

## 1. Data Polling Utama

**File**: `/lib/hooks/useALSPolling.ts`
- **Interval**: 2 detik (2000ms)
- **Status**: Aktif secara otomatis saat aplikasi start
- **Metode**: `setTimeout` dengan recursive polling

### API Endpoints yang Di-polling:

1. **Device Channel**: `/api/v1/data/get/device/ch`
   - Response: `number` (jumlah channel)
   - Fallback: `0`

2. **Device Name**: `/api/v1/data/get/device/name`
   - Response: `string` (nama device)
   - Fallback: `''`

3. **Device Serial**: `/api/v1/data/get/device/serial`
   - Response: `string` (serial number)
   - Fallback: `''`

4. **LED Mode**: `/api/v1/data/get/mode`
   - Response: `number` (mode LED saat ini)
   - Fallback: `'0'`

5. **Timing Delay**: `/api/v1/data/get/delay`
   - Response: `number` (delay timing)
   - Fallback: `0`

### Implementasi Concurrent Fetching:

```typescript
const [channelResult, deviceNameResult, serialNumberResult, modeResult, delayResult] =
  await Promise.allSettled([
    fetch(`${baseUrl}/data/get/device/ch`).then(res => res.json()).catch(() => 0),
    fetch(`${baseUrl}/data/get/device/name`).then(res => res.text()).catch(() => ''),
    fetch(`${baseUrl}/data/get/device/serial`).then(res => res.text()).catch(() => ''),
    fetch(`${baseUrl}/data/get/mode`).then(res => res.json()).then(data => data.toString()).catch(() => '0'),
    fetch(`${baseUrl}/data/get/delay`).then(res => res.json()).catch(() => 0)
  ])
```

### Fitur Polling:

- **Deep Equality Check**: Mencegah re-render yang tidak perlu
- **Dynamic API Config**: Update konfigurasi tanpa menghentikan polling
- **Error Handling**: Graceful fallback values
- **Concurrent Requests**: Parallel API calls untuk performa optimal

## 2. Connection Monitor Polling

**File**: `/lib/utils/connection.ts`
- **Interval**: 30 detik (30000ms) - default, dapat dikonfigurasi
- **Metode**: `setInterval`
- **Fungsi**: Monitor status koneksi ke ESP32 device

### Implementasi:

```typescript
public startMonitoring(intervalMs: number = 30000): void {
  this.stopMonitoring()

  this.checkConnection()
  this.intervalId = setInterval(() => {
    this.checkConnection()
  }, intervalMs)
}
```

### Status Tracking:

```typescript
interface ConnectionStatus {
  isConnected: boolean
  lastChecked: Date
  responseTime?: number
  error?: string
}
```

## 3. API Testing Polling

**File**: `/app/developer/api-test/content.tsx`
- **Delay**: 500ms - 1000ms
- **Fungsi**: Development testing dengan controlled delay
- **Status**: Manual trigger (development only)

### Delay Configuration:

```typescript
// Delay 500ms antar API test
await new Promise(resolve => setTimeout(resolve, 500))

// Delay 1000ms untuk sequence testing
await new Promise(resolve => setTimeout(resolve, 1000))
```

## 4. Request Timeout Protection

**File**: `/lib/api/config.ts`
- **Timeout**: 5 detik (5000ms)
- **Fungsi**: Abort request yang terlalu lama
- **Implementasi**: `AbortController`

```typescript
const controller = new AbortController()
const timeoutId = setTimeout(() => controller.abort(), 5000)
```

## Arsitektur Polling

### Flow Diagram:

```
WebClient App Start
        ↓
useALSPolling Hook Initialize
        ↓
Initial Data Fetch (Promise.allSettled)
        ↓
Start Recursive Polling (2s interval)
        ↓
Update React Context State
        ↓
Re-render Components (if data changed)
        ↓
Continue Polling Loop
```

### Connection Monitor Flow:

```
ConnectionMonitor Start
        ↓
Initial Connection Check
        ↓
setInterval (30s default)
        ↓
Test Connection API
        ↓
Update Status & Notify Listeners
        ↓
Continue Monitoring Loop
```

## Optimasi Performa

### 1. Concurrent API Calls
- Menggunakan `Promise.allSettled()` untuk parallel requests
- Mengurangi total response time dari ~1s menjadi ~200ms

### 2. Deep Equality Check
- Mencegah unnecessary re-renders
- Membandingkan data sebelum update state

### 3. Error Handling
- Graceful fallback values untuk setiap endpoint
- Tidak menghentikan polling saat ada error

### 4. Dynamic Configuration
- API config dapat diupdate tanpa restart polling
- Real-time config changes support

## Konfigurasi Polling

### Default Settings:

```typescript
const POLLING_INTERVALS = {
  DATA_POLLING: 2000,        // 2 detik - data utama
  CONNECTION_MONITOR: 30000, // 30 detik - status koneksi
  API_TIMEOUT: 5000,         // 5 detik - request timeout
  TEST_DELAY: 500            // 500ms - testing delay
}
```

### API Base URL Format:

```typescript
const baseUrl = `${protocol}://${host}:${port}/api/v1`
// Contoh: http://192.168.1.100:80/api/v1
```

## Control Methods

### Start/Stop Polling:

```typescript
// Data polling
const { startPolling, stopPolling } = useALSPolling()

// Connection monitoring
const monitor = new ConnectionMonitor(apiConfig)
monitor.startMonitoring(30000) // Custom interval
monitor.stopMonitoring()
```

### Manual Data Fetch:

```typescript
const { fetchData } = useALSPolling()
await fetchData() // Manual trigger
```

## Error Handling Strategy

### 1. API Request Errors
- Individual endpoint failures tidak mengganggu endpoints lain
- Fallback values untuk setiap data type

### 2. Network Errors
- Connection timeout (5s)
- Automatic retry pada polling cycle berikutnya

### 3. Parsing Errors
- Try-catch untuk JSON parsing
- Type-safe fallbacks

## Monitoring & Debugging

### Development Tools:
- Console logs untuk failed requests
- Connection status indicators
- Response time monitoring
- API test page untuk manual testing

### Performance Metrics:
- Response time tracking
- Connection status history
- Error rate monitoring

## Integrasi dengan React Context

Polling system terintegrasi dengan `ALSContext` untuk state management:

```typescript
const { setDeviceName, setDeviceSerial, setDeviceCh, setMode, setDelay } = useALS()
```

Data dari polling langsung mengupdate global state yang digunakan oleh seluruh komponen aplikasi.