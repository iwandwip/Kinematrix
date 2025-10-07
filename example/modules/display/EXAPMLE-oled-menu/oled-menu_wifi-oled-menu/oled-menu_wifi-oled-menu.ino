#if !defined(ESP32) && !defined(ESP8266)
#error "This example requires ESP32 or ESP8266"
#endif

#include "oled-menu.h"

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#define UP_PIN 5     // GPIO5 for ESP
#define DOWN_PIN 4    // GPIO4 for ESP
#define SELECT_PIN 14 // GPIO14 for ESP
#define BACK_PIN 12   // GPIO12 for ESP

// WiFi credentials
const char* ssid = "YourWiFiName";
const char* password = "YourWiFiPassword";

OledMenu display(128, 64);
MenuCursor menuCursor = {false, false, false, false, true};

MenuProperties *mainMenu;
MenuProperties *wifiMenu;
MenuProperties *infoMenu;
MenuProperties *scanMenu;

// Variables to store WiFi info
int rssiValue = 0;
int networkCount = 0;
String ipAddress = "0.0.0.0";
String macAddress = "";
String wifiStatus = "Disconnected";
bool scanning = false;

// For network scanning
String networks[10];
int networkSignals[10];

void setup() {
  Serial.begin(115200);
  
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(SELECT_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);
  
  // Initialize OLED
  display.initialize(true);
  
  // Get device MAC address
  macAddress = WiFi.macAddress();
  
  // Create menus
  mainMenu = display.createMenu(4, "WiFi Connect", "WiFi Scan", "Info", "About");
  wifiMenu = display.createMenu(3, "Connect", "Disconnect", "Back");
  infoMenu = display.createMenu(5, "Status", "IP Address", "Signal", "MAC", "Back");
  scanMenu = display.createEmptyMenu(11); // 10 networks + Back
  
  // Initialize scan menu with placeholders
  for (int i = 0; i < 10; i++) {
    display.formatMenu(scanMenu, i, "Scanning...");
  }
  display.formatMenu(scanMenu, 10, "Back");
  
  // Welcome screen
  showWelcomeScreen();
}

void loop() {
  // Check WiFi status
  updateWifiStatus();
  
  // Process button input
  display.onListen(&menuCursor, listenButtons);
  
  // Show main menu
  display.showMenu(mainMenu);
  
  // Handle main menu selections
  display.onSelect(mainMenu, "WiFi Connect", openWiFiMenu);
  display.onSelect(mainMenu, "WiFi Scan", openScanMenu);
  display.onSelect(mainMenu, "Info", openInfoMenu);
  display.onSelect(mainMenu, "About", aboutCallback);
  
  // Handle WiFi submenu
  if (strcmp(mainMenu->option, "WiFi Connect") == 0) {
    display.showMenu(wifiMenu);
    
    display.onSelect(wifiMenu, "Connect", connectCallback);
    display.onSelect(wifiMenu, "Disconnect", disconnectCallback);
    display.onSelect(wifiMenu, "Back", backToMain);
  }
  
  // Handle Info submenu
  if (strcmp(mainMenu->option, "Info") == 0) {
    display.showMenu(infoMenu);
    
    display.onSelect(infoMenu, "Status", statusCallback);
    display.onSelect(infoMenu, "IP Address", ipAddressCallback);
    display.onSelect(infoMenu, "Signal", signalCallback);
    display.onSelect(infoMenu, "MAC", macAddressCallback);
    display.onSelect(infoMenu, "Back", backToMain);
  }
  
  // Handle Scan submenu
  if (strcmp(mainMenu->option, "WiFi Scan") == 0) {
    // Start a scan if we haven't already
    if (!scanning) {
      startWifiScan();
    }
    
    display.showMenu(scanMenu);
    
    // Add callbacks for each network
    for (int i = 0; i < networkCount; i++) {
      display.onSelect(scanMenu, networks[i].c_str(), scanItemCallback);
    }
    
    display.onSelect(scanMenu, "Back", backToMain);
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

void showWelcomeScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 5);
  display.println("WiFi Menu");
  
  display.setTextSize(1);
  display.setCursor(20, 25);
  display.println("ESP WiFi Scanner");
  display.setCursor(20, 35);
  display.println("and Connection Tool");
  
  display.setCursor(5, 55);
  display.println("Press any button to start");
  display.display();
  
  waitForButton();
}

void updateWifiStatus() {
  wifiStatus = "Disconnected";
  ipAddress = "0.0.0.0";
  rssiValue = 0;
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiStatus = "Connected to " + String(WiFi.SSID());
    ipAddress = WiFi.localIP().toString();
    rssiValue = WiFi.RSSI();
  } else if (WiFi.status() == WL_CONNECT_FAILED) {
    wifiStatus = "Connection Failed";
  } else if (WiFi.status() == WL_IDLE_STATUS) {
    wifiStatus = "Idle";
  } else if (WiFi.status() == WL_NO_SSID_AVAIL) {
    wifiStatus = "No SSID Available";
  }
}

void startWifiScan() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Scanning WiFi Networks...");
  display.drawRect(10, 20, 108, 10, WHITE);
  display.display();
  
  // Start WiFi scan
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  WiFi.scanDelete(); // Delete results of previous scan
  
  // Start scan
  scanning = true;
  networkCount = WiFi.scanNetworks();
  scanning = false;
  
  display.clearDisplay();
  
  if (networkCount == 0) {
    display.setCursor(0, 20);
    display.println("No networks found!");
    display.display();
    delay(2000);
  } else {
    display.setCursor(0, 0);
    display.print("Found ");
    display.print(networkCount);
    display.println(" networks");
    
    // Fill progress bar
    display.fillRect(10, 20, 108, 10, WHITE);
    display.display();
    delay(1000);
    
    // Update scan menu
    int numToShow = min(10, networkCount);
    for (int i = 0; i < numToShow; i++) {
      String ssidName = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);
      
      // Store network info
      networks[i] = ssidName;
      networkSignals[i] = rssi;
      
      // Format menu item with signal strength indicator
      char menuText[24];
      snprintf(menuText, 24, "%-14s %ddBm", ssidName.substring(0, 14).c_str(), rssi);
      display.formatMenu(scanMenu, i, menuText);
    }
    
    // Ensure we have Back option
    display.formatMenu(scanMenu, numToShow, "Back");
  }
}

// Menu callbacks
void openWiFiMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Connection");
  display.setCursor(0, 15);
  display.println("Status: " + wifiStatus);
  display.display();
  delay(1000);
}

void openInfoMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Information");
  display.setCursor(0, 15);
  display.println("Select an option");
  display.display();
  delay(1000);
}

void openScanMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Network Scanner");
  display.setCursor(0, 15);
  display.println("Starting scan...");
  display.display();
  delay(1000);
}

void connectCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi");
  display.setCursor(0, 15);
  display.println("SSID: " + String(ssid));
  display.drawRect(0, 30, 128, 10, WHITE);
  display.display();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int timeout = 20; // 20 seconds timeout
  int progress = 0;
  
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    timeout--;
    
    // Update progress bar
    progress = map(20 - timeout, 0, 20, 0, 128);
    display.fillRect(0, 30, progress, 10, WHITE);
    
    // Blink connecting message
    if (timeout % 2 == 0) {
      display.fillRect(0, 45, 128, 8, BLACK);
      display.setCursor(0, 45);
      display.print("Connecting");
      for (int i = 0; i < (20 - timeout) % 4; i++) {
        display.print(".");
      }
    }
    
    display.display();
  }
  
  display.fillRect(0, 45, 128, 8, BLACK);
  
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(0, 45);
    display.println("Connected!");
    display.setCursor(0, 55);
    display.print("IP: ");
    display.println(WiFi.localIP().toString());
  } else {
    display.setCursor(0, 45);
    display.println("Connection failed!");
  }
  
  display.display();
  delay(2000);
  updateWifiStatus();
  menuCursor.back = true;
}

void disconnectCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Disconnecting WiFi");
  display.display();
  
  WiFi.disconnect();
  delay(1000);
  
  display.setCursor(0, 20);
  display.println("Disconnected!");
  display.display();
  
  delay(2000);
  updateWifiStatus();
  menuCursor.back = true;
}

void statusCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("WiFi Status");
  display.drawFastHLine(0, 9, 128, WHITE);
  
  display.setCursor(0, 15);
  display.println("Status:");
  
  // To display long status in scrolling mode
  String status = wifiStatus;
  if (status.length() > 21) {
    for (int i = 0; i <= status.length() - 21; i++) {
      display.fillRect(0, 25, 128, 8, BLACK);
      display.setCursor(0, 25);
      display.println(status.substring(i, i + 21));
      display.display();
      delay(300);
      
      if (buttonPressed()) break;
    }
  } else {
    display.setCursor(0, 25);
    display.println(status);
  }
  
  // Draw signal strength if connected
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(0, 40);
    display.println("Signal Strength:");
    
    // Draw signal bars
    int signalQuality = map(rssiValue, -100, -40, 1, 5);
    for (int i = 0; i < 5; i++) {
      int barHeight = 4 + (i * 2);
      int barX = 80 + (i * 8);
      if (i < signalQuality) {
        display.fillRect(barX, 62 - barHeight, 6, barHeight, WHITE);
      } else {
        display.drawRect(barX, 62 - barHeight, 6, barHeight, WHITE);
      }
    }
  }
  
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void ipAddressCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("IP Address Info");
  display.drawFastHLine(0, 9, 128, WHITE);
  
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(0, 15);
    display.println("IP Address:");
    display.setCursor(0, 25);
    display.println(ipAddress);
    
    display.setCursor(0, 40);
    display.println("Subnet Mask:");
    display.setCursor(0, 50);
    display.println(WiFi.subnetMask().toString());
    
  } else {
    display.setCursor(0, 25);
    display.println("Not connected");
    display.setCursor(0, 35);
    display.println("Connect to WiFi first");
  }
  
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void signalCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Signal Strength");
  display.drawFastHLine(0, 9, 128, WHITE);
  
  if (WiFi.status() == WL_CONNECTED) {
    display.setCursor(0, 15);
    display.print("RSSI: ");
    display.print(rssiValue);
    display.println(" dBm");
    
    // Display signal quality category
    display.setCursor(0, 25);
    display.print("Quality: ");
    
    if (rssiValue >= -50) {
      display.println("Excellent");
    } else if (rssiValue >= -60) {
      display.println("Good");
    } else if (rssiValue >= -70) {
      display.println("Fair");
    } else {
      display.println("Poor");
    }
    
    // Draw signal meter
    display.drawRect(0, 40, 128, 10, WHITE);
    int barWidth = map(rssiValue, -100, -40, 0, 128);
    display.fillRect(0, 40, barWidth, 10, WHITE);
    
    // Draw scale markers
    display.setCursor(0, 55);
    display.print("-100");
    display.setCursor(105, 55);
    display.print("-40");
    display.setCursor(50, 55);
    display.print("-70");
    
  } else {
    display.setCursor(0, 25);
    display.println("Not connected");
    display.setCursor(0, 35);
    display.println("Connect to WiFi first");
  }
  
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void macAddressCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("MAC Address");
  display.drawFastHLine(0, 9, 128, WHITE);
  
  display.setCursor(0, 20);
  display.println("Device MAC:");
  display.setCursor(0, 30);
  display.println(macAddress);
  
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void scanItemCallback() {
  // Get selected network
  String selectedNetwork = scanMenu->option;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Network Details");
  display.drawFastHLine(0, 9, 128, WHITE);
  
  display.setCursor(0, 15);
  display.println("SSID:");
  display.setCursor(0, 25);
  display.println(selectedNetwork);
  
  // Find signal strength for this network
  int signal = 0;
  for (int i = 0; i < networkCount; i++) {
    if (networks[i] == selectedNetwork) {
      signal = networkSignals[i];
      break;
    }
  }
  
  display.setCursor(0, 35);
  display.print("Signal: ");
  display.print(signal);
  display.println(" dBm");
  
  // Draw signal bars
  int signalQuality = map(signal, -100, -40, 1, 5);
  for (int i = 0; i < 5; i++) {
    int barHeight = 4 + (i * 2);
    int barX = 80 + (i * 8);
    if (i < signalQuality) {
      display.fillRect(barX, 52 - barHeight, 6, barHeight, WHITE);
    } else {
      display.drawRect(barX, 52 - barHeight, 6, barHeight, WHITE);
    }
  }
  
  display.setCursor(0, 55);
  display.println("Press any button");
  display.display();
  
  waitForButton();
  menuCursor.back = true;
}

void aboutCallback() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("About WiFi Menu");
  display.drawFastHLine(0, 9, 128, WHITE);
  
  display.setCursor(0, 15);
  display.println("WiFi Scanner &");
  display.println("Connection Manager");
  display.println("Version 1.0 (2025)");
  display.println("");
  display.println("For ESP32/ESP8266");
  
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