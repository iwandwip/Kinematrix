/*
 *  telegram-bot.cpp
 *
 *  telegram-bot module
 *  Created on: 2023. 4. 3
 */

#ifdef USE_TELEGRAM_BOT
#include "telegram-bot.h"

#if defined(ESP32) || defined(ESP8266)

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