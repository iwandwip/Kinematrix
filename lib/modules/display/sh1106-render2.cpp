#include "sh1106-menu.h"

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

void SH1106Menu::renderInfoScreenCenter(const char *title, const char *line1, const char *line2, const char *line3) {
    clear();

    setColor(WHITE);
    fillRect(0, 0, 128, 15);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(64, 3, title);

    setColor(WHITE);
    drawRect(0, 15, 128, 49);

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(64, 20, line1);
    drawString(64, 32, line2);
    drawString(64, 44, line3);

    display();
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

        int maxSeconds = 60;
        int progress = map(seconds, 0, maxSeconds, 0, barWidth);
        if (progress > 0) {
            fillRect(barX, barY, progress, barHeight);
        }
    }

    setFont(ArialMT_Plain_10);
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
    int topMargin = 4;
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

    percentage = constrain(percentage, 0, 100);

    int batteryWidth = 40;
    int batteryHeight = 20;
    int batteryX = (displayWidth - batteryWidth) / 2;
    int batteryY = 10;

    drawRect(batteryX, batteryY, batteryWidth, batteryHeight);

    int terminalWidth = 4;
    int terminalHeight = 8;
    drawRect(batteryX + batteryWidth, batteryY + (batteryHeight - terminalHeight) / 2,
             terminalWidth, terminalHeight);

    int fillWidth = map(percentage, 0, 100, 0, batteryWidth - 4);
    if (fillWidth > 0) {
        fillRect(batteryX + 2, batteryY + 2, fillWidth, batteryHeight - 4);
    }

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

    String percentText = String(percentage) + "%";
    drawString(displayWidth / 2, batteryY + batteryHeight + 5, percentText);

    if (charging) {
        drawString(displayWidth / 2, batteryY + batteryHeight + 20, "Charging");
    }

    display();
}

void SH1106Menu::renderSignalStrength(int strength, const char *networkName) {
    clear();

    strength = constrain(strength, 0, 4);

    int barWidth = 6;
    int barGap = 2;
    int maxBarHeight = 20;
    int baseX = (displayWidth - (barWidth * 4 + barGap * 3)) / 2;
    int baseY = 20;

    for (int i = 0; i < 4; i++) {
        int barHeight = map(i + 1, 1, 4, 5, maxBarHeight);

        if (i < strength) {
            fillRect(baseX + i * (barWidth + barGap),
                     baseY + maxBarHeight - barHeight,
                     barWidth, barHeight);
        } else {
            drawRect(baseX + i * (barWidth + barGap),
                     baseY + maxBarHeight - barHeight,
                     barWidth, barHeight);
        }
    }

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

    if (networkName != nullptr) {
        drawString(displayWidth / 2, baseY + maxBarHeight + 20, networkName);
    }

    display();
}

void SH1106Menu::renderClock(int hour, int minute, int second, bool isAnalog) {
    clear();

    if (isAnalog) {
        int centerX = displayWidth / 2;
        int centerY = displayHeight / 2;
        int radius = min(displayWidth, displayHeight) / 2 - 10;

        drawCircle(centerX, centerY, radius);

        for (int i = 0; i < 12; i++) {
            float angle = i * 30 * 3.14159 / 180;
            int markerStartX = centerX + (radius - 4) * sin(angle);
            int markerStartY = centerY - (radius - 4) * cos(angle);
            int markerEndX = centerX + radius * sin(angle);
            int markerEndY = centerY - radius * cos(angle);

            drawLine(markerStartX, markerStartY, markerEndX, markerEndY);
        }

        float hourAngle = ((hour % 12) * 30 + minute / 2) * 3.14159 / 180;
        float minuteAngle = minute * 6 * 3.14159 / 180;
        float secondAngle = second * 6 * 3.14159 / 180;

        int hourHandLength = radius * 0.5;
        int hourHandX = centerX + hourHandLength * sin(hourAngle);
        int hourHandY = centerY - hourHandLength * cos(hourAngle);
        drawLine(centerX, centerY, hourHandX, hourHandY);

        int minuteHandLength = radius * 0.7;
        int minuteHandX = centerX + minuteHandLength * sin(minuteAngle);
        int minuteHandY = centerY - minuteHandLength * cos(minuteAngle);
        drawLine(centerX, centerY, minuteHandX, minuteHandY);

        if (second >= 0) {
            int secondHandLength = radius * 0.8;
            int secondHandX = centerX + secondHandLength * sin(secondAngle);
            int secondHandY = centerY - secondHandLength * cos(secondAngle);
            drawLine(centerX, centerY, secondHandX, secondHandY);
        }

        fillCircle(centerX, centerY, 2);
    } else {
        setTextAlignment(TEXT_ALIGN_CENTER);

        char timeStr[9];
        if (second >= 0) {
            sprintf(timeStr, "%02d:%02d:%02d", hour, minute, second);
        } else {
            sprintf(timeStr, "%02d:%02d", hour, minute);
        }

        setFont(ArialMT_Plain_24);
        drawString(displayWidth / 2, (displayHeight - 24) / 2, timeStr);

        setFont(ArialMT_Plain_10);
    }

    display();
}

void SH1106Menu::renderPercentageCircle(int percentage, const char *text) {
    clear();

    percentage = constrain(percentage, 0, 100);

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2;
    int radius = min(displayWidth, displayHeight) / 2 - 10;

    drawCircle(centerX, centerY, radius);

    float startAngle = -90 * 3.14159 / 180;
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

    setTextAlignment(TEXT_ALIGN_CENTER);

    String percentText = String(percentage) + "%";

    if (text != nullptr) {
        setFont(ArialMT_Plain_16);
        drawString(centerX, centerY - 12, percentText);

        setFont(ArialMT_Plain_10);
        drawString(centerX, centerY + 8, text);
    } else {
        setFont(ArialMT_Plain_24);
        drawString(centerX, centerY - 12, percentText);
    }

    setFont(ArialMT_Plain_10);

    display();
}

void SH1106Menu::renderScrollingText(const char *text, int speed, int scrollCount) {
    int textWidth = getStringWidth(text);

    if (textWidth <= displayWidth) {
        clear();
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, (displayHeight - 10) / 2, text);
        display();
        delay(speed * 10);
        return;
    }

    int totalScroll = displayWidth + textWidth;

    setTextAlignment(TEXT_ALIGN_LEFT);

    for (int count = 0; count < scrollCount; count++) {
        for (int offset = displayWidth; offset > -textWidth; offset -= 2) {
            clear();
            drawString(offset, (displayHeight - 10) / 2, text);
            display();
            delay(speed);
        }

        delay(speed * 5);
    }
}

void SH1106Menu::renderAnimatedLoading(int frame) {
    clear();

    int centerX = displayWidth / 2;
    int centerY = displayHeight / 2;
    int radius = 15;

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(centerX, centerY - 25, "Loading...");

    frame = frame % 8;

    for (int i = 0; i < 8; i++) {
        float startAngle = i * 45 * 3.14159 / 180;

        int distance = (i - frame + 8) % 8;
        bool isActive = (distance < 3);

        int x1 = centerX + radius * cos(startAngle);
        int y1 = centerY + radius * sin(startAngle);
        int x2 = centerX + (radius - 5) * cos(startAngle);
        int y2 = centerY + (radius - 5) * sin(startAngle);

        if (isActive) {
            drawLine(x1, y1, x2, y2);
            drawLine(x1 + 1, y1, x2 + 1, y2);
            drawLine(x1, y1 + 1, x2, y2 + 1);
        } else {
            drawLine(x1, y1, x2, y2);
        }
    }

    display();
}

void SH1106Menu::renderToggleSwitch(const char *label, bool state) {
    clear();

    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, 10, label);

    int switchWidth = 30;
    int switchHeight = 15;
    int switchX = (displayWidth - switchWidth) / 2;
    int switchY = 30;

    drawRoundedRect(switchX, switchY, switchWidth, switchHeight, switchHeight / 2);

    int knobSize = switchHeight - 4;
    int knobX = state ? (switchX + switchWidth - knobSize - 2) : (switchX + 2);

    if (state) {
        fillRoundedRect(switchX, switchY, switchWidth, switchHeight, switchHeight / 2);

        setColor(BLACK);
        fillCircle(knobX + knobSize / 2, switchY + switchHeight / 2, knobSize / 2);
        setColor(WHITE);
    } else {
        fillCircle(knobX + knobSize / 2, switchY + switchHeight / 2, knobSize / 2);
    }

    drawString(displayWidth / 2, switchY + switchHeight + 10, state ? "ON" : "OFF");

    display();
}

void SH1106Menu::renderNotification(const char *message, int timeMs) {
    uint8_t *tempBuffer = new uint8_t[displayBufferSize];
    memcpy(tempBuffer, buffer, displayBufferSize);

    int notificationHeight = 20;
    int notificationY = 0;

    for (int i = -notificationHeight; i <= 0; i += 2) {
        clear();

        memcpy(buffer, tempBuffer, displayBufferSize);

        fillRect(0, i, displayWidth, notificationHeight);

        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, i + 5, message);
        setColor(WHITE);

        display();
        delay(10);
    }

    delay(timeMs);

    for (int i = 0; i >= -notificationHeight; i -= 2) {
        clear();

        memcpy(buffer, tempBuffer, displayBufferSize);

        fillRect(0, i, displayWidth, notificationHeight);

        setColor(BLACK);
        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, i + 5, message);
        setColor(WHITE);

        display();
        delay(10);
    }

    memcpy(buffer, tempBuffer, displayBufferSize);
    display();

    delete[] tempBuffer;
}

void SH1106Menu::renderModal(const char *title, const char *message, const char *buttonText, bool isSelected, bool isError) {
    clear();

    int modalPadding = 3;
    int modalWidth = displayWidth - (2 * modalPadding);
    int titleHeight = 14;
    int buttonHeight = (buttonText != nullptr) ? 14 : 0;
    int messageMaxHeight = displayHeight - titleHeight - buttonHeight - (4 * modalPadding);

    int messageHeight = 0;
    String tempMessage = String(message);
    int messageLen = tempMessage.length();
    int lineHeight = 11;
    int lineWidth = modalWidth - (2 * modalPadding);
    int lineStart = 0;
    int lineCount = 0;

    for (int i = 0; i <= messageLen; i++) {
        if (i == messageLen || message[i] == '\n' ||
            getStringWidth(tempMessage.substring(lineStart, i+1).c_str()) > lineWidth) {
            lineCount++;
            lineStart = i;
            if (i < messageLen && message[i] == '\n') {
                lineStart++;
            }
        }
    }

    messageHeight = lineCount * lineHeight;
    messageHeight = min(messageHeight, messageMaxHeight);

    int modalHeight = titleHeight + messageHeight + buttonHeight + (3 * modalPadding);
    int modalX = modalPadding;
    int modalY = (displayHeight - modalHeight) / 2;

    setColor(BLACK);
    fillRect(0, 0, displayWidth, displayHeight);
    setColor(BLACK);
    fillRect(modalX, modalY + titleHeight, modalWidth, modalHeight - titleHeight);
    setColor(WHITE);
    fillRect(modalX, modalY, modalWidth, titleHeight);
    setColor(BLACK);
    setTextAlignment(TEXT_ALIGN_CENTER);
    drawString(displayWidth / 2, modalY + 2, title);

    setColor(WHITE);
    setTextAlignment(TEXT_ALIGN_LEFT);

    lineStart = 0;
    int currentY = modalY + titleHeight + modalPadding;

    for (int i = 0; i <= messageLen; i++) {
        if (i == messageLen || message[i] == '\n' ||
            getStringWidth(tempMessage.substring(lineStart, i+1).c_str()) > lineWidth) {
            int endIdx = (i == messageLen || message[i] == '\n') ? i : i;

            if (currentY < modalY + messageMaxHeight) {
                String line = tempMessage.substring(lineStart, endIdx);
                drawString(modalX + modalPadding, currentY, line.c_str());
                currentY += lineHeight;
            }

            lineStart = i;
            if (i < messageLen && message[i] == '\n') {
                lineStart++;
            }
        }
    }

    if (buttonText != nullptr) {
        int buttonWidth = min(100, modalWidth - (2 * modalPadding));
        int buttonX = (displayWidth - buttonWidth) / 2;
        int buttonY = modalY + modalHeight - buttonHeight - modalPadding;

        if (isSelected) {
            setColor(WHITE);
            fillRoundedRect(buttonX, buttonY, buttonWidth, buttonHeight, 3);
            setColor(BLACK);
        } else {
            setColor(WHITE);
            drawRoundedRect(buttonX, buttonY, buttonWidth, buttonHeight, 3);
        }

        setTextAlignment(TEXT_ALIGN_CENTER);
        drawString(displayWidth / 2, buttonY + 2, buttonText);
    }

    if (isError) {
        setColor(WHITE);
        drawRect(modalX - 1, modalY - 1, modalWidth + 2, modalHeight + 2);
    }

    display();
}

void SH1106Menu::drawRoundedRect(int x, int y, int width, int height, int radius) {
    drawLine(x + radius, y, x + width - radius, y);
    drawLine(x + radius, y + height, x + width - radius, y + height);
    drawLine(x, y + radius, x, y + height - radius);
    drawLine(x + width, y + radius, x + width, y + height - radius);

    drawCircleQuads(x + radius, y + radius, radius, 1);
    drawCircleQuads(x + width - radius, y + radius, radius, 2);
    drawCircleQuads(x + width - radius, y + height - radius, radius, 4);
    drawCircleQuads(x + radius, y + height - radius, radius, 8);
}

void SH1106Menu::fillRoundedRect(int x, int y, int width, int height, int radius) {
    fillRect(x + radius, y, width - 2 * radius, height);
    fillRect(x, y + radius, width, height - 2 * radius);

    fillCircle(x + radius, y + radius, radius);
    fillCircle(x + width - radius, y + radius, radius);
    fillCircle(x + width - radius, y + height - radius, radius);
    fillCircle(x + radius, y + height - radius, radius);
}