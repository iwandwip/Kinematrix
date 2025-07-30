# Panduan Membuat TaskSequence Baru - AutoLight V3

## Deskripsi
Panduan lengkap untuk menambahkan mode/sequence LED baru pada sistem AutoLight V3. Sistem ini menggunakan interrupt-driven architecture yang memungkinkan perubahan mode secara real-time meskipun sequence sedang berjalan.

---

## 📋 Persyaratan

- AutoLight V3 library telah terinstall
- ESP32 development board
- PCF8574 I2C expander (opsional)
- LED strip atau individual LEDs
- Basic knowledge C++ dan Arduino IDE

---

## 🏗️ Arsitektur TaskSequence

### Struktur Dasar
```
TaskSequence System:
├── Constants.h          → Definisi konstanta
├── BaseChannel.h        → Deklarasi function
├── BaseChannelSequence.cpp → Implementasi sequence
├── BaseChannel.cpp      → Registrasi function
└── Apps.ino            → Konfigurasi total sequence
```

### Mode yang Tersedia
- **Mode 0**: OFF (sistem mati)
- **Mode 1**: ON (semua LED nyala)
- **Mode 2-15**: Custom sequences (14 mode)
- **Mode 16+**: Mode tambahan (dapat ditambah)

---

## 🚀 Langkah-langkah Membuat TaskSequence Baru

### STEP 1: Update Konstanta (Opsional)

Jika ingin menambah total sequence di atas 16:

```cpp
// File: Common/Constants.h
namespace AutoLight {
    namespace Constants {
        const int MAXNUM_TOTAL_TASK_SEQUENCE = 17; // Ubah dari 16 ke 17
    }
}
```

### STEP 2: Tambah Deklarasi Function

```cpp
// File: Channel/BaseChannel.h
// Tambahkan di section private:
class BaseChannel {
private:
    // ... existing functions
    void taskSequence16(); // ⭐ TAMBAH INI
};
```

### STEP 3: Implementasi Sequence Logic

```cpp
// File: Channel/BaseChannelSequence.cpp
// Tambahkan function baru di akhir file:

void BaseChannel::taskSequence16() {
    // ⭐ IMPLEMENTASI PATTERN LED KAMU DI SINI
    
    // Contoh 1: Blink Pattern
    static bool state = false;
    
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        int pcf_index = i / 8;
        int pin_index = i % 8;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, state ? HIGH : LOW);
    }
    
    state = !state;
    delay(500); // Blink setiap 500ms
}
```

### STEP 4: Registrasi Function

```cpp
// File: Channel/BaseChannel.cpp
// Cari function setTaskSequenceFunction() dan tambahkan:

void BaseChannel::setTaskSequenceFunction() {
    total_mode_[0] = &BaseChannel::taskSequence0;   // OFF
    total_mode_[1] = &BaseChannel::taskSequence1;   // ON
    total_mode_[2] = &BaseChannel::taskSequence2;   // BLINK
    // ... existing sequences
    total_mode_[15] = &BaseChannel::taskSequence15;
    total_mode_[16] = &BaseChannel::taskSequence16; // ⭐ TAMBAH INI
}
```

### STEP 5: Update Total Sequence Count

```cpp
// File: Apps.ino (atau file aplikasi kamu)
void setup() {
    // ... existing setup
    led.setTotalSequence(17); // ⭐ UPDATE dari 16 ke 17
}
```

---

## 🎨 Template Pattern LED

### 1. Pattern Blink Sederhana
```cpp
void BaseChannel::taskSequenceX() {
    static bool state = false;
    
    // Nyalakan/matikan semua LED
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        int pcf_index = i / 8;
        int pin_index = i % 8;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, state ? HIGH : LOW);
    }
    
    state = !state;
    delay(300); // Blink speed
}
```

### 2. Pattern Chase (Bergerak)
```cpp
void BaseChannel::taskSequenceX() {
    static int currentLed = 0;
    
    // Matikan semua LED
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        int pcf_index = i / 8;
        int pin_index = i % 8;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, LOW);
    }
    
    // Nyalakan LED saat ini
    int pcf_index = currentLed / 8;
    int pin_index = currentLed % 8;
    expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, HIGH);
    
    // Pindah ke LED berikutnya
    currentLed = (currentLed + 1) % config_data_ptr_->header.pin_size_;
    
    delay(100); // Chase speed
}
```

### 3. Pattern Wave (Gelombang)
```cpp
void BaseChannel::taskSequenceX() {
    static int wavePosition = 0;
    static int direction = 1;
    
    // Matikan semua LED
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        int pcf_index = i / 8;
        int pin_index = i % 8;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, LOW);
    }
    
    // Nyalakan 3 LED berurutan (wave effect)
    for (int offset = -1; offset <= 1; offset++) {
        int ledIndex = wavePosition + offset;
        if (ledIndex >= 0 && ledIndex < config_data_ptr_->header.pin_size_) {
            int pcf_index = ledIndex / 8;
            int pin_index = ledIndex % 8;
            expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, HIGH);
        }
    }
    
    // Update posisi wave
    wavePosition += direction;
    if (wavePosition >= config_data_ptr_->header.pin_size_ - 1 || wavePosition <= 0) {
        direction *= -1; // Balik arah
    }
    
    delay(80); // Wave speed
}
```

### 4. Pattern Random
```cpp
void BaseChannel::taskSequenceX() {
    // Matikan semua LED
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        int pcf_index = i / 8;
        int pin_index = i % 8;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, LOW);
    }
    
    // Nyalakan 5 LED random
    for (int i = 0; i < 5; i++) {
        int randomLed = random(0, config_data_ptr_->header.pin_size_);
        int pcf_index = randomLed / 8;
        int pin_index = randomLed % 8;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, HIGH);
    }
    
    delay(200); // Random change speed
}
```

### 5. Pattern dengan Timing Control
```cpp
void BaseChannel::taskSequenceX() {
    static unsigned long lastUpdate = 0;
    static int step = 0;
    
    // Control timing tanpa blocking
    if (millis() - lastUpdate > 150) {
        // Update pattern
        for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
            int pcf_index = i / 8;
            int pin_index = i % 8;
            
            // Pattern matematika
            bool ledState = (i + step) % 4 < 2;
            expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, ledState ? HIGH : LOW);
        }
        
        step = (step + 1) % 8;
        lastUpdate = millis();
    }
    
    // Non-blocking - interrupt tetap responsif
}
```

---

## 🔧 Tips & Best Practices

### 1. Gunakan Static Variables
```cpp
// ✅ BENAR - State tersimpan antar pemanggilan
static int counter = 0;
static bool direction = true;
static unsigned long lastTime = 0;

// ❌ SALAH - State hilang setiap pemanggilan
int counter = 0;
```

### 2. Akses PCF8574 yang Benar
```cpp
// Formula akses LED:
int pcf_index = led_number / 8;    // PCF ke berapa (0-7)
int pin_index = led_number % 8;    // Pin dalam PCF (0-7)

// Kontrol LED:
expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, HIGH/LOW);
```

### 3. Timing Control
```cpp
// ✅ Blocking (interrupt tetap jalan)
delay(100);

// ✅ Non-blocking (lebih responsif)
static unsigned long lastTime = 0;
if (millis() - lastTime > 100) {
    // Update pattern
    lastTime = millis();
}
```

### 4. Check Mode Change (Opsional)
```cpp
void BaseChannel::taskSequenceX() {
    for (int cycle = 0; cycle < 100; cycle++) {
        // Pattern logic here
        
        delay(50);
        
        // Exit early jika ada interrupt
        if (channel_data_.is_mode_changed_) {
            return;
        }
    }
}
```

---

## 🧪 Testing & Debugging

### 1. Test Sequence Baru
```cpp
// Di Apps.ino atau main code:
led.changeModeApp(16); // Test sequence ke-16
```

### 2. Debug Pattern
```cpp
void BaseChannel::taskSequenceX() {
    Serial.print("Sequence X running, step: ");
    Serial.println(step_counter);
    
    // Pattern logic here
}
```

### 3. Monitor Button Interrupt
```cpp
// Check apakah interrupt berfungsi
void loop() {
    if (led.getSequenceIndex() != last_sequence) {
        Serial.println("Mode changed by interrupt!");
        last_sequence = led.getSequenceIndex();
    }
}
```

---

## ⚡ Interrupt System

### Cara Kerja Interrupt
1. **User tekan button** → Hardware interrupt triggered
2. **ISR (Interrupt Service Routine)** → Set volatile flags
3. **Main loop** → Detect flag changes
4. **Mode switch** → Langsung ganti ke sequence baru
5. **TaskSequence** → Mulai eksekusi pattern baru

### Button Modes Tersedia
- **4-Button Mode**: ON/OFF/NEXT/PREV
- **1-Button Mode**: Cycle through all sequences
- **2-Button Mode**: Toggle + Next
- **3-Button Mode**: Separate ON/OFF/Next

### Interrupt Response Time
- **Hardware interrupt**: < 1ms
- **Mode switching**: < 5ms
- **Pattern change**: Immediate

---

## 📁 File Structure Reference

```
AutoLightv3/
├── Common/
│   └── Constants.h              # Konstanta global
├── Channel/
│   ├── BaseChannel.h            # Deklarasi class & functions
│   ├── BaseChannel.cpp          # Constructor & registrasi
│   └── BaseChannelSequence.cpp  # Implementasi sequences
├── Config/
│   ├── ConfigData.h            # Struktur data
│   └── BaseConfig.h            # Konfigurasi I2C
└── ButtonInterrupt.h           # Sistem interrupt
```

---

## 🚨 Troubleshooting

### Error: Function tidak terdefinisi
**Solusi**: Pastikan deklarasi di `BaseChannel.h` dan implementasi di `BaseChannelSequence.cpp`

### Error: Sequence tidak jalan
**Solusi**: Cek registrasi di `setTaskSequenceFunction()` dan `setTotalSequence()`

### LED tidak nyala
**Solusi**: Pastikan `config_data_ptr_->header.pin_size_` sudah benar dan PCF8574 terhubung

### Button tidak responsif
**Solusi**: Pastikan interrupt pin dan pull-up resistor terpasang dengan benar

---

## 📚 Contoh Lengkap

### File: BaseChannel.h
```cpp
// Tambah deklarasi
private:
    void taskSequence16(); // Rainbow pattern
    void taskSequence17(); // Scanner pattern
```

### File: BaseChannelSequence.cpp
```cpp
void BaseChannel::taskSequence16() {
    // Rainbow pattern implementation
    static int colorStep = 0;
    
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        int pcf_index = i / 8;
        int pin_index = i % 8;
        
        // Rainbow math
        bool state = (i + colorStep) % 6 < 3;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, state ? HIGH : LOW);
    }
    
    colorStep = (colorStep + 1) % 24;
    delay(100);
}

void BaseChannel::taskSequence17() {
    // Scanner pattern (seperti KITT car)
    static int position = 0;
    static int direction = 1;
    
    // Clear all
    for (int i = 0; i < config_data_ptr_->header.pin_size_; i++) {
        int pcf_index = i / 8;
        int pin_index = i % 8;
        expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, LOW);
    }
    
    // Light up scanner
    for (int offset = 0; offset < 4; offset++) {
        int ledPos = position + offset;
        if (ledPos < config_data_ptr_->header.pin_size_) {
            int pcf_index = ledPos / 8;
            int pin_index = ledPos % 8;
            expander_io_.io_base_[pcf_index]->digitalWrite(pin_index, HIGH);
        }
    }
    
    position += direction;
    if (position >= config_data_ptr_->header.pin_size_ - 4 || position <= 0) {
        direction *= -1;
    }
    
    delay(60);
}
```

### File: BaseChannel.cpp
```cpp
void BaseChannel::setTaskSequenceFunction() {
    // ... existing sequences
    total_mode_[16] = &BaseChannel::taskSequence16; // Rainbow
    total_mode_[17] = &BaseChannel::taskSequence17; // Scanner
}
```

### File: Apps.ino
```cpp
void setup() {
    led.setTotalSequence(18); // Update total
    
    // Test new sequences
    led.changeModeApp(16); // Start with rainbow
}
```

---

## 🎉 Selamat!

Kamu sekarang bisa membuat TaskSequence LED yang kreatif dan responsif. Sistem interrupt AutoLight V3 memastikan user experience yang smooth dengan response time yang cepat.

**Happy Coding!** 🚀

---

*Dokumentasi ini dibuat untuk AutoLight V3 Library - Kinematrix Framework*