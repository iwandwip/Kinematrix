#include "http-api-client.h"
#include "response.h"

struct HttpRequest {
    HttpClient* client;
    const char* url;
    const char* payload;
    void (*callback)(Response);
    String authToken;
    String params;
    String body;
};

HttpClient::HttpClient() {
    xSemaphore = xSemaphoreCreateMutex();
    requestReady = false;
}

bool HttpClient::begin(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    requestReady = true;
    return true;
}

void HttpClient::setAuthorization(const char* authToken) {
    authorizationToken = authToken;
}

void HttpClient::addParam(const char* key, const char* value) {
    if (urlParams.length() > 0) {
        urlParams += "&";
    }
    urlParams += String(key) + "=" + String(value);
}

void HttpClient::clearParams() {
    urlParams = "";
}

void HttpClient::addFormData(const char* key, const char* value) {
    if (formData.length() > 0) {
        formData += ",";
    }
    formData += "\"" + String(key) + "\":\"" + String(value) + "\"";
}

void HttpClient::clearFormData() {
    formData = "";
}

bool HttpClient::isReady() {
    return requestReady;
}

void HttpClient::get(const char* url, void (*responseCallback)(Response)) {
    HttpRequest* request = new HttpRequest{this, url, nullptr, responseCallback, authorizationToken, urlParams, ""};
    xTaskCreate(httpGetTask, "httpGetTask", 8192, request, 1, NULL);
}

void HttpClient::post(const char* url, void (*responseCallback)(Response)) {
    String payload = "{" + formData + "}";
    HttpRequest* request = new HttpRequest{this, url, payload.c_str(), responseCallback, authorizationToken, "", payload};
    xTaskCreate(httpPostTask, "httpPostTask", 8192, request, 1, NULL);
}

void HttpClient::put(const char* url, void (*responseCallback)(Response)) {
    String payload = "{" + formData + "}";
    HttpRequest* request = new HttpRequest{this, url, payload.c_str(), responseCallback, authorizationToken, "", payload};
    xTaskCreate(httpPutTask, "httpPutTask", 8192, request, 1, NULL);
}

void HttpClient::deleteReq(const char* url, void (*responseCallback)(Response)) {
    HttpRequest* request = new HttpRequest{this, url, nullptr, responseCallback, authorizationToken, urlParams, ""};
    xTaskCreate(httpDeleteTask, "httpDeleteTask", 8192, request, 1, NULL);
}

String HttpClient::collectHeaders() {
    String headers = "";
    for (int i = 0; i < httpClient.headers(); i++) {
        headers += httpClient.headerName(i) + ": " + httpClient.header(i) + "\r\n";
    }
    return headers;
}

void HttpClient::httpGetTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpClient* client = request->client;
    if (xSemaphoreTake(client->xSemaphore, portMAX_DELAY) == pdTRUE) {
        HTTPClient httpClient;
        String fullUrl = String(request->url) + "?" + request->params;
        httpClient.begin(fullUrl);
        if (!request->authToken.isEmpty()) {
            httpClient.addHeader("Authorization", request->authToken);
        }
        int httpCode = httpClient.GET();
        String payload = "";
        if (httpCode > 0) {
            payload = httpClient.getString();
        }
        String headers = client->collectHeaders();
        httpClient.end();
        if (request->callback) {
            request->callback(Response(httpCode, payload, headers));
        }
        delete request;
        xSemaphoreGive(client->xSemaphore);
        vTaskDelete(NULL);
    }
}

void HttpClient::httpPostTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpClient* client = request->client;
    if (xSemaphoreTake(client->xSemaphore, portMAX_DELAY) == pdTRUE) {
        HTTPClient httpClient;
        httpClient.begin(request->url);
        httpClient.addHeader("Content-Type", "application/json");
        if (!request->authToken.isEmpty()) {
            httpClient.addHeader("Authorization", request->authToken);
        }
        int httpCode = httpClient.POST(request->body);
        String response = "";
        if (httpCode > 0) {
            response = httpClient.getString();
        }
        String headers = client->collectHeaders();
        httpClient.end();
        if (request->callback) {
            request->callback(Response(httpCode, response, headers));
        }
        delete request;
        xSemaphoreGive(client->xSemaphore);
        vTaskDelete(NULL);
    }
}

void HttpClient::httpPutTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpClient* client = request->client;
    if (xSemaphoreTake(client->xSemaphore, portMAX_DELAY) == pdTRUE) {
        HTTPClient httpClient;
        httpClient.begin(request->url);
        httpClient.addHeader("Content-Type", "application/json");
        if (!request->authToken.isEmpty()) {
            httpClient.addHeader("Authorization", request->authToken);
        }
        int httpCode = httpClient.PUT(request->body);
        String response = "";
        if (httpCode > 0) {
            response = httpClient.getString();
        }
        String headers = client->collectHeaders();
        httpClient.end();
        if (request->callback) {
            request->callback(Response(httpCode, response, headers));
        }
        delete request;
        xSemaphoreGive(client->xSemaphore);
        vTaskDelete(NULL);
    }
}

void HttpClient::httpDeleteTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpClient* client = request->client;
    if (xSemaphoreTake(client->xSemaphore, portMAX_DELAY) == pdTRUE) {
        HTTPClient httpClient;
        String fullUrl = String(request->url) + "?" + request->params;
        httpClient.begin(fullUrl);
        if (!request->authToken.isEmpty()) {
            httpClient.addHeader("Authorization", request->authToken);
        }
        int httpCode = httpClient.sendRequest("DELETE");
        String payload = "";
        if (httpCode > 0) {
            payload = httpClient.getString();
        }
        String headers = client->collectHeaders();
        httpClient.end();
        if (request->callback) {
            request->callback(Response(httpCode, payload, headers));
        }
        delete request;
        xSemaphoreGive(client->xSemaphore);
        vTaskDelete(NULL);
    }
}