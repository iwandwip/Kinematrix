#ifdef USE_HTTP_API_CLIENT

#include "response.h"
//
//Response::Response(int statusCode, String body, String headers)
//        : statusCode(statusCode), body(body), headers(headers) {}
//
//int Response::getStatusCode() const {
//    return statusCode;
//}
//
//String Response::getBody() const {
//    return body;
//}
//
//String Response::getHeader(const String& headerName) const {
//    std::vector<Header> headerVector = parseHeaders();
//    for (const auto& header : headerVector) {
//        if (header.key.equalsIgnoreCase(headerName)) {
//            return header.value;
//        }
//    }
//    return "";
//}
//
//bool Response::parseJson(JsonDocument& doc) const {
//    DeserializationError error = deserializeJson(doc, body);
//    return !error;
//}
//
//std::vector<Response::Header> Response::parseHeaders() const {
//    std::vector<Header> headerVector;
//    int start = 0;
//    while (start < headers.length()) {
//        int end = headers.indexOf("\r\n", start);
//        if (end == -1) break;
//        String line = headers.substring(start, end);
//        int separator = line.indexOf(": ");
//        if (separator != -1) {
//            String key = line.substring(0, separator);
//            String value = line.substring(separator + 2);
//            headerVector.push_back({key, value});
//        }
//        start = end + 2;
//    }
//    return headerVector;
//}

#endif