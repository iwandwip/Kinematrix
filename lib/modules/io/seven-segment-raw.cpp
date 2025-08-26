/*
 *  seven-segment-raw.cpp
 *
 *  Raw 7-segment display library - Simple but powerful
 *  Created on: 2024. 12. 18
 */

#include "seven-segment-raw.h"

const uint8_t SevenSegmentRaw::digitPatterns[18] = {
        0b11111100,  // 0: ABCDEF__
        0b01100000,  // 1: _BC_____
        0b11011010,  // 2: AB_DE_G_
        0b11110010,  // 3: ABCD__G_
        0b01100110,  // 4: _BC___GF
        0b10110110,  // 5: A_CD_FG_
        0b10111110,  // 6: A_CDEFG_
        0b11100000,  // 7: ABC_____
        0b11111110,  // 8: ABCDEFG_
        0b11110110,  // 9: ABCD_FG_
        0b11101110,  // A: ABC__FG_
        0b00111110,  // b: __CDEFG_
        0b10011100,  // C: A__DEF__
        0b01111010,  // d: _BCD_G__
        0b10011110,  // E: A__DEFG_
        0b10001110,  // F: A____FG_
        0b00000000,  // 16: Blank (all segments off)
        0b00000010   // 17: Dash (G segment only)
};

SevenSegmentRaw* SevenSegmentRaw::instance = nullptr;

SevenSegmentRaw::SevenSegmentRaw(uint8_t segA, uint8_t segB, uint8_t segC, uint8_t segD,
                                 uint8_t segE, uint8_t segF, uint8_t segG, uint8_t segDP,
                                 uint8_t *digitPins, uint8_t numDigits,
                                 bool segmentActiveHigh, bool digitActiveHigh) {
    // Bounds checking
    if (numDigits == 0) numDigits = 1;
    if (numDigits > MAX_DIGITS) numDigits = MAX_DIGITS;
    
    this->numDigits = numDigits;
    this->segmentActiveHigh = segmentActiveHigh;
    this->digitActiveHigh = digitActiveHigh;
    this->brightness = 50;
    this->lastRefresh = 0;
    this->currentDigit = 0;
    this->paddingMode = PADDING_BLANKS;
    this->timingMode = TIMING_MILLIS;
    this->dpPosition = 255;
    this->decimals = 0;
    
    // Initialize instance variables
    this->currentNumber = 0;
    this->currentHexNumber = 0;
    this->currentFloat = 0.0;
    this->currentDecimals = 0;
    memset(this->currentText, 0, sizeof(this->currentText))

    segmentPins = new uint8_t[8];
    segmentPins[0] = segA;
    segmentPins[1] = segB;
    segmentPins[2] = segC;
    segmentPins[3] = segD;
    segmentPins[4] = segE;
    segmentPins[5] = segF;
    segmentPins[6] = segG;
    segmentPins[7] = segDP;

    this->digitPins = new uint8_t[numDigits];
    for (uint8_t i = 0; i < numDigits; i++) {
        this->digitPins[i] = digitPins[i];
        this->displayBuffer[i] = PATTERN_BLANK;
    }
    
    instance = this;
}

SevenSegmentRaw::~SevenSegmentRaw() {
    setTimingMode(TIMING_MILLIS); // Stop any timers
    delete[] segmentPins;
    delete[] digitPins;
    if (instance == this) instance = nullptr;
}

void SevenSegmentRaw::begin() {
    for (uint8_t i = 0; i < 8; i++) {
        pinMode(segmentPins[i], OUTPUT);
    }
    for (uint8_t i = 0; i < numDigits; i++) {
        pinMode(digitPins[i], OUTPUT);
    }
    clearAll();
}

void SevenSegmentRaw::setSegment(uint8_t pattern) {
    for (uint8_t i = 0; i < 8; i++) {
        bool segmentOn = bitRead(pattern, 7 - i);
        if (!segmentActiveHigh) segmentOn = !segmentOn;
        digitalWrite(segmentPins[i], segmentOn);
    }
}

void SevenSegmentRaw::selectDigit(uint8_t digit) {
    for (uint8_t i = 0; i < numDigits; i++) {
        bool digitOn = (i == digit);
        if (!digitActiveHigh) digitOn = !digitOn;
        digitalWrite(digitPins[i], digitOn);
    }
}

void SevenSegmentRaw::clearAll() {
    setSegment(0x00);
    for (uint8_t i = 0; i < numDigits; i++) {
        digitalWrite(digitPins[i], digitActiveHigh ? LOW : HIGH);
        displayBuffer[i] = PATTERN_BLANK;
    }
}

// Timing Mode Methods
void SevenSegmentRaw::setTimingMode(TimingMode mode) {
    if (timingMode == mode) return;
    
    // Disable Timer1 if currently active
    if (timingMode == TIMING_TIMER1) {
        Timer1.detachInterrupt();
        Timer1.stop();
    }
    
    timingMode = mode;
    
    // Enable Timer1 if switching to Timer1 mode
    if (mode == TIMING_TIMER1) {
        updateTimer();
        Timer1.attachInterrupt(timerISR);
    }
}

TimingMode SevenSegmentRaw::getTimingMode() {
    return timingMode;
}

void SevenSegmentRaw::updateTimer() {
    if (numDigits == 0) return; // Prevent division by zero
    uint32_t period = ((100 - brightness) * 10) / numDigits;
    if (period < MIN_REFRESH_PERIOD) period = MIN_REFRESH_PERIOD;
    if (period > MAX_REFRESH_PERIOD) period = MAX_REFRESH_PERIOD;
    Timer1.initialize(period);
}

void SevenSegmentRaw::timerISR() {
    if (instance != nullptr) {
        instance->refreshDisplayISR();
    }
}

void SevenSegmentRaw::refreshDisplayISR() {
    selectDigit(currentDigit);
    uint8_t pattern = displayBuffer[currentDigit];
    uint8_t segmentData;
    
    if (pattern < 18) {
        segmentData = digitPatterns[pattern];
    } else {
        segmentData = digitPatterns[PATTERN_DASH];
    }
    
    // Add decimal point if needed (Timer1 mode support)
    if (currentDigit == dpPosition && decimals > 0) {
        segmentData |= DECIMAL_POINT_BIT;
    }
    
    setSegment(segmentData);
    currentDigit = (currentDigit + 1) % numDigits;
}

void SevenSegmentRaw::handleRefresh() {
    if (numDigits == 0) return; // Prevent division by zero
    
    switch (timingMode) {
        case TIMING_DELAY:
            // Blocking delay mode - display each digit with delay
            for (uint8_t i = 0; i < numDigits; i++) {
                selectDigit(i);
                uint8_t pattern = displayBuffer[i] < 18 ? digitPatterns[displayBuffer[i]] : digitPatterns[PATTERN_DASH];
                if (i == dpPosition && decimals > 0) {
                    pattern |= DECIMAL_POINT_BIT;
                }
                setSegment(pattern);
                delay((100 - brightness) / numDigits + 1); // +1 to prevent zero delay
                clearAll();
            }
            break;
            
        case TIMING_MILLIS:
            // Non-blocking millis mode
            uint32_t interval = (100 - brightness) / numDigits;
            if (interval == 0) interval = 1; // Prevent zero interval
            if (millis() - lastRefresh >= interval) {
                refreshDisplayISR();
                lastRefresh = millis();
            }
            break;
            
        case TIMING_TIMER1:
            // Timer1 handles refresh automatically in ISR
            break;
    }
}

void SevenSegmentRaw::displayNumber(int32_t number) {
    static uint8_t digits[MAX_DIGITS];
    bool negative = false;

    if (number != currentNumber) {
        currentNumber = number;
        negative = number < 0;
        if (negative) number = -number;

        uint32_t maxNumber = 1;
        for (uint8_t i = 0; i < numDigits - (negative ? 1 : 0); i++) {
            maxNumber *= 10;
        }

        if (number >= maxNumber) {
            for (uint8_t i = 0; i < numDigits; i++) {
                digits[i] = PATTERN_DASH;
            }
        } else {
            for (uint8_t i = 0; i < numDigits; i++) {
                digits[numDigits - 1 - i] = number % 10;
                number /= 10;
            }

            // Apply padding mode
            for (uint8_t i = 0; i < numDigits; i++) {
                if (shouldPad(i, currentNumber)) {
                    switch (paddingMode) {
                        case PADDING_ZEROS:
                            digits[i] = 0;
                            break;
                        case PADDING_BLANKS:
                        case PADDING_NONE:
                            digits[i] = PATTERN_BLANK;
                            break;
                        case PADDING_DASHES:
                            digits[i] = PATTERN_DASH;
                            break;
                    }
                }
            }

            if (negative && numDigits > 0) {
                uint8_t pos = 0;
                while (pos < numDigits - 1 && (digits[pos] == 0 || digits[pos] == PATTERN_BLANK || digits[pos] == PATTERN_DASH)) pos++;
                if (pos > 0) digits[pos - 1] = PATTERN_DASH;
            }
        }
        
        // Update display buffer
        noInterrupts();
        for (uint8_t i = 0; i < numDigits; i++) {
            displayBuffer[i] = digits[i];
        }
        interrupts();
    }

    handleRefresh();
}

void SevenSegmentRaw::displayFloat(float number, uint8_t decimals) {
    if (decimals >= numDigits) decimals = numDigits - 1;
    if (number != currentFloat || decimals != currentDecimals) {
        currentFloat = number;
        currentDecimals = decimals;
        
        bool negative = number < 0;
        if (negative) number = -number;

        uint32_t multiplier = 1;
        for (uint8_t i = 0; i < decimals; i++) multiplier *= 10;

        int32_t intNumber = (int32_t) (number * multiplier + 0.5);

        static uint8_t digits[MAX_DIGITS];

        for (uint8_t i = 0; i < numDigits; i++) {
            digits[numDigits - 1 - i] = intNumber % 10;
            intNumber /= 10;
        }

        // Store decimal position for Timer1 mode
        noInterrupts();
        this->dpPosition = numDigits - 1 - decimals;
        this->decimals = decimals;
        for (uint8_t i = 0; i < numDigits; i++) {
            displayBuffer[i] = digits[i];
        }
        interrupts();
    }

    // Handle refresh for all timing modes (decimal point now works in Timer1 too)
    handleRefresh();
}

void SevenSegmentRaw::displayHex(uint16_t number) {
    static uint8_t digits[MAX_DIGITS];

    if (number != currentHexNumber) {
        currentHexNumber = number;
        for (uint8_t i = 0; i < numDigits; i++) {
            digits[numDigits - 1 - i] = number & 0x0F;
            number >>= 4;
        }
        
        // Update display buffer
        noInterrupts();
        for (uint8_t i = 0; i < numDigits; i++) {
            displayBuffer[i] = digits[i];
        }
        interrupts();
    }

    handleRefresh();
}

void SevenSegmentRaw::displayText(const char *text) {
    static uint8_t digits[MAX_DIGITS];

    if (strcmp(text, currentText) != 0) {
        strncpy(currentText, text, MAX_DIGITS);
        currentText[MAX_DIGITS] = '\0';

        for (uint8_t i = 0; i < numDigits; i++) {
            if (i < strlen(text)) {
                char c = text[i];
                if (c >= '0' && c <= '9') {
                    digits[i] = c - '0';
                } else if (c >= 'A' && c <= 'F') {
                    digits[i] = c - 'A' + 10;
                } else if (c >= 'a' && c <= 'f') {
                    digits[i] = c - 'a' + 10;
                } else if (c == '-') {
                    digits[i] = PATTERN_DASH;
                } else {
                    digits[i] = PATTERN_BLANK;
                }
            } else {
                digits[i] = PATTERN_BLANK;
            }
        }
        
        // Update display buffer
        noInterrupts();
        for (uint8_t i = 0; i < numDigits; i++) {
            displayBuffer[i] = digits[i];
        }
        interrupts();
    }

    handleRefresh();
}

void SevenSegmentRaw::setBrightness(uint8_t level) {
    if (level > 99) level = 99;
    if (level < 1) level = 1; // Prevent zero brightness causing issues
    brightness = level;
    if (timingMode == TIMING_TIMER1) updateTimer();
}

void SevenSegmentRaw::clear() {
    noInterrupts();
    for (uint8_t i = 0; i < numDigits; i++) {
        displayBuffer[i] = PATTERN_BLANK;
    }
    dpPosition = 255;
    decimals = 0;
    interrupts();
    
    // Reset instance variables
    currentNumber = 0;
    currentHexNumber = 0;
    currentFloat = 0.0;
    currentDecimals = 0;
    memset(currentText, 0, sizeof(currentText));
    
    if (timingMode != TIMING_TIMER1) {
        clearAll();
    }
}

void SevenSegmentRaw::refresh() {
    if (timingMode == TIMING_MILLIS) refreshDisplayISR(); // Manual refresh for millis mode
}

bool SevenSegmentRaw::shouldPad(uint8_t position, int32_t number) {
    if (number < 0) number = -number;
    if (numDigits == 0) return false;

    // Calculate minimum digits needed for this number
    uint8_t significantDigits = 1;
    int32_t temp = number;
    while (temp >= 10) {
        temp /= 10;
        significantDigits++;
    }

    // Position needs padding if it's a leading zero position
    return (position < numDigits - significantDigits);
}

void SevenSegmentRaw::setPaddingMode(PaddingMode mode) {
    paddingMode = mode;
}

PaddingMode SevenSegmentRaw::getPaddingMode() {
    return paddingMode;
}

void SevenSegmentRaw::testDigits(uint16_t delay_ms, uint8_t loops) {
    for (uint8_t loop = 0; loop < loops; loop++) {
        for (uint8_t digit = 0; digit < numDigits; digit++) {
            clearAll();
            selectDigit(digit);

            uint8_t displayValue = (digit + 1) % 10;
            setSegment(digitPatterns[displayValue]);

            delay(delay_ms);
        }
    }
    clearAll();
}

void SevenSegmentRaw::testSegments(uint8_t digit, uint16_t delay_ms) {
    if (digit >= numDigits) digit = 0;

    const uint8_t segmentMasks[8] = {
            0b10000000,  // A
            0b01000000,  // B
            0b00100000,  // C
            0b00010000,  // D
            0b00001000,  // E
            0b00000100,  // F
            0b00000010,  // G
            0b00000001   // DP
    };

    clearAll();
    selectDigit(digit);

    for (uint8_t seg = 0; seg < 8; seg++) {
        setSegment(segmentMasks[seg]);
        delay(delay_ms);
    }

    clearAll();
}

void SevenSegmentRaw::testAllSegments(uint16_t delay_ms) {
    const uint8_t segmentMasks[8] = {
            0b10000000,  // A
            0b01000000,  // B
            0b00100000,  // C
            0b00010000,  // D
            0b00001000,  // E
            0b00000100,  // F
            0b00000010,  // G
            0b00000001   // DP
    };

    for (uint8_t seg = 0; seg < 8; seg++) {
        for (uint8_t digit = 0; digit < numDigits; digit++) {
            digitalWrite(digitPins[digit], digitActiveHigh ? HIGH : LOW);
        }

        setSegment(segmentMasks[seg]);
        delay(delay_ms);

        for (uint8_t digit = 0; digit < numDigits; digit++) {
            digitalWrite(digitPins[digit], digitActiveHigh ? LOW : HIGH);
        }
    }

    clearAll();
}

void SevenSegmentRaw::runFullDiagnostics(uint16_t delay_ms) {
    testDigits(delay_ms, 2);
    delay(delay_ms * 2);

    for (uint8_t digit = 0; digit < numDigits; digit++) {
        testSegments(digit, delay_ms / 2);
        delay(delay_ms);
    }

    testAllSegments(delay_ms);
    delay(delay_ms);

    clearAll();
}