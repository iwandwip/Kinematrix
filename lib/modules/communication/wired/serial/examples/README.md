# CeriaSerial Examples

Professional examples demonstrating real-world inter-microcontroller communication patterns using CeriaSerial library.

## 🚀 Quick Start

Each example consists of **2 files** - one for each microcontroller. Upload the appropriate code to each device and connect them according to the hardware setup instructions.

## 📁 Available Examples

### 1. **Full-Duplex Communication**
- **Folders**: `full-duplex-mcu-a/`, `full-duplex-mcu-b/`
- **Hardware**: ESP32 ↔ ESP32 (dedicated TX/RX lines)
- **Use Case**: Real-time sensor data + control commands
- **Features**: Simultaneous bidirectional communication, automatic control decisions

**MCU A**: Sensor controller - collects data, executes commands
**MCU B**: Control center - monitors data, makes decisions, user interface

### 2. **Half-Duplex Communication**  
- **Folders**: `half-duplex-master/`, `half-duplex-slave/`
- **Hardware**: ESP32 ↔ Arduino (shared communication line)
- **Use Case**: Master-slave request-response pattern
- **Features**: Collision detection, turn-based communication, automatic retry

**Master**: Initiates all communication, requests data from slave
**Slave**: Responds to requests, never initiates communication

### 3. **Sensor Network**
- **Folders**: `sensor-network-hub/`, `sensor-network-node/`
- **Hardware**: ESP32 Hub ↔ Arduino Node(s)
- **Use Case**: Scalable sensor network with multiple nodes
- **Features**: Node registration, health monitoring, alert system

**Hub**: Central data collector, node management, network coordination
**Node**: Autonomous sensor device, automatic reporting, power management

## 🔧 Hardware Setup

### Full-Duplex (ESP32 ↔ ESP32)
```
MCU A (ESP32):           MCU B (ESP32):
├─ TX2 (GPIO17) ────────→ RX2 (GPIO16)
└─ RX2 (GPIO16) ←────── TX2 (GPIO17)
├─ GND ─────────────── GND
```

### Half-Duplex (ESP32 ↔ Arduino)
```
Master (ESP32):          Slave (Arduino):
├─ TX1 (GPIO1) ─────────→ RX (Pin 0)
└─ RX1 (GPIO3) ←────── TX (Pin 1)
├─ GND ─────────────── GND
```

### Sensor Network (ESP32 ↔ Arduino)
```
Hub (ESP32):             Node (Arduino):
├─ TX2 (GPIO17) ────────→ RX (Pin 10) [SoftwareSerial]
└─ RX2 (GPIO16) ←────── TX (Pin 11) [SoftwareSerial]
├─ GND ─────────────── GND
```

## 💡 Usage Tips

1. **Baud Rate**: Use consistent baud rates (115200 for ESP32, 9600 for Arduino)
2. **Frame Size**: Adjust based on available memory (ESP32: 256B, Arduino: 64-128B)
3. **Timeout**: Set appropriate timeouts for your use case (1-5 seconds typical)
4. **Error Handling**: Monitor error callbacks for network diagnostics

## 🎯 Choosing the Right Example

| **Scenario** | **Recommended Example** | **Why** |
|--------------|------------------------|---------|
| High-speed data exchange | Full-Duplex | Simultaneous communication |
| Resource-constrained | Half-Duplex | Lower pin count, simpler protocol |
| Multiple sensors | Sensor Network | Scalable architecture |
| Master-slave systems | Half-Duplex | Clear request-response pattern |
| Real-time control | Full-Duplex | No communication delays |

## 🔍 Understanding the Communication

### Full-Duplex
- Both devices can send and receive simultaneously
- No collision detection needed
- Maximum throughput
- Requires dedicated TX/RX lines

### Half-Duplex  
- Only one device transmits at a time
- Built-in collision detection and retry
- Shared communication medium
- Master initiates all communication

### Sensor Network
- Hub coordinates all communication
- Nodes report data and respond to commands
- Automatic node registration and health monitoring
- Scalable to multiple sensor nodes

## 🛠️ Customization

Modify these examples for your specific needs:
- Change sensor types and data formats
- Adjust timing intervals
- Add custom commands and responses  
- Implement specific error handling
- Scale to more devices