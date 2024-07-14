#ifndef RESPONSE_H
#define RESPONSE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

class Response {
public:
    Response(int statusCode, String body, String headers = "");
    int getStatusCode() const;
    String getBody() const;
    String getHeader(const String& headerName) const;
    bool parseJson(JsonDocument& doc) const;

private:
    int statusCode;
    String body;
    String headers;

    struct Header {
        String key;
        String value;
    };
    std::vector<Header> parseHeaders() const;
};

#endif // RESPONSE_H
