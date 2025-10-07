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
MenuProperties *dynamicMenu;

int counter = 0;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  lcd.initialize();
  lcd.setLen(LCD_COLS, LCD_ROWS);
  
  mainMenu = lcd.createMenu(3, "Dynamic Menu", "Counter", "Reset");
  dynamicMenu = lcd.createEmptyMenu(5);
  
  updateDynamicMenu();
}

void loop() {
  lcd.onListen(&menuCursor, listenButtons);
  
  lcd.showMenu(mainMenu);
  
  lcd.onSelect(mainMenu, "Dynamic Menu", openDynamicMenu);
  lcd.onSelect(mainMenu, "Counter", incrementCounter);
  lcd.onSelect(mainMenu, "Reset", resetCounter);
  
  if (strcmp(mainMenu->option, "Dynamic Menu") == 0) {
    if (millis() - lastUpdate > 5000) {
      updateDynamicMenu();
      lastUpdate = millis();
    }
    
    lcd.showMenu(dynamicMenu);
    
    for (int i = 0; i < 5; i++) {
      char option[MAX_BUFF_LEN];
      sprintf(option, "Item %d", i+1);
      lcd.onSelect(dynamicMenu, option, itemCallback);
    }
    
    lcd.onSelect(dynamicMenu, "Back", backToMain);
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

void updateDynamicMenu() {
  for (int i = 0; i < 4; i++) {
    char buffer[MAX_BUFF_LEN];
    sprintf(buffer, "Item %d", i+1);
    lcd.formatMenu(dynamicMenu, i, buffer);
  }
  lcd.formatMenu(dynamicMenu, 4, "Back");
}

void openDynamicMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dynamic Menu");
  lcd.setCursor(0, 1);
  lcd.print("Count: ");
  lcd.print(counter);
  delay(1000);
}

void incrementCounter() {
  counter++;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Counter: ");
  lcd.print(counter);
  delay(1000);
  menuCursor.back = true;
}

void resetCounter() {
  counter = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Counter Reset");
  delay(1000);
  menuCursor.back = true;
}

void itemCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Selected:");
  lcd.setCursor(0, 1);
  lcd.print(dynamicMenu->option);
  delay(1500);
  menuCursor.back = true;
}

void backToMain(MenuCursor *cursor) {
  cursor->back = true;
}