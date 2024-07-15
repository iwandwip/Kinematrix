/*
 *  i2c-scanner.cpp
 *
 *  i2c scanner c
 *  Created on: 2023. 4. 3
 */


#ifdef USE_I2C_SCANNER
#include "i2c-scanner.h"
//
//I2CScanner::I2CScanner()
//        : n_devices_(0) {
//}
//
//I2CScanner::~I2CScanner() {
//    delete this;
//}
//
//void I2CScanner::beginTransmission() {
//    Serial.println("==============================");
//    Serial.println("| START SCANNING I2C");
//    Serial.println("==============================");
//
//    Wire.begin();
//    Serial.println("| WIRE BEGIN");
//}
//
//void I2CScanner::scanI2CAddress() {
//    byte error;
//    int address;
//    Serial.println("| Scanning...");
//    n_devices_ = 0;
//    for (address = 1; address < 127; address++) {
//        Wire.beginTransmission(address);
//        error = Wire.endTransmission();
//        if (error == 0) {
//            n_address_[n_devices_] = address;
//            n_devices_++;
//        } else if (error == 4) {
//            Serial.print("| Unknown error at address 0x");
//            if (address < 16)
//                Serial.print("0");
//            Serial.println(address, HEX);
//        }
//    }
//    if (n_devices_ == 0)
//        Serial.println("| No I2C devices found");
//    else
//        Serial.println("| Done Scanning");
//}
//
//void I2CScanner::endTransmission() {
//    if (n_devices_ > 0) {
//        Serial.print("| I2C devices found at address : ");
//        for (int i = 0; i < n_devices_; ++i) {
//            Serial.print("| 0x000");
//            Serial.print(n_address_[i], HEX);
//        }
//        Serial.println();
//
//        Serial.print("| Total devices found          : ");
//        Serial.print("| ");
//        Serial.print(n_devices_);
//        Serial.println();
//    }
//
//    Wire.end();
//    Serial.println("| WIRE END");
//}

#endif