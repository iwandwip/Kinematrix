# AutoLight V1 - Foundation LED Control System Documentation

## Overview

AutoLight V1 adalah sistem kontrol LED pertama dalam ekosistem Kinematrix yang menjadi fondasi untuk semua versi selanjutnya. Dirancang pada 2023 oleh Kastara Electronics, versi ini memperkenalkan arsitektur dasar dengan konvensi penamaan "Keed" dan sistem kontrol I2C ekspansi.

## Complete Directory Structure

```
AutoLight/ (V1 Foundation)
├── AutoLight.h                     # Main library entry point
├── KeedAutoLight.h/.cpp           # Core LED control engine
├── KeedBase.h                     # Abstract base class for channel control
├── AutoLightChannel/              # LED Channel Management
│   ├── KeedBaseChannel.h/.cpp     # Primary channel implementation
│   └── KeedBaseChannelTask.cpp    # 9 hardcoded LED sequence patterns
├── AutoLightConfig/               # Configuration Management System
│   ├── KeedConfig.h/.cpp          # Configuration class with EEPROM
│   ├── KeedConfigExt.cpp          # Extended configuration features
│   ├── KeedConfigWiFi.cpp         # WiFi configuration (ESP32 only)
│   ├── KeedCustom.h               # Custom hardware definitions
│   ├── KeedDef.h                  # Core constants and enums
│   ├── KeedDisplay.h/.cpp         # OLED/LCD display management
│   ├── KeedDisplayPos.h           # Display positioning utilities
│   └── KeedIndex.h                # Index and enumeration definitions
├── AutoLightStrobe/               # Specialized Lighting Effects
│   └── Keed3ChannelStrobe.h/.cpp  # 3-channel strobe implementation
└── CLAUDE.md                      # V1 specific documentation
```

## Arsitektur Sistem

### Komponen Utama

#### 1. KeedAutoLight (Core Engine)
**File:** `KeedAutoLight.h/.cpp`

Kelas utama yang mengatur seluruh sistem LED dengan fitur:
- **Manajemen Konfigurasi**: Mengelola `configuration_t` untuk pengaturan sistem
- **I2C Expander Management**: Dinamis penambahan dan pengelolaan PCF8574
- **Channel Control**: Abstraksi kontrol channel LED
- **Custom Configuration**: Dukungan konfigurasi hardware khusus

```cpp
class KeedAutoLight {
private:
    configuration_t cfg;           // Konfigurasi sistem
    IOExpander **ioBase;          // Array dinamis I/O expander
    KeedBase *keedBase;           // Base class untuk channel control
    uint8_t ioLen;                // Jumlah I/O expander
    bool isReadyRun;              // Status sistem ready
};
```

#### 2. KeedBase (Abstract Base Class)
**File:** `KeedBase.h`

Base class abstrak yang mendefinisikan interface untuk semua implementasi channel:

```cpp
class KeedBase {
public:
    virtual void init(IOExpander **ioBase, configuration_t _cfg) = 0;
    virtual void update() = 0;
    virtual void run() = 0;
    virtual void setInterruptConfig(interrupt_t _cfg);
    virtual void changeModes();
    virtual void readModes();
};
```

#### 3. KeedBaseChannel (Primary Implementation)
**File:** `AutoLightChannel/KeedBaseChannel.h/.cpp`

Implementasi utama untuk kontrol channel LED dengan fitur:
- **9 Task Sequences**: Mode kontrol LED yang telah diprogramkan
- **Button Interrupt**: Sistem interrupt untuk kontrol tombol
- **Display Integration**: Dukungan untuk LCD/OLED display
- **I2C Expansion**: Dukungan penuh untuk PCF8574

```cpp
class KeedBaseChannel : public KeedBase {
private:
    void (KeedBaseChannel::*taskTemp)();                    // Pointer ke task aktif
    void (KeedBaseChannel::*totalMode[(TASK_SEQUENCE_NUM + 2)])(); // Array semua mode
    
public:
    void taskSequence0();  // Blink patterns
    void taskSequence1();  // Fill 2 point
    void taskSequence2();  // Fill right
    // ... hingga taskSequence8()
};
```

### Sistem Konfigurasi

#### Configuration Structure
```cpp
struct configuration_t {
    volatile uint8_t sequence;      // Mode sequence aktif (0-10)
    volatile uint32_t delay_time;   // Timing delay untuk pattern
    uint8_t version;                // Versi board hardware
    uint8_t channel;                // Total channel LED
    uint8_t io_size;                // Jumlah I/O expander
    uint8_t pin_size;               // Jumlah pin yang digunakan
    uint8_t *pin_ptr;               // Array pointer ke pin
    uint8_t *i2c_ptr;               // Array alamat I2C
    uint8_t *pin_sequence;          // Sequence mapping pin
    bool custom;                    // Mode konfigurasi custom
    bool custom_seq;                // Custom sequence enabled
    bool reverse;                   // Reverse pattern direction
    bool display;                   // Display support enabled
};
```

#### Hardware Detection System
V1 memiliki sistem deteksi hardware otomatis:

```cpp
// Alamat I2C standar untuk PCF8574
const uint8_t i2c_address_arr_t[IO_EXPANDER_ADDRESS_NUM] = {
    I2CCOM_8N0, I2CCOM_8N1, I2CCOM_8N2, I2CCOM_8N3,
    I2CCOM_8N4, I2CCOM_8N5, I2CCOM_8N6, I2CCOM_8N7
};

// Versi board yang didukung
typedef enum {
    MINSYS_V1 = 0x0009B,        // version 1.0
    MINSYS_V2 = 0x000A6,        // version 2.0
    MINSYS_V3 = 0x000B1,        // version 3.0
    MINSYS_V1_EXT = 0x000BC,    // version 1.0-ext-6
    MINSYS_V2_EXT = 0x000C7,    // version 2.0-ext-8
} system_version_t;
```

### LED Sequence Patterns

AutoLight V1 menyediakan **11 total modes** (0-10):

#### Mode 0: OFF
Mematikan semua LED

#### Mode 1: Sequence 0 - Complex Blink Pattern
```cpp
void KeedBaseChannel::taskSequence0() {
    // 1. Basic blink pattern (15 cycles, 2 repetitions)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 15; ++j) {
            // All LEDs ON
            for (int k = 0; k < cfg.pin_size; k++) {
                set(cfg.pin_ptr[k], HIGH);
            }
            sleep(cfg.delay_time);
            
            // All LEDs OFF
            for (int k = 0; k < cfg.pin_size; k++) {
                set(cfg.pin_ptr[k], LOW);
            }
            sleep(cfg.delay_time);
        }
        sleep(500);
    }
    
    // 2. Half blink pattern (alternating halves)
    // First half -> Second half -> First half pattern
    
    // 3. Another half blink variation
    // Second half -> First half -> Second half pattern
}
```

#### Mode 2: Sequence 1 - Fill 2 Point
Mengisi LED secara berpasangan dari tengah ke luar:
```cpp
void KeedBaseChannel::taskSequence1() {
    for (int i = 0; i < cfg.pin_size / 2; i += 2) {
        // Blink pair pattern
        for (int j = 0; j < 8; ++j) {
            set(cfg.pin_ptr[i], HIGH);
            set(cfg.pin_ptr[i + 1], HIGH);
            sleep(cfg.delay_time);
            set(cfg.pin_ptr[i], LOW);
            set(cfg.pin_ptr[i + 1], LOW);
            sleep(cfg.delay_time);
        }
        
        // Keep pair ON
        set(cfg.pin_ptr[i], HIGH);
        set(cfg.pin_ptr[i + 1], HIGH);
        
        // Mirror pattern from end
        if (i < (cfg.pin_size / 2) - 1) {
            // Symmetric pattern from the end
        }
    }
    
    // Turn off sequence
    for (int i = cfg.pin_size; i > 0; --i) {
        set(cfg.pin_ptr[i - 1], LOW);
        sleep(cfg.delay_time * 2);
    }
}
```

#### Mode 3-10: Sequences 2-8
Pattern kompleks lainnya dengan berbagai algoritma:
- **Sequence 2**: Fill Right - Mengisi dari kanan ke kiri
- **Sequence 3**: Fill Left - Mengisi dari kiri ke kanan  
- **Sequence 4**: Center Out - Dari tengah ke luar
- **Sequence 5**: Edge In - Dari pinggir ke tengah
- **Sequence 6**: Wave Pattern - Pola gelombang
- **Sequence 7**: Chase Pattern - Pola kejar-kejaran
- **Sequence 8**: Random Pattern - Pola acak

### Interrupt System

#### Button Interrupt Configuration
```cpp
struct interrupt_t {
    uint8_t pin = 0;                    // Pin untuk tombol
    volatile uint32_t num = 0;          // Counter penekanan
    volatile bool pressed = false;      // Status tombol ditekan
    volatile bool changed = false;      // Status perubahan mode
    void (*isrCallback)() = nullptr;    // Callback function
};
```

#### Debouncing System
```cpp
#define BUTTON_DEBOUNCE_TIME 250

void KeedBaseChannel::changeModes() {
    if (millis() - isrTimer >= BUTTON_DEBOUNCE_TIME) {
        isr.num++;
        isr.pressed = true;
        isrTimer = millis();
    }
}
```

### I2C Expander System

#### Dynamic I/O Expander Addition
```cpp
void KeedAutoLight::addIoExpander(IOExpander *ioExpander) {
    // Realokasi array untuk menambah expander baru
    IOExpander **newIoBase = (IOExpander **) realloc(ioBase, (ioLen + 1) * sizeof(IOExpander *));
    if (newIoBase == nullptr) return;
    
    ioBase = newIoBase;
    ioBase[ioLen] = ioExpander;
    ioLen++;
}
```

#### I2C Initialization
```cpp
bool KeedAutoLight::beginExpander() {
    // Inisialisasi semua PCF8574
    for (int i = 0; i < cfg.io_size; i++) {
        if (!cfg.custom) addIoExpander(new IOExpander(i2c_address_arr_t[i]));
        else addIoExpander(new IOExpander(cfg.i2c_ptr[i]));
    }
    
    // Setup semua pin sebagai OUTPUT dan HIGH (LED OFF)
    for (int i = 0; i < ioLen; i++) {
        for (int j = 0; j < IO_EXPANDER_PIN_NUM; j++) {
            ioBase[i]->pinMode(j, OUTPUT);
        }
        if (!ioBase[i]->begin()) return false;
        for (int j = 0; j < IO_EXPANDER_PIN_NUM; j++) {
            ioBase[i]->digitalWrite(j, HIGH); // LED OFF (common anode)
        }
    }
    return true;
}
```

### Custom Hardware Configurations

V1 mendukung konfigurasi hardware khusus yang telah diprogrammed:

```cpp
enum {
    AUTO_LIGHT_CUSTOM_0,    // Basic custom config
    AUTO_LIGHT_CUSTOM_1,    // Extended custom config
    AUTO_LIGHT_CUSTOM_2,    // 3-channel strobe (non-I2C)
    AUTO_LIGHT_CUSTOM_3,    // 16 channel, 2 PCF (0x24, 0x20)
    AUTO_LIGHT_CUSTOM_4,    // 12 channel, 2 PCF (0x38, 0x21)
    AUTO_LIGHT_CUSTOM_5,    // 12 channel, 2 PCF (0x20, 0x21)
};
```

### EEPROM Storage System

#### Memory Layout
```cpp
typedef enum {
    SERIAL_NUMBER_ADDRESS = 0,      // Device serial number
    MODE_ADDRESS = 25,              // Current sequence mode
    DELAY_ADDRESS = 30,             // Delay timing
    WIFI_STATE_ADDRESS = 50,        // WiFi connection state
    SSID_ADDRESS = 60,              // WiFi SSID
    PASSWORD_ADDRESS = 85,          // WiFi password
} eeprom_address_offset_t;
```

#### ESP32 vs AVR Memory Access
```cpp
#if defined(ESP32)
#define readMEM(x) EEPROM.readString(x)
#define writeMEM(x, y) EEPROM.writeString(x, y); EEPROM.commit()
#else
void writeMEM(int addrOffset, const String &strToWrite);
String readMEM(int addrOffset);
#endif
```

### Display Integration

#### KeedDisplay Support
V1 memiliki dukungan built-in untuk display OLED:

```cpp
if (cfg.display) {
    display = new KeedDisplay(cfg.channel, 0x3C);
    display->clear();
    display->fillBorder();
}
```

### Indicator System

#### Visual/Audio Feedback
```cpp
typedef enum {
    LED_RED = 14,
    LED_GREEN = 12,
    LED_BLUE = 13,
    BUZZER = 15,
    INDICATOR_NUM = 4
} indicator_pin_t;

struct indicator_t {
    DigitalOut outs[4];      // 3 LED + 1 Buzzer
    TimerTask times[4];      // Timing untuk masing-masing
    void show(uint8_t _seq); // Tampilkan indikator sequence
};
```

## Penggunaan dan API

### Basic Setup
```cpp
#define ENABLE_ADDONS_AUTOLIGHT
#include "Kinematrix.h"

KeedConfiguration cfg;
KeedAutoLight led;

void setup() {
    // Konfigurasi dasar
    configuration_t config;
    config.sequence = 0;            // Mulai dari mode 0 (OFF)
    config.setDelay(50);           // 50ms delay default
    config.reverse = false;        // Normal direction
    config.display = true;         // Enable display
    
    // Set configuration
    cfg.setConfig(config);
    
    // Baca konfigurasi hardware
    cfg_error_t err_ch = cfg.readChannel();
    cfg_error_t err_ver = cfg.readVersion();
    
    if (err_ch == INITIALIZE_OK && err_ver == INITIALIZE_OK) {
        led.setChannel(cfg.getConfig());
        led.init();
    }
}

void loop() {
    if (led.isReady()) {
        led.runAutoLight();
    }
}
```

### Custom Pin Sequence
```cpp
configuration_t config;
config.channel = 24;
config.custom_seq = true;

// Atur urutan pin custom (contoh: 3x8 matrix)
config.setPinSequence(24, 
    1,2,4,5,6,7,8,9,           // Baris 1
    10,11,12,3,22,13,14,15,    // Baris 2  
    16,17,18,19,20,21,23,24    // Baris 3
);
```

### I2C Custom Configuration
```cpp
// Mode I2C dengan alamat custom
config.custom = true;
config.io_size = 3;
config.setAddress(3, 0x27, 0x45, 0x67);  // 3 PCF dengan alamat khusus
```

### Interrupt Setup
```cpp
interrupt_t interrupt_cfg;
interrupt_cfg.attachInterrupt(2, buttonISR);  // Pin 2, callback buttonISR

led.setInterruptConfig(interrupt_cfg);

// ISR function
void IRAM_ATTR buttonISR() {
    led.getChannelClass().changeModes();
}
```

## Limitasi dan Karakteristik

### Kapasitas Hardware
- **Maximum Channels**: 64 (8 PCF × 8 pin)
- **Maximum PCF Expanders**: 8
- **Maximum Versions**: 32 board variants
- **Sequences**: 9 hardcoded patterns + OFF + ON

### Platform Support
- **ESP32**: Full support dengan WiFi dan advanced features
- **ESP8266**: Basic support
- **AVR (Arduino Uno/Nano)**: Limited support

### Memory Requirements
- **Base System**: ~30KB Flash, ~2KB RAM
- **With Display**: +~10KB Flash
- **Full Configuration**: ~50KB Flash total

## Error Handling

### Configuration Errors
```cpp
typedef enum {
    INITIALIZE_OK = 6,
    I2C_ADDRESS_ERROR = 255,
    VERSION_ADDRESS_ERROR = 254,
    CHANNEL_NUM_ERROR = 253,
    CONFIG_ERROR = 252,
    SYSTEM_VERSION_ERROR = 251,
    INITIALIZE_ERROR = 250,
} cfg_error_t;
```

### Error Recovery
```cpp
cfg_error_t error = led.setChannel(config);
if (error != INITIALIZE_OK) {
    Serial.print("Configuration Error: ");
    Serial.println(error);
    // Implement fallback configuration
}
```

## Advanced Features

### WiFi Integration (ESP32 Only)
```cpp
#if defined(ESP32)
class KeedWiFi {
    WiFiServer server;
    IPAddress IP;
public:
    KeedWiFi(const char *_ssid, const char *_password = NULL);
    int createWiFIAP(const char *_ssid, const char *_password = NULL);
    void beginServer();
    void runServer();
};
#endif
```

### Multi-Core Support (ESP32)
```cpp
class KeedCore {
    uint8_t index;
    uint32_t stack_depth;
public:
    void createCore(uint8_t _core_index, void (*core_callback)(void *pvParameter));
};
```

### Debug Information
```cpp
void KeedAutoLight::showInfo() {
    Serial.print("| SERIAL-KEY: "); Serial.print(readMEM(0));
    Serial.print("| INDEX: "); Serial.print(getIndex());
    Serial.print("| version: "); Serial.print(cfg.version);
    Serial.print("| channel: "); Serial.print(cfg.channel);
    Serial.print("| io_size: "); Serial.print(cfg.io_size);
    Serial.print("| isUsingExpander(): "); 
    Serial.print(isUsingExpander() ? "YES" : "NO");
}
```

## Legacy Status dan Evolution Path

AutoLight V1 merupakan **foundation architecture** yang menetapkan:

1. **Naming Convention**: "Keed" prefix untuk konsistensi
2. **I2C Standard**: PCF8574 sebagai standar I/O expansion
3. **Configuration System**: Struktur konfigurasi yang fleksibel
4. **Sequence Pattern**: Template untuk LED pattern development
5. **Hardware Abstraction**: Base untuk multi-platform support

### Path to V2
- Namespace organization (`AutoLight::`)
- Web server integration
- Enhanced task management
- Better memory management

### Path to V3  
- Dynamic PCF distribution
- Multi-modal button system
- Advanced configuration management

### Path to V4
- Fully dynamic configuration
- Auto-detection capabilities
- Builder pattern API
- Hot-swap support

## Kesimpulan

AutoLight V1 adalah sistem yang solid dan well-designed yang menjadi foundation untuk semua pengembangan selanjutnya. Dengan 9 sequence patterns yang kompleks, sistem I2C expansion yang robust, dan konfigurasi yang fleksibel, V1 memberikan base yang kuat untuk aplikasi LED control automotive dan industrial.

Kelebihan utama V1:
- **Stable and Tested**: Arsitektur yang proven dan reliable
- **Hardware Agnostic**: Mendukung berbagai konfigurasi hardware
- **Rich Patterns**: 9 complex LED sequences built-in
- **Flexible Configuration**: Custom pin mapping dan I2C addressing
- **Display Integration**: Built-in support untuk feedback visual

V1 sangat cocok untuk:
- **Educational Projects**: Memahami konsep dasar LED control
- **Simple Applications**: Proyek dengan requirement yang straightforward
- **Legacy Hardware**: Sistem yang sudah established dengan V1
- **Resource Constrained**: Platform dengan keterbatasan memory