/*  // https://stackoverflow.com/questions/77746047/sending-data-with-esp32-to-firebase-error-invalid-data-couldnt-parse-json-obj
 *  gm67-sens.h
 *
 *  gm67 sensor lib
 *  Created on: 2023. 4. 3
 */

#pragma once

#ifndef GM67_SENS_H
#define GM67_SENS_H

#pragma message("[COMPILED]: gm67-sens.h")

#include "Arduino.h"
#include "base/sensor-module.h"

/* GPIO TABLE FOR ESP32
 * GPIO	FUNCTION	GPIO	FUNCTION
 * GPIO32	RX/TX	GPIO15	RX/TX
 * GPIO33	RX/TX	GPIO21	RX/TX
 * GPIO25	RX/TX	GPIO22	RX/TX
 * GPIO26	RX/TX	GPIO17	RX/TX
 * GPIO27	RX/TX	GPIO16	RX/TX
 * GPIO14	RX/TX
 * GPIO13	RX/TX
 */

class GM67Sens : public BaseSens {
private:
    JsonDocument *doc;
    const char *name;

#if defined(ESP32)
    HardwareSerial *serialPtr;
#endif

    uint8_t sensorPin;
    uint32_t sensorTimer;

public:
#if defined(ESP32)
    explicit GM67Sens(HardwareSerial *_serialPtr = &Serial2, long baud = 9600,
                      SerialConfig cfg = SERIAL_8N1, int8_t rxPin = 15, int8_t txPin = 14);
#endif
    virtual ~GM67Sens();
    bool init() override;
    bool update() override;

    void setDocument(const char *objName) override;
    void setDocumentValue(JsonDocument *docBase) override;
    JsonDocument getDocument() override;
    JsonVariant getVariant(const char *searchName) override;

    float getValueGM67Sens() const;
    void setPins(uint8_t _pin);
};

#endif  // GM67_SENS_H