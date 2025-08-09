#ifndef CERIA_MODULE_ESP32_NTP_H
#define CERIA_MODULE_ESP32_NTP_H

#include "Arduino.h"

#ifdef ESP32
#include <WiFi.h>
#include <time.h>

// Check ESP32 Arduino Core version for compatibility
#if ESP_ARDUINO_VERSION_MAJOR >= 3
#include "lwip/apps/sntp.h"
#define ESP32_CORE_V3_OR_HIGHER
#elif ESP_ARDUINO_VERSION_MAJOR >= 2
#include "esp_sntp.h"
#define ESP32_CORE_V2_COMPATIBLE
#else
#error "ESP32 Arduino Core 2.0.0 or higher required"
#endif

namespace CeriaDevOP {

    enum NTPSyncStatus {
        NTP_NOT_INITIALIZED = 0,
        NTP_WAITING_FOR_WIFI = 1,
        NTP_INITIALIZING = 2,
        NTP_SYNCING = 3,
        NTP_SYNCHRONIZED = 4,
        NTP_SYNC_FAILED = 5,
        NTP_WIFI_DISCONNECTED = 6
    };

    enum NTPSyncMode {
        NTP_SYNC_ON_DEMAND = 0,
        NTP_SYNC_PERIODIC = 1,
        NTP_SYNC_AGGRESSIVE = 2
    };

    struct NTPConfig {
        String primaryServer;
        String secondaryServer;
        String tertiaryServer;
        String timezone;
        long gmtOffsetSec;
        int daylightOffsetSec;
        uint32_t syncIntervalMs;
        uint32_t timeoutMs;
        bool autoRetry;
        uint8_t maxRetries;
    };

    struct NTPTimeInfo {
        uint16_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t weekday;
        uint16_t yearday;
        bool isDST;
        int32_t gmtOffsetSec;
        time_t timestamp;
        String dateString;
        String timeString;
        String dateTimeString;
        String timezoneString;
    };

    struct NTPStatus {
        NTPSyncStatus status;
        bool isTimeValid;
        bool isWiFiConnected;
        uint32_t lastSyncTime;
        uint32_t syncCount;
        uint32_t failCount;
        uint32_t nextSyncTime;
        String lastError;
        uint32_t uptime;
    };

    typedef void (*NTPSyncCallback)(bool success);
    typedef void (*NTPTimeCallback)(const NTPTimeInfo &timeInfo);

    class CeriaModuleESP32NTP {
    private:
        NTPConfig _config;
        NTPStatus _status;
        NTPTimeInfo _currentTime;

        bool _initialized;
        bool _autoSyncEnabled;
        uint32_t _lastUpdateTime;
        uint32_t _lastSyncAttempt;
        uint32_t _syncInterval;

        NTPSyncCallback _onSyncCallback;
        NTPTimeCallback _onTimeCallback;

        const char *_commonTimezones[24] = {
                "UTC0",                                    // UTC
                "GMT0BST,M3.5.0/01,M10.5.0/02",          // London
                "CET-1CEST,M3.5.0/02,M10.5.0/03",       // Germany/Paris
                "EET-2EEST,M3.5.0/03,M10.5.0/04",       // Helsinki
                "MSK-3",                                  // Moscow
                "IST-5:30",                              // India
                "JST-9",                                 // Japan
                "AEST-10AEDT,M10.1.0/02,M4.1.0/03",     // Sydney
                "NZST-12NZDT,M9.5.0/02,M4.1.0/03",      // New Zealand
                "HST10",                                 // Hawaii
                "AKST9AKDT,M3.2.0/02,M11.1.0/02",       // Alaska
                "PST8PDT,M3.2.0/02,M11.1.0/02",         // US Pacific
                "MST7MDT,M3.2.0/02,M11.1.0/02",         // US Mountain
                "CST6CDT,M3.2.0/02,M11.1.0/02",         // US Central
                "EST5EDT,M3.2.0/02,M11.1.0/02",         // US Eastern
                "WIB-7",                                 // Indonesia Western
                "WITA-8",                                // Indonesia Central
                "WIT-9",                                 // Indonesia Eastern
                "<-03>3<-02>,M10.1.0/0,M2.3.0/0",       // Brazil
                "ART3",                                  // Argentina
                "CAT-2",                                 // South Africa
                "WAT-1",                                 // West Africa
                "EAT-3",                                 // East Africa
                "<+08>-8"                                // Singapore/Malaysia
        };

        void updateTimeInfo();
        void triggerSyncCallback(bool success);
        void triggerTimeCallback();
        void setLastError(const String &error);
        bool isWiFiReady();
        static void timeAvailableCallback(struct timeval *t);
        String formatTwoDigits(uint8_t value);

    public:
        CeriaModuleESP32NTP();

        bool begin(const String &timezone = "UTC0", const String &ntpServer = "pool.ntp.org");
        bool begin(const NTPConfig &config);
        void update();

        bool isInitialized();
        bool isTimeValid();
        bool hasNewTime();

        bool startSync();
        bool stopSync();
        void forceSync();
        bool waitForSync(uint32_t timeoutMs = 30000);

        void setTimezone(const String &posixTZ);
        void setTimezone(long gmtOffsetSec, int daylightOffsetSec = 0);
        void setNTPServers(const String &primary, const String &secondary = "", const String &tertiary = "");
        void setSyncInterval(uint32_t intervalMs);
        void setSyncMode(NTPSyncMode mode);
        void setTimeout(uint32_t timeoutMs);

        void enableAutoSync(bool enable = true);
        void setMaxRetries(uint8_t retries);
        void setAutoRetry(bool enable);

        NTPTimeInfo getTime();
        NTPStatus getStatus();
        NTPConfig getConfig();

        uint16_t getYear();
        uint8_t getMonth();
        uint8_t getDay();
        uint8_t getHour();
        uint8_t getMinute();
        uint8_t getSecond();
        uint8_t getWeekday();
        bool isDST();
        time_t getTimestamp();
        uint32_t getUptime();

        String getDateString();
        String getTimeString();
        String getDateTimeString();
        String getTimeString12Hour();
        String getTimezoneString();
        String getWeekdayName();
        String getMonthName();
        String getFormattedTime(const String &format);

        void setTime(time_t timestamp);
        void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
        void setTime(const String &dateTime);

        String getCommonTimezone(uint8_t index);
        uint8_t getCommonTimezoneCount();
        String findTimezoneByCountry(const String &country);

        void onSyncComplete(NTPSyncCallback callback);
        void onTimeUpdate(NTPTimeCallback callback);

        String getNTPString();
        String getStatusString();
        String getFullStatusString();
        String getConfigString();

        bool isConnected(uint32_t timeoutMs = 5000);
        void reset();
        void resetStatistics();

        // Static utility functions
        static String timestampToString(time_t timestamp);
        static time_t stringToTimestamp(const String &dateTime);
        static bool validateTimezone(const String &timezone);
        static String getCurrentNTPServer();
        static uint32_t getLastSyncTime();
    };

}

#endif

#endif