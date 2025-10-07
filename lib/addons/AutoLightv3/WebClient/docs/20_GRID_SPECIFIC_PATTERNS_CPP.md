# Grid-Specific LED Patterns Implementation Plan - C++ Version

## Overview

Dokumen ini berisi rencana implementasi 10 mode tambahan untuk AutoLight V3 yang dioptimalkan secara spesifik untuk grid layout 4×4 dalam area 8×6. Layout ini berdasarkan screenshot yang menunjukkan 16 channel terdistribusi dalam 4 baris × 4 kolom.

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

### Channel Position Mapping
```cpp
// Array positions untuk 16 channel
const int gridPositions[16][2] = {
    {0,0}, {0,1}, {0,2}, {0,3},  // Channel 0-3
    {1,0}, {1,1}, {1,2}, {1,3},  // Channel 4-7
    {2,0}, {2,1}, {2,2}, {2,3},  // Channel 8-11
    {3,0}, {3,1}, {3,2}, {3,3}   // Channel 12-15
};

// Helper function untuk grid mapping
int getChannelFromGridPosition(int row, int col) {
    if (row >= 0 && row < 4 && col >= 0 && col < 4) {
        return row * 4 + col;
    }
    return -1;  // Invalid position
}
```

## Pattern Implementations

### 1. Row Wave Pattern (taskSequence16)
```cpp
void BaseChannel::taskSequence16RowWave() {
    // Gelombang horizontal melintasi 4 baris
    for (int pass = 0; pass < 3; pass++) {
        for (int row = 0; row < 4; row++) {
            // Aktifkan seluruh baris
            for (int col = 0; col < 4; col++) {
                int channel = row * 4 + col;
                if (channel < 16) {
                    set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                }
            }
            sleep(channel_data_.delay_time_ * 2);

            // Matikan baris
            for (int col = 0; col < 4; col++) {
                int channel = row * 4 + col;
                if (channel < 16) {
                    set(config_data_ptr_->header.pin_ptr_[channel], LOW);
                }
            }
            sleep(channel_data_.delay_time_);
        }
        sleep(channel_data_.delay_time_ * 2);
    }
}
```

### 2. Column Fill Pattern (taskSequence17)
```cpp
void BaseChannel::taskSequence17ColumnFill() {
    // Pengisian vertikal melintasi 4 kolom
    for (int col = 0; col < 4; col++) {
        // Fill dari atas ke bawah
        for (int row = 0; row < 4; row++) {
            int channel = row * 4 + col;
            if (channel < 16) {
                set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                sleep(channel_data_.delay_time_);
            }
        }

        sleep(channel_data_.delay_time_ * 3);

        // Clear dari bawah ke atas
        for (int row = 3; row >= 0; row--) {
            int channel = row * 4 + col;
            if (channel < 16) {
                set(config_data_ptr_->header.pin_ptr_[channel], LOW);
                sleep(channel_data_.delay_time_);
            }
        }
    }
}
```

### 3. Diagonal Cross Pattern (taskSequence18)
```cpp
void BaseChannel::taskSequence18DiagonalCross() {
    // Diagonal utama (kiri atas ke kanan bawah)
    for (int i = 0; i < 4; i++) {
        int channel = i * 4 + i;  // [0,0], [1,1], [2,2], [3,3]
        if (channel < 16) {
            set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
            sleep(channel_data_.delay_time_);
        }
    }

    sleep(channel_data_.delay_time_ * 2);

    // Diagonal sekunder (kanan atas ke kiri bawah)
    for (int i = 0; i < 4; i++) {
        int channel = i * 4 + (3 - i);  // [0,3], [1,2], [2,1], [3,0]
        if (channel < 16) {
            set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
            sleep(channel_data_.delay_time_);
        }
    }

    sleep(channel_data_.delay_time_ * 3);
    off();
}
```

### 4. Spiral Square Pattern (taskSequence19)
```cpp
void BaseChannel::taskSequence19SpiralSquare() {
    // Spiral melingkar pada grid 4x4
    int spiralOrder[16] = {
        0, 1, 2, 3,      // Top row
        7, 11, 15, 14,    // Right column + bottom
        13, 12, 9, 8,     // Bottom row + left column
        4, 5, 6, 10       // Inner square
    };

    for (int i = 0; i < 16; i++) {
        int channel = spiralOrder[i];
        if (channel < 16) {
            set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
            sleep(channel_data_.delay_time_);
        }
    }

    sleep(channel_data_.delay_time_ * 2);

    // Reverse spiral
    for (int i = 15; i >= 0; i--) {
        int channel = spiralOrder[i];
        if (channel < 16) {
            set(config_data_ptr_->header.pin_ptr_[channel], LOW);
            sleep(channel_data_.delay_time_);
        }
    }
}
```

### 5. Checkerboard Pattern (taskSequence20)
```cpp
void BaseChannel::taskSequence20Checkerboard() {
    // Pola kotak-kotak classic
    for (int cycle = 0; cycle < 4; cycle++) {
        // Black squares (even positions)
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                if ((row + col) % 2 == 0) {
                    int channel = row * 4 + col;
                    if (channel < 16) {
                        set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                    }
                }
            }
        }
        sleep(channel_data_.delay_time_ * 2);
        off();
        sleep(channel_data_.delay_time_);

        // White squares (odd positions)
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                if ((row + col) % 2 == 1) {
                    int channel = row * 4 + col;
                    if (channel < 16) {
                        set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                    }
                }
            }
        }
        sleep(channel_data_.delay_time_ * 2);
        off();
        sleep(channel_data_.delay_time_);
    }
}
```

### 6. Frame Pattern (taskSequence21)
```cpp
void BaseChannel::taskSequence21Frame() {
    // Pola bingkai/frame
    int frameChannels[] = {
        0, 1, 2, 3,      // Top edge
        7, 11, 15, 14,    // Right edge + bottom
        13, 12, 9, 8,     // Bottom edge + left
        4                  // Complete the frame
    };

    for (int cycle = 0; cycle < 3; cycle++) {
        // Draw frame
        for (int i = 0; i < 12; i++) {
            int channel = frameChannels[i];
            if (channel < 16) {
                set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                sleep(channel_data_.delay_time_ / 2);
            }
        }

        sleep(channel_data_.delay_time_ * 2);

        // Fill center
        for (int center = 5; center <= 10; center++) {
            if (center != 7 && center != 11) {  // Skip frame positions
                set(config_data_ptr_->header.pin_ptr_[center], HIGH);
                sleep(channel_data_.delay_time_);
            }
        }

        sleep(channel_data_.delay_time_ * 2);
        off();
    }
}
```

### 7. Snake Matrix Pattern (taskSequence22)
```cpp
void BaseChannel::taskSequence22SnakeMatrix() {
    // Pola ular ularan melintasi grid 4x4
    for (int cycle = 0; cycle < 2; cycle++) {
        // Forward snake
        for (int row = 0; row < 4; row++) {
            if (row % 2 == 0) {
                // Kiri ke kanan
                for (int col = 0; col < 4; col++) {
                    int channel = row * 4 + col;
                    if (channel < 16) {
                        set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                        sleep(channel_data_.delay_time_);
                    }
                }
            } else {
                // Kanan ke kiri
                for (int col = 3; col >= 0; col--) {
                    int channel = row * 4 + col;
                    if (channel < 16) {
                        set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                        sleep(channel_data_.delay_time_);
                    }
                }
            }
        }

        sleep(channel_data_.delay_time_ * 2);
        off();

        // Reverse snake
        for (int row = 3; row >= 0; row--) {
            if (row % 2 == 0) {
                // Kiri ke kanan
                for (int col = 0; col < 4; col++) {
                    int channel = row * 4 + col;
                    if (channel < 16) {
                        set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                        sleep(channel_data_.delay_time_);
                    }
                }
            } else {
                // Kanan ke kiri
                for (int col = 3; col >= 0; col--) {
                    int channel = row * 4 + col;
                    if (channel < 16) {
                        set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
                        sleep(channel_data_.delay_time_);
                    }
                }
            }
        }

        sleep(channel_data_.delay_time_ * 2);
        off();
    }
}
```

### 8. Cross Pattern (taskSequence23)
```cpp
void BaseChannel::taskSequence23Cross() {
    // Pola cross horizontal dan vertikal
    for (int cycle = 0; cycle < 3; cycle++) {
        // Horizontal cross (baris tengah)
        for (int col = 0; col < 4; col++) {
            int channel1 = 1 * 4 + col;  // Row 1
            int channel2 = 2 * 4 + col;  // Row 2

            if (channel1 < 16) set(config_data_ptr_->header.pin_ptr_[channel1], HIGH);
            if (channel2 < 16) set(config_data_ptr_->header.pin_ptr_[channel2], HIGH);
            sleep(channel_data_.delay_time_);
        }

        sleep(channel_data_.delay_time_ * 2);

        // Vertical cross (kolom tengah)
        for (int row = 0; row < 4; row++) {
            int channel1 = row * 4 + 1;  // Col 1
            int channel2 = row * 4 + 2;  // Col 2

            if (channel1 < 16) set(config_data_ptr_->header.pin_ptr_[channel1], HIGH);
            if (channel2 < 16) set(config_data_ptr_->header.pin_ptr_[channel2], HIGH);
            sleep(channel_data_.delay_time_);
        }

        sleep(channel_data_.delay_time_ * 2);
        off();
    }
}
```

### 9. Corner Expansion Pattern (taskSequence24)
```cpp
void BaseChannel::taskSequence24CornerExpansion() {
    // Ekspansi dari 4 corner ke tengah
    int corners[] = {0, 3, 12, 15};  // 4 corners
    int center[] = {5, 6, 9, 10};   // Center positions

    // Activate corners
    for (int i = 0; i < 4; i++) {
        if (corners[i] < 16) {
            set(config_data_ptr_->header.pin_ptr_[corners[i]], HIGH);
        }
    }
    sleep(channel_data_.delay_time_ * 2);

    // Expand edges
    int edges[] = {1, 2, 4, 7, 8, 11, 13, 14};
    for (int i = 0; i < 8; i++) {
        if (edges[i] < 16) {
            set(config_data_ptr_->header.pin_ptr_[edges[i]], HIGH);
            sleep(channel_data_.delay_time_);
        }
    }

    sleep(channel_data_.delay_time_ * 2);

    // Fill center
    for (int i = 0; i < 4; i++) {
        if (center[i] < 16) {
            set(config_data_ptr_->header.pin_ptr_[center[i]], HIGH);
            sleep(channel_data_.delay_time_);
        }
    }

    sleep(channel_data_.delay_time_ * 3);
    off();
}
```

### 10. Random Cluster Pattern (taskSequence25)
```cpp
void BaseChannel::taskSequence25RandomCluster() {
    // Pola cluster acak dengan koordinasi grid
    for (int cycle = 0; cycle < 8; cycle++) {
        // Pilih baris acak
        int randomRow = random(4);

        // Aktifkan cluster di baris tersebut
        for (int col = 0; col < 4; col++) {
            int channel = randomRow * 4 + col;
            if (channel < 16) {
                set(config_data_ptr_->header.pin_ptr_[channel], HIGH);
            }
        }

        sleep(channel_data_.delay_time_ * 2);

        // Matikan beberapa channel acak
        for (int i = 0; i < 3; i++) {
            int randomChannel = random(16);
            set(config_data_ptr_->header.pin_ptr_[randomChannel], LOW);
            sleep(channel_data_.delay_time_ / 2);
        }
    }
    off();
}
```

## Integration Steps

### 1. Update BaseChannel.h
```cpp
class BaseChannel {
private:
    // ... existing function declarations

    // New grid-specific patterns
    void taskSequence16RowWave();
    void taskSequence17ColumnFill();
    void taskSequence18DiagonalCross();
    void taskSequence19SpiralSquare();
    void taskSequence20Checkerboard();
    void taskSequence21Frame();
    void taskSequence22SnakeMatrix();
    void taskSequence23Cross();
    void taskSequence24CornerExpansion();
    void taskSequence25RandomCluster();
};
```

### 2. Update BaseChannel.cpp Mode Selection
```cpp
void BaseChannel::runAutoLight() {
    switch (channel_data_.mode) {
        // ... existing cases 2-15
        case 16: taskSequence16RowWave(); break;
        case 17: taskSequence17ColumnFill(); break;
        case 18: taskSequence18DiagonalCross(); break;
        case 19: taskSequence19SpiralSquare(); break;
        case 20: taskSequence20Checkerboard(); break;
        case 21: taskSequence21Frame(); break;
        case 22: taskSequence22SnakeMatrix(); break;
        case 23: taskSequence23Cross(); break;
        case 24: taskSequence24CornerExpansion(); break;
        case 25: taskSequence25RandomCluster(); break;
    }
}
```

### 3. Add Implementation Functions
- Copy semua fungsi taskSequence16-25 ke BaseChannel.cpp
- Pastikan menggunakan konfigurasi yang sama dengan pola existing
- Test masing-masing pattern secara individual

## Benefits

1. **Grid Optimization**: Pola dioptimalkan untuk layout 4×4 spesifik
2. **Visual Coherence**: Menghasilkan efek visual yang terstruktur dan dapat diprediksi
3. **Spatial Awareness**: Memanfaatkan posisi relatif channel dalam grid
4. **Enhanced Effects**: Menciptakan efek 2D yang menarik pada layout grid
5. **Easy Maintenance**: Kode terstruktur dengan jelas dan mudah dimodifikasi

## Testing Strategy

1. **Channel Mapping Verification**: Pastikan channel sesuai layout dari screenshot
2. **Individual Pattern Testing**: Test setiap pattern secara terpisah
3. **Performance Testing**: Verifikasi timing dan memory usage
4. **Visual Validation**: Pastikan efek visual sesuai ekspektasi
5. **Integration Testing**: Test dalam alur mode selection yang lengkap

## Implementation Timeline

- **Phase 1**: Implementasi helper functions dan basic patterns (taskSequence16-18)
- **Phase 2**: Advanced patterns dengan logic kompleks (taskSequence19-22)
- **Phase 3**: Mathematical dan random patterns (taskSequence23-25)
- **Phase 4**: Integration testing dan optimization
- **Phase 5**: Documentation dan deployment

---

**Note**: Implementasi ini didesain khusus untuk grid layout 4×4 sesuai screenshot. Jika layout berubah, sesuaikan channel mapping dan pattern logic sesuai kebutuhan.