#include "CeriaSensorGPS.h"

namespace CeriaDevOP {

    CeriaSensorGPS::CeriaSensorGPS() : TinyGPSPlus() {
        _serial = nullptr;
        _hasNewData = false;
        _lastUpdateTime = 0;
        _timezoneOffsetHours = 0;
        _timezoneOffsetMinutes = 0;
    }

    void CeriaSensorGPS::begin(Stream &serial, unsigned long baud) {
        _serial = &serial;
        
        if (_serial == &Serial) {
            Serial.begin(baud);
        }
        #if defined(HAVE_HWSERIAL1) || defined(ESP32)
        else if (_serial == &Serial1) {
            Serial1.begin(baud);
        }
        #endif
        #if defined(HAVE_HWSERIAL2) || defined(ESP32)
        else if (_serial == &Serial2) {
            Serial2.begin(baud);
        }
        #endif
        #if defined(HAVE_HWSERIAL3) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)
        else if (_serial == &Serial3) {
            Serial3.begin(baud);
        }
        #endif
    }

    void CeriaSensorGPS::update() {
        if (!_serial) return;

        bool newData = false;
        while (_serial->available()) {
            char c = _serial->read();
            if (encode(c)) {
                newData = true;
            }
        }

        if (newData) {
            _hasNewData = true;
            _lastUpdateTime = millis();
        }
    }

    bool CeriaSensorGPS::hasNewData() {
        bool result = _hasNewData;
        _hasNewData = false;
        return result;
    }

    bool CeriaSensorGPS::isConnected(uint32_t timeoutMs) {
        return (millis() - _lastUpdateTime) < timeoutMs && charsProcessed() > 0;
    }

    bool CeriaSensorGPS::hasFix() {
        return location.isValid() && satellites.value() >= 3;
    }

    double CeriaSensorGPS::getLatitude() {
        return location.lat();
    }

    double CeriaSensorGPS::getLongitude() {
        return location.lng();
    }

    bool CeriaSensorGPS::isLocationValid() {
        return location.isValid();
    }

    uint8_t CeriaSensorGPS::getSatellites() {
        return satellites.value();
    }

    double CeriaSensorGPS::getSpeed() {
        return speed.kmph();
    }

    double CeriaSensorGPS::getAltitude() {
        return altitude.meters();
    }

    String CeriaSensorGPS::getDate() {
        if (!date.isValid()) return "No Date";

        String result = "";
        if (date.day() < 10) result += "0";
        result += String(date.day()) + "/";
        if (date.month() < 10) result += "0";
        result += String(date.month()) + "/" + String(date.year());

        return result;
    }

    String CeriaSensorGPS::getTime() {
        if (!time.isValid()) return "No Time";

        String result = "";
        if (time.hour() < 10) result += "0";
        result += String(time.hour()) + ":";
        if (time.minute() < 10) result += "0";
        result += String(time.minute()) + ":";
        if (time.second() < 10) result += "0";
        result += String(time.second());

        return result;
    }

    String CeriaSensorGPS::getLocation() {
        if (!location.isValid()) return "No Fix";
        return String(location.lat(), 6) + "," + String(location.lng(), 6);
    }

    double CeriaSensorGPS::distanceTo(double lat, double lng) {
        if (!location.isValid()) return -1.0;
        return distanceBetween(location.lat(), location.lng(), lat, lng);
    }

    bool CeriaSensorGPS::waitForFix(uint32_t timeoutMs) {
        uint32_t startTime = millis();
        while (millis() - startTime < timeoutMs) {
            update();
            if (hasFix()) {
                return true;
            }
            delay(100);
        }
        return false;
    }

    void CeriaSensorGPS::setTimezone(int8_t offsetHours) {
        _timezoneOffsetHours = offsetHours;
        _timezoneOffsetMinutes = 0;
    }

    void CeriaSensorGPS::setTimezone(int8_t offsetHours, int8_t offsetMinutes) {
        _timezoneOffsetHours = offsetHours;
        _timezoneOffsetMinutes = offsetMinutes;
    }

    int8_t CeriaSensorGPS::getTimezoneHours() {
        return _timezoneOffsetHours;
    }

    int8_t CeriaSensorGPS::getTimezoneMinutes() {
        return _timezoneOffsetMinutes;
    }

    String CeriaSensorGPS::getLocalTime() {
        if (!time.isValid()) return "No Time";

        int localHour = time.hour() + _timezoneOffsetHours;
        int localMinute = time.minute() + _timezoneOffsetMinutes;
        int localSecond = time.second();

        if (localMinute >= 60) {
            localMinute -= 60;
            localHour++;
        } else if (localMinute < 0) {
            localMinute += 60;
            localHour--;
        }

        if (localHour >= 24) {
            localHour -= 24;
        } else if (localHour < 0) {
            localHour += 24;
        }

        String result = "";
        if (localHour < 10) result += "0";
        result += String(localHour) + ":";
        if (localMinute < 10) result += "0";
        result += String(localMinute) + ":";
        if (localSecond < 10) result += "0";
        result += String(localSecond);

        return result;
    }

}