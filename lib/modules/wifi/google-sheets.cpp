/*
 *  google-sheets.cpp
 *  Simplified Google Sheets handler implementation
 */

#include "google-sheets.h"

#if defined(ESP32) || defined(ESP8266)

GoogleSheetClient::GoogleSheetClient() : _isInitialized(false) {
    set_ssl_client_insecure_and_buffer(_sslClient);
}

GoogleSheetClient::~GoogleSheetClient() {
    // Clean up resources if necessary
}

bool GoogleSheetClient::begin(const char *clientEmail, const char *projectId, const char *privateKey) {
    _clientEmail = clientEmail;
    _projectId = projectId;
    _privateKey = privateKey;

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

void GoogleSheetClient::process() {
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
    return _lastError.length() > 0 ? _lastError : (_isInitialized ? GSheet.errorReason() : "Not initialized");
}

String GoogleSheetClient::createSpreadsheet(const char *title, const char *sheetName, int rowCount, int columnCount, const char *userEmail) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    FirebaseJson spreadsheet;
    spreadsheet.set("properties/title", title);
    spreadsheet.set("sheets/[0]/properties/title", sheetName);
    spreadsheet.set("sheets/[0]/properties/sheetId", 1);
    spreadsheet.set("sheets/[0]/properties/sheetType", "GRID");
    spreadsheet.set("sheets/[0]/properties/gridProperties/rowCount", rowCount);
    spreadsheet.set("sheets/[0]/properties/gridProperties/columnCount", columnCount);

    bool success = GSheet.create(&_response, &spreadsheet, userEmail);

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

String GoogleSheetClient::getSpreadsheet(const char *spreadsheetId) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.get(&_response, spreadsheetId);

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

String GoogleSheetClient::getSpreadsheetByDataFilter(const char *spreadsheetId, const char *dataFiltersJson) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    FirebaseJsonArray dataFiltersArr;
    FirebaseJson dataFilters;
    dataFilters.setJsonData(dataFiltersJson);
    dataFiltersArr.add(dataFilters);

    bool success = GSheet.getByDataFilter(&_response, spreadsheetId, &dataFiltersArr, "true");

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

String GoogleSheetClient::deleteSpreadsheet(const char *spreadsheetId) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.deleteFile(&_response, spreadsheetId);

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

String GoogleSheetClient::listSpreadsheets() {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.listFiles(&_response);

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

String GoogleSheetClient::copySheetTo(const char *sourceSpreadsheetId, int sheetId, const char *destinationSpreadsheetId) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.sheets.copyTo(&_response, sourceSpreadsheetId, sheetId, destinationSpreadsheetId);

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

bool GoogleSheetClient::updateValues(const char *spreadsheetId, const char *range, FirebaseJson *valueRange) {
    if (!_isInitialized || valueRange == nullptr) {
        _lastError = !_isInitialized ? "Not initialized" : "Invalid valueRange (null)";
        return false;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.update(&_response, spreadsheetId, range, valueRange);

    if (!success) {
        _lastError = GSheet.errorReason();
    }

    return success;
}

bool GoogleSheetClient::batchUpdateValues(const char *spreadsheetId, FirebaseJsonArray *valueRangeArr) {
    if (!_isInitialized || valueRangeArr == nullptr) {
        _lastError = !_isInitialized ? "Not initialized" : "Invalid valueRangeArr (null)";
        return false;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.batchUpdate(&_response, spreadsheetId, valueRangeArr);

    if (!success) {
        _lastError = GSheet.errorReason();
    }

    return success;
}

bool GoogleSheetClient::batchUpdateValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFilterValueRangeArr) {
    if (!_isInitialized || dataFilterValueRangeArr == nullptr) {
        _lastError = !_isInitialized ? "Not initialized" : "Invalid dataFilterValueRangeArr (null)";
        return false;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.batchUpdateByDataFilter(&_response, spreadsheetId, dataFilterValueRangeArr);

    if (!success) {
        _lastError = GSheet.errorReason();
    }

    return success;
}

bool GoogleSheetClient::appendValues(const char *spreadsheetId, const char *range, FirebaseJson *valueRange) {
    if (!_isInitialized || valueRange == nullptr) {
        _lastError = !_isInitialized ? "Not initialized" : "Invalid valueRange (null)";
        return false;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.append(&_response, spreadsheetId, range, valueRange);

    if (!success) {
        _lastError = GSheet.errorReason();
    }

    return success;
}

String GoogleSheetClient::getValues(const char *spreadsheetId, const char *range) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.get(&_response, spreadsheetId, range);

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

String GoogleSheetClient::batchGetValues(const char *spreadsheetId, const char *ranges) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.batchGet(&_response, spreadsheetId, ranges);

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

String GoogleSheetClient::batchGetValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFiltersArr) {
    if (!_isInitialized || dataFiltersArr == nullptr) {
        _lastError = !_isInitialized ? "Not initialized" : "Invalid dataFiltersArr (null)";
        return "Error: " + _lastError;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.batchGetByDataFilter(&_response, spreadsheetId, dataFiltersArr, "ROWS");

    if (success) {
        return _response.raw();
    } else {
        _lastError = GSheet.errorReason();
        return "Error: " + _lastError;
    }
}

bool GoogleSheetClient::clearValues(const char *spreadsheetId, const char *range) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return false;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.clear(&_response, spreadsheetId, range);

    if (!success) {
        _lastError = GSheet.errorReason();
    }

    return success;
}

bool GoogleSheetClient::batchClearValues(const char *spreadsheetId, const char *ranges) {
    if (!_isInitialized) {
        _lastError = "Not initialized";
        return false;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.batchClear(&_response, spreadsheetId, ranges);

    if (!success) {
        _lastError = GSheet.errorReason();
    }
    GSheet.getTokenType();
    return success;
}

bool GoogleSheetClient::batchClearValuesByDataFilter(const char *spreadsheetId, FirebaseJsonArray *dataFiltersArr) {
    if (!_isInitialized || dataFiltersArr == nullptr) {
        _lastError = !_isInitialized ? "Not initialized" : "Invalid dataFiltersArr (null)";
        return false;
    }

    _response.clear();
    _lastError = "";

    bool success = GSheet.values.batchClearByDataFilter(&_response, spreadsheetId, dataFiltersArr);

    if (!success) {
        _lastError = GSheet.errorReason();
    }

    return success;
}

FirebaseJson *GoogleSheetClient::getResponse() {
    return &_response;
}

String GoogleSheetClient::getTokenType() {
    return GSheet.getTokenType();
}

String GoogleSheetClient::getTokenType(TokenInfo info) {
    return GSheet.getTokenType(info);
}

String GoogleSheetClient::getTokenStatus() {
    return GSheet.getTokenStatus();
}

String GoogleSheetClient::getTokenStatus(TokenInfo info) {
    return GSheet.getTokenStatus(info);
}

String GoogleSheetClient::getTokenError() {
    return GSheet.getTokenError();
}

String GoogleSheetClient::getTokenError(TokenInfo info) {
    return GSheet.getTokenError(info);
}

#endif // ESP32 || ESP8266