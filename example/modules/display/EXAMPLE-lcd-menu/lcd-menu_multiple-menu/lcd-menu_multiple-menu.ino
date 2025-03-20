#include "lcd-menu.h"

#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4
#define BACK_PIN 5

LcdMenu lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
MenuCursor menuCursor = {false, false, false, false, true};
MenuProperties *mainMenu;
MenuProperties *subMenu1;
MenuProperties *subMenu2;

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  lcd.initialize();
  lcd.setLen(LCD_COLS, LCD_ROWS);
  
  mainMenu = lcd.createMenu(3, "Settings", "Display", "About");
  subMenu1 = lcd.createMenu(3, "Brightness", "Contrast", "Back");
  subMenu2 = lcd.createMenu(3, "Mode 1", "Mode 2", "Back");
}

void loop() {
  lcd.onListen(&menuCursor, listenButtons);
  
  lcd.showMenu(mainMenu);
  
  lcd.onSelect(mainMenu, "Settings", openSubMenu1);
  lcd.onSelect(mainMenu, "Display", openSubMenu2);
  lcd.onSelect(mainMenu, "About", showAbout);
  
  if (strcmp(mainMenu->option, "Settings") == 0) {
    lcd.showMenu(subMenu1);
    lcd.onSelect(subMenu1, "Brightness", brightnessCallback);
    lcd.onSelect(subMenu1, "Contrast", contrastCallback);
    lcd.onSelect(subMenu1, "Back", backToMain);
  }
  
  if (strcmp(mainMenu->option, "Display") == 0) {
    lcd.showMenu(subMenu2);
    lcd.onSelect(subMenu2, "Mode 1", mode1Callback);
    lcd.onSelect(subMenu2, "Mode 2", mode2Callback);
    lcd.onSelect(subMenu2, "Back", backToMain);
  }
  
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

void openSubMenu1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Settings Menu");
  delay(500);
}

void openSubMenu2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Display Menu");
  delay(500);
}

void showAbout() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Menu System");
  lcd.setCursor(0, 1);
  lcd.print("Version 1.0");
  delay(2000);
  menuCursor.back = true;
}

void brightnessCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Brightness: 75%");
  delay(2000);
  menuCursor.back = true;
}

void contrastCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Contrast: 50%");
  delay(2000);
  menuCursor.back = true;
}

void mode1Callback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode 1 Selected");
  delay(2000);
  menuCursor.back = true;
}

void mode2Callback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode 2 Selected");
  delay(2000);
  menuCursor.back = true;
}

void backToMain(MenuCursor *cursor) {
  cursor->back = true;
}