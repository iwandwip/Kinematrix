#include "lcd-menu.h"

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args)
#else
#define printByte(args)  print(args,BYTE)
#endif

#define CURSOR_DOWN_CHAR  0
#define CURSOR_UP_CHAR  1
#define CURSOR_BLANK_CHAR  2

uint8_t cursorDownChar[8] = {0x00, 0x00, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04};
uint8_t cursorUpChar[8] = {0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x00, 0x00};
uint8_t cursorBlankChar[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void LcdMenu::initialize(bool _debug, void (*initCallback)()) {
    setLen();
    LiquidCrystal_I2C::init();
    LiquidCrystal_I2C::backlight();
    LiquidCrystal_I2C::clear();

    if (initCallback != nullptr) initCallback();

    LiquidCrystal_I2C::createChar(CURSOR_DOWN_CHAR, cursorDownChar);
    LiquidCrystal_I2C::createChar(CURSOR_UP_CHAR, cursorUpChar);
    LiquidCrystal_I2C::createChar(CURSOR_BLANK_CHAR, cursorBlankChar);
    if (!_debug) return;
    Serial.print("| [NAME]: ");
    Serial.print("LCD");
    Serial.print(" [INFO]: Init Success");
    Serial.println();
}

void LcdMenu::setLen(int row, int col) {
    lcdTotalRow = row - 1;
    lcdTotalCol = col;
}

void LcdMenu::onListen(MenuCursor *menuCursor, void (*listenCallback)()) {
    cursor_ = menuCursor;
    listenCallback();
}

void LcdMenu::showMenu(MenuProperties *properties, bool forced, uint32_t showTime) {
    if (strlen(properties->option) != 0 && !forced) return;
    char buffer[properties->len][MAX_BUFF_LEN];

    for (int i = 0; i < properties->len; ++i) {
        snprintf(buffer[i], MAX_BUFF_LEN, "  %s                    ", properties->text[i]);
        if (properties->index == i && forced) buffer[i][0] = ' ';
        if (properties->index == i && !forced) buffer[i][0] = '>';
    }

    if (millis() - lcdPrintTimer >= showTime || forced) {
        if (!forced) this->showCursor(properties);
        LiquidCrystal_I2C::setCursor(0, 0);
        for (int i = 0; i < lcdTotalRow; ++i) LiquidCrystal_I2C::print(buffer[properties->select][i]);
        if (!forced) this->showCursor(properties);

        if (!forced) this->showCursor(properties);
        LiquidCrystal_I2C::setCursor(0, 1);
        for (int i = 0; i < lcdTotalRow; ++i) LiquidCrystal_I2C::print(buffer[properties->select + 1][i]);
        if (!forced) this->showCursor(properties);

        if (lcdTotalCol > 2) {
            if (!forced) this->showCursor(properties);
            LiquidCrystal_I2C::setCursor(0, 2);
            for (int i = 0; i < lcdTotalRow; ++i) LiquidCrystal_I2C::print(buffer[properties->select + 2][i]);
            if (!forced) this->showCursor(properties);

            if (!forced) this->showCursor(properties);
            LiquidCrystal_I2C::setCursor(0, 3);
            for (int i = 0; i < lcdTotalRow; ++i) LiquidCrystal_I2C::print(buffer[properties->select + 3][i]);
            if (!forced) this->showCursor(properties);
            lcdPrintTimer = millis();
        }
    }

    if (forced) return;
    this->onCursor(properties);
}

void LcdMenu::onCursor(MenuProperties *properties) {
    if (cursor_->down) {
        if (properties->index < properties->len - 1) {
            if (properties->upCount == lcdTotalCol - 1 && properties->select < properties->len - lcdTotalCol)
                properties->select++;
            if (properties->upCount < lcdTotalCol - 1) properties->upCount++;
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
            properties->upCount = lcdTotalCol - 1;
            properties->select = properties->len - lcdTotalCol;
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

void LcdMenu::showCursor(MenuProperties *properties) {
    if (cursor_->show) {
        if (lcdTotalCol <= 2) {
            if (properties->index == 0) {
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 0);
                LiquidCrystal_I2C::printByte(CURSOR_BLANK_CHAR);
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 1);
                LiquidCrystal_I2C::printByte(CURSOR_DOWN_CHAR);
            } else if (properties->index == properties->len - 1) {
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 0);
                LiquidCrystal_I2C::printByte(CURSOR_UP_CHAR);
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 1);
                LiquidCrystal_I2C::printByte(CURSOR_BLANK_CHAR);
            } else {
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 0);
                LiquidCrystal_I2C::printByte(CURSOR_UP_CHAR);
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 1);
                LiquidCrystal_I2C::printByte(CURSOR_DOWN_CHAR);
            }
        } else {
            if (properties->index == 0) {
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 0);
                LiquidCrystal_I2C::printByte(CURSOR_BLANK_CHAR);
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 3);
                LiquidCrystal_I2C::printByte(CURSOR_DOWN_CHAR);
            } else if (properties->index == properties->len - 1) {
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 0);
                LiquidCrystal_I2C::printByte(CURSOR_UP_CHAR);
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 3);
                LiquidCrystal_I2C::printByte(CURSOR_BLANK_CHAR);
            } else {
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 0);
                LiquidCrystal_I2C::printByte(CURSOR_UP_CHAR);
                LiquidCrystal_I2C::setCursor(lcdTotalRow, 3);
                LiquidCrystal_I2C::printByte(CURSOR_DOWN_CHAR);
            }
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
        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
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

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor)) {
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

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
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

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(int state)) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        int itemIndex = -1;
        for (int i = 0; i < properties->len; ++i) {
            if (strcmp(properties->text[i], options) == 0) {
                itemIndex = i;
                break;
            }
        }

        if (itemIndex >= 0) {
            optionCallback(properties->itemState[itemIndex]);
        }

        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
                       void (*optionCallback)(int state)) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
            properties->callbackMenu[i] = onClickCallback;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        int itemIndex = -1;
        for (int i = 0; i < properties->len; ++i) {
            if (strcmp(properties->text[i], options) == 0) {
                itemIndex = i;
                break;
            }
        }

        if (itemIndex >= 0) {
            optionCallback(properties->itemState[itemIndex]);
        }

        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor, int state)) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        int itemIndex = -1;
        for (int i = 0; i < properties->len; ++i) {
            if (strcmp(properties->text[i], options) == 0) {
                itemIndex = i;
                break;
            }
        }

        if (itemIndex >= 0) {
            optionCallback(cursor_, properties->itemState[itemIndex]);
        }

        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

void LcdMenu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
                       void (*optionCallback)(MenuCursor *cursor, int state)) {
    for (int i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            properties->isHasCb[i] = true;
            properties->callbackMenu[i] = onClickCallback;
        }
    }

    if (strcmp(properties->option, options) == 0 && optionCallback != nullptr) {
        int itemIndex = -1;
        for (int i = 0; i < properties->len; ++i) {
            if (strcmp(properties->text[i], options) == 0) {
                itemIndex = i;
                break;
            }
        }

        if (itemIndex >= 0) {
            optionCallback(cursor_, properties->itemState[itemIndex]);
        }

        if (cursor_->back) {
            cursor_->back = 0;
            freeCharArray(properties->option);
            properties->select = 0;
            properties->index = 0;
            properties->upCount = 0;
        }
    }
}

int LcdMenu::getState(MenuProperties *properties, uint8_t index) {
    if (index >= properties->len) {
        return -1;
    }
    return properties->itemState[index];
}

int LcdMenu::getState(MenuProperties *properties, const char *options) {
    for (uint8_t i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            return properties->itemState[i];
        }
    }
    return -1;
}

void LcdMenu::setState(MenuProperties *properties, uint8_t index, int state) {
    if (index < properties->len) {
        properties->itemState[index] = state;
    }
}

void LcdMenu::setState(MenuProperties *properties, const char *options, int state) {
    for (uint8_t i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            setState(properties, i, state);
            return;
        }
    }
}

void LcdMenu::updateMenuText(MenuProperties *properties, uint8_t index, int state, const char *format, ...) {
    if (index >= properties->len) {
        return;
    }

    properties->itemState[index] = state;

    va_list args;
    va_start(args, format);
    vsnprintf(properties->text[index], MAX_BUFF_LEN, format, args);
    va_end(args);
}

void LcdMenu::formatMenu(MenuProperties *properties, uint8_t index, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(properties->text[index], MAX_BUFF_LEN, format, args);
    va_end(args);
    return;
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
        properties->callbackMenu = new CallbackMenu[menuSize];
        properties->itemState = new int[menuSize];

        for (uint8_t i = 0; i < menuSize; ++i) {
            const char *menuItem = va_arg(args, const char *);
            properties->text[i] = new char[MAX_BUFF_LEN];
            strcpy(properties->text[i], "default");
            if (menuItem != nullptr) strcpy(properties->text[i], menuItem);
            properties->isHasCb[i] = false;
            properties->callbackMenu[i] = nullptr;
            properties->itemState[i] = 0;
        }
    } else {
        properties->text = nullptr;
        properties->isHasCb = nullptr;
        properties->callbackMenu = nullptr;
        properties->itemState = nullptr;
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
        properties->callbackMenu = new CallbackMenu[menuSize];
        properties->itemState = new int[menuSize];

        for (uint8_t i = 0; i < menuSize; ++i) {
            properties->text[i] = new char[MAX_BUFF_LEN];
            strcpy(properties->text[i], "default");
            if (text != nullptr) strcpy(properties->text[i], text);
            properties->isHasCb[i] = false;
            properties->callbackMenu[i] = nullptr;
            properties->itemState[i] = 0;
        }
    } else {
        properties->text = nullptr;
        properties->isHasCb = nullptr;
        properties->callbackMenu = nullptr;
        properties->itemState = nullptr;
    }
    return properties;
}

void LcdMenu::freeMenu(MenuProperties *menuProperties) {
    if (menuProperties->text) {
        for (uint8_t i = 0; i < menuProperties->len; ++i) {
            delete[] menuProperties->text[i];
        }
        delete[] menuProperties->text;
    }
    delete[] menuProperties->isHasCb;
    delete[] menuProperties->callbackMenu;
    delete[] menuProperties->itemState;
    delete menuProperties;
}

void LcdMenu::showCursor(bool state) {
    cursor_->show = state;
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

#if defined(ESP32) || defined(ESP8266)

bool LcdMenu::connectToWiFi(const char *ssid, const char *password, int timeoutSeconds) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    clear();
    setCursor(0, 0);
    print("Connecting to");
    setCursor(0, 1);
    print(ssid);

    if (lcdTotalCol > 2) {
        setCursor(0, 2);
        print("Please wait...");
    }

    int timeout = timeoutSeconds * 2;
    int tries = 0;

    while (WiFi.status() != WL_CONNECTED && tries < timeout) {
        delay(500);
        setCursor(tries % lcdTotalRow, lcdTotalCol > 2 ? 3 : 1);
        print(".");
        Serial.print(".");
        tries++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        clear();
        setCursor(0, 0);
        print("WiFi Connected!");
        setCursor(0, 1);
        print("IP Address:");

        if (lcdTotalCol > 2) {
            setCursor(0, 2);
            print(WiFi.localIP().toString());
        } else {
            String ipStr = WiFi.localIP().toString();
            for (int i = 0; i < ipStr.length() + lcdTotalRow; i++) {
                setCursor(0, 1);
                if (i < ipStr.length()) {
                    print(ipStr.substring(i, min(i + lcdTotalRow, (int) ipStr.length())));
                }
                delay(300);
            }
        }

        delay(2000);
        return true;
    } else {
        Serial.println("WiFi connection failed");

        clear();
        setCursor(0, 0);
        print("WiFi Connection");
        setCursor(0, 1);
        print("Failed!");
        delay(2000);
        return false;
    }
}

String LcdMenu::getWiFiStatus() {
    switch (WiFi.status()) {
        case WL_CONNECTED:
            return "Connected to " + String(WiFi.SSID());
        case WL_CONNECT_FAILED:
            return "Connection Failed";
        case WL_IDLE_STATUS:
            return "Idle";
        case WL_NO_SSID_AVAIL:
            return "No SSID Available";
        case WL_DISCONNECTED:
            return "Disconnected";
        default:
            return "Status: " + String(WiFi.status());
    }
}

String LcdMenu::getIPAddress() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    } else {
        return "0.0.0.0";
    }
}

#endif