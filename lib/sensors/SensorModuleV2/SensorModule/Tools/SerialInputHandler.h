#ifndef SERIAL_INPUT_HANDLER_H
#define SERIAL_INPUT_HANDLER_H

#include <Arduino.h>

enum InputModeType {
    INPUT_MODE_NONE,
    INPUT_MODE_FLOAT,
    INPUT_MODE_INT,
    INPUT_MODE_CHAR,
    INPUT_MODE_YES_NO,
    INPUT_MODE_STRING
};

class SerialInputHandler {
public:
    SerialInputHandler();
    ~SerialInputHandler();

    void begin(Stream *serialPtr, unsigned long timeout = 10000);
    
    float readFloat(const char *prompt, float defaultValue = 0.0, unsigned long timeout = 0);
    int readInt(const char *prompt, int defaultValue = 0, unsigned long timeout = 0);
    bool readYesNo(const char *prompt, bool defaultValue = true, unsigned long timeout = 0);
    char readChar(const char *prompt, char defaultValue = '\0', unsigned long timeout = 0);
    String readString(const char *prompt, const char *defaultValue = "", unsigned long timeout = 0);

    bool isWaitingForInput() const;
    bool handleSerialInput();
    void clearSerialBuffer();
    void waitForSerialInput();

private:
    Stream *_serial;
    unsigned long _timeout;
    
    InputModeType _inputMode;
    String _inputBuffer;
    bool _inputComplete;
    
    float _inputDefaultFloat;
    int _inputDefaultInt;
    bool _inputDefaultBool;
    char _inputDefaultChar;
    String _inputDefaultString;
    
    void beginInputMode(InputModeType mode, const char *prompt);
    void processSerialInput();
    void completeInputMode();
};

#endif