#ifndef VIRTUAL_PCF_H
#define VIRTUAL_PCF_H

#include <Arduino.h>
#include <Wire.h>

class VirtualPCF {
private:
    uint8_t address;
    uint8_t pin_states[8];
    bool is_active;
    String label;

public:
    VirtualPCF(uint8_t addr, String lbl = "");

    void writePin(uint8_t pin, uint8_t state);
    void writeByte(uint8_t data);
    uint8_t readPin(uint8_t pin);
    uint8_t readByte();

    uint8_t getAddress() const { return address; }

    String getLabel() const { return label; }

    bool isActive() const { return is_active; }

    void setActive(bool active) { is_active = active; }

    void printState();
    void clearPins();
};

class VirtualPCFManager {
private:
    static VirtualPCF *pcf_devices[16];
    static uint8_t active_pcf_count;
    static uint8_t current_address;
    static bool simulation_enabled;

public:
    static void enableSimulation();
    static void disableSimulation();
    static bool isSimulationEnabled();

    static void addPCF(uint8_t address, String label = "");
    static void removePCF(uint8_t address);
    static VirtualPCF *getPCF(uint8_t address);
    static uint8_t getPCFCount();

    static void begin();
    static void beginTransmission(uint8_t address);
    static size_t write(uint8_t data);
    static uint8_t endTransmission();

    static void printAllPCFs();
    static void printPCFLayout();
    static void clearAllPCFs();

    static void testPCF(uint8_t address);
    static void testAllPins(uint8_t address);
};

#endif