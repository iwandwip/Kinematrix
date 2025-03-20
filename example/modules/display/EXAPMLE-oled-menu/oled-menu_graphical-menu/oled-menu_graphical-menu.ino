#include "oled-menu.h"

#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4
#define BACK_PIN 5

OledMenu display(128, 64);
MenuCursor menuCursor = {false, false, false, false, true};

MenuProperties *mainMenu;

// Icon data - 16x16 pixels
static const unsigned char PROGMEM icon_settings[] = {
  0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x0F, 0xF0, 0x0C, 0x30, 0x0C, 0x30,
  0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0C, 0x30, 0x0C, 0x30,
  0x0F, 0xF0, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM icon_graph[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x04, 0x80, 0x06, 0x80, 0x07, 0x80,
  0x05, 0x80, 0x05, 0x80, 0x7D, 0x80, 0x55, 0x80, 0x55, 0x80, 0x55, 0x80,
  0x55, 0xBE, 0x7F, 0x80, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM icon_clock[] = {
  0x00, 0x00, 0x03, 0xC0, 0x0C, 0x30, 0x10, 0x08, 0x20, 0x04, 0x20, 0x84,
  0x40, 0x42, 0x40, 0x22, 0x40, 0x02, 0x40, 0x02, 0x20, 0x04, 0x20, 0x04,
  0x10, 0x08, 0x0C, 0x30, 0x03, 0xC0, 0x00, 0x00
};

static const unsigned char PROGMEM icon_info[] = {
  0x00, 0x00, 0x03, 0xC0, 0x0C, 0x30, 0x10, 0x08, 0x20, 0x04, 0x23, 0xC4,
  0x23, 0xC4, 0x20, 0x04, 0x20, 0x04, 0x21, 0x84, 0x21, 0x84, 0x21, 0x84,
  0x20, 0x04, 0x10, 0x08, 0x0C, 0x30, 0x03, 0xC0
};

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  display.initialize(true);
  display.setDisplayParams(18, 8, 10, 3); // Taller items for icons
  
  // Create main menu
  mainMenu = display.createMenu(4, "Settings", "Charts", "Clock", "Info");
  
  // Show welcome screen with animation
  showWelcomeScreen();
}

void loop() {
  display.onListen(&menuCursor, listenButtons);
  
  // Show custom graphical menu instead of standard text menu
  showGraphicalMenu();
  
  // Handle menu options
  display.onSelect(mainMenu, "Settings", settingsCallback);
  display.onSelect(mainMenu, "Charts", chartsCallback);
  display.onSelect(mainMenu, "Clock", clockCallback);
  display.onSelect(mainMenu, "Info", infoCallback);
  
  delay(10);
}

void listenButtons() {
  menuCursor.up = false;
  menuCursor.down = false;
  menuCursor.select = false;
  menuCursor.back = false;
  
  if (digitalRead(UP_PIN) == LOW) {
    menuCursor.up = true;
    delay(200);
  }
  
  if (digitalRead(DOWN_PIN) == LOW) {
    menuCursor.down = true;
    delay(200);
  }
  
  if (digitalRead(SELECT_PIN) == LOW) {
    menuCursor.select = true;
    delay(200);
  }
  
  if (digitalRead(BACK_PIN) == LOW) {
    menuCursor.back = true;
    delay(200);
  }
}

void showWelcomeScreen() {
  // Animation parameters
  int logoSize = 20;
  int centerX = display.width() / 2;
  int centerY = display.height() / 2;
  
  display.clearDisplay();
  
  // Draw growing circle
  for (int i = 0; i < logoSize; i++) {
    display.clearDisplay();
    display.drawCircle(centerX, centerY, i, WHITE);
    display.display();
    delay(50);
  }
  
  // Draw cross inside
  display.drawLine(centerX - logoSize/2, centerY, centerX + logoSize/2, centerY, WHITE);
  display.drawLine(centerX, centerY - logoSize/2, centerX, centerY + logoSize/2, WHITE);
  display.display();
  delay(500);
  
  // Draw text
  display.setCursor(centerX - 40, centerY + logoSize + 5);
  display.println("Graphical Menu");
  display.display();
  delay(1000);
  
  // Fade out
  for (int i = 0; i < 5; i++) {
    display.invertDisplay(true);
    delay(100);
    display.invertDisplay(false);
    delay(100);
  }
}

void showGraphicalMenu() {
  // Process cursor changes first
  display.onCursor(mainMenu);
  
  display.clearDisplay();
  
  // Draw title bar
  display.fillRect(0, 0, display.width(), 10, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 1);
  display.print("GRAPHICAL MENU");
  display.setTextColor(WHITE);
  
  // Draw menu items
  for (int i = 0; i < min(3, mainMenu->len - mainMenu->select); i++) {
    int itemY = 13 + i * 18;
    bool isSelected = (mainMenu->index == mainMenu->select + i);
    
    // Draw selection box
    if (isSelected) {
      display.fillRoundRect(0, itemY - 1, display.width(), 17, 2, WHITE);
      display.setTextColor(BLACK);
    } else {
      display.drawRoundRect(0, itemY - 1, display.width(), 17, 2, WHITE);
      display.setTextColor(WHITE);
    }
    
    // Draw icon
    switch (mainMenu->select + i) {
      case 0: // Settings
        display.drawBitmap(4, itemY, icon_settings, 16, 16, isSelected ? BLACK : WHITE);
        break;
      case 1: // Charts
        display.drawBitmap(4, itemY, icon_graph, 16, 16, isSelected ? BLACK : WHITE);
        break;
      case 2: // Clock
        display.drawBitmap(4, itemY, icon_clock, 16, 16, isSelected ? BLACK : WHITE);
        break;
      case 3: // Info
        display.drawBitmap(4, itemY, icon_info, 16, 16, isSelected ? BLACK : WHITE);
        break;
    }
    
    // Draw text
    display.setCursor(24, itemY + 4);
    display.print(mainMenu->text[mainMenu->select + i]);
  }
  
  // Draw scrollbar if needed
  if (mainMenu->len > 3) {
    int sbHeight = display.height() - 14;
    int sbThumbSize = max(8, sbHeight * 3 / mainMenu->len);
    int sbThumbPos = map(mainMenu->select, 0, mainMenu->len - 3, 0, sbHeight - sbThumbSize);
    
    display.drawRect(display.width() - 4, 13, 4, sbHeight, WHITE);
    display.fillRect(display.width() - 4, 13 + sbThumbPos, 4, sbThumbSize, WHITE);
  }
  
  // Draw navigation hints
  display.drawLine(10, 63, 20, 56, WHITE); // Up arrow
  display.drawLine(20, 56, 30, 63, WHITE);
  
  display.fillRect(50, 56, 28, 8, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(54, 57);
  display.print("SELECT");
  
  display.drawLine(100, 56, 110, 63, WHITE); // Down arrow
  display.drawLine(110, 63, 120, 56, WHITE);
  
  display.display();
}

void settingsCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Draw icon enlarged
  display.drawBitmap(56, 5, icon_settings, 16, 16, WHITE);
  
  // Draw header
  display.drawFastHLine(0, 24, display.width(), WHITE);
  display.setCursor(40, 26);
  display.println("SETTINGS");
  display.drawFastHLine(0, 34, display.width(), WHITE);
  
  // Draw settings items with toggle switches
  display.setCursor(5, 40);
  display.print("WiFi:");
  drawToggleSwitch(60, 40, true);
  
  display.setCursor(5, 50);
  display.print("Sound:");
  drawToggleSwitch(60, 50, false);
  
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void chartsCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Draw icon enlarged
  display.drawBitmap(56, 5, icon_graph, 16, 16, WHITE);
  
  // Draw header
  display.drawFastHLine(0, 24, display.width(), WHITE);
  display.setCursor(45, 26);
  display.println("CHARTS");
  display.drawFastHLine(0, 34, display.width(), WHITE);
  
  // Draw sample bar chart
  int barValues[5] = {10, 25, 15, 30, 20};
  int barWidth = 10;
  int barGap = 5;
  int baseY = 60;
  
  for (int i = 0; i < 5; i++) {
    int barHeight = barValues[i];
    int barX = 30 + (i * (barWidth + barGap));
    display.fillRect(barX, baseY - barHeight, barWidth, barHeight, WHITE);
  }
  
  // Draw axis
  display.drawFastHLine(20, baseY, 100, WHITE);
  display.drawFastVLine(20, 35, 25, WHITE);
  
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void clockCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Draw icon enlarged
  display.drawBitmap(56, 5, icon_clock, 16, 16, WHITE);
  
  // Draw header
  display.drawFastHLine(0, 24, display.width(), WHITE);
  display.setCursor(45, 26);
  display.println("CLOCK");
  display.drawFastHLine(0, 34, display.width(), WHITE);
  
  // Get start time reference
  unsigned long startTime = millis();
  unsigned long elapsedSeconds = 0;
  int lastSecond = -1;
  
  // Draw simple analog clock face
  int clockCenterX = 64;
  int clockCenterY = 48;
  int clockRadius = 20;
  
  display.drawCircle(clockCenterX, clockCenterY, clockRadius, WHITE);
  
  // Draw clock markers
  for (int i = 0; i < 12; i++) {
    float angle = i * 30 * 3.14159 / 180;
    int markerX = clockCenterX + sin(angle) * clockRadius;
    int markerY = clockCenterY - cos(angle) * clockRadius;
    display.drawPixel(markerX, markerY, WHITE);
  }
  
  display.display();
  
  // Clock animation loop
  while (!buttonPressed()) {
    elapsedSeconds = (millis() - startTime) / 1000;
    
    // Only update when second changes
    if (int(elapsedSeconds) != lastSecond) {
      lastSecond = elapsedSeconds;
      
      // Clear previous hands
      display.fillRect(clockCenterX - clockRadius + 2, 
                       clockCenterY - clockRadius + 2, 
                       clockRadius * 2 - 4, 
                       clockRadius * 2 - 4, BLACK);
      
      // Draw second hand
      float secondAngle = (elapsedSeconds % 60) * 6 * 3.14159 / 180;
      int secondX = clockCenterX + sin(secondAngle) * (clockRadius - 2);
      int secondY = clockCenterY - cos(secondAngle) * (clockRadius - 2);
      display.drawLine(clockCenterX, clockCenterY, secondX, secondY, WHITE);
      
      // Draw minute hand
      float minuteAngle = (elapsedSeconds % 3600) * 0.1 * 3.14159 / 180;
      int minuteX = clockCenterX + sin(minuteAngle) * (clockRadius - 6);
      int minuteY = clockCenterY - cos(minuteAngle) * (clockRadius - 6);
      display.drawLine(clockCenterX, clockCenterY, minuteX, minuteY, WHITE);
      
      // Draw hour hand
      float hourAngle = (elapsedSeconds % 43200) * 0.00833 * 3.14159 / 180;
      int hourX = clockCenterX + sin(hourAngle) * (clockRadius - 10);
      int hourY = clockCenterY - cos(hourAngle) * (clockRadius - 10);
      display.drawLine(clockCenterX, clockCenterY, hourX, hourY, WHITE);
      
      // Draw center dot
      display.fillCircle(clockCenterX, clockCenterY, 2, WHITE);
      
      // Update time text
      display.fillRect(20, 68, 88, 8, BLACK);
      display.setCursor(20, 68);
      
      int hours = (elapsedSeconds / 3600) % 12;
      if (hours == 0) hours = 12;
      int minutes = (elapsedSeconds / 60) % 60;
      int seconds = elapsedSeconds % 60;
      
      // Format time as HH:MM:SS
      display.print(hours < 10 ? " " : "");
      display.print(hours);
      display.print(":");
      display.print(minutes < 10 ? "0" : "");
      display.print(minutes);
      display.print(":");
      display.print(seconds < 10 ? "0" : "");
      display.print(seconds);
      
      display.display();
    }
    
    delay(50);
  }
  
  menuCursor.back = true;
}

void infoCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Draw icon enlarged
  display.drawBitmap(56, 5, icon_info, 16, 16, WHITE);
  
  // Draw header
  display.drawFastHLine(0, 24, display.width(), WHITE);
  display.setCursor(50, 26);
  display.println("INFO");
  display.drawFastHLine(0, 34, display.width(), WHITE);
  
  // Draw info in fancy box
  display.drawRoundRect(10, 38, 108, 24, 4, WHITE);
  display.setCursor(15, 42);
  display.println("Graphical Menu Demo");
  display.setCursor(15, 52);
  display.println("Version 1.0 (2025)");
  
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void drawToggleSwitch(int x, int y, bool state) {
  // Draw switch background
  display.drawRoundRect(x, y, 20, 8, 4, WHITE);
  
  if (state) {
    // ON state - filled right side and knob on right
    display.fillRoundRect(x + 10, y, 10, 8, 4, WHITE);
    display.fillCircle(x + 14, y + 4, 3, WHITE);
    display.fillCircle(x + 6, y + 4, 3, BLACK);
    display.drawCircle(x + 6, y + 4, 3, WHITE);
  } else {
    // OFF state - filled left side and knob on left
    display.fillRoundRect(x, y, 10, 8, 4, WHITE);
    display.fillCircle(x + 6, y + 4, 3, WHITE);
    display.fillCircle(x + 14, y + 4, 3, BLACK);
    display.drawCircle(x + 14, y + 4, 3, WHITE);
  }
}

void waitForButton() {
  while (!buttonPressed()) {
    delay(10);
  }
  delay(200);
}

bool buttonPressed() {
  return (digitalRead(UP_PIN) == LOW || 
          digitalRead(DOWN_PIN) == LOW || 
          digitalRead(SELECT_PIN) == LOW || 
          digitalRead(BACK_PIN) == LOW);
}