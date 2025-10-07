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

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  lcd.initialize();
  lcd.setLen(LCD_COLS, LCD_ROWS);
  
  mainMenu = lcd.createMenu(4, "Option 1", "Option 2", "Option 3", "Option 4");
}

void loop() {
  lcd.onListen(&menuCursor, listenButtons);
  lcd.showMenu(mainMenu);
  
  lcd.onSelect(mainMenu, "Option 1", option1Callback);
  lcd.onSelect(mainMenu, "Option 2", option2Callback);
  lcd.onSelect(mainMenu, "Option 3", option3Callback);
  lcd.onSelect(mainMenu, "Option 4", option4Callback);
  
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

void option1Callback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Option 1 Selected");
  delay(2000);
  menuCursor.back = true;
}

void option2Callback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Option 2 Selected");
  delay(2000);
  menuCursor.back = true;
}

void option3Callback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Option 3 Selected");
  delay(2000);
  menuCursor.back = true;
}

void option4Callback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Option 4 Selected");
  delay(2000);
  menuCursor.back = true;
}