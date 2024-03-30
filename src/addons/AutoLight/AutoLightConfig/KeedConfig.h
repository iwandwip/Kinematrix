/*
 *  KeedConfig.h
 *
 *  Kastara Electronics Embedded Development
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef KEED_CONFIG_H
#define KEED_CONFIG_H

#include "modules/communication/wired/i2c/io-expander.h"
#include "modules/io/output-module.h"
#include "modules/time/timer-task.h"
#include "KeedDef.h"
#include "KeedDisplay.h"

#include "EEPROM.h"

struct configuration_t {
    volatile uint8_t sequence;
    volatile uint32_t delay_time;
    uint8_t version;
    uint8_t channel;
    uint8_t io_size;
    uint8_t pin_size;
    uint8_t *pin_ptr;
    uint8_t *i2c_ptr;
    uint8_t *pin_sequence;
    bool custom;
    bool custom_seq;
    bool reverse;
    bool display;
    configuration_t();
    void setPins(int size, ...);
    void setAddress(int size, ...);
    void setDelay(uint32_t _time);
    void setPinSequence(int size, ...);
};

class KeedConfiguration {
private:
    configuration_t cfg;
    bool debug;
    bool is_initialize;

public:
    explicit KeedConfiguration(bool _debug = false);
    cfg_error_t initialize(void (*init_callback)() = nullptr);
    cfg_error_t readChannel();
    cfg_error_t readVersion();

    void setConfig(configuration_t _cfg);
    configuration_t getConfig();
    bool isUsingExpander() const;
    bool isInitialize() const;
    configuration_t getConfig() const;
};

#if defined(ESP32)

#include <WiFi.h>

class KeedCore {
private:
    uint8_t index;
    uint32_t stack_depth;
public:
    KeedCore();
    void createCore(uint8_t _core_index, void (*core_callback)(void *pvParameter));
};

class KeedWiFi {
private:
    WiFiServer server;
    String header;
    IPAddress IP;
public:
    KeedWiFi();
    KeedWiFi(const char *_ssid, const char *_password = NULL);
    int createWiFIAP(const char *_ssid, const char *_password = NULL);
    void beginServer();
    void showServerInfo();
    void runServer();
};

#endif

struct indicator_t {
    DigitalOut outs[4];
    TimerTask times[4];
    indicator_t();
    void show(uint8_t _seq);
};

#if defined(ESP32)
#define readMEM(x) EEPROM.readString(x)
#define writeMEM(x, y) EEPROM.writeString(x, y); EEPROM.commit()
#else
void writeMEM(int addrOffset, const String &strToWrite);
String readMEM(int addrOffset);
String memstr(const byte *byteArray, size_t size);
#endif

#endif // KEED_CONFIG_H