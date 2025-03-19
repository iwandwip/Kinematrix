/*
 *  oled-menu.cpp
 *
 *  OLED menu implementation
 *  Created on: 2025. 2. 26
 */

#include "oled-menu.h"

void OledMenu::initialize(bool _debug, void (*initCallback)()) {
    // Default values for 128x64 OLED with 3 menu items
    setDisplayParams(16, 10, 10, 3);

    // Initialize display
    if (!Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
        if (_debug) Serial.println("SSD1306 allocation failed");
        for (;;); // Don't proceed, loop forever
    }

    clearDisplay();
    setTextSize(1);
    setTextColor(WHITE);
    display();

    if (initCallback != nullptr) initCallback();

    if (!_debug) return;
    Serial.print("| [NAME]: ");
    Serial.print("OLED");
    Serial.print(" [INFO]: Init Success");
    Serial.println();
}

void OledMenu::setDisplayParams(int itemHeight, int startY, int cursorW, int rows) {
    menuItemHeight = itemHeight;
    menuStartY = startY;
    cursorWidth = cursorW;
    displayRows = rows;
}

void OledMenu::onListen(MenuCursor *menuCursor, void (*listenCallback)()) {
    cursor_ = menuCursor;
    listenCallback();
}

void OledMenu::showMenu(MenuProperties *properties, bool forced, uint32_t showTime) {
    if (strlen(properties->option) != 0 && !forced) return;

    if (millis() - oledPrintTimer >= showTime || forced) {
        clearDisplay();

        // Display visible menu items
        for (int i = 0; i < displayRows && i + properties->select < properties->len; i++) {
            setCursor(cursorWidth + 5, menuStartY + i * menuItemHeight);
            print(properties->text[properties->select + i]);
        }

        // Show the cursor indicators if needed
        if (!forced && cursor_->show) {
            // Draw selection cursor (triangle pointing right)
            int cursorY = menuStartY + (properties->index - properties->select) * menuItemHeight;
            fillTriangle(0, cursorY + menuItemHeight / 2,
                         cursorWidth, cursorY + menuItemHeight / 4,
                         cursorWidth, cursorY + 3 * menuItemHeight / 4,
                         WHITE);

            // Draw up/down indicators if needed
            if (properties->select > 0) {
                // Draw up arrow at the top
                drawCursor(menuStartY - menuItemHeight / 2, true, false);
            }

            if (properties->select + displayRows < properties->len) {
                // Draw down arrow at bottom
                drawCursor(menuStartY + displayRows * menuItemHeight, false, true);
            }
        }

        display();
        oledPrintTimer = millis();
    }

    if (forced) return;
    this->onCursor(properties);
}

void OledMenu::onCursor(MenuProperties *properties) {
    if (cursor_->down) {
        if (properties->index < properties->len - 1) {
            if (properties->upCount == displayRows - 1 && properties->select < properties->len - displayRows)
                properties->select++;
            if (properties->upCount < displayRows - 1) properties->upCount++;
            properties->index++;
        } else {
            properties->index = 0;
            properties->upCount = 0;
            properties->select = 0;
        }
    }
    if (cursor_->up) {
        if (properties->index > 0) {
            if (properties->upCount == 0 && properties->select > 0) properties->select--;
            if (properties->upCount > 0) properties->upCount--;
            properties->index--;
        } else {
            properties->index = properties->len - 1;
            properties->upCount = displayRows - 1;
            properties->select = properties->len - displayRows;
            if (properties->select < 0) properties->select = 0; // In case, there are fewer items than displayRows
        }
    }

    if (cursor_->select) {
        cursor_->select = 0;
        if (properties->isHasCb[properties->index]) {
            if (properties->callbackMenu != nullptr && properties->callbackMenu[properties->index] != nullptr) {
                properties->callbackMenu[properties->index]();
            }
            strcpy(properties->option, properties->text[properties->index]);
        }
    }
}

void OledMenu::drawCursor(int y, bool isUp, bool isDown) {
    if (isUp) {
        // Draw up arrow
        fillTriangle(width() / 2 - 5, y,
                     width() / 2 + 5, y,
                     width() / 2, y - 5,
                     WHITE);
    } else if (isDown) {
        // Draw down arrow
        fillTriangle(width() / 2 - 5, y,
                     width() / 2 + 5, y,
                     width() / 2, y + 5,
                     WHITE);
    }
}

void OledMenu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)()) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        optionCallback();
        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void OledMenu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
                        void (*optionCallback)()) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
            properties->callbackMenu[i] = onClickCallback;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        optionCallback();
        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void OledMenu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor)) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        optionCallback(cursor_);
        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void OledMenu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
                        void (*optionCallback)(MenuCursor *cursor)) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
            properties->callbackMenu[i] = onClickCallback;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        optionCallback(cursor_);
        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void OledMenu::formatMenu(MenuProperties *properties, uint8_t index, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(properties->text[index], MAX_BUFF_LEN, format, args);
    va_end(args);
    return;
}

void OledMenu::clearMenu(MenuProperties *firstMenu, ...) {
    cursor_->up = false;
    cursor_->down = false;
    cursor_->select = false;
    cursor_->back = false;

    va_list args;
    va_start(args, firstMenu);
    MenuProperties *currentMenu = firstMenu;
    while (currentMenu != nullptr) {
        freeCharArray(currentMenu->option);
        currentMenu->select = 0;
        currentMenu->index = 0;
        currentMenu->upCount = 0;
        currentMenu = va_arg(args, MenuProperties*);
    }
    va_end(args);
}

int OledMenu::begin(int nums) {
    return nums;
}

int OledMenu::get(int nums) {
    return nums;
}

MenuProperties *OledMenu::end() {
    return nullptr;
}

void OledMenu::freeCharArray(char *str) {
    for (int i = 0; str[i] != '\0'; ++i) {
        str[i] = '\0';
    }
}

MenuProperties *OledMenu::createMenu(int menuSize, ...) {
    va_list args;
    va_start(args, menuSize);

    MenuProperties *properties = new MenuProperties;

    properties->option[0] = '\0';
    properties->len = menuSize;

    properties->select = 0;
    properties->index = 0;
    properties->upCount = 0;

    if (menuSize > 0) {
        properties->text = new char *[menuSize];
        properties->isHasCb = new bool[menuSize];
        properties->callbackMenu = new CallbackMenu[menuSize];
        for (uint8_t i = 0; i < menuSize; ++i) {
            const char *menuItem = va_arg(args, const char *);
            properties->text[i] = new char[MAX_BUFF_LEN];
            strcpy(properties->text[i], "default");
            if (menuItem != nullptr) strcpy(properties->text[i], menuItem);
            properties->isHasCb[i] = false;
            properties->callbackMenu[i] = nullptr;
        }
    } else {
        properties->text = nullptr;
        properties->isHasCb = nullptr;
        properties->callbackMenu = nullptr;
    }

    va_end(args);
    return properties;
}

MenuProperties *OledMenu::createEmptyMenu(int menuSize, const char *text) {
    MenuProperties *properties = new MenuProperties;

    properties->option[0] = '\0';
    properties->len = menuSize;

    properties->select = 0;
    properties->index = 0;
    properties->upCount = 0;

    if (menuSize > 0) {
        properties->text = new char *[menuSize];
        properties->isHasCb = new bool[menuSize];
        properties->callbackMenu = new CallbackMenu[menuSize];
        for (uint8_t i = 0; i < menuSize; ++i) {
            properties->text[i] = new char[MAX_BUFF_LEN];
            strcpy(properties->text[i], "default");
            if (text != nullptr) strcpy(properties->text[i], text);
            properties->isHasCb[i] = false;
            properties->callbackMenu[i] = nullptr;
        }
    } else {
        properties->text = nullptr;
        properties->isHasCb = nullptr;
        properties->callbackMenu = nullptr;
    }
    return properties;
}

void OledMenu::freeMenu(MenuProperties *menuProperties) {
    if (menuProperties->text) {
        for (uint8_t i = 0; i < menuProperties->len; ++i) {
            delete[] menuProperties->text[i];
        }
        delete[] menuProperties->text;
    }
    delete[] menuProperties->isHasCb;
    delete[] menuProperties->callbackMenu;
    delete menuProperties;
}

void OledMenu::showCursor(bool state) {
    cursor_->show = state;
}

void OledMenu::debugPrint(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
    va_end(args);
}

void OledMenu::debug(MenuProperties *properties, uint8_t index) {
    debugPrint(properties->text[index]);
}

void OledMenu::wait(uint32_t time) {
    delay(time);
}