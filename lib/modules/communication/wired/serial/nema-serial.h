#ifndef KINEMATRIX_NEMA_SERIAL
#define KINEMATRIX_NEMA_SERIAL

#include "Arduino.h"
#include "ArduinoJson.h"

#define PKT_START_MARKER      0x7E
#define PKT_END_MARKER        0x7F
#define PKT_ESCAPE_CHAR       0x7D
#define PKT_XOR_CHAR          0x20

#define CMD_ACK               0x06
#define CMD_NAK               0x15
#define CMD_PING              0x01
#define CMD_DATA              0x02
#define CMD_REGISTER_READ     0x03
#define CMD_REGISTER_WRITE    0x04
#define CMD_JSON_DATA         0x10
#define CMD_MIXED_DATA        0x11
#define CMD_CUSTOM_START      0x40

#define DATA_TYPE_INT8        0x01
#define DATA_TYPE_INT16       0x02
#define DATA_TYPE_INT32       0x03
#define DATA_TYPE_UINT8       0x04
#define DATA_TYPE_UINT16      0x05
#define DATA_TYPE_UINT32      0x06
#define DATA_TYPE_FLOAT       0x07
#define DATA_TYPE_STRING      0x08
#define DATA_TYPE_BOOL        0x09

#define ERR_NONE              0x00
#define ERR_TIMEOUT           0x01
#define ERR_CRC               0x02
#define ERR_LENGTH            0x03
#define ERR_COMMAND           0x04
#define ERR_BUFFER_OVERFLOW   0x06
#define ERR_FORMAT            0x07
#define ERR_SEQUENCE          0x08
#define ERR_BUSY              0x09
#define ERR_JSON              0x0A
#define ERR_MEMORY            0x0B

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
#define DEFAULT_BUFFER_SIZE 64
#define DEFAULT_CHUNK_SIZE 16
#elif defined(ESP8266)
#define DEFAULT_BUFFER_SIZE 256
#define DEFAULT_CHUNK_SIZE 64
#elif defined(ESP32)
#define DEFAULT_BUFFER_SIZE 512
#define DEFAULT_CHUNK_SIZE 128
#else
#define DEFAULT_BUFFER_SIZE 128
#define DEFAULT_CHUNK_SIZE 32
#endif

#define PKT_HEADER_SIZE       3
#define PKT_MAX_STRING_LEN    64

#define DEBUG_NONE            0
#define DEBUG_ERRORS          1
#define DEBUG_INFO            2
#define DEBUG_VERBOSE         3

typedef void (*PacketHandlerFunction)(uint8_t command, uint8_t *data, uint16_t length);
typedef void (*ErrorHandlerFunction)(uint8_t errorCode);
typedef void (*JsonHandlerFunction)(JsonDocument &jsonDoc);

class NemaSerial {
private:
    Stream *_serial;
    uint8_t _sequenceNumber;

    uint8_t *_receiveBuffer;
    uint16_t _receiveBufferSize;
    uint16_t _receiveBufferIndex;

    uint8_t *_transmitBuffer;
    uint16_t _transmitBufferSize;
    uint16_t _transmitBufferIndex;
    uint16_t _transmitBufferCapacity;

    bool _escape;
    bool _inPacket;

    unsigned long _timeout;
    uint8_t _maxRetries;
    bool _requireAck;

    uint8_t _debugLevel;
    Stream *_debugStream;

    uint32_t _packetsSent;
    uint32_t _packetsReceived;
    uint32_t _packetsRetried;
    uint32_t _packetErrors;
    uint8_t _lastError;

    PacketHandlerFunction _packetHandlerCallback;
    ErrorHandlerFunction _errorHandlerCallback;
    JsonHandlerFunction _jsonHandlerCallback;

    uint16_t calculateCRC(uint8_t *buffer, uint16_t length);
    void resetReceiveBuffer();
    bool resizeReceiveBuffer(uint16_t newSize);
    bool resizeTransmitBuffer(uint16_t minRequiredSize);
    void debugPrint(uint8_t level, const char *message);
    void debugPrintHex(uint8_t level, uint8_t *buffer, uint16_t length);
    bool processIncomingByte(uint8_t inByte);
    void sendAck(uint8_t sequenceNumber);
    void sendNak(uint8_t sequenceNumber, uint8_t errorCode);

    bool addToTransmitBuffer(uint8_t *data, uint16_t length);
    void addInt8(int8_t value);
    void addInt16(int16_t value);
    void addInt32(int32_t value);
    void addUInt8(uint8_t value);
    void addUInt16(uint16_t value);
    void addUInt32(uint32_t value);
    void addFloat(float value);
    void addString(const char *str);
    void addBool(bool value);

public:
    NemaSerial();
    ~NemaSerial();

    bool begin(Stream *serialPort, uint16_t receiveBufferSize = DEFAULT_BUFFER_SIZE);
    void end();
    void setTimeout(unsigned long timeout);
    void setMaxRetries(uint8_t retries);
    void setRequireAck(bool require);

    bool setReceiveBufferSize(uint16_t newSize);
    uint16_t getReceiveBufferSize() const;
    uint16_t getTransmitBufferSize() const;

    void setDebugLevel(uint8_t level);
    void setDebugStream(Stream &stream);
    void dumpStats();
    void resetStats();
    uint8_t getLastError() const;
    const char *getErrorString(uint8_t errorCode) const;

    void setPacketHandler(PacketHandlerFunction callback);
    void setErrorHandler(ErrorHandlerFunction callback);
    void setJsonHandler(JsonHandlerFunction callback);

    bool sendPacket(uint8_t command, uint8_t *data, uint16_t length);
    bool receivePacket(uint8_t *command, uint8_t *data, uint16_t *length, unsigned long timeout = 0);
    void processIncoming();

    bool sendCommand(uint8_t command);
    bool ping();
    bool readRegister(uint16_t reg, uint8_t *value);
    bool writeRegister(uint16_t reg, uint8_t value);

    NemaSerial &beginPacket();
    NemaSerial &write8(int8_t value);
    NemaSerial &write16(int16_t value);
    NemaSerial &write32(int32_t value);
    NemaSerial &writeU8(uint8_t value);
    NemaSerial &writeU16(uint16_t value);
    NemaSerial &writeU32(uint32_t value);
    NemaSerial &writeFloat(float value);
    NemaSerial &writeString(const char *str);
    NemaSerial &writeBool(bool value);
    bool endPacket(uint8_t command);

    bool sendJson(JsonDocument &doc, uint8_t command = CMD_JSON_DATA);
    bool parseJson(uint8_t *data, uint16_t length, JsonDocument &doc);

    int8_t readInt8(uint8_t *data, uint16_t *index);
    int16_t readInt16(uint8_t *data, uint16_t *index);
    int32_t readInt32(uint8_t *data, uint16_t *index);
    uint8_t readUInt8(uint8_t *data, uint16_t *index);
    uint16_t readUInt16(uint8_t *data, uint16_t *index);
    uint32_t readUInt32(uint8_t *data, uint16_t *index);
    float readFloat(uint8_t *data, uint16_t *index);
    String readString(uint8_t *data, uint16_t *index);
    bool readBool(uint8_t *data, uint16_t *index);
};

#endif