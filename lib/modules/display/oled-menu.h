/*
 *  oled-menu.h
 *
 *  OLED menu header
 *  Created on: 2025. 2. 26
 */

#ifndef KINEMATRIX_OLED_MENU_H
#define KINEMATRIX_OLED_MENU_H

#pragma message("[COMPILED]: oled-menu.h")

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SPI.h"
#include "Wire.h"

const uint8_t MAX_BUFF_LEN = 24;

typedef void (*CallbackMenu)();

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
    CallbackMenu *callbackMenu;
    uint8_t len;
    int select;
    int index;
    int upCount;
};

class OledMenu : public Adafruit_SSD1306 {
private:
    MenuCursor *cursor_;
    uint32_t oledPrintTimer;
    int menuItemHeight;    // Height of each menu item in pixels
    int menuStartY;        // Starting Y position for a menu
    int cursorWidth;       // Width of cursor indicator
    int displayRows;       // Number of rows to display (3 by default)

public:
    // Constructor for the 128x64 OLED display (using I2C)
    OledMenu(uint8_t w = 128, uint8_t h = 64, TwoWire *twi = &Wire,
             int8_t rst_pin = -1, uint32_t clkDuring = 400000UL, uint32_t clkAfter = 100000UL)
            : Adafruit_SSD1306(w, h, twi, rst_pin, clkDuring, clkAfter),
              oledPrintTimer(0) {}

    void initialize(bool _debug = false, void (*initCallback)() = nullptr);
    void setDisplayParams(int itemHeight = 16, int startY = 10, int cursorW = 10, int rows = 3);
    void onListen(MenuCursor *menuCursor, void (*listenCallback)());
    void showMenu(MenuProperties *properties, bool forced = false, uint32_t showTime = 250);
    void onCursor(MenuProperties *properties);
    void drawCursor(int y, bool isUp = false, bool isDown = false);
    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)());
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)());
    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor));
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
                  void (*optionCallback)(MenuCursor *cursor));
    void formatMenu(MenuProperties *properties, uint8_t index, const char *format, ...);
    void clearMenu(MenuProperties *firstMenu, ...);
    int begin(int nums);
    int get(int nums);
    MenuProperties *end();
    void freeCharArray(char *str);
    MenuProperties *createMenu(int menuSize, ...);
    MenuProperties *createEmptyMenu(int menuSize, const char *text = nullptr);
    void freeMenu(MenuProperties *menuProperties);
    void showCursor(bool state);
    void debugPrint(const char *format, ...);
    void debug(MenuProperties *properties, uint8_t index);
    void wait(uint32_t time);
};

#endif  // KINEMATRIX_OLED_MENU_H