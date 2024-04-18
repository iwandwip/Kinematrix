/*
 *  lcd-menu.h
 *
 *  lcd menu header
 *  Created on: 2023. 4. 3
 */

#ifndef KADITA_LCD_MENU_H
#define KADITA_LCD_MENU_H

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "SPI.h"

const uint8_t MAX_BUFF_LEN = 20;

struct MenuCursor {
    bool up;
    bool down;
    bool select;
    bool back;
    bool show;
};

struct MenuProperties {
    char option[MAX_BUFF_LEN];
    char **text;
    bool *isHasCb;
    uint8_t len;
    int select;
    int index;
    int upCount;
};

class LcdMenu : public LiquidCrystal_I2C {
private:
    MenuCursor *cursor_;
    uint32_t lcdPrintTimer;
    using LiquidCrystal_I2C::LiquidCrystal_I2C;

public:
    void initialize();
    void onListen(MenuCursor *menuCursor, void (*listenCallback)());
    void showMenu(MenuProperties *properties, bool forced = false);
    void showCursor(MenuProperties *properties);
    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)() = nullptr);
    void formatMenu(MenuProperties *properties, uint8_t index, const char *format, ...);
    void backToMenu(MenuProperties *beforeProperties, MenuProperties *afterProperties);
    void clearMenu(MenuProperties *firstMenu, ...);
    int begin(int nums);
    int get(int nums);
    MenuProperties *end();
    LcdMenu &operator[](uint8_t index);
    void freeCharArray(char *str);
    MenuProperties *createMenu(int menuSize, ...);
    MenuProperties *createEmptyMenu(int menuSize, const char *text = nullptr);
    void freeMenu(MenuProperties *menuProperties);
    void showCursor(bool state);
    MenuCursor getCursor();
    bool cursorUp();
    bool cursorDown();
    bool cursorSelect();
    bool cursorBack();
    void debugPrint(const char *format, ...);
    void debug(MenuProperties *properties, uint8_t index);
    void wait(uint32_t time);
};

#endif  // KADITA_LCD_MENU_H
