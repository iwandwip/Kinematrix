#include "sh1106-menu.h"

void SH1106Menu::renderDropdownMenu(const char *title, const char *options[], int numOptions, int selectedOption) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int menuWidth = displayWidth - 10;
    int menuX = 5;
    int menuY = 15;

    drawRect(menuX, menuY, menuWidth, 18);

    int arrowX = menuX + menuWidth - 10;
    int arrowY = menuY + 9;

    drawTriangle(arrowX - 5, arrowY - 3, arrowX + 5, arrowY - 3, arrowX, arrowY + 3);

    setTextAlignment(TEXT_ALIGN_LEFT);
    drawString(menuX + 5, menuY + 5, options[selectedOption]);

    if (numOptions > 0) {
        int dropdownHeight = min(numOptions, 3) * 14;
        fillRect(menuX, menuY + 18, menuWidth, dropdownHeight);

        setColor(BLACK);
        for (int i = 0; i < min(numOptions, 3); i++) {
            int optionY = menuY + 18 + i * 14;
            drawString(menuX + 5, optionY + 3, options[i]);
        }

        setColor(WHITE);
        drawRect(menuX, menuY + 18, menuWidth, dropdownHeight);
    }

    display();
}

void SH1106Menu::renderListMenu(const char *title, const char *options[], int numOptions, int selectedOption, int startIndex) {
    clear();

    int headerHeight = 12;

    setColor(WHITE);
    fillRect(0, 0, displayWidth, headerHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);
    setColor(WHITE);

    int visibleOptions = min(4, numOptions);
    int endIndex = min(startIndex + visibleOptions, numOptions);

    for (int i = startIndex; i < endIndex; i++) {
        int y = headerHeight + (i - startIndex) * 13;

        if (i == selectedOption) {
            fillRect(0, y, displayWidth, 13);
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(5, y + 2, options[i]);
            setColor(WHITE);
        } else {
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(5, y + 2, options[i]);
        }
    }

    if (startIndex > 0) {
        fillTriangle(displayWidth - 8, headerHeight + 3, displayWidth - 4, headerHeight + 3, displayWidth - 6, headerHeight);
    }

    if (endIndex < numOptions) {
        fillTriangle(displayWidth - 8, displayHeight - 3, displayWidth - 4, displayHeight - 3, displayWidth - 6, displayHeight);
    }

    display();
}

void SH1106Menu::renderCheckboxMenu(const char *title, const char *options[], bool checked[], int numOptions, int selectedOption) {
    clear();

    int headerHeight = 12;

    setColor(WHITE);
    fillRect(0, 0, displayWidth, headerHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);
    setColor(WHITE);

    int visibleOptions = min(4, numOptions);

    for (int i = 0; i < visibleOptions; i++) {
        int y = headerHeight + i * 13;

        if (i == selectedOption) {
            fillRect(0, y, displayWidth, 13);
            setColor(BLACK);
        }

        drawRect(5, y + 2, 9, 9);

        if (checked[i]) {
            if (i == selectedOption) {
                setColor(WHITE);
                drawLine(7, y + 7, 9, y + 9);
                drawLine(9, y + 9, 12, y + 4);
                setColor(BLACK);
            } else {
                drawLine(7, y + 7, 9, y + 9);
                drawLine(9, y + 9, 12, y + 4);
            }
        }

        setTextAlignment(TEXT_ALIGN_LEFT);
        drawString(20, y + 2, options[i]);

        if (i == selectedOption) {
            setColor(WHITE);
        }
    }

    display();
}

void SH1106Menu::renderRadioMenu(const char *title, const char *options[], int numOptions, int selectedOption) {
    clear();

    int headerHeight = 14;
    int itemHeight = 15;
    int itemPadding = 2;
    int radioRadius = 4;
    int innerRadius = 2;
    int textLeftMargin = 22;
    int radioX = 10;

    setColor(WHITE);
    fillRect(0, 0, displayWidth, headerHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, (headerHeight - 10) / 2, title);

    setColor(WHITE);

    int visibleOptions = min(4, numOptions);
    int menuHeight = visibleOptions * itemHeight;
    int menuStartY = headerHeight + itemPadding;

    for (int i = 0; i < visibleOptions; i++) {
        int itemY = menuStartY + i * itemHeight;
        int radioY = itemY + itemHeight / 2;

        if (i == selectedOption) {
            fillRect(0, itemY, displayWidth, itemHeight);
            setColor(BLACK);
        } else {
            setColor(WHITE);
        }

        drawCircle(radioX, radioY, radioRadius);

        if (i == selectedOption) {
            fillCircle(radioX, radioY, innerRadius);
        }

        setTextAlignment(TEXT_ALIGN_LEFT);
        drawString(textLeftMargin, itemY + (itemHeight - 10) / 2, options[i]);  // Vertical center text

        if (i == selectedOption) {
            setColor(WHITE);
        }
    }

    display();
}

void SH1106Menu::renderSelectionWheel(const char *title, const char *options[], int numOptions, int selectedOption) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int wheelCenterY = 35;
    int wheelHeight = 40;

    drawLine(10, wheelCenterY - wheelHeight / 2, displayWidth - 10, wheelCenterY - wheelHeight / 2);
    drawLine(10, wheelCenterY + wheelHeight / 2, displayWidth - 10, wheelCenterY + wheelHeight / 2);

    int visibleOptions = 3;

    for (int i = -1; i <= 1; i++) {
        int idx = (selectedOption + i + numOptions) % numOptions;
        int y = wheelCenterY + i * 15;

        if (i == 0) {
            setFont(ArialMT_Plain_16);
            fillRect(0, y - 9, displayWidth, 18);
            setColor(BLACK);
            drawString(displayWidth / 2, y - 8, options[idx]);
            setColor(WHITE);
        } else {
            setFont(ArialMT_Plain_10);
            drawString(displayWidth / 2, y - 5, options[idx]);
        }
    }

    setFont(ArialMT_Plain_10);

    fillTriangle(5, wheelCenterY, 10, wheelCenterY - 5, 10, wheelCenterY + 5);
    fillTriangle(displayWidth - 5, wheelCenterY, displayWidth - 10, wheelCenterY - 5, displayWidth - 10, wheelCenterY + 5);

    display();
}

void SH1106Menu::renderMenuTiles(const char *titles[], const uint8_t *icons[], int numTiles, int columns, int selectedTile) {
    clear();

    int rows = (numTiles + columns - 1) / columns;
    int tileWidth = displayWidth / columns;
    int tileHeight = displayHeight / rows;

    for (int i = 0; i < numTiles; i++) {
        int row = i / columns;
        int col = i % columns;

        int x = col * tileWidth;
        int y = row * tileHeight;

        if (i == selectedTile) {
            drawRect(x, y, tileWidth, tileHeight);
        }

        if (icons != nullptr && icons[i] != nullptr) {
            int iconSize = min(tileWidth, tileHeight) - 10;
            drawFastImage(x + (tileWidth - iconSize) / 2, y + 2, iconSize, iconSize, icons[i]);
        }

        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(x + tileWidth / 2, y + tileHeight - 10, titles[i]);
    }

    display();
}

void SH1106Menu::renderHorizontalMenu(const char *options[], int numOptions, int selectedOption) {
    clear();

    int menuY = (displayHeight - 20) / 2;
    int itemWidth = min(displayWidth / min(numOptions, 5), 30);
    int visibleOptions = min(numOptions, 5);
    int menuWidth = itemWidth * visibleOptions;
    int menuX = (displayWidth - menuWidth) / 2;

    for (int i = 0; i < visibleOptions; i++) {
        int x = menuX + i * itemWidth;

        if (i == selectedOption % visibleOptions) {
            fillRect(x, menuY, itemWidth, 20);
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(x + itemWidth / 2, menuY + 6, options[(selectedOption / visibleOptions) * visibleOptions + i]);
            setColor(WHITE);
        } else {
            drawRect(x, menuY, itemWidth, 20);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(x + itemWidth / 2, menuY + 6, options[(selectedOption / visibleOptions) * visibleOptions + i]);
        }
    }

    if (selectedOption >= visibleOptions) {
        fillTriangle(menuX - 10, menuY + 10, menuX - 5, menuY + 5, menuX - 5, menuY + 15);
    }

    if (selectedOption / visibleOptions < numOptions / visibleOptions) {
        fillTriangle(menuX + menuWidth + 10, menuY + 10, menuX + menuWidth + 5, menuY + 5, menuX + menuWidth + 5, menuY + 15);
    }

    display();
}

void SH1106Menu::renderVerticalMenu(const char *options[], int numOptions, int selectedOption, int startIndex) {
    clear();

    int menuWidth = displayWidth - 20;
    int menuX = 10;
    int menuY = 10;
    int itemHeight = 12;
    int visibleOptions = min(numOptions, 4);
    int endIndex = min(startIndex + visibleOptions, numOptions);

    for (int i = startIndex; i < endIndex; i++) {
        int y = menuY + (i - startIndex) * itemHeight;

        if (i == selectedOption) {
            fillRect(menuX, y, menuWidth, itemHeight);
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(menuX + 5, y + 2, options[i]);
            setColor(WHITE);
        } else {
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(menuX + 5, y + 2, options[i]);
        }
    }

    if (startIndex > 0) {
        fillTriangle(menuX + menuWidth / 2, menuY - 5,
                     menuX + menuWidth / 2 - 5, menuY - 2,
                     menuX + menuWidth / 2 + 5, menuY - 2);
    }

    if (endIndex < numOptions) {
        fillTriangle(menuX + menuWidth / 2, menuY + visibleOptions * itemHeight + 5,
                     menuX + menuWidth / 2 - 5, menuY + visibleOptions * itemHeight + 2,
                     menuX + menuWidth / 2 + 5, menuY + visibleOptions * itemHeight + 2);
    }

    display();
}

void SH1106Menu::renderIconMenu(const char *options[], const uint8_t *icons[], int numOptions, int selectedOption) {
    clear();

    int iconSize = 24;
    int itemWidth = 42;
    int itemHeight = 36;
    int visibleCols = 3;
    int visibleRows = 2;
    int menuWidth = itemWidth * visibleCols;
    int menuHeight = itemHeight * visibleRows;
    int menuX = (displayWidth - menuWidth) / 2;
    int menuY = (displayHeight - menuHeight) / 2;

    for (int i = 0; i < min(numOptions, visibleCols * visibleRows); i++) {
        int row = i / visibleCols;
        int col = i % visibleCols;

        int x = menuX + col * itemWidth;
        int y = menuY + row * itemHeight;

        if (i == selectedOption) {
            drawRect(x, y, itemWidth, itemHeight);
        }

        if (icons != nullptr && icons[i] != nullptr) {
            drawFastImage(x + (itemWidth - iconSize) / 2, y + 2, iconSize, iconSize, icons[i]);
        }

        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(x + itemWidth / 2, y + itemHeight - 10, options[i]);
    }

    display();
}

void SH1106Menu::renderNestedMenu(const char *title, const char *items[], bool hasSubmenu[], int numItems, int selectedItem) {
    clear();

    int headerHeight = 12;

    setColor(WHITE);
    fillRect(0, 0, displayWidth, headerHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);
    setColor(WHITE);

    int visibleItems = min(4, numItems);

    for (int i = 0; i < visibleItems; i++) {
        int y = headerHeight + i * 13;

        if (i == selectedItem) {
            fillRect(0, y, displayWidth, 13);
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(5, y + 2, items[i]);

            if (hasSubmenu[i]) {
                drawTriangle(displayWidth - 8, y + 6, displayWidth - 4, y + 6, displayWidth - 6, y + 9);
            }

            setColor(WHITE);
        } else {
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(5, y + 2, items[i]);

            if (hasSubmenu[i]) {
                drawTriangle(displayWidth - 8, y + 6, displayWidth - 4, y + 6, displayWidth - 6, y + 9);
            }
        }
    }

    display();
}

void SH1106Menu::renderContextMenu(const char *options[], int numOptions, int x, int y, int selectedOption) {
    int maxWidth = 0;
    for (int i = 0; i < numOptions; i++) {
        int width = getStringWidth(options[i]);
        if (width > maxWidth) {
            maxWidth = width;
        }
    }

    int menuWidth = maxWidth + 20;
    int menuHeight = numOptions * 12;

    if (x + menuWidth > displayWidth) {
        x = displayWidth - menuWidth;
    }

    if (y + menuHeight > displayHeight) {
        y = displayHeight - menuHeight;
    }

    fillRect(x, y, menuWidth, menuHeight);
    drawRect(x, y, menuWidth, menuHeight);

    for (int i = 0; i < numOptions; i++) {
        int itemY = y + i * 12;

        if (i == selectedOption) {
            setColor(BLACK);
            fillRect(x + 1, itemY, menuWidth - 2, 12);
            setColor(WHITE);
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(x + 5, itemY + 2, options[i]);
            setColor(BLACK);
        } else {
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(x + 5, itemY + 2, options[i]);
        }
    }

    setColor(WHITE);
    display();
}

void SH1106Menu::renderSettingsMenu(const char *title, const char *settings[], const char *values[], int numSettings, int selectedSetting) {
    clear();

    int headerHeight = 12;

    setColor(WHITE);
    fillRect(0, 0, displayWidth, headerHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);
    setColor(WHITE);

    int visibleSettings = min(4, numSettings);

    for (int i = 0; i < visibleSettings; i++) {
        int y = headerHeight + i * 13;

        if (i == selectedSetting) {
            fillRect(0, y, displayWidth, 13);
            setColor(BLACK);
        }

        setTextAlignment(TEXT_ALIGN_LEFT);
        drawString(5, y + 2, settings[i]);

        setTextAlignment(TEXT_ALIGN_RIGHT);
        drawString(displayWidth - 5, y + 2, values[i]);

        if (i == selectedSetting) {
            setColor(WHITE);
        }
    }

    display();
}

void SH1106Menu::renderNumericInput(const char *title, int value, int minValue, int maxValue, int step) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int inputY = 20;
    int inputWidth = 80;
    int inputHeight = 25;
    int inputX = (displayWidth - inputWidth) / 2;

    drawRect(inputX, inputY, inputWidth, inputHeight);

    setFont(ArialMT_Plain_16);
    drawString(displayWidth / 2, inputY + 5, String(value).c_str());
    setFont(ArialMT_Plain_10);

    if (value > minValue) {
        fillTriangle(inputX + 10, inputY + inputHeight / 2,
                     inputX + 15, inputY + inputHeight / 2 - 5,
                     inputX + 15, inputY + inputHeight / 2 + 5);
    }

    if (value < maxValue) {
        fillTriangle(inputX + inputWidth - 10, inputY + inputHeight / 2,
                     inputX + inputWidth - 15, inputY + inputHeight / 2 - 5,
                     inputX + inputWidth - 15, inputY + inputHeight / 2 + 5);
    }

    int stepUpY = inputY + inputHeight + 5;
    int stepDownY = stepUpY + 15;

    drawString(inputX + 20, stepUpY, "Step:");
    drawString(inputX + 60, stepUpY, "+" + String(step));
    drawString(inputX + 60, stepDownY, "-" + String(step));

    display();
}

void SH1106Menu::renderDatePicker(const char *title, int year, int month, int day) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int pickerY = 16;
    int itemWidth = 40;
    int itemHeight = 20;

    const char *monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    drawRect(20, pickerY, itemWidth, itemHeight);
    drawString(20 + itemWidth / 2, pickerY + 6, String(day).c_str());

    drawRect(20 + itemWidth + 4, pickerY, itemWidth, itemHeight);
    drawString(20 + itemWidth + 4 + itemWidth / 2, pickerY + 6, monthNames[month - 1]);

    drawRect(20 + 2 * (itemWidth + 4), pickerY, itemWidth, itemHeight);
    drawString(20 + 2 * (itemWidth + 4) + itemWidth / 2, pickerY + 6, String(year).c_str());

    fillTriangle(40, pickerY - 5, 35, pickerY - 2, 45, pickerY - 2);
    fillTriangle(40, pickerY + itemHeight + 5, 35, pickerY + itemHeight + 2, 45, pickerY + itemHeight + 2);

    fillTriangle(40 + itemWidth + 4, pickerY - 5, 35 + itemWidth + 4, pickerY - 2, 45 + itemWidth + 4, pickerY - 2);
    fillTriangle(40 + itemWidth + 4, pickerY + itemHeight + 5, 35 + itemWidth + 4, pickerY + itemHeight + 2, 45 + itemWidth + 4, pickerY + itemHeight + 2);

    fillTriangle(40 + 2 * (itemWidth + 4), pickerY - 5, 35 + 2 * (itemWidth + 4), pickerY - 2, 45 + 2 * (itemWidth + 4), pickerY - 2);
    fillTriangle(40 + 2 * (itemWidth + 4), pickerY + itemHeight + 5, 35 + 2 * (itemWidth + 4), pickerY + itemHeight + 2, 45 + 2 * (itemWidth + 4), pickerY + itemHeight + 2);

    int calY = pickerY + itemHeight + 10;

    drawString(displayWidth / 2, calY, monthNames[month - 1] + String(" ") + String(year));

    const int daysInMonth[] = {0, 31, 28 + (year % 4 == 0 ? 1 : 0), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    calY += 12;

    drawString(25, calY, "S");
    drawString(39, calY, "M");
    drawString(53, calY, "T");
    drawString(67, calY, "W");
    drawString(81, calY, "T");
    drawString(95, calY, "F");
    drawString(109, calY, "S");

    calY += 10;

    int firstDay = 1;
    int dayCount = 1;

    for (int row = 0; row < 6 && dayCount <= daysInMonth[month]; row++) {
        for (int col = 0; col < 7 && dayCount <= daysInMonth[month]; col++) {
            if (row == 0 && col < firstDay) {
                continue;
            }

            int x = 25 + col * 14;
            int y = calY + row * 10;

            if (dayCount == day) {
                fillRect(x - 5, y - 1, 10, 10);
                setColor(BLACK);
                drawString(x, y, String(dayCount).c_str());
                setColor(WHITE);
            } else {
                drawString(x, y, String(dayCount).c_str());
            }

            dayCount++;
        }
    }

    display();
}

void SH1106Menu::renderTimePicker(const char *title, int hour, int minute) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int pickerY = 20;
    int digitWidth = 20;
    int digitHeight = 25;
    int digitGap = 2;
    int timeWidth = 2 * digitWidth + digitGap + 10;
    int timeX = (displayWidth - timeWidth) / 2;

    drawRect(timeX, pickerY, digitWidth, digitHeight);
    drawRect(timeX + digitWidth + digitGap, pickerY, digitWidth, digitHeight);

    drawString(timeX + digitWidth + digitGap + digitWidth + 5, pickerY + 10, ":");

    drawRect(timeX + digitWidth + digitGap + digitWidth + 10, pickerY, digitWidth, digitHeight);
    drawRect(timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth + digitGap, pickerY, digitWidth, digitHeight);

    setFont(ArialMT_Plain_16);

    String hourStr = hour < 10 ? "0" + String(hour) : String(hour);
    String minuteStr = minute < 10 ? "0" + String(minute) : String(minute);

    drawString(timeX + digitWidth / 2, pickerY + 5, String(hourStr[0]).c_str());
    drawString(timeX + digitWidth + digitGap + digitWidth / 2, pickerY + 5, String(hourStr[1]).c_str());

    drawString(timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth / 2, pickerY + 5, String(minuteStr[0]).c_str());
    drawString(timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth + digitGap + digitWidth / 2, pickerY + 5, String(minuteStr[1]).c_str());

    setFont(ArialMT_Plain_10);

    int buttonY = pickerY + digitHeight + 10;

    drawString(timeX, buttonY, "Hour");
    drawString(timeX + digitWidth + digitGap + digitWidth + 10, buttonY, "Minute");

    fillTriangle(timeX + digitWidth, buttonY + 10, timeX + digitWidth - 5, buttonY + 15, timeX + digitWidth + 5, buttonY + 15);
    fillTriangle(timeX + digitWidth, buttonY + 25, timeX + digitWidth - 5, buttonY + 20, timeX + digitWidth + 5, buttonY + 20);

    fillTriangle(timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth, buttonY + 10, timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth - 5, buttonY + 15, timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth + 5, buttonY + 15);
    fillTriangle(timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth, buttonY + 25, timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth - 5, buttonY + 20, timeX + digitWidth + digitGap + digitWidth + 10 + digitWidth + 5, buttonY + 20);

    display();
}

void SH1106Menu::renderColorPicker(const char *title, uint8_t r, uint8_t g, uint8_t b) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int previewSize = 30;
    int previewX = (displayWidth - previewSize) / 2;
    int previewY = 14;

    drawRect(previewX, previewY, previewSize, previewSize);

    for (int y = 0; y < previewSize - 2; y++) {
        for (int x = 0; x < previewSize - 2; x++) {
            if ((r > 128 && g > 128 && b > 128) ||
                (r > 200 && g > 100) ||
                (r > 200 && b > 100) ||
                (g > 200 && b > 100)) {
                setPixel(previewX + 1 + x, previewY + 1 + y);
            }
        }
    }

    int sliderY = previewY + previewSize + 5;
    int sliderWidth = 100;
    int sliderHeight = 8;
    int sliderX = (displayWidth - sliderWidth) / 2;

    drawRect(sliderX, sliderY, sliderWidth, sliderHeight);
    drawRect(sliderX, sliderY + sliderHeight + 5, sliderWidth, sliderHeight);
    drawRect(sliderX, sliderY + 2 * (sliderHeight + 5), sliderWidth, sliderHeight);

    drawString(sliderX - 10, sliderY, "R");
    drawString(sliderX - 10, sliderY + sliderHeight + 5, "G");
    drawString(sliderX - 10, sliderY + 2 * (sliderHeight + 5), "B");

    int rPos = map(r, 0, 255, 0, sliderWidth - 4);
    int gPos = map(g, 0, 255, 0, sliderWidth - 4);
    int bPos = map(b, 0, 255, 0, sliderWidth - 4);

    fillRect(sliderX + 2, sliderY + 2, rPos, sliderHeight - 4);
    fillRect(sliderX + 2, sliderY + sliderHeight + 5 + 2, gPos, sliderHeight - 4);
    fillRect(sliderX + 2, sliderY + 2 * (sliderHeight + 5) + 2, bPos, sliderHeight - 4);

    String colorStr = "RGB(" + String(r) + "," + String(g) + "," + String(b) + ")";
    drawString(displayWidth / 2, sliderY + 3 * (sliderHeight + 5), colorStr.c_str());

    display();
}

void SH1106Menu::renderSlidingMenu(const char *title, const char *options[], int numOptions, int selectedOption, int slidePosition) {
    clear();

    int headerHeight = 12;

    setColor(WHITE);
    fillRect(0, 0, displayWidth, headerHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);
    setColor(WHITE);

    int menuWidth = displayWidth - 10;
    int menuHeight = 40;
    int menuX = 5 + slidePosition;
    int menuY = 20;

    int visibleOptions = min(numOptions, 3);

    for (int i = 0; i < visibleOptions; i++) {
        int x = menuX + i * menuWidth;

        if (i == selectedOption % visibleOptions) {
            fillRect(x, menuY, menuWidth, menuHeight);
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(x + menuWidth / 2, menuY + menuHeight / 2 - 5, options[(selectedOption / visibleOptions) * visibleOptions + i]);
            setColor(WHITE);
        } else {
            drawRect(x, menuY, menuWidth, menuHeight);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(x + menuWidth / 2, menuY + menuHeight / 2 - 5, options[(selectedOption / visibleOptions) * visibleOptions + i]);
        }
    }

    int indicatorY = menuY + menuHeight + 5;
    for (int i = 0; i < (numOptions + visibleOptions - 1) / visibleOptions; i++) {
        int x = displayWidth / 2 - ((numOptions + visibleOptions - 1) / visibleOptions) * 5 + i * 10;

        if (i == selectedOption / visibleOptions) {
            fillCircle(x, indicatorY, 3);
        } else {
            drawCircle(x, indicatorY, 3);
        }
    }

    display();
}

void SH1106Menu::renderSegmentedControl(const char *title, const char *segments[], int numSegments, int selectedSegment) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int segmentHeight = 20;
    int segmentWidth = (displayWidth - 10) / numSegments;
    int controlX = 5;
    int controlY = 15;

    drawRect(controlX, controlY, segmentWidth * numSegments, segmentHeight);

    for (int i = 0; i < numSegments; i++) {
        int x = controlX + i * segmentWidth;

        if (i == selectedSegment) {
            fillRect(x, controlY, segmentWidth, segmentHeight);
            setColor(BLACK);
            drawString(x + segmentWidth / 2, controlY + 6, segments[i]);
            setColor(WHITE);
        } else {
            if (i > 0) {
                drawLine(x, controlY, x, controlY + segmentHeight);
            }
            drawString(x + segmentWidth / 2, controlY + 6, segments[i]);
        }
    }

    int contentY = controlY + segmentHeight + 10;
    drawString(displayWidth / 2, contentY, "Selected:");
    drawString(displayWidth / 2, contentY + 12, segments[selectedSegment]);

    display();
}

void SH1106Menu::renderExpandableMenu(const char *title, const char *categories[], const char *items[][5], int numItemsPerCategory[], int numCategories, int expandedCategory) {
    clear();

    int headerHeight = 12;

    setColor(WHITE);
    fillRect(0, 0, displayWidth, headerHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 1, title);
    setColor(WHITE);

    int menuY = headerHeight;
    setTextAlignment(TEXT_ALIGN_LEFT);

    for (int i = 0; i < numCategories; i++) {
        drawRect(0, menuY, displayWidth, 15);
        drawString(5, menuY + 3, categories[i]);

        if (i == expandedCategory) {
            drawTriangle(displayWidth - 10, menuY + 7, displayWidth - 5, menuY + 7, displayWidth - 7, menuY + 10);

            menuY += 15;

            for (int j = 0; j < numItemsPerCategory[i]; j++) {
                drawString(15, menuY + 3, items[i][j]);
                menuY += 12;
            }
        } else {
            drawTriangle(displayWidth - 10, menuY + 7, displayWidth - 5, menuY + 7, displayWidth - 7, menuY + 4);
            menuY += 15;
        }
    }

    display();
}

void SH1106Menu::renderDrawerMenu(const char *title, const char *options[], int numOptions, int selectedOption, bool isOpen) {
    clear();

    int drawerWidth = min(100, displayWidth - 20);
    int drawerX = isOpen ? 0 : -drawerWidth;

    int headerHeight = 15;

    fillRect(drawerX, 0, drawerWidth, displayHeight);

    setColor(BLACK);
    fillRect(drawerX, 0, drawerWidth, headerHeight);
    setColor(WHITE);

    setTextAlignment(TEXT_ALIGN_LEFT);
    drawString(drawerX + 5, 3, title);

    setColor(BLACK);

    for (int i = 0; i < numOptions; i++) {
        int y = headerHeight + i * 12;

        if (i == selectedOption) {
            setColor(WHITE);
            fillRect(drawerX, y, drawerWidth, 12);
            setColor(BLACK);
        }

        drawString(drawerX + 10, y + 2, options[i]);

        if (i == selectedOption) {
            setColor(BLACK);
        }
    }

    setColor(WHITE);

    if (isOpen) {
        fillRect(drawerWidth, 0, displayWidth - drawerWidth, displayHeight);

        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(drawerWidth + (displayWidth - drawerWidth) / 2, displayHeight / 2 - 5, "Content Area");

        setColor(WHITE);
        drawLine(drawerWidth, 0, drawerWidth, displayHeight);
    }

    display();
}