/*
 *  timer-duration.cpp
 *
 *  arduino timer duration c
 *  Created on: 2023. 4. 3
 */

#include "SDArduino.h"

#if defined(ESP32)
#else

#if KINEMATRIX_ENABLE_SD_GUARD

#include "Arduino.h"

SDArduino::SDArduino() {

}

SDArduino::~SDArduino() {

}

bool SDArduino::begin(uint8_t _cs_pin, void (*beginCallback)()) {
    bool isBegin = SD.begin(_cs_pin);
    if (isBegin) {
        Serial.print("| [INFO] SD Card init success");
        Serial.println();
        if (beginCallback != nullptr) beginCallback();
    } else {
        Serial.print("| [ERROR] SD Card init failed");
    }
    return isBegin;
}

bool SDArduino::exists(const char *_file_name) {
    return SD.exists(_file_name);
}

void SDArduino::createFiles(const char *_file_name, bool debug) {
    if (exists(_file_name)) {
        if (debug) {
            Serial.print("| [ERROR] file already exists: ");
            Serial.print(_file_name);
            Serial.println();
        }
        return;
    }
    file = SD.open(_file_name, FILE_WRITE);
    if (file) {
        if (debug) {
            Serial.print("| [INFO] file created: ");
            Serial.print(_file_name);
            Serial.println();
        }
        file.close();
    } else {
        if (debug) {
            Serial.print("| [ERROR] failed to create file: ");
            Serial.print(_file_name);
            Serial.println();
        }
    }
}

bool SDArduino::deleteFiles(const char *_file_name, bool debug) {
    if (!exists(_file_name)) {
        if (debug) {
            Serial.print("| [ERROR] file already deleted: ");
            Serial.print(_file_name);
            Serial.println();
        }
        return 0;
    }
    bool isRemove = SD.remove(_file_name);
    if (isRemove) {
        if (debug) {
            Serial.print("| [INFO] file deleted: ");
            Serial.print(_file_name);
            Serial.println();
        }
    } else {
        if (debug) {
            Serial.print("| [ERROR] file failed to delete: ");
            Serial.print(_file_name);
            Serial.println();
        }
    }
    return isRemove;
}

void SDArduino::printDir(File dir, int numTabs) {
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++) {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.println("/");
            printDir(entry, numTabs + 1);
        } else {
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

void SDArduino::listDir(int numTabs, bool debug) {
    file = SD.open("/");
    if (debug) {
        Serial.print("| [INFO] list directories start");
        Serial.println();
    }
    printDir(file, numTabs);
    file.close();
    if (debug) {
        Serial.print("| [INFO] list directories end");
        Serial.println();
    }
}

void SDArduino::writeFiles(const char *_file_name, const char *_text, bool debug) {
    file = SD.open(_file_name, O_WRITE);
    if (file) {
        if (debug) {
            Serial.print("| [INFO] write files");
            Serial.println();
        }
        file.print(_text);
        file.close();
        if (debug) {
            Serial.print("| [INFO] write files done");
            Serial.println();
        }
    } else {
        if (debug) {
            Serial.println("| [ERROR] error opening files");
        }
    }
}

void SDArduino::appendFiles(const char *_file_name, const char *_text, bool debug) {
    file = SD.open(_file_name, O_READ | O_WRITE | O_APPEND);
    if (file) {
        if (debug) {
            Serial.print("| [INFO] append files");
            Serial.println();
        }
        file.print(_text);
        file.close();
        if (debug) {
            Serial.print("| [INFO] append files done");
            Serial.println();
        }
    } else {
        if (debug) {
            Serial.println("| [ERROR] error opening files");
        }
    }
}

void SDArduino::emptyFiles(const char *_file_name, bool debug) {
    file = SD.open(_file_name, FILE_WRITE);
    if (file) {
        file.close();
        if (debug) {
            Serial.println("| [INFO] File emptied successfully");
        }
    } else {
        if (debug) {
            Serial.println("| [ERROR] Failed to empty file");
        }
    }
}

void SDArduino::readFiles(const char *_file_name, bool debug, void (*readCallback)(int next)) {
    file = SD.open(_file_name);
    if (file) {
        if (debug) {
            Serial.println("| [INFO] read files: ");
        }
        while (file.available()) {
            if (readCallback == nullptr) {
                Serial.write(file.read());
            } else {
                readCallback(file.read());
            }
        }
        if (debug) {
            Serial.println();
            Serial.println("| [INFO] read files end");
        }
        file.close();
    } else {
        if (debug) {
            Serial.println("| [ERROR] error opening files");
        }
    }
}

String SDArduino::readFilesString(const char *_file_name, bool debug) {
    String result = "";
    file = SD.open(_file_name);
    if (file) {
        if (debug) {
            Serial.println("| [INFO] read files: ");
        }
        while (file.available()) {
            result += (char) file.read();
        }
        if (debug) {
            Serial.println("| [INFO] read files end");
        }
        file.close();
    } else {
        if (debug) {
            Serial.println("| [ERROR] error opening files");
        }
    }
    return result;
}

#endif

#endif