/*
 *  whatsapp_bot_handler.h
 *
 *  whatsapp_bot handler module
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef WHATSAPP_BOT_HANDLER_H
#define WHATSAPP_BOT_HANDLER_H

#if defined(ESP32) || defined(ESP8266)

#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"
#include "UrlEncode.h"

class WhatsappBot {
private:
    String phoneNumber;
    String apiKey;

public:
    WhatsappBot();
    WhatsappBot(String _phoneNumber, String _apiKey);
    bool init(String _phoneNumber, String _apiKey);
    bool sendMessages(String _message, void (*resultCb)(String, String) = nullptr);
    static void resultStatusCallback(String res, String err);
};

#endif
#endif