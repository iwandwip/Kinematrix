#include "sh1106-menu.h"

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