#include "oled-menu.h"

#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4
#define BACK_PIN 5

OledMenu display(128, 64);
MenuCursor menuCursor = {false, false, false, false, true};
MenuProperties *mainMenu;

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  display.initialize(true);
  
  mainMenu = display.createMenu(5, "Option 1", "Option 2", "Option 3", "Option 4", "Option 5");
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("OLED Menu Demo");
  display.println("Use buttons to navigate");
  display.display();
  delay(2000);
}

void loop() {
  display.onListen(&menuCursor, listenButtons);
  display.showMenu(mainMenu);
  
  display.onSelect(mainMenu, "Option 1", option1Callback);
  display.onSelect(mainMenu, "Option 2", option2Callback);
  display.onSelect(mainMenu, "Option 3", option3Callback);
  display.onSelect(mainMenu, "Option 4", option4Callback);
  display.onSelect(mainMenu, "Option 5", option5Callback);
  
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
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Option 1 Selected");
  display.println("This is a simple demo");
  display.println("of the OLED Menu system");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void option2Callback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Option 2 Selected");
  display.println("Press any button");
  display.println("to go back");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void option3Callback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Option 3 Selected");
  
  for (int i = 0; i < 10; i++) {
    display.setCursor(0, 16);
    display.print("Counter: ");
    display.println(i);
    display.display();
    delay(500);
    if (buttonPressed()) break;
  }
  
  menuCursor.back = true;
}

void option4Callback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Option 4 Selected");
  
  display.drawRect(10, 20, 108, 30, WHITE);
  display.fillRect(20, 30, 88, 10, WHITE);
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void option5Callback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Option 5 Selected");
  
  display.setCursor(0, 20);
  display.println("This is the last option");
  display.println("Press any button to return");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void waitForButton() {
  while (!buttonPressed()) {
    delay(10);
  }
}

bool buttonPressed() {
  return (digitalRead(UP_PIN) == LOW || 
          digitalRead(DOWN_PIN) == LOW || 
          digitalRead(SELECT_PIN) == LOW || 
          digitalRead(BACK_PIN) == LOW);
}