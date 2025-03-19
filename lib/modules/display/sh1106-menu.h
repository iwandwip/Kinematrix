#ifndef KINEMATRIX_SH1106_MENU_H
#define KINEMATRIX_SH1106_MENU_H

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

public:
  SH1106Menu(uint8_t address = 0x3C, int sda = -1, int scl = -1, OLEDDISPLAY_GEOMETRY g = GEOMETRY_128_64)
    : SH1106Wire(address, sda, scl, g),
      oledPrintTimer(0), menuItemHeight(12), menuStartY(14),
      cursorWidth(8), displayRows(3), displayWidth(128), displayHeight(64),
      listenerCallback(nullptr) {}

  void initialize(bool _debug = false, void (*initCallback)() = nullptr);
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
};

#endif