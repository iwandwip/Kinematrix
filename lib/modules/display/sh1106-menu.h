#ifndef SH1106_MENU_H
#define SH1106_MENU_H

#pragma message("[COMPILED]: sh1106-menu.h")

#include "Arduino.h"
#include "SH1106Wire.h"
#include "SPI.h"

// Wi-Fi support only for ESP32 and ESP8266
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
};

class SH1106Menu : public SH1106Wire {
private:
    MenuCursor *cursor_;
    uint32_t oledPrintTimer;
    int menuItemHeight;
    int menuStartY;
    int cursorWidth;
    int displayRows;
    int16_t displayWidth;
    int16_t displayHeight;
    void (*listenerCallback)();

    void drawRoundedRect(int x, int y, int width, int height, int radius);
    void fillRoundedRect(int x, int y, int width, int height, int radius);

public:
    SH1106Menu(uint8_t address = 0x3C, int sda = -1, int scl = -1, OLEDDISPLAY_GEOMETRY g = GEOMETRY_128_64)
            : SH1106Wire(address, sda, scl, g),
              oledPrintTimer(0), menuItemHeight(12), menuStartY(14),
              cursorWidth(8), displayRows(3), displayWidth(128), displayHeight(64),
              listenerCallback(nullptr) {}

    void initialize(bool _debug = false, void (*initCallback)() = nullptr, bool flipVertical = false, bool flipHorizontal = false);
    void setDisplayParams(int itemHeight = 16, int startY = 10, int cursorW = 10, int rows = 3);
    void onListen(MenuCursor *menuCursor, void (*listenCallback)());
    void setListenerCallback(void (*callback)());
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
    void renderInfoScreen(const char *title, const char *line1, const char *line2, const char *line3);

#ifdef WIFI_SUPPORTED
    bool connectToWiFi(const char *ssid, const char *password, int timeoutSeconds = 10);
    String getWiFiStatus();
    String getIPAddress();
#endif

    void showLoadingBar(const char *title, int progressPercent);
    void showLoadingBarTimed(const char *title, int durationSeconds);
    void showCircleLoading(const char *title, int frame);

    void showToast(const char *message, uint16_t durationMs = 2000);
    bool showConfirmation(const char *title, const char *message);
    void drawBarChart(const char *title, int values[], int numValues, int maxValue);
    void drawLineChart(const char *title, int values[], int numValues, int minValue, int maxValue);
    void showScrollableText(const char *title, const char *text);
    int showSlider(const char *title, int minValue, int maxValue, int currentValue);
    void showTextInput(const char *title, char *buffer, int maxLength);

    void flipVertical(bool flip);
    void flipHorizontal(bool flip);
    void setFlip(bool horizontalFlip, bool verticalFlip);

    void renderLargeText(const char *text, int fontSize = 24, bool withBox = false);
    void renderSplashScreen(const char *title, const char *subtitle = nullptr, const uint8_t *logo = nullptr);
    void renderStatusScreen(const char *title, const char *status, bool isOk = true);
    void renderDualValueScreen(const char *title, const char *label1, const char *value1, const char *label2, const char *value2);
    void renderCountdownScreen(const char *title, int seconds, bool showProgress = true);
    void renderIconTextRow(int y, const uint8_t *icon, const char *text, int iconWidth = 16, int iconHeight = 16);
    void renderCenteredText(const char *text1, const char *text2 = nullptr, const char *text3 = nullptr);
    void renderMetricScreen(const char *title, const char *value, const char *unit, const char *subtitle = nullptr);
    void renderBoxedText(const char *lines[], int numLines);

    void renderBatteryStatus(int percentage, bool charging = false);
    void renderSignalStrength(int strength, const char *networkName = nullptr); // strength 0-4
    void renderClock(int hour, int minute, int second = -1, bool isAnalog = false);
    void renderPercentageCircle(int percentage, const char *text = nullptr);
    void renderScrollingText(const char *text, int speed = 50, int scrollCount = 1);
    void renderAnimatedLoading(int frame);
    void renderToggleSwitch(const char *label, bool state);
    void renderNotification(const char *message, int timeMs = 3000);
};

#endif