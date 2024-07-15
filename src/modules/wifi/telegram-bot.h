/*  https://randomnerdtutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
 *  telegram-bot.h
 *
 *  telegram-bot module
 *  Created on: 2023. 4. 3
 */

#ifdef USE_TELEGRAM_BOT
#pragma once

#ifndef TELEGRAM_BOT_HANDLER_H
#define TELEGRAM_BOT_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

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


void TelegramBot::begin(void (*onBegin)()) {
    botTimer = millis();
    if (onBegin != nullptr) onBegin();
}

void TelegramBot::setChatId(const String &chatId) {
    botChatId = chatId;
}

void TelegramBot::handleNewMessages(int numNewMessages, void (*onReceive)(const String &, const String &),
                                    void (*onError)()) {
    for (int i = 0; i < numNewMessages; i++) {
        String chatId = String(UniversalTelegramBot::messages[i].chat_id);
        if (chatId != this->botChatId) {
            if (onError != nullptr) onError();
            else UniversalTelegramBot::sendMessage(chatId, "Unauthorized user", "");
            continue;
        }
        String text = UniversalTelegramBot::messages[i].text;
        String from_name = UniversalTelegramBot::messages[i].from_name;
        onReceive(from_name, text);
    }
}

void TelegramBot::onReceiveMessages(uint32_t time, void (*onReceive)(const String &, const String &),
                                    void (*onError)()) {
    if (millis() - botTimer >= time) {
        int numNewMessages = UniversalTelegramBot::getUpdates(UniversalTelegramBot::last_message_received + 1);
        while (numNewMessages) {
            this->handleNewMessages(numNewMessages, onReceive, onError);
            numNewMessages = UniversalTelegramBot::getUpdates(UniversalTelegramBot::last_message_received + 1);
        }
        botTimer = millis();
    }
}

void TelegramBot::sendMessages(const String &msg) {
    UniversalTelegramBot::sendMessage(botChatId, msg, "");
}

#endif
#endif
#endif
#endif