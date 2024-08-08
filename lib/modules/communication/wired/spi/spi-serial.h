//#pragma once
//
//#ifndef KINEMATRIX_SPI_MASTER_H
//#define KINEMATRIX_SPI_MASTER_H
//
//#include "Arduino.h"
//#include "ESP32DMASPIMaster.h"
//
//class SPIMaster : public ESP32DMASPI::Master {
//private:
//    String dataSend;
//    uint8_t *dma_tx_buf;
//    uint8_t *dma_rx_buf;
//
//public:
//    SPIMaster();
//    void initDMABuffer();
//
//    template<typename T>
//    void addData(T newData, const char *separator = ";") {
//        dataSend += String(newData);
//        dataSend += separator;
//    }
//
//    void clearData();
//    void sendAndReceive(void (*onReceive)(const String &));
//    void dumpBuffers(const char *title, const uint8_t *buf, size_t start, size_t len);
//    bool verifyAndDumpDifference(const char *a_title, const uint8_t *a_buf, size_t a_size, const char *b_title,
//                                 const uint8_t *b_buf, size_t b_size);
//    void initializeBuffers(uint8_t *tx, uint8_t *rx, size_t size, size_t offset = 0);
//    int stringToBytes(String str, uint8_t *byteArray);
//    String bytesToString(uint8_t *byteArray, int length);
//    String parseStr(String data, char separator[], int index);
//
//    static constexpr size_t BUFFER_SIZE = 256;  // should be multiple of 4
//    static constexpr size_t QUEUE_SIZE = 1;
//};
//
//#endif  // KINEMATRIX_SPI_MASTER_H