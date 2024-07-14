#ifdef USE_HTTP_API_CLIENT

#include "http-api-client.h"
#include "response.h"

struct HttpRequest {
    HttpApiClient* client;
    const char* url;
    const char* payload;
    void (*callback)(Response);
    String authToken;
    String params;
    String body;
};

HttpApiClient::HttpApiClient() {
    xSemaphore = xSemaphoreCreateMutex();
    requestReady = false;
}

bool HttpApiClient::begin(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    requestReady = true;
    return true;
}

void HttpApiClient::setAuthorization(const char* authToken) {
    authorizationToken = authToken;
}

void HttpApiClient::addParam(const char* key, const char* value) {
    if (urlParams.length() > 0) {
        urlParams += "&";
    }
    urlParams += String(key) + "=" + String(value);
}

void HttpApiClient::clearParams() {
    urlParams = "";
}

void HttpApiClient::addFormData(const char* key, const char* value) {
    if (formData.length() > 0) {
        formData += ",";
    }
    formData += "\"" + String(key) + "\":\"" + String(value) + "\"";
}

void HttpApiClient::clearFormData() {
    formData = "";
}

bool HttpApiClient::isReady() {
    return requestReady;
}

void HttpApiClient::get(const char* url, void (*responseCallback)(Response)) {
    HttpRequest* request = new HttpRequest{this, url, nullptr, responseCallback, authorizationToken, urlParams, ""};
    xTaskCreate(httpGetTask, "httpGetTask", 8192, request, 1, NULL);
}

void HttpApiClient::post(const char* url, void (*responseCallback)(Response)) {
    String payload = "{" + formData + "}";
    HttpRequest* request = new HttpRequest{this, url, payload.c_str(), responseCallback, authorizationToken, "", payload};
    xTaskCreate(httpPostTask, "httpPostTask", 8192, request, 1, NULL);
}

void HttpApiClient::put(const char* url, void (*responseCallback)(Response)) {
    String payload = "{" + formData + "}";
    HttpRequest* request = new HttpRequest{this, url, payload.c_str(), responseCallback, authorizationToken, "", payload};
    xTaskCreate(httpPutTask, "httpPutTask", 8192, request, 1, NULL);
}

void HttpApiClient::deleteReq(const char* url, void (*responseCallback)(Response)) {
    HttpRequest* request = new HttpRequest{this, url, nullptr, responseCallback, authorizationToken, urlParams, ""};
    xTaskCreate(httpDeleteTask, "httpDeleteTask", 8192, request, 1, NULL);
}

String HttpApiClient::collectHeaders() {
    String headers = "";
    for (int i = 0; i < httpClient.headers(); i++) {
        headers += httpClient.headerName(i) + ": " + httpClient.header(i) + "\r\n";
    }
    return headers;
}

void HttpApiClient::httpGetTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpApiClient* client = request->client;
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

void HttpApiClient::httpPostTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpApiClient* client = request->client;
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

void HttpApiClient::httpPutTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpApiClient* client = request->client;
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

void HttpApiClient::httpDeleteTask(void* pvParameters) {
    HttpRequest* request = (HttpRequest*) pvParameters;
    HttpApiClient* client = request->client;
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

#endif