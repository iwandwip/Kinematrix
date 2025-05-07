#include "sh1106-menu.h"

void SH1106Menu::initialize(bool _debug, void (*initCallback)(), bool flipVertical, bool flipHorizontal) {
    setDisplayParams(16, 0, 8, 4);
    SH1106Wire::init();
    SH1106Wire::setFont(ArialMT_Plain_10);
    SH1106Wire::setTextAlignment(TEXT_ALIGN_LEFT);
    SH1106Wire::clear();
    setFlip(flipHorizontal, flipVertical);

    SH1106Wire::drawRect(0, 0, displayWidth, displayHeight);
    SH1106Wire::drawString(5, 5, "SH1106 INIT");
    SH1106Wire::display();
    delay(500);
    SH1106Wire::clear();
    SH1106Wire::display();

    if (initCallback != nullptr) initCallback();

    if (!_debug) return;
    Serial.print("| [NAME]: ");
    Serial.print("SH1106");
    Serial.print(" [INFO]: Init Success");
    Serial.println();
}

void SH1106Menu::setDisplayParams(int itemHeight, int startY, int cursorW, int rows) {
    menuItemHeight = itemHeight;
    menuStartY = startY;
    cursorWidth = cursorW;
    displayRows = rows;
    displayWidth = this->width();
    displayHeight = this->height();
}

void SH1106Menu::onListen(MenuCursor *menuCursor, void (*listenCallback)()) {
    cursor_ = menuCursor;
    listenCallback();
}

void SH1106Menu::setListenerCallback(void (*callback)()) {
    listenerCallback = callback;
}

void SH1106Menu::showMenu(MenuProperties *properties, bool forced, uint32_t showTime) {
    if (strlen(properties->option) != 0 && !forced) return;

    if (millis() - oledPrintTimer >= showTime || forced) {
        clear();
        setTextAlignment(TEXT_ALIGN_LEFT);
        setFont(ArialMT_Plain_10);

        int maxItems = min(displayRows, properties->len - properties->select);
        int menuY = menuStartY;

        for (int i = 0; i < maxItems; i++) {
            int textIndex = properties->select + i;
            bool isSelected = (properties->index == textIndex);

            if (isSelected) {
                setColor(WHITE);
                fillRect(0, menuY, displayWidth, menuItemHeight);
                setColor(BLACK);
            } else {
                setColor(BLACK);
                fillRect(0, menuY, displayWidth, menuItemHeight);
                setColor(WHITE);
                drawRect(0, menuY, displayWidth, menuItemHeight);
            }

            int textYposition = menuY + (menuItemHeight - 10) / 2;

            if (isSelected) {
                drawString(8, textYposition, properties->text[textIndex]);
            } else {
                drawString(8, textYposition, properties->text[textIndex]);
            }

            menuY += menuItemHeight;
        }

        if (!forced && cursor_->show) {
            if (properties->select > 0) {
                setColor(WHITE);
                fillTriangle(
                        displayWidth - 10, 5,
                        displayWidth - 5, 5,
                        displayWidth - 7, 2);
            }

            if (properties->select + displayRows < properties->len) {
                setColor(WHITE);
                fillTriangle(
                        displayWidth - 10, displayHeight - 5,
                        displayWidth - 5, displayHeight - 5,
                        displayWidth - 7, displayHeight - 2);
            }
        }

        display();
        oledPrintTimer = millis();
    }

    if (forced) return;
    this->onCursor(properties);
}

void SH1106Menu::onCursor(MenuProperties *properties) {
    if (cursor_->down) {
        if (properties->index < properties->len - 1) {
            properties->index++;

            if (properties->index >= properties->select + displayRows) {
                properties->select++;
            }
        } else {
            properties->index = 0;
            properties->select = 0;
        }
    }

    if (cursor_->up) {
        if (properties->index > 0) {
            properties->index--;

            if (properties->index < properties->select) {
                properties->select--;
            }
        } else {
            properties->index = properties->len - 1;
            int maxItemsVisible = min(displayRows, (int) properties->len);
            properties->select = max(0, properties->len - maxItemsVisible);
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

void SH1106Menu::drawCursor(int y, bool isUp, bool isDown) {
    if (isUp) {
        setColor(WHITE);
        fillTriangle(
                displayWidth / 2 - 5, y,
                displayWidth / 2 + 5, y,
                displayWidth / 2, y - 5);
    } else if (isDown) {
        setColor(WHITE);
        fillTriangle(
                displayWidth / 2 - 5, y,
                displayWidth / 2 + 5, y,
                displayWidth / 2, y + 5);
    }
}

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)()) {
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

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
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

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor)) {
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

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
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

int SH1106Menu::getState(MenuProperties *properties, uint8_t index) {
    if (index >= properties->len) {
        return -1;
    }
    return properties->itemState[index];
}

int SH1106Menu::getState(MenuProperties *properties, const char *options) {
    for (uint8_t i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            return properties->itemState[i];
        }
    }
    return -1;
}

void SH1106Menu::setState(MenuProperties *properties, uint8_t index, int state) {
    if (index < properties->len) {
        properties->itemState[index] = state;
    }
}

void SH1106Menu::setState(MenuProperties *properties, const char *options, int state) {
    for (uint8_t i = 0; i < properties->len; ++i) {
        if (strcmp(properties->text[i], options) == 0) {
            setState(properties, i, state);
            return;
        }
    }
}

void SH1106Menu::updateMenuText(MenuProperties *properties, uint8_t index, int state, const char *format, ...) {
    if (index >= properties->len) {
        return;
    }

    properties->itemState[index] = state;

    va_list args;
    va_start(args, format);
    vsnprintf(properties->text[index], MAX_BUFF_LEN, format, args);
    va_end(args);
}

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(int state)) {
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

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
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

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*optionCallback)(MenuCursor *cursor, int state)) {
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

void SH1106Menu::onSelect(MenuProperties *properties, const char *options, void (*onClickCallback)(),
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

void SH1106Menu::formatMenu(MenuProperties *properties, uint8_t index, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(properties->text[index], MAX_BUFF_LEN, format, args);
    va_end(args);
    return;
}

void SH1106Menu::clearMenu(MenuProperties *firstMenu, ...) {
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
        currentMenu = va_arg(args, MenuProperties *);
    }
    va_end(args);
}

int SH1106Menu::begin(int nums) {
    return nums;
}

int SH1106Menu::get(int nums) {
    return nums;
}

MenuProperties *SH1106Menu::end() {
    return nullptr;
}

void SH1106Menu::freeCharArray(char *str) {
    for (int i = 0; str[i] != '\0'; ++i) {
        str[i] = '\0';
    }
}

MenuProperties *SH1106Menu::createMenu(int menuSize, ...) {
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

MenuProperties *SH1106Menu::createEmptyMenu(int menuSize, const char *text) {
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

void SH1106Menu::freeMenu(MenuProperties *menuProperties) {
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

void SH1106Menu::showCursor(bool state) {
    cursor_->show = state;
}

void SH1106Menu::debugPrint(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.println(buffer);
    va_end(args);
}

void SH1106Menu::debug(MenuProperties *properties, uint8_t index) {
    debugPrint(properties->text[index]);
}

void SH1106Menu::wait(uint32_t time) {
    delay(time);
}

#ifdef WIFI_SUPPORTED

bool SH1106Menu::connectToWiFi(const char *ssid, const char *password, int timeoutSeconds) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    clear();
    setTextAlignment(TEXT_ALIGN_CENTER);
    setFont(ArialMT_Plain_10);
    drawString(64, 10, "Connecting to WiFi");
    drawString(64, 25, ssid);
    drawRect(24, 40, 80, 10);

    int timeout = timeoutSeconds * 2;
    int tries = 0;
    while (WiFi.status() != WL_CONNECTED && tries < timeout) {
        int progressBarWidth = map(tries, 0, timeout, 0, 80);
        fillRect(24, 40, progressBarWidth, 10);
        display();

        delay(500);
        Serial.print(".");
        tries++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        clear();
        drawString(64, 10, "WiFi Connected!");
        drawString(64, 25, "IP Address:");
        drawString(64, 40, WiFi.localIP().toString());
        display();
        delay(2000);
        return true;
    } else {
        Serial.println("WiFi connection failed");

        clear();
        drawString(64, 25, "WiFi Connection Failed");
        display();
        delay(2000);
        return false;
    }
}

String SH1106Menu::getWiFiStatus() {
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

String SH1106Menu::getIPAddress() {
    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    } else {
        return "0.0.0.0";
    }
}

#endif

void SH1106Menu::showLoadingBar(const char *title, int progressPercent) {
    progressPercent = constrain(progressPercent, 0, 100);

    clear();
    setTextAlignment(TEXT_ALIGN_CENTER);
    setFont(ArialMT_Plain_10);
    drawString(64, 10, title);

    drawRect(14, 30, 100, 15);
    fillRect(14, 30, progressPercent, 15);

    String percentText = String(progressPercent) + "%";
    drawString(64, 50, percentText);

    display();
}

void SH1106Menu::showLoadingBarTimed(const char *title, int durationSeconds) {
    unsigned long startTime = millis();
    unsigned long endTime = startTime + (durationSeconds * 1000);

    while (millis() < endTime) {
        unsigned long currentTime = millis();
        int progress = map(currentTime, startTime, endTime, 0, 100);

        showLoadingBar(title, progress);
        delay(50);
    }

    showLoadingBar(title, 100);
    delay(500);
}

void SH1106Menu::showCircleLoading(const char *title, int frame) {
    const int centerX = 64;
    const int centerY = 42;
    const int radius = 12;
    const int dotRadius = 2;
    const int numDots = 8;

    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    setFont(ArialMT_Plain_10);
    drawString(centerX, 5, title);

    frame = frame % numDots;

    for (int i = 0; i < numDots; i++) {
        float angle = 2 * PI * i / numDots;
        int x = centerX + cos(angle) * radius;
        int y = centerY + sin(angle) * radius;

        if (i == frame) {
            fillCircle(x, y, dotRadius);
        } else {
            int distance = (i > frame) ? i - frame : numDots - frame + i;
            int brightness = map(distance, 1, numDots - 1, 200, 50);

            setColor(WHITE);
            drawCircle(x, y, dotRadius);
            if (distance < numDots / 2) {
                drawCircle(x, y, dotRadius - 1);
            }
        }
    }

    display();
}

void SH1106Menu::showToast(const char *message, uint16_t durationMs) {
    int msgWidth = getStringWidth(message);
    int boxWidth = min(displayWidth - 10, msgWidth + 20);
    int boxHeight = 20;
    int boxX = (displayWidth - boxWidth) / 2;
    int boxY = displayHeight - boxHeight - 10;

    unsigned long startTime = millis();
    bool running = true;

    while (running) {
        if (millis() - startTime >= durationMs) {
            running = false;
        }

        if (cursor_->back) {
            cursor_->back = false;
            running = false;
        }

        if (listenerCallback != nullptr) {
            listenerCallback();
        }

        clear();
        setColor(WHITE);
        fillRect(boxX, boxY, boxWidth, boxHeight);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, boxY + 5, message);
        display();

        delay(10);
    }
}

bool SH1106Menu::showConfirmation(const char *title, const char *message) {
    bool result = false;
    bool deciding = true;
    bool selected = false;

    cursor_->up = false;
    cursor_->down = false;
    cursor_->select = false;
    cursor_->back = false;

    while (deciding) {
        if (listenerCallback != nullptr) {
            listenerCallback();
        }

        clear();

        setColor(WHITE);
        fillRect(0, 0, displayWidth, 15);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, 1, title);

        setColor(WHITE);
        setTextAlignment(TEXT_ALIGN_CENTER);

        drawString(displayWidth / 2, 20, message);

        int btnWidth = 40;
        int btnHeight = 15;
        int btnGap = 10;
        int yesX = (displayWidth / 2) - btnWidth - (btnGap / 2);
        int noX = (displayWidth / 2) + (btnGap / 2);
        int btnY = 38;

        if (selected) {
            setColor(WHITE);
            fillRect(yesX, btnY, btnWidth, btnHeight);
            setColor(BLACK);
            drawString(yesX + btnWidth / 2, btnY + 3, "Ya");
        } else {
            setColor(WHITE);
            drawRect(yesX, btnY, btnWidth, btnHeight);
            drawString(yesX + btnWidth / 2, btnY + 3, "Ya");
        }

        if (!selected) {
            setColor(WHITE);
            fillRect(noX, btnY, btnWidth, btnHeight);
            setColor(BLACK);
            drawString(noX + btnWidth / 2, btnY + 3, "Tidak");
        } else {
            setColor(WHITE);
            drawRect(noX, btnY, btnWidth, btnHeight);
            drawString(noX + btnWidth / 2, btnY + 3, "Tidak");
        }

        display();

        if (cursor_->up || cursor_->down) {
            selected = !selected;
            cursor_->up = false;
            cursor_->down = false;
        }

        if (cursor_->select) {
            deciding = false;
            result = selected;
            cursor_->select = false;
        }

        if (cursor_->back) {
            deciding = false;
            result = false;
            cursor_->back = false;
        }

        delay(50);
    }

    return result;
}

void SH1106Menu::drawBarChart(const char *title, int values[], int numValues, int maxValue) {
    clear();

    setColor(WHITE);
    fillRect(0, 0, displayWidth, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);

    setColor(WHITE);

    int chartX = 10;
    int chartY = 20;
    int chartWidth = displayWidth - 20;
    int chartHeight = displayHeight - chartY - 5;

    drawRect(chartX, chartY, chartWidth, chartHeight);

    if (numValues > 0 && maxValue > 0) {
        int barWidth = (chartWidth - 4) / numValues;
        int barMaxHeight = chartHeight - 4;

        for (int i = 0; i < numValues; i++) {
            int barHeight = map(values[i], 0, maxValue, 0, barMaxHeight);
            int barX = chartX + 2 + (i * barWidth);
            int barY = chartY + chartHeight - 2 - barHeight;

            fillRect(barX, barY, barWidth - 1, barHeight);
        }
    }

    display();
}

void SH1106Menu::drawLineChart(const char *title, int values[], int numValues, int minValue, int maxValue) {
    clear();

    setColor(WHITE);
    fillRect(0, 0, displayWidth, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);

    setColor(WHITE);

    int chartX = 10;
    int chartY = 20;
    int chartWidth = displayWidth - 20;
    int chartHeight = displayHeight - chartY - 5;

    drawRect(chartX, chartY, chartWidth, chartHeight);

    if (numValues > 1 && (maxValue > minValue)) {
        int pointSpacing = (chartWidth - 4) / (numValues - 1);

        for (int i = 0; i < numValues - 1; i++) {
            int x1 = chartX + 2 + (i * pointSpacing);
            int y1 = chartY + chartHeight - 2 - map(values[i], minValue, maxValue, 0, chartHeight - 4);

            int x2 = chartX + 2 + ((i + 1) * pointSpacing);
            int y2 = chartY + chartHeight - 2 - map(values[i + 1], minValue, maxValue, 0, chartHeight - 4);

            drawLine(x1, y1, x2, y2);
            fillCircle(x1, y1, 2);
        }

        fillCircle(chartX + 2 + ((numValues - 1) * pointSpacing),
                   chartY + chartHeight - 2 - map(values[numValues - 1], minValue, maxValue, 0, chartHeight - 4), 2);
    }

    display();
}

void SH1106Menu::showScrollableText(const char *title, const char *text) {
    int textLength = strlen(text);
    int maxLinesPerPage = 4;
    int lineHeight = 10;
    int textY = 18;
    int currentLine = 0;
    bool exiting = false;

    cursor_->up = false;
    cursor_->down = false;
    cursor_->select = false;
    cursor_->back = false;

    while (!exiting) {
        if (listenerCallback != nullptr) {
            listenerCallback();
        }

        clear();

        setColor(WHITE);
        fillRect(0, 0, displayWidth, 15);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, 1, title);

        setColor(WHITE);
        setTextAlignment(TEXT_ALIGN_LEFT);

        String textCopy = String(text);
        int startPos = 0;
        int lineStart = 0;
        int linesDrawn = 0;

        int totalLines = 0;
        int tempPos = 0;
        while (tempPos < textLength) {
            String tempLine = "";
            bool lineEnded = false;
            while (!lineEnded && tempPos < textLength) {
                tempLine += text[tempPos++];
                if (text[tempPos - 1] == '\n' || getStringWidth(tempLine.c_str()) > displayWidth - 20) {
                    lineEnded = true;
                }
            }
            totalLines++;
        }

        for (int i = 0; i <= currentLine + maxLinesPerPage && i < totalLines; i++) {
            String line = "";
            bool lineEnded = false;

            while (!lineEnded && startPos < textLength) {
                line += text[startPos++];
                if (text[startPos - 1] == '\n' || getStringWidth(line.c_str()) > displayWidth - 20) {
                    lineEnded = true;
                }
            }

            if (i >= currentLine && linesDrawn < maxLinesPerPage) {
                drawString(10, textY + (linesDrawn * lineHeight), line);
                linesDrawn++;
            }
        }

        if (currentLine > 0) {
            drawTriangle(displayWidth / 2 - 4, 16, displayWidth / 2 + 4, 16, displayWidth / 2, 13);
        }

        if (currentLine + maxLinesPerPage < totalLines) {
            drawTriangle(displayWidth / 2 - 4, displayHeight - 3, displayWidth / 2 + 4, displayHeight - 3, displayWidth / 2, displayHeight);
        }

        display();

        if (cursor_->up && currentLine > 0) {
            currentLine--;
            cursor_->up = false;
        }

        if (cursor_->down && currentLine + maxLinesPerPage < totalLines) {
            currentLine++;
            cursor_->down = false;
        }

        if (cursor_->back) {
            exiting = true;
            cursor_->back = false;
        }

        delay(50);
    }
}

int SH1106Menu::showSlider(const char *title, int minValue, int maxValue, int currentValue) {
    int sliderWidth = displayWidth - 30;
    int sliderX = 15;
    int sliderY = 35;
    int sliderHeight = 10;
    int result = currentValue;
    bool exiting = false;
    bool valueChanging = true;

    cursor_->up = false;
    cursor_->down = false;
    cursor_->select = false;
    cursor_->back = false;

    while (!exiting) {
        if (listenerCallback != nullptr) {
            listenerCallback();
        }

        clear();

        setColor(WHITE);
        fillRect(0, 0, displayWidth, 15);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, 1, title);

        setColor(WHITE);
        setTextAlignment(TEXT_ALIGN_CENTER);

        String valueText = String(result);
        drawString(displayWidth / 2, 20, valueText);

        drawRect(sliderX, sliderY, sliderWidth, sliderHeight);

        int knobPos = map(result, minValue, maxValue, 0, sliderWidth - 4);
        fillRect(sliderX + 2 + knobPos, sliderY - 2, 4, sliderHeight + 4);

        setTextAlignment(TEXT_ALIGN_LEFT);
        drawString(sliderX, sliderY + sliderHeight + 5, String(minValue));

        setTextAlignment(TEXT_ALIGN_RIGHT);
        drawString(sliderX + sliderWidth, sliderY + sliderHeight + 5, String(maxValue));

        setTextAlignment(TEXT_ALIGN_CENTER);
        if (valueChanging) {
            drawString(displayWidth / 2, 50, "^ Kurangi  v Tambah");
        } else {
            drawString(displayWidth / 2, 50, "Tekan Select untuk OK");
        }

        display();

        if (valueChanging) {
            if (cursor_->up && result > minValue) {
                result--;
                cursor_->up = false;
            }

            if (cursor_->down && result < maxValue) {
                result++;
                cursor_->down = false;
            }

            if (cursor_->select) {
                valueChanging = false;
                cursor_->select = false;
            }
        } else {
            if (cursor_->up || cursor_->down) {
                valueChanging = true;
                cursor_->up = false;
                cursor_->down = false;
            }

            if (cursor_->select) {
                exiting = true;
                cursor_->select = false;
            }
        }

        if (cursor_->back) {
            exiting = true;
            cursor_->back = false;
        }

        delay(50);
    }

    return result;
}

void SH1106Menu::showTextInput(const char *title, char *buffer, int maxLength) {
    const char *charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 !@#$%^&*()-_=+[]{}|;:'\",.<>/?\\";
    int charsetLength = strlen(charset);
    int currentChar = 0;
    int cursorPos = strlen(buffer);
    bool exiting = false;
    bool modeSelectChar = true;

    cursor_->up = false;
    cursor_->down = false;
    cursor_->select = false;
    cursor_->back = false;

    while (!exiting) {
        if (listenerCallback != nullptr) {
            listenerCallback();
        }

        clear();

        setColor(WHITE);
        fillRect(0, 0, displayWidth, 15);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, 1, title);

        setColor(WHITE);

        String currentText = String(buffer);
        if (getStringWidth(currentText.c_str()) > displayWidth - 10) {
            int visibleLength = 0;
            for (int i = cursorPos - 1; i >= 0; i--) {
                visibleLength++;
                String subText = currentText.substring(i, cursorPos + 1);
                if (getStringWidth(subText.c_str()) > displayWidth - 20) {
                    currentText = "..." + currentText.substring(i + 1);
                    break;
                }
            }
        }

        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, 18, currentText);

        if (millis() % 1000 < 500) {
            int cursorX = (displayWidth / 2) - (getStringWidth(currentText.c_str()) / 2);
            for (int i = 0; i < cursorPos; i++) {
                if (i < currentText.length()) {
                    cursorX += getStringWidth(String(currentText[i]).c_str());
                }
            }
            drawLine(cursorX, 18, cursorX, 26);
        }

        if (modeSelectChar) {
            setTextAlignment(TEXT_ALIGN_CENTER);

            char prevChar = charset[(currentChar - 1 + charsetLength) % charsetLength];
            char currChar = charset[currentChar];
            char nextChar = charset[(currentChar + 1) % charsetLength];

            drawString(displayWidth / 2, 30, "^");

            String charStr = String(currChar);
            fillRect(displayWidth / 2 - 6, 36, 12, 12);
            setColor(BLACK);
            drawString(displayWidth / 2, 38, charStr);
            setColor(WHITE);

            drawString(displayWidth / 2, 50, "v");

            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(displayWidth / 2, 56, "Select: pilih");
        } else {
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(displayWidth / 2, 38, "Select: tambah");
            drawString(displayWidth / 2, 48, "Back: selesai");
        }

        display();

        if (modeSelectChar) {
            if (cursor_->up) {
                currentChar = (currentChar - 1 + charsetLength) % charsetLength;
                cursor_->up = false;
            }

            if (cursor_->down) {
                currentChar = (currentChar + 1) % charsetLength;
                cursor_->down = false;
            }

            if (cursor_->select) {
                modeSelectChar = false;
                cursor_->select = false;
            }
        } else {
            if (cursor_->up || cursor_->down) {
                modeSelectChar = true;
                cursor_->up = false;
                cursor_->down = false;
            }

            if (cursor_->select && cursorPos < maxLength - 1) {
                for (int i = strlen(buffer) + 1; i > cursorPos; i--) {
                    buffer[i] = buffer[i - 1];
                }
                buffer[cursorPos] = charset[currentChar];
                cursorPos++;
                modeSelectChar = true;
                cursor_->select = false;
            }
        }

        if (cursor_->back) {
            if (modeSelectChar) {
                exiting = true;
            } else {
                modeSelectChar = true;
            }
            cursor_->back = false;
        }

        delay(50);
    }
}

void SH1106Menu::flipVertical(bool flip) {
    if (flip) {
        flipScreenVertically();
    } else {
        resetOrientation();
    }
}

void SH1106Menu::flipHorizontal(bool flip) {
    if (flip) {
        mirrorScreen();
    } else {
        resetOrientation();
    }
}

void SH1106Menu::setFlip(bool horizontalFlip, bool verticalFlip) {
    resetOrientation();
    if (horizontalFlip && verticalFlip) {
        flipScreenVertically();
        mirrorScreen();
    } else if (horizontalFlip) {
        mirrorScreen();
    } else if (verticalFlip) {
        flipScreenVertically();
    }
}