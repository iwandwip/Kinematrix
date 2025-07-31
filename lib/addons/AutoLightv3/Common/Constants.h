#pragma once
#ifndef AUTOLIGHT_CONSTANTS_H
#define AUTOLIGHT_CONSTANTS_H

namespace AutoLight {
    namespace Constants {

        const int MAXNUM_TASK_SEQUENCE = 14;
        const int MAXNUM_TASK_SEQUENCE_ADDITION = 2;
        const int MAXNUM_TOTAL_TASK_SEQUENCE = 16;
        const int MAXNUM_TASK_TIME = 250;
        const int DEFAULT_DELAY_TIME = 50;
        const int DEFAULT_INIT_SEQUENCE = 0;

        const int MAXNUM_CHANNEL = 64;
        const int MAXNUM_IO_EXPANDER = 8;
        const int MAXNUM_VERSION = 32;
        const int MAXNUM_CHANNEL_PIN = 5;
        const int MAXNUM_I2C_SLAVE_ADDRESS = 127;

        const int MAXNUM_INDICATOR = 4;
        const int LED_RED_PIN = 14;
        const int LED_GREEN_PIN = 12;
        const int LED_BLUE_PIN = 13;
        const int BUZZER_PIN = 15;

        const int MAX_BUTTONS = 4;

        const int MAXNUM_TASK_NAME_CHAR = 20;

        namespace ConfigI2CMax {
            const int VERSION = 32;
            const int PIN_ADDRESS = 5;
            const int I2C_ADDRESS = 8;
        }

        namespace FilePaths {
            const char *const CUSTOM_PRESET_1 = "/custom/preset_1.txt";
            const char *const CUSTOM_PRESET_2 = "/custom/preset_2.txt";
            const char *const CUSTOM_PRESET_3 = "/custom/preset_3.txt";
        }
    }
}

#endif
