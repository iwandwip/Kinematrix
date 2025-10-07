#ifndef KINEMATRIX_LCD_MENU_H
#define KINEMATRIX_LCD_MENU_H

#pragma message("[COMPILED]: lcd-menu.h")

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "SPI.h"

#if defined(ESP32) || defined(ESP8266)
#define WIFI_SUPPORTED
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#endif

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
    int *itemState;
};

class LcdMenu : public LiquidCrystal_I2C {
private:
    MenuCursor *cursor_;
    uint32_t lcdPrintTimer;
    int lcdTotalRow;
    int lcdTotalCol;
    using LiquidCrystal_I2C::LiquidCrystal_I2C;

public:
    void initialize(bool _debug = false, void (*initCallback)() = nullptr);
    void setLen(int row = 16, int col = 2);
    void onListen(MenuCursor *menuCursor, void (*listenCallback)());
    void showMenu(MenuProperties *properties, bool forced = false, uint32_t showTime = 250);
    void onCursor(MenuProperties *properties);
    void showCursor(MenuProperties *properties);

    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)());
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)());
    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor));
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)(MenuCursor *cursor));

    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(int state));
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)(int state));
    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor, int state));
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)(MenuCursor *cursor, int state));

    int getState(MenuProperties *properties, uint8_t index);
    int getState(MenuProperties *properties, const char *options);
    void setState(MenuProperties *properties, uint8_t index, int state);
    void setState(MenuProperties *properties, const char *options, int state);
    void updateMenuText(MenuProperties *properties, uint8_t index, int state, const char *format, ...);

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

#ifdef WIFI_SUPPORTED
    bool connectToWiFi(const char *ssid, const char *password, int timeoutSeconds = 10);
    String getWiFiStatus();
    String getIPAddress();
#endif
};

#endif