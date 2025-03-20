#include "oled-menu.h"

#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4
#define BACK_PIN 5

OledMenu display(128, 64);
MenuCursor menuCursor = {false, false, false, false, true};

MenuProperties *mainMenu;
MenuProperties *settingsMenu;
MenuProperties *displayMenu;
MenuProperties *systemMenu;

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  display.initialize(true);
  display.setDisplayParams(12, 5, 8, 4); // Smaller item height to fit more items
  
  // Create main menu
  mainMenu = display.createMenu(4, "Settings", "Display", "System", "About");
  
  // Create submenu for Settings
  settingsMenu = display.createMenu(4, "Brightness", "Sound", "Network", "Back");
  
  // Create submenu for Display
  displayMenu = display.createMenu(4, "Contrast", "Flip", "Invert", "Back");
  
  // Create submenu for System
  systemMenu = display.createMenu(4, "Info", "Memory", "Reset", "Back");
  
  // Show welcome screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  display.println("OLED MENU");
  display.setTextSize(1);
  display.setCursor(20, 30);
  display.println("Multilevel Menu Demo");
  display.setCursor(20, 45);
  display.println("Press any button");
  display.display();
  
  waitForButton();
}

void loop() {
  display.onListen(&menuCursor, listenButtons);
  
  // Display main menu
  display.showMenu(mainMenu);
  
  // Handle menu options
  display.onSelect(mainMenu, "Settings", openSettingsMenu);
  display.onSelect(mainMenu, "Display", openDisplayMenu);
  display.onSelect(mainMenu, "System", openSystemMenu);
  display.onSelect(mainMenu, "About", showAbout);
  
  // Handle Settings submenu
  if (strcmp(mainMenu->option, "Settings") == 0) {
    display.showMenu(settingsMenu);
    
    display.onSelect(settingsMenu, "Brightness", brightnessCallback);
    display.onSelect(settingsMenu, "Sound", soundCallback);
    display.onSelect(settingsMenu, "Network", networkCallback);
    display.onSelect(settingsMenu, "Back", backToMain);
  }
  
  // Handle Display submenu
  if (strcmp(mainMenu->option, "Display") == 0) {
    display.showMenu(displayMenu);
    
    display.onSelect(displayMenu, "Contrast", contrastCallback);
    display.onSelect(displayMenu, "Flip", flipCallback);
    display.onSelect(displayMenu, "Invert", invertCallback);
    display.onSelect(displayMenu, "Back", backToMain);
  }
  
  // Handle System submenu
  if (strcmp(mainMenu->option, "System") == 0) {
    display.showMenu(systemMenu);
    
    display.onSelect(systemMenu, "Info", infoCallback);
    display.onSelect(systemMenu, "Memory", memoryCallback);
    display.onSelect(systemMenu, "Reset", resetCallback);
    display.onSelect(systemMenu, "Back", backToMain);
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

// Menu callbacks
void openSettingsMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Settings Menu");
  display.display();
  delay(500);
}

void openDisplayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Display Menu");
  display.display();
  delay(500);
}

void openSystemMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("System Menu");
  display.display();
  delay(500);
}

void showAbout() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("About");
  display.setCursor(0, 16);
  display.println("OLED Menu System");
  display.println("Version 1.0");
  display.println("(c) 2025");
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

// Settings submenu callbacks
void brightnessCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Brightness");
  
  // Show a brightness bar
  display.drawRect(10, 20, 108, 10, WHITE);
  display.fillRect(10, 20, 75, 10, WHITE);
  display.setCursor(10, 35);
  display.println("Current: 75%");
  display.setCursor(10, 50);
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void soundCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Sound Settings");
  display.setCursor(0, 16);
  display.println("Volume: ON");
  display.println("Beep: ON");
  display.println("Alerts: ON");
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void networkCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Network Settings");
  display.setCursor(0, 16);
  display.println("WiFi: Connected");
  display.println("IP: 192.168.1.100");
  display.println("Signal: Strong");
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

// Display submenu callbacks
void contrastCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Contrast Settings");
  
  // Show a contrast bar
  display.drawRect(10, 20, 108, 10, WHITE);
  display.fillRect(10, 20, 60, 10, WHITE);
  display.setCursor(10, 35);
  display.println("Current: 60%");
  display.setCursor(10, 50);
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void flipCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Flip Screen");
  display.setCursor(0, 16);
  display.println("Current: Normal");
  display.println("Options:");
  display.println("- Normal");
  display.println("- 180 Degrees");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void invertCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Invert Display");
  display.setCursor(0, 16);
  display.println("Current: Normal");
  display.setCursor(0, 32);
  display.println("Press any button");
  display.fillRect(0, 48, 128, 16, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(10, 52);
  display.println("Inverted example");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

// System submenu callbacks
void infoCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("System Information");
  display.setCursor(0, 16);
  display.println("Model: OLED 128x64");
  display.println("FW: v1.0.0");
  display.println("Build: 2025-03-19");
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void memoryCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Memory Status");
  display.setCursor(0, 16);
  
  int total = 32; // Total KB
  int used = 24;  // Used KB
  int free = 8;   // Free KB
  
  display.println("RAM Status:");
  
  // Draw memory bar
  display.drawRect(0, 32, 128, 10, WHITE);
  display.fillRect(0, 32, (used * 128) / total, 10, WHITE);
  
  display.setCursor(0, 45);
  display.print("Used: ");
  display.print(used);
  display.print("KB (");
  display.print((used * 100) / total);
  display.println("%)");
  
  display.print("Free: ");
  display.print(free);
  display.println("KB");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void resetCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Reset System?");
  display.setCursor(0, 16);
  display.println("This will reset all");
  display.println("settings to defaults.");
  display.println("Select = Confirm");
  display.println("Back = Cancel");
  display.display();
  
  bool resetConfirmed = false;
  bool exitScreen = false;
  
  while (!exitScreen) {
    if (digitalRead(SELECT_PIN) == LOW) {
      resetConfirmed = true;
      exitScreen = true;
      delay(200);
    }
    
    if (digitalRead(BACK_PIN) == LOW) {
      exitScreen = true;
      delay(200);
    }
    
    delay(10);
  }
  
  if (resetConfirmed) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Resetting...");
    display.display();
    
    // Simulate reset with progress bar
    display.drawRect(0, 20, 128, 10, WHITE);
    for (int i = 0; i < 128; i++) {
      display.fillRect(0, 20, i, 10, WHITE);
      display.display();
      delay(20);
    }
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Reset Complete!");
    display.println("Press any button");
    display.display();
    
    waitForButton();
  }
  
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