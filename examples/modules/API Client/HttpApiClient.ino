
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* get_url = "https://dummyjson.com/products/1";
const char* post_url = "https://dummyjson.com/auth/login";

HttpClient httpClient;

void setup() {
    Serial.begin(115200);
    if (httpClient.begin(ssid, password)) {
        Serial.println("WiFi connected");
    } else {
        Serial.println("WiFi connection failed");
    }
}

void getResponseCallback(Response response) {
    Serial.println("GET Response:");
    Serial.printf("Status Code: %d\n", response.getStatusCode());
    Serial.println("Body:");
    Serial.println(response.getBody());
}

void postResponseCallback(Response response) {
    Serial.println("POST Response:");
    Serial.printf("Status Code: %d\n", response.getStatusCode());
    Serial.println("Body:");
    Serial.println(response.getBody());

    StaticJsonDocument<200> doc;
    if (response.parseJson(doc)) {
        serializeJsonPretty(doc, Serial);
    } else {
        Serial.println("Failed to parse JSON response");
    }
}


void loop() {
    if (httpClient.isReady()) {
        httpClient.get(get_url, getResponseCallback);
        delay(5000);
        httpClient.clearFormData();

        httpClient.addFormData("username", "sophiab");
        httpClient.addFormData("password", "sophiabpass");
        httpClient.post(post_url, postResponseCallback);
        delay(5000);

        httpClient.clearFormData();

        httpClient.addFormData("username", "jamesd");
        httpClient.addFormData("password", "jamesdpass");
        httpClient.post(post_url, postResponseCallback);
        delay(5000);

        httpClient.clearFormData();

        httpClient.addFormData("username", "emmaj");
        httpClient.addFormData("password", "emmajpass");
        httpClient.post(post_url, postResponseCallback);
        delay(10000);
    }
}