# DWIN HMI Complete Knowledge Base

## Hardware Architecture

### T5L ASIC Specifications
- **Dual-Core 8051**: GUI Core + OS Core
- **Clock**: 600MHz dual core, main frequency = crystal × 56/3
- **Memory**: 1MB NOR Flash (512KB user database)
- **Package**: ELQFP128 (16×16×1.5mm)
- **I/O**: 28 ports (P0-P2: 8-bit, P3.0-P3.3: 4-bit)
- **Interfaces**: CAN, PWM, AD, UARTs, I2C, SPI
- **Performance**: 130-150 instructions per microsecond
- **Rewrite Cycle**: >100,000 times

## DGUS Software Ecosystem

### Development Tools
- **DGUS V7.6**: Latest GUI development environment
- **DGUS SDK V4.9**: Software development kit
- **T5L DGUSII**: Application development platform
- **DWIN OS Platform**: Low-level programming environment

### File Structure
```
DWIN_SET/                  # SD Card root folder
├── T5LCFG.CFG            # Configuration file
├── 0_DWIN_ASC.HZK        # Font file
├── 14*.BIN               # Variable config (32 bytes/command)
├── 32.ICL                # Image library
├── DGUS_*.BIN            # Firmware updates
└── [resources]           # Images, sounds, etc.
```

## Communication Protocol

### UART Frame Structure
```
[Header H][Header L][Byte Count][Command][Data...][CRC H][CRC L]
   5A        A5        Length      CMD     Payload   Optional
```

### Core Commands
- **0x80/0x81**: Register read/write
- **0x82**: Variable write (VP write)
- **0x83**: Variable read (VP read)
- **0x84**: Curve data write
- **0x85**: Touch event (custom)

### Variable Management
- **VP (Variable Pointer)**: Data storage addresses
- **SP (System Pointer)**: Display property descriptors
- **Data Types**: Type 0,5=2 bytes; Type 1,2,3,6=4 bytes; Type 4=8 bytes

## Development Workflow

### Phase 1: Design (DGUS Software)
1. Create background images dan UI elements
2. Configure variable addresses (VP mapping)
3. Setup touch controls dan display objects
4. Generate font files (.HZK)
5. Compile to binary files (.BIN)

### Phase 2: Programming (SD Card)
1. Format SD card: FAT32, ≤8GB capacity
2. Create DWIN_SET folder
3. Copy all .BIN files to DWIN_SET
4. Insert SD card dengan display OFF
5. Power on → blue upload screen → completion

### Phase 3: Integration (UART)
```cpp
void sendToDisplay(uint16_t vp, uint16_t value) {
    Serial.write(0x5A); Serial.write(0xA5);
    Serial.write(0x05); Serial.write(0x82);
    Serial.write(vp >> 8); Serial.write(vp & 0xFF);
    Serial.write(value >> 8); Serial.write(value & 0xFF);
}
```

## Advanced Features

### Dual Development Systems
- **DGUSII Mode**: Visual GUI development (no coding)
- **TA Mode**: Instruction set programming (assembly-like)
- **DWIN OS**: Full C51 programming environment

### Touch Event System
- Touch coordinates available via VP
- Button press events dengan return codes
- Multi-touch support pada capacitive models
- Gesture recognition capabilities

### Graphics Capabilities
- Image compression untuk storage optimization
- Animation support dengan frame sequences
- Real-time graphing untuk data visualization
- Custom fonts dan multi-language support

## Official Resources

### Documentation Sources
- **Official Website**: dwin-global.com/development-guide/
- **T5L_DGUSII Application Development Guide V2.9**
- **Development Guide of T5L ASIC**
- **DWIN OS Development Guide**

### Community Support
- **Official Forum**: forums.dwin-global.com
- **T5L DGUS2 Section**: Dedicated support area
- **Arduino Forum**: Active DWIN programming discussions
- **GitHub**: Official Arduino library available

## Hardware Models

### Popular Display Models
- **DMG80480C043_03WTC**: 4.3", 800×480 capacitive
- **DMT48270T043_3WT**: 4.3" resistive touchscreen
- **DMG Series**: Basic graphic LCD displays
- **DMT Series**: TFT touchscreen dengan T5L ASIC
- **DWC Series**: Capacitive touchscreen variants

## Programming Examples

### Arduino Integration
```cpp
#include <SoftwareSerial.h>
SoftwareSerial dwinSerial(2, 3);

void setup() {
    dwinSerial.begin(115200);
    writeVP(0x1000, 1234);  // Write number to VP
}

void writeVP(uint16_t vp, uint16_t value) {
    dwinSerial.write(0x5A); dwinSerial.write(0xA5);
    dwinSerial.write(0x05); dwinSerial.write(0x82);
    dwinSerial.write(vp >> 8); dwinSerial.write(vp & 0xFF);
    dwinSerial.write(value >> 8); dwinSerial.write(value & 0xFF);
}
```

### ESP32 Integration
```cpp
void setup() {
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    setPage(0);
    setBrightness(80);
}

void setPage(uint8_t page) {
    Serial2.write(0x5A); Serial2.write(0xA5);
    Serial2.write(0x04); Serial2.write(0x80);
    Serial2.write(0x03); Serial2.write(0x00);
    Serial2.write(page);
}
```

## Common Issues & Solutions

### Communication Problems
- **Check baud rate**: 115200 optimal untuk ESP32, 9600 untuk Arduino
- **Verify wiring**: TX/RX tidak terbalik, ground connection
- **Power supply**: 5V minimum 500mA
- **Electromagnetic interference**: Use shielded cables

### SD Card Issues
- **Format**: FAT32 only, ≤8GB capacity
- **Folder structure**: DWIN_SET folder required
- **File naming**: Follow DGUS conventions
- **Insert timing**: SD card inserted dengan display OFF

### Variable Address Conflicts
- **Unique VP addresses**: Avoid overlapping ranges
- **Type compatibility**: Match data types dengan DGUS config
- **Read-only VPs**: Check DGUS configuration

## Performance Characteristics

### Memory Usage
- **RAM**: ~2KB untuk basic operations
- **Flash**: ~15KB library code
- **Variable Storage**: 32 VP maximum

### Communication Performance
- **Baud Rate**: 115200 (ESP32), 9600 (Arduino)
- **Frame Latency**: <10ms response time
- **Update Rate**: 100Hz recommended

### Platform Compatibility
- **ESP32**: Full feature set, Serial2 support
- **ESP8266**: Limited features, SoftwareSerial
- **Arduino Uno/Nano**: Basic functionality only

## CeriaDisplayDWIN Implementation

### Key Features
- **Complete T5L Protocol**: All commands implemented
- **VP Management**: 32 variables dengan type-safe operations
- **Event Callbacks**: Touch, page, error, variable callbacks
- **Error Handling**: 7 error types dengan recovery
- **Platform Support**: ESP32, ESP8266, Arduino
- **No STD Library**: Pure Arduino/embedded implementation

### API Highlights
```cpp
CeriaDisplayDWIN display;
display.begin(Serial2, 115200);
display.setPage(0);
display.writeVP(0x1000, 25.6f);
display.setText(0x1001, "Hello World");
display.setTouchCallback(handleTouch);
```

### Integration Benefits
- **CeriaSerial Compatible**: Unified communication
- **Modular Design**: Fits CeriaDev ecosystem
- **Event-driven**: Real-time responsiveness
- **Debug Support**: Comprehensive monitoring
- **Production Ready**: Error handling & recovery

## Technical Specifications Summary

### Hardware Limits
- **Display Resolution**: Up to 1024×600
- **Touch Points**: Single/multi-touch support
- **Memory**: 512KB user space
- **Update Rate**: 60Hz display refresh
- **Temperature**: -20°C to +70°C operating

### Software Limits
- **Variables**: 512 VP addresses maximum
- **Pages**: 255 pages maximum
- **Images**: 65535 images maximum
- **Fonts**: Multiple font files support
- **Animations**: 255 frames per animation

### Communication Specs
- **UART Speed**: 9600-115200 baud
- **Frame Size**: 3-255 bytes
- **CRC**: 16-bit polynomial 0xA001
- **Timeout**: Configurable (default 2000ms)
- **Buffer**: Platform adaptive (64B-256B)

This knowledge base covers complete DWIN HMI ecosystem untuk embedded development dengan fokus pada practical implementation dan integration patterns.