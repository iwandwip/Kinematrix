#pragma once
#ifndef LED_MATRIX_VISUALIZER_H
#define LED_MATRIX_VISUALIZER_H

#if defined(ESP32)

#include "Arduino.h"
#include "../Common/Constants.h"

namespace AutoLight {
    using namespace Constants;

    class BaseChannel;

#ifndef DEFAULT_MATRIX_ROWS
#define DEFAULT_MATRIX_ROWS 8
#endif

#ifndef DEFAULT_MATRIX_COLS
#define DEFAULT_MATRIX_COLS 8
#endif

#ifndef MAX_LEDS
#define MAX_LEDS 128
#endif

    class LEDMatrixVisualizer {
    public:
        LEDMatrixVisualizer();
        LEDMatrixVisualizer(uint8_t rows, uint8_t cols);
        virtual ~LEDMatrixVisualizer();

        void setLayout(uint8_t *layout_data, uint8_t layout_rows, uint8_t layout_cols);
        void setLayoutFromArray(uint8_t layout_array[][32], uint8_t array_rows, uint8_t array_cols);
        void fillSequential(uint8_t start_led = 0);

        void setLayout2x8(uint8_t layout[2][8]);
        void setLayout1x16(uint8_t layout[1][16]);
        void setLayout4x4(uint8_t layout[4][4]);
        void setLayout3x5(uint8_t layout[3][5]);
        void setLayout8x8(uint8_t layout[8][8]);

        void placeLED(uint8_t led_index, uint8_t row, uint8_t col);
        void setBorder(bool enable);
        void clearMatrix();

        void updateFromAutoLight(BaseChannel *led_controller);
        void printMatrix();
        void forceUpdate();

        void showLayoutMap();
        bool hasChanges();
        uint8_t getTotalLEDs();

        uint8_t getRows() { return rows_; }

        uint8_t getCols() { return cols_; }

    private:
        uint8_t **matrix_;
        bool led_states_[MAX_LEDS];
        bool show_border_;
        bool matrix_changed_;
        uint8_t total_leds_;
        uint8_t rows_, cols_;

        void allocateMatrix();
        void deallocateMatrix();
        void markChanged();
        bool isValidPosition(uint8_t row, uint8_t col);
        void updateTotalLEDs();
    };

} // namespace AutoLight

#define SET_LAYOUT_2D(visualizer, array) \
    visualizer.setLayoutFromArray(array, sizeof(array)/sizeof(array[0]), sizeof(array[0])/sizeof(array[0][0]))

#endif // ESP32

#endif