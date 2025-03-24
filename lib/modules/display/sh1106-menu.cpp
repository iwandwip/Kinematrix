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

void SH1106Menu::freeMenu(MenuProperties *menuProperties) {
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

void SH1106Menu::renderInfoScreen(const char *title, const char *line1, const char *line2, const char *line3) {
    clear();

    setColor(WHITE);
    fillRect(0, 0, 128, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(64, 3, title);

    setColor(WHITE);
    drawRect(0, 15, 128, 49);

    setTextAlignment(TEXT_ALIGN_LEFT);
    drawString(8, 20, line1);
    drawString(8, 32, line2);
    drawString(8, 44, line3);
    display();
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

        // Draw header
        setColor(WHITE);
        fillRect(0, 0, displayWidth, 15);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, 1, title);

        setColor(WHITE);
        setTextAlignment(TEXT_ALIGN_CENTER);

        // Draw a message with position adjustment
        drawString(displayWidth / 2, 20, message);

        // Horizontal layout for buttons to fit better on screen
        int btnWidth = 40;
        int btnHeight = 15;
        int btnGap = 10;
        int yesX = (displayWidth / 2) - btnWidth - (btnGap / 2);
        int noX = (displayWidth / 2) + (btnGap / 2);
        int btnY = 38;  // Move buttons higher up

        // Draw Yes button
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

        // Draw No button
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

        // Draw header
        setColor(WHITE);
        fillRect(0, 0, displayWidth, 15);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, 1, title);

        setColor(WHITE);

        // Display current text
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

        // Show text cursor at current position
        if (millis() % 1000 < 500) {
            int cursorX = (displayWidth / 2) - (getStringWidth(currentText.c_str()) / 2);
            for (int i = 0; i < cursorPos; i++) {
                if (i < currentText.length()) {
                    cursorX += getStringWidth(String(currentText[i]).c_str());
                }
            }
            drawLine(cursorX, 18, cursorX, 26);
        }

        // Character selection UI
        if (modeSelectChar) {
            setTextAlignment(TEXT_ALIGN_CENTER);

            char prevChar = charset[(currentChar - 1 + charsetLength) % charsetLength];
            char currChar = charset[currentChar];
            char nextChar = charset[(currentChar + 1) % charsetLength];

            // Display current character with up/down arrows
            drawString(displayWidth / 2, 30, "^");

            // Make the selected character more prominent
            String charStr = String(currChar);
            fillRect(displayWidth / 2 - 6, 36, 12, 12);
            setColor(BLACK);
            drawString(displayWidth / 2, 38, charStr);
            setColor(WHITE);

            drawString(displayWidth / 2, 50, "v");

            // Display instructions at the bottom
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(displayWidth / 2, 56, "Select: pilih");
        } else {
            // Confirmation mode UI
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
                // Insert character at cursor position
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

void SH1106Menu::renderLargeText(const char *text, int fontSize, bool withBox) {
    clear();
    if (fontSize == 24) {
        setFont(ArialMT_Plain_24);
    } else if (fontSize == 16) {
        setFont(ArialMT_Plain_16);
    } else {
        setFont(ArialMT_Plain_10);
    }
    setTextAlignment(TEXT_ALIGN_CENTER);
    int textWidth = getStringWidth(text);
    int yPos = (displayHeight - fontSize) / 2;
    if (withBox) {
        int boxPadding = 6;
        int boxWidth = textWidth + (2 * boxPadding);
        int boxHeight = fontSize + (2 * boxPadding);
        int boxX = (displayWidth - boxWidth) / 2;
        int boxY = yPos - boxPadding;
        drawRect(boxX, boxY, boxWidth, boxHeight);
    }
    drawString(displayWidth / 2, yPos, text);
    display();
}

void SH1106Menu::renderSplashScreen(const char *title, const char *subtitle, const uint8_t *logo) {
    clear();
    int yPos = 10;
    if (logo != nullptr) {
        int logoWidth = 32;
        int logoHeight = 32;
        int logoX = (displayWidth - logoWidth) / 2;
        drawFastImage(logoX, yPos, logoWidth, logoHeight, logo);
        yPos += logoHeight + 5;
    }
    setFont(ArialMT_Plain_16);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, yPos, title);

    if (subtitle != nullptr) {
        setFont(ArialMT_Plain_10);
        drawString(displayWidth / 2, yPos + 18, subtitle);
    }
    display();
}

void SH1106Menu::renderStatusScreen(const char *title, const char *status, bool isOk) {
    clear();
    setColor(WHITE);
    fillRect(0, 0, displayWidth, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 2, title);

    setColor(WHITE);
    int centerX = displayWidth / 2;
    int centerY = 30;
    int radius = 10;

    if (isOk) {
        drawCircle(centerX, centerY, radius);
        drawLine(centerX - 5, centerY, centerX - 2, centerY + 3);
        drawLine(centerX - 2, centerY + 3, centerX + 5, centerY - 4);
    } else {
        drawCircle(centerX, centerY, radius);
        drawLine(centerX - 5, centerY - 5, centerX + 5, centerY + 5);
        drawLine(centerX + 5, centerY - 5, centerX - 5, centerY + 5);
    }

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(centerX, centerY + 15, status);
    display();
}

void SH1106Menu::renderDualValueScreen(const char *title, const char *label1, const char *value1, const char *label2, const char *value2) {
    clear();
    setColor(WHITE);
    fillRect(0, 0, displayWidth, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 2, title);

    setColor(WHITE);
    setTextAlignment(TEXT_ALIGN_LEFT);

    int yPos1 = 20;
    drawString(5, yPos1, label1);
    setFont(ArialMT_Plain_16);
    drawString(5, yPos1 + 12, value1);

    int yPos2 = 40;
    setFont(ArialMT_Plain_10);
    drawString(5, yPos2, label2);
    setFont(ArialMT_Plain_16);
    drawString(5, yPos2 + 12, value2);

    setFont(ArialMT_Plain_10);
    display();
}

void SH1106Menu::renderCountdownScreen(const char *title, int seconds, bool showProgress) {
    clear();
    setColor(WHITE);
    fillRect(0, 0, displayWidth, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 2, title);

    setColor(WHITE);

    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    char timeString[10];
    sprintf(timeString, "%02d:%02d", minutes, remainingSeconds);

    setFont(ArialMT_Plain_24);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 20, timeString);

    if (showProgress) {
        int barWidth = 100;
        int barHeight = 6;
        int barX = (displayWidth - barWidth) / 2;
        int barY = 52;

        drawRect(barX, barY, barWidth, barHeight);

        // Calculate progress (assuming 60 seconds = full bar)
        int maxSeconds = 60;  // Change this to match your use case
        int progress = map(seconds, 0, maxSeconds, 0, barWidth);
        if (progress > 0) {
            fillRect(barX, barY, progress, barHeight);
        }
    }

    setFont(ArialMT_Plain_10);  // Reset font
    display();
}

void SH1106Menu::renderIconTextRow(int y, const uint8_t *icon, const char *text, int iconWidth, int iconHeight) {
    if (icon != nullptr) {
        drawFastImage(5, y, iconWidth, iconHeight, icon);
        drawString(10 + iconWidth, y + (iconHeight - 10) / 2, text);
    } else {
        drawString(5, y, text);
    }
}

void SH1106Menu::renderCenteredText(const char *text1, const char *text2, const char *text3) {
    clear();

    int yPos = displayHeight / 2;
    int spacing = 12;
    int totalHeight = 0;

    if (text1) totalHeight += spacing;
    if (text2) totalHeight += spacing;
    if (text3) totalHeight += spacing;

    yPos = (displayHeight - totalHeight) / 2;
    setTextAlignment(TEXT_ALIGN_CENTER);

    if (text1) {
        drawString(displayWidth / 2, yPos, text1);
        yPos += spacing;
    }
    if (text2) {
        drawString(displayWidth / 2, yPos, text2);
        yPos += spacing;
    }
    if (text3) {
        drawString(displayWidth / 2, yPos, text3);
    }
    display();
}

void SH1106Menu::renderMetricScreen(const char *title, const char *value, const char *unit, const char *subtitle) {
    clear();

    setColor(WHITE);
    fillRect(0, 0, displayWidth, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 2, title);

    setColor(WHITE);
    setFont(ArialMT_Plain_24);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 20, value);

    setFont(ArialMT_Plain_10);
    drawString(displayWidth / 2, 46, unit);

    if (subtitle != nullptr) {
        drawString(displayWidth / 2, 56, subtitle);
    }
    display();
}

void SH1106Menu::renderBoxedText(const char *lines[], int numLines) {
    clear();
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawRect(0, 0, displayWidth, displayHeight);

    int centerX = displayWidth / 2;
    int totalHeight = 0;
    int lineHeight = 0;
    int topMargin = 4; // Margin from top of screen
    switch (numLines) {
        case 1:
            setFont(ArialMT_Plain_24);
            lineHeight = 24;
            totalHeight = lineHeight;
            break;
        case 2:
            setFont(ArialMT_Plain_16);
            lineHeight = 16;
            totalHeight = lineHeight * 2;
            break;
        case 3:
            setFont(ArialMT_Plain_10);
            lineHeight = 12;
            totalHeight = lineHeight * 3;
            break;
        case 4:
            setFont(ArialMT_Plain_10);
            lineHeight = 10;
            totalHeight = lineHeight * 4;
            break;
        default:
            setFont(ArialMT_Plain_10);
            lineHeight = 10;
            totalHeight = lineHeight * numLines;
    }
    int startY = topMargin + ((displayHeight - 2 * topMargin - totalHeight) / 2);
    for (int i = 0; i < numLines && i < 4; i++) {
        if (lines[i] != nullptr) {
            int yPos = startY + (i * lineHeight);
            drawString(centerX, yPos, lines[i]);
        }
    }
    setFont(ArialMT_Plain_10);
    display();
}

void SH1106Menu::renderBatteryStatus(int percentage, bool charging) {
    clear();
    setTextAlignment(TEXT_ALIGN_CENTER);

    // Ensure percentage is between 0-100
    percentage = constrain(percentage, 0, 100);

    // Draw battery outline
    int batteryWidth = 40;
    int batteryHeight = 20;
    int batteryX = (displayWidth - batteryWidth) / 2;
    int batteryY = 10;

    // Battery body
    drawRect(batteryX, batteryY, batteryWidth, batteryHeight);

    // Battery terminal
    int terminalWidth = 4;
    int terminalHeight = 8;
    drawRect(batteryX + batteryWidth, batteryY + (batteryHeight - terminalHeight) / 2,
             terminalWidth, terminalHeight);

    // Battery fill level
    int fillWidth = map(percentage, 0, 100, 0, batteryWidth - 4);
    if (fillWidth > 0) {
        fillRect(batteryX + 2, batteryY + 2, fillWidth, batteryHeight - 4);
    }

    // Charging symbol if charging
    if (charging) {
        setColor(BLACK);
        drawLine(batteryX + batteryWidth / 2 - 3, batteryY + batteryHeight / 2,
                 batteryX + batteryWidth / 2 - 6, batteryY + batteryHeight / 2 + 5);
        drawLine(batteryX + batteryWidth / 2 - 3, batteryY + batteryHeight / 2,
                 batteryX + batteryWidth / 2 + 3, batteryY + batteryHeight / 2);
        drawLine(batteryX + batteryWidth / 2 + 3, batteryY + batteryHeight / 2,
                 batteryX + batteryWidth / 2, batteryY + batteryHeight / 2 - 5);
        setColor(WHITE);
    }

    // Draw percentage text
    String percentText = String(percentage) + "%";
    drawString(displayWidth / 2, batteryY + batteryHeight + 5, percentText);

    // Draw charging text if needed
    if (charging) {
        drawString(displayWidth / 2, batteryY + batteryHeight + 20, "Charging");
    }

    display();
}

void SH1106Menu::renderSignalStrength(int strength, const char *networkName) {
    clear();

    // Ensure strength is between 0-4
    strength = constrain(strength, 0, 4);

    int barWidth = 6;
    int barGap = 2;
    int maxBarHeight = 20;
    int baseX = (displayWidth - (barWidth * 4 + barGap * 3)) / 2;
    int baseY = 20;

    // Draw signal bars
    for (int i = 0; i < 4; i++) {
        int barHeight = map(i + 1, 1, 4, 5, maxBarHeight);

        if (i < strength) {
            // Filled bar for active signal level
            fillRect(baseX + i * (barWidth + barGap),
                     baseY + maxBarHeight - barHeight,
                     barWidth, barHeight);
        } else {
            // Empty bar for inactive signal level
            drawRect(baseX + i * (barWidth + barGap),
                     baseY + maxBarHeight - barHeight,
                     barWidth, barHeight);
        }
    }

    // Draw signal text
    String signalText;
    if (strength == 0) {
        signalText = "No Signal";
    } else if (strength == 1) {
        signalText = "Weak";
    } else if (strength == 2) {
        signalText = "Fair";
    } else if (strength == 3) {
        signalText = "Good";
    } else {
        signalText = "Excellent";
    }

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, baseY + maxBarHeight + 5, signalText);

    // Draw network name if provided
    if (networkName != nullptr) {
        drawString(displayWidth / 2, baseY + maxBarHeight + 20, networkName);
    }

    display();
}

void SH1106Menu::renderClock(int hour, int minute, int second, bool isAnalog) {
    clear();

    if (isAnalog) {
        // Draw an analog clock
        int centerX = displayWidth / 2;
        int centerY = displayHeight / 2;
        int radius = min(displayWidth, displayHeight) / 2 - 10;

        // Draw clock face
        drawCircle(centerX, centerY, radius);

        // Draw hour markers
        for (int i = 0; i < 12; i++) {
            float angle = i * 30 * 3.14159 / 180;
            int markerStartX = centerX + (radius - 4) * sin(angle);
            int markerStartY = centerY - (radius - 4) * cos(angle);
            int markerEndX = centerX + radius * sin(angle);
            int markerEndY = centerY - radius * cos(angle);

            drawLine(markerStartX, markerStartY, markerEndX, markerEndY);
        }

        // Calculate angles for hands
        float hourAngle = ((hour % 12) * 30 + minute / 2) * 3.14159 / 180;
        float minuteAngle = minute * 6 * 3.14159 / 180;
        float secondAngle = second * 6 * 3.14159 / 180;

        // Draw hour hand
        int hourHandLength = radius * 0.5;
        int hourHandX = centerX + hourHandLength * sin(hourAngle);
        int hourHandY = centerY - hourHandLength * cos(hourAngle);
        drawLine(centerX, centerY, hourHandX, hourHandY);

        // Draw minute hand
        int minuteHandLength = radius * 0.7;
        int minuteHandX = centerX + minuteHandLength * sin(minuteAngle);
        int minuteHandY = centerY - minuteHandLength * cos(minuteAngle);
        drawLine(centerX, centerY, minuteHandX, minuteHandY);

        // Draw second hand if seconds specified
        if (second >= 0) {
            int secondHandLength = radius * 0.8;
            int secondHandX = centerX + secondHandLength * sin(secondAngle);
            int secondHandY = centerY - secondHandLength * cos(secondAngle);
            drawLine(centerX, centerY, secondHandX, secondHandY);
        }

        // Draw center dot
        fillCircle(centerX, centerY, 2);
    } else {
        // Draw a digital clock
        setTextAlignment(TEXT_ALIGN_CENTER);

        // Format time string
        char timeStr[9];
        if (second >= 0) {
            sprintf(timeStr, "%02d:%02d:%02d", hour, minute, second);
        } else {
            sprintf(timeStr, "%02d:%02d", hour, minute);
        }

        // Draw time with larger font
        setFont(ArialMT_Plain_24);
        drawString(displayWidth / 2, (displayHeight - 24) / 2, timeStr);

        // Reset font
        setFont(ArialMT_Plain_10);
    }

    display();
}

void SH1106Menu::renderPercentageCircle(int percentage, const char *text) {
    clear();

    // Ensure percentage is between 0-100
    percentage = constrain(percentage, 0, 100);

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2;
    int radius = min(displayWidth, displayHeight) / 2 - 10;

    // Draw empty circle
    drawCircle(centerX, centerY, radius);

    // Draw filled arc for percentage
    float startAngle = -90 * 3.14159 / 180; // Start at 12 o'clock positions
    float endAngle = (percentage * 3.6 - 90) * 3.14159 / 180;

    if (percentage > 0) {
        for (int i = 0; i < radius; i++) {
            float r = i;
            for (float angle = startAngle; angle <= endAngle; angle += 0.05) {
                int x = centerX + r * cos(angle);
                int y = centerY + r * sin(angle);
                setPixel(x, y);
            }
        }
    }

    // Draw percentage text in a center
    setTextAlignment(TEXT_ALIGN_CENTER);

    String percentText = String(percentage) + "%";

    if (text != nullptr) {
        // If an additional text is provided, show percentage larger
        setFont(ArialMT_Plain_16);
        drawString(centerX, centerY - 12, percentText);

        setFont(ArialMT_Plain_10);
        drawString(centerX, centerY + 8, text);
    } else {
        // Just show percentage big
        setFont(ArialMT_Plain_24);
        drawString(centerX, centerY - 12, percentText);
    }

    // Reset font
    setFont(ArialMT_Plain_10);

    display();
}

void SH1106Menu::renderScrollingText(const char *text, int speed, int scrollCount) {
    int textWidth = getStringWidth(text);

    // Only scroll if a text is wider than the screen
    if (textWidth <= displayWidth) {
        // Just center the text if it fits
        clear();
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, (displayHeight - 10) / 2, text);
        display();
        delay(speed * 10); // Show static text for a while
        return;
    }

    // Calculate total travel distance
    int totalScroll = displayWidth + textWidth;

    setTextAlignment(TEXT_ALIGN_LEFT);

    for (int count = 0; count < scrollCount; count++) {
        // Scroll text from right to left
        for (int offset = displayWidth; offset > -textWidth; offset -= 2) {
            clear();
            drawString(offset, (displayHeight - 10) / 2, text);
            display();
            delay(speed);
        }

        // Pause at the end of each scroll
        delay(speed * 5);
    }
}

void SH1106Menu::renderAnimatedLoading(int frame) {
    clear();

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2;
    int radius = 15;

    // Draw loading text
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(centerX, centerY - 25, "Loading...");

    // Normalize frame to 0-7 range for 8 animation frames
    frame = frame % 8;

    // Draw segments with different intensities based on frame
    for (int i = 0; i < 8; i++) {
        // Calculate an angle for this segment
        float startAngle = i * 45 * 3.14159 / 180;

        // Calculate segment brightness based on distance from the current frame
        int distance = (i - frame + 8) % 8;
        bool isActive = (distance < 3); // Active segments are the current and next 2

        // Draw segment
        int x1 = centerX + radius * cos(startAngle);
        int y1 = centerY + radius * sin(startAngle);
        int x2 = centerX + (radius - 5) * cos(startAngle);
        int y2 = centerY + (radius - 5) * sin(startAngle);

        if (isActive) {
            // Active segment - draw thicker
            drawLine(x1, y1, x2, y2);
            // Draw adjacent pixels for thickness
            drawLine(x1 + 1, y1, x2 + 1, y2);
            drawLine(x1, y1 + 1, x2, y2 + 1);
        } else {
            // Inactive segment - draw thinner
            drawLine(x1, y1, x2, y2);
        }
    }

    display();
}

void SH1106Menu::renderToggleSwitch(const char *label, bool state) {
    clear();

    // Draw label
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 10, label);

    // Draw toggle switch
    int switchWidth = 30;
    int switchHeight = 15;
    int switchX = (displayWidth - switchWidth) / 2;
    int switchY = 30;

    // Draw a switch background
    drawRoundedRect(switchX, switchY, switchWidth, switchHeight, switchHeight / 2);

    // Draw switch state
    int knobSize = switchHeight - 4;
    int knobX = state ? (switchX + switchWidth - knobSize - 2) : (switchX + 2);

    // Fill background based on state
    if (state) {
        // Fill a switch background when on
        fillRoundedRect(switchX, switchY, switchWidth, switchHeight, switchHeight / 2);

        // Draw knob (contrasting color)
        setColor(BLACK);
        fillCircle(knobX + knobSize / 2, switchY + switchHeight / 2, knobSize / 2);
        setColor(WHITE);
    } else {
        // Just draw the knob for off state
        fillCircle(knobX + knobSize / 2, switchY + switchHeight / 2, knobSize / 2);
    }

    // Draw state text
    drawString(displayWidth / 2, switchY + switchHeight + 10, state ? "ON" : "OFF");

    display();
}

void SH1106Menu::renderNotification(const char *message, int timeMs) {
    // Store current display buffer to restore after notification
    uint8_t *tempBuffer = new uint8_t[displayBufferSize];
    memcpy(tempBuffer, buffer, displayBufferSize);

    // Prepare notification
    int notificationHeight = 20;
    int notificationY = 0;

    // Animation - slide in
    for (int i = -notificationHeight; i <= 0; i += 2) {
        clear();

        // Restore original content below notification
        memcpy(buffer, tempBuffer, displayBufferSize);

        // Draw notification box
        fillRect(0, i, displayWidth, notificationHeight);

        // Draw notification text
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, i + 5, message);
        setColor(WHITE);

        display();
        delay(10);
    }

    // Wait for specified duration
    delay(timeMs);

    // Animation - slide out
    for (int i = 0; i >= -notificationHeight; i -= 2) {
        clear();

        // Restore original content below notification
        memcpy(buffer, tempBuffer, displayBufferSize);

        // Draw notification box
        fillRect(0, i, displayWidth, notificationHeight);

        // Draw notification text
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, i + 5, message);
        setColor(WHITE);

        display();
        delay(10);
    }

    // Restore original display
    memcpy(buffer, tempBuffer, displayBufferSize);
    display();

    delete[] tempBuffer;
}

// Helper function for rounded rectangles (for toggle switch)
void SH1106Menu::drawRoundedRect(int x, int y, int width, int height, int radius) {
    // Draw straight edges
    drawLine(x + radius, y, x + width - radius, y);
    drawLine(x + radius, y + height, x + width - radius, y + height);
    drawLine(x, y + radius, x, y + height - radius);
    drawLine(x + width, y + radius, x + width, y + height - radius);

    // Draw rounded corners
    drawCircleQuads(x + radius, y + radius, radius, 1);
    drawCircleQuads(x + width - radius, y + radius, radius, 2);
    drawCircleQuads(x + width - radius, y + height - radius, radius, 4);
    drawCircleQuads(x + radius, y + height - radius, radius, 8);
}

void SH1106Menu::fillRoundedRect(int x, int y, int width, int height, int radius) {
    // Fill the middle
    fillRect(x + radius, y, width - 2 * radius, height);
    fillRect(x, y + radius, width, height - 2 * radius);

    // Fill rounded corners using fillCircle instead
    fillCircle(x + radius, y + radius, radius);
    fillCircle(x + width - radius, y + radius, radius);
    fillCircle(x + width - radius, y + height - radius, radius);
    fillCircle(x + radius, y + height - radius, radius);
}