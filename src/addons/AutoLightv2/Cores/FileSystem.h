/*
 *  FileSystem.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */
#ifdef  USE_AUTOLIGHT_v2
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#if defined(ESP32)

#include "Arduino.h"
#include "FS.h"
#include "LittleFS.h"

namespace AutoLight {

#define ROOT_DIR                ("/")
#define CONFIG_DIR              ("/config")
#define CUSTOM_DIR              ("/custom")
#define CONFIGURATION_FILE      ("/config/configuration.txt")
#define CONFIG_SYSTEM_FILE      ("/config/system.txt")
#define CUSTOM_PRESET_1_FILE    ("/custom/preset_1.txt")
#define CUSTOM_PRESET_2_FILE    ("/custom/preset_2.txt")
#define CUSTOM_PRESET_3_FILE    ("/custom/preset_3.txt")

    class FileSystem {
    public:
        FileSystem(bool _is_debug = false);
        ~FileSystem();

        void init();
        void end();

        void listDir(const char *dirname, uint8_t levels);
        void createDir(const char *path);
        void removeDir(const char *path);

        void readFile(const char *path);
        void readFileStr(const char *path, char *&content);
        char *readFileStr(const char *path);
        void freeFileStr(char *content);

        void parseAndStoreValuesUint(const char *str, uint8_t *value, uint8_t size);
        uint8_t **parseAndStoreValuesUint2D(const char *str, uint8_t row, uint8_t col);
        void freeValuesUint2D(uint8_t **value, uint8_t row);

        char *createBlankStr();
        size_t readFileSize(const char *path);
        size_t countTotalRow(const char *str);

        void writeFile(const char *path, const char *message);
        void appendFile(const char *path, const char *message);
        void renameFile(const char *path1, const char *path2);
        void deleteFile(const char *path);

        void testFileIO(const char *path);
    private:
        bool is_debug_;
    };
}

#endif

#endif // FILE_SYSTEM_H
#endif