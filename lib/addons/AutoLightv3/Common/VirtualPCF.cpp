#include "VirtualPCF.h"

VirtualPCF *VirtualPCFManager::pcf_devices[16] = {nullptr};
uint8_t VirtualPCFManager::active_pcf_count = 0;
uint8_t VirtualPCFManager::current_address = 0;
bool VirtualPCFManager::simulation_enabled = false;

VirtualPCF::VirtualPCF(uint8_t addr, String lbl)
        : address(addr), is_active(true), label(lbl) {
    clearPins();
}

void VirtualPCF::writePin(uint8_t pin, uint8_t state) {
    if (pin < 8) {
        pin_states[pin] = state ? 1 : 0;
    }
}

void VirtualPCF::writeByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        pin_states[i] = (data >> i) & 0x01;
    }
}

uint8_t VirtualPCF::readPin(uint8_t pin) {
    return (pin < 8) ? pin_states[pin] : 0;
}

uint8_t VirtualPCF::readByte() {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        if (pin_states[i]) result |= (1 << i);
    }
    return result;
}

void VirtualPCF::printState() {
    Serial.printf("PCF 0x%02X", address);
    if (label.length() > 0) {
        Serial.printf(" (%s)", label.c_str());
    }
    Serial.println(":");

    Serial.print("  P0 P1 P2 P3 P4 P5 P6 P7\n  ");
    for (int i = 0; i < 8; i++) {
        Serial.printf("%s  ", pin_states[i] ? "#" : ".");
    }
    Serial.println();
}

void VirtualPCF::clearPins() {
    memset(pin_states, 0, 8);
}

void VirtualPCFManager::enableSimulation() {
    simulation_enabled = true;
    Serial.println("[VIRTUAL PCF] Simulation mode enabled");
}

void VirtualPCFManager::disableSimulation() {
    simulation_enabled = false;
    Serial.println("[VIRTUAL PCF] Hardware mode enabled");
}

bool VirtualPCFManager::isSimulationEnabled() {
    return simulation_enabled;
}

void VirtualPCFManager::addPCF(uint8_t address, String label) {
    if (active_pcf_count >= 16) {
        Serial.println("[VIRTUAL PCF] Maximum PCF count reached!");
        return;
    }

    for (int i = 0; i < active_pcf_count; i++) {
        if (pcf_devices[i] && pcf_devices[i]->getAddress() == address) {
            Serial.printf("[VIRTUAL PCF] PCF 0x%02X already exists!\n", address);
            return;
        }
    }

    pcf_devices[active_pcf_count] = new VirtualPCF(address, label);
    active_pcf_count++;

    Serial.printf("[VIRTUAL PCF] Added PCF 0x%02X", address);
    if (label.length() > 0) {
        Serial.printf(" (%s)", label.c_str());
    }
    Serial.println();
}

void VirtualPCFManager::removePCF(uint8_t address) {
    for (int i = 0; i < active_pcf_count; i++) {
        if (pcf_devices[i] && pcf_devices[i]->getAddress() == address) {
            delete pcf_devices[i];

            for (int j = i; j < active_pcf_count - 1; j++) {
                pcf_devices[j] = pcf_devices[j + 1];
            }
            pcf_devices[active_pcf_count - 1] = nullptr;
            active_pcf_count--;

            Serial.printf("[VIRTUAL PCF] Removed PCF 0x%02X\n", address);
            return;
        }
    }
    Serial.printf("[VIRTUAL PCF] PCF 0x%02X not found!\n", address);
}

VirtualPCF *VirtualPCFManager::getPCF(uint8_t address) {
    for (int i = 0; i < active_pcf_count; i++) {
        if (pcf_devices[i] && pcf_devices[i]->getAddress() == address) {
            return pcf_devices[i];
        }
    }
    return nullptr;
}

uint8_t VirtualPCFManager::getPCFCount() {
    return active_pcf_count;
}

void VirtualPCFManager::begin() {
    if (simulation_enabled) {
        Serial.println("[VIRTUAL PCF] Virtual I2C initialized");
        return;
    }
    Wire.begin();
}

void VirtualPCFManager::beginTransmission(uint8_t address) {
    if (simulation_enabled) {
        current_address = address;
        return;
    }
    Wire.beginTransmission(address);
}

size_t VirtualPCFManager::write(uint8_t data) {
    if (simulation_enabled) {
        VirtualPCF *pcf = getPCF(current_address);
        if (pcf && pcf->isActive()) {
            pcf->writeByte(data);
            return 1;
        }
        return 0;
    }
    return Wire.write(data);
}

uint8_t VirtualPCFManager::endTransmission() {
    if (simulation_enabled) {
        VirtualPCF *pcf = getPCF(current_address);
        if (pcf && pcf->isActive()) {
            return 0;
        } else {
            return 2;
        }
    }
    return Wire.endTransmission();
}

void VirtualPCFManager::printAllPCFs() {
    if (!simulation_enabled) {
        Serial.println("[VIRTUAL PCF] Not in simulation mode");
        return;
    }

    Serial.println("\n╔═══ Virtual PCF Status ═══╗");
    for (int i = 0; i < active_pcf_count; i++) {
        if (pcf_devices[i]) {
            pcf_devices[i]->printState();
        }
    }
    Serial.println("╚═══════════════════════════╝");
}

void VirtualPCFManager::printPCFLayout() {
    Serial.printf("\n[VIRTUAL PCF] Layout: %d active PCFs\n", active_pcf_count);
    for (int i = 0; i < active_pcf_count; i++) {
        if (pcf_devices[i]) {
            Serial.printf("  • PCF 0x%02X: %s (8 pins)\n",
                          pcf_devices[i]->getAddress(),
                          pcf_devices[i]->getLabel().c_str());
        }
    }
}

void VirtualPCFManager::clearAllPCFs() {
    for (int i = 0; i < active_pcf_count; i++) {
        if (pcf_devices[i]) {
            pcf_devices[i]->clearPins();
        }
    }
    Serial.println("[VIRTUAL PCF] All PCFs cleared");
}

void VirtualPCFManager::testPCF(uint8_t address) {
    VirtualPCF *pcf = getPCF(address);
    if (!pcf) {
        Serial.printf("[VIRTUAL PCF] PCF 0x%02X not found!\n", address);
        return;
    }

    Serial.printf("[VIRTUAL PCF] Testing PCF 0x%02X...\n", address);

    pcf->writeByte(0xFF);
    Serial.print("All ON:  ");
    pcf->printState();
    delay(500);

    pcf->writeByte(0x00);
    Serial.print("All OFF: ");
    pcf->printState();
}

void VirtualPCFManager::testAllPins(uint8_t address) {
    VirtualPCF *pcf = getPCF(address);
    if (!pcf) {
        Serial.printf("[VIRTUAL PCF] PCF 0x%02X not found!\n", address);
        return;
    }

    Serial.printf("[VIRTUAL PCF] Testing individual pins on PCF 0x%02X...\n", address);

    for (int pin = 0; pin < 8; pin++) {
        pcf->clearPins();
        pcf->writePin(pin, HIGH);
        Serial.printf("Pin P%d: ", pin);
        pcf->printState();
        delay(300);
    }

    pcf->clearPins();
    Serial.print("Cleared: ");
    pcf->printState();
}
