#include "lcd-menu.h"

#define LCD_ADDRESS 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define UP_PIN 2
#define DOWN_PIN 3
#define SELECT_PIN 4
#define BACK_PIN 5

LcdMenu lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
MenuCursor menuCursor = {false, false, false, false, true};
MenuProperties *mainMenu;
MenuProperties *settingsMenu;
MenuProperties *systemMenu;
MenuProperties *infoMenu;

unsigned long systemTime = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  lcd.initialize();
  lcd.setLen(LCD_COLS, LCD_ROWS);
  
  mainMenu = lcd.createMenu(8, "Settings", "System", "Network", "Display", 
                            "Storage", "Security", "Info", "Power");
  
  settingsMenu = lcd.createMenu(5, "Language", "Date & Time", "Sound", "Theme", "Back");
  systemMenu = lcd.createMenu(6, "CPU", "Memory", "Storage", "Battery", "Update", "Back");
  infoMenu = lcd.createMenu(5, "Version", "Serial", "Model", "About", "Back");
  
  systemTime = millis();
}

void loop() {
  lcd.onListen(&menuCursor, listenButtons);
  
  lcd.showMenu(mainMenu);
  
  lcd.onSelect(mainMenu, "Settings", openSettingsMenu);
  lcd.onSelect(mainMenu, "System", openSystemMenu);
  lcd.onSelect(mainMenu, "Network", networkCallback);
  lcd.onSelect(mainMenu, "Display", displayCallback);
  lcd.onSelect(mainMenu, "Storage", storageCallback);
  lcd.onSelect(mainMenu, "Security", securityCallback);
  lcd.onSelect(mainMenu, "Info", openInfoMenu);
  lcd.onSelect(mainMenu, "Power", powerCallback);
  
  if (strcmp(mainMenu->option, "Settings") == 0) {
    lcd.showMenu(settingsMenu);
    
    lcd.onSelect(settingsMenu, "Language", languageCallback);
    lcd.onSelect(settingsMenu, "Date & Time", timeCallback);
    lcd.onSelect(settingsMenu, "Sound", soundCallback);
    lcd.onSelect(settingsMenu, "Theme", themeCallback);
    lcd.onSelect(settingsMenu, "Back", backToMain);
  }
  
  if (strcmp(mainMenu->option, "System") == 0) {
    lcd.showMenu(systemMenu);
    
    lcd.onSelect(systemMenu, "CPU", cpuCallback);
    lcd.onSelect(systemMenu, "Memory", memoryCallback);
    lcd.onSelect(systemMenu, "Storage", sysStorageCallback);
    lcd.onSelect(systemMenu, "Battery", batteryCallback);
    lcd.onSelect(systemMenu, "Update", updateCallback);
    lcd.onSelect(systemMenu, "Back", backToMain);
  }
  
  if (strcmp(mainMenu->option, "Info") == 0) {
    lcd.showMenu(infoMenu);
    
    lcd.onSelect(infoMenu, "Version", versionCallback);
    lcd.onSelect(infoMenu, "Serial", serialCallback);
    lcd.onSelect(infoMenu, "Model", modelCallback);
    lcd.onSelect(infoMenu, "About", aboutCallback);
    lcd.onSelect(infoMenu, "Back", backToMain);
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

void displayInfoScreen(const char *title, const char *line1, const char *line2, const char *line3) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(title);
  lcd.setCursor(0, 1);
  lcd.print(line1);
  lcd.setCursor(0, 2);
  lcd.print(line2);
  lcd.setCursor(0, 3);
  lcd.print(line3);
  
  bool exitScreen = false;
  while (!exitScreen) {
    if (digitalRead(BACK_PIN) == LOW || digitalRead(SELECT_PIN) == LOW) {
      exitScreen = true;
      delay(200);
    }
    delay(10);
  }
}

void openSettingsMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Settings Menu");
  delay(500);
}

void openSystemMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Menu");
  delay(500);
}

void openInfoMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Information Menu");
  delay(500);
}

void networkCallback() {
  displayInfoScreen(
    "Network Status", 
    "Mode: WiFi", 
    "Status: Connected", 
    "IP: 192.168.1.100"
  );
  menuCursor.back = true;
}

void displayCallback() {
  displayInfoScreen(
    "Display Settings", 
    "Mode: Standard", 
    "Brightness: 80%", 
    "Timeout: 60s"
  );
  menuCursor.back = true;
}

void storageCallback() {
  displayInfoScreen(
    "Storage Info", 
    "Total: 8GB", 
    "Used: 3.2GB (40%)", 
    "Free: 4.8GB (60%)"
  );
  menuCursor.back = true;
}

void securityCallback() {
  displayInfoScreen(
    "Security Settings", 
    "Lock: PIN", 
    "Timeout: 5min", 
    "Last Login: Today"
  );
  menuCursor.back = true;
}

void powerCallback() {
  displayInfoScreen(
    "Power Options", 
    "1: Shutdown", 
    "2: Restart", 
    "3: Sleep Mode"
  );
  menuCursor.back = true;
}

void languageCallback() {
  displayInfoScreen(
    "Language Settings", 
    "Current: English", 
    "Available: 5", 
    "Default: System"
  );
  menuCursor.back = true;
}

void timeCallback() {
  unsigned long runTime = (millis() - systemTime) / 1000;
  char timeBuff[21];
  sprintf(timeBuff, "Uptime: %lus", runTime);
  
  displayInfoScreen(
    "Time Settings", 
    "Set: Auto", 
    timeBuff, 
    "Format: 24h"
  );
  menuCursor.back = true;
}

void soundCallback() {
  displayInfoScreen(
    "Sound Settings", 
    "Volume: 70%", 
    "Alerts: Enabled", 
    "Keypress: Off"
  );
  menuCursor.back = true;
}

void themeCallback() {
  displayInfoScreen(
    "Theme Settings", 
    "Theme: Default", 
    "Contrast: 50%", 
    "Invert: Off"
  );
  menuCursor.back = true;
}

void cpuCallback() {
  displayInfoScreen(
    "CPU Information", 
    "Model: ARM Cortex", 
    "Clock: 16MHz", 
    "Temp: 25C"
  );
  menuCursor.back = true;
}

void memoryCallback() {
  displayInfoScreen(
    "Memory Status", 
    "Total: 2KB", 
    "Used: 1.2KB (60%)", 
    "Free: 0.8KB (40%)"
  );
  menuCursor.back = true;
}

void sysStorageCallback() {
  displayInfoScreen(
    "System Storage", 
    "Flash: 32KB", 
    "Used: 24KB (75%)", 
    "Free: 8KB (25%)"
  );
  menuCursor.back = true;
}

void batteryCallback() {
  displayInfoScreen(
    "Battery Status", 
    "Level: 85%", 
    "Voltage: 3.7V", 
    "Charging: No"
  );
  menuCursor.back = true;
}

void updateCallback() {
  displayInfoScreen(
    "System Update", 
    "Current: v1.0.0", 
    "Latest: v1.0.0", 
    "Status: Up to date"
  );
  menuCursor.back = true;
}

void versionCallback() {
  displayInfoScreen(
    "Version Info", 
    "FW: v1.0.0", 
    "HW: v2.0", 
    "Build: 20250319"
  );
  menuCursor.back = true;
}

void serialCallback() {
  displayInfoScreen(
    "Serial Number", 
    "SN: LCD-00123456", 
    "Made: 03/2025", 
    "Batch: A001"
  );
  menuCursor.back = true;
}

void modelCallback() {
  displayInfoScreen(
    "Model Information", 
    "Model: LCD-20x4", 
    "Type: I2C", 
    "Variant: STD"
  );
  menuCursor.back = true;
}

void aboutCallback() {
  displayInfoScreen(
    "About System", 
    "LCD Menu Demo", 
    "By: Claude Labs", 
    "(C) 2025"
  );
  menuCursor.back = true;
}

void backToMain(MenuCursor *cursor) {
  cursor->back = true;
}