#include "SerialCommander.h"
#include "Common/VirtualPCF.h"
#include <Wire.h>

namespace AutoLight {

SerialCommander::SerialCommander() : 
    led_(nullptr), config_(nullptr), viz_(nullptr), button_(nullptr),
    viz_enabled_(false), last_viz_update_(0), viz_refresh_rate_(100) {
    strcpy(ap_config_.ssid, "");
    strcpy(ap_config_.password, "");
    ap_config_.is_loaded_from_preferences = false;
    current_mode_ = SERIAL_READ_ONLY;
    activation_timeout_ = 0;
}

void SerialCommander::init(BaseChannel* led, BaseConfig* config, LEDMatrixVisualizer* viz, ButtonInterrupt* button) {
    led_ = led;
    config_ = config;
    viz_ = viz;
    button_ = button;
    loadWiFiPreferences();
}

void SerialCommander::initWiFiAP() {
    startAccessPoint();
    
    if (!MDNS.begin("als")) {
        Serial.println("Error starting mDNS");
        return;
    }
    Serial.println("mDNS started: als.local");
}

void SerialCommander::loadWiFiPreferences() {
    Preferences prefs;
    prefs.begin("ap_config", false);
    
    String saved_ssid = prefs.getString("ssid", "");
    String saved_pass = prefs.getString("password", "");
    
    if (saved_ssid.length() > 0) {
        strcpy(ap_config_.ssid, saved_ssid.c_str());
        strcpy(ap_config_.password, saved_pass.c_str());
        ap_config_.is_loaded_from_preferences = true;
        Serial.println("✓ WiFi AP config loaded from preferences");
    } else {
        strcpy(ap_config_.ssid, "AutoLightV3");
        strcpy(ap_config_.password, "");
        ap_config_.is_loaded_from_preferences = false;
        Serial.println("ℹ Using default WiFi AP config");
    }
    
    prefs.end();
}

void SerialCommander::startAccessPoint() {
    WiFi.mode(WIFI_AP);
    
    bool success;
    if (strlen(ap_config_.password) == 0) {
        success = WiFi.softAP(ap_config_.ssid);
    } else {
        success = WiFi.softAP(ap_config_.ssid, ap_config_.password);
    }
    
    if (success) {
        IPAddress ip = WiFi.softAPIP();
        Serial.printf("✓ AP Started: '%s'\n", ap_config_.ssid);
        Serial.printf("  IP: %s\n", ip.toString().c_str());
        Serial.printf("  Security: %s\n", strlen(ap_config_.password) == 0 ? "Open" : "WPA2");
        Serial.println("  Web Interface: http://" + ip.toString());
        Serial.println("  mDNS: http://als.local");
    } else {
        Serial.println("❌ Failed to start AP");
    }
}

void SerialCommander::process() {
    checkTimeout();
    
    if (!Serial.available()) return;
    
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() == 0) return;
    
    int space = input.indexOf(' ');
    String cmd = (space > 0) ? input.substring(0, space) : input;
    String args = (space > 0) ? input.substring(space + 1) : "";
    
    if (cmd == "activate") activate(args);
    else if (cmd == "deactivate") deactivate(args);
    else if (cmd == "help") help(args);
    else if (cmd == "wifi") {
        if (isCommandAllowed("wifi")) wifi(args);
    }
    else if (cmd == "led") {
        if (isCommandAllowed("led")) led_control(args);
    }
    else if (cmd == "seq") {
        if (isCommandAllowed("seq")) sequence(args);
    }
    else if (cmd == "pcf") {
        if (isCommandAllowed("pcf")) pcf_manage(args);
    }
    else if (cmd == "viz") {
        if (isCommandAllowed("viz")) visualize(args);
    }
    else if (cmd == "btn") {
        if (isCommandAllowed("btn")) button(args);
    }
    else if (cmd == "serial") {
        if (isCommandAllowed("serial")) serial_manage(args);
    }
    else Serial.println("❌ Unknown command. Type 'help'");
}

void SerialCommander::parseQuotedArgs(String input, String& arg1, String& arg2) {
    int first_quote = input.indexOf('"');
    int second_quote = input.indexOf('"', first_quote + 1);
    int third_quote = input.indexOf('"', second_quote + 1);
    int fourth_quote = input.indexOf('"', third_quote + 1);
    
    if (first_quote != -1 && second_quote != -1) {
        arg1 = input.substring(first_quote + 1, second_quote);
    }
    
    if (third_quote != -1 && fourth_quote != -1) {
        arg2 = input.substring(third_quote + 1, fourth_quote);
    }
}

void SerialCommander::wifi(String args) {
    if (args.length() == 0) {
        showWiFiInfo();
    }
    else if (args.startsWith("set ")) {
        String credentials = args.substring(4);
        String new_ssid, new_pass;
        parseQuotedArgs(credentials, new_ssid, new_pass);
        
        if (new_ssid.length() == 0 || new_ssid.length() > 31) {
            Serial.println("❌ SSID must be 1-31 characters");
            return;
        }
        
        if (new_pass.length() > 0 && new_pass.length() < 8) {
            Serial.println("❌ Password must be empty (open) or 8+ characters");
            return;
        }
        
        ConfigManager::getInstance().updateCredentials(new_ssid, new_pass);
        
        strcpy(ap_config_.ssid, new_ssid.c_str());
        strcpy(ap_config_.password, new_pass.c_str());
        ap_config_.is_loaded_from_preferences = true;
        
        Serial.printf("✓ AP credentials saved: '%s'\n", new_ssid.c_str());
        Serial.println("  Use 'wifi restart' to apply changes");
    }
    else if (args == "restart") {
        restartAP();
    }
    else if (args == "reset") {
        resetToDefaults();
    }
    else if (args == "clients") {
        showConnectedClients();
    }
    else {
        Serial.println("Usage: wifi, wifi set \"ssid\" \"pass\", wifi restart, wifi reset, wifi clients");
    }
}

void SerialCommander::led_control(String args) {
    if (!led_) {
        Serial.println("❌ LED controller not initialized");
        return;
    }
    
    if (args.startsWith("test all")) {
        if (!isCommandAllowed("led test")) return;
        for (uint8_t i = 0; i < led_->getTotalLEDs(); i++) {
            testChannel(i);
        }
    }
    else if (args.startsWith("test ")) {
        if (!isCommandAllowed("led test")) return;
        uint8_t ch = args.substring(5).toInt();
        if (ch < led_->getTotalLEDs()) {
            testChannel(ch);
        }
    }
    else if (args.endsWith(" on")) {
        uint8_t ch = args.substring(0, args.length()-3).toInt();
        if (ch < led_->getTotalLEDs()) {
            led_->setStateHigh(ch, -1);
            Serial.printf("✓ Channel %d ON\n", ch);
        } else {
            Serial.printf("❌ Invalid channel: %d\n", ch);
        }
    }
    else if (args.endsWith(" off")) {
        uint8_t ch = args.substring(0, args.length()-4).toInt();
        if (ch < led_->getTotalLEDs()) {
            led_->setStateLow(ch, -1);
            Serial.printf("✓ Channel %d OFF\n", ch);
        } else {
            Serial.printf("❌ Invalid channel: %d\n", ch);
        }
    }
    else if (args == "status") {
        showChannelStatus();
    }
    else {
        Serial.println("Usage: led test <ch>, led test all, led <ch> on/off, led status");
    }
}

void SerialCommander::sequence(String args) {
    if (!led_) {
        Serial.println("❌ LED controller not initialized");
        return;
    }
    
    if (args.length() > 0 && isdigit(args[0])) {
        uint8_t mode = args.toInt();
        if (mode <= 15) {
            led_->changeModeApp(mode);
            Serial.printf("✓ Sequence mode set to %d\n", mode);
        } else {
            Serial.println("❌ Invalid sequence mode (valid: 0-15)");
        }
    }
    else if (args == "next") {
        led_->nextMode();
        Serial.printf("✓ Next sequence: %d\n", led_->getSequenceIndex());
    }
    else if (args == "prev") {
        led_->previousMode();
        Serial.printf("✓ Previous sequence: %d\n", led_->getSequenceIndex());
    }
    else if (args == "list") {
        Serial.println("📝 Available LED Sequences:");
        const char* seq_names[] = {
            "ALL OFF", "ALL ON", "BLINK ALL", "CHASE F-R", "CHASE L-R",
            "WAVE", "ALT F/R", "CENTER OUT", "STROBE", "RANDOM",
            "PULSE", "BREATHING", "FADE", "KNIGHT RIDER", "RAINBOW", "SPARKLE"
        };
        
        for (int i = 0; i < 16; i++) {
            Serial.printf("%2d: %s %s\n", i, seq_names[i], 
                led_->getSequenceIndex() == i ? "← Current" : "");
        }
    }
    else if (args.startsWith("speed ")) {
        uint32_t speed = args.substring(6).toInt();
        if (speed >= 10 && speed <= 2000) {
            led_->setInitDelay(speed);
            Serial.printf("✓ Sequence speed set to %dms\n", speed);
        } else {
            Serial.println("❌ Speed must be 10-2000ms");
        }
    }
    else {
        Serial.printf("Current: %d | Usage: seq <mode>, seq next/prev, seq list, seq speed <ms>\n", 
            led_->getSequenceIndex());
    }
}

void SerialCommander::pcf_manage(String args) {
    if (args == "scan") {
        scanI2CBus();
    }
    else if (args.startsWith("test ")) {
        uint8_t address = parseHexAddress(args.substring(5));
        testSinglePCF(address);
    }
    else if (args == "status") {
        showAllPCFStatus();
    }
    else if (args.startsWith("reset ")) {
        uint8_t address = parseHexAddress(args.substring(6));
        resetPCF(address);
    }
    else {
        Serial.println("Usage: pcf scan, pcf test <addr>, pcf status, pcf reset <addr>");
    }
}

void SerialCommander::visualize(String args) {
    if (!viz_) {
        Serial.println("❌ Visualizer not initialized");
        return;
    }
    
    if (args == "on") {
        viz_enabled_ = true;
        Serial.println("✓ Visualization enabled");
        viz_->updateFromAutoLight(led_);
        viz_->printMatrix();
    }
    else if (args == "off") {
        viz_enabled_ = false;
        Serial.println("✓ Visualization disabled");
    }
    else if (args == "refresh") {
        if (viz_enabled_) {
            viz_->updateFromAutoLight(led_);
            viz_->printMatrix();
        } else {
            Serial.println("❌ Visualization is disabled");
        }
    }
    else if (args.startsWith("layout ")) {
        int space = args.indexOf(' ', 7);
        if (space > 0) {
            uint8_t rows = args.substring(7, space).toInt();
            uint8_t cols = args.substring(space + 1).toInt();
            
            if (rows > 0 && cols > 0 && rows * cols >= led_->getTotalLEDs()) {
                delete viz_;
                viz_ = new LEDMatrixVisualizer(rows, cols);
                
                for (uint8_t i = 0; i < led_->getTotalLEDs(); i++) {
                    uint8_t row = i / cols;
                    uint8_t col = i % cols;
                    viz_->placeLED(i, row, col);
                }
                
                Serial.printf("✓ Matrix layout set to %dx%d\n", rows, cols);
                if (viz_enabled_) {
                    viz_->updateFromAutoLight(led_);
                    viz_->printMatrix();
                }
            } else {
                Serial.printf("❌ Layout %dx%d too small for %d channels\n", 
                    rows, cols, led_->getTotalLEDs());
            }
        }
    }
    else {
        Serial.printf("Status: %s | Usage: viz on/off, viz refresh, viz layout <rows> <cols>\n",
            viz_enabled_ ? "Enabled" : "Disabled");
    }
}

void SerialCommander::button(String args) {
    if (!button_ || !led_) {
        Serial.println("❌ Button handler not initialized");
        return;
    }
    
    if (args.length() > 0 && isdigit(args[0])) {
        uint8_t btn_id = args.toInt();
        if (btn_id <= 3) {
            led_->smartButtonPress(btn_id);
            Serial.printf("✓ Button %d pressed | Sequence: %d\n", btn_id, led_->getSequenceIndex());
        } else {
            Serial.println("❌ Invalid button ID (valid: 0-3)");
        }
    }
    else if (args.startsWith("mode ")) {
        String mode = args.substring(5);
        if (mode == "1button") {
            led_->setButtonMode(BUTTON_MODE_1BUTTON);
            Serial.println("✓ Button mode: 1-Button (Cycle sequences)");
        }
        else if (mode == "2button") {
            led_->setButtonMode(BUTTON_MODE_2BUTTON);
            Serial.println("✓ Button mode: 2-Button (Toggle + Next)");
        }
        else if (mode == "3button") {
            led_->setButtonMode(BUTTON_MODE_3BUTTON);
            Serial.println("✓ Button mode: 3-Button (ON/OFF/Next)");
        }
        else if (mode == "4button") {
            led_->setButtonMode(BUTTON_MODE_4BUTTON);
            Serial.println("✓ Button mode: 4-Button (ON/OFF/Next/Prev)");
        }
        else {
            Serial.println("❌ Invalid mode. Available: 1button, 2button, 3button, 4button");
        }
    }
    else {
        Serial.println("Usage: btn <id>, btn mode <1button|2button|3button|4button>");
    }
}

void SerialCommander::serial_manage(String args) {
    if (args.startsWith("set ")) {
        String serial_input = args.substring(4);
        String new_serial;
        
        int first_quote = serial_input.indexOf('"');
        int second_quote = serial_input.indexOf('"', first_quote + 1);
        
        if (first_quote != -1 && second_quote != -1) {
            new_serial = serial_input.substring(first_quote + 1, second_quote);
        } else {
            new_serial = serial_input;
            new_serial.trim();
        }
        
        if (new_serial.length() == 0) {
            Serial.println("❌ Serial number cannot be empty");
            return;
        }
        
        if (new_serial.length() > 32) {
            Serial.println("❌ Serial number too long (max 32 characters)");
            return;
        }
        
        ConfigManager::getInstance().updateSerial(new_serial);
        Serial.printf("✓ Serial number updated: %s\n", new_serial.c_str());
    }
    else if (args == "" || args == "show") {
        auto creds = ConfigManager::getInstance().getCredentials();
        Serial.printf("Current serial number: %s\n", creds.serial.c_str());
    }
    else {
        Serial.println("Usage: serial, serial set \"string\"");
    }
}

void SerialCommander::help(String args) {
    Serial.println("🎛️  AutoLight V3 SerialCommander Help");
    Serial.println("=====================================");
    
    const char* mode_names[] = {"DISABLED", "READ_ONLY", "SAFE_CONTROL", "FULL_ACCESS"};
    Serial.printf("Current Mode: %s\n", mode_names[current_mode_]);
    if (current_mode_ > SERIAL_READ_ONLY && activation_timeout_ > 0) {
        uint32_t remaining = (TIMEOUT_MS - (millis() - activation_timeout_)) / 1000;
        Serial.printf("Auto-deactivate in: %d seconds\n", remaining);
    }
    
    auto creds = ConfigManager::getInstance().getCredentials();
    Serial.printf("WiFi SSID: %s\n", creds.ssid.c_str());
    Serial.printf("WiFi Password: %s\n", creds.password.length() > 0 ? "****" : "(open)");
    Serial.printf("Serial Number: %s\n", creds.serial.c_str());
    Serial.println();
    
    Serial.println("🔐 Activation Commands:");
    Serial.println("  activate safe          - Enable LED/sequence control");
    Serial.println("  activate full          - Enable all commands (including tests)");
    Serial.println("  deactivate             - Disable control commands");
    Serial.println("  help                   - Show this help\n");
    
    if (current_mode_ >= SERIAL_READ_ONLY) {
        Serial.println("📶 WiFi AP Commands:");
        Serial.println("  wifi                    - Show current AP status");
        Serial.println("  wifi set \"ssid\" \"pass\" - Set new AP credentials");  
        Serial.println("  wifi restart           - Restart AP with current settings");
        Serial.println("  wifi reset             - Reset to default settings");
        Serial.println("  wifi clients           - Show connected clients\n");
        
        Serial.println("🔢 Serial Number Commands:");
        Serial.println("  serial                 - Show current serial number");
        Serial.println("  serial set \"string\"    - Set device serial number (e.g., serial set \"ALS024041\")\n");
    }
    
    if (current_mode_ >= SERIAL_SAFE_CONTROL) {
        Serial.println("💡 LED Channel Commands:");
        Serial.println("  led <ch> on            - Turn on specific channel");
        Serial.println("  led <ch> off           - Turn off specific channel");
        Serial.println("  led status             - Show all channel states\n");
        
        Serial.println("🌈 Sequence Commands:");
        Serial.println("  seq <mode>             - Set sequence mode (0-15)");
        Serial.println("  seq next               - Go to next sequence");
        Serial.println("  seq prev               - Go to previous sequence");
        Serial.println("  seq list               - List all available sequences");
        Serial.println("  seq speed <ms>         - Set sequence speed (10-2000ms)\n");
        
        Serial.println("👁️  Visualization Commands:");
        Serial.println("  viz on                 - Enable LED matrix visualization");
        Serial.println("  viz off                - Disable visualization");
        Serial.println("  viz refresh            - Force visualization update");
        Serial.println("  viz layout <r> <c>     - Change matrix layout\n");
        
        Serial.println("🔘 Button Commands:");
        Serial.println("  btn <id>               - Simulate button press (0-3)");
        Serial.println("  btn mode 1button       - Single button cycling mode");
        Serial.println("  btn mode 2button       - Two button mode");
        Serial.println("  btn mode 3button       - Three button mode");
        Serial.println("  btn mode 4button       - Four button mode\n");
    }
    
    if (current_mode_ >= SERIAL_FULL_ACCESS) {
        Serial.println("⚠️  Hardware Test Commands:");
        Serial.println("  led test <ch>          - Test specific channel (0-11)");
        Serial.println("  led test all           - Test all channels sequentially\n");
        
        Serial.println("🔌 PCF8574 I2C Commands:");
        Serial.println("  pcf scan               - Scan I2C bus for PCF8574 devices");
        Serial.println("  pcf test <addr>        - Test specific PCF (e.g., 0x20)");
        Serial.println("  pcf status             - Show all PCF states");
        Serial.println("  pcf reset <addr>       - Reset PCF (all pins OFF)\n");
    }
    
    if (current_mode_ == SERIAL_READ_ONLY) {
        Serial.println("ℹ️  Use 'activate safe' to enable LED control commands");
        Serial.println("ℹ️  Use 'activate full' to enable hardware test commands\n");
    }
    
    Serial.println("💡 Examples:");
    if (current_mode_ >= SERIAL_SAFE_CONTROL) {
        Serial.println("  seq 3                  - Set chase pattern");
        Serial.println("  viz layout 2 6         - 2x6 matrix layout");
    }
    if (current_mode_ >= SERIAL_FULL_ACCESS) {
        Serial.println("  led test 5             - Test channel 5");
    }
    Serial.println("  wifi set \"MyAP\" \"\"     - Create open AP");
    Serial.println("=====================================");
}

void SerialCommander::testChannel(uint8_t channel) {
    Serial.printf("Ch%d: ", channel);
    led_->setStateHigh(channel, -1);
    delay(300);
    Serial.print("ON ");
    led_->setStateLow(channel, -1);
    delay(100);
    Serial.println("✓");
}

void SerialCommander::showChannelStatus() {
    Serial.println("📊 Channel Status:");
    Serial.println("Ch | State | PCF  | Pin");
    Serial.println("---|-------|------|----");
    
    for (uint8_t i = 0; i < led_->getTotalLEDs(); i++) {
        bool state = led_->getLEDState(i);
        Serial.printf("%2d | %-5s | 0x%02X | P%d\n", 
            i, state ? "ON" : "OFF", 0x20 + (i / 8), i % 8);
    }
    Serial.println();
}

void SerialCommander::showWiFiInfo() {
    Serial.println("📶 WiFi AP Status:");
    Serial.printf("  SSID: %s\n", ap_config_.ssid);
    Serial.printf("  Security: %s\n", strlen(ap_config_.password) == 0 ? "Open" : "WPA2");
    Serial.printf("  IP Address: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("  Connected Clients: %d\n", WiFi.softAPgetStationNum());
    Serial.printf("  Config Source: %s\n", 
        ap_config_.is_loaded_from_preferences ? "Preferences" : "Default");
    Serial.println("  Web Interface: http://" + WiFi.softAPIP().toString());
    Serial.println("  mDNS: http://als.local");
}

void SerialCommander::showConnectedClients() {
    uint8_t client_count = WiFi.softAPgetStationNum();
    Serial.printf("👥 Connected Clients: %d\n", client_count);
}

void SerialCommander::restartAP() {
    Serial.println("🔄 Restarting AP...");
    WiFi.softAPdisconnect(true);
    delay(1000);
    startAccessPoint();
}

void SerialCommander::resetToDefaults() {
    Preferences prefs;
    prefs.begin("ap_config", false);
    prefs.clear();
    prefs.end();
    
    strcpy(ap_config_.ssid, "AutoLightV3");
    strcpy(ap_config_.password, "");
    ap_config_.is_loaded_from_preferences = false;
    
    Serial.println("✓ Reset to default AP settings");
    Serial.println("  Use 'wifi restart' to apply");
}

uint8_t SerialCommander::parseHexAddress(String args) {
    if (args.startsWith("0x") || args.startsWith("0X")) {
        return (uint8_t)strtol(args.c_str(), NULL, 16);
    }
    return (uint8_t)args.toInt();
}

void SerialCommander::scanI2CBus() {
    Serial.println("🔍 Scanning I2C bus for PCF8574 devices...");
    Serial.println("Address | Status");
    Serial.println("--------|--------");
    
    uint8_t found = 0;
    for (uint8_t addr = 0x20; addr <= 0x27; addr++) {
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.printf("0x%02X    | ✓ Found\n", addr);
            found++;
        } else {
            Serial.printf("0x%02X    | - None\n", addr);
        }
    }
    
    Serial.printf("\n✅ Scan complete: %d PCF8574 devices found\n", found);
}

void SerialCommander::testSinglePCF(uint8_t address) {
    Serial.printf("🔧 Testing PCF8574 at 0x%02X...\n", address);
    
    Wire.beginTransmission(address);
    if (Wire.endTransmission() != 0) {
        Serial.printf("❌ PCF8574 not found at 0x%02X\n", address);
        return;
    }
    
    Serial.println("  Testing all pins:");
    for (uint8_t pin = 0; pin < 8; pin++) {
        Serial.printf("    Pin P%d: ", pin);
        
        uint8_t test_value = ~(1 << pin);
        if (writeToI2C(address, test_value)) {
            Serial.print("ON  ");
            delay(300);
            writeToI2C(address, 0xFF);
            Serial.println("✓");
            delay(100);
        } else {
            Serial.println("❌");
        }
    }
    
    Serial.println("  All pins blink test:");
    for (int i = 0; i < 3; i++) {
        writeToI2C(address, 0x00);
        delay(200);
        writeToI2C(address, 0xFF);
        delay(200);
    }
    
    Serial.printf("✅ PCF8574 0x%02X test completed!\n", address);
}

void SerialCommander::showAllPCFStatus() {
    if (!config_) {
        Serial.println("❌ Config not initialized");
        return;
    }
    
    Serial.println("📊 PCF8574 Status:");
    Serial.println("Addr | Channels | State");
    Serial.println("-----|----------|--------");
    
    DynamicConfigData* dyn_config = config_->getCurrentDistribution();
    if (dyn_config) {
        for (uint8_t i = 0; i < dyn_config->pcf_count; i++) {
            PCFDistribution& pcf = dyn_config->pcf_list[i];
            uint8_t state = readFromI2C(pcf.address);
            
            Serial.printf("0x%02X | %8d | ", pcf.address, pcf.used_pins);
            for (int bit = 7; bit >= 0; bit--) {
                Serial.print((~state & (1 << bit)) ? "1" : "0");
            }
            Serial.println();
        }
    }
}

void SerialCommander::resetPCF(uint8_t address) {
    if (writeToI2C(address, 0xFF)) {
        Serial.printf("✓ PCF8574 0x%02X reset (all pins OFF)\n", address);
    } else {
        Serial.printf("❌ Failed to reset PCF8574 0x%02X\n", address);
    }
}

bool SerialCommander::writeToI2C(uint8_t address, uint8_t data) {
    Wire.beginTransmission(address);
    Wire.write(data);
    return Wire.endTransmission() == 0;
}

uint8_t SerialCommander::readFromI2C(uint8_t address) {
    Wire.requestFrom(address, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0xFF;
}

void SerialCommander::setMode(SerialMode mode) {
    current_mode_ = mode;
    if (mode > SERIAL_READ_ONLY) {
        activation_timeout_ = millis();
    }
}

SerialMode SerialCommander::getMode() const {
    return current_mode_;
}

bool SerialCommander::isCommandAllowed(const String& command_type) {
    if (command_type == "help" || command_type == "activate" || command_type == "deactivate") {
        return true;
    }
    if (current_mode_ == SERIAL_DISABLED) {
        return false;
    }
    if (command_type == "wifi" || command_type == "viz" || command_type == "seq list" || command_type == "serial") {
        return current_mode_ >= SERIAL_READ_ONLY;
    }
    if (command_type == "led" || command_type == "seq" || command_type == "btn") {
        return current_mode_ >= SERIAL_SAFE_CONTROL;
    }
    if (command_type == "pcf" || command_type == "led test") {
        return current_mode_ >= SERIAL_FULL_ACCESS;
    }
    return false;
}

void SerialCommander::activate(String args) {
    args.trim();
    if (args == "safe") {
        setMode(SERIAL_SAFE_CONTROL);
    } else if (args == "full") {
        setMode(SERIAL_FULL_ACCESS);
    } else {
        current_mode_ = SERIAL_READ_ONLY;
    }
}

void SerialCommander::deactivate(String args) {
    setMode(SERIAL_READ_ONLY);
}

void SerialCommander::checkTimeout() {
    if (current_mode_ > SERIAL_READ_ONLY && activation_timeout_ > 0) {
        if (millis() - activation_timeout_ > TIMEOUT_MS) {
            setMode(SERIAL_READ_ONLY);
        }
    }
}

}