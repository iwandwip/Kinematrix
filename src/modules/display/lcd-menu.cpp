/*
 *  lcd-menu.cpp
 *
 *  lcd menu c
 *  Created on: 2023. 4. 3
 */

#include "lcd-menu.h"

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args)
#else
#define printByte(args)  print(args,BYTE)
#endif

#define CURSOR_DOWN_CHAR  0
#define CURSOR_UP_CHAR  1
#define CURSOR_BLANK_CHAR  2

const uint8_t MAX_LCD_ROW_LEN = 2;
const uint8_t MAX_LCD_ROW = 16 - 1;

uint8_t cursorDownChar[8] = {0x00, 0x00, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04};
uint8_t cursorUpChar[8] = {0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x00, 0x00};
uint8_t cursorBlankChar[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void LcdMenu::initialize(void (*initCallback)()) {
    LiquidCrystal_I2C::init();
    LiquidCrystal_I2C::backlight();
    LiquidCrystal_I2C::createChar(CURSOR_DOWN_CHAR, cursorDownChar);
    LiquidCrystal_I2C::createChar(CURSOR_UP_CHAR, cursorUpChar);
    LiquidCrystal_I2C::createChar(CURSOR_BLANK_CHAR, cursorBlankChar);
    if (initCallback != nullptr) initCallback();
}

void LcdMenu::onListen(MenuCursor *menuCursor, void (*listenCallback)()) {
    cursor_ = menuCursor;
    listenCallback();
}

void LcdMenu::showMenu(MenuProperties *properties, bool forced) {
    if (strlen(properties->option) != 0 && !forced) return;
    char buffer[properties->len][MAX_BUFF_LEN];

    for (int i = 0; i < properties->len; ++i) {
        snprintf(buffer[i], MAX_BUFF_LEN, "  %s            ", properties->text[i]);
        if (properties->index == i) buffer[i][0] = '>';
    }

    if (millis() - lcdPrintTimer >= 250 || forced) {
        if (!forced) showCursor(properties);
        LiquidCrystal_I2C::setCursor(0, 0);
        LiquidCrystal_I2C::print(buffer[properties->select]);
        if (!forced) showCursor(properties);

        if (!forced) showCursor(properties);
        LiquidCrystal_I2C::setCursor(0, 1);
        LiquidCrystal_I2C::print(buffer[properties->select + 1]);
        if (!forced) showCursor(properties);
        lcdPrintTimer = millis();
    }

    if (forced) return;
    if (cursor_->down) {
        if (properties->index < properties->len - 1) {
            if (properties->upCount == MAX_LCD_ROW_LEN - 1 && properties->select < properties->len - MAX_LCD_ROW_LEN)
                properties->select++;
            if (properties->upCount < MAX_LCD_ROW_LEN - 1) properties->upCount++;
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
            properties->upCount = 1;
            properties->select = (properties->len - 1) - 1;
        }
    }
    if (cursor_->select) {
        cursor_->select = 0;
        if (properties->isHasCb[properties->index]) {
            strcpy(properties->option, properties->text[properties->index]);
        }
    }
}

void LcdMenu::showCursor(MenuProperties *properties) {
    if (cursor_->show) {
        if (properties->index == 0) {
            LiquidCrystal_I2C::setCursor(MAX_LCD_ROW, 0);
            LiquidCrystal_I2C::printByte(CURSOR_BLANK_CHAR);
            LiquidCrystal_I2C::setCursor(MAX_LCD_ROW, 1);
            LiquidCrystal_I2C::printByte(CURSOR_DOWN_CHAR);
        } else if (properties->index == properties->len - 1) {
            LiquidCrystal_I2C::setCursor(MAX_LCD_ROW, 0);
            LiquidCrystal_I2C::printByte(CURSOR_UP_CHAR);
            LiquidCrystal_I2C::setCursor(MAX_LCD_ROW, 1);
            LiquidCrystal_I2C::printByte(CURSOR_BLANK_CHAR);
        } else {
            LiquidCrystal_I2C::setCursor(MAX_LCD_ROW, 0);
            LiquidCrystal_I2C::printByte(CURSOR_UP_CHAR);
            LiquidCrystal_I2C::setCursor(MAX_LCD_ROW, 1);
            LiquidCrystal_I2C::printByte(CURSOR_DOWN_CHAR);
        }
    }
}

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)()) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
        }
    }
    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        optionCallback();
        return;
    }
}

void LcdMenu::formatMenu(MenuProperties *properties, uint8_t index, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(properties->text[index], MAX_BUFF_LEN, format, args);
    va_end(args);
    return;
}

void LcdMenu::backToMenu(MenuProperties *beforeProperties, MenuProperties *afterProperties) {
    freeCharArray(beforeProperties->option);
    beforeProperties->select = 0;
    beforeProperties->index = 0;
    beforeProperties->upCount = 0;

    freeCharArray(afterProperties->option);
    afterProperties->select = 0;
    afterProperties->index = 0;
    afterProperties->upCount = 0;
}

void LcdMenu::clearMenu(MenuProperties *firstMenu, ...) {
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

int LcdMenu::begin(int nums) {
    return nums;
}

int LcdMenu::get(int nums) {
    return nums;
}

MenuProperties *LcdMenu::end() {
    return nullptr;
}

LcdMenu &LcdMenu::operator[](uint8_t index) {
    return *this;
}

void LcdMenu::freeCharArray(char *str) {
    for (int i = 0; str[i] != '\0'; ++i) {
        str[i] = '\0';
    }
}

MenuProperties *LcdMenu::createMenu(int menuSize, ...) {
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
        for (uint8_t i = 0; i < menuSize; ++i) {
            const char *menuItem = va_arg(args, const char *);
            properties->text[i] = new char[20];
            strcpy(properties->text[i], "default");
            if (menuItem != nullptr) strcpy(properties->text[i], menuItem);
            properties->isHasCb[i] = false;
        }
    } else {
        properties->text = nullptr;
        properties->isHasCb = nullptr;
    }

    va_end(args);
    return properties;
}

MenuProperties *LcdMenu::createEmptyMenu(int menuSize, const char *text) {
    MenuProperties *properties = new MenuProperties;

    properties->option[0] = '\0';
    properties->len = menuSize;

    properties->select = 0;
    properties->index = 0;
    properties->upCount = 0;

    if (menuSize > 0) {
        properties->text = new char *[menuSize];
        properties->isHasCb = new bool[menuSize];
        for (uint8_t i = 0; i < menuSize; ++i) {
            properties->text[i] = new char[20];
            strcpy(properties->text[i], "default");
            if (text != nullptr) strcpy(properties->text[i], text);
            properties->isHasCb[i] = false;
        }
    } else {
        properties->text = nullptr;
        properties->isHasCb = nullptr;
    }
    return properties;
}

void LcdMenu::freeMenu(MenuProperties *menuProperties) {
    for (uint8_t i = 0; i < menuProperties->len; ++i) {
        delete[] menuProperties->text[i];
    }
    delete[] menuProperties->text;
    delete[] menuProperties->isHasCb;
    delete menuProperties;
}

void LcdMenu::showCursor(bool state) {
    cursor_->show = state;
}

MenuCursor LcdMenu::getCursor() {
    return *cursor_;
}

bool LcdMenu::cursorUp() {
    return cursor_->up;
}

bool LcdMenu::cursorDown() {
    return cursor_->down;
}

bool LcdMenu::cursorSelect() {
    return cursor_->select;
}

bool LcdMenu::cursorBack() {
    return cursor_->back;
}

void LcdMenu::debugPrint(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
    va_end(args);
}

void LcdMenu::debug(MenuProperties *properties, uint8_t index) {
    debugPrint(properties->text[index]);
}

void LcdMenu::wait(uint32_t time) {
    delay(time);
}