/*
 *  google-sheets.cpp
 *
 *  Simplified Google Sheets handler implementation
 */

#include "google-sheets.h"

#if defined(ESP32) || defined(ESP8266)

GoogleSheetClient::GoogleSheetClient() : _isInitialized(false), _taskComplete(false) {
    set_ssl_client_insecure_and_buffer(_sslClient);
}

GoogleSheetClient::~GoogleSheetClient() {
    // Clean up resources if necessary
}

bool GoogleSheetClient::begin(const char *clientEmail, const char *projectId, const char *privateKey) {
    _clientEmail = clientEmail;
    _projectId = projectId;
    _privateKey = privateKey;

    // Initialize the Google Sheet Client
    GSheet.begin(_clientEmail.c_str(), _projectId.c_str(), _privateKey.c_str());

    _isInitialized = true;
    return _isInitialized;
}

void GoogleSheetClient::addAP(const char *ssid, const char *password) {
    if (_isInitialized) {
        GSheet.addAP(ssid, password);
    }
}

void GoogleSheetClient::clearAP() {
    if (_isInitialized) {
        GSheet.clearAP();
    }
}

void GoogleSheetClient::setCert(const char *cert) {
    if (_isInitialized) {
        GSheet.setCert(cert);
    }
}

void GoogleSheetClient::setCertFile(const char *certFile, esp_google_sheet_file_storage_type storageType) {
    if (_isInitialized) {
        GSheet.setCertFile(certFile, storageType);
    }
}

void GoogleSheetClient::setTokenCallback(TokenStatusCallback callback) {
    _tokenCallback = callback;
    if (_isInitialized) {
        GSheet.setTokenCallback(_tokenCallback);
    }
}

void GoogleSheetClient::setPrerefreshSeconds(int seconds) {
    if (_isInitialized) {
        GSheet.setPrerefreshSeconds(seconds);
    }
}

void GoogleSheetClient::loop() {
    // Call ready() repeatedly in loop for authentication checking and processing
    if (_isInitialized) {
        GSheet.ready();
    }
}

bool GoogleSheetClient::ready() {
    if (_isInitialized) {
        return GSheet.ready();
    }
    return false;
}

String GoogleSheetClient::errorReason() {
    if (_isInitialized) {
        return GSheet.errorReason();
    }
    return "Not initialized";
}

String GoogleSheetClient::createSpreadsheet(const char *title, const char *sheetName, int rowCount, int columnCount, const char *userEmail) {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    FirebaseJson spreadsheet;
    spreadsheet.set("properties/title", title);
    spreadsheet.set("sheets/[0]/properties/title", sheetName);
    spreadsheet.set("sheets/[0]/properties/sheetId", 1);
    spreadsheet.set("sheets/[0]/properties/sheetType", "GRID");
    spreadsheet.set("sheets/[0]/properties/gridProperties/rowCount", rowCount);
    spreadsheet.set("sheets/[0]/properties/gridProperties/columnCount", columnCount);

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets/create
    bool success = GSheet.create(&_response, &spreadsheet, userEmail);

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

String GoogleSheetClient::getSpreadsheet(const char *spreadsheetId) {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets/get
    bool success = GSheet.get(&_response, spreadsheetId);

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

String GoogleSheetClient::getSpreadsheetByDataFilter(const char *spreadsheetId, const char *dataFiltersJson) {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    FirebaseJsonArray dataFiltersArr;
    FirebaseJson dataFilters;
    dataFilters.setJsonData(dataFiltersJson);
    dataFiltersArr.add(dataFilters);

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets/getByDataFilter
    bool success = GSheet.getByDataFilter(&_response, spreadsheetId, &dataFiltersArr, "true");

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

String GoogleSheetClient::deleteSpreadsheet(const char *spreadsheetId) {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    bool success = GSheet.deleteFile(&_response, spreadsheetId);

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

String GoogleSheetClient::listSpreadsheets() {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    bool success = GSheet.listFiles(&_response);

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

String GoogleSheetClient::copySheetTo(const char *sourceSpreadsheetId, int sheetId, const char *destinationSpreadsheetId) {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.sheets/copyTo
    bool success = GSheet.sheets.copyTo(&_response, sourceSpreadsheetId, sheetId, destinationSpreadsheetId);

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

bool GoogleSheetClient::updateValues(const char *spreadsheetId, const char *range, FirebaseJson *valueRange) {
    if (!_isInitialized || valueRange == nullptr) {
        return false;
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/update
    bool success = GSheet.values.update(&_response, spreadsheetId, range, valueRange);

    return success;
}

bool GoogleSheetClient::batchUpdateValues(const char *spreadsheetId, FirebaseJsonArray *valueRangeArr) {
    if (!_isInitialized || valueRangeArr == nullptr) {
        return false;
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/batchUpdate
    bool success = GSheet.values.batchUpdate(&_response, spreadsheetId, valueRangeArr);

    return success;
}

bool GoogleSheetClient::batchUpdateValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFilterValueRangeArr) {
    if (!_isInitialized || dataFilterValueRangeArr == nullptr) {
        return false;
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/batchUpdateByDataFilter
    bool success = GSheet.values.batchUpdateByDataFilter(&_response, spreadsheetId, dataFilterValueRangeArr);

    return success;
}

bool GoogleSheetClient::appendValues(const char *spreadsheetId, const char *range, FirebaseJson *valueRange) {
    if (!_isInitialized || valueRange == nullptr) {
        return false;
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/append
    bool success = GSheet.values.append(&_response, spreadsheetId, range, valueRange);

    return success;
}

String GoogleSheetClient::getValues(const char *spreadsheetId, const char *range) {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/get
    bool success = GSheet.values.get(&_response, spreadsheetId, range);

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

String GoogleSheetClient::batchGetValues(const char *spreadsheetId, const char *ranges) {
    if (!_isInitialized) {
        return "Error: Not initialized";
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/batchGet
    bool success = GSheet.values.batchGet(&_response, spreadsheetId, ranges);

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

String GoogleSheetClient::batchGetValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFiltersArr) {
    if (!_isInitialized || dataFiltersArr == nullptr) {
        return "Error: Not initialized or invalid dataFiltersArr";
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/batchGetByDataFilter
    bool success = GSheet.values.batchGetByDataFilter(&_response, spreadsheetId, dataFiltersArr, "ROWS");

    if (success) {
        return _response.raw();
    } else {
        return "Error: " + GSheet.errorReason();
    }
}

bool GoogleSheetClient::clearValues(const char *spreadsheetId, const char *range) {
    if (!_isInitialized) {
        return false;
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/clear
    bool success = GSheet.values.clear(&_response, spreadsheetId, range);

    return success;
}

bool GoogleSheetClient::batchClearValues(const char *spreadsheetId, const char *ranges) {
    if (!_isInitialized) {
        return false;
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/batchClear
    bool success = GSheet.values.batchClear(&_response, spreadsheetId, ranges);

    return success;
}

bool GoogleSheetClient::batchClearValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFiltersArr) {
    if (!_isInitialized || dataFiltersArr == nullptr) {
        return false;
    }

    _taskComplete = false;
    _response.clear();

    // For Google Sheet API ref doc, go to https://developers.google.com/sheets/api/reference/rest/v4/spreadsheets.values/batchClearByDataFilter
    bool success = GSheet.values.batchClearByDataFilter(&_response, spreadsheetId, dataFiltersArr);

    return success;
}

FirebaseJson *GoogleSheetClient::getResponse() {
    return &_response;
}

void GoogleSheetClient::processAsyncResult(FirebaseJson &response, bool success) {
    _taskComplete = true;
    if (!success) {
        _lastError = GSheet.errorReason();
    }
}

bool GoogleSheetClient::makeRequest(const char *operation, const char *param1, const char *param2,
                                    FirebaseJson *json, FirebaseJsonArray *jsonArray) {
    // Helper method for common operation patterns
    // Implementation would depend on the specific operation
    return false; // Placeholder
}

#endif // ESP32 || ESP8266