void initWiFi() {
  if (WiFi.begin(credentials.ssid, credentials.password) == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
  } else {
    // Serial.println("Failed to connect to WiFi, switching to SoftAP mode");
    WiFi.mode(WIFI_OFF);
    delay(200);
    WiFi.softAP(credentials.ssid, credentials.password);
    // Serial.println("SoftAP activated, IP address: " + WiFi.softAPIP().toString());
  }
  if (!MDNS.begin("als")) {
    Serial.println("Error starting mDNS");
    return;
  }
  // Serial.println(WiFi.localIP());
}

void initSDCard() {
  Serial.println("| START DEBUG SD CARD");
  if (!SD.begin(5)) {
    Serial.println("| [ERROR] Card Mount Failed");
    return;
  }
  Serial.println("| [SUCCESS] Card Mount Success");
}