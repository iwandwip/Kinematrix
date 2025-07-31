/*
 *  Constants.h
 *
 *  AutoLight V3 - Centralized Constants
 *  Kastara Electronics Embedded Development
 *  Professional constants organization - single source of truth
 */

#pragma once
#ifndef AUTOLIGHT_CONSTANTS_H
#define AUTOLIGHT_CONSTANTS_H

namespace AutoLight {
    namespace Constants {
        
        // ==========================================
        // CHANNEL CONSTANTS
        // ==========================================
        const int MAXNUM_TASK_SEQUENCE = 14;
        const int MAXNUM_TASK_SEQUENCE_ADDITION = 2;
        const int MAXNUM_TOTAL_TASK_SEQUENCE = 16;
        const int MAXNUM_TASK_TIME = 250;
        const int DEFAULT_DELAY_TIME = 50;
        const int DEFAULT_INIT_SEQUENCE = 0;
        
        // ==========================================
        // CONFIG CONSTANTS
        // ==========================================
        const int MAXNUM_CHANNEL = 64;
        const int MAXNUM_IO_EXPANDER = 8;
        const int MAXNUM_VERSION = 32;
        const int MAXNUM_CHANNEL_PIN = 5;
        const int MAXNUM_I2C_SLAVE_ADDRESS = 127;
        
        // ==========================================
        // HARDWARE CONSTANTS
        // ==========================================
        // Indicator pins
        const int MAXNUM_INDICATOR = 4;
        const int LED_RED_PIN = 14;
        const int LED_GREEN_PIN = 12;
        const int LED_BLUE_PIN = 13;
        const int BUZZER_PIN = 15;
        
        // Button configuration
        const int MAX_BUTTONS = 4;
        
        // ==========================================
        // SYSTEM CONSTANTS
        // ==========================================
        const int MAXNUM_TASK_NAME_CHAR = 20;
        
        // ==========================================
        // CONFIG DATA CLASS CONSTANTS
        // ==========================================
        namespace ConfigI2CMax {
            const int VERSION = 32;
            const int PIN_ADDRESS = 5;
            const int I2C_ADDRESS = 8;
        }
        
        // ==========================================
        // FILE SYSTEM PATHS  
        // ==========================================
        namespace FilePaths {
            const char* const CUSTOM_PRESET_1 = "/custom/preset_1.txt";
            const char* const CUSTOM_PRESET_2 = "/custom/preset_2.txt";
            const char* const CUSTOM_PRESET_3 = "/custom/preset_3.txt";
        }
    }
}

#endif // AUTOLIGHT_CONSTANTS_H