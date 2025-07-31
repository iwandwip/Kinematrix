#include "LEDMatrixVisualizer.h"

#if defined(ESP32)

#include "../Channel/BaseChannel.h"

namespace AutoLight {

    LEDMatrixVisualizer::LEDMatrixVisualizer() {
        rows_ = DEFAULT_MATRIX_ROWS;
        cols_ = DEFAULT_MATRIX_COLS;
        matrix_ = nullptr;
        show_border_ = true;
        matrix_changed_ = true;
        total_leds_ = 0;

        for (uint8_t i = 0; i < MAX_LEDS; i++) {
            led_states_[i] = false;
        }

        allocateMatrix();
        clearMatrix();
        Serial.println("[MATRIX] Initialized with default 8x8 size");
    }

    LEDMatrixVisualizer::LEDMatrixVisualizer(uint8_t rows, uint8_t cols) {
        if (rows == 0 || cols == 0) {
            Serial.println("[MATRIX] ERROR: Invalid matrix size, using default 8x8");
            rows = DEFAULT_MATRIX_ROWS;
            cols = DEFAULT_MATRIX_COLS;
        }

        rows_ = rows;
        cols_ = cols;
        matrix_ = nullptr;
        show_border_ = true;
        matrix_changed_ = true;
        total_leds_ = 0;

        for (uint8_t i = 0; i < MAX_LEDS; i++) {
            led_states_[i] = false;
        }

        allocateMatrix();
        clearMatrix();
        Serial.printf("[MATRIX] Initialized with custom %dx%d size\n", rows_, cols_);
    }

    LEDMatrixVisualizer::~LEDMatrixVisualizer() {
        deallocateMatrix();
    }

    void LEDMatrixVisualizer::markChanged() {
        matrix_changed_ = true;
    }

    void LEDMatrixVisualizer::allocateMatrix() {
        if (matrix_) deallocateMatrix();

        matrix_ = new uint8_t *[rows_];
        if (!matrix_) {
            Serial.println("[MATRIX] ERROR: Failed to allocate row pointers");
            return;
        }

        for (uint8_t i = 0; i < rows_; i++) {
            matrix_[i] = new uint8_t[cols_];
            if (!matrix_[i]) {
                Serial.printf("[MATRIX] ERROR: Failed to allocate row %d\n", i);
                for (uint8_t j = 0; j < i; j++) {
                    delete[] matrix_[j];
                }
                delete[] matrix_;
                matrix_ = nullptr;
                return;
            }
        }
        Serial.printf("[MATRIX] Memory allocated for %dx%d matrix\n", rows_, cols_);
    }

    void LEDMatrixVisualizer::deallocateMatrix() {
        if (matrix_) {
            for (uint8_t i = 0; i < rows_; i++) {
                delete[] matrix_[i];
            }
            delete[] matrix_;
            matrix_ = nullptr;
            Serial.println("[MATRIX] Memory deallocated");
        }
    }

    bool LEDMatrixVisualizer::isValidPosition(uint8_t row, uint8_t col) {
        return (row < rows_ && col < cols_);
    }

    void LEDMatrixVisualizer::updateTotalLEDs() {
        uint8_t max_led = 0;
        bool found_led = false;

        for (uint8_t r = 0; r < rows_; r++) {
            for (uint8_t c = 0; c < cols_; c++) {
                if (matrix_[r][c] != 255) {
                    if (!found_led || matrix_[r][c] > max_led) {
                        max_led = matrix_[r][c];
                        found_led = true;
                    }
                }
            }
        }

        total_leds_ = found_led ? max_led + 1 : 0;
    }

    void LEDMatrixVisualizer::setLayout(uint8_t *layout_data, uint8_t layout_rows, uint8_t layout_cols) {
        Serial.println("[MATRIX] Setting bulk layout (1D array)...");

        clearMatrix();

        uint8_t max_rows = (layout_rows < rows_) ? layout_rows : rows_;
        uint8_t max_cols = (layout_cols < cols_) ? layout_cols : cols_;

        for (uint8_t r = 0; r < max_rows; r++) {
            for (uint8_t c = 0; c < max_cols; c++) {
                uint8_t led_index = layout_data[r * layout_cols + c];

                if (led_index != 255 && led_index < MAX_LEDS) {
                    matrix_[r][c] = led_index;
                } else {
                    matrix_[r][c] = 255;
                }
            }
        }

        updateTotalLEDs();
        markChanged();
        Serial.printf("[MATRIX] Bulk layout set successfully. Total LEDs: %d\n", total_leds_);
    }

    void LEDMatrixVisualizer::setLayoutFromArray(uint8_t layout_array[][32], uint8_t array_rows, uint8_t array_cols) {
        Serial.println("[MATRIX] Setting bulk layout (2D array)...");

        if (!matrix_) {
            Serial.println("[MATRIX] ERROR: Matrix not allocated");
            return;
        }

        if (array_cols > 32) {
            Serial.printf("[MATRIX] ERROR: Array cols %d exceeds max supported 32\n", array_cols);
            return;
        }

        clearMatrix();

        uint8_t max_rows = (array_rows < rows_) ? array_rows : rows_;
        uint8_t max_cols = (array_cols < cols_) ? array_cols : cols_;

        if (max_cols > 32) max_cols = 32;

        for (uint8_t r = 0; r < max_rows; r++) {
            for (uint8_t c = 0; c < max_cols; c++) {
                uint8_t led_index = layout_array[r][c];

                if (led_index != 255 && led_index < MAX_LEDS) {
                    matrix_[r][c] = led_index;
                } else {
                    matrix_[r][c] = 255;
                }
            }
        }

        updateTotalLEDs();
        markChanged();
        Serial.printf("[MATRIX] 2D array layout set successfully. Total LEDs: %d\n", total_leds_);
    }

    void LEDMatrixVisualizer::setLayout2x8(uint8_t layout[2][8]) {
        Serial.println("[MATRIX] Setting 2x8 layout...");
        clearMatrix();

        if (rows_ >= 2 && cols_ >= 8) {
            for (uint8_t r = 0; r < 2; r++) {
                for (uint8_t c = 0; c < 8; c++) {
                    matrix_[r][c] = layout[r][c];
                }
            }
            updateTotalLEDs();
            markChanged();
            Serial.printf("[MATRIX] 2x8 layout set. Total LEDs: %d\n", total_leds_);
        } else {
            Serial.printf("[MATRIX] ERROR: Matrix size %dx%d too small for 2x8 layout\n", rows_, cols_);
        }
    }

    void LEDMatrixVisualizer::setLayout1x16(uint8_t layout[1][16]) {
        Serial.println("[MATRIX] Setting 1x16 layout...");
        clearMatrix();

        if (rows_ >= 1 && cols_ >= 16) {
            for (uint8_t c = 0; c < 16; c++) {
                matrix_[0][c] = layout[0][c];
            }
            updateTotalLEDs();
            markChanged();
            Serial.printf("[MATRIX] 1x16 layout set. Total LEDs: %d\n", total_leds_);
        } else {
            Serial.printf("[MATRIX] ERROR: Matrix size %dx%d too small for 1x16 layout\n", rows_, cols_);
        }
    }

    void LEDMatrixVisualizer::setLayout4x4(uint8_t layout[4][4]) {
        Serial.println("[MATRIX] Setting 4x4 layout...");
        clearMatrix();

        if (rows_ >= 4 && cols_ >= 4) {
            for (uint8_t r = 0; r < 4; r++) {
                for (uint8_t c = 0; c < 4; c++) {
                    matrix_[r][c] = layout[r][c];
                }
            }
            updateTotalLEDs();
            markChanged();
            Serial.printf("[MATRIX] 4x4 layout set. Total LEDs: %d\n", total_leds_);
        } else {
            Serial.printf("[MATRIX] ERROR: Matrix size %dx%d too small for 4x4 layout\n", rows_, cols_);
        }
    }

    void LEDMatrixVisualizer::setLayout3x5(uint8_t layout[3][5]) {
        Serial.println("[MATRIX] Setting 3x5 layout...");
        clearMatrix();

        if (rows_ >= 3 && cols_ >= 5) {
            for (uint8_t r = 0; r < 3; r++) {
                for (uint8_t c = 0; c < 5; c++) {
                    matrix_[r][c] = layout[r][c];
                }
            }
            updateTotalLEDs();
            markChanged();
            Serial.printf("[MATRIX] 3x5 layout set. Total LEDs: %d\n", total_leds_);
        } else {
            Serial.printf("[MATRIX] ERROR: Matrix size %dx%d too small for 3x5 layout\n", rows_, cols_);
        }
    }

    void LEDMatrixVisualizer::setLayout8x8(uint8_t layout[8][8]) {
        Serial.println("[MATRIX] Setting 8x8 layout...");
        clearMatrix();

        if (rows_ >= 8 && cols_ >= 8) {
            for (uint8_t r = 0; r < 8; r++) {
                for (uint8_t c = 0; c < 8; c++) {
                    matrix_[r][c] = layout[r][c];
                }
            }
            updateTotalLEDs();
            markChanged();
            Serial.printf("[MATRIX] 8x8 layout set. Total LEDs: %d\n", total_leds_);
        } else {
            Serial.printf("[MATRIX] ERROR: Matrix size %dx%d too small for 8x8 layout\n", rows_, cols_);
        }
    }

    void LEDMatrixVisualizer::fillSequential(uint8_t start_led) {
        Serial.printf("[MATRIX] Filling sequential starting from LED %d...\n", start_led);

        clearMatrix();

        uint8_t led_index = start_led;
        for (uint8_t r = 0; r < rows_; r++) {
            for (uint8_t c = 0; c < cols_; c++) {
                if (led_index >= MAX_LEDS) {
                    Serial.printf("[MATRIX] WARNING: Reached MAX_LEDS limit (%d). Remaining positions set to empty.\n", MAX_LEDS);
                    matrix_[r][c] = 255;
                } else {
                    matrix_[r][c] = led_index;
                    led_index++;
                }
            }
        }

        updateTotalLEDs();
        markChanged();
        Serial.printf("[MATRIX] Sequential fill completed. Total LEDs: %d\n", total_leds_);
    }

    void LEDMatrixVisualizer::placeLED(uint8_t led_index, uint8_t row, uint8_t col) {
        if (!isValidPosition(row, col)) {
            Serial.printf("[MATRIX] WARNING: LED %d position (%d,%d) out of bounds (%dx%d). Ignored.\n",
                          led_index, row, col, rows_, cols_);
            return;
        }

        if (led_index >= MAX_LEDS) {
            Serial.printf("[MATRIX] WARNING: LED index %d too high (max %d). Ignored.\n",
                          led_index, MAX_LEDS - 1);
            return;
        }

        matrix_[row][col] = led_index;
        updateTotalLEDs();
        markChanged();

        Serial.printf("[MATRIX] LED %d placed at (%d,%d)\n", led_index, row, col);
    }

    void LEDMatrixVisualizer::setBorder(bool enable) {
        show_border_ = enable;
        markChanged();
    }

    void LEDMatrixVisualizer::clearMatrix() {
        if (!matrix_) return;

        for (uint8_t r = 0; r < rows_; r++) {
            for (uint8_t c = 0; c < cols_; c++) {
                matrix_[r][c] = 255;
            }
        }
        for (uint8_t i = 0; i < MAX_LEDS; i++) {
            led_states_[i] = false;
        }
        total_leds_ = 0;
        markChanged();
    }

    void LEDMatrixVisualizer::updateFromAutoLight(BaseChannel *led_controller) {
        if (!led_controller) return;

        bool changed = false;
        uint8_t controller_total = led_controller->getTotalLEDs();

        if (controller_total > total_leds_) {
            total_leds_ = controller_total;
        }

        for (uint8_t i = 0; i < total_leds_ && i < MAX_LEDS; i++) {
            bool new_state = led_controller->getLEDState(i);

            if (led_states_[i] != new_state) {
                led_states_[i] = new_state;
                changed = true;
            }
        }

        if (changed) markChanged();
    }

    void LEDMatrixVisualizer::printMatrix() {
        if (!matrix_changed_ || !matrix_) return;

        Serial.println();

        if (show_border_) {
            Serial.print("┌");
            for (uint8_t c = 0; c < cols_ * 2; c++) Serial.print("─");
            Serial.println("┐");
        }

        for (uint8_t r = 0; r < rows_; r++) {
            if (show_border_) Serial.print("│");

            for (uint8_t c = 0; c < cols_; c++) {
                uint8_t led_index = matrix_[r][c];

                if (led_index == 255) {
                    Serial.print("  ");
                } else {
                    char symbol = led_states_[led_index] ? '#' : '.';
                    Serial.print(symbol);
                    Serial.print(" ");
                }
            }

            if (show_border_) Serial.print("│");
            Serial.println();
        }

        if (show_border_) {
            Serial.print("└");
            for (uint8_t c = 0; c < cols_ * 2; c++) Serial.print("─");
            Serial.println("┘");
        }

        matrix_changed_ = false;
    }

    void LEDMatrixVisualizer::showLayoutMap() {
        if (!matrix_) return;

        Serial.println("\n[MATRIX] LED Layout Map:");
        Serial.printf("[MATRIX] Matrix Size: %dx%d | Total LEDs: %d\n",
                      rows_, cols_, total_leds_);

        if (show_border_) {
            Serial.print("┌");
            for (uint8_t c = 0; c < cols_ * 3; c++) Serial.print("─");
            Serial.println("┐");
        }

        for (uint8_t r = 0; r < rows_; r++) {
            if (show_border_) Serial.print("│");

            for (uint8_t c = 0; c < cols_; c++) {
                uint8_t led_index = matrix_[r][c];

                if (led_index == 255) {
                    Serial.print("   ");
                } else {
                    Serial.printf("%2d ", led_index);
                }
            }

            if (show_border_) Serial.print("│");
            Serial.println();
        }

        if (show_border_) {
            Serial.print("└");
            for (uint8_t c = 0; c < cols_ * 3; c++) Serial.print("─");
            Serial.println("┘");
        }
        Serial.println();
    }

    void LEDMatrixVisualizer::forceUpdate() {
        markChanged();
    }

    bool LEDMatrixVisualizer::hasChanges() {
        return matrix_changed_;
    }

    uint8_t LEDMatrixVisualizer::getTotalLEDs() {
        return total_leds_;
    }

} // namespace AutoLight

#endif