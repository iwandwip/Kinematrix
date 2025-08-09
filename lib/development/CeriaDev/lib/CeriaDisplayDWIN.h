#pragma once

#ifndef CERIA_DISPLAY_DWIN_H
#define CERIA_DISPLAY_DWIN_H

#pragma message("[COMPILED]: CeriaDisplayDWIN.h")

#include "Arduino.h"
#include "Stream.h"

#define DWIN_HEADER_H                    0x5A
#define DWIN_HEADER_L                    0xA5
#define DWIN_CMD_REG_WRITE              0x80
#define DWIN_CMD_REG_READ               0x81
#define DWIN_CMD_VAR_WRITE              0x82
#define DWIN_CMD_VAR_READ               0x83
#define DWIN_CMD_CURVE_WRITE            0x84
#define DWIN_CMD_TOUCH_EVENT            0x85

#define DWIN_REG_PAGE                   0x03
#define DWIN_REG_BRIGHTNESS             0x01
#define DWIN_REG_VOLUME                 0x02
#define DWIN_REG_RTC                    0x10

#define DWIN_MAX_VARIABLES              32
#define DWIN_MAX_PAGES                  255
#define DWIN_FRAME_TIMEOUT              100
#define DWIN_DEFAULT_TIMEOUT            2000

namespace CeriaDevOP {

    enum DWINDataType : uint8_t {
        DWIN_TYPE_UINT8 = 0,
        DWIN_TYPE_UINT16 = 1,
        DWIN_TYPE_UINT32 = 2,
        DWIN_TYPE_FLOAT = 3,
        DWIN_TYPE_STRING = 4,
        DWIN_TYPE_BINARY = 5
    };

    enum DWINError : uint8_t {
        DWIN_ERR_NONE = 0,
        DWIN_ERR_TIMEOUT,
        DWIN_ERR_INVALID_FRAME,
        DWIN_ERR_BUFFER_FULL,
        DWIN_ERR_NO_CONNECTION,
        DWIN_ERR_INVALID_VP,
        DWIN_ERR_INVALID_PAGE,
        DWIN_ERR_CRC_FAILED
    };

    enum DWINTouchEvent : uint8_t {
        DWIN_TOUCH_NONE = 0,
        DWIN_TOUCH_PRESS = 1,
        DWIN_TOUCH_RELEASE = 2,
        DWIN_TOUCH_LONG_PRESS = 3,
        DWIN_TOUCH_DRAG = 4
    };

    struct DWINVariablePointer {
        uint16_t address;
        DWINDataType dataType;
        uint16_t value16;
        uint32_t value32;
        float valueFloat;
        char valueString[32];
        uint32_t lastUpdate;
        bool updated;
        bool autoUpload;
    };

    struct DWINTouchData {
        uint16_t vp;
        uint16_t x;
        uint16_t y;
        DWINTouchEvent event;
        uint32_t timestamp;
    };

    struct DWINPageData {
        uint8_t currentPage;
        uint8_t previousPage;
        uint32_t pageChangeTime;
        bool pageChanged;
    };

    typedef void (*DWINTouchCallback)(uint16_t vp, uint16_t value, DWINTouchEvent event);
    typedef void (*DWINPageCallback)(uint8_t newPage, uint8_t oldPage);
    typedef void (*DWINErrorCallback)(DWINError error);
    typedef void (*DWINVariableCallback)(uint16_t vp, DWINDataType type, uint32_t value);

    class CeriaDisplayDWIN {
    private:
        Stream *_serial;
        uint32_t _baudRate;
        uint32_t _timeout;

        uint8_t _rxBuffer[64];
        uint8_t _txBuffer[64];
        uint8_t _rxIndex;
        bool _frameComplete;
        uint32_t _lastFrameTime;

        DWINVariablePointer _variables[DWIN_MAX_VARIABLES];
        uint8_t _variableCount;

        DWINPageData _pageData;
        DWINTouchData _touchData;

        uint32_t _lastHeartbeat;
        uint32_t _lastActivity;
        bool _connected;

        DWINTouchCallback _onTouch;
        DWINPageCallback _onPageChange;
        DWINErrorCallback _onError;
        DWINVariableCallback _onVariable;

        uint32_t _framesSent;
        uint32_t _framesReceived;
        uint32_t _errorCount;
        uint8_t _currentBrightness;
        uint8_t _currentVolume;

        bool initializeDisplay();
        void processIncomingByte(uint8_t byte);
        void processCompleteFrame();
        bool validateFrame(uint8_t *buffer, uint8_t length);
        uint16_t calculateCRC16(uint8_t *data, uint8_t length);

        bool sendCommand(uint8_t cmd, uint16_t address, uint8_t *data, uint8_t dataLength);
        bool sendRegisterWrite(uint8_t reg, uint16_t value);
        bool sendRegisterRead(uint8_t reg);
        bool sendVariableWrite(uint16_t vp, uint8_t *data, uint8_t length);
        bool sendVariableRead(uint16_t vp, uint8_t length);

        int8_t findVariableIndex(uint16_t vp);
        bool addVariable(uint16_t vp, DWINDataType type, bool autoUpload = false);
        void updateVariableValue(uint8_t index, uint8_t *data, uint8_t length);

        void handleTouchEvent(uint8_t *data, uint8_t length);
        void handlePageChange(uint8_t *data, uint8_t length);
        void handleVariableUpdate(uint8_t *data, uint8_t length);
        void triggerError(DWINError error);

        void encodeString(const String &str, uint8_t *buffer, uint8_t maxLength);
        void decodeString(uint8_t *buffer, uint8_t length, char *output);
        uint32_t bytesToUint32(uint8_t *data);
        void uint32ToBytes(uint32_t value, uint8_t *data);
        uint16_t bytesToUint16(uint8_t *data);
        void uint16ToBytes(uint16_t value, uint8_t *data);
        float bytesToFloat(uint8_t *data);
        void floatToBytes(float value, uint8_t *data);

    public:
        CeriaDisplayDWIN();
        ~CeriaDisplayDWIN();

        bool begin(Stream &serial, uint32_t baud = 115200);
        void update();

        void tick() { update(); }

        bool hasNewData();
        bool hasNewTouch();
        bool hasNewPage();
        bool isConnected(uint32_t timeoutMs = 5000);
        void setTimeout(uint32_t ms);

        void setTouchCallback(DWINTouchCallback callback);
        void setPageCallback(DWINPageCallback callback);
        void setErrorCallback(DWINErrorCallback callback);
        void setVariableCallback(DWINVariableCallback callback);

        bool setPage(uint8_t page);
        uint8_t getCurrentPage();
        uint8_t getPreviousPage();
        bool waitForPageChange(uint32_t timeoutMs = 3000);

        bool setBrightness(uint8_t level);
        uint8_t getBrightness();
        bool setVolume(uint8_t level);
        uint8_t getVolume();

        bool writeVP(uint16_t vp, uint8_t value);
        bool writeVP(uint16_t vp, uint16_t value);
        bool writeVP(uint16_t vp, uint32_t value);
        bool writeVP(uint16_t vp, int value);
        bool writeVP(uint16_t vp, long value);
        bool writeVP(uint16_t vp, float value);
        bool writeVP(uint16_t vp, const String &value);
        bool writeVP(uint16_t vp, const char *value);

        bool readVP(uint16_t vp, DWINDataType type);
        uint8_t getVPUint8(uint16_t vp);
        uint16_t getVPUint16(uint16_t vp);
        uint32_t getVPUint32(uint16_t vp);
        float getVPFloat(uint16_t vp);
        String getVPString(uint16_t vp);

        bool registerVP(uint16_t vp, DWINDataType type, bool autoUpload = false);
        bool unregisterVP(uint16_t vp);
        bool isVPRegistered(uint16_t vp);
        bool isVPUpdated(uint16_t vp);
        uint32_t getVPLastUpdate(uint16_t vp);

        DWINTouchData getLastTouch();
        uint16_t getTouchVP();
        uint16_t getTouchX();
        uint16_t getTouchY();
        DWINTouchEvent getTouchEvent();

        bool setText(uint16_t vp, const String &text);
        bool setNumber(uint16_t vp, int32_t number);
        bool setFloat(uint16_t vp, float number, uint8_t decimals = 2);
        bool setIcon(uint16_t vp, uint16_t iconId);
        bool setProgress(uint16_t vp, uint8_t percentage);

        bool enableAutoUpload(uint16_t vp);
        bool disableAutoUpload(uint16_t vp);
        void enableHeartbeat(bool enable = true);
        void sendHeartbeat();

        void getStats(uint32_t &sent, uint32_t &received, uint32_t &errors);
        void resetStats();
        String getDisplayString();
        String getConnectionString();
        String getPageString();
        String getTouchString();

        bool playSound(uint8_t soundId);
        bool setRTC(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
        bool getRTC();

        bool waitForConnection(uint32_t timeoutMs = 10000);
        bool waitForTouch(uint32_t timeoutMs = 5000);
        bool waitForVPUpdate(uint16_t vp, uint32_t timeoutMs = 3000);

        void reset();
        void factoryReset();

        bool uploadFile(const String &filename, uint8_t *data, uint32_t length);
        bool downloadFile(const String &filename, uint8_t *buffer, uint32_t &length);

        String getVersionString();
        String getDebugString();
    };

#if defined(ESP32)
#define CERIA_DWIN_DEFAULT_SERIAL    Serial2
#define CERIA_DWIN_DEFAULT_RX        16
#define CERIA_DWIN_DEFAULT_TX        17
#elif defined(ESP8266)
#define CERIA_DWIN_DEFAULT_SERIAL    Serial
#define CERIA_DWIN_DEFAULT_RX        D6
#define CERIA_DWIN_DEFAULT_TX        D7
#else
#define CERIA_DWIN_DEFAULT_SERIAL    Serial1
#define CERIA_DWIN_DEFAULT_RX        2
#define CERIA_DWIN_DEFAULT_TX        3
#endif

}

#endif