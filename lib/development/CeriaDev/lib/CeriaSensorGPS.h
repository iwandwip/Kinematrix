#ifndef CERIA_SENSOR_GPS_H
#define CERIA_SENSOR_GPS_H

#include "Arduino.h"
#include <TinyGPSPlus.h>

namespace CeriaDevOP {

    class CeriaSensorGPS : public TinyGPSPlus {
    public:
        CeriaSensorGPS();

        void begin(Stream &serial, unsigned long baud = 9600);
        void update();

        bool hasNewData();
        bool isConnected(uint32_t timeoutMs = 5000);
        bool hasFix();

        double getLatitude();
        double getLongitude();
        bool isLocationValid();

        uint8_t getSatellites();
        double getSpeed();
        double getAltitude();

        String getDate();
        String getTime();
        String getLocalTime();
        String getLocation();

        void setTimezone(int8_t offsetHours);
        void setTimezone(int8_t offsetHours, int8_t offsetMinutes);
        int8_t getTimezoneHours();
        int8_t getTimezoneMinutes();

        double distanceTo(double lat, double lng);

        bool waitForFix(uint32_t timeoutMs = 30000);

    private:
        Stream *_serial;
        bool _hasNewData;
        uint32_t _lastUpdateTime;
        int8_t _timezoneOffsetHours;
        int8_t _timezoneOffsetMinutes;
    };

}

#endif