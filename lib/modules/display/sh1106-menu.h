#ifndef SH1106_MENU_H
#define SH1106_MENU_H

#pragma message("[COMPILED]: sh1106-menu.h")

#include "Arduino.h"
#include "SH1106Wire.h"
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
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)(MenuCursor *cursor));

    int getState(MenuProperties *properties, uint8_t index);
    int getState(MenuProperties *properties, const char *options);
    void setState(MenuProperties *properties, uint8_t index, int state);
    void setState(MenuProperties *properties, const char *options, int state);
    void updateMenuText(MenuProperties *properties, uint8_t index, int state, const char *format, ...);

    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(int state));
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)(int state));
    void onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor, int state));
    void onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(), void (*optionCallback)(MenuCursor *cursor, int state));

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

    void flipVertical(bool flip);
    void flipHorizontal(bool flip);
    void setFlip(bool horizontalFlip, bool verticalFlip);

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

    void renderInfoScreen(const char *title, const char *line1, const char *line2, const char *line3);
    void renderInfoScreenCenter(const char *title, const char *line1, const char *line2, const char *line3);
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
    void renderSignalStrength(int strength, const char *networkName = nullptr);
    void renderClock(int hour, int minute, int second = -1, bool isAnalog = false);
    void renderPercentageCircle(int percentage, const char *text = nullptr);
    void renderScrollingText(const char *text, int speed = 50, int scrollCount = 1);
    void renderAnimatedLoading(int frame);
    void renderToggleSwitch(const char *label, bool state);
    void renderNotification(const char *message, int timeMs = 3000);
    void renderModal(const char *title, const char *message, const char *buttonText, bool isSelected, bool isError);

    void renderRadialMenu(const char *title, const char *options[], int numOptions, int selectedOption = -1);
    void renderTabView(const char *tabs[], int numTabs, int activeTab, const char *content);
    void renderHistogram(const char *title, int values[], int numValues, int maxValue);
    void renderCalendar(int year, int month, int highlightDay = -1);
    void renderGridMenu(const char *options[], int numOptions, int columns, int selectedOption = -1);
    void renderSensorGraph(const char *title, int values[], int numValues, int minValue, int maxValue, bool autoScroll = true);
    void renderQRCode(const char *data, int size = 3);
    void renderGauge(const char *title, int value, int minValue, int maxValue);
    void renderSpeedometer(int speed, int maxSpeed = 100);
    void renderWeatherIcon(int weatherType, const char *temperature = nullptr);
    void renderPopupMenu(const char *title, const char *options[], int numOptions, int selectedOption = 0);
    void renderAlertBox(const char *title, const char *message, bool isError = false);
    void renderProgressCircle(const char *title, int percentage);
    void renderSplitScreen(const char *leftTitle, const char *leftContent, const char *rightTitle, const char *rightContent);
    void renderDataTable(const char *headers[], const char *data[][3], int rows, int cols);
    void renderBubbleChart(const char *title, int values[], int numValues, int maxValue);
    void renderStarRating(const char *title, float rating, float maxRating = 5.0);
    void renderRoundedButton(int x, int y, int width, int height, const char *text, bool inverted = false);
    void renderPieChart(const char *title, int values[], int numValues);
    void renderTabbedData(const char *title, const char *tabs[], const char *contents[], int numTabs, int activeTab);

    void renderDropdownMenu(const char *title, const char *options[], int numOptions, int selectedOption = 0);
    void renderListMenu(const char *title, const char *options[], int numOptions, int selectedOption = 0, int startIndex = 0);
    void renderCheckboxMenu(const char *title, const char *options[], bool checked[], int numOptions, int selectedOption = 0);
    void renderRadioMenu(const char *title, const char *options[], int numOptions, int selectedOption = 0);
    void renderSelectionWheel(const char *title, const char *options[], int numOptions, int selectedOption = 0);
    void renderMenuTiles(const char *titles[], const uint8_t *icons[], int numTiles, int columns, int selectedTile = -1);
    void renderHorizontalMenu(const char *options[], int numOptions, int selectedOption = 0);
    void renderVerticalMenu(const char *options[], int numOptions, int selectedOption = 0, int startIndex = 0);
    void renderIconMenu(const char *options[], const uint8_t *icons[], int numOptions, int selectedOption = -1);
    void renderNestedMenu(const char *title, const char *items[], bool hasSubmenu[], int numItems, int selectedItem = 0);
    void renderContextMenu(const char *options[], int numOptions, int x, int y, int selectedOption = 0);
    void renderSettingsMenu(const char *title, const char *settings[], const char *values[], int numSettings, int selectedSetting = 0);
    void renderNumericInput(const char *title, int value, int minValue, int maxValue, int step = 1);
    void renderDatePicker(const char *title, int year, int month, int day);
    void renderTimePicker(const char *title, int hour, int minute);
    void renderColorPicker(const char *title, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255);
    void renderSlidingMenu(const char *title, const char *options[], int numOptions, int selectedOption = 0, int slidePosition = 0);
    void renderSegmentedControl(const char *title, const char *segments[], int numSegments, int selectedSegment = 0);
    void renderExpandableMenu(const char *title, const char *categories[], const char *items[][5], int numItemsPerCategory[], int numCategories, int expandedCategory = -1);
    void renderDrawerMenu(const char *title, const char *options[], int numOptions, int selectedOption = 0, bool isOpen = true);
};

#endif