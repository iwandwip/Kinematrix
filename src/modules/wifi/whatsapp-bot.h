/*
 *  whatsapp-bot.h
 *
 *  whatsapp-bot module
 *  Created on: 2023. 4. 3
 */
#ifdef USE_WHATSAPP_BOT
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


WhatsappBot::WhatsappBot() {

}

WhatsappBot::WhatsappBot(String _phoneNumber, String _apiKey) {
    this->init(_phoneNumber, _apiKey);
}

bool WhatsappBot::init(String _phoneNumber, String _apiKey) {
    phoneNumber = _phoneNumber;
    apiKey = _apiKey;
    return true;
}

bool WhatsappBot::sendMessages(String _message, void (*resultCb)(String, String)) {
    // Data to send with HTTP POST
    String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" +
                 urlEncode(_message);
    String res, err;
    HTTPClient http;
    http.begin(url);
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Send HTTP POST request
    int httpResponseCode = http.POST(url);
    if (httpResponseCode == 200) {
        res = "Message sent successfully";
    } else {
        err = "Error sending the message, HTTP response code: " + String(httpResponseCode);
    }
    if (resultCb != nullptr) resultCb(res, err);
    // Free resources
    http.end();
    return httpResponseCode == 200;
}

void WhatsappBot::resultStatusCallback(String res, String err) {
    if (!err.isEmpty()) {
        Serial.print("| err: ");
        Serial.print(err);
        Serial.println();
    }
}

#endif
#endif
#endif