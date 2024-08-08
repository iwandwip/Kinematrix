/*  https://randomnerdtutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
 *  telegram-bot.h
 *
 *  telegram-bot module
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef TELEGRAM_BOT_HANDLER_H
#define TELEGRAM_BOT_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

#pragma message("[COMPILED]: telegram-bot.h")

#include "Arduino.h"

#ifdef ESP32

#include "WiFi.h"

#else
#include "ESP8266WiFi.h"
#endif

#include "WiFiClientSecure.h"
#include "UniversalTelegramBot.h"
#include "ArduinoJson.h"
#include "WiFiManager.h"

class TelegramBot : public UniversalTelegramBot {
private:
    String botChatId;
    String botToken;
    uint32_t botTimer;

    using UniversalTelegramBot::UniversalTelegramBot;
public:
    void begin(void (*onBegin)() = nullptr);
    void setChatId(const String &chatId);
    void handleNewMessages(int numNewMessages, void (*onReceive)(const String &, const String &),
                           void (*onError)() = nullptr);
    void onReceiveMessages(uint32_t time, void (*onReceive)(const String &, const String &),
                           void (*onError)() = nullptr);
    void sendMessages(const String& msg);
public:
};

#endif
#endif