//#include "spi-serial.h"
//
//SPIMaster::SPIMaster()
//        : ESP32DMASPI::Master() {
//}
//
//void SPIMaster::initDMABuffer() {
//    dma_tx_buf = ESP32DMASPI::Master::allocDMABuffer(
//            SPIMaster::BUFFER_SIZE);  // to use DMA buffer, use these methods to allocate buffer
//    dma_rx_buf = ESP32DMASPI::Master::allocDMABuffer(SPIMaster::BUFFER_SIZE);
//}
//
//void SPIMaster::clearData() {
//    dataSend = "";
//}
//
//void SPIMaster::sendAndReceive(void (*onReceive)(const String &)) {
//    initializeBuffers(dma_tx_buf, dma_rx_buf, SPIMaster::BUFFER_SIZE);
//    int len = stringToBytes(dataSend, dma_tx_buf);
//    const size_t received_bytes = ESP32DMASPI::Master::transfer(dma_tx_buf, dma_rx_buf, SPIMaster::BUFFER_SIZE);
//    String msg = bytesToString(dma_rx_buf, SPIMaster::BUFFER_SIZE);
//    onReceive(msg);
//}
//
//void SPIMaster::dumpBuffers(const char *title, const uint8_t *buf, size_t start, size_t len) {
//    if (len == 1) Serial.printf("%s [%d]: ", title, start);
//    else Serial.printf("%s [%d-%d]: ", title, start, start + len - 1);
//    for (size_t i = 0; i < len; i++) {
//        Serial.printf("%02X ", buf[start + i]);
//    }
//    Serial.printf("\n");
//}
//
//bool SPIMaster::verifyAndDumpDifference(const char *a_title, const uint8_t *a_buf, size_t a_size, const char *b_title,
//                                        const uint8_t *b_buf, size_t b_size) {
//    bool verified = true;
//    if (a_size != b_size) {
//        Serial.printf("received data size does not match: expected = %d / actual = %d\n", a_size, b_size);
//        return false;
//    }
//    for (size_t i = 0; i < a_size; i++) {
//        // if a_buf and b_buf is same, continue
//        if (a_buf[i] == b_buf[i]) {
//            continue;
//        }
//        verified = false;
//        // if a_buf[i] and b_buf[i] is not same, check the range that has difference
//        size_t j = 1;
//        while (a_buf[i + j] != b_buf[i + j]) {
//            j++;
//        }
//        // dump different data range
//        dumpBuffers(a_title, a_buf, i, j);
//        dumpBuffers(b_title, b_buf, i, j);
//        // restart from next same index (-1 considers i++ in for())
//        i += j - 1;
//    }
//    return verified;
//}
//
//void SPIMaster::initializeBuffers(uint8_t *tx, uint8_t *rx, size_t size, size_t offset) {
//    if (tx) {
//        for (size_t i = 0; i < size; i++) {
//            tx[i] = (i + offset) & 0xFF;
//        }
//    }
//    if (rx) {
//        memset(rx, 0, size);
//    }
//}
//
//int SPIMaster::stringToBytes(String str, uint8_t *byteArray) {
//    int length = str.length();
//    if (length > BUFFER_SIZE) {
//        length = BUFFER_SIZE;
//    }
//    for (int i = 0; i < length; i++) {
//        byteArray[i] = str.charAt(i);
//    }
//    return length;
//}
//
//String SPIMaster::bytesToString(uint8_t *byteArray, int length) {
//    String str = "";
//    for (int i = 0; i < length; i++) {
//        if (byteArray[i] != 0) {
//            str += (char) byteArray[i];
//        }
//    }
//    return str;
//}
//
//String SPIMaster::parseStr(String data, char separator[], int index) {
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