/*
 *  google-sheets.h
 *
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

    // Initialize with service account credentials
    bool begin(const char *clientEmail, const char *projectId, const char *privateKey);

    // For handling Wi-Fi connection on Pico W, that doesn't have a reconnected feature
    void addAP(const char *ssid, const char *password);
    void clearAP();

    // Set certificate (optional)
    void setCert(const char *cert);
    void setCertFile(const char *certFile, esp_google_sheet_file_storage_type storageType);

    // Set the token callback (for debug only)
    typedef void (*TokenStatusCallback)(TokenInfo info);
    void setTokenCallback(TokenStatusCallback callback);

    // Set the seconds to refresh auth token before expire
    void setPrerefreshSeconds(int seconds);

    // Call in loop for processing
    void loop();
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

    // Direct access to response
    FirebaseJson* getResponse();

private:
    // SSL client
    SSL_CLIENT _sslClient;

    // GSheet client instance
    bool _isInitialized;
    FirebaseJson _response;
    String _lastError;
    TokenStatusCallback _tokenCallback;

    // Service account credentials
    String _clientEmail;
    String _projectId;
    String _privateKey;

    // Method to process async results
    void processAsyncResult(FirebaseJson &response, bool success);

    // Helper methods for simplified operation
    bool makeRequest(const char *operation, const char *param1 = "", const char *param2 = "",
                     FirebaseJson *json = nullptr, FirebaseJsonArray *jsonArray = nullptr);

    // Task completion flag
    bool _taskComplete;
};

#endif // ESP32 || ESP8266
#endif // GOOGLE_SHEET_CLIENT_H