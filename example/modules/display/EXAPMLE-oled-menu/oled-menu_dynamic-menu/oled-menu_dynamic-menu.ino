#include "oled-menu.h"

#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4
#define BACK_PIN 5

OledMenu display(128, 64);
MenuCursor menuCursor = {false, false, false, false, true};

MenuProperties *mainMenu;
MenuProperties *dynamicMenu;
MenuProperties *statsMenu;

// Global variables for demo purposes
int counter = 0;
int sensorValue = 512;
unsigned long updateTimer = 0;
unsigned long statsTimer = 0;
bool autoIncrement = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  display.initialize(true);
  
  // Create main menu
  mainMenu = display.createMenu(4, "Dynamic Menu", "Auto Increment", "Reset Counter", "Stats");
  
  // Create dynamic menu with placeholder content
  dynamicMenu = display.createEmptyMenu(6);
  updateDynamicMenu(); // Initially populate menu
  
  // Create stats menu with placeholder content
  statsMenu = display.createEmptyMenu(5);
  updateStatsMenu(); // Initially populate menu
  
  // Welcome screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15, 10);
  display.println("Dynamic OLED Menu");
  display.setCursor(15, 25);
  display.println("This menu updates");
  display.setCursor(15, 35);
  display.println("content dynamically");
  display.setCursor(15, 50);
  display.println("Press any button");
  display.display();
  
  waitForButton();
}

void loop() {
  // Update dynamic content
  if (millis() - updateTimer >= 2000) { // Update every 2 seconds
    updateTimer = millis();
    
    // Auto increment counter if enabled
    if (autoIncrement) {
      counter++;
    }
    
    // Simulate a changing sensor value
    sensorValue = 400 + random(0, 200);
    
    // Update dynamic menus with new values
    updateDynamicMenu();
    updateStatsMenu();
  }
  
  display.onListen(&menuCursor, listenButtons);
  
  // Display main menu
  display.showMenu(mainMenu);
  
  // Handle main menu selections
  display.onSelect(mainMenu, "Dynamic Menu", openDynamicMenu);
  display.onSelect(mainMenu, "Auto Increment", toggleAutoIncrement);
  display.onSelect(mainMenu, "Reset Counter", resetCounter);
  display.onSelect(mainMenu, "Stats", openStatsMenu);
  
  // Handle dynamic menu
  if (strcmp(mainMenu->option, "Dynamic Menu") == 0) {
    display.showMenu(dynamicMenu);
    
    display.onSelect(dynamicMenu, "Counter ++", incrementCounter);
    display.onSelect(dynamicMenu, "Counter --", decrementCounter);
    display.onSelect(dynamicMenu, "Sensor Demo", sensorDemo);
    display.onSelect(dynamicMenu, "Time Demo", timeDemo);
    display.onSelect(dynamicMenu, "Progress Demo", progressDemo);
    display.onSelect(dynamicMenu, "Back", backToMain);
  }
  
  // Handle stats menu
  if (strcmp(mainMenu->option, "Stats") == 0) {
    // Update stat values more frequently in this view
    if (millis() - statsTimer >= 500) {
      statsTimer = millis();
      sensorValue = 400 + random(0, 200);
      updateStatsMenu();
    }
    
    display.showMenu(statsMenu);
    display.onSelect(statsMenu, "Back", backToMain);
  }
  
  delay(10);
}

void updateDynamicMenu() {
  // Update menu item texts with current values
  char buffer[24];
  
  sprintf(buffer, "Counter ++   [%d]", counter);
  display.formatMenu(dynamicMenu, 0, buffer);
  
  sprintf(buffer, "Counter --   [%d]", counter);
  display.formatMenu(dynamicMenu, 1, buffer);
  
  sprintf(buffer, "Sensor Demo  [%d]", sensorValue);
  display.formatMenu(dynamicMenu, 2, buffer);
  
  unsigned long seconds = millis() / 1000;
  sprintf(buffer, "Time Demo    [%lu s]", seconds);
  display.formatMenu(dynamicMenu, 3, buffer);
  
  display.formatMenu(dynamicMenu, 4, "Progress Demo");
  display.formatMenu(dynamicMenu, 5, "Back");
}

void updateStatsMenu() {
  char buffer[24];
  
  sprintf(buffer, "Counter: %d", counter);
  display.formatMenu(statsMenu, 0, buffer);
  
  sprintf(buffer, "Sensor: %d", sensorValue);
  display.formatMenu(statsMenu, 1, buffer);
  
  unsigned long uptime = millis() / 1000;
  sprintf(buffer, "Uptime: %lu s", uptime);
  display.formatMenu(statsMenu, 2, buffer);
  
  display.formatMenu(statsMenu, 3, autoIncrement ? "Auto Inc: ON" : "Auto Inc: OFF");
  display.formatMenu(statsMenu, 4, "Back");
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

// Menu callbacks
void openDynamicMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Dynamic Menu");
  display.println("Current count: " + String(counter));
  display.display();
  delay(500);
}

void toggleAutoIncrement() {
  autoIncrement = !autoIncrement;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Auto Increment");
  display.setCursor(0, 16);
  display.print("Status: ");
  display.println(autoIncrement ? "ON" : "OFF");
  display.println("Counter will " + String(autoIncrement ? "" : "not ") + "increase");
  display.println("automatically.");
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void resetCounter() {
  counter = 0;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Counter Reset");
  display.setCursor(0, 16);
  display.println("Counter value set to 0");
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void openStatsMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Statistics");
  display.display();
  delay(500);
}

void incrementCounter() {
  counter++;
  updateDynamicMenu();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Counter Incremented");
  display.setCursor(0, 16);
  display.print("New value: ");
  display.println(counter);
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void decrementCounter() {
  counter--;
  updateDynamicMenu();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Counter Decremented");
  display.setCursor(0, 16);
  display.print("New value: ");
  display.println(counter);
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void sensorDemo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Sensor Reading Demo");
  
  // Draw a blank chart
  display.drawLine(0, 63, 127, 63, WHITE); // X axis
  display.drawLine(0, 16, 0, 63, WHITE);   // Y axis
  
  int values[128];
  int x = 0;
  
  // Show realtime sensor sampling
  while (!buttonPressed() && x < 128) {
    // Simulate sensor reading (replace with actual sensor)
    int newValue = 400 + random(0, 200);
    values[x] = newValue;
    
    // Map sensor value to display height (400-600 -> 63-16)
    int displayValue = map(newValue, 400, 600, 63, 16);
    
    // Clear value display area
    display.fillRect(50, 0, 78, 15, BLACK);
    
    // Update value display
    display.setCursor(50, 0);
    display.print("Value: ");
    display.print(newValue);
    
    // Draw sensor reading
    display.drawPixel(x, displayValue, WHITE);
    display.display();
    
    delay(50);
    x++;
  }
  
  menuCursor.back = true;
}

void timeDemo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Time Demo");
  
  unsigned long startTime = millis();
  unsigned long elapsedTime;
  
  // Show a 10-second countdown
  while (!buttonPressed()) {
    elapsedTime = (millis() - startTime) / 1000;
    
    if (elapsedTime >= 10) {
      break;
    }
    
    display.fillRect(0, 16, 128, 48, BLACK);
    
    display.setCursor(0, 16);
    display.print("Elapsed: ");
    display.print(elapsedTime);
    display.println(" s");
    
    display.setCursor(0, 32);
    display.print("Remaining: ");
    display.print(10 - elapsedTime);
    display.println(" s");
    
    // Draw progress bar
    display.drawRect(0, 50, 128, 10, WHITE);
    display.fillRect(0, 50, (elapsedTime * 128) / 10, 10, WHITE);
    
    display.display();
    delay(100);
  }
  
  display.fillRect(0, 16, 128, 48, BLACK);
  display.setCursor(0, 30);
  display.println("Demo complete!");
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void progressDemo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Progress Bar Demo");
  display.display();
  
  // Horizontal progress bar
  display.setCursor(0, 16);
  display.println("Horizontal:");
  display.drawRect(0, 26, 128, 10, WHITE);
  display.display();
  
  for (int i = 0; i <= 128; i += 2) {
    display.fillRect(0, 26, i, 10, WHITE);
    
    // Show percentage
    display.fillRect(50, 40, 30, 8, BLACK);
    display.setCursor(50, 40);
    display.print((i * 100) / 128);
    display.print("%");
    
    display.display();
    
    if (buttonPressed()) {
      break;
    }
    
    delay(30);
  }
  
  // Vertical progress bar
  display.setCursor(80, 16);
  display.println("Vertical:");
  display.drawRect(110, 16, 10, 48, WHITE);
  display.display();
  
  for (int i = 0; i <= 48; i += 1) {
    display.fillRect(110, 64 - i, 10, i, WHITE);
    display.display();
    
    if (buttonPressed()) {
      break;
    }
    
    delay(30);
  }
  
  display.setCursor(0, 50);
  display.println("Demo complete!");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void backToMain(MenuCursor *cursor) {
  cursor->back = true;
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