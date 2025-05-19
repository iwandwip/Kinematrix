#include "sh1106-menu.h"

void SH1106Menu::renderRadialMenu(const char *title, const char *options[], int numOptions, int selectedOption) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2 + 5;
    int radius = min(displayWidth, displayHeight) / 2 - 14;

    drawCircle(centerX, centerY, radius / 3);

    for (int i = 0; i < numOptions; i++) {
        float angle = i * 2 * PI / numOptions;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);

        if (i == selectedOption) {
            fillCircle(x, y, 5);
            setColor(BLACK);
            drawString(x, y - 2, String(i + 1).c_str());
            setColor(WHITE);
        } else {
            drawCircle(x, y, 5);
            drawString(x, y - 2, String(i + 1).c_str());
        }

        int textX = centerX + (radius / 2) * cos(angle);
        int textY = centerY + (radius / 2) * sin(angle);

        if (x < centerX) {
            setTextAlignment(TEXT_ALIGN_RIGHT);
            drawString(textX - 5, textY - 5, options[i]);
        } else {
            setTextAlignment(TEXT_ALIGN_LEFT);
            drawString(textX + 5, textY - 5, options[i]);
        }
    }

    display();
}

void SH1106Menu::renderTabView(const char *tabs[], int numTabs, int activeTab, const char *content) {
    clear();

    int tabWidth = displayWidth / numTabs;
    int tabHeight = 12;

    for (int i = 0; i < numTabs; i++) {
        int tabX = i * tabWidth;

        if (i == activeTab) {
            fillRect(tabX, 0, tabWidth, tabHeight);
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(tabX + tabWidth / 2, 2, tabs[i]);
            setColor(WHITE);
        } else {
            drawRect(tabX, 0, tabWidth, tabHeight);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(tabX + tabWidth / 2, 2, tabs[i]);
        }
    }

    drawLine(0, tabHeight, displayWidth, tabHeight);

    setTextAlignment(TEXT_ALIGN_LEFT);
    drawStringMaxWidth(2, tabHeight + 4, displayWidth - 4, content);

    display();
}

void SH1106Menu::renderHistogram(const char *title, int values[], int numValues, int maxValue) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int chartX = 10;
    int chartY = 14;
    int chartWidth = displayWidth - 20;
    int chartHeight = displayHeight - chartY - 10;

    drawRect(chartX, chartY, chartWidth, chartHeight);

    if (numValues > 0 && maxValue > 0) {
        int columnWidth = (chartWidth - 4) / numValues;
        int columnMaxHeight = chartHeight - 4;

        for (int i = 0; i < numValues; i++) {
            int columnHeight = map(values[i], 0, maxValue, 0, columnMaxHeight);
            int columnX = chartX + 2 + (i * columnWidth);
            int columnY = chartY + chartHeight - 2 - columnHeight;

            fillRect(columnX, columnY, columnWidth - 1, columnHeight);

            if (i % 2 == 0) {
                setTextAlignment(TEXT_ALIGN_CENTER);
                drawString(columnX + columnWidth / 2, chartY + chartHeight + 1, String(i).c_str());
            }
        }
    }

    display();
}

void SH1106Menu::renderCalendar(int year, int month, int highlightDay) {
    clear();

    const char *monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    String header = String(monthNames[month - 1]) + " " + String(year);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, header.c_str());

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(18, 12, "S");
    drawString(36, 12, "M");
    drawString(54, 12, "T");
    drawString(72, 12, "W");
    drawString(90, 12, "T");
    drawString(108, 12, "F");
    drawString(126, 12, "S");

    int daysInMonth[] = {31, 28 + (year % 4 == 0 ? 1 : 0), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int firstDay = 1;
    int startDay = firstDay;
    int day = 1;

    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            int x = 18 + col * 18;
            int y = 24 + row * 10;

            if (row == 0 && col < startDay) {
                continue;
            }

            if (day <= daysInMonth[month - 1]) {
                if (day == highlightDay) {
                    fillRect(x - 8, y - 4, 16, 10);
                    setColor(BLACK);
                    drawString(x, y, String(day).c_str());
                    setColor(WHITE);
                } else {
                    drawString(x, y, String(day).c_str());
                }
                day++;
            }
        }
    }

    display();
}

void SH1106Menu::renderGridMenu(const char *options[], int numOptions, int columns, int selectedOption) {
    clear();

    int rows = (numOptions + columns - 1) / columns;
    int cellWidth = displayWidth / columns;
    int cellHeight = displayHeight / rows;

    for (int i = 0; i < numOptions; i++) {
        int row = i / columns;
        int col = i % columns;

        int x = col * cellWidth;
        int y = row * cellHeight;

        if (i == selectedOption) {
            fillRect(x, y, cellWidth, cellHeight);
            setColor(BLACK);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(x + cellWidth / 2, y + cellHeight / 2 - 4, options[i]);
            setColor(WHITE);
        } else {
            drawRect(x, y, cellWidth, cellHeight);
            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(x + cellWidth / 2, y + cellHeight / 2 - 4, options[i]);
        }
    }

    display();
}

void SH1106Menu::renderSensorGraph(const char *title, int values[], int numValues, int minValue, int maxValue, bool autoScroll) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int graphX = 30;
    int graphY = 14;
    int graphWidth = displayWidth - graphX - 2;
    int graphHeight = displayHeight - graphY - 2;

    drawRect(graphX, graphY, graphWidth, graphHeight);

    setTextAlignment(TEXT_ALIGN_RIGHT);
    drawString(graphX - 2, graphY - 5, String(maxValue).c_str());
    drawString(graphX - 2, graphY + graphHeight - 5, String(minValue).c_str());

    if (numValues > 1 && (maxValue > minValue)) {
        int pointSpacing = (graphWidth - 4) / min(numValues - 1, graphWidth - 4);
        int startIdx = autoScroll ? max(0, numValues - graphWidth / pointSpacing) : 0;

        for (int i = startIdx; i < numValues - 1; i++) {
            int idx1 = i - startIdx;
            int idx2 = i + 1 - startIdx;

            int x1 = graphX + 2 + (idx1 * pointSpacing);
            int y1 = graphY + graphHeight - 2 - map(values[i], minValue, maxValue, 0, graphHeight - 4);

            int x2 = graphX + 2 + (idx2 * pointSpacing);
            int y2 = graphY + graphHeight - 2 - map(values[i + 1], minValue, maxValue, 0, graphHeight - 4);

            drawLine(x1, y1, x2, y2);
        }
    }

    display();
}

void SH1106Menu::renderQRCode(const char *data, int size) {
    clear();

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2;
    int qrSize = min(displayWidth, displayHeight) - 10;
    int moduleSize = qrSize / (size * 4 + 17);
    int offset = (min(displayWidth, displayHeight) - moduleSize * (size * 4 + 17)) / 2;

    int startX = centerX - qrSize / 2 + offset;
    int startY = centerY - qrSize / 2 + offset;

    drawRect(startX - 2, startY - 2, qrSize + 4, qrSize + 4);

    for (int i = 0; i < strlen(data) && i < size * size; i++) {
        int x = i % size;
        int y = i / size;

        if (data[i] == '1') {
            fillRect(startX + x * moduleSize * 4, startY + y * moduleSize * 4,
                     moduleSize * 4, moduleSize * 4);
        }
    }

    display();
}

void SH1106Menu::renderGauge(const char *title, int value, int minValue, int maxValue) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int centerX = displayWidth / 2;
    int centerY = displayHeight - 5;
    int radius = displayHeight - 20;

    drawString(centerX, centerY - radius - 10, String(value).c_str());

    float startAngle = PI;
    float endAngle = 2 * PI;

    for (float angle = startAngle; angle <= endAngle; angle += 0.05) {
        int x1 = centerX + (radius - 2) * cos(angle);
        int y1 = centerY + (radius - 2) * sin(angle);
        int x2 = centerX + radius * cos(angle);
        int y2 = centerY + radius * sin(angle);

        drawLine(x1, y1, x2, y2);
    }

    float valueAngle = startAngle + (endAngle - startAngle) * (value - minValue) / (maxValue - minValue);
    int needleLength = radius - 5;

    int needleX = centerX + needleLength * cos(valueAngle);
    int needleY = centerY + needleLength * sin(valueAngle);

    drawLine(centerX, centerY, needleX, needleY);
    fillCircle(centerX, centerY, 3);

    int tickMinX = centerX + radius * cos(startAngle);
    int tickMinY = centerY + radius * sin(startAngle);
    drawString(tickMinX - 10, tickMinY + 2, String(minValue).c_str());

    int tickMaxX = centerX + radius * cos(endAngle);
    int tickMaxY = centerY + radius * sin(endAngle);
    drawString(tickMaxX + 10, tickMaxY + 2, String(maxValue).c_str());

    display();
}

void SH1106Menu::renderSpeedometer(int speed, int maxSpeed) {
    clear();

    int centerX = displayWidth / 2;
    int centerY = displayHeight - 5;
    int radius = displayHeight - 20;

    setFont(ArialMT_Plain_16);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(centerX, 2, String(speed).c_str());
    setFont(ArialMT_Plain_10);
    drawString(centerX, 20, "km/h");

    float startAngle = PI + PI / 4;
    float endAngle = 3 * PI - PI / 4;

    for (float angle = startAngle; angle <= endAngle; angle += 0.05) {
        int x1 = centerX + (radius - 2) * cos(angle);
        int y1 = centerY + (radius - 2) * sin(angle);
        int x2 = centerX + radius * cos(angle);
        int y2 = centerY + radius * sin(angle);

        drawLine(x1, y1, x2, y2);
    }

    float valueAngle = startAngle + (endAngle - startAngle) * min(speed, maxSpeed) / maxSpeed;
    int needleLength = radius - 5;

    int needleX = centerX + needleLength * cos(valueAngle);
    int needleY = centerY + needleLength * sin(valueAngle);

    drawLine(centerX, centerY, needleX, needleY);
    fillCircle(centerX, centerY, 3);

    int steps = 5;
    for (int i = 0; i <= steps; i++) {
        float angle = startAngle + (endAngle - startAngle) * i / steps;
        int tickX = centerX + radius * cos(angle);
        int tickY = centerY + radius * sin(angle);

        int value = maxSpeed * i / steps;
        String valueStr = String(value);

        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(tickX, tickY + 5, valueStr.c_str());
    }

    display();
}

void SH1106Menu::renderWeatherIcon(int weatherType, const char *temperature) {
    clear();

    int iconCenterX = displayWidth / 2;
    int iconCenterY = 25;
    int iconSize = 30;

    switch (weatherType) {
        case 0: // Sunny
            fillCircle(iconCenterX, iconCenterY, 10);

            for (int i = 0; i < 8; i++) {
                float angle = i * PI / 4;
                int rayX1 = iconCenterX + 12 * cos(angle);
                int rayY1 = iconCenterY + 12 * sin(angle);
                int rayX2 = iconCenterX + 16 * cos(angle);
                int rayY2 = iconCenterY + 16 * sin(angle);

                drawLine(rayX1, rayY1, rayX2, rayY2);
            }
            break;

        case 1: // Cloudy
            fillCircle(iconCenterX - 7, iconCenterY, 7);
            fillCircle(iconCenterX + 3, iconCenterY - 2, 9);
            fillCircle(iconCenterX + 10, iconCenterY + 4, 6);
            break;

        case 2: // Rainy
            fillCircle(iconCenterX - 7, iconCenterY - 5, 7);
            fillCircle(iconCenterX + 3, iconCenterY - 7, 9);
            fillCircle(iconCenterX + 10, iconCenterY - 3, 6);

            for (int i = 0; i < 4; i++) {
                int dropX = iconCenterX - 10 + i * 7;
                drawLine(dropX, iconCenterY + 5, dropX - 3, iconCenterY + 15);
            }
            break;

        case 3: // Snowy
            fillCircle(iconCenterX - 7, iconCenterY - 5, 7);
            fillCircle(iconCenterX + 3, iconCenterY - 7, 9);
            fillCircle(iconCenterX + 10, iconCenterY - 3, 6);

            for (int i = 0; i < 3; i++) {
                int x = iconCenterX - 8 + i * 8;
                int y = iconCenterY + 8;
                drawLine(x - 3, y, x + 3, y);
                drawLine(x, y - 3, x, y + 3);
            }
            break;

        case 4: // Thunderstorm
            fillCircle(iconCenterX - 7, iconCenterY - 5, 7);
            fillCircle(iconCenterX + 3, iconCenterY - 7, 9);
            fillCircle(iconCenterX + 10, iconCenterY - 3, 6);

            drawLine(iconCenterX + 5, iconCenterY + 5, iconCenterX - 2, iconCenterY + 15);
            drawLine(iconCenterX - 2, iconCenterY + 15, iconCenterX + 3, iconCenterY + 15);
            drawLine(iconCenterX + 3, iconCenterY + 15, iconCenterX - 4, iconCenterY + 25);
            break;
    }

    if (temperature != nullptr) {
        setTextAlignment(TEXT_ALIGN_CENTER);
        setFont(ArialMT_Plain_16);
        drawString(iconCenterX, iconCenterY + 20, temperature);
        setFont(ArialMT_Plain_10);
    }

    display();
}

void SH1106Menu::renderPopupMenu(const char *title, const char *options[], int numOptions, int selectedOption) {
    int menuWidth = min(100, displayWidth - 10);
    int menuHeight = min(50, displayHeight - 10);
    int menuX = (displayWidth - menuWidth) / 2;
    int menuY = (displayHeight - menuHeight) / 2;

    fillRect(menuX, menuY, menuWidth, menuHeight);
    setColor(BLACK);

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(menuX + menuWidth / 2, menuY + 2, title);

    drawRect(menuX, menuY, menuWidth, 12);

    setTextAlignment(TEXT_ALIGN_LEFT);
    int optionY = menuY + 14;
    int optionHeight = (menuHeight - 14) / min(numOptions, 3);

    for (int i = 0; i < min(numOptions, 3); i++) {
        if (i == selectedOption) {
            fillRect(menuX + 2, optionY, menuWidth - 4, optionHeight);
            setColor(WHITE);
            drawString(menuX + 5, optionY + 2, options[i]);
            setColor(BLACK);
        } else {
            drawString(menuX + 5, optionY + 2, options[i]);
        }

        optionY += optionHeight;
    }

    setColor(WHITE);
    display();
}

void SH1106Menu::renderAlertBox(const char *title, const char *message, bool isError) {
    int boxWidth = min(116, displayWidth - 10);
    int boxHeight = min(54, displayHeight - 10);
    int boxX = (displayWidth - boxWidth) / 2;
    int boxY = (displayHeight - boxHeight) / 2;

    fillRect(boxX, boxY, boxWidth, boxHeight);

    setColor(BLACK);

    int titleBarHeight = 14;
    if (isError) {
        fillRect(boxX, boxY, boxWidth, titleBarHeight);
        setColor(WHITE);
    }

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(boxX + boxWidth / 2, boxY + 2, title);

    if (isError) {
        setColor(BLACK);
    }

    int iconSize = 12;
    int iconX = boxX + iconSize;
    int iconY = boxY + titleBarHeight + (boxHeight - titleBarHeight - iconSize) / 2;

    if (isError) {
        drawCircle(iconX, iconY, iconSize / 2);
        drawLine(iconX - 3, iconY - 3, iconX + 3, iconY + 3);
        drawLine(iconX + 3, iconY - 3, iconX - 3, iconY + 3);
    } else {
        drawCircle(iconX, iconY, iconSize / 2);
        drawLine(iconX - 3, iconY, iconX - 1, iconY + 3);
        drawLine(iconX - 1, iconY + 3, iconX + 3, iconY - 3);
    }

    setTextAlignment(TEXT_ALIGN_LEFT);
    drawStringMaxWidth(boxX + iconSize * 2, boxY + titleBarHeight + 4,
                       boxWidth - iconSize * 3, message);

    setColor(WHITE);
    display();
}

void SH1106Menu::renderProgressCircle(const char *title, int percentage) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2 + 4;
    int radius = min(displayWidth, displayHeight) / 2 - 14;

    drawCircle(centerX, centerY, radius);

    if (percentage > 0) {
        float startAngle = -90 * PI / 180;
        float endAngle = (percentage * 3.6 - 90) * PI / 180;

        for (float angle = startAngle; angle <= endAngle; angle += 0.05) {
            int x1 = centerX + (radius - 2) * cos(angle);
            int y1 = centerY + (radius - 2) * sin(angle);
            int x2 = centerX + radius * cos(angle);
            int y2 = centerY + radius * sin(angle);

            drawLine(x1, y1, x2, y2);
        }
    }

    String percentText = String(percentage) + "%";
    drawString(centerX, centerY - 5, percentText.c_str());

    display();
}

void SH1106Menu::renderSplitScreen(const char *leftTitle, const char *leftContent, const char *rightTitle, const char *rightContent) {
    clear();

    int dividerX = displayWidth / 2;

    drawLine(dividerX, 0, dividerX, displayHeight);

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(dividerX / 2, 0, leftTitle);
    drawString(dividerX + dividerX / 2, 0, rightTitle);

    setTextAlignment(TEXT_ALIGN_LEFT);
    drawStringMaxWidth(2, 14, dividerX - 4, leftContent);
    drawStringMaxWidth(dividerX + 2, 14, dividerX - 4, rightContent);

    display();
}

void SH1106Menu::renderDataTable(const char *headers[], const char *data[][3], int rows, int cols) {
    clear();

    int colWidth = displayWidth / min(cols, 3);
    int rowHeight = 12;

    for (int col = 0; col < min(cols, 3); col++) {
        int x = col * colWidth;

        fillRect(x, 0, colWidth, rowHeight);

        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(x + colWidth / 2, 2, headers[col]);
        setColor(WHITE);
    }

    for (int row = 0; row < min(rows, 4); row++) {
        int y = (row + 1) * rowHeight;

        for (int col = 0; col < min(cols, 3); col++) {
            int x = col * colWidth;

            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(x + colWidth / 2, y + 2, data[row][col]);

            drawLine(x, y, x + colWidth, y);
        }
    }

    for (int col = 0; col <= min(cols, 3); col++) {
        int x = col * colWidth;
        drawLine(x, 0, x, (min(rows, 4) + 1) * rowHeight);
    }

    display();
}

void SH1106Menu::renderBubbleChart(const char *title, int values[], int numValues, int maxValue) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int chartX = 10;
    int chartY = 14;
    int chartWidth = displayWidth - 20;
    int chartHeight = displayHeight - chartY - 10;

    drawRect(chartX, chartY, chartWidth, chartHeight);

    if (numValues > 0 && maxValue > 0) {
        int bubbleMaxRadius = 10;
        int bubbleSpacing = chartWidth / (numValues + 1);

        for (int i = 0; i < numValues; i++) {
            int bubbleRadius = map(values[i], 0, maxValue, 2, bubbleMaxRadius);
            int bubbleX = chartX + bubbleSpacing * (i + 1);
            int bubbleY = chartY + chartHeight / 2;

            fillCircle(bubbleX, bubbleY, bubbleRadius);

            setTextAlignment(TEXT_ALIGN_CENTER);
            drawString(bubbleX, chartY + chartHeight + 1, String(i + 1).c_str());
        }
    }

    display();
}

void SH1106Menu::renderStarRating(const char *title, float rating, float maxRating) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int starSize = 10;
    int numStars = (int) maxRating;
    int totalWidth = numStars * starSize + (numStars - 1) * 2;
    int startX = (displayWidth - totalWidth) / 2;
    int startY = 20;

    for (int i = 0; i < numStars; i++) {
        int starX = startX + i * (starSize + 2);
        int starCenterX = starX + starSize / 2;
        int starCenterY = startY + starSize / 2;

        float fill = min(max(rating - i, 0.0f), 1.0f);

        if (fill >= 1.0) {
            for (int j = 0; j < 5; j++) {
                float angle = -PI / 2 + j * 2 * PI / 5;
                int x = starCenterX + starSize / 2 * cos(angle);
                int y = starCenterY + starSize / 2 * sin(angle);

                int nextIdx = (j + 2) % 5;
                float nextAngle = -PI / 2 + nextIdx * 2 * PI / 5;
                int nextX = starCenterX + starSize / 2 * cos(nextAngle);
                int nextY = starCenterY + starSize / 2 * sin(nextAngle);

                drawLine(starCenterX, starCenterY, x, y);
                drawLine(x, y, nextX, nextY);
            }

            fillCircle(starCenterX, starCenterY, starSize / 4);
        } else if (fill > 0.0) {
            for (int j = 0; j < 5; j++) {
                float angle = -PI / 2 + j * 2 * PI / 5;
                int x = starCenterX + starSize / 2 * cos(angle);
                int y = starCenterY + starSize / 2 * sin(angle);

                int nextIdx = (j + 2) % 5;
                float nextAngle = -PI / 2 + nextIdx * 2 * PI / 5;
                int nextX = starCenterX + starSize / 2 * cos(nextAngle);
                int nextY = starCenterY + starSize / 2 * sin(nextAngle);

                drawLine(starCenterX, starCenterY, x, y);
                drawLine(x, y, nextX, nextY);
            }
        } else {
            for (int j = 0; j < 5; j++) {
                float angle = -PI / 2 + j * 2 * PI / 5;
                int x = starCenterX + starSize / 2 * cos(angle);
                int y = starCenterY + starSize / 2 * sin(angle);

                int nextIdx = (j + 2) % 5;
                float nextAngle = -PI / 2 + nextIdx * 2 * PI / 5;
                int nextX = starCenterX + starSize / 2 * cos(nextAngle);
                int nextY = starCenterY + starSize / 2 * sin(nextAngle);

                drawLine(x, y, nextX, nextY);
            }
        }
    }

    String ratingText = String(rating, 1) + "/" + String(maxRating, 0);
    drawString(displayWidth / 2, 40, ratingText.c_str());

    display();
}

void SH1106Menu::renderRoundedButton(int x, int y, int width, int height, const char *text, bool inverted) {
    int radius = min(width, height) / 4;

    if (inverted) {
        fillRoundedRect(x, y, width, height, radius);
        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(x + width / 2, y + (height - 10) / 2, text);
        setColor(WHITE);
    } else {
        drawRoundedRect(x, y, width, height, radius);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(x + width / 2, y + (height - 10) / 2, text);
    }
}

void SH1106Menu::renderPieChart(const char *title, int values[], int numValues) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2 + 4;
    int radius = min(displayWidth, displayHeight) / 2 - 14;

    int total = 0;
    for (int i = 0; i < numValues; i++) {
        total += values[i];
    }

    if (total <= 0) {
        drawCircle(centerX, centerY, radius);
        display();
        return;
    }

    float startAngle = 0;

    for (int i = 0; i < numValues; i++) {
        float sliceAngle = 2 * PI * values[i] / total;
        float endAngle = startAngle + sliceAngle;

        for (float angle = startAngle; angle < endAngle; angle += 0.05) {
            for (int r = 0; r <= radius; r++) {
                int x = centerX + r * cos(angle);
                int y = centerY + r * sin(angle);
                setPixel(x, y);
            }
        }

        float midAngle = startAngle + sliceAngle / 2;
        int labelX = centerX + (radius + 10) * cos(midAngle);
        int labelY = centerY + (radius + 10) * sin(midAngle);

        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(labelX, labelY - 5, String(i + 1).c_str());

        startAngle = endAngle;
    }

    display();
}

void SH1106Menu::renderTabbedData(const char *title, const char *tabs[], const char *contents[], int numTabs, int activeTab) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 0, title);

    int tabY = 12;
    int tabHeight = 12;
    int contentY = tabY + tabHeight;
    int contentHeight = displayHeight - contentY;

    int tabWidth = displayWidth / numTabs;

    for (int i = 0; i < numTabs; i++) {
        int tabX = i * tabWidth;

        if (i == activeTab) {
            fillRect(tabX, tabY, tabWidth, tabHeight);
            setColor(BLACK);
            drawString(tabX + tabWidth / 2, tabY + 2, tabs[i]);
            setColor(WHITE);
        } else {
            drawRect(tabX, tabY, tabWidth, tabHeight);
            drawString(tabX + tabWidth / 2, tabY + 2, tabs[i]);
        }
    }

    setTextAlignment(TEXT_ALIGN_LEFT);
    drawStringMaxWidth(2, contentY + 2, displayWidth - 4, contents[activeTab]);

    display();
}