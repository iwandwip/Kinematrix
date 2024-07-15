#ifdef USE_ESP_NOW
#include "esp-now.h"

#if defined(ESP32) || defined(ESP8266)
//
//ESPNow::ESPNow() {
//    macAddressList = nullptr;
//    macAddressLen = 0;
//    macAddressThisDeviceIndex = 0;
//}
//
//ESPNow::~ESPNow() {
//    for (int i = 0; i < macAddressLen; i++) {
//        free(macAddressList[i]);
//    }
//    free(macAddressList);
//}
//
//bool ESPNow::begin(esp_now_send_cb_t sendCb, esp_now_recv_cb_t recvCb) {
//    WiFi.mode(WIFI_STA);
//
//    Serial.print("| [INFO]: ");
//    Serial.print("| This Device MacAddress: ");
//    Serial.print(WiFi.macAddress());
//    Serial.println();
//
//    if (esp_now_init() != ESP_OK) {
//        Serial.println("| [ERROR]: initializing ESP-NOW");
//        return false;
//    }
//
//    esp_now_register_send_cb(sendCb);
//    esp_now_register_recv_cb(recvCb);
//
//    setThisDeviceIndex(getThisDeviceIndexByMacAddress());
//    Serial.print("| [INFO]: MacAddressIndex: ");
//    Serial.print(macAddressThisDeviceIndex);
//    Serial.println();
//
//    if (macAddressLen <= 0) return false;
//
//    bool isInit = true;
//    for (int i = 0; i < macAddressLen; i++) {
//        if (notThisDevice(macAddressList[i], macAddressList[macAddressThisDeviceIndex])) {
//            esp_now_peer_info_t peerInfo = {};
//            memcpy(peerInfo.peer_addr, macAddressList[i], 6);
//            peerInfo.channel = 1;
//            peerInfo.encrypt = false;
//
//            if (esp_now_add_peer(&peerInfo) != ESP_OK) {
//                Serial.print("| [ERROR]: Failed add ");
//                Serial.print(macAddressToString(macAddressList[i]));
//                isInit = false;
//            } else {
//                Serial.print("| [SUCCESS]: Success add ");
//                Serial.print(macAddressToString(macAddressList[i]));
//            }
//            Serial.print(" to peer");
//            Serial.println();
//        }
//    }
//    return isInit;
//}
//
//void ESPNow::setThisDeviceIndex(uint8_t index) {
//    macAddressThisDeviceIndex = index;
//}
//
//int ESPNow::getThisDeviceIndex() {
//    return macAddressThisDeviceIndex;
//}
//
//int ESPNow::getThisDeviceIndexByMacAddress() {
//    for (int i = 0; i < macAddressLen; i++) {
//        if (macAddressToString(macAddressList[i]) == WiFi.macAddress()) {
//            return i;
//        }
//    }
//    return -1;
//}
//
//void ESPNow::addMacAddress(String macStr, int master) {
//    uint8_t *newMac = (uint8_t *) malloc(6 * sizeof(uint8_t));
//    if (newMac == nullptr) {
//        Serial.println("Memory Allocation Failed !");
//        return;
//    }
//    macStringToUint8(macStr.c_str(), newMac);
//    if (macAddressLen >= MAX_MAC_COUNT) {
//        Serial.println("Maximum MAC addresses reached!");
//        free(newMac);
//        return;
//    }
//    macAddressList = (uint8_t **) realloc(macAddressList, (macAddressLen + 1) * sizeof(uint8_t *));
//    if (macAddressList == nullptr) {
//        free(newMac);
//        Serial.println("Memory Reallocation Failed !");
//        return;
//    }
//    macAddressList[macAddressLen] = newMac;
//    if (master != -1) setThisDeviceIndex(macAddressLen);
//    macAddressLen++;
//}
//
//void ESPNow::debugMacAddressList() {
//    Serial.println("Debugging MAC Address List:");
//    for (int i = 0; i < macAddressLen; i++) {
//        Serial.print("MAC Address ");
//        Serial.print(i);
//        Serial.print(": ");
//        Serial.println(macAddressToString(macAddressList[i]));
//    }
//}
//
//esp_err_t ESPNow::sendData(const uint8_t *peer_addr, const uint8_t *data, size_t len) {
//    return esp_now_send(peer_addr, data, len);
//}
//
//bool ESPNow::broadcastData(const uint8_t *data, size_t len, void (*err_callback)()) {
//    for (int i = 0; i < macAddressLen; ++i) {
//        if (notThisDevice(macAddressList[i], macAddressList[macAddressThisDeviceIndex])) {
//            esp_err_t err = sendData(macAddressList[i], data, len);
//            if (err != ESP_OK && err_callback != nullptr) err_callback();
//        }
//    }
//    return true;
//}
//
//String ESPNow::macAddressToString(uint8_t *macArray) {
//    char macStr[18];
//    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
//             macArray[0], macArray[1], macArray[2], macArray[3], macArray[4], macArray[5]);
//    return String(macStr);
//}
//
//void ESPNow::macStringToUint8(const char *macStr, uint8_t *macArray) {
//    char *token = strtok((char *) macStr, ":");
//    int index = 0;
//    while (token != NULL) {
//        macArray[index++] = strtol(token, NULL, 16);
//        token = strtok(NULL, ":");
//    }
//}
//
//void ESPNow::getData(void *__restrict destination, const void *__restrict message, size_t len) {
//    memcpy(destination, message, len);
//}
//
//int ESPNow::getMacIndex(const uint8_t *left) {
//    for (int i = 0; i < macAddressLen; i++) {
//        if (isThisDevice(left, macAddressList[i])) return i;
//    }
//    return -1;
//}
//
//bool ESPNow::isThisDevice(uint8_t *left, uint8_t *right) {
//    for (int i = 0; i < 6; i++) if (left[i] != right[i]) return false;
//    return true;
//}
//
//bool ESPNow::isThisDevice(const uint8_t *left, uint8_t *right) {
//    for (int i = 0; i < 6; i++) if (left[i] != right[i]) return false;
//    return true;
//}
//
//bool ESPNow::notThisDevice(uint8_t *left, uint8_t *right) {
//    for (int i = 0; i < 6; i++) if (left[i] != right[i]) return true;
//    return false;
//}
//
//String ESPNow::parseStr(String data, char separator[], int index) {
//    int found = 0;
//    int strIndex[] = {0, -1};
//    int maxIndex = data.length() - 1;
//    for (int i = 0; i <= maxIndex && found <= index; i++) {
//        if (data.charAt(i) == separator[0] || i == maxIndex) {
//            found++;
//            strIndex[0] = strIndex[1] + 1;
//            strIndex[1] = (i == maxIndex) ? i + 1 : i;
//        }
//    }
//    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
//}
//
//void ESPNow::defaultSendCallback(const uint8_t *mac_addr, esp_now_send_status_t status) {
//
//}
//
//void ESPNow::defaultReceiveCallback(const uint8_t *mac, const uint8_t *incomingData, int len) {
//
//}

#endif
#endif