/*
 *  google-sheets.h
 *  Simplified Google Sheets handler
 */

#ifndef GOOGLE_SHEET_CLIENT_H
#define GOOGLE_SHEET_CLIENT_H

#if defined(ESP32) || defined(ESP8266)

#include <Arduino.h>
#include <ESP_Google_Sheet_Client.h>
#include <time.h>

#if defined(ESP32)
#include <WiFiClientSecure.h>
typedef WiFiClientSecure SSL_CLIENT;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
typedef BearSSL::WiFiClientSecure SSL_CLIENT;
#endif

#ifndef SSL_CLIENT_BUFFER_SET
#define SSL_CLIENT_BUFFER_SET

inline void set_ssl_client_insecure_and_buffer(SSL_CLIENT &client) {
#if defined(ESP32)
    client.setInsecure();
#elif defined(ESP8266)
    client.setInsecure();
    client.setBufferSizes(4096, 1024);
#endif
}

#endif // SSL_CLIENT_BUFFER_SET

class GoogleSheetClient {
public:
    GoogleSheetClient();
    ~GoogleSheetClient();

    bool begin(const char *clientEmail, const char *projectId, const char *privateKey);

    void addAP(const char *ssid, const char *password);
    void clearAP();

    void setCert(const char *cert);
    void setCertFile(const char *certFile, esp_google_sheet_file_storage_type storageType);

    typedef void (*TokenStatusCallback)(TokenInfo info);
    void setTokenCallback(TokenStatusCallback callback);

    void setPrerefreshSeconds(int seconds);

    void process();
    bool ready();
    String errorReason();

    // Spreadsheet operations
    String createSpreadsheet(const char *title, const char *sheetName, int rowCount, int columnCount, const char *userEmail);
    String getSpreadsheet(const char *spreadsheetId);
    String getSpreadsheetByDataFilter(const char *spreadsheetId, const char *dataFiltersJson);
    String deleteSpreadsheet(const char *spreadsheetId);
    String listSpreadsheets();

    // Sheet operations
    String copySheetTo(const char *sourceSpreadsheetId, int sheetId, const char *destinationSpreadsheetId);

    // Values operations
    bool updateValues(const char *spreadsheetId, const char *range, FirebaseJson *valueRange);
    bool batchUpdateValues(const char *spreadsheetId, FirebaseJsonArray *valueRangeArr);
    bool batchUpdateValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFilterValueRangeArr);
    bool appendValues(const char *spreadsheetId, const char *range, FirebaseJson *valueRange);
    String getValues(const char *spreadsheetId, const char *range);
    String batchGetValues(const char *spreadsheetId, const char *ranges);
    String batchGetValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFiltersArr);
    bool clearValues(const char *spreadsheetId, const char *range);
    bool batchClearValues(const char *spreadsheetId, const char *ranges);
    bool batchClearValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFiltersArr);

    // Access to response
    FirebaseJson *getResponse();

    static String getTokenType();
    static String getTokenType(TokenInfo info);
    static String getTokenStatus();
    static String getTokenStatus(TokenInfo info);
    static String getTokenError();
    static String getTokenError(TokenInfo info);

private:
    SSL_CLIENT _sslClient;
    bool _isInitialized;
    FirebaseJson _response;
    String _lastError;
    TokenStatusCallback _tokenCallback;

    String _clientEmail;
    String _projectId;
    String _privateKey;
};

#endif // ESP32 || ESP8266
#endif // GOOGLE_SHEET_CLIENT_H