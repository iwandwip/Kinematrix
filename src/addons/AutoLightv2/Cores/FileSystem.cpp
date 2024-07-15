/*
 *  FileSystem.cpp
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */
#ifdef  USE_AUTOLIGHT_v2
#include "FileSystem.h"

#if defined(ESP32)

namespace AutoLight {
    FileSystem::FileSystem(bool _is_debug)
            : is_debug_(_is_debug) {
    }

    FileSystem::~FileSystem() {
        delete this;
    }

    void FileSystem::init() {
        if (!LittleFS.begin(true)) {
            if (is_debug_) Serial.println("| [LittleFS] Mount Failed");
            return;
        }
        if (is_debug_) Serial.println("| [LittleFS] Mount Success");
    }

    void FileSystem::end() {
        LittleFS.end();
        if (is_debug_) Serial.println("| [LittleFS] UnMount Success");
    }

    void FileSystem::listDir(const char *dirname, uint8_t levels) {
        if (is_debug_) Serial.printf("| Listing directory: %s\r\n", dirname);

        File root = LittleFS.open(dirname);
        if (!root) {
            if (is_debug_) Serial.println("| [ERROR] - failed to open directory");
            return;
        }
        if (!root.isDirectory()) {
            if (is_debug_) Serial.println("| [WARNING] - not a directory");
            return;
        }

        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                if (is_debug_) Serial.print("|  DIR : ");
                if (is_debug_) Serial.println(file.name());
                if (levels) {
                    listDir(file.path(), levels - 1);
                }
            } else {
                if (is_debug_) Serial.print("|  FILE: ");
                if (is_debug_) Serial.print(file.name());
                if (is_debug_) Serial.print("\tSIZE: ");
                if (is_debug_) Serial.println(file.size());
            }
            file = root.openNextFile();
        }
    }

    void FileSystem::createDir(const char *path) {
        if (is_debug_) Serial.printf("| Creating Dir: %s\n", path);
        if (LittleFS.mkdir(path)) {
            if (is_debug_) Serial.println("| [SUCCESS] Dir created");
        } else {
            if (is_debug_) Serial.println("| [ERROR] mkdir failed");
        }
    }

    void FileSystem::removeDir(const char *path) {
        if (is_debug_) Serial.printf("| Removing Dir: %s\n", path);
        if (LittleFS.rmdir(path)) {
            if (is_debug_) Serial.println("| [SUCCESS] Dir removed");
        } else {
            if (is_debug_) Serial.println("| [ERROR] rmdir failed");
        }
    }

    void FileSystem::readFile(const char *path) {
        if (is_debug_) Serial.printf("| Reading file: %s\r\n", path);

        File file = LittleFS.open(path);
        if (!file || file.isDirectory()) {
            if (is_debug_) Serial.println("| [ERROR] - failed to open file for reading");
            return;
        }

        if (is_debug_) {
            Serial.print("| [SUCCESS] - read from file with size: ");
            Serial.println(file.size());
        }
        while (file.available()) {
            if (is_debug_) Serial.write(file.read());
        }
        file.close();
    }

    void FileSystem::readFileStr(const char *path, char *&content) {
        if (is_debug_) Serial.printf("| Reading file: %s\r\n", path);
        File file = LittleFS.open(path);
        if (!file || file.isDirectory()) {
            if (is_debug_) Serial.println("| [ERROR] - failed to open file for reading");
            return;
        }
        if (is_debug_) {
            Serial.print("| [SUCCESS] - read from file with size: ");
            Serial.println(file.size());
        }
        // Allocate memory for content based on file size
        content = new char[file.size() + 1]; // +1 for null-terminator
        // Read file content into the allocated memory
        size_t bytesRead = file.readBytes(content, file.size());
        content[bytesRead] = '\0'; // Null-terminate the string
        file.close();
    }

    char *FileSystem::readFileStr(const char *path) {
        if (is_debug_) Serial.printf("| Reading file: %s\r\n", path);
        File file = LittleFS.open(path);
        if (!file || file.isDirectory()) {
            if (is_debug_) Serial.println("| [ERROR] - failed to open file for reading");
            return nullptr;
        }
        if (is_debug_) {
            Serial.print("| [SUCCESS] - read from file with size: ");
            Serial.println(file.size());
        }
        char *content = new char[file.size() + 1]; // +1 for null-terminator // Allocate memory for content based on file size
        size_t bytesRead = file.readBytes(content, file.size()); // Read file content into the allocated memory
        content[bytesRead] = '\0'; // Null-terminate the string
        file.close();
        return content;
    }

    void FileSystem::freeFileStr(char *content) {
        delete[] content;
        content = nullptr;
    }

    void FileSystem::parseAndStoreValuesUint(const char *str, uint8_t *value, uint8_t size) {
        char *token;
        char *strCopy = strdup(str);
        token = strtok(strCopy, ";");
        for (int i = 0; i < size && token != nullptr; ++i) {
            value[i] = static_cast<uint8_t>(atoi(token));
            token = strtok(nullptr, ";");
        }
        free(strCopy);
    }

    uint8_t **FileSystem::parseAndStoreValuesUint2D(const char *str, uint8_t row, uint8_t col) {
        uint8_t **values = new uint8_t *[row];
        for (int i = 0; i < row; i++) {
            values[i] = new uint8_t[col];
        }
        char *token;
        char *strCopy = strdup(str);
        token = strtok(strCopy, ";\n");
        for (int i = 0; i < row && token != nullptr; i++) {
            for (int j = 0; j < col && token != nullptr; j++) {
                values[i][j] = atoi(token);
                token = strtok(nullptr, ";\n");
            }
        }
        free(strCopy);
        return values;
    }

    void FileSystem::freeValuesUint2D(uint8_t **value, uint8_t row) {
        for (int i = 0; i < row; i++) {
            delete[] value[i];
        }
        delete[] value;
        value = nullptr;
    }

    char *FileSystem::createBlankStr() {
        return nullptr;
    }

    size_t FileSystem::readFileSize(const char *path) {
        File file = LittleFS.open(path);
        if (!file || file.isDirectory()) return 0;
        return file.size();
    }

    size_t FileSystem::countTotalRow(const char *str) {
        int rows = 0;
        const char *newline = "\n";
        const char *ptr = str;
        while ((ptr = strstr(ptr, newline)) != nullptr) {
            rows++;
            ptr++;
        }
        if (strlen(str) > 0 && str[strlen(str) - 1] != '\n') {
            rows++;
        }
        return rows;
    }

    void FileSystem::writeFile(const char *path, const char *message) {
        if (is_debug_) Serial.printf("| Writing file: %s\r\n", path);

        File file = LittleFS.open(path, FILE_WRITE);
        if (!file) {
            if (is_debug_) Serial.println("| [ERROR] - failed to open file for writing");
            return;
        }
        if (file.print(message)) {
            if (is_debug_) Serial.println("| [SUCCESS] - file written");
        } else {
            if (is_debug_) Serial.println("| [ERROR] - write failed");
        }
        file.close();
    }

    void FileSystem::appendFile(const char *path, const char *message) {
        if (is_debug_) Serial.printf("| Appending to file: %s\r\n", path);

        File file = LittleFS.open(path, FILE_APPEND);
        if (!file) {
            if (is_debug_) Serial.println("| [ERROR] - failed to open file for appending");
            return;
        }
        if (file.print(message)) {
            if (is_debug_) Serial.println("| [SUCCESS] - message appended");
        } else {
            if (is_debug_) Serial.println("| [ERROR] - append failed");
        }
        file.close();
    }

    void FileSystem::renameFile(const char *path1, const char *path2) {
        if (is_debug_) Serial.printf("| Renaming file %s to %s\r\n", path1, path2);
        if (LittleFS.rename(path1, path2)) {
            if (is_debug_) Serial.println("| [SUCCESS] - file renamed");
        } else {
            if (is_debug_) Serial.println("| [ERROR] - rename failed");
        }
    }

    void FileSystem::deleteFile(const char *path) {
        if (is_debug_) Serial.printf("| Deleting file: %s\r\n", path);
        if (LittleFS.remove(path)) {
            if (is_debug_) Serial.println("| [SUCCESS] - file deleted");
        } else {
            if (is_debug_) Serial.println("| [ERROR] - delete failed");
        }
    }

    void FileSystem::testFileIO(const char *path) {
        if (is_debug_) Serial.printf("| Testing file I/O with %s\r\n", path);

        static uint8_t buf[512];
        size_t len = 0;
        File file = LittleFS.open(path, FILE_WRITE);
        if (!file) {
            if (is_debug_) Serial.println("| - failed to open file for writing");
            return;
        }

        size_t i;
        if (is_debug_) Serial.print("| - writing");
        uint32_t start = millis();
        for (i = 0; i < 2048; i++) {
            if ((i & 0x001F) == 0x001F) {
                if (is_debug_) Serial.print(".");
            }
            file.write(buf, 512);
        }
        if (is_debug_) Serial.println("");
        uint32_t end = millis() - start;
        if (is_debug_) Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
        file.close();

        file = LittleFS.open(path);
        start = millis();
        end = start;
        i = 0;
        if (file && !file.isDirectory()) {
            len = file.size();
            size_t flen = len;
            start = millis();
            if (is_debug_) Serial.print("- reading");
            while (len) {
                size_t toRead = len;
                if (toRead > 512) {
                    toRead = 512;
                }
                file.read(buf, toRead);
                if ((i++ & 0x001F) == 0x001F) {
                    if (is_debug_) Serial.print(".");
                }
                len -= toRead;
            }
            if (is_debug_) Serial.println("");
            end = millis() - start;
            if (is_debug_) Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
            file.close();
        } else {
            if (is_debug_) Serial.println("- failed to open file for reading");
        }
    }

}

#endif
#endif