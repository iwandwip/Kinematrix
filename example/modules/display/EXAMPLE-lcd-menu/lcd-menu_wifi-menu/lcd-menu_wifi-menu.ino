#include "lcd-menu.h"

#if !defined(ESP32) && !defined(ESP8266)
#error "This example requires ESP32 or ESP8266"
#endif

#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

#define UP_PIN 5
#define DOWN_PIN 4
#define SELECT_PIN 14
#define BACK_PIN 12

const char* wifi_ssid = "YourWiFiName";
const char* wifi_password = "YourWiFiPassword";

LcdMenu lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
MenuCursor menuCursor = {false, false, false, false, true};
MenuProperties *mainMenu;
MenuProperties *wifiMenu;
MenuProperties *statusMenu;

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  lcd.initialize();
  lcd.setLen(LCD_COLS, LCD_ROWS);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Menu Demo");
  lcd.setCursor(0, 1);
  lcd.print("ESP32/ESP8266");
  delay(2000);
  
  mainMenu = lcd.createMenu(3, "WiFi Options", "Status", "About");
  wifiMenu = lcd.createMenu(3, "Connect", "Disconnect", "Back");
  statusMenu = lcd.createMenu(3, "WiFi Status", "IP Address", "Back");
}

void loop() {
  lcd.onListen(&menuCursor, listenButtons);
  
  lcd.showMenu(mainMenu);
  
  lcd.onSelect(mainMenu, "WiFi Options", openWiFiMenu);
  lcd.onSelect(mainMenu, "Status", openStatusMenu);
  lcd.onSelect(mainMenu, "About", aboutCallback);
  
  if (strcmp(mainMenu->option, "WiFi Options") == 0) {
    lcd.showMenu(wifiMenu);
    lcd.onSelect(wifiMenu, "Connect", connectWiFiCallback);
    lcd.onSelect(wifiMenu, "Disconnect", disconnectWiFiCallback);
    lcd.onSelect(wifiMenu, "Back", backToMain);
  }
  
  if (strcmp(mainMenu->option, "Status") == 0) {
    lcd.showMenu(statusMenu);
    lcd.onSelect(statusMenu, "WiFi Status", wifiStatusCallback);
    lcd.onSelect(statusMenu, "IP Address", ipAddressCallback);
    lcd.onSelect(statusMenu, "Back", backToMain);
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

void openWiFiMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Options");
  delay(500);
}

void openStatusMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status Menu");
  delay(500);
}

void connectWiFiCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to:");
  lcd.setCursor(0, 1);
  lcd.print(wifi_ssid);
  
  bool connected = lcd.connectToWiFi(wifi_ssid, wifi_password, 15);
  
  if (connected) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected!");
    lcd.setCursor(0, 1);
    lcd.print("Press any button");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Press any button");
  }
  
  waitForAnyButton();
  menuCursor.back = true;
}

void disconnectWiFiCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Disconnecting...");
  
  WiFi.disconnect();
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Disconnected!");
  lcd.setCursor(0, 1);
  lcd.print("Press any button");
  
  waitForAnyButton();
  menuCursor.back = true;
}

void wifiStatusCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Status:");
  lcd.setCursor(0, 1);
  
  String status = lcd.getWiFiStatus();
  
  if (status.length() > LCD_COLS) {
    for (int i = 0; i <= status.length() - LCD_COLS; i++) {
      lcd.setCursor(0, 1);
      lcd.print(status.substring(i, i + LCD_COLS));
      delay(400);
    }
  } else {
    lcd.print(status);
  }
  
  waitForAnyButton();
  menuCursor.back = true;
}

void ipAddressCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP Address:");
  lcd.setCursor(0, 1);
  lcd.print(lcd.getIPAddress());
  
  waitForAnyButton();
  menuCursor.back = true;
}

void aboutCallback() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD WiFi Menu");
  lcd.setCursor(0, 1);
  lcd.print("Version 1.0");
  
  waitForAnyButton();
  menuCursor.back = true;
}

void backToMain(MenuCursor *cursor) {
  cursor->back = true;
}

void waitForAnyButton() {
  bool buttonPressed = false;
  while (!buttonPressed) {
    if (digitalRead(UP_PIN) == LOW || 
        digitalRead(DOWN_PIN) == LOW || 
        digitalRead(SELECT_PIN) == LOW || 
        digitalRead(BACK_PIN) == LOW) {
      buttonPressed = true;
      delay(200);
    }
    delay(10);
  }
}